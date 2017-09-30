#include "dump.h"
int main( void )
{
    double d = 1.0;

    double &rd = d;		// ok
    const double &rcd = d;	// ok

    const double &rcd2 = 2;	// ok, make a temporary
    printf( "%f\n", d );
    printf( "%f\n", rd );
    printf( "%f\n", rcd );
    printf( "%f\n", rcd2 );
    return 0;
}
