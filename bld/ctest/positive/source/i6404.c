#include "fail.h"

unsigned __int64 p[] = {
    401953,
    834527,
    940001,
    1123477,
    1234577,
    1318697,
    2113,
    2129,
    2,
    3,
};
void test( unsigned __int64 x, unsigned __int64 y ) {
    unsigned __int64 c = x * y;
    unsigned __int64 d;
    unsigned __int64 e = y-1;

    if( c % y != 0 ) _fail;
    if( c % x != 0 ) _fail;
    if( c % y >= y ) _fail;
    if( c % x >= x ) _fail;
    if( c / y != x ) _fail;
    if( c / x != y ) _fail;
    if( (c+e) % y != e ) _fail;
    d = c;
    c *= 11;
    if( c % y != 0 ) _fail;
    if( c % x != 0 ) _fail;
    if( c % y >= y ) _fail;
    if( c % x >= x ) _fail;
    if( c / y != 11*x ) _fail;
    if( c / x != 11*y ) _fail;
    if( c / d != 11 ) _fail;
    if( (c+e) % y != e ) _fail;
    d = c;
    c *= 13;
    if( c % y != 0 ) _fail;
    if( c % x != 0 ) _fail;
    if( c % y >= y ) _fail;
    if( c % x >= x ) _fail;
    if( c / y != 13*11*x ) _fail;
    if( c / x != 13*11*y ) _fail;
    if( c / d != 13 ) _fail;
    if( (c+e) % y != e ) _fail;
    d = c;
    c *= 2111;
    if( c % y != 0 ) _fail;
    if( c % x != 0 ) _fail;
    if( c % y >= y ) _fail;
    if( c % x >= x ) _fail;
    if( c / y != 2111L*13*11*x ) _fail;
    if( c / x != 2111L*13*11*y ) _fail;
    if( c / d != 2111 ) _fail;
    if( (c+e) % y != e ) _fail;
}
void stest( __int64 x, __int64 y ) {
    __int64 q, r;
    q = x / y;
    r = x % y;
    if( q*y + r != x ) _fail;
    x = -x;
    q = x / y;
    r = x % y;
    if( q*y + r != x ) _fail;
    y = -y;
    q = x / y;
    r = x % y;
    if( q*y + r != x ) _fail;
    x = -x;
    q = x / y;
    q = x / y;
    r = x % y;
    if( q*y + r != x ) _fail;
}
int main() {
    int i, j;
    for( i = 0; i < sizeof(p)/sizeof(p[0]); ++i ) {
        for( j = 0; j < sizeof(p)/sizeof(p[0]); ++j ) {
            test( p[i], p[j] );
            stest( p[i], p[j] );
        }
    }
    _PASS;
}
