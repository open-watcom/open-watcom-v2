/* 
 * Test of conversion from / to _Bool bit-field
 */

#include "fail.h"
#include <stdbool.h>

#define TRUE    1
#define FALSE   0

static bool func_bol( const bool x ){ return x; }

struct bit2 {
    unsigned int uibf : 7;
      signed int sibf : 7;
             int pibf : 7;
            bool bobf : 1;
            bool bobx : 2;
} bits = { -1, -1, -1, -1, -1 };

int main( void ){
    bool    bol;
    double  d;

    if( func_bol( FALSE ) == TRUE ) fail( __LINE__ );
    if( func_bol( TRUE ) != TRUE ) fail( __LINE__ );
    if( func_bol( bits.uibf ) != TRUE ) fail( __LINE__ );
    if( func_bol( bits.sibf ) != TRUE ) fail( __LINE__ );
    if( func_bol( bits.pibf ) != TRUE ) fail( __LINE__ );
    if( func_bol( bits.bobf ) != TRUE ) fail( __LINE__ );
    if( func_bol( bits.bobx ) != TRUE ) fail( __LINE__ );
    d = 4.33;
    bol = d;
    if( func_bol( d ) != TRUE ) fail( __LINE__ );
    bits.uibf = bol;
    bits.sibf = bol;
    bits.pibf = bol;
    bits.bobf = bol;
    bits.bobx = bol;
    if( bol != TRUE ) fail( __LINE__ );
    if( bits.uibf != TRUE ) fail( __LINE__ );
    if( bits.sibf != TRUE ) fail( __LINE__ );
    if( bits.pibf != TRUE ) fail( __LINE__ );
    if( bits.bobf != TRUE ) fail( __LINE__ );
    if( bits.bobx != TRUE ) fail( __LINE__ );
    if( func_bol( bol ) != TRUE ) fail( __LINE__ );
    if( func_bol( bits.uibf ) != TRUE ) fail( __LINE__ );
    if( func_bol( bits.sibf ) != TRUE ) fail( __LINE__ );
    if( func_bol( bits.pibf ) != TRUE ) fail( __LINE__ );
    if( func_bol( bits.bobf ) != TRUE ) fail( __LINE__ );
    if( func_bol( bits.bobx ) != TRUE ) fail( __LINE__ );
    _PASS;
}
