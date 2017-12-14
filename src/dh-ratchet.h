#ifndef SPUD_DHRATCHET_H
#define SPUD_DHRATCHET_H

#include "key.h"

// This class stores the Diffie-Hellman ratchet.
// It's used by Session to keep track of the root chain.
// Users shouldn't need to use it directly.

namespace Spud {
  class DHRatchet {
  protected:
    Key mPublic;
    Key mSecret;
    Key mRemote;
    Key mShared;
    Key mOutput;
  public:
    DHRatchet(const DHRatchet& other);
    DHRatchet(const uint8_t init[32], const uint8_t rk[32]);
    DHRatchet(const uint8_t init[32], const uint8_t pk[32], const uint8_t sk[32]);

    void debug() const;
    const Key& output() const;
    const Key& publik() const;
    void ratchet();
    void refresh();
    void refresh(const uint8_t input[32]);
    const Key& remote() const;
  };
}

#endif
