#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
int m;
namespace A {
      namespace B {
	      void f1(int);
      }
}
namespace C {
      namespace D {
	      void f1(int){ m -= 100; };
      }
}
using namespace A;
using namespace C::D;
void B::f1(int){ m += 2; }  // okay, defines A::B::f1(int)
void f1(int){ m += 1; }  // okay, defines ::f1(int)

int main() {
    ::f1(__LINE__);
    A::B::f1(__LINE__);
    if( m != 3 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
