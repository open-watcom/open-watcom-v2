#include "fail.h"
// Test implementation of ellipsis functions

// #define DBG                  // uncomment for debugging output

#include <stdio.h>
#include <stdlib.h>

int ctor_count = 0;

struct GBL
{
    ~GBL();
};

GBL::~GBL()
{
    if( ctor_count > 0 ) {
        fail( __LINE__);	// un-dtored elements
    }
}

GBL stat;

struct S                    // Small struct ==> passed back in register
{   int s;
    S( int );
    ~S();
    S( const S& );
    S S_copy( int n, ... );
};

S::S( int sv ) : s(sv)
{
    if( s > 100 ) {
        fail( __LINE__);	// CTOR S -- impossible initialization value
    }
    ++ctor_count;
    #ifdef DBG
        printf( "CTOR[%x] S(%d) count = %d\n", this, s, ctor_count );
    #endif
}

S::S( const S& src ) : s( src.s ) 
{
    if( s > 100 ) {
        fail( __LINE__);	// COPY S -- impossible initialization value
    }
    ++ctor_count;
    #ifdef DBG
        printf( "COPY[%x] S(%d) count = %d\n", this, s, ctor_count );
    #endif
}

S::~S()
{
    if( s > 100 ) {
        fail( __LINE__);	// DTOR S -- impossible initialization value
    }
    #ifdef DBG
        printf( "DTOR[%x] S(%d) count = %d\n", this, s, ctor_count );
    #endif
    --ctor_count;
}

S S::S_copy( int n, ... )
{
    return S(n);  
}


S S_ellip( int n ... )
{
    return S(n);
}

extern "C" S S_ell( int n, ... )
{
    return S(n);
}

struct B                    // Big struct ==> passed back in memory
{   int s;
    int c[5];
    B( int );
    ~B();
    B( const B& );
    B B_copy( int n, ... );
};

B::B( int sv ) : s(sv)
{
    if( s > 100 ) {
        fail( __LINE__);	// CTOR B -- impossible initialization value
    }
    c[0] = ++ctor_count;
    #ifdef DBG
        printf( "CTOR[%x] B(%d) count = %d\n", this, s, ctor_count );
    #endif
}

B::B( const B& src ) : s( src.s )
{
    if( s > 100 ) {
        fail( __LINE__);	// COPY B -- impossible initialization value
    }
    c[0] = ++ctor_count;
    #ifdef DBG
        printf( "COPY[%x] B(%d) count = %d\n", this, s, ctor_count );
    #endif
}

B::~B()
{
    if( s > 100 ) {
        fail( __LINE__);	// DTOR B -- impossible initialization value
    }
    if( c[0] != ctor_count ) {
        fail( __LINE__);	// DTOR B -- dtor order mismatch
    }
    #ifdef DBG
        printf( "DTOR[%x] B(%d) count = %d\n", this, s, ctor_count );
    #endif
    --ctor_count;
}

B B::B_copy( int n, ... )
{
    return B(n);  
}


B B_ellip( int n ... )
{
    return B(n);
}

extern "C" B B_ell( int n, ... )
{
    return B(n);
}

S S_gbl(9);
B B_gbl(13);

int main()
{
    S S_stat( 8 );
    S_ellip( 10 );
    S_gbl.S_copy( 11 );
    S_stat = S_ell( 12 );
    B B_stat( 14 );
    B_ellip( 15 );
    B_gbl.B_copy( 16 );
    B_ell( 17 );
    _PASS;
}
