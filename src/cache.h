#ifndef SPUD_CACHE_H
#define SPUD_CACHE_H

#include "key.h"
#include <map>

// This is a cache of (root-key, seq) -> message-key
// It's used by the Session class to keep track of skipped keys.
// Users shouldn't need to use this class directly.

namespace Spud {
  class Cache {
    struct ID {
      uint32_t n;
      uint8_t k[32];

      ID(): n(0) {
        std::memset(this->k, 0, 32);
      }

      ID(const uint8_t k[32], uint32_t n): n(n) {
        std::memcpy(this->k, k, 32);
      }

      bool operator < (const ID& other) const {
        return (n == other.n)? std::memcmp(k, other.k, 32) : (n < other.n);
      }
    };
  protected:
    std::map<ID, Key> mCache;
    ID        mEpochs[32];
    uint32_t  mIndex;
  public:
    Cache();

    void       del(const uint8_t rk[32], uint32_t n);
    const Key* get(const uint8_t rk[32], uint32_t n) const;
    void       set(const uint8_t rk[32], uint32_t n, const Key& key);
  };
}

#endif
