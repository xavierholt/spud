#include "dh-ratchet.h"
#include "wrappers.h"

// Thirty-two bytes of constant:         01234567890123456789012345678901234567890
static const uint8_t SPUD_CONSTANT[33] = "use me as the SPUD unique string";
static const uint8_t ZERO_CONSTANT[32] = {0};

#include <cstdio>
static void printhex(const char* header, const uint8_t* data, uint32_t length) {
  printf("%s: ", header);
  for(uint32_t i = 0; i < length; ++i) {
    printf("%02x", data[i]);
  }

  printf("\n");
}

namespace Spud {
  DHRatchet::DHRatchet(const uint8_t init[32], const uint8_t rk[32]) {
    mOutput = init;
    mRemote = rk;
    refresh();
  }

  DHRatchet::DHRatchet(const uint8_t init[32], const uint8_t pk[32], const uint8_t sk[32]) {
    mOutput = init;
    mRemote = ZERO_CONSTANT;
    mPublic = pk;
    mSecret = sk;
  }

  DHRatchet::DHRatchet(const uint8_t init[32], const uint8_t pk[32], const uint8_t sk[32], const uint8_t rk[32]) {
    mOutput = init;
    mPublic = pk;
    mSecret = sk;
    refresh(rk);
  }

  void DHRatchet::debug() const {
    printhex("Public", mPublic, 32);
    printhex("Secret", mSecret, 32);
    printhex("Remote", mRemote, 32);
    printhex("Shared", mShared, 32);
    printhex("Output", mOutput, 32);
    printf("\n");
  }

  const Key& DHRatchet::output() const {
    return mOutput;
  }

  const Key& DHRatchet::publik() const {
    return mPublic;
  }

  void DHRatchet::ratchet() {
    Spud::exp(mShared, mSecret, mRemote);
    Spud::kdf(mOutput, mShared, mOutput, SPUD_CONSTANT);
  }

  void DHRatchet::refresh() {
    Spud::keygen(mPublic, mSecret);
    ratchet();
  }

  void DHRatchet::refresh(const uint8_t rk[32]) {
    mRemote = rk;
    ratchet();
  }

  const Key& DHRatchet::remote() const {
    return mRemote;
  }
}
