#include "fail.h"

struct foo {
  int a,b;
  foo operator=(foo z) {
     a = z.a+1;
     b = z.b+2;
     return *this;
  }
};

foo a,b,c;

int main() {
   a = b = c;
   if( b.a != 1 ) _fail;
   if( b.b != 2 ) _fail;
   if( a.a != 2 ) _fail;
   if( a.b != 4 ) _fail;
   _PASS;
}
