#ifndef SPUD_SOCKET_H
#define SPUD_SOCKET_H

#include <cstdint>
#include <map>

#include "address.h"
#include "key.h"
#include "session.h"

// Network / UDP stuff happens here.
// This class opens a socket on creation, then can send and receive as requested.
// It can take a known keypair to listen with (server mode) if desired.
// It maintains a map of (UDP Address) -> (Session) for all known connections.
// The "accepting" flag determines whether or not it allows connections from unknown sources.

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
