#include "hmac-ratchet.h"
#include "wrappers.h"

// Thirty-two bytes of constant:        01234567890123456789012345678901234567890
static const uint8_t CHAIN_CONSTANT[] = "this is the input for chain keys";
static const uint8_t VALUE_CONSTANT[] = "and this one is for message keys";

#include <cstdio>
static void printhex(const char* header, const uint8_t* data, uint32_t length) {
  printf("%s: ", header);
  for(uint32_t i = 0; i < length; ++i) {
    printf("%02x", data[i]);
  }

  printf("\n");
}

namespace Spud {
  HMACRatchet::HMACRatchet(const uint8_t init[32], const uint8_t key[32]): mCount(0) {
    mSetupKey = init;
    refresh(key);
  }

  uint32_t HMACRatchet::count() const {
    return mCount;
  }

  void HMACRatchet::debug() {
    printhex("Setup", mSetupKey, 32);
    printhex("Chain", mChainKey, 32);
    printhex("Value", mValueKey, 32);
    printf("\n");
  }

  const Key& HMACRatchet::output() const {
    return mValueKey;
  }

  uint32_t HMACRatchet::prev() const {
    return mPrev;
  }

  void HMACRatchet::ratchet() {
    Spud::mac(mValueKey, VALUE_CONSTANT, 32, mChainKey);
    Spud::mac(mChainKey, CHAIN_CONSTANT, 32, mChainKey);
    mCount += 1;
  }

  void HMACRatchet::refresh(const uint8_t key[32]) {
    Spud::mac(mChainKey, mSetupKey, 32, key);
    mPrev  = mCount;
    mCount = 0;
  }
}
