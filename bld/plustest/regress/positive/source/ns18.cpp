#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace A {
      struct x { };
      int x = 'x';
      int y;
}
namespace B {
      struct y {};
}
namespace C {
      using namespace A;
      using namespace B;
      int i = C::x; // ok, A::x (of type 'int')
}
int main() {
    if( C::i != 'x' ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
