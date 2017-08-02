#include "dump.h"

struct A {
    int a1;
    int a2;
    int a3;
};

int main( void )
{
    int A::*pa[4] = { &A::a1, &A::a2, &A::a3 };

    DUMP( pa );
    return 0;
}
