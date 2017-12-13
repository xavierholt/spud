#ifndef SPUD_HMACRATCHET_H
#define SPUD_HMACRATCHET_H

#include "key.h"

namespace Spud {
  class HMACRatchet {
  protected:
    Key      mSetupKey;
    Key      mChainKey;
    Key      mValueKey;
    uint32_t mCount;
    uint32_t mPrev;
  public:
    HMACRatchet(const uint8_t init[32], const uint8_t key[32]);

    void debug();
    uint32_t count() const;
    const Key& output() const;
    uint32_t prev() const;
    void ratchet();
    void refresh(const uint8_t key[32]);
  };
}

#endif
