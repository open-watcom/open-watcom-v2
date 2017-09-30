#include "fail.h"

struct L0 {
    virtual void f00( void ) {};
    virtual void f10( void ) {};
    virtual void f20( void ) {};
    virtual void f30( void ) {};
    virtual void f40( void ) {};
    virtual void f50( void ) {};
    virtual void f60( void ) {};
    virtual void f70( void ) {};
    virtual void f80( void ) {};
    virtual void f90( void ) {};
    int a[10];
};

struct L1 {
    virtual void f00( void ) {};
    virtual void f10( void ) {};
    virtual void f20( void ) {};
    virtual void f30( void ) {};
    virtual void f40( void ) {};
    virtual void f50( void ) {};
    virtual void f60( void ) {};
    virtual void f70( void ) {};
    virtual void f80( void ) {};
    virtual void f90( void ) {};
    virtual void f01( void ) {};
    virtual void f11( void ) {};
    virtual void f21( void ) {};
    virtual void f31( void ) {};
    virtual void f41( void ) {};
    virtual void f51( void ) {};
    virtual void f61( void ) {};
    virtual void f71( void ) {};
    virtual void f81( void ) {};
    virtual void f91( void ) {};
    int a[10];
};

struct L2 {
    virtual void f00( void ) {};
    virtual void f10( void ) {};
    virtual void f20( void ) {};
    virtual void f30( void ) {};
    virtual void f40( void ) {};
    virtual void f50( void ) {};
    virtual void f60( void ) {};
    virtual void f70( void ) {};
    virtual void f80( void ) {};
    virtual void f90( void ) {};
    virtual void f01( void ) {};
    virtual void f11( void ) {};
    virtual void f21( void ) {};
    virtual void f31( void ) {};
    virtual void f41( void ) {};
    virtual void f51( void ) {};
    virtual void f61( void ) {};
    virtual void f71( void ) {};
    virtual void f81( void ) {};
    virtual void f91( void ) {};
    virtual void f02( void ) {};
    virtual void f12( void ) {};
    virtual void f22( void ) {};
    virtual void f32( void ) {};
    virtual void f42( void ) {};
    virtual void f52( void ) {};
    virtual void f62( void ) {};
    virtual void f72( void ) {};
    virtual void f82( void ) {};
    virtual void f92( void ) {};
    int a[10];
};

struct L3 {
    virtual void f00( void ) {};
    virtual void f10( void ) {};
    virtual void f20( void ) {};
    virtual void f30( void ) {};
    virtual void f40( void ) {};
    virtual void f50( void ) {};
    virtual void f60( void ) {};
    virtual void f70( void ) {};
    virtual void f80( void ) {};
    virtual void f90( void ) {};
    virtual void f01( void ) {};
    virtual void f11( void ) {};
    virtual void f21( void ) {};
    virtual void f31( void ) {};
    virtual void f41( void ) {};
    virtual void f51( void ) {};
    virtual void f61( void ) {};
    virtual void f71( void ) {};
    virtual void f81( void ) {};
    virtual void f91( void ) {};
    virtual void f02( void ) {};
    virtual void f12( void ) {};
    virtual void f22( void ) {};
    virtual void f32( void ) {};
    virtual void f42( void ) {};
    virtual void f52( void ) {};
    virtual void f62( void ) {};
    virtual void f72( void ) {};
    virtual void f82( void ) {};
    virtual void f92( void ) {};
    virtual void f03( void ) {};
    virtual void f13( void ) {};
    virtual void f23( void ) {};
    virtual void f33( void ) {};
    virtual void f43( void ) {};
    virtual void f53( void ) {};
    virtual void f63( void ) {};
    virtual void f73( void ) {};
    virtual void f83( void ) {};
    virtual void f93( void ) {};
    int a[10];
};

struct B {
    int b0;
    int b1;
    int b2;
    int b3;
    virtual void set0( int x ) = 0;
    virtual void set1( int x ) = 0;
    virtual void set2( int x ) = 0;
    virtual void set3( int x ) = 0;
};
struct D0 : L0, B {
    virtual void set0( int x ) { if( b0 != -1 ) fail(__LINE__); b0 = x; }
    int d0[10];
};
struct D1 : L1, D0 {
    virtual void set1( int x ) { if( b1 != -2 ) fail(__LINE__); b1 = x; }
    int d1[10];
};
struct D2 : L2, D1 {
    virtual void set2( int x ) { if( b2 != -3 ) fail(__LINE__); b2 = x; }
    int d2[10];
};
struct D3 : L3, D2 {
    virtual void set3( int x ) { if( b3 != -4 ) fail(__LINE__); b3 = x; }
    int d3[10];
    virtual void f00( void ) {};
    virtual void f10( void ) {};
    virtual void f20( void ) {};
    virtual void f30( void ) {};
    virtual void f40( void ) {};
    virtual void f50( void ) {};
    virtual void f60( void ) {};
    virtual void f70( void ) {};
    virtual void f80( void ) {};
    virtual void f90( void ) {};
    virtual void f01( void ) {};
    virtual void f11( void ) {};
    virtual void f21( void ) {};
    virtual void f31( void ) {};
    virtual void f41( void ) {};
    virtual void f51( void ) {};
    virtual void f61( void ) {};
    virtual void f71( void ) {};
    virtual void f81( void ) {};
    virtual void f91( void ) {};
    virtual void f02( void ) {};
    virtual void f12( void ) {};
    virtual void f22( void ) {};
    virtual void f32( void ) {};
    virtual void f42( void ) {};
    virtual void f52( void ) {};
    virtual void f62( void ) {};
    virtual void f72( void ) {};
    virtual void f82( void ) {};
    virtual void f92( void ) {};
    virtual void f03( void ) {};
    virtual void f13( void ) {};
    virtual void f23( void ) {};
    virtual void f33( void ) {};
    virtual void f43( void ) {};
    virtual void f53( void ) {};
    virtual void f63( void ) {};
    virtual void f73( void ) {};
    virtual void f83( void ) {};
    virtual void f93( void ) {};
};

void doit( B *p )
{
    if( p->b0 != -1 ) fail(__LINE__);
    p->set0( 1 );
    if( p->b0 != 1 ) fail(__LINE__);
    if( p->b1 != -2 ) fail(__LINE__);
    p->set1( 2 );
    if( p->b1 != 2 ) fail(__LINE__);
    if( p->b2 != -3 ) fail(__LINE__);
    p->set2( 3 );
    if( p->b2 != 3 ) fail(__LINE__);
    if( p->b3 != -4 ) fail(__LINE__);
    p->set3( 4 );
    if( p->b3 != 4 ) fail(__LINE__);
}

int main()
{
    D3 qq;

    qq.b0 = -1;
    qq.b1 = -2;
    qq.b2 = -3;
    qq.b3 = -4;
    doit( &qq );
    _PASS;
}
