#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
int x;
namespace Y {
      void f(float){ x |= 1; };
      void h(int){ x |= 1; };
}
namespace Z {
      void h(double){ x |= 16; };
}
namespace A {
      using namespace Y;
      void f(int){ x |= 2; };
      void g(int){ x |= 1; };
      int i;
}
namespace B {
      using namespace Z;
      void f(char){ x |= 8; };
      int i;
}
namespace AB {
      using namespace A;
      using namespace B;
      void g() { x |= 4; };
}

void h()
{
      AB::g();     // g is declared directly in AB,
		   // therefore S is { AB::g() } and AB::g() is chosen
      AB::f(1);    // f is not declared directly in AB so the rules are
		   // applied recursively to A and B;
		   // namespace Y is not searched and Y::f(float)
		   // is not considered;
		   // S is { A::f(int), B::f(char) } and overload
		   // resolution chooses A::f(int)
      AB::f('c');  // as above but resolution chooses B::f(char)

      AB::h(16.8); // h is not declared directly in AB and
		   // not declared directly in A or B so the rules are
		   // applied recursively to Y and Z,
		   // S is { Y::h(int), Z::h(float) } and overload
		   // resolution chooses Z::h(float)
}
int main() {
    h();
    if( x != (16|8|4|2) ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
