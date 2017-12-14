#include "../src/spud.h"

#include <cstdio>
#include <fstream>
#include <iostream>

int main() {
  Spud::Key pk;
  Spud::Key sk;

  std::ifstream pubfile("test/server.pub");
  pubfile.read((char*) pk.data(), 32);
  pubfile.close();

  std::ifstream keyfile("test/server.key");
  keyfile.read((char*) sk.data(), 32);
  keyfile.close();

  Spud::Address bindaddr(7783);
  Spud::Socket socket(pk, sk, bindaddr);
  socket.accept(true);

  uint8_t m[2048];
  uint8_t b[2048];
  while(true) {
    Spud::Address address;
    uint32_t ml = socket.recv(address, m, 2048);
    m[ml] = '\0'; // Null-terminated C strings!

    // Label who said what for a better client experience:
    uint32_t bl = snprintf((char*) b, 2048, "[Client %5d]: %s", address.port(), (char*) m);
    socket.broadcast(b, bl);
  }

  return 0;
}
