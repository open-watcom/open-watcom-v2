#include "fail.h"

// -xds, -xdt, -xst ==> PASS
// -xs, -xss ==> failure

struct CD {
    int v;
    int nv;
    CD( int );
    CD( CD const & );
    ~CD();
    void operator =( CD const & );
};
CD::CD( int v ) : v(v), nv(-v) {
    //printf("CD(%p,%d)\n", this, v );
}
CD::CD( CD const &s ) : v(s.v), nv(s.nv) {
    //printf("CD(%p,%p)\n", this, &s );
}
CD::~CD() {
    //printf("~CD(%p)\n", this );
    if( v != -nv ) fail(__LINE__);
    v = -__LINE__;
    nv = -__LINE__;
}
void CD::operator =( CD const &s )
{
    //printf("op=(%p,%p)\n", this, &s );
    v = s.v;
}

CD operator *( CD const &r1, CD const &r2 )
{
    //printf("op*(%p,%p)\n", &r1, &r2 );
    return CD( r1.v * r2.v );
}
CD operator *( CD const &r1, int r2 )
{
    //printf("op*(%p,%d)\n", &r1, r2 );
    return CD( r1.v * r2 );
}
CD operator *( int r1, CD const &r2 )
{
    //printf("op*(%d,%p)\n", r1, &r2 );
    return CD( r1 * r2.v );
}
int inline operator ==( CD const &r1, CD const &r2 ) {
    //printf("op==(%p,%p)\n", &r1, &r2 );
    return( r1.v == r2.v && r1.nv == r2.nv );
}

void check( int b, unsigned line )
{
    if( !b ) fail(line);
}

CD test() {
    CD x(2);
    CD y(3);
    CD cxy(2*3);
    CD xy = x * y;
    check( xy == cxy, __LINE__ );
    xy = x * 3;
    check( xy == cxy, __LINE__ );
    xy = 2 * y;
    check( xy == cxy, __LINE__ );
    xy = x * y;
    check( xy == cxy, __LINE__ );
    return xy;
}

int main() {
    {
	CD cxy( CD(2) * CD(3) );
	check( test() == cxy, __LINE__ );
    }
    _PASS;
}
