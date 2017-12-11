#include "connection.h"

#include "nacl/crypto_auth_hmacsha512256.h"
#include "nacl/crypto_box_curve25519xsalsa20poly1305.h"
#include "nacl/crypto_scalarmult_curve25519.h"

// Read 16 bits from network byte order:
static inline uint16_t r16(const uint8_t* bytes) {
  uint16_t a = uint16_t(bytes[0]) << 8;
  uint16_t b = uint16_t(bytes[1]) << 0;
  return a | b;
}

// Store 16 bits in network byte order:
static inline void w16(uint16_t value, uint8_t* bytes) {
  bytes[0] = value >> 8;
  bytes[1] = value >> 0;
}

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
// 64 - 67: Root Key ID
// 68 - 69: Send Key ID
// 70 - 71: Message Length
// 72 - ??: Message Data

namespace Spud {
  Connection::Connection(const Key& pk, const Key& sk, const uint8_t* rk):
    mRootChain(ROOT_CONSTANT, pk, sk, rk),
    mSendChain(SEND_CONSTANT, mRootChain.output()),
    mRecvChain(RECV_CONSTANT, mRootChain.output())
  {
    // TODO
  }

  Connection::~Connection() {
    // Nothing to do.
  }

  Connection* Connection::client(const uint8_t* rk) {
    // TODO: Generate ephemeral DH keys!
    Connection* connection = new Connection(pk, sk, rk);
  }

  Connection* Connection::client(const Key& pk, const Key& sk, const uint8_t* rk) {
    Connection* connection = new Connection(pk, sk, rk);
  }

  uint32_t Connection::decode(const uint8_t* c, uint32_t cl, uint8_t* m, uint32_t ml) {
    uint32_t repoch = r32(c + 64);
    uint32_t mepoch = r16(c + 68);
    uint32_t length = r16(c + 70);

    if(length != cl + 72) {
      throw DecodeError("Message length does not match packet length.");
    }

    if(length > ml) {
      throw DecodeError("Buffer too short to hold message!");
    }

    if(repoch == mRootChain.epoch()) {
      // Nothing to do - we're already in agreement.
    }
    else if(repoch == mRootChain.epoch() + 1) {
      // The other end changed their key - ratchet!
      mRootChain.ratchet(c + 32);
    }
    else if(repoch > mRootChain.epoch()) {
      throw DecodeError("Remote epoch is too far in the future!");
    }

    const Key* key = key(repoch, mepoch);
    if(key == NULL) throw DecodeError("Could not find decryption key.")
    if(crypto_auth_verify_sha512256(c, c + 32, length + 40, *key) != 0) {
      throw DecodeError("Invalid message authentication code.");
    }

    // Note: Using the Root Epoch + Send Epoch + Length (c[64:72]) as a nonce.
    crypto_stream_salsa20_xor(message, c + 72, length, c + 64, *key);
    mKeyCache.del(repoch, mepoch);
    mRootChain.refresh();
    return length;
  }

  void Connection::encode(uint8_t* c, uint32_t cl, const uint8_t* m, uint32_t ml) {
    w32(mRootChain.epoch(), c + 64);
    w16(mSendChain.epoch(), c + 68);
    w16(length,             c + 70);

    // Note: Using the Root Epoch + Send Epoch + Length (c[64:72]) as a nonce.
    crypto_stream_salsa20_xor(c + 72, m, ml, c + 64, mSendChain.output());
    crypto_auth_sha512256(c, c + 32, ml + 40, mSendChain.output());
    mSendChain.ratchet();
  }

  const Key* Connection::key(uint32_t repoch, uint32_t mepoch) {
    if(repoch == mRootChain.epoch()) {
      while(mepoch > mRecvChain.epoch()) {
        mKeyCache.set(repoch, mRecvChain.epoch(), mRecvChain.output());
        mRecvChain.ratchet();
      }

      if(mepoch == mRecvChain.epoch()) {
        return &mRecvChain.output();
      }
    }

    return mKeyCache.get(repoch, mepoch);
  }

  void Connection::send(const uint8_t* m, uint32_t ml) {
    uint8_t c[2048];
    encode(c, 2048, m, ml);

    // TODO: Send!
  }

  uint32_t Connection::recv(uint8_t* m, uint16_t ml) {
    uint8_t c[2048];
    //TODO: Get ciphertext!

    return decode(c, 2048, m, ml);
  }
}
