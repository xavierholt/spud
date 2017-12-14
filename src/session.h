#ifndef SPUD_SESSION_H
#define SPUD_SESSION_H

#include "cache.h"
#include "dh-ratchet.h"
#include "hmac-ratchet.h"

// This is where all the action is.
// A Session keeps track of all the ratchets for one end of a single connection.
// The encode and decode methods do en- and decryption, and adjust the ratchets as necessary.
// This class does not do any networking - that all happens in Socket.

namespace Spud {
  class Session {
  protected:
    DHRatchet   mRootChain;
    HMACRatchet mSendChain;
    HMACRatchet mRecvChain;
    Cache       mKeyCache;
  public:
    Session(const uint8_t rk[32]);
    Session(const uint8_t pk[32], const uint8_t sk[32]);
    Session(const uint8_t pk[32], const uint8_t sk[32], const uint8_t rk[32]);
    ~Session();

    uint32_t   decode(const uint8_t* c, uint32_t cl, uint8_t* m, uint32_t ml);
    uint32_t   encode(uint8_t* c, uint32_t cl, const uint8_t* m, uint32_t ml);
    const Key* key(const uint8_t rk[32], uint32_t mn, uint32_t pn);
    void       seek(const uint8_t rk[32], uint32_t n);
  };
}

#endif
