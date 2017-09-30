#include "dump.h"

const int a = 1;
int b = a;

void g0( void )
{
    DUMP( a );
    DUMP( b );
}

void g1( void )
{
    const int a = 2;
    int b = a;

    DUMP( a );
    DUMP( b );
}

void g2( void )
{
    static const int a = 3;
    int b = a;

    DUMP( a );
    DUMP( b );
}

void g3( void )
{
    const int a = 4;
    DUMP( a );
}

void g4( void )
{
    static const int a = 5;
    DUMP( a );
}
int main( void )
{
    g0();
    g1();
    g2();
    g3();
    g4();
    return 0;
}
