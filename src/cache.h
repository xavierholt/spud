#ifndef SPUD_CACHE_H
#define SPUD_CACHE_H

#include "key.h"
#include <map>

namespace Spud {
  class Cache {
    typedef std::pair<Key, uint32_t> ID;
  protected:
    std::map<ID, Key> mCache;
    ID*       mEpochs;
    uint32_t  mIndex;
    uint32_t  mCount;
  public:
    Cache(uint32_t count);
    ~Cache();

    void       del(const uint8_t rk[32], uint32_t n);
    const Key* get(const uint8_t rk[32], uint32_t n) const;
    void       set(const uint8_t rk[32], uint32_t n, const Key& key);
  };
}

#endif
