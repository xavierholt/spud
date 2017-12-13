#include "socket.h"
#include "error.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// static void printhex(const char* header, const void* data, uint32_t length) {
//   printf("%s: ", header);
//   for(uint32_t i = 0; i < length; ++i) {
//     printf("%02x", ((char*)data)[i]);
//   }

//   printf("\n");
// }

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
    // printhex("Add Addr", &addr, addr.size());
    Session* session = new Session(pubkey);
    mConnections[addr] = session;

    // uint8_t c[2048];
    // uint32_t cl = session->encode(c, 2048, NULL, 0);
    // if(sendto(mFileDescriptor, c, cl, 0, addr, addr.size()) < 0) {
    //   throw Spud::Error("Broadcast failed.");
    // }
  }

  uint32_t Socket::recv(uint8_t* m, uint32_t ml) {
    uint8_t c[2048];
    Address addr;

    socklen_t al = addr.size();
    ssize_t cl = recvfrom(mFileDescriptor, c, 2048, 0, addr, &al);
    if(cl < 0) {
      throw Spud::Error("Receive failed.");
    }

    // printhex("Get Addr", &addr, addr.size());
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
