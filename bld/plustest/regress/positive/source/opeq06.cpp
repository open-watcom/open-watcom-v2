#include "fail.h"

struct V {
    int va;
    int vb;
    V( int v ) : va(v), vb(v) {
    }
};

struct V1 : virtual V {
    int v1a;
    int v1b;
    V1( int v ) : v1a(v), v1b(v), V(v) {
    }
};

struct V2 : virtual V {
    int v2a;
    int v2b;
    V2( int v ) : v2a(v), v2b(v), V(v) {
    }
};

struct V3 : virtual V {
    int v3a;
    int v3b;
    V3( int v ) : v3a(v), v3b(v), V(v) {
    }
};

struct B0 : virtual V1, virtual V2 {
    B0( int v ) : V(v), V1(v), V2(v) {
    }
};

struct B1 : virtual V1, virtual V3 {
    B1( int v ) : V(v), V1(v), V3(v) {
    }
};

struct B2 : virtual V2, virtual V1 {
    B2( int v ) : V(v), V2(v), V1(v) {
    }
};

struct B3 : virtual V2, virtual V3 {
    B3( int v ) : V(v), V2(v), V3(v) {
    }
};

struct B4 : virtual V3, virtual V1 {
    B4( int v ) : V(v), V3(v), V1(v) {
    }
};

struct B5 : virtual V3, virtual V2 {
    B5( int v ) : V(v), V3(v), V2(v) {
    }
};

struct D : B0, B1, B2, B3, B4, B5 {
    int sig;
    D( int d ) : sig(d)
               , B0(d)
	       , B1(d)
	       , B2(d)
	       , B3(d)
	       , B4(d)
	       , B5(d)
	       , V(d)
	       , V1(d)
	       , V2(d)
	       , V3(d) {
    }
};

void check_B0s( B0 *d, B0 *s )
{
    if( d->va != s->va ) fail(__LINE__);
    if( d->vb != s->vb ) fail(__LINE__);
    if( d->v1a != s->v1a ) fail(__LINE__);
    if( d->v1b != s->v1b ) fail(__LINE__);
    if( d->v2a != s->v2a ) fail(__LINE__);
    if( d->v2b != s->v2b ) fail(__LINE__);
}

void check_B1s( B1 *d, B1 *s )
{
    if( d->va != s->va ) fail(__LINE__);
    if( d->vb != s->vb ) fail(__LINE__);
    if( d->v1a != s->v1a ) fail(__LINE__);
    if( d->v1b != s->v1b ) fail(__LINE__);
    if( d->v3a != s->v3a ) fail(__LINE__);
    if( d->v3b != s->v3b ) fail(__LINE__);
}

void check_B2s( B2 *d, B2 *s )
{
    if( d->va != s->va ) fail(__LINE__);
    if( d->vb != s->vb ) fail(__LINE__);
    if( d->v1a != s->v1a ) fail(__LINE__);
    if( d->v1b != s->v1b ) fail(__LINE__);
    if( d->v2a != s->v2a ) fail(__LINE__);
    if( d->v2b != s->v2b ) fail(__LINE__);
}

void check_B3s( B3 *d, B3 *s )
{
    if( d->va != s->va ) fail(__LINE__);
    if( d->vb != s->vb ) fail(__LINE__);
    if( d->v2a != s->v2a ) fail(__LINE__);
    if( d->v2b != s->v2b ) fail(__LINE__);
    if( d->v3a != s->v3a ) fail(__LINE__);
    if( d->v3b != s->v3b ) fail(__LINE__);
}

void check_B4s( B4 *d, B4 *s )
{
    if( d->va != s->va ) fail(__LINE__);
    if( d->vb != s->vb ) fail(__LINE__);
    if( d->v1a != s->v1a ) fail(__LINE__);
    if( d->v1b != s->v1b ) fail(__LINE__);
    if( d->v3a != s->v3a ) fail(__LINE__);
    if( d->v3b != s->v3b ) fail(__LINE__);
}

void check_B5s( B5 *d, B5 *s )
{
    if( d->va != s->va ) fail(__LINE__);
    if( d->vb != s->vb ) fail(__LINE__);
    if( d->v2a != s->v2a ) fail(__LINE__);
    if( d->v2b != s->v2b ) fail(__LINE__);
    if( d->v3a != s->v3a ) fail(__LINE__);
    if( d->v3b != s->v3b ) fail(__LINE__);
}

void assign_B0_ref( B0 &d, B0 &s )
{
    d = s;
    check_B0s( &d, &s );
}

void assign_B1_ref( B1 &d, B1 &s )
{
    d = s;
    check_B1s( &d, &s );
}

void assign_B2_ref( B2 &d, B2 &s )
{
    d = s;
    check_B2s( &d, &s );
}

void assign_B3_ref( B3 &d, B3 &s )
{
    d = s;
    check_B3s( &d, &s );
}

void assign_B4_ref( B4 &d, B4 &s )
{
    d = s;
    check_B4s( &d, &s );
}

void assign_B5_ref( B5 &d, B5 &s )
{
    d = s;
    check_B5s( &d, &s );
}

void assign_B0_ptr( B0 *d, B0 *s )
{
    d = s;
    check_B0s( d, s );
}

void assign_B1_ptr( B1 *d, B1 *s )
{
    *d = *s;
    check_B1s( d, s );
}

void assign_B2_ptr( B2 *d, B2 *s )
{
    *d = *s;
    check_B2s( d, s );
}

void assign_B3_ptr( B3 *d, B3 *s )
{
    *d = *s;
    check_B3s( d, s );
}

void assign_B4_ptr( B4 *d, B4 *s )
{
    *d = *s;
    check_B4s( d, s );
}

void assign_B5_ptr( B5 *d, B5 *s )
{
    *d = *s;
    check_B5s( d, s );
}

int main()
{
    {
	D x1(1);
	D x2(2);
	D x3(3);
	D x4(4);
	D x5(5);
	D x6(6);
	D x7(7);
	D x8(8);
	D x9(9);
	D x10(10);
	D x11(11);
	D x12(12);
    
	assign_B0_ref( x1, x2 );
	assign_B1_ref( x3, x4 );
	assign_B2_ref( x5, x6 );
	assign_B3_ref( x7, x8 );
	assign_B4_ref( x9, x10 );
	assign_B5_ref( x11, x12 );
    }
    {
	D x1(-1);
	D x2(-2);
	D x3(-3);
	D x4(-4);
	D x5(-5);
	D x6(-6);
	D x7(-7);
	D x8(-8);
	D x9(-9);
	D x10(-10);
	D x11(-11);
	D x12(-12);
    
	assign_B0_ptr( &x1, &x2 );
	assign_B1_ptr( &x3, &x4 );
	assign_B2_ptr( &x5, &x6 );
	assign_B3_ptr( &x7, &x8 );
	assign_B4_ptr( &x9, &x10 );
	assign_B5_ptr( &x11, &x12 );
    }
    _PASS;
}
