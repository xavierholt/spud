#ifndef SPUD_CACHE_H
#define SPUD_CACHE_H

#include "key.h"
#include <map>

namespace Spud {
  class Cache {
  protected:
    std::map<uint64_t,Key> mCache;
    uint64_t* mEpochs;
    uint32_t  mIndex;
    uint32_t  mCount;
  public:
    Cache(uint32_t count);
    ~Cache();

    void       del(uint32_t repoch, uint32_t mepoch);
    const Key* get(uint32_t repoch, uint32_t mepoch) const;
    void       set(uint32_t repoch, uint32_t mepoch, const Key& key);
  };
}

#endif
