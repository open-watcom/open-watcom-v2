#include "dump.h"
int main( void )
{
    int a = 12345;
    const int b = a;
    int c = b;
    const int *p0 = &a;
    const int *p1 = &b;
    int *p2 = &a;

    int * const p3 = p2;
    const int *p4 = p1;

    DUMP( a );
    DUMP( b );
    DUMP( c );
    DUMP( *p0 );
    DUMP( *p1 );
    DUMP( *p2 );
    DUMP( *p3 );
    DUMP( *p4 );
    return 0;
}
