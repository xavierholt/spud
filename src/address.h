#ifndef SPUD_ADDRESS_H
#define SPUD_ADDRESS_H

#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>

// This is a wrapper around the sockaddr_in struct.
// It doesn't do much - just makes them easier to work with.

namespace Spud {
  class Address {
  protected:
    struct sockaddr_in mData;
  public:
    Address() {
      std::memset(&mData, 0, sizeof(mData));
    }

    Address(uint16_t port) {
      std::memset(&mData, 0, sizeof(mData));
      mData.sin_family = AF_INET;
      mData.sin_addr.s_addr = htonl(INADDR_ANY);
      mData.sin_port = htons(port);
    }

    Address(uint32_t bind, uint16_t port) {
      std::memset(&mData, 0, sizeof(mData));
      mData.sin_family = AF_INET;
      mData.sin_addr.s_addr = htonl(bind);
      mData.sin_port = htons(port);
    }

    Address(const char* bind, uint16_t port) {
      std::memset(&mData, 0, sizeof(mData));
      mData.sin_family = AF_INET;
      mData.sin_addr.s_addr = inet_addr(bind);
      mData.sin_port = htons(port);
    }

    uint32_t ip() const {
      return ntohl(mData.sin_addr.s_addr);
    }

    uint16_t port() const {
      return ntohs(mData.sin_port);
    }

    int size() const {
      return sizeof(mData);
    }

    operator struct sockaddr*() {
      return (struct sockaddr*) &mData;
    }

    operator const struct sockaddr*() const {
      return (struct sockaddr*) &mData;
    }

    bool operator < (const Address& other) const {
      return (ip() == other.ip())? (port() < other.port()) : (ip() < other.ip());
    }

    bool operator == (const Address& other) const {
      return (ip() == other.ip()) && (port() == other.port());
    }
  };
}

#endif
