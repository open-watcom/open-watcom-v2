#include "dump.h"


void f2( char, int (*)[5][4][3][2][1], float ) GOOD;
void f2( char, int (*)[5][4][3][2][2], float ) GOOD;
void g2( void )
{
         char a;
	 int b[1][5][4][3][2][1];
	 int c[1][5][4][3][2][2];
	 float d;

    f2( a, b, d );
    f2( a, c, d );
}
int main( void ) {
    g2();
    CHECK_GOOD( 9 );
    return errors != 0;
}
