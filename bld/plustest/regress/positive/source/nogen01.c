#if defined( _M_I86 )

#include "fail.h"

#include <fstream.h>
#include <iostream.h>

struct tryit {
  ifstream fin;
  tryit();
  ~tryit();
};

int got_here;

tryit::tryit() {
    ++got_here;
}

tryit::~tryit() {
    ++got_here;
}

int main() {
    {
    tryit xx;
    }
    if( got_here != 2 ) fail(__LINE__);
    _PASS;
}

#else

#include "fail.h"

ALWAYS_PASS

#endif
