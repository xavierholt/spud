#include "socket.h"
#include "error.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace Spud {
  Socket::Socket(const Address& bindaddr): mConnections() {
    mFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if(mFileDescriptor < 0) {
      throw Spud::Error("Socket creating failed.");
    }

    if(bind(mFileDescriptor, bindaddr, bindaddr.size()) < 0) {
      throw Spud::Error("Socket binding failed.");
    }
  }

  Socket::Socket(const Key& pk, const Key& sk, const Address& bindaddr): mConnections(), mPublicKey(pk), mSecretKey(sk) {
    mFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if(mFileDescriptor < 0) {
      throw Spud::Error("Socket creating failed.");
    }

    if(bind(mFileDescriptor, bindaddr, bindaddr.size()) < 0) {
      throw Spud::Error("Socket binding failed.");
    }
  }

  Socket::~Socket() {
    close(mFileDescriptor);
  }

  void Socket::accept(bool accepting) {
    mAccepting = accepting;
  }

  void Socket::broadcast(const uint8_t* m, uint32_t ml) {
    uint8_t c[2048];

    std::map<Address,Session*>::iterator i = mConnections.begin();
    while(i != mConnections.end()) {
      uint32_t cl = i->second->encode(c, 2048, m, ml);
      if(sendto(mFileDescriptor, c, cl, 0, i->first, i->first.size()) < 0) {
        throw Spud::Error("Broadcast failed.");
      }

      ++i;
    }
  }

  void Socket::connect(const Address& addr, const Key& pubkey) {
    Session* session = new Session(pubkey);
    mConnections[addr] = session;
  }

  uint32_t Socket::recv(uint8_t* m, uint32_t ml) {
    Address addr;
    return recv(addr, m, ml);
  }

  uint32_t Socket::recv(Address& addr, uint8_t* m, uint32_t ml) {
    uint8_t c[2048];
    socklen_t al = addr.size();
    ssize_t cl = recvfrom(mFileDescriptor, c, 2048, 0, addr, &al);
    if(cl < 0) {
      throw Spud::Error("Receive failed.");
    }

    std::map<Address,Session*>::iterator i = mConnections.find(addr);
    if(i == mConnections.end()) {
      if(mAccepting) {
        Session* session = new Session(mPublicKey, mSecretKey);
        mConnections[addr] = session;
        return session->decode(c, cl, m, ml);
      }
      else {
        throw Spud::Error("Unacceptable connection attempt.");
      }
    }
    else {
      return i->second->decode(c, cl, m, ml);
    }
  }

  void Socket::send(const Address& addr, const uint8_t* m, uint32_t ml) {
    uint8_t c[2048];

    std::map<Address,Session*>::iterator i = mConnections.find(addr);
    if(i == mConnections.end()) {
      throw Spud::Error("No such connection.");
    }

    uint32_t cl = i->second->encode(c, 2048, m, ml);
    if(sendto(mFileDescriptor, c, cl, 0, i->first, i->first.size()) < 0) {
      throw Spud::Error("Broadcast failed.");
    }
  }
}
