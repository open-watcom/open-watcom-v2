#include "fail.h"
/*
   MSC7 doesn't execute this program correctly.  MetaWare screws up the
   order of construction for the virtual bases but if the program is
   changed to compensate for this, MetaWare executes it properly.
*/
/*
	    V1 V2  V2 V1
	    || ||  || ||
	    \\ //  \\ //
	      A	     B
	       \\   //
	        \\ //
		  C
		  ||
		  D

    D should construct in the order V1, V2, A, B, and C.
*/
static void verify( int old, int n, int line )
{
    --n;
    if( n != old ) {
	fail(line);
    }
}
struct V1 {
    int v1;
    V1();
    virtual void set_v1( int );
};
V1::V1()
{
    v1 = 0x1100 + 1;
    set_v1( 0x1100 + 2 );
}
void V1::set_v1( int x )
{
    verify( v1 , x, __LINE__ );
    v1 = x;
}
struct V2 {
    int v2;
    V2();
    virtual void set_v2( int );
};
V2::V2()
{
    v2 = 0x2200 + 1;
    set_v2( 0x2200 + 2 );
}
void V2::set_v2( int x )
{
    verify( v2 , x, __LINE__ );
    v2 = x;
}
struct A : virtual V1, virtual V2 {
    int a;
    A();
    virtual void set_v1( int );
    virtual void set_v2( int );
};
A::A()
{
    verify( v1 , 0x1100 + 3, __LINE__ );
    v1 = 0x1100 + 3;
    set_v1( 0x1100 + 4 );
    verify( v2 , 0x2200 + 3, __LINE__ );
    v2 = 0x2200 + 3;
    set_v2( 0x2200 + 4 );
}
void A::set_v1( int x )
{
    verify( v1 , x, __LINE__ );
    v1 = x;
}
void A::set_v2( int x )
{
    verify( v2 , x, __LINE__ );
    v2 = x;
}
struct B : virtual V2, virtual V1 {
    int b;
    B();
    virtual void set_v1( int );
    virtual void set_v2( int );
};
B::B()
{
    verify( v1 , 0x1100 + 5, __LINE__ );
    v1 = 0x1100 + 5;
    set_v1( 0x1100 + 6 );
    verify( v2 , 0x2200 + 5, __LINE__ );
    v2 = 0x2200 + 5;
    set_v2( 0x2200 + 6 );
}
void B::set_v1( int x )
{
    verify( v1 , x, __LINE__ );
    v1 = x;
}
void B::set_v2( int x )
{
    verify( v2 , x, __LINE__ );
    v2 = x;
}
struct P {
    int p;
};
struct C : P, virtual A, virtual B {
    int c;
    C();
    virtual void set_v1( int );
    virtual void set_v2( int );
};
C::C()
{
    verify( v1 , 0x1100 + 7, __LINE__ );
    v1 = 0x1100 + 7;
    set_v1( 0x1100 + 8 );
    verify( v2 , 0x2200 + 7, __LINE__ );
    v2 = 0x2200 + 7;
    set_v2( 0x2200 + 8 );

    A *a = this;
    verify( v1 , 0x1100 + 9, __LINE__ );
    v1 = 0x1100 + 9;
    a->set_v1( 0x1100 + 10 );
    verify( v2 , 0x2200 + 9, __LINE__ );
    v2 = 0x2200 + 9;
    a->set_v2( 0x2200 + 10 );

    B *b = this;
    verify( v1 , 0x1100 + 11, __LINE__ );
    v1 = 0x1100 + 11;
    b->set_v1( 0x1100 + 12 );
    verify( v2 , 0x2200 + 11, __LINE__ );
    v2 = 0x2200 + 11;
    b->set_v2( 0x2200 + 12 );
}
void C::set_v1( int x )
{
    verify( v1 , x, __LINE__ );
    v1 = x;
}
void C::set_v2( int x )
{
    verify( v2 , x, __LINE__ );
    v2 = x;
}
struct R {
    int : 0;
};
struct D : R, virtual C {
    D();
    virtual void set_v1( int );
    virtual void set_v2( int );
};
D::D()
{
    verify( v1 , 0x1100 + 13, __LINE__ );
    v1 = 0x1100 + 13;
    set_v1( 0x1100 + 14 );
    verify( v2 , 0x2200 + 13, __LINE__ );
    v2 = 0x2200 + 13;
    set_v2( 0x2200 + 14 );

    A *a = this;
    verify( v1 , 0x1100 + 15, __LINE__ );
    v1 = 0x1100 + 15;
    a->set_v1( 0x1100 + 16 );
    verify( v2 , 0x2200 + 15, __LINE__ );
    v2 = 0x2200 + 15;
    a->set_v2( 0x2200 + 16 );

    B *b = this;
    verify( v1 , 0x1100 + 17, __LINE__ );
    v1 = 0x1100 + 17;
    b->set_v1( 0x1100 + 18 );
    verify( v2 , 0x2200 + 17, __LINE__ );
    v2 = 0x2200 + 17;
    b->set_v2( 0x2200 + 18 );
}
void D::set_v1( int x )
{
    verify( v1 , x, __LINE__ );
    v1 = x;
}
void D::set_v2( int x )
{
    verify( v2 , x, __LINE__ );
    v2 = x;
}

int main( void )
{
    static D x;
    _PASS;
}
