#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace A {
      int a;
}
namespace B {
      using namespace A;
}
namespace C {
      using namespace A;
}
namespace BC {
      using namespace B;
      using namespace C;
}
void f()
{
      BC::a++;  // ok: S is { A::a, A::a }
}
namespace D {
      using A::a;
}
namespace BD {
      using namespace B;
      using namespace D;
}

void g()
{
      BD::a++;  // ok: S is { A::a, A::a }
}

int main() {
    f();
    g();
    if( A::a != 2 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
