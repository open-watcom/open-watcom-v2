#include "fail.h"
// 93/11/18 (jww) fixed on 10.0 (not on 9.5)

// reported by a customer (AFS)
/* Watcom 9.5, patch A:  Executing the "for" loop causes Foo's constructor
   to be called 4 times (correct), and its destructor to be called
   1 time (oops).

   Seems to be independent of -d1 or -d2, optimizer flags, etc.
*/

#include <stdio.h>

unsigned count;

class Foo
{
public:
  Foo(int);
  ~Foo();

  int x() const;
private:
  int x_;
};

Foo::Foo(int x) : x_(x)
{
    ++count;
}
Foo::~Foo()
{
    --count;
}
int Foo::x() const
{
  return x_;
}

Foo func()
{
  return Foo(3);
}

int main(int, char**)
{
    for (int i = 0 ; i < func().x() ; i++)
    {
    }
    if( count != 0 ) fail(__LINE__);
    _PASS;
}
