#include "hmac-ratchet.h"

#include "nacl/crypto_auth_hmacsha512256.h"

// Thirty-two bytes of constant:        01234567890123456789012345678901234567890
static const uint8_t CHAIN_CONSTANT[] = "this is the input for chain keys";
static const uint8_t VALUE_CONSTANT[] = "and this one is for message keys";

namespace Spud {
  HMACRatchet::HMACRatchet(const uint8_t initial[32]) {
    mChainKey = initial;
  }

  uint32_t HMACRatchet::epoch() const {
    return mEpoch;
  }

  const Key& HMACRatchet::output() const {
    return mValueKey;
  }

  void HMACRatchet::ratchet() {
    crypto_auth_hmacsha512256(mValueKey, VALUE_CONSTANT, 32, mChainKey);
    crypto_auth_hmacsha512256(mChainKey, CHAIN_CONSTANT, 32, mChainKey);
    mEpoch += 1;
  }

  void HMACRatchet::refresh(const uint8_t input[32]) {
    crypto_auth_hmacsha512256(mValueKey, VALUE_CONSTANT, 32, input);
    crypto_auth_hmacsha512256(mChainKey, CHAIN_CONSTANT, 32, input);
    mEpoch = 1;
  }
}
