#include "fail.h"

/* this was a bug where a member template function would not be
 * instatiated (so linker failed) if the referencing function 
 * was also a member template function
 */

template< class S >
struct D{
    template< class T >
    T fd( T t ){ return t+1; }
};

template< class S >
struct C{
    template< class T >
    T fc( T t ){ D<int> d; return d.fd( t*2 ); }
};

struct B{
    template < class T >
    B( T t ){ x = t; }
    
    B(){};
    
    template< class T >
    T fb( T t )
        { C<char> c; return c.fc( t + x ); }
    //    { return t; }
    
    int x;
};

template< class S >
struct A : S{
    template< class T >
    A( T t ) : S( t ) {}
    
    template< class T >
    T fa( T t ){ return fb( t ); }
};

int main( void )
{
    A<B> a( 2 );
    
    if( a.fa( 3 ) != 11 ) fail( __LINE__ );
    
    _PASS
};




