#include "../src/spud.h"

#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: keygen <filebase>\n";
    exit(1);
  }

  Spud::Key publik;
  Spud::Key sekret;
  Spud::keygen(publik, sekret);
  std::string base(argv[1]);

  std::ofstream pubfile(base + ".pub");
  pubfile.write((char*) publik.data(), 32);
  pubfile.close();

  std::ofstream keyfile(base + ".key");
  keyfile.write((char*) sekret.data(), 32);
  keyfile.close();

  return 0;
}
