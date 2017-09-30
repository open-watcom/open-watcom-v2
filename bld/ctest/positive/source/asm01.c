#include "fail.h"

int parity( long x )
{
    volatile char result;

#if defined(_M_IX86)
#if _M_IX86 < 300
extern char __parity( short );
#pragma aux __parity = \
    "xor al,ah" \
    "xor ah,ah" \
    "test al,al" \
    "jnp L1" \
    "mov ah,1" \
    "L1:" \
    "mov result,ah" \
    parm caller [ax] value [ah];
#else
extern char __parity( short );
#pragma aux __parity = \
    "xor al,ah" \
    "setp al" \
    "mov byte ptr result,al" \
    parm caller [ax] value [al];
#endif
    x &= 0x0ffffL;
    __parity( x );
#else
    x &= 0x0ffffL;
    x = ( x & 0x00ff ) ^ (( x >> 8 ) & 0x00ff );
    x = ( x & 0x000f ) ^ (( x >> 4 ) & 0x000f );
    x = ( x & 0x0003 ) ^ (( x >> 2 ) & 0x0003 );
    x = ( x & 0x0001 ) ^ (( x >> 1 ) & 0x0001 );
    result = !x;
#endif
    return( result );
}

int main() {
    if( parity( 0 ) != 1 ) fail(__LINE__);
    if( parity( 1 ) != 0 ) fail(__LINE__);
    if( parity( 2 ) != 0 ) fail(__LINE__);
    if( parity( 3 ) != 1 ) fail(__LINE__);
    if( parity( 0x111 ) != 0 ) fail(__LINE__);
    if( parity( 0x2222 ) != 1 ) fail(__LINE__);
    if( parity( 0x3333 ) != 1 ) fail(__LINE__);
    _PASS;
}
