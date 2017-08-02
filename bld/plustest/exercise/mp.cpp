#include <stdio.h>

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

L data;

#define f() diag( __LINE__ );


void prt_diag( int L::*p )
{
    if( p ) {
        printf( " %c", data.*p );
    } else {
        printf( " %c", '-' );
    }
}


void diag( unsigned line )
{
    printf( "%3d -", line );
    prt_diag( pa );
    prt_diag( pb );
    prt_diag( pc );
    prt_diag( pd );
    prt_diag( pe );
    prt_diag( pg );
    prt_diag( ph );
    prt_diag( pl );
    prt_diag( p1 );
    prt_diag( p2 );
    printf( "\n" );
}

void safe( void )
{
    printf( "\nSafe\n****\n" );
    f();
    pb = pa;            // copy
    f();
    pc = pb;            // delta
    f();
    pd = pa;            // delta, constant vindex
    f();
    pe = pc;            // constant vindex
    f();
    pg = pd;            // test for 0, constant vindex
    f();
    ph = pd;            // mapping
    f();
}

void unsafe( void )
{
    printf( "\nUnsafe\n******\n" );
    f();
    pa = (int A::*) pb; // copy
    f();
    pb = (int B::*) pc; // delta
    f();
    pa = (int A::*) pd; // vidx = 0, delta
    f();
    pd = (int D::*) pg; // > 4? 0
    f();
    pd = (int D::*) ph; // mapping
    f();
    ph = (int H::*) pl; // > 7? 0, if == 0 delta
    f();
    pe = (int E::*) ph; // mapping, if == 0 delta
    f();
}

void test( void )
{
    if( pe ) {                  // generates bad code
        if( pe != pg ) {
            if( pg != 0 ) {
                if( pg == &V1::v1 || pe == &E::v1 ) {
                    f();
                }
            }
        }
    }
}


int main( void )
{
    data.a = 'a';
    data.b = 'b';
    data.c = 'c';
    data.d = 'd';
    data.e = 'e';
    data.f = 'f';
    data.g = 'g';
    data.h = 'h';
    data.i = 'i';
    data.j = 'j';
    data.k = 'k';
    data.p = 'p';
    data.v = 'v';
    data.v1 = '1';
    data.v2 = '2';
    data.v3 = '3';
    data.k1 = 'K';
    data.f1 = 'F';
    safe();
    unsafe();
    return 0;
}
