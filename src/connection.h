#ifndef SPUD_CONNECTION_H
#define SPUD_CONNECTION_H

#include "cache.h"
#include "dh-ratchet.h"
#include "hmac-ratchet.h"

namespace Spud {
  class Connection {
  protected:
    DHRatchet   mRootChain;
    HMACRatchet mSendChain;
    HMACRatchet mRecvChain;
    Cache       mKeyCache;
  public:
    static Connection* client(const Key& pk, const Key& sk, const uint8_t* rk);
    static Connection* server(const Key& pk, const Key& sk, const uint8_t* rk);
  public:
    Connection(const Key& pk, const Key& sk, const uint8_t* rk);
    ~Connection();

    uint32_t   decode(const uint8_t* c, uint32_t cl, uint8_t* m, uint32_t ml);
    void       encode(uint8_t* c, uint32_t cl, const uint8_t* m, uint32_t ml);
    const Key* key(uint32_t repoch, uint32_t mepoch);
    void       send(const uint8_t* message, uint16_t length);
    uint32_t   recv(uint8_t* message, uint16_t maxlength);
  };
}

#endif
