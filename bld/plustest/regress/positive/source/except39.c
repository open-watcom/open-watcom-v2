#include "fail.h"

int x = 0;

static void nothing() {
 x++;
}

struct foo {
  ~foo();
};
foo::~foo() { nothing(); }

struct bar : foo {
 bar() { throw 0; }
};

int main()
{
  try {
    bar b;
  }
  catch(...) {}
  if( x != 1 ) _fail;
  _PASS;
}
