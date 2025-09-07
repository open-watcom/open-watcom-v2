#include "fail.h"
#include <stdio.h> 
#include <stdlib.h>
#include <limits.h>

int ll_test(long long x) 
{ 
    switch (x) 
    { 
      case  1:
      case  0:
      case  2:
      case 42: return 1;
      default: return 0;
    } 
}
 
int ull_test(unsigned long long x)
{ 
    switch (x) 
    {
      case 127:
      case 0x7fff:
      case 0xffff:
      case 0x7fffffff: return 1;
      default: return 0;
    }
}

/*
    who doesn't know these numbers missed a great system:
    sys 49152   start SMON ( a [dis]assembler/monitor in 4kb ), loaded at $C000
    STA $D021   set the background color
    sys 64738   reset the C64 ( cold start )
    JSR $FFD2   CHROUT: write the accumulator to the current output
*/
int ull_c64(unsigned long long x)
{ 
    switch (x)
    {
      case  49152:
      case 0xD021:
      case  64738:
      case 0xffd2: return 1;
      default: return 0;
    }
}

/* 64 bit case values do not work yet */

 
int main(void)
{
    if( ll_test( 0 ) != 1 ) fail( __LINE__ );
    if( ll_test( 1 ) != 1 ) fail( __LINE__ );
    if( ll_test( 2 ) != 1 ) fail( __LINE__ );
    if( ll_test( 42 ) != 1 ) fail( __LINE__ );
    if( ll_test( 99 ) != 0 ) fail( __LINE__ );  
    if( ll_test( 0x12345678 ) != 0 ) fail( __LINE__ );


    if( ull_test( 0x7f ) != 1 ) fail( __LINE__ );
    if( ull_test( 32767ULL ) != 1 ) fail( __LINE__ );
    if( ull_test( 65535ULL ) != 1 ) fail( __LINE__ );

    if( ull_test( 42ULL ) != 0 ) fail( __LINE__ );
    if( ull_test( 99ULL ) != 0 ) fail( __LINE__ );
    if( ull_test( 0x12345678 ) != 0 ) fail( __LINE__ );

    if( ull_c64( 0xc000 ) != 1 ) fail( __LINE__ );
    if( ull_c64(  53281 ) != 1 ) fail( __LINE__ );
    if( ull_c64( 0xfce2 ) != 1 ) fail( __LINE__ );
    if( ull_c64(  65490 ) != 1 ) fail( __LINE__ );

    if( ull_c64( 42ull ) != 0 ) fail( __LINE__ );
    if( ull_c64( 99ull ) != 0 ) fail( __LINE__ );

    _PASS;
} 
