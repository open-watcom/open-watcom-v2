#include "dump.h"
int bar( int a ) { return a*3; }

void foo( void )
{
    auto int a[20] = { 1, 2, 3, 4, bar(4), bar(5), 32 };
    DUMP( a );
}
class C {
    static C a[];
    static C b[];
    static C c;
    static C d;
    C();
};
C::C() GOOD;
C C::a[] = { C(), C() };
C C::b[2];
C C::c = C();
C C::d;
int main( void )
{
    foo();
    CHECK_GOOD( 96 );
    return 0;
}
