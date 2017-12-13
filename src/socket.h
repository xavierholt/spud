#ifndef SPUD_SOCKET_H
#define SPUD_SOCKET_H

#include <cstdint>
#include <map>

#include "address.h"
#include "key.h"
#include "session.h"

namespace Spud {
  class Socket {
  protected:
    int  mFileDescriptor;
    std::map<Address,Session*> mConnections;
    Key  mPublicKey;
    Key  mSecretKey;
    bool mAccepting;
  public:
    Socket(const Address& bindaddr);
    Socket(const Key& pk, const Key& sk, const Address& bindaddr);
    ~Socket();

    void     accept(bool accepting);
    void     broadcast(const uint8_t* m, uint32_t ml);
    void     connect(const Address& addr, const Key& pubkey);
    void     send(const Address& addr, const uint8_t* m, uint32_t ml);
    uint32_t recv(uint8_t* m, uint32_t ml);
  };
}

#endif
