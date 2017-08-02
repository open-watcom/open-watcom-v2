#include "dump.h"
class A { public: int a; A( int a1, int a2 ){ a = a1+a2; }; };
static A esca( 3, 3 );
int main( void )
{
    DUMP( esca );
    return 0;
}
