#include "fail.h"

// Case involving virtual base classes that shows that access
// is influenced by friendship at intermediate steps.
struct C;
struct A { int i; };
struct B : virtual private A { friend A * f(); };
struct BB : virtual private A { friend A * fb(); };
struct C : virtual private B, virtual private BB {
  friend A * f();
  friend A * fb();
};
A * f() {
  C *p = new C;
  p->i = 1;  // Okay because of friendship on C-->B-->A
  return p;
}
A * fb() {
  C *p = new C;
  p->i = 2;  // Okay because of friendship on C-->BB-->A
  return p;
}

int main()
{
    A *p1 = f();
    A *p2 = fb();

    if( p1->i != 1 ) fail(__LINE__);
    if( p2->i != 2 ) fail(__LINE__);
    _PASS;
}
