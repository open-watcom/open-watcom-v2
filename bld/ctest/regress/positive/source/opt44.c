#include "fail.h"

#ifdef _M_I86
    #define SHIFT_AMT   8
#else
    #define SHIFT_AMT   16
#endif

// Make sure that merging of shift operations works properly. At least
// since 10.x, code such as
//
//  i << s1;
//  i << s2;
//
// was merged to
//
//  i << (s1 + s2);
//
// which blew up if (s1 + s2) was equal or greater than register width.

int shl( int a )
{
    return( (a << SHIFT_AMT) << SHIFT_AMT );
}

int sar( int a )
{
    return( (a >> SHIFT_AMT) >> SHIFT_AMT );
}

unsigned shr( unsigned a )
{
    return( (a >> SHIFT_AMT) >> SHIFT_AMT );
}

int main( void )
{
    if( shl( 33 ) != 0 ) _fail;
    if( shl( -8 ) != 0 ) _fail;
    if( sar( 33 ) != 0 ) _fail;
    if( sar( -8 ) != -1 ) _fail;
    if( shr( 33 ) != 0 ) _fail;
    if( shr( -8 ) != 0 ) _fail;
    _PASS;
}
