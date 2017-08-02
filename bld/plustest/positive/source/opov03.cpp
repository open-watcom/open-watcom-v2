#include "fail.h"

struct S {
    int operator ()( int, int = 2, int = 3 );
    int operator ()( char, int = -1 );
    int s;
    S( int s ) : s(s) {
    }
};
int S::operator ()( int x, int y, int z )
{
    return x + y + z;
}
int S::operator ()( char a, int x )
{
    return s + x + a;
}

int main() {
    S x(__LINE__);
    if( x(-5) != 0 ) fail(__LINE__);
    if( x(5) != 10 ) fail(__LINE__);
    if( x('a') != (__LINE__+'a'-1-3) ) fail(__LINE__);
    if( x('b',-2) != (__LINE__+'b'-2-4) ) fail(__LINE__);
    if( x(5,4) != 12 ) fail(__LINE__);
    if( x(5,4,6) != 15 ) fail(__LINE__);
    _PASS;
}
