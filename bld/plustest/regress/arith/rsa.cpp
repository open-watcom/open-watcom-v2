#include <stdio.h>

// from Introduction to Algorithms (Cormen,Leiserson,Rivest) Chapter 33
#ifdef __WATCOM_INT64__
typedef __int64 INT;
#define FMT "I64"
#else
typedef long INT;
#define FMT "l"
#endif

// Public(e,n) Secret(d,n)
INT n;
INT e;
INT d;

INT pow_mod( INT x, INT e, INT mod ) {
    INT m;
    INT s;

    s = 1;
    for( m = 1; m != 0 && m <= e; m <<= 1 ) {
        if( e & m ) {
            s *= x;
            s %= mod;
        }
        x *= x;
        x %= mod;
    }
    return( s );
}

void test( char c ) {
    INT x1 = pow_mod( c, e, n );
    INT x2 = pow_mod( x1, d, n );
    printf( "%c -> %c\n", c, (char) x2 );
}

char text[] = "this is example text to encrypt\0";

INT gcd( INT n1, INT n2 ) {
    INT p = 0;

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

void extended_euclid( INT a, INT b, INT *r ) {
    INT t[3];

    if( b == 0 ) {
        r[0] = a;
        r[1] = 1;
        r[2] = 0;
    } else {
        extended_euclid( b, a % b, t );
        r[0] = t[0];
        r[1] = t[2];
        r[2] = t[1] - t[2] * ( a / b );
    }
}

void rsa( INT p, INT q )
{
    INT phi;
    INT odd;
    INT r[3];

    n = p * q;
    phi = ( p - 1 ) * ( q - 1 );
    for( odd = 3; odd < phi; ++odd ) {
        if( gcd( odd, phi ) == 1 ) {
            break;
        }
    }
    e = odd;
    // find 'd' where e*d == 1 (mod phi)
    extended_euclid( e, phi, r );
    d = ( r[1] / r[0] ) % phi;
    if( d < 0 ) {
        d += (( d / phi ) + 1 ) * phi;
    }
}

int is_prime( INT x )
{
    // if x is prime, a^x-1 == 1 ( mod x )
    if( pow_mod( 2, x - 1, x ) != 1 ) {
        return( 0 );
    }
    return( 1 );
}

int main() {
    char *c;
    INT p = 11;
    INT q = 9;
    p *= p;
    q *= q;
#ifdef __WATCOM_INT64__
    p *= p;
    q *= q;
#endif
    while( ! is_prime( p ) ) {
        p += 2;
    }
    while( ! is_prime( q ) ) {
        q += 2;
    }
    rsa( p, q );
    for( c = text; *c; ++c ) {
        test( *c );
    }
    return 0;
}
