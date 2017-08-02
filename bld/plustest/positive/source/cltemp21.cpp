#include "fail.h"

// updated template class explicit instatiation syntax
// definitions in cltmp21b.c
template< class T >
class X{
public:
    void fn( T x );
    T v;
};

template< class T >
class Y{
public:
    T fnb( T x );
};

// more explicit template type stuff can go here once it is implemented...
// template function
// template member function

int main() {
    X< long > x;
    Y< double > y;
    x.fn( 3 );
    if( y.fnb( 0.5 + x.v ) != 30.25 ) fail( __LINE__ );
    _PASS;
}
