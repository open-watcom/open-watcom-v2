#include "dump.h"

struct A {
    int a1;
    char a2;
    float a3;
};


int main( void )
{
    A a = { 1, 'a', 2.0 };
    A *pa = &a;
    int A::*pa1 = &A::a1;
    char A::*pa2 = &A::a2;
    float A::*pa3 = &A::a3;

    DUMP( a );
    DUMP( *pa );

    DUMP( a.a1 );
    DUMP( pa->a1 );
    DUMP( a.*pa1 );
    DUMP( pa->*pa1 );

    DUMP( a.a2 );
    DUMP( pa->a2 );
    DUMP( a.*pa2 );
    DUMP( pa->*pa2 );

    DUMP( a.a3 );
    DUMP( pa->a3 );
    DUMP( a.*pa3 );
    DUMP( pa->*pa3 );
    return 0;
}
