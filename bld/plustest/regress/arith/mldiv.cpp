#include <iostream.h>
// SP&E Vol.24(6) 579-601 (June 1994)
// "Multiple-length Division Revisited: a Tour of the Minefield"
// Per Brinch Hansen
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma inline_depth 0

template <UBIG w,UBIG b>
    struct MLDiv {
        UBIG d[w+1];
        MLDiv( void ) {
            zero( *this );
        }
        MLDiv( UBIG x ) {
            UBIG i;

            for( i = 0; i <= w; ++i ) {
                if( x == 0 ) break;
                d[i] = x % 10;
                x /= 10;
            }
            for( i = i; i <= w; ++i ) {
                d[i] = 0;
            }
        }
        static void fatal( char *m ) {
            fprintf( stderr, "'%s'\n", m );
            exit( EXIT_FAILURE );
        }
        static UBIG length( MLDiv const &r ) {
            UBIG i;
            UBIG j;

            i = w;
            j = 0;
            while( i != j ) {
                if( r.d[i] != 0 ) {
                    j = i;
                } else {
                    --i;
                }
            }
            // cout << "length(" << r << ") " << (i+1) << endl;
            return( i + 1 );
        }
        static void zero( MLDiv &r ) {
            memset( r.d, 0, sizeof( r.d ) );
        }
        static void product( MLDiv &x, MLDiv const &y, UBIG k ) {
            UBIG i;
            UBIG m;
            UBIG carry;
            UBIG temp;

            m = length( y );
            zero( x );
            carry = 0;
            for( i = 0; i < m; ++i ) {
                temp = y.d[i] * k + carry;
                x.d[i] = temp % b;
                carry = temp / b;
            }
            if( m <= w ) {
                x.d[m] = carry;
            } else {
                if( carry != 0 ) {
                    fatal( "product overflow" );
                }
            }
            // cout << "product: " << y << "*" << k << "=" << x << endl;
        }
        static void quotient( MLDiv &x, MLDiv const &y, UBIG k ) {
            int i;
            UBIG m;
            UBIG carry;
            UBIG temp;

            assert( &x != &y );
            m = length( y );
            zero( x );
            carry = 0;
            for( i = m-1; i >= 0; --i ) {
                temp = carry * b + y.d[i];
                x.d[i] = temp / k;
                carry = temp % k;
            }
            // cout << "quotient: " << y << "/" << k << "=" << x << endl;
        }
        static void remainder( MLDiv &x, MLDiv const &y, UBIG k ) {
            int i;
            UBIG m;
            UBIG carry;

            m = length( y );
            zero( x );
            carry = 0;
            for( i = m-1; i >= 0; --i ) {
                carry = ( carry * b + y.d[i] ) % k;
            }
            x.d[0] = carry;
            // cout << "remainder: " << y << "%" << k << "=" << x << endl;
        }
        static MLDiv prefix( MLDiv const &r, UBIG m, UBIG n ) {
            MLDiv p;

            zero( p );
            while( n != 0 ) {
                p.d[ n-1 ] = r.d[ m ];
                --n;
                --m;
            }
            return( p );
        }
        static UBIG trial( MLDiv const &r, MLDiv const &d, UBIG k, UBIG m ) {
            UBIG d2;
            UBIG km;
            UBIG r3;
            UBIG x;

            assert( 2 <= m && m <= (k+m) && (k+m) <= w );
            km = k + m;
            r3 = ( r.d[km]*b + r.d[ km-1 ] ) * b + r.d[ km-2 ];
            d2 = d.d[ m-1 ]*b + d.d[ m - 2 ];
            x = r3 / d2;
            if( (b-1) < x ) {
                x = b-1;
            }
            // cout << "trial: " << prefix( r, km, 3 ) << "/" << prefix( d, m-1, 2 ) << "=" << x << endl;
            return( x );
        }
        static UBIG smaller( MLDiv const &r, MLDiv const &dq, UBIG k, UBIG m ) {
            UBIG i;
            UBIG j;
            int ret;

            assert( k <= (k+m) && (k+m) <= w );
            i = m;
            j = 0;
            while( i != j ) {
                if( r.d[i+k] != dq.d[i] ) {
                    j = i;
                } else {
                    --i;
                }
            }
            ret = ( r.d[i+k] < dq.d[i] );
            // cout << "smaller: " << prefix( r, k+m, m + 1 ) << "<" << dq << "=" << ret << endl;
            return( ret );
        }
        static void difference( MLDiv &r, MLDiv const &dq, UBIG k, UBIG m ) {
            UBIG borrow;
            int diff;
            UBIG i;
            //MLDiv sr( prefix( r, m+k, m+1 ) );

            assert( k <= (k+m) && (k+m) <= w );
            // cout << "difference: " << sr << "-" << dq << "=" << prefix( r, m+k, m+1 ) << endl;
            borrow = 0;
            for( i = 0; i <= m; ++i ) {
                diff = r.d[ i+k ] - dq.d[i] - borrow + b;
                r.d[ i+k ] = diff % b;
                borrow = 1 - diff / b;
            }
            if( borrow != 0 ) {
                fatal( "difference overflow" );
            }
            // cout << "difference: " << sr << "-" << dq << "=" << prefix( r, m+k, m+1 ) << endl;
        }
        static void longdivide( MLDiv const &x, MLDiv const &y,
                                MLDiv &q, MLDiv &r,
                                UBIG n, UBIG m ) {
            MLDiv d;
            MLDiv dq;
            MLDiv tr;
            UBIG f;
            int k;
            UBIG qt;

            assert( 2 <= m && m <= n && n <= w );
            f = b / ( y.d[m-1] + 1 );
            product( tr, x, f );
            product( d, y, f );
            zero( q );
            for( k = n-m; k >= 0; --k ) {
                assert( 2 <= m && m <= (k+m) && (k+m) <= n && n <= w );
                qt = trial( tr, d, k, m );
                product( dq, d, qt );
                if( smaller( tr, dq, k, m ) ) {
                    --qt;
                    product( dq, d, qt );
                }
                q.d[k] = qt;
                difference( tr, dq, k, m );
            }
            quotient( r, tr, f );
        }
        static void division( MLDiv const &x, MLDiv const &y,
                              MLDiv &q, MLDiv &r ) {
            UBIG m;
            UBIG n;
            UBIG y1;

            m = length( y );
            if( m == 1 ) {
                y1 = y.d[ m - 1 ];
                if( y1 > 0 ) {
                    quotient( q, x, y1 );
                    remainder( r, x, y1 );
                } else {
                    fatal( "divide by 0" );
                }
            } else {
                n = length( x );
                if( m > n ) {
                    zero( q );
                    r = x;
                } else {
                    assert( 2 <= m && m <= n && n <= w );
                    longdivide( x, y, q, r, n, m );
                }
            }
        }
        friend ostream & operator <<( ostream &o, MLDiv const &r ) {
            int i;

            for( i = w-1; i > 0; --i ) {
                if( r.d[i] != 0 ) break;
            }
            for( i = i; i >= 0; --i ) {
                cout << (unsigned)r.d[i];
            }
            return o;
        }
    };

#define IBOUND 10000
#define I_INCR 1721
#define JBOUND 10000
#define J_INCR 1193

int main() {
    MLDiv<18,10> q;
    MLDiv<18,10> r;

    for( int i = 1; i < IBOUND; i += I_INCR ) {
        MLDiv<18,10> y(i);
        for( int j = 1; j < JBOUND; j += J_INCR ) {
            MLDiv<18,10> x(j);
            x.division( x, y, q, r );
            cout << x << "/" << y << " = " << q << " (remainder " << r << ")" << endl;
        }
    }
    return 0;
}
