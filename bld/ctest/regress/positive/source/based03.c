#include "fail.h"

#ifdef _M_I86    // 16-bit only!

#include <i86.h>

// Test casts to __segment type

__segment get_ds( void );
#pragma aux get_ds =    \
    "mov    ax,ds"      \
    value [ax]

__segment get_ss( void );
#pragma aux get_ss =    \
    "mov    ax,ss"      \
    value [ax]

__segment get_cs( void );
#pragma aux get_cs =    \
    "mov    ax,cs"      \
    value [ax]

__segment seg = 0x3456;

__segment fn1( int ptr )
{
    return( (__segment)&ptr );      // returns SS (may equal DS)
}

__segment fn2( void __far *ptr )
{
    return( (__segment)ptr );       // returns segment portion of ptr
}

__segment fn3( int val )
{
    return( (__segment)val );       // returns unmodified 'val'
}

__segment fn4( void )
{
    return( (__segment)0xB800 );    // returns unmodified constant
}

__segment fn5( void __based( seg ) *ptr )
{
    return( (__segment)ptr );       // returns current value of 'seg'
}

__segment fn6( void __based( __segname( "_CODE" ) ) *ptr )
{
    return( (__segment)ptr );       // returns code segment
}

__segment fn7( void )
{
    // returns data segment; talk about bizarre!
    return( (__segment)(void __based( __segname( "_DATA" ) )*)&fn7 );
}

int main( void )
{
    int                 var;
    int __far           *pvar;
    int __based( seg )  *bvar;
    int __based( void ) *bvvar;
    __segment           ds_seg;
    __segment           ss_seg;
    __segment           cs_seg;

    pvar   = &var;
    cs_seg = get_cs();
    ds_seg = get_ds();
    ss_seg = get_ss();

    if( fn1( 3 ) != ss_seg ) fail( __LINE__ );
    if( fn2( pvar ) != FP_SEG( pvar ) ) fail( __LINE__ );
    if( fn3( 0x7654 ) != 0x7654 ) fail( __LINE__ );
    if( fn4() != 0xB800 ) fail( __LINE__ );
    if( fn5( bvar ) != seg ) fail( __LINE__ );
    if( fn6( bvvar ) != cs_seg ) fail( __LINE__ );
    if( fn7() != ds_seg ) fail( __LINE__ );

    _PASS;
}

#else

ALWAYS_PASS

#endif
