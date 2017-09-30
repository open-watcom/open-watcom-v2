#include "fail.h"

template< class T >
struct A;

template< class T >
struct B {
    A< B< T > > operator +( const T & ) const;
    template< class U > friend int operator +( const U &, const B< U > &);
};

template< class T >
struct A
    : public T
{ };

template< class T > B< T > &operator +( B< T > &b, const T &t) {
    return b;
}

template< class T > A< B< T > > operator +(const B< T > &, const B< T > &) {
    return A< B< T > >();
}

template< class T > A< B< T > > B< T >::operator +( const T &t ) const {
    return A< B< T > >();
}

int main() {
   B< int > b;
   B< int > r = b + 3;

   _PASS;
}
