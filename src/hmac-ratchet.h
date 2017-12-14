#ifndef SPUD_HMACRATCHET_H
#define SPUD_HMACRATCHET_H

#include "key.h"

// This class stores a message key ratchet.
// It's used by Session to keep track of the send and recv chains.
// Users shouldn't need to use it directly.

namespace Spud {
  class HMACRatchet {
  protected:
    Key      mSetupKey;
    Key      mChainKey;
    Key      mValueKey;
    uint32_t mCount;
    uint32_t mPrev;
  public:
    HMACRatchet(const HMACRatchet& other);
    HMACRatchet(const uint8_t init[32], const uint8_t key[32]);

    uint32_t count() const;
    void debug() const;
    const Key& output() const;
    uint32_t prev() const;
    void ratchet();
    void refresh(const uint8_t key[32]);
  };
}

#endif
