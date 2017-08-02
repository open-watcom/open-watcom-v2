#include "dump.h"

//Trivial Conversions
// This file should compile without error.

// from T	to T&
void fri( ... ) BAD;
void fri( int & ) GOOD;
void gri( int a )
{
    fri( a );
}

// from T&	to T
void fi( ... ) BAD;
void fi( int ) GOOD;
void gi( int &a )
{
    fi( a );
}

// from T[]	to T*
void fpi( ... ) BAD;
void fpi( int * ) GOOD;
void gpi( int a[] )
{
    fpi( a );
}

// from T*	to T[]
void fai( ... ) BAD;
void fai( int [] ) GOOD;
void gai( int *a )
{
    fai( a );
}

// from T(args)	to T(*)(args)
void fpfi( ... ) BAD;
typedef int (*pFTiRi)( int );
void fpfi( pFTiRi ) GOOD;
int  hpfi( int ){ return 0; };
void gpfi( void )
{
    fpfi( hpfi );
}

// from T	to const T
void fci( ... ) BAD;
void fci( const int ) GOOD;
void gci( int a )
{
    fci( a );
}

// from T	to volatile T
void fvi( ... ) BAD;
void fvi( volatile int ) GOOD;
void gvi( int a )
{
    fvi( a );
}

// from T*	to const T*
typedef int *PIcpi;
void fcpi( ... ) BAD;
void fcpi( const PIcpi ) GOOD;
void gcpi( PIcpi a )
{
    fcpi( a );
}

// from T*	to volatile T*
typedef int *PIvpi;
void fvpi( ... ) BAD;
void fvpi( volatile PIvpi ) GOOD;
void gvpi( PIvpi a )
{
    fvpi( a );
}

// from T& to T * const &
void goo(...) BAD;
void goo( char * const & ) GOOD;
void foo( char a )
{
    goo( &a );
}

int main( void ) {
    int gri_a;
    int &gi_a = gri_a;
    int gpi_a[3];
    int *gai_a;
    int gci_a;
    int gvi_a;
    PIcpi gcpi_a;
    PIvpi gvpi_a;
    int a;

    gri( gri_a );
    gi( gi_a );
    gpi( gpi_a );
    gai( gai_a );
    gpfi( );
    gci( gci_a );
    gvi( gvi_a );
    gcpi( gcpi_a );
    gvpi( gvpi_a );
    foo(a);
    CHECK_GOOD( 372+84 );
    return errors != 0;
}
