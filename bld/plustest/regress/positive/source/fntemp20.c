#include "fail.h"

struct A
{ };

struct B
    : public A
{ };


template< class T >
int f( const T * const * a )
{
    return 1;
}


template< class T >
int f1( T a, T b )
{
    return 1;
}

template< class T >
int f2( const T * const *a, const T * const *b )
{
    return 1;
}


template< class T >
int f2( const T **a, const T **b )
{
    return 2;
}


template< class T >
int f3( const T &a, const A &b )
{
    return 1;
}

template< class T >
int f3( const T &a, const T &b )
{
    return 2;
}


template< class T >
struct C
{ };

template< class T >
struct D
{
    D()
        : val( 0 )
    { }

    template< class U >
    D( const D< U > & )
        : val( 1 )
    { }

    D( const D< int > & )
        : val( 2 )
    { }

    template< class U >
    D( const C< U > & )
        : val( 3 )
    { }


    int val;
};


struct E
{
    E()
        : val( 0 )
    { }

    template< class T >
    E( const T * const * )
        : val( 1 )
    { }

    explicit E( const char * const * )
        : val( 2 )
    { }

    int val;
};


template< class T >
void g( C< T > & )
{ }


int main()
{
    char **c = 0;

    if( f( c ) != 1 ) fail( __LINE__ );
    if( f( (char **) c ) != 1 ) fail( __LINE__ );
    if( f( (char * const *) c ) != 1 ) fail( __LINE__ );
    if( f( (const char **) c ) != 1 ) fail( __LINE__ );
    if( f( (const char * const *) c ) != 1 ) fail( __LINE__ );


    int *p = 0;

    if( f1( p, (int * const) p ) != 1 ) fail( __LINE__ );
    if( f1( p, p ) != 1 ) fail( __LINE__ );
    if( f1( (int * const) p, p ) != 1 ) fail( __LINE__ );
    if( f1( 0, 0 ) != 1 ) fail( __LINE__ );


    int * const *pp = 0;
    const int * const *qq = 0;
    const int **rr = 0;

    if( f2( pp, pp ) != 1 ) fail( __LINE__ );
    if( f2( qq, qq ) != 1 ) fail( __LINE__ );
    if( f2( rr, rr ) != 2 ) fail( __LINE__ );

    if( f3( A(), B()) != 1 ) fail( __LINE__ );
    if( f3( B(), A()) != 1 ) fail( __LINE__ );
    if( f3( B(), B()) != 2 ) fail( __LINE__ );


    D< int > d1;
    if( d1.val != 0 ) fail( __LINE__ );

    D< long > d2( d1 );
    if( d2.val != 2 ) fail( __LINE__ );

    D< short > d3;
    if( d3.val != 0 ) fail( __LINE__ );

    d3 = d1;
    if( d3.val != 2 ) fail( __LINE__ );

    d3 = d2;
    if( d3.val != 1 ) fail( __LINE__ );


    E e1;
    if( e1.val != 0 ) fail( __LINE__ );
    
    E e2( c );
    if( e2.val != 2 ) fail( __LINE__ );

    e2 = c;
    if( e2.val != 1 ) fail( __LINE__ );


    C< char > cc;
    void ( *gp1 ) ( C< char > & ) = g;
    void ( *gp2 ) ( C< int > & ) = g;

    if( reinterpret_cast< void * >( gp1 ) == reinterpret_cast< void * >( gp2 ) ) fail( __LINE__ );


    _PASS;
}
