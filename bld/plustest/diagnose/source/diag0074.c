/* there are two final possibilities for set_v1 in C A:: or B:: */
struct V1 {
    int v1;
    V1();
    virtual void set_v1( int );
};
V1::V1()
{
    v1 = 1;
    set_v1( 2 );
}
void V1::set_v1( int x )
{
    v1 = x;
}
struct V2 {
    int v2;
    V2();
    virtual void set_v2( int );
};
V2::V2()
{
    v2 = 3;
    set_v2( 4 );
}
void V2::set_v2( int x )
{
    v2 = x;
}
struct A : virtual V1, virtual V2 {
    A();
    virtual void set_v1( int );
    virtual void set_v2( int );
};
A::A()
{
    v1 = 4;
    set_v1( 5 );
    v2 = 6;
    set_v2( 7 );
}
void A::set_v1( int x )
{
    v1 = x;
}
void A::set_v2( int x )
{
    v2 = x;
}
struct B : virtual V2, virtual V1 {
    B();
    virtual void set_v1( int );
    virtual void set_v2( int );
};
B::B()
{
    v1 = 8;
    set_v1( 9 );
    v2 = 10;
    set_v2( 11 );
}
void B::set_v1( int x )
{
    v1 = x;
}
void B::set_v2( int x )
{
    v2 = x;
}
struct C : A, B {
    C();
};
C::C()
{
}
/* double letter version is OK */
struct VV1 {
    int vv1;
    VV1();
    virtual void set_vv1( int );
};
VV1::VV1()
{
}
void VV1::set_vv1( int )
{
}
struct VV2 {
    int vv2;
    VV2();
    virtual void set_vv2( int );
};
VV2::VV2()
{
}
void VV2::set_vv2( int )
{
}
struct AA : virtual VV1, virtual VV2 {
    AA();
};
AA::AA()
{
}
struct BB : virtual VV2, virtual VV1 {
    BB();
    virtual void set_vv1( int );
    virtual void set_vv2( int );
};
BB::BB()
{
}
void BB::set_vv1( int )
{
}
void BB::set_vv2( int )
{
}
struct CC : AA, BB {
    CC();
};
CC::CC()
{
}

void main( void )
{
    static C x;
    static CC y;
}

struct OK_TO_NEST1 {
    OK_TO_NEST1::OK_TO_NEST1();
    OK_TO_NEST1::~OK_TO_NEST1();
    typedef int OK_TO_NEST1::T;
    int OK_TO_NEST1::a;
    int OK_TO_NEST1::fun();
};
OK_TO_NEST1::~OK_TO_NEST1()
{
}

struct OK_TO_NEST2 {
    class OK_TO_NEST_INSIDE {
	OK_TO_NEST2::OK_TO_NEST_INSIDE::OK_TO_NEST_INSIDE();
	OK_TO_NEST2::OK_TO_NEST_INSIDE::OK_TO_NEST_INSIDE(int);
	OK_TO_NEST2::OK_TO_NEST_INSIDE::~OK_TO_NEST_INSIDE();
	typedef int OK_TO_NEST2::OK_TO_NEST_INSIDE::T;
	int OK_TO_NEST2::OK_TO_NEST_INSIDE::a;
	int OK_TO_NEST2::OK_TO_NEST_INSIDE::foo();
    };
};
OK_TO_NEST2::OK_TO_NEST_INSIDE::OK_TO_NEST_INSIDE(int x)
{
}
struct OK1 {
    typedef struct N {} T;
    OK1( T );
};
OK1::OK1( T f )
{
    f=f;
}
struct OK2 {
    typedef int T;
    OK2( T );
    int OK2;
};
OK2::OK2( T x )
{
    x++;
}
struct BAD1 {
    int T;
    BAD1();
};
BAD1::BAD1( T )
{
};
struct BAD2 {
    int T;
    BAD2();
    int BAD2;
};
BAD2::BAD2( T )
{
}
