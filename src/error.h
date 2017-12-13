#ifndef SPUD_ERROR_H
#define SPUD_ERROR_H

#include <string>

namespace Spud {
  class Error: std::exception {
  protected:
    std::string mMessage;
  public:
    Error(const std::string& message): mMessage(message) {
      mMessage.c_str();
    }

    ~Error() throw() {
      // Nothing to do.
    }

    const char* what() const throw() {
      return mMessage.c_str();
    }
  };
}

#endif
