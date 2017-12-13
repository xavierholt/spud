#ifndef SPUD_KEY_H
#define SPUD_KEY_H

#include <cstdint>
#include <cstring>

namespace Spud {
  class Key {
  protected:
    uint8_t mData[32];
  public:
    Key() {}
    Key(const uint8_t data[32]) {
      std::memcpy(mData, data, 32);
    }

    ~Key() {
      volatile uint32_t* data = (uint32_t*) mData;
      for(int i = 0; i < 8; ++i) {
        data[i] = 0;
      }
    }

    uint8_t* data() {
      return mData;
    }

    operator uint8_t*() {
      return mData;
    }

    operator const uint8_t*() const {
      return mData;
    }

    const Key& operator = (const uint8_t* other) {
      std::memcpy(mData, other, 32);
      return *this;
    }

    bool operator == (const uint8_t* other) const {
      bool b = true;
      for(uint32_t i = 0; i < 32; ++i) {
        b &= (mData[i] == other[i]);
      }

      return b;
    }

    bool operator != (const uint8_t* other) const {
      bool b = false;
      for(uint32_t i = 0; i < 32; ++i) {
        b |= (mData[i] != other[i]);
      }

      return b;
    }
  };
}

#endif
