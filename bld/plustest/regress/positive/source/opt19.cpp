// wpp -1 fails
#include "fail.h"

void check( short x, short y ) {
    if( x != 15 ) fail(__LINE__);
    if( y != 23 ) fail(__LINE__);
}

void test( short c )
{
    short n;
    
    if( c > 255 ) {
	n = ( c >> 8 );
	check( n, ( c & 0x00FF ) );
    } else {
	fail(__LINE__);
    }
}

int main() {
    test( 0x0f17 );
    _PASS;
}
