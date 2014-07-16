#include "fail.h"
#include <limits.h>

int y;
int x;

unsigned long check[] = {
    0x0000007f,
    0x0000003f,
    0x0000001f,
    0x0000000f,
    0x00000007,
    0x00000003,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
#if INT_MAX > 65535
    0x7fffffff,
    0x3fffffff,
    0x1fffffff,
    0x0fffffff,
    0x07ffffff,
    0x03ffffff,
#else
    0x00007fff,
    0x00003fff,
    0x00001fff,
    0x00000fff,
    0x000007ff,
    0x000003ff,
#endif
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0x7fffffff,
    0x3fffffff,
    0x1fffffff,
    0x0fffffff,
    0x07ffffff,
    0x03ffffff,
};

void disp( unsigned long v ) {
    if( v != check[y++] ) {
	printf( "%08lx\n", v );
	_fail;
    }
}

void use( signed int v ) {
    disp( v );
    x += 1;
}
void use( unsigned int v ) {
    disp( v );
    x += 2;
}
void use( signed int long v ) {
    disp( v );
    x += 4;
}
void use( unsigned int long v ) {
    disp( v );
    x += 8;
}

unsigned char _uc = 1;
signed char _sc = 2;
int _i = 3;
unsigned _u = 4;
signed long _l = 5;
unsigned long _ul = 6;

void test( unsigned char uc, signed char sc, int i, unsigned u, signed long l, unsigned long ul ) {

    use( uc >> _uc );	// si
    use( uc >> _sc );	// si
    use( uc >> _i );	// si
    use( uc >> _u );	// si
    use( uc >> _l );	// si
    use( uc >> _ul );	// si

    if( x != 6*1 ) _fail;
    x = 0;

    use( sc >> _uc );	// si
    use( sc >> _sc );	// si
    use( sc >> _i );	// si
    use( sc >> _u );	// si
    use( sc >> _l );	// si
    use( sc >> _ul );	// si

    if( x != 6*1 ) _fail;
    x = 0;

    use( i >> _uc );	// si
    use( i >> _sc );	// si
    use( i >> _i );	// si
    use( i >> _u );	// si
    use( i >> _l );	// si
    use( i >> _ul );	// si

    if( x != 6*1 ) _fail;
    x = 0;

    use( u >> _uc );	// ui
    use( u >> _sc );	// ui
    use( u >> _i );	// ui
    use( u >> _u );	// ui
    use( u >> _l );	// ui
    use( u >> _ul );	// ui

    if( x != 6*2 ) _fail;
    x = 0;

    use( l >> _uc );	// sl
    use( l >> _sc );	// sl
    use( l >> _i );	// sl
    use( l >> _u );	// sl
    use( l >> _l );	// sl
    use( l >> _ul );	// sl

    if( x != 6*4 ) _fail;
    x = 0;

    use( ul >> _uc );	// ul
    use( ul >> _sc );	// ul
    use( ul >> _i );	// ul
    use( ul >> _u );	// ul
    use( ul >> _l );	// ul
    use( ul >> _ul );	// ul

    if( x != 6*8 ) _fail;
    x = 0;
}

int main() {
    test( -1, -1, -1, -1, -1, -1 );
    _PASS;
}
