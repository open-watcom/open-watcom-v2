#include "fail.h"
#ifdef __WATCOM_NAMESPACE__
namespace B {
      int b;
}
namespace A {
      using namespace B;
      int a;
}
namespace B {
      using namespace A;
}
void f()
{
      A::a++;  // ok: a declared directly in A, S is { A::a }
      B::a++;  // ok: both A and B searched (once), S is { A::a }
      A::b++;  // ok: both A and B searched (once), S is { B::b }
      B::b++;  // ok: b declared directly in B, S is { B::b }
}
int main() {
    f();
    if( A::a != 2 ) _fail;
    if( B::b != 2 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
