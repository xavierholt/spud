#include "wrappers.h"

#include "nacl/crypto_auth_hmacsha512256.h"
#include "nacl/crypto_box_curve25519xsalsa20poly1305.h"
#include "nacl/crypto_scalarmult_curve25519.h"
#include "nacl/crypto_stream_salsa20.h"


namespace Spud {
  void decrypt(const uint8_t* c, uint8_t* m, uint32_t l, const uint8_t nonce[8], const uint8_t* key) {
    // Decrypts by XORing with the Salsa 20 stream cipher.
    crypto_stream_salsa20_xor(m, c, l, nonce, key);
  }

  void encrypt(uint8_t* c, const uint8_t* m, uint32_t l, const uint8_t nonce[8], const uint8_t* key) {
    // Encrypts by XORing with the Salsa 20 stream cipher.
    crypto_stream_salsa20_xor(c, m, l, nonce, key);
  }

  void exp(uint8_t* gxy, const uint8_t* y, const uint8_t* gx) {
    // The Diffie-Hellman calculation (exponentiation in DH, multiplication in EC).
    crypto_scalarmult_curve25519(gxy, y, gx);
  }

  void kdf(uint8_t* out, const uint8_t* kmat, const uint8_t* salt, const uint8_t* info) {
    // Key derivation; like Krawczysk, but without the index.
    // See https://eprint.iacr.org/2010/264.pdf
    // Or  https://en.wikipedia.org/wiki/HKDF
    crypto_auth_hmacsha512256(out, kmat, 32, salt);
    crypto_auth_hmacsha512256(out, info, 32, out);
  }

  void keygen(uint8_t* pk, uint8_t* sk) {
    // Complicated name, but all it does is generate a Curve 25519 keypair.
    crypto_box_curve25519xsalsa20poly1305_keypair(pk, sk);
  }

  void mac(uint8_t* c, const uint8_t* m, uint32_t ml, const uint8_t* key) {
    // Generates a MAC.
    crypto_auth_hmacsha512256(c, m, ml, key);
  }

  bool verify(const uint8_t* c, const uint8_t* m, uint32_t ml, const uint8_t* key) {
    // Verifies a MAC.
    return (crypto_auth_hmacsha512256_verify(c, m, ml, key) == 0);
  }
}
