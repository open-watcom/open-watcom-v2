// test results of reinterpret_cast

#include "fail.h"

#ifndef __WATCOM_RTTI__

ALWAYS_PASS

#else

struct B1 {
    int b1;
};

struct B2 {
    int b2;
};

struct D : public B1, public B2 {
    int d;
};

D d;

B2* cast_ptr( D* dp )
{
    return reinterpret_cast<B2*>( dp );
}

B2& cast_ref( D& dp )
{
    return reinterpret_cast<B2&>( dp );
}

int B1::* cast_mptr( int B2::* mp )
{
    return reinterpret_cast<int B1::*>( mp );
}

int main()
{
    d.b1 = 1;
    d.b2 = 2;
    d.d = 3;
    B2* b2p = cast_ptr( &d );
    B2& b2r = cast_ref(  d );
    int B1::* dm = cast_mptr( &B2::b2 );
    if( (void*)b2p != (void*)&d ) fail( __LINE__ );
    if( (void*)&b2r != (void*)&d ) fail( __LINE__ );
    if( 1 != d.*dm ) fail( __LINE__ );
    _PASS
}

#endif
