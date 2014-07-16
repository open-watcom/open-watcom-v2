#include "fail.h"

enum E { E0 = 0 };

struct B
{
    bool operator +( int ) {
        return true;
    }

    bool operator +( int ) const {
        return false;
    }

    operator bool() {
        return true;
    }

    operator bool() const {
        return false;
    }

    bool f() const {
        return !*this;
    }

    bool g() const {
        void *ptr = 0;
        return h( ptr );
    }

    bool h( void * ) {
        return false;
    }

    bool h( const void * ) const {
        return true;
    }
};

bool operator -( B &, int ) {
    return true;
}

bool operator -( const B &, int ) {
    return false;
}


bool f( E & ) {
    return true;
}

bool f( const E & ) {
    return false;
}


bool f( int & ) {
    return true;
}

bool f( const int & ) {
    return false;
}


bool f( B & ) {
    return true;
}

bool f( const B & ) {
    return false;
}


E e() {
    E e = E0;
    return e;
}

B b() {
    B b;
    return b;
}


int main()
{
    E e1 = E0;
    const E e2 = E0;

    if( f( e1 ) != true ) fail( __LINE__ );
    if( f( e2 ) != false ) fail( __LINE__ );
    if( f( E0 ) != false ) fail( __LINE__ );
    if( f( e() ) != false ) fail( __LINE__ );


    int i1 = 0;
    const int i2 = 0;

    if( f( i1 ) != true ) fail( __LINE__ );
    if( f( i2 ) != false ) fail( __LINE__ );
    if( f( 0 ) != false ) fail( __LINE__ );


    B b1;
    const B b2 = b1;

    if( f( b1 ) != true ) fail( __LINE__ );
    if( f( b2 ) != false ) fail( __LINE__ );
    if( f( B() ) != false ) fail( __LINE__ );
    if( f( b() ) != false ) fail( __LINE__ );


    if( b1 + 0 != true ) fail( __LINE__ );
    if( b2 + 0 != false ) fail( __LINE__ );
    if( b() + 0 != true ) fail( __LINE__ );
    if( B() + 0 != true ) fail( __LINE__ );

    if( b1 - 0 != true ) fail( __LINE__ );
    if( b2 - 0 != false ) fail( __LINE__ );
    if( b() - 0 != false ) fail( __LINE__ );
    if( B() - 0 != false ) fail( __LINE__ );

    if( b1 != true ) fail( __LINE__ );
    if( b2 != false ) fail( __LINE__ );
    if( b() != true ) fail( __LINE__ );
    if( B() != true ) fail( __LINE__ );

    if( b2.f() != true ) fail( __LINE__ );
    if( b2.g() != true ) fail( __LINE__ );


    _PASS;
}
