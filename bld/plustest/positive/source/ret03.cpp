#include "fail.h"
// #pragma on ( dump_emit_ic )

// Test for proper destruction with returns in the middle of routines.

void error( const char* text )
{
    printf( "FAILED RETO3 -- %s\n", text );
    fail(__LINE__);
}

int dtors;
int ctors;

struct S {
    int s;
    S( int v ) : s(v) { ++ ctors; }
    ~S() { ++ dtors; }
};

inline void foo( int v )
{
    S s1( 1 );
    if( v == 1 ) return;
    S s2( 2 );
    if( v == 2 ) return;
    S s3( 3 );
    if( v == 3 ) return;
    error( "foo has bad value" );
}

inline void boo( int v )
{
    foo( v );
    S s1( 10 );
    if( v == 1 ) return;
    S s2( 20 );
    if( v == 2 ) return;
    S s3( 30 );
    if( v == 3 ) return;
    error( "boo has bad value" );
}

inline void poo( int v )
{
    boo( v );
    S s1( 10 );
    if( v == 1 ) return;
    S s2( 20 );
    if( v == 2 ) return;
    S s3( 30 );
    if( v == 3 ) return;
    error( "poo has bad value" );
}

void check_dtors()
{
    if( dtors != ctors ) {
        error( "bad number of dtors" );
    }
    dtors = 0;
    ctors = 0;
}

int main()
{
    poo( 1 );
    check_dtors();
    poo( 2 );
    check_dtors();
    poo( 1 );
    check_dtors();

    _PASS;
}

// #pragma on ( dump_exec_ic )
