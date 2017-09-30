struct VV {
    int v;
};

struct MM : private virtual VV {
    VV::v;
};

struct BB : private MM, private virtual VV {
    VV::v;
};

void ok( BB *p )
{
    p->v = 1;
}

void not_ok( BB *p )
{
    p->MM::v = 1;
}

struct S {
    int & foo( int, int = 1 );
    int const & foo( int, int = 2 ) const;
};

struct V {
    V();
    V(int);
};
struct X : virtual V {
    X();
};
X::X() : V('x')
{
}
struct Y : virtual V {
    Y();
};
Y::Y() : V('y')
{
}
struct Z : X, Y {
    Z();
};
Z::Z() : V('z')
{
}

struct A {
    int a;
};

struct B : private A {
    A::a;
};

struct D : private B {
    B::a;
};

void foo( D *p )
{
    p->a = 1;
}

struct UDC_B {
    virtual operator int();
    virtual operator double();
};

struct UDC_D : private UDC_B {
    UDC_B::operator int;
};

double explicit1( UDC_D *p )
{
    return p->operator double();
}

int explicit2( UDC_D *p )
{
    return p->operator int();
}

double implicit1( UDC_D *p )
{
    return *p;
}

int implicit2( UDC_D *p )
{
    return *p;
}

struct UDC_CV {
    operator int ();
    operator int () const;
    int s;
};

UDC_CV *p;
UDC_CV const *q;

int cv_foo( void )
{
    return(*p);
}

int cv_bar( void )
{
    return(*q);
}

void cv_sam( void )
{
    p->operator int();
    q->operator int();
}
