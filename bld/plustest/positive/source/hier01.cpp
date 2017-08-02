#include "fail.h"

// TESTSCOP.C -- TEST SCOPING WITHIN CLASSES
//
// 91/10/24	-- Ian McHardy		-- defined

// here is the class structure built and tested:
//            A   B	init: A( a=1,b=2 ), B( c=5 )
//             \ / \\
//	        |   D \	init: D( d=6 )
//	         \ /\ |
//                C / /	init: C( d=8,f=9 )
//           A   / / D	init: A( a=3,b=4 ), D( d=7 )
//	      \	 |/ /
//	       G E H	init: G( k=13 ), E( g=10 ), H( i=11, j=12 )
//		\|/
//		 F	init: F( h=14 )

class A
{
public:
    int a;
    int b;
};

class B
{
public:
    float c;
};

class D:virtual public B
{
public:
    int d;
};

class C:public A, virtual public B, virtual public D
{
public:
    int d;
    long f;
};

class E:public C, public virtual D
{
public:
    int g;
};

class G:public A
{
public:
    unsigned k;
};

class H: public D
{
public:
    int i;
    int j;
};

class F:public G, public E, public H
{
public:
    int h;
};


int main( void )
{
    F f;

    f.C::a  = 1;		// initialization
    f.C::b = 2;
    f.G::a = 3;
    f.G::b = 4;
    f.c = 5;
    f.C::d = 6;
    f.H::d = 7;
    f.E::d = 8;
    f.f = 9;
    f.g = 10;
    f.i = 11;
    f.j = 12;
    f.k = 13;
    f.h = 14;

    if( f.C::d != 8 ) fail(__LINE__);
    if( f.E::a != 1 ) fail(__LINE__);
    f.C::c++;
    if( f.c != 6 ) fail(__LINE__);
    if( f.H::c != 6 ) fail(__LINE__);
    if( sizeof( f ) < sizeof( int ) * 12 + sizeof( float )
	+ sizeof( long ) ) fail(__LINE__);
    if( f.D::c != 6 ) fail(__LINE__);
    if( f.H::d != 7 ) fail(__LINE__);
    if( f.G::k != 13 ) fail(__LINE__);
    _PASS;
}
