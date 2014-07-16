#include "fail.h"

int count;

struct C {
    C(int);
    ~C();
};

C::C(int) {
    ++count;
}
C::~C() {
    --count;
    if( count < 0 ) fail(__LINE__);
}

int o1( int v1, int v2 )
{
    return (C(v1),v1)||(C(v2),v2);
}
int o2( int v1, int v2 )
{
    return (C(v1),v1)||(v2);
}
int o3( int v1, int v2 )
{
    return (v1)||(C(v2),v2);
}

int a1( int v1, int v2 )
{
    return (C(v1),v1)&&(C(v2),v2);
}
int a2( int v1, int v2 )
{
    return (C(v1),v1)&&(v2);
}
int a3( int v1, int v2 )
{
    return (v1)&&(C(v2),v2);
}

int main()
{
    if( o1( 0, 0 ) != 0 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( o1( 1, 0 ) != 1 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( o1( 0, 1 ) != 1 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( o1( 1, 1 ) != 1 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( o2( 0, 0 ) != 0 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( o2( 1, 0 ) != 1 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( o2( 0, 1 ) != 1 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( o2( 1, 1 ) != 1 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( o3( 0, 0 ) != 0 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( o3( 1, 0 ) != 1 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( o3( 0, 1 ) != 1 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( o3( 1, 1 ) != 1 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( a1( 0, 0 ) != 0 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( a1( 1, 0 ) != 0 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( a1( 0, 1 ) != 0 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( a1( 1, 1 ) != 1 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( a2( 0, 0 ) != 0 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( a2( 1, 0 ) != 0 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( a2( 0, 1 ) != 0 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( a2( 1, 1 ) != 1 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( a3( 0, 0 ) != 0 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( a3( 1, 0 ) != 0 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( a3( 0, 1 ) != 0 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    if( a3( 1, 1 ) != 1 ) fail(__LINE__);
    if( count != 0 ) { count = 0; fail(__LINE__); }
    _PASS;
}
