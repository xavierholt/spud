#include "../src/spud.h"

#include <iostream>

bool equals(const uint8_t* a, uint32_t al, const uint8_t* b, uint32_t bl) {
  return (al == bl) && (std::memcmp(a, b, al) == 0);
}

void send(Spud::Session& src, Spud::Session& dst, const char* message) {
  uint8_t m[2048];
  uint8_t c[2048];
  uint8_t d[2048];

  uint32_t ml = 2048;
  uint32_t cl = 2048;
  uint32_t dl = 2048;

  ml = std::strlen(message);
  std::memcpy(m, message, ml);

  try {
    cl = src.encode(c, 2048, m, ml);
    dl = dst.decode(c, cl, d, 2048);
    std::cout.write((char*) d, dl);
    std::cout << "\n";
  }
  catch(Spud::Error& e) {
    std::cout << "ERROR: " << e.what() << "\n";
  }
}

int main() {
  Spud::Key bpublic;
  Spud::Key bsecret;
  Spud::keygen(bpublic, bsecret);

  Spud::Session a(bpublic);
  Spud::Session b(bpublic, bsecret);

  send(a, b, "> Hi Bob. This is Alice.");
  send(a, b, "> Have you seen my slippers?");
  send(a, b, "> I can't find them anywhere.");

  send(b, a, "< Hey Alice.");
  send(b, a, "< I think I might have...");

  send(a, b, "> Oh good!");

  send(b, a, "< Well...");

  send(a, b, "> Well what?");

  send(b, a, "< I've only seen one.");
  send(b, a, "< It wasn't in great shape.");
  send(b, a, "< My cat was trying to eat it.");

  send(a, b, "> Oh.");

  send(b, a, "< I think he may have already eaten the other one.");
  send(b, a, "< Sorry.");
  send(b, a, "< Do you want this one back?");

  send(a, b, "> Uhh...");
  send(a, b, "> You can keep it.");
  return 0;
}
