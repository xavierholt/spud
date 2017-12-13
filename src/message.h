#ifndef SPUD_MESSAGE_H
#define SPUD_MESSAGE_H

namespace Spud {
  class Message {
    uint8_t mData[2048];
  public:
    uint8_t* data();

    uint32_t number() const;
    uint32_t prev() const;
  };
}

#endif
