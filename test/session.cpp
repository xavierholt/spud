#include "moka/moka.h"
#include "../src/spud.h"

Moka::Context tests("Session", [](Moka::Context& it) {
  it.should("be able to send a message", []() {
    Spud::Key pubkey;
    Spud::Key secret;
    Spud::keygen(pubkey, secret);

    Spud::Session a(pubkey);
    Spud::Session b(pubkey, secret);

    uint8_t m[2048] = "hello";
    uint8_t c[2048] = {0};
    uint8_t d[2048] = {0};

    uint32_t cl = a.encode(c, 2048, m, 5);
    uint32_t dl = b.decode(c, cl, d, 2048);

    must_equal(dl, 5);
    must_equal(std::memcmp(m, d, 5), 0);
  });

  it.should("be able to handle skipped messages", []() {
    Spud::Key pubkey;
    Spud::Key secret;
    Spud::keygen(pubkey, secret);

    Spud::Session a(pubkey);
    Spud::Session b(pubkey, secret);

    uint8_t m1[2048] = "hello";
    uint8_t m2[2048] = "there";
    uint8_t c[2048]  = {0};
    uint8_t d[2048]  = {0};

    uint32_t cl = a.encode(c, 2048, m1, 5);
    cl = a.encode(c, 2048, m2, 5);
    uint32_t dl = b.decode(c, cl, d, 2048);

    must_equal(dl, 5);
    must_equal(std::memcmp(m2, d, 5), 0);
  });

  it.should("be able to handle out-of-order messages", []() {
    Spud::Key pubkey;
    Spud::Key secret;
    Spud::keygen(pubkey, secret);

    Spud::Session a(pubkey);
    Spud::Session b(pubkey, secret);

    uint8_t m1[256] = "Let's play";
    uint8_t m2[256] = "leapfrog!";
    uint8_t c1[256] = {0};
    uint8_t c2[256] = {0};
    uint8_t d1[256] = {0};
    uint8_t d2[256] = {0};

    uint32_t cl1 = a.encode(c1, 256, m1, 10);
    uint32_t cl2 = a.encode(c2, 256, m2, 9);
    uint32_t dl2 = b.decode(c2, cl2, d2, 256);
    uint32_t dl1 = b.decode(c1, cl1, d1, 256);

    must_equal(dl1, 10);
    must_equal(std::memcmp(m1, d1, dl1), 0);

    must_equal(dl2, 9);
    must_equal(std::memcmp(m2, d2, dl2), 0);
  });
});

int main() {
  return tests.run();
}