/*
 *  Constant folding of a signed '%' after inlining: the constants of a
 *  narrower type held their bits as unsigned values (-1234L as 0xFFFFFB2E)
 *  and FoldMod divided them as 64-bit integers (98765432L % -1234L gave
 *  98765432, 30000 % -7 gave 30000)
 */

#include "fail.h"

long lmod( long x, long y ) { return( x % y ); }
int  imod( int x, int y ) { return( x % y ); }
long ldiv_( long x, long y ) { return( x / y ); }
unsigned long ulmod( unsigned long x, unsigned long y ) { return( x % y ); }

int main( void )
{
    if( lmod( 98765432L, -1234L ) != 1008L ) _fail;
    if( lmod( -98765432L, 1234L ) != -1008L ) _fail;
    if( lmod( -98765432L, -1234L ) != -1008L ) _fail;
    if( lmod( 98765432L, 1234L ) != 1008L ) _fail;
    if( lmod( -2147483647L - 1, -65536L ) != 0L ) _fail;
    if( lmod( -2147483647L - 1, 7L ) != -2L ) _fail;
    if( imod( 30000, -7 ) != 5 ) _fail;
    if( imod( -30000, 7 ) != -5 ) _fail;
    if( imod( -30000, -7 ) != -5 ) _fail;
    if( imod( -1, 3 ) != -1 ) _fail;
    if( ldiv_( 98765432L, -1234L ) != -80036L ) _fail;
    if( ulmod( 4294966062UL, 98765432UL ) != 4294966062UL % 98765432UL ) _fail;
    _PASS;
}
