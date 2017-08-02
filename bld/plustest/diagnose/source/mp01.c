struct A { int a; };

struct B : A { int b; };

struct P { int p; };

struct C : P, B { int c; };

struct V : C { int v; };
struct V1 { int v1; };
struct V2 { int v2; };
struct V3 { int v3; };

struct D : virtual V1, virtual V2, virtual V3, virtual V { int d; };

struct E : virtual V1, virtual V2, virtual V3, virtual V, virtual D { int e; };

struct F { int f; };
struct F1 { int f1; };

struct G : F, E, virtual F1 { int g; };

struct H : virtual V3, virtual V2, virtual V, virtual V1, virtual G { int h; };

struct I { int i; };

struct J : I, H { int j; };

struct K { int k; };
struct K1 { int k1; };

struct L : J, virtual K, virtual K1 { int l; };

int A::* pa = &A::a;
int B::* pb = &B::a;
int C::* pc = &C::b;
int D::* pd = &D::v1;
int E::* pe = &E::v1;
int G::* pg = &G::v1;
int H::* ph = &H::v3;
int L::* pl = &H::v3;

int L::* p1 = (int L::*) &A::a;
int L::* p2 = (int L::*) 0;

extern void f();

void safe( void )
{
    f();
    pb = pa;		// copy
    f();
    pc = pb;		// delta
    f();
    pd = pa;		// delta, constant vindex
    f();
    pe = pc;		// constant vindex
    f();
    pg = pd;		// test for 0, constant vindex
    f();
    ph = pd;		// mapping
    f();
}

void unsafe( void )
{
    f();
    pa = (int A::*) pb;	// copy
    f();
    pb = (int B::*) pc;	// delta
    f();
    pa = (int A::*) pd;	// vidx = 0, delta
    f();
    pd = (int D::*) pg;	// > 4? 0
    f();
    pd = (int D::*) ph;	// mapping
    f();
    ph = (int H::*) pl;	// > 7? 0, if == 0 delta
    f();
    pe = (int E::*) ph;	// mapping, if == 0 delta
    f();
}

void test( void )
{
    if( pe ) {
	if( pe != pg ) {
	    if( pg != 0 ) {
		if( pg == &V1::v1 || pe == &E::v1 ) {
		    f();
		}
	    }
	}
    }
}
