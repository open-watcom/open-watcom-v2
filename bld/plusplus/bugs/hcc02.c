/* MetaWare doesn't initialize the virtual bases in the correct order */
struct V1 {
    int v1;
    V1();
    virtual void set_v1( int );
};
V1::V1()
{
    v1 = 0x0100 + 1;
    set_v1( 0x0100 + 2 );
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
    v2 = 0x0200 + 3;
    set_v2( 0x0200 + 4 );
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
    v1 = 0x0100 + 5;
    set_v1( 0x0100 + 6 );
    v2 = 0x0200 + 7;
    set_v2( 0x0200 + 8 );
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
    v1 = 0x0100 + 9;
    set_v1( 0x0100 + 10 );
    v2 = 0x0200 + 11;
    set_v2( 0x0200 + 12 );
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
    virtual void set_v1( int );
    virtual void set_v2( int );
};
C::C()
{
    v1 = 0x0100 + 13;
    set_v1( 0x0100 + 14 );
    v2 = 0x0200 + 15;
    set_v2( 0x0200 + 16 );
}
void C::set_v1( int x )
{
    v1 = x;
}
void C::set_v2( int x )
{
    v2 = x;
}

void main( void )
{
    static C x;
}
