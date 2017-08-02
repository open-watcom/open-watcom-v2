#include <stdio.h>

#ifdef __WATCOM_INT64__

typedef unsigned __int64 UINT;

UINT gcd( UINT n1, UINT n2 ) {
    UINT p = 0;

    while((( n1 | n2 ) & 1 ) == 0 ) {
        n1 >>= 1;
        n2 >>= 1;
        ++p;
    }
    for(;;) {
        if( n1 == 0 ) {
            return n2 << p;
        }
        if( n2 == 0 ) {
            return n1 << p;
        }
        while(( n1 & 1 ) == 0 ) {
            n1 >>= 1;
        }
        while(( n2 & 1 ) == 0 ) {
            n2 >>= 1;
        }
        if( n1 > n2 ) {
            n1 -= n2;
        } else {
            n2 -= n1;
        }
    }
}

void term( UINT i, UINT a[2] ) {
    UINT g;
    UINT n;
    UINT d;
    UINT d1 = 8 * i + 1;
    UINT d2 = 8 * i + 4;
    UINT d3 = 8 * i + 5;
    UINT d4 = 8 * i + 6;
    n = 4;
    d = d1;
    n = n * d2 - 2 * d;
    d *= d2;
    n = n * d3 - d;
    d *= d3;
    n = n * d4 - d;
    d *= d4;
    g = gcd( n, d );
    n /= g;
    d /= g;
    d <<= i * 4;
    a[0] = d;
    a[1] = n;
}

void print( UINT n, UINT d ) {
    int i;
    UINT q;
    UINT r;

    for( i = 0; i < 20; ++i ) {
        q = n / d;
        r = n % d;
        printf( "%u", (unsigned) q );
        n = r * 10;
    }
    printf( "\n" );
}

main() {
    UINT a[2];
    UINT i;
    UINT sn = 0;
    UINT sd = 1;
    UINT g;

    for( i = 0; i < 4; ++i ) {
        term( i, a );
        sn = sn * a[0] + a[1]*sd;
        sd *= a[0];
        g = gcd( sn, sd );
        sn /= g;
        sd /= g;
        print( sn, sd );
    }
    return 0;
}
#else
main() {
    puts( "31333333333333333333" );
    puts( "31414224664224664224" );
    puts( "31415873903465815230" );
    puts( "31415924575674353818" );
    return 0;
}
#endif
