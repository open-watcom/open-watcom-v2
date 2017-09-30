#include "fail.h"


struct A {
};

template< class T >
class B {
public:
    B( ) {}
    B( int ii ) : i( ii ) {}

    template< class U >
    friend A& operator<<( A&, const B< U > & );

    template< class U >
    friend int fnb( B<U> );

    template< class U >
    friend int fnb2( U );

    friend int fnb3( int );

private:
    int i;
};

template< class T >
A& operator << ( A& a, const B<T> & ) {
    return( a );
}

template< class T >
int fnb( B< T > b )
{
    return( b.i );
}

template< class T >
int fnb2( T i )
{
    B< T > b( 3 );
    return( b.i+i );
}

int fnb3( int i )
{
    B< int > b( 7 );
    return( b.i+i );
}

B<char> fn( void )
{ 
    return( B<char>() );
}


struct C {
    struct D {
        friend class C;
    };
};


int main()
{
    A out;
    B<int> b(3);
    
    if( fnb( b ) != 3 ) fail(__LINE__);
    if( fnb2( 2 ) != 5 ) fail(__LINE__);
    if( fnb3( 3 ) != 10 ) fail(__LINE__);
    out << b;
    out << fn();
    
    _PASS;
}
