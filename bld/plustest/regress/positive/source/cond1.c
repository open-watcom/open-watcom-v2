#include "fail.h"

int Ac = 0, Ad = 0;
int Bc = 0, Bd = 0;

class A {
public:
    A( bool b )
        : b ( b )
    { Ac++; }

    operator bool() { return b; }

    ~A() { Ad++; }

private:
    bool b;
};

class B {
public:
    B( int i )
        : i ( i )
    { Bc++; }

    operator int() { return i; }

    ~B() { Bd++; }

private:
    int i;
};

int main()
{
    for( int i = 0; i < 2; i++ ) {
        if( A a = ( i == 0 ) ) {
            bool b = a;
            if( ! b ) fail( __LINE__ );
            if( i != 0 ) fail( __LINE__ );
            if( Ac != ( Ad + 1 ) ) fail( __LINE__ );
        } else {
            bool b = a;
            if( b ) fail( __LINE__ );
            if( i == 0 ) fail( __LINE__ );
            if( Ac != ( Ad + 1 ) ) fail( __LINE__ );
        }
        if( ! ( Ac == ( 2*i + 1 ) && Ad == ( 2*i + 1) ) ) fail( __LINE__ );

        if( A a = ( i != 0 ) ) {
            bool b = a;
            if( ! b ) fail( __LINE__ );
            if( i == 0 ) fail( __LINE__ );
            if( Ac != ( Ad + 1 ) ) fail( __LINE__ );
        } else {
            bool b = a;
            if( b ) fail( __LINE__ );
            if( i != 0 ) fail( __LINE__ );
            if( Ac != ( Ad + 1 ) ) fail( __LINE__ );
        }
        if( ! ( Ac == ( 2*i + 2 ) && Ad == ( 2*i + 2) ) ) fail( __LINE__ );
        int a = 0;

        switch( B b = i ) {
        case 0:
            if( b != 0 ) fail( __LINE__ );
            if( Bc != ( Bd + 1 ) ) fail( __LINE__ );
            break;

        case 1:
            if( b != 1 ) fail( __LINE__ );
            if( Bc != ( Bd + 1 ) ) fail( __LINE__ );
            break;
        }
        int b = 0;

        if( ! ( Bc == ( i + 1 ) && Bd == (i + 1 ) ) ) fail( __LINE__ );
    }

    _PASS;
}
