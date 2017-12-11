#include "dh-ratchet.h"

#include "nacl/crypto_auth_hmacsha512256.h"
#include "nacl/crypto_box_curve25519xsalsa20poly1305.h"
#include "nacl/crypto_scalarmult_curve25519.h"


// Thirty-two bytes of constant:       01234567890123456789012345678901234567890
static const uint8_t INIT_CONSTANT[] = "there once was a man from verona";
static const uint8_t SPUD_CONSTANT[] = "use me as the SPUD unique string";


namespace Spud {
  DHRatchet::DHRatchet(const uint8_t rk[32]) {
    mOutput = INIT_CONSTANT;
    mRemote = rk;
    mEpoch  = 0;
    refresh();
  }

  DHRatchet::DHRatchet(const uint8_t pk[32], const uint8_t sk[32], const uint8_t rk[32]) {
    mOutput = INIT_CONSTANT;
    mPublic = pk;
    mSecret = sk;
    mEpoch  = 0;
    refresh(rk);
  }

  uint32_t DHRatchet::epoch() const {
    return mEpoch;
  }

  const Key& DHRatchet::output() const {
    return mOutput;
  }

  void DHRatchet::ratchet() {
    Key shared;
    // Diffie-Hellman Key Agreement:
    crypto_scalarmult_curve25519(shared, mSecret, mRemote);

    // HKDF for the next root key (like Krawczysk, but without the index):
    // See https://eprint.iacr.org/2010/264.pdf for details.
    crypto_auth_hmacsha512256(mOutput, shared, 32, mOutput);
    crypto_auth_hmacsha512256(mOutput, SPUD_CONSTANT, 32, mOutput);
    mEpoch += 1;
  }

  void DHRatchet::refresh() {
    // Generate a new keypair:
    crypto_box_curve25519xsalsa20poly1305_keypair(mPublic, mSecret);
    ratchet();
  }

  void DHRatchet::refresh(const uint8_t remote[32]) {
    // Got a new reomte public key:
    mRemote = remote;
    ratchet();
  }
}
