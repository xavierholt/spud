#ifndef SPUD_WRAPPERS_H
#define SPUD_WRAPPERS_H

#include <cstdint>

namespace Spud {
  void decrypt(const uint8_t* c, uint8_t* m, uint32_t l, const uint8_t nonce[8], const uint8_t* key);
  void encrypt(uint8_t* c, const uint8_t* m, uint32_t l, const uint8_t nonce[8], const uint8_t* key);
  void exp(uint8_t* gxy, const uint8_t* y, const uint8_t* gx);
  void kdf(uint8_t* out, const uint8_t* kmat, const uint8_t* salt, const uint8_t* info);
  void keygen(uint8_t* pk, uint8_t* sk);
  void mac(uint8_t* c, const uint8_t* m, uint32_t ml, const uint8_t* key);
  bool verify(const uint8_t* c, const uint8_t* m, uint32_t ml, const uint8_t* key);
}

#endif
