#include "fail.h"

int ctr = 100;

class foo {
  public:

  int a,b;
  foo operator=(foo z)
  {
     a = z.a;
     b = z.b;
     return *this;
  }
  foo();
};

foo::foo() {
  a = ctr ++;
  b = ctr ++;
}


int main() {
    foo a,b,c;
    if( a.a != 100 ) _fail;
    if( a.b != 101 ) _fail;
    if( b.a != 102 ) _fail;
    if( b.b != 103 ) _fail;
    if( c.a != 104 ) _fail;
    if( c.b != 105 ) _fail;
    a = b = c;
    if( a.a != 104 ) _fail;
    if( a.b != 105 ) _fail;
    if( b.a != 104 ) _fail;
    if( b.b != 105 ) _fail;
    if( c.a != 104 ) _fail;
    if( c.b != 105 ) _fail;
    _PASS;
}
