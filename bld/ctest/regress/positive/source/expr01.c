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

void usesi( signed int v ) {
    disp( v );
    x += 1;
}
void useui( unsigned int v ) {
    disp( v );
    x += 2;
}
void usesl( signed int long v ) {
    disp( v );
    x += 4;
}
void useul( unsigned int long v ) {
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

    usesi( uc >> _uc );   // si
    usesi( uc >> _sc );   // si
    usesi( uc >> _i );    // si
    usesi( uc >> _u );    // si
    usesi( uc >> _l );    // si
    usesi( uc >> _ul );   // si

    if( x != 6*1 ) _fail;
    x = 0;

    usesi( sc >> _uc );   // si
    usesi( sc >> _sc );   // si
    usesi( sc >> _i );    // si
    usesi( sc >> _u );    // si
    usesi( sc >> _l );    // si
    usesi( sc >> _ul );   // si

    if( x != 6*1 ) _fail;
    x = 0;

    usesi( i >> _uc );    // si
    usesi( i >> _sc );    // si
    usesi( i >> _i );     // si
    usesi( i >> _u );     // si
    usesi( i >> _l );     // si
    usesi( i >> _ul );    // si

    if( x != 6*1 ) _fail;
    x = 0;

    useui( u >> _uc );    // ui
    useui( u >> _sc );    // ui
    useui( u >> _i );     // ui
    useui( u >> _u );     // ui
    useui( u >> _l );     // ui
    useui( u >> _ul );    // ui

    if( x != 6*2 ) _fail;
    x = 0;

    usesl( l >> _uc );    // sl
    usesl( l >> _sc );    // sl
    usesl( l >> _i );     // sl
    usesl( l >> _u );     // sl
    usesl( l >> _l );     // sl
    usesl( l >> _ul );    // sl

    if( x != 6*4 ) _fail;
    x = 0;

    useul( ul >> _uc );   // ul
    useul( ul >> _sc );   // ul
    useul( ul >> _i );    // ul
    useul( ul >> _u );    // ul
    useul( ul >> _l );    // ul
    useul( ul >> _ul );   // ul

    if( x != 6*8 ) _fail;
    x = 0;
}

int main() {
    test( -1, -1, -1, -1, -1, -1 );
    _PASS;
}
