#ifndef SPUD_DHRATCHET_H
#define SPUD_DHRATCHET_H

#include "key.h"

namespace Spud {
  class DHRatchet {
  protected:
    Key      mPublic;
    Key      mSecret;
    Key      mRemote;
    Key      mOutput;
    uint32_t mEpoch;
  public:
    DHRatchet(const uint8_t rk[32]);
    DHRatchet(const uint8_t pk[32], const uint8_t sk[32], const uint8_t rk[32]);

    uint32_t epoch() const;
    const Key& output() const;
    void ratchet();
    void refresh();
    void refresh(const uint8_t input[32]);
  };
}

#endif
