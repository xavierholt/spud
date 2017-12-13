#include "../src/spud.h"

#include <iostream>
#include <fstream>
#include <string>
#include <thread>

int main() {
  Spud::Key servkey;
  std::ifstream pubfile("test/server.pub");
  pubfile.read((char*) servkey.data(), 32);
  pubfile.close();

  Spud::Address bindaddr(0);
  Spud::Address servaddr("127.0.0.1", 7783);
  Spud::Socket  socket(bindaddr);

  socket.connect(servaddr, servkey);
  std::thread printer([&]() {
    uint8_t m[2048];
    uint32_t ml;

    while(true) {
      ml = socket.recv(m, 2048);
      std::cout << "> ";
      std::cout.write((char*) m, ml);
      std::cout << "\n";
    }
  });

  std::string input;
  while(std::getline(std::cin, input)) {
    socket.send(servaddr, (uint8_t*) input.data(), input.length());
  }

  return 0;
}
