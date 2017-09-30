#include "fail.h"
#include <string.h>

struct S {
    S();
    S( S const & );
    ~S();
    int s[10];
};

S::S() {
    for( int i = 0; i < 10; ++i ) {
	s[i] = i;
    }
}
S::S(S const &) {
    for( int i = 0; i < 10; ++i ) {
	s[i] = i;
    }
}
S::~S() {
    for( int i = 0; i < 10; ++i ) {
	s[i] = -i;
    }
}

struct P {
    int p[10];
};

struct B1 {
    virtual S rS( int, P, S, int );
    virtual P rP( int, P, S, int );
    int b1[10];
};

struct B2 {
    virtual S rS( int, P, S, int );
    virtual P rP( int, P, S, int );
    int b2[10];
};

struct D : B1, B2 {
    virtual S rS( int, P, S, int );
    virtual P rP( int, P, S, int );
    int d[10];
    D( int );
};

D::D( int v ) {
    for( int i = 0; i < 10; ++i ) {
	b1[i] = v;
	b2[i] = v+1;
	d[i] = v-1;
    }
}

S B1::rS( int x, P p, S s, int v )
{
    p.p[x] = __LINE__;
    s.s[x] = v;
    return s;
}

S B2::rS( int x, P p, S s, int v )
{
    p.p[x] = __LINE__;
    s.s[x] = v;
    return s;
}

S D::rS( int x, P p, S s, int v )
{
    p.p[x] = __LINE__;
    s.s[x] = v;
    return s;
}

P B1::rP( int x, P p, S s, int v )
{
    p.p[x] = v;
    s.s[x] = __LINE__;
    return p;
}

P B2::rP( int x, P p, S s, int v )
{
    p.p[x] = v;
    s.s[x] = __LINE__;
    return p;
}

P D::rP( int x, P p, S s, int v )
{
    p.p[x] = v;
    s.s[x] = __LINE__;
    return p;
}

int main()
{
    P vp;
    S vs;
    P rp;
    S rs;
    D v1(6);

    memset( &vp, 0, sizeof( vp ) );
    memset( &vs, -1, sizeof( vs ) );
    B1 *pB1 = &v1;
    rp = pB1->rP( 5, vp, vs, 5 );
    if( rp.p[5] != 5 ) fail(__LINE__);
    rs = pB1->rS( 6, vp, vs, 6 );
    if( rs.s[6] != 6 ) fail(__LINE__);
    B2 *pB2 = &v1;
    rp = pB2->rP( 5, vp, vs, 5 );
    if( rp.p[5] != 5 ) fail(__LINE__);
    rs = pB2->rS( 6, vp, vs, 6 );
    if( rs.s[6] != 6 ) fail(__LINE__);
    _PASS;
}
