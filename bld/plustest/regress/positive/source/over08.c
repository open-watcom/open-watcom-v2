#include "fail.h"

class B {
    public:
      B() { }
      int b, c;
      int operator [] (int) { return 1; }
      int operator [] (int) const { return 2; }

    private:
      int a;
};

class D : private B {
    public:
      D() { }
      B::c;
      B::operator [];
      int e;

    private:
      int d;
};

int main()
{
    const D d;
    if( d[5] != 2 ) fail(__LINE__);
    _PASS;
}
