#ifndef SPUD_KEY_H
#define SPUD_KEY_H

#include <cstdint>
#include <cstring>

namespace Spud {
  class Key {
  protected:
    uint8_t mData[32];
  public:
    ~Key() {
      volatile uint32_t* data = (uint32_t*) mData;
      for(int i = 0; i < 8; ++i) {
        data[i] = 0;
      }
    }

    operator uint8_t*() {
      return mData;
    }

    operator const uint8_t*() const {
      return mData;
    }

    const Key& operator = (const Key& other) {
      std::memcpy(mData, other, 32);
      return *this;
    }

    const Key& operator = (const uint8_t* other) {
      std::memcpy(mData, other, 32);
      return *this;
    }
  };
}

#endif
