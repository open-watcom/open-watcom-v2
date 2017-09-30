#include "fail.h"

void f()
{ }

struct A
{
  ~A()
  {
      f();
  }
};

struct B
{
  B( const A &a = A() )
  { }
};

static const B b;


int main()
{
    _PASS;
}
