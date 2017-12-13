#include "../src/spud.h"

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
  while(true) {
    uint32_t ml = socket.recv(m, 2048);
    socket.broadcast(m, ml);
  }

  return 0;
}
