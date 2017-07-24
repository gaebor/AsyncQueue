#include "aq/Exception.h"

namespace aq {

    Exception::Exception(const char* str) : what_(str) {}
    const char* Exception::what() const throw () { return what_; }

}
