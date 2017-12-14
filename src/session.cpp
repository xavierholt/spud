#include "session.h"

#include "error.h"
#include "wrappers.h"

#include <cstdio>

// Read 32 bits from network byte order:
static inline uint32_t r32(const uint8_t* bytes) {
  uint32_t a = uint32_t(bytes[0]) << 24;
  uint32_t b = uint32_t(bytes[1]) << 16;
  uint32_t c = uint32_t(bytes[2]) <<  8;
  uint32_t d = uint32_t(bytes[3]) <<  0;
  return a | b | c | d;
}

// Store 32 bits in network byte order:
static inline void w32(uint32_t value, uint8_t* bytes) {
  bytes[0] = value >> 24;
  bytes[1] = value >> 16;
  bytes[2] = value >>  8;
  bytes[3] = value >>  0;
}

// Thirty-two bytes of constant:       01234567890123456789012345678901234567890
static const uint8_t ROOT_CONSTANT[] = "so y'all need a root ratchet...";
static const uint8_t SEND_CONSTANT[] = "this'll initialize send ratchets";
static const uint8_t RECV_CONSTANT[] = "and this one'll do recv ratchets";

// Message layout (bytes):
//  0 - 31: Authentication Code
// 32 - 63: Next DH Key
// 64 - 67: Current Message Number
// 68 - 71: Previous Message Count
// 72 - ??: Message Data

namespace Spud {
  Session::Session(const uint8_t rk[32]):
    mRootChain(ROOT_CONSTANT, rk),
    mSendChain(SEND_CONSTANT, mRootChain.output()),
    mRecvChain(RECV_CONSTANT, mRootChain.output()),
    mKeyCache(128)
  {
    // Client-side (creates an ephemeral keypair).
  }

  Session::Session(const uint8_t pk[32], const uint8_t sk[32]):
    mRootChain(ROOT_CONSTANT, pk, sk),
    mSendChain(RECV_CONSTANT, mRootChain.output()),
    mRecvChain(SEND_CONSTANT, mRootChain.output()),
    mKeyCache(128)
  {
    // Server-side with a known keypair.
    // Note that the send/recv constants are swapped!
  }

  Session::~Session() {
    // Nothing to do.
  }

  uint32_t Session::decode(const uint8_t* c, uint32_t cl, uint8_t* m, uint32_t ml) {
    uint32_t mn = r32(c + 64);
    uint32_t pn = r32(c + 68);

    if(ml < cl - 72) {
      throw Spud::Error("Buffer too short to hold message!");
    }

    const Key* k = key(c + 32, mn, pn);
    if(k == NULL) throw Spud::Error("Could not find decryption key.");
    if(!Spud::verify(c, c + 32, cl - 32, *k)) {
      throw Spud::Error("Invalid message authentication code.");
    }

    // Note: Using the message numbers / counts (c[64:72]) as a nonce.
    Spud::decrypt(c + 72, m, cl - 72, c + 64, *k);
    mKeyCache.del(c + 32, mn);
    return cl - 72;
  }

  uint32_t Session::encode(uint8_t* c, uint32_t cl, const uint8_t* m, uint32_t ml) {
    if(cl < ml + 72) {
      throw Spud::Error("Buffer too short to hold ciphertext!");
    }

    mSendChain.ratchet();
    std::memcpy(c + 32, mRootChain.publik(), 32);
    w32(mSendChain.count(), c + 64);
    w32(mSendChain.prev(),  c + 68);

    Spud::encrypt(c + 72, m, ml, c + 64, mSendChain.output());
    Spud::mac(c, c + 32, ml + 40, mSendChain.output());
    return ml + 72;
  }

  const Key* Session::key(const uint8_t rk[32], uint32_t mn, uint32_t pn) {
    if(mRootChain.remote() != rk) {
      // Skip unseen keys from the old chain:
      seek(mRootChain.remote(), pn);

      // Generate a new root key:
      mRootChain.refresh(rk);
      mRecvChain.refresh(mRootChain.output());
      mRootChain.refresh();
      mSendChain.refresh(mRootChain.output());
    }

    // Skip unseen keys from the new/current chain:
    seek(mRootChain.remote(), mn - 1);
    if(mRecvChain.count() == mn - 1) {
      mRecvChain.ratchet();
      return &mRecvChain.output();
    }

    return mKeyCache.get(rk, mn);
  }

  void Session::seek(const uint8_t rk[32], uint32_t n) {
    if(n > mRecvChain.count()) {
      if(n - mRecvChain.count() > 8) {
        throw Spud::Error("Too many keys to skip.");
      }

      while(mRecvChain.count() < n) {
        mRecvChain.ratchet();
        mKeyCache.set(rk, mRecvChain.count(), mRecvChain.output());
      }
    }
  }
}
