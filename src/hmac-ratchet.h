#ifndef SPUD_HMACRATCHET_H
#define SPUD_HMACRATCHET_H

#include "key.h"

namespace Spud {
  class HMACRatchet {
  protected:
    Key      mChainKey;
    Key      mValueKey;
    uint32_t mEpoch;
  public:
    HMACRatchet(const uint8_t initial[32]);

    uint32_t epoch() const;
    const Key& output() const;
    void ratchet();
    void refresh(const uint8_t input[32]);
  };
}

#endif
