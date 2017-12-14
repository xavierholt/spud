#include "session.h"

namespace Spud {
  Session::Session(const uint8_t rk[32]): mState(rk) {
    // Client-side (creates an ephemeral keypair).
  }

  Session::Session(const uint8_t pk[32], const uint8_t sk[32]): mState(pk, sk) {
    // Server-side with a known keypair.
  }

  uint32_t Session::decode(const uint8_t* c, uint32_t cl, uint8_t* m, uint32_t ml) {
    State idaho(mState);
    ml = idaho.decode(c, cl, m, ml);
    mState = idaho;
    return ml;
  }

  uint32_t Session::encode(uint8_t* c, uint32_t cl, const uint8_t* m, uint32_t ml) {
    State idaho(mState);
    ml = idaho.encode(c, cl, m, ml);
    mState = idaho;
    return ml;
  }
}
