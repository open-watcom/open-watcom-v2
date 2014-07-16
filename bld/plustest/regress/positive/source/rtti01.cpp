// test const_cast

#include "fail.h"

#ifdef __WATCOM_RTTI__

int val = 963;

extern int const * p = & val;
int* p1 = const_cast<int*>( p );
int const *p2 = const_cast<int const*>( p );
int const volatile *p3 = const_cast<int const volatile*>( p );
int volatile *p4 = const_cast<int volatile*>( p );

extern int const & r = val;
int& r1 = const_cast<int&>( r );
int const &r2 = const_cast<int const&>( r );
int const volatile &r3 = const_cast<int const volatile&>( r );
int volatile &r4 = const_cast<int volatile&>( r );

void check_ptr( int const volatile * p )
{
    if( *p != 963 ) fail( __LINE__ );
}

void check_int( int v )
{
    if( v != 963 ) fail( __LINE__ );
}

int main()
{
    check_ptr( p  );
    check_ptr( p1 );
    check_ptr( p2 );
    check_ptr( p3 );
    check_int( r  );
    check_int( r1 );
    check_int( r2 );
    check_int( r3 );
    _PASS;
}

#else

ALWAYS_PASS

#endif

