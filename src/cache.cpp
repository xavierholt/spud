#include "cache.h"

namespace Spud {
  Cache::Cache(uint32_t count) {
    mIndex  = 0;
    mCount  = count;
    mEpochs = new ID[count];
    // std::memset(mEpochs, 0, count * sizeof(ID));
  }

  Cache::~Cache() {
    delete [] mEpochs;
  }

  void Cache::del(const uint8_t rk[32], uint32_t n) {
    ID cachekey(rk, n);
    mCache.erase(cachekey);
  }

  const Key* Cache::get(const uint8_t rk[32], uint32_t n) const {
    ID cachekey(rk, n);
    std::map<ID,Key>::const_iterator i = mCache.find(cachekey);
    return (i != mCache.end())? &i->second : NULL;
  }

  void Cache::set(const uint8_t rk[32], uint32_t n, const Key& key) {
    ID cachekey(rk, n);
    // mCache.erase(mEpochs[mIndex]);
    // mEpochs[mIndex] = cachekey;
    // mIndex = (mIndex + 1) % mCount;

    mCache[cachekey] = key;
  }
}
