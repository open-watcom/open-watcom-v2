#include "fail.h"

template< class T >
struct A
  : public T
{ };

template< class T >
struct B
{
    int f( A< B< T > > ) {
        return 0;
    }

    A< B< T > > *a1;
};



template< class T >
inline void g( T &a )
{ }

template< class T >
struct C {
    enum E { n };
    struct D {
        E e;
        T t;
    };

    ~C() {
        f();
    }

    void f();
};

template< class T >
void C< T >::f() {
    D d;
    D *dp = &d;
    dp->t.~T();
    g( dp->e );
}


int main() {
    B< int > b;

    A< B< int > > a;
    if( b.f( a ) != 0) fail( __LINE__ );

    C< C< int > > c;

    _PASS;
}
