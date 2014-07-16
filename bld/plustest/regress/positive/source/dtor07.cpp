#include "fail.h"
// Test that thunks and DTOR calls from CGBKUTIL handle reversed parms

void* a_a;
void* a_d;
void* a_e;

void* c_a;
void* c_d;
void* c_e;

void* d_a;
void* d_d;
void* d_e;

#define WEXP __pascal
struct A {
    virtual WEXP ~A();
    WEXP A();
};

WEXP A::A()
{
    c_a = this;
}

WEXP A::~A()
{
    d_a = this;
}

struct D {
    virtual WEXP ~D();
    WEXP D();
};

WEXP D::D()
{
    c_d = this;
}

WEXP D::~D()
{
    d_d = this;
}


struct E : A, D {
     WEXP ~E();
     WEXP E();
};

WEXP E::E()
{
    c_e = this;
}

WEXP E::~E()
{
    d_e = this;
}

#define chk( a, b )             \
    if( a != b ) {              \
        printf( "*** FAILED " #a " != " #b " %x %x\n", a, b );  \
	fail(__LINE__); \
    }

int main()
{
    {
        E e;
        a_e = &e;
        a_a = (A*)&e;
        a_d = (D*)&e;
    }
    chk( c_a, d_a );
    chk( c_d, d_d );
    chk( c_e, d_e );
    chk( a_a, d_a );
    chk( a_d, d_d );
    chk( a_e, d_e );
    _PASS;
}
