#include "fail.h"
#include <limits.h>

int y;
int x;

unsigned long check[] = {
    0x000001fe,
    0x000003fc,
    0x000007f8,
    0x00000ff0,
    0x00001fe0,
    0x00003fc0,
    0xfffffffe,
    0xfffffffc,
    0xfffffff8,
    0xfffffff0,
    0xffffffe0,
    0xffffffc0,
    0xfffffffe,
    0xfffffffc,
    0xfffffff8,
    0xfffffff0,
    0xffffffe0,
    0xffffffc0,
#if INT_MAX > 65535
    0xfffffffe,
    0xfffffffc,
    0xfffffff8,
    0xfffffff0,
    0xffffffe0,
    0xffffffc0,
#else
    0x0000fffe,
    0x0000fffc,
    0x0000fff8,
    0x0000fff0,
    0x0000ffe0,
    0x0000ffc0,
#endif
    0xfffffffe,
    0xfffffffc,
    0xfffffff8,
    0xfffffff0,
    0xffffffe0,
    0xffffffc0,
    0xfffffffe,
    0xfffffffc,
    0xfffffff8,
    0xfffffff0,
    0xffffffe0,
    0xffffffc0,
};

void disp( unsigned long v ) {
#if 0
    printf( "%08lx\n", v );
#else
    if( v != check[y++] ) {
        printf( "%08lx\n", v );
        _fail;
    }
#endif
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

    usesi( uc << _uc );   // si
    usesi( uc << _sc );   // si
    usesi( uc << _i );    // si
    usesi( uc << _u );    // si
    usesi( uc << _l );    // si
    usesi( uc << _ul );   // si

    if( x != 6*1 ) _fail;
    x = 0;

    usesi( sc << _uc );   // si
    usesi( sc << _sc );   // si
    usesi( sc << _i );    // si
    usesi( sc << _u );    // si
    usesi( sc << _l );    // si
    usesi( sc << _ul );   // si

    if( x != 6*1 ) _fail;
    x = 0;

    usesi( i << _uc );    // si
    usesi( i << _sc );    // si
    usesi( i << _i );     // si
    usesi( i << _u );     // si
    usesi( i << _l );     // si
    usesi( i << _ul );    // si

    if( x != 6*1 ) _fail;
    x = 0;

    useui( u << _uc );    // ui
    useui( u << _sc );    // ui
    useui( u << _i );     // ui
    useui( u << _u );     // ui
    useui( u << _l );     // ui
    useui( u << _ul );    // ui

    if( x != 6*2 ) _fail;
    x = 0;

    usesl( l << _uc );    // sl
    usesl( l << _sc );    // sl
    usesl( l << _i );     // sl
    usesl( l << _u );     // sl
    usesl( l << _l );     // sl
    usesl( l << _ul );    // sl

    if( x != 6*4 ) _fail;
    x = 0;

    useul( ul << _uc );   // ul
    useul( ul << _sc );   // ul
    useul( ul << _i );    // ul
    useul( ul << _u );    // ul
    useul( ul << _l );    // ul
    useul( ul << _ul );   // ul

    if( x != 6*8 ) _fail;
    x = 0;
}

int main() {
    test( -1, -1, -1, -1, -1, -1 );
    _PASS;
}
