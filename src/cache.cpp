#include "cache.h"

static inline uint64_t keyfor(uint64_t repoch, uint64_t mepoch) {
  return (repoch << 32) | mepoch;
}

namespace Spud {
  Cache::Cache(uint32_t count) {
    mIndex  = 0;
    mCount  = count;
    mEpochs = new uint64_t[count];
    for(uint32_t i = 0; i < count; ++i) {
      mEpochs[i] = 0;
    }
  }

  Cache::~Cache() {
    delete [] mEpochs;
  }

  void Cache::del(uint32_t repoch, uint32_t mepoch) {
    mCache.erase(keyfor(repoch, mepoch));
  }

  const Key* Cache::get(uint32_t repoch, uint32_t mepoch) const {
    std::map<uint64_t,Key>::const_iterator i = mCache.find(keyfor(repoch, mepoch));
    return (i != mCache.end())? &i->second : NULL;
  }

  void Cache::set(uint32_t repoch, uint32_t mepoch, const Key& key) {
    mCache.erase(mEpochs[mIndex]);
    mEpochs[mIndex] = keyfor(repoch, mepoch);
    mIndex = (mIndex + 1) % mCount;

    mCache[keyfor(repoch, mepoch)] = key;
  }
}
