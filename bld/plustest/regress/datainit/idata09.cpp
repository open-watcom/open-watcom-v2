#include "dump.h"
class A { public: int a; A( int a1, int a2 ){ a = a1+a2; }; };
static A esca[3] = { A( 1, 1 ), A( 2, 2 ), A( 3, 3 ) };
int main( void )
{
    DUMP( esca );
    DUMP( esca[0] );
    DUMP( esca[1] );
    DUMP( esca[2] );
    return 0;
}
