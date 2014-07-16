#include "fail.h"

// TSTSCOP2.C -- TEST STATIC AND ENUM VARIABLES SHARED IN CLASSES
//
// 91/10/25	-- Ian McHardy		-- defined

//		    A
//		   / \
//		  B   C
//		  |  /|
//		  \ / |
//		   D /
//		    E
	
struct A
{
    static int a;
    enum{ enum1, enum2, enum3 };
};

int A::a = __LINE__;

class B: public A{};

class C: public A{};

class D: public B, public virtual C{};

class E: public D, public virtual C{};

int main( void )
{
    E e;
    int j;

    e.a = 1;
    j = e.enum2;

    if( e.B::a != 1 ) fail(__LINE__);
    if( e.C::a != 1 ) fail(__LINE__);
    if( e.a != 1 ) fail(__LINE__);
    if( e.B::enum1 != 0 ) fail(__LINE__);
    if( e.C::enum2 != 1 ) fail(__LINE__);
    if( e.D::enum3 != 2 ) fail(__LINE__);
    _PASS;
};
     
