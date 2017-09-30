#include "dump.h"

#if __WATCOM_REVISION__ < 8
struct B;

struct A {
    A() BAD;
    A( B ) BAD;
};

struct B {
    operator A () { BAD; return A(); }
    operator int () { GOOD; return 0; }
};

void f( A ) BAD;
void f( int ) GOOD;

int main( void ) {
    B b;
    f( b );
    CHECK_GOOD( 13+17 );
    return errors != 0;
}
#else

int main( void )
{
  FORCE_GOOD(13);
  FORCE_GOOD(17);
  CHECK_GOOD(13+17);
  return errors != 0;
}

#endif
