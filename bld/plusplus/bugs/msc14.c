/*
    This program reveals a fundamental flaw with the MS C7 ctor-disp method
    as described in the Microsoft Object Model document.  The Object Model
    can be fixed by describing the resolution to this case.

    If we have the following hierarchy:

   (bad_v1) V1\\
            ||  \\
            ||   \\
   (bad_v1) A \\  ||
            || || ||
            || || ||
            C  || ||
         R  || || ||
          \ ||// //
           \||///
             D

    with a virtual function 'bad_v1' originating in 'V1' overridden in 'A'
    but not in 'C' or 'D', a problem occurs when 'bad_v1' is called in the
    context of 'V1'.  The problem is that the MS C7 ctor-disp only gets one
    to the position where it "would be" during exact construction.  If the
    function was overridden in another virtual base, one must be able to
    get to the new position.  The MS C7 doc doesn't describe this problem.

    The solution is to use the exact class' virtual base table (it must have
    one!) to re-adjust the 'this' value back up the derivation tree.
    Fortunately, WATCOM C++ already handles this case!

    MSC7 and MetaWare fail to execute this program properly.
*/
extern "C" int printf( char *, ... );
int errors;
static void verify( int old, int n, int line )
{
    --n;
    if( n != old ) {
        printf( "error on line %u (%x should be %x)\n", line, old, n );
        ++errors;
    }
}
struct V1 {
    int v1;
    V1();
    virtual void set_v1( int );
    virtual void bad_v1( int );
};
V1::V1()
{
    V1 *pv = this;
    verify( pv->v1, 1, __LINE__ );
    pv->v1 = 0x1100 + 1;
    pv->set_v1( 0x1100 + 2 );
    pv->bad_v1( 0x1100 + 3 );
}
void V1::set_v1( int x )
{
    verify( v1 , x, __LINE__ );
    v1 = x;
}
void V1::bad_v1( int x )
{
    verify( v1 , x, __LINE__ );
    v1 = x;
}
struct A : virtual V1 {
    int a;
    A();
    virtual void set_v1( int );
    virtual void bad_v1( int );
};
A::A()
{
    V1 *pv = this;
    verify( pv->v1, 0x1100 + 4, __LINE__ );
    pv->v1 = 0x1100 + 4;
    pv->set_v1( 0x1100 + 5 );
    pv->bad_v1( 0x1100 + 6 );
}
void A::set_v1( int x )
{
    verify( v1 , x, __LINE__ );
    v1 = x;
}
void A::bad_v1( int x )
{
    verify( v1 , x, __LINE__ );
    v1 = x;
}
struct P {
    int p;
};
struct C : P, virtual A {
    int c;
    C();
    virtual void set_v1( int );
};
C::C()
{
    V1 *pv = this;
    verify( pv->v1, 0x1100 + 7, __LINE__ );
    pv->v1 = 0x1100 + 7;
    pv->set_v1( 0x1100 + 8 );
    pv->bad_v1( 0x1100 + 9 );

    A *pa = this;
    verify( pa->v1, 0x1100 + 10, __LINE__ );
    pa->v1 = 0x1100 + 10;
    pa->set_v1( 0x1100 + 11 );
    pa->bad_v1( 0x1100 + 12 );
}
void C::set_v1( int x )
{
    verify( v1 , x, __LINE__ );
    v1 = x;
}
struct R {
    int : 0;
};
struct D : R, virtual A, virtual V1, virtual C {
    D();
    virtual void set_v1( int );
};
D::D()
{
    V1 *pv = this;
    verify( pv->v1, 0x1100 + 13, __LINE__ );
    pv->v1 = 0x1100 + 13;
    pv->set_v1( 0x1100 + 14 );
    pv->bad_v1( 0x1100 + 15 );

    A *pa = this;
    verify( pa->v1, 0x1100 + 16, __LINE__ );
    pa->v1 = 0x1100 + 16;
    pa->set_v1( 0x1100 + 17 );
    pa->bad_v1( 0x1100 + 18 );

    C *pc = this;
    verify( pc->v1, 0x1100 + 19, __LINE__ );
    pc->v1 = 0x1100 + 19;
    pc->set_v1( 0x1100 + 20 );
    pc->bad_v1( 0x1100 + 21 );
}
void D::set_v1( int x )
{
    verify( v1 , x, __LINE__ );
    v1 = x;
}

void main( void )
{
    static D x;
    if( errors == 0 ) {
        printf( "PASS\n" );
    } else {
        printf( "FAIL: %u error(s)\n", errors );
    }
}
