#include <limits.h>
struct M {
    int m;
    static void do_it(){};
};

void f();

typedef enum {ONE, TWO, TREE, __MAX=UINT_MAX } nums;

typedef struct {
   unsigned int ubitfield : 3;
   signed int sbitfield : 4;
} bit_struct;

void main()
{
    char c;
    signed char sc;
    unsigned char uc;
    unsigned short us;
    signed short ss;
    unsigned int u;
    unsigned long ul;
    signed long sl; 
    long l;
    signed long long sll; 
    long long ll;
    char *p;
    char *p1;
    nums e;
    bit_struct bits;

    if( p < (char *)0 );    // always 0
    if( (char *)0 > p );    // always 0
    if( 0 <= p );           // always 1
    if( p >= 0 );           // always 1

    if( c <= 127 );         // OK
    if( c >= -127 );        // always 1

    if( sc <= 127 );        // always 1
    if( sc >= -128 );       // always 1

    if( uc <= 0xfff );      // always 1
    if( uc >= 0 );          // always 1
    if( 0xff >= uc );       // always 1
    if( -1 <= uc );         // always 1

    if( 0xff >= uc );       // always 1
    if( 0 <= uc );          // always 1
    if( uc <= 0xff );       // always 1
    if( uc >= 0 );          // always 1

    if( uc < 0xff );        // OK
    if( uc > 0 );           // OK
    if( 0xff > uc );        // OK
    if( 0 < uc );           // OK

    if( uc > 0xff );        // always 0
    if( uc < 0 );           // always 0
    if( 0xff < uc );        // always 0
    if( 0 > uc );           // always 0

    if( u >= 0 );           // always 1
    if( u < 0 );            // always 0
    if( uc >= 0 );          // always 1
    if( uc < 0 );           // always 0
    if( us >= 0 );          // always 1
    if( us < 0 );           // always 0
    if( ss > SHRT_MAX );    // always 0
    if( ss < SHRT_MIN );    // always 0
    if( ul >= 0 );          // always 1
    if( ul < 0 );           // always 0

    if( sl >= LONG_MIN );   // always 1
    if( sl <= LONG_MAX );   // always 1

    if( l <= LONG_MAX );    // always 1
    if( l < LONG_MIN );     // always 0

    if( sll >= LLONG_MIN ); // always 1
    if( sll <= LLONG_MAX ); // always 1

    if( ll <= LLONG_MAX );  // always 1
    if( ll < LLONG_MIN );   // always 0

    if( p >= 0 );           // always 1
    if( 0 <= p );           // always 1
    if( p < 0 );            // always 0
    if( 0 > p );            // always 0

    if( e < 0 );            // always 0
    if( e > 12 );           // always 1

    if( &f >= 0 );          // always 1
    if( 0 <= &f );          // always 1
    if( &f < 0 );           // always 0
    if( 0 > &f );           // always 0

    if( &M::do_it >= 0 );   // always 1
    if( 0 <= &M::do_it );   // always 1
    if( &M::do_it < 0 );    // always 0
    if( 0 > &M::do_it );    // always 0

    if( bits.ubitfield < 0 );   // always 0
    if( bits.ubitfield > 7 );   // always 0 but can't diagnose this yet
    if( bits.sbitfield < -8 );  // always 0 but can't diagnose this yet
    if( bits.sbitfield > 7 );   // always 0 but can't diagnose this yet

    if( (l=1,c) >= (l=1,-3) );  // always 1
    if( (l=1,p1) >= (l=1,0) );  // always 1

}
