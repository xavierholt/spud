#ifndef SPUD_ERROR_H
#define SPUD_ERROR_H

#include <string>

// This is the Spud error class.
// It's thrown whenever something goes wrong.
// The error messages are a bit generic...

namespace Spud {
  class Error: public std::exception {
  protected:
    const char* mMessage;
  public:
    Error(const char* message): mMessage(message) {
      // All done.
    }

    ~Error() throw() {
      // Nothing to do.
    }

    const char* what() const throw() {
      return mMessage;
    }
  };
}

#endif
