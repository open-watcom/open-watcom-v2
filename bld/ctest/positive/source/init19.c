#include "fail.h"
#include <stdlib.h>
#include <string.h>

/* Initialization examples from C99 standard, section 6.7.8.
 * Examples 9-12 are covered by test init18.c.
 */

/* Example 1 */
int i = 3.5;
//complex c = 5 + 3 * I; whenever complex support is done

/* Example 2 */
int x[] = { 1, 3, 5 };
int _x[3] = { 1, 3, 5 };

/* Example 3 */
int y[4][3] = {
    { 1, 3, 5 },
    { 2, 4, 6 },
    { 3, 5, 7 },
};

int _y[] = {1,3,5,2,4,6,3,5,7,0,0,0};

/* Example 4 */
int z[4][3] = {
    { 1 }, { 2 }, { 3 }, { 4 }
};

int _z[] = {1,0,0,2,0,0,3,0,0,4,0,0};

/* Example 5 */
struct { int a[3], b; } w[] = { { 1 }, 2 };

int _w[] = {
    1,0,0,0,
    2,0,0,0,
};

/* Example 6 */
short q[4][3][2] = {
    { 1 },
    { 2, 3 },
    { 4, 5, 6 }
};

short _q[] = {
    1,0,0,0,0,0,
    2,3,0,0,0,0,
    4,5,6,0,0,0,
    0,0,0,0,0,0
};

/* Example 7 */
typedef int A[];
A a = { 1, 2 }, b = { 3, 4, 5 };

_a[2] = {1,2};
_b[3] = {3,4,5};

/* Example 8 */
char s[] = "abc", t[3] = "abc";


int main( void ) {
    /* Example 1 */
    if( i != 3 ) fail( __LINE__ );
    /* Example 2 */
    if( sizeof( x ) != 3 * sizeof( int ) ) fail( __LINE__ );
    if( memcmp( &x, &_x, sizeof( x ) ) ) fail( __LINE__ );
    /* Example 3 */
    if( sizeof( y ) != sizeof( _y ) ) fail( __LINE__ );
    if( memcmp( &y, &_y, sizeof( y ) ) ) fail( __LINE__ );
    /* Example 4 */
    if( sizeof( z ) != sizeof( _z ) ) fail( __LINE__ );
    if( memcmp( &z, &_z, sizeof( z ) ) ) fail( __LINE__ );
    /* Example 5 */
    if( sizeof( w ) != sizeof( _w ) ) fail( __LINE__ );
    if( memcmp( &w, &_w, sizeof( w ) ) ) fail( __LINE__ );
    /* Example 6 */
    if( sizeof( q ) != sizeof( _q ) ) fail( __LINE__ );
    if( memcmp( &q, &_q, sizeof( q ) ) ) fail( __LINE__ );
    /* Example 7 */
    if( sizeof( a ) != sizeof( _a ) ) fail( __LINE__ );
    if( memcmp( &a, &_a, sizeof( a ) ) ) fail( __LINE__ );
    if( sizeof( b ) != sizeof( _b ) ) fail( __LINE__ );
    if( memcmp( &b, &_b, sizeof( b ) ) ) fail( __LINE__ );
    /* Example 8 */
    if( sizeof( s ) != 4 ) fail( __LINE__ );
    if( sizeof( t ) != 3 ) fail( __LINE__ );
    if( strcmp( s, "abc" ) ) fail( __LINE__ );
    if( strncmp( t, "abc", 3 ) ) fail( __LINE__ );
    _PASS;
}
