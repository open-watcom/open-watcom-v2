#include "fail.h"
// Test different argument passing conventions
//
// 93/08/19 -- J.W.Welch    -- defined

#include <stdarg.h>
#include <stdio.h>

#if defined( _M_IX86 )

#pragma aux REVERSED parm reverse;
#define __reversed __pragma( "REVERSED" )

#else

#define __reversed

#endif


#define ELLARGS                 \
    int a2;                     \
    va_list arg;                \
    va_start( arg, a1 );        \
    a2 = va_arg( arg, int );    \
    va_end( arg );


#define CHKARGS( v1, v2, txt ) if( a1 != v1 || a2 != v2 ) myfail( txt );

#define VOIDFUN( fun, v1, v2 ) fun( int a1, int a2 ) \
    { CHKARGS( v1, v2, #fun ); }

#define VOIDDEFFUN( fun, v1, v2 ) fun( int a1, int a2 = v2 ) \
    { CHKARGS( v1, v2, #fun ); }

#define VOIDELLFUN( fun, v1, v2 ) fun( int a1, ... ) \
    { ELLARGS CHKARGS( v1, v2, #fun ); }

#define RETNFUN( fun, v1, v2, v3 ) fun( int a1, int a2 ) \
    { CHKARGS( v1, v2, #fun ); return S( v3 ); }

#define RETNDEFFUN( fun, v1, v2, v3 ) fun( int a1, int a2 = v2 ) \
    { CHKARGS( v1, v2, #fun ); return S( v3 ); }

#define RETNELLFUN( fun, v1, v2, v3 ) fun( int a1 ... ) \
    { ELLARGS CHKARGS( v1, v2, #fun ); return S( v3 ); }


void myfail( const char* msg )
{
    printf( "ERROR -- failed %s\n", msg );
    fail(__LINE__);
}


struct S {
    int v;
    S( int i ) : v(i) {}
};


void            VOIDDEFFUN( f1, 1,   2 )
S               RETNDEFFUN( f2, 3,   4, 129 )
void __reversed VOIDDEFFUN( f3, 5,   6 ) 
S __reversed    RETNDEFFUN( f4, 7,   8, 130 )
S __pascal      RETNDEFFUN( f5, 13, 14, 131 )


struct T {
    void memb( int, int = 10 );
    void __reversed rev( int, int = 12 );
    void __pascal pas( int, int = 16 );
    S smemb( int, int = 26 );
    S __reversed srev( int, int = 28 );
    S __pascal spas( int, int = 30 );
    void ememb( int ... );
    S esmemb( int ... );
};

T tv;

void            VOIDFUN   ( T::memb, 9, 10 )
void __reversed VOIDFUN   ( T::rev, 11, 12 );
void __pascal   VOIDFUN   ( T::pas, 15, 16 );
S               RETNFUN   ( T::smemb, 25, 26, 132  )
S __reversed    RETNFUN   ( T::srev, 27, 28, 133 );
S __pascal      RETNFUN   ( T::spas, 29, 30, 134  );
void            VOIDELLFUN( T::ememb, 31, 32 )
S               RETNELLFUN( T::esmemb, 33, 34, 135 )

extern "C" void             VOIDDEFFUN( cf1, 17, 18 );
extern "C" void __reversed  VOIDDEFFUN( cf2, 19, 20 );
extern "C" void __pascal    VOIDDEFFUN( cf3, 21, 22 );

void            VOIDELLFUN( el1, 23, 24 );
S               RETNELLFUN( el2, 35, 36, 136 );
extern "C" void VOIDELLFUN( el3, 37, 38 );
extern "C" S    RETNELLFUN( el4, 39, 40, 137 );

struct DV : virtual S {
    DV( unsigned );
    DV( int ... );
    __reversed DV( char*, int );
    __pascal DV( void*, int );
};

DV::DV( unsigned a1 ) : S( a1 ) {
    int a2 = a1;
    CHKARGS( 200, 200, "DV::DV(unsigned)" );
}

DV::DV( int a1 ... ) : S( a1 ) {
    ELLARGS
    CHKARGS( 200, 1, "DV::DV(int...)" );
    v = 201;
}

DV::DV( char* a1, int a2 ) : S( a2 ) {
    CHKARGS( 0, 202, "__reversed DV::DV(void*,int)" );
}

DV::DV( void* a1, int a2 ) : S( a2 ) {
    CHKARGS( 0, 203, "__pascal DV::DV(void*,int)" );
}

#define CHKRET( f, a1, a2, sv, rv ) \
    S sv = f( a1, a2 ); if( sv.v != rv ) myfail( "return from" #f );

#define CHKDEFRET( f, a1, sv, rv ) \
    S sv = f( a1 ); if( sv.v != rv ) myfail( "return from" #f );

#define CHKDV( var, val ) if( var.v != val ) myfail( "DV failed: " #val );

int main()
{
    f1( 1, 2 );
    CHKRET( f2, 3, 4, s1, 129 );
    f3( 5, 6 );
    CHKRET( f4, 7, 8, s2, 130 );
    CHKRET( f5, 13, 14, s3, 131 );

    tv.memb( 9, 10 );
    tv.rev( 11, 12 );
    tv.pas( 15, 16 );
    CHKRET( tv.smemb, 25, 26, s4, 132 );
    CHKRET( tv.srev,  27, 28, s5, 133 );
    CHKRET( tv.spas,  29, 30, s6, 134 );
    tv.ememb( 31, 32 );
    CHKRET( tv.esmemb,33, 34, s7, 135 );

    cf1( 17, 18 );
    cf2( 19, 20 );
    cf3( 21, 22 );
    
    f1( 1 );
    CHKDEFRET( f2, 3, sa, 129 );
    f3( 5 );
    CHKDEFRET( f4, 7, sb, 130 );
    CHKDEFRET( f5, 13, sc, 131 );

    tv.memb( 9 );
    tv.rev( 11 );
    tv.pas( 15 );
    CHKDEFRET( tv.smemb, 25, sd, 132 );
    CHKDEFRET( tv.srev,  27, se, 133 );
    CHKDEFRET( tv.spas,  29, sf, 134 );

    cf1( 17 );
    cf2( 19 );
    cf3( 21 );

    DV dv01( (unsigned)200 );       CHKDV( dv01, 200 );
    DV dv02( 200, 1 );              CHKDV( dv02, 201 );
    DV dv03( (char*)0, 202 );       CHKDV( dv03, 202 );
    DV dv04( (void*)0, 203 );       CHKDV( dv04, 203 );

    _PASS;
}
