#include "fail.h"

struct __declspec( __cdecl ) A;

struct A {
    A()
        : a( 0 )
    { }

    int f( int i ) {
        a++;
        return i + a;
    }

    int a;
};


int main() {
    int __declspec( __cdecl ) ( A::*g )( int ) = &A::f;

    A a;

    if( a.f( 2 ) != 3 ) fail( __LINE__ );
    if( ( a.*g )( 3 ) != 5 ) fail( __LINE__ );
    if( ( (&a)->*g )( 4 ) != 7 ) fail( __LINE__ );


    _PASS;
}


struct __declspec(dllexport) B { };

template< class T > struct C : public B { };

struct D : public C< char > { };

template class  __declspec(dllexport) C< char >;
