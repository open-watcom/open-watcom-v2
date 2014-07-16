#include "fail.h"

struct S {
    long v;
    S( long v ) : v(v) {}
    operator long() { return v; }
};

S m(2);
long n = 28;

int main()
{
    n -= m;
    if( n != 26 ) fail(__LINE__);
    n += m;
    if( n != 28 ) fail(__LINE__);
    n *= m;
    if( n != 56 ) fail(__LINE__);
    n /= m;
    if( n != 28 ) fail(__LINE__);
    n %= m;
    if( n != 0 ) fail(__LINE__);
    ++n;
    n <<= m;
    if( n != 4 ) fail(__LINE__);
    n >>= m;
    if( n != 1 ) fail(__LINE__);
    n ^= m;
    if( n != 3 ) fail(__LINE__);
    n &= m;
    if( n != 2 ) fail(__LINE__);
    n &= ~m;
    if( n != 0 ) fail(__LINE__);
    n |= m;
    if( n != 2 ) fail(__LINE__);
    _PASS;
}
