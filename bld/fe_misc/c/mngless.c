/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "mngless.h"

//  a <= x <=  b   i.e range of x is between a and b
enum  case_range {
    CASE_LOW,         // c < a
    CASE_LOW_EQ,      // c == a
    CASE_HIGH,        // c > b
    CASE_HIGH_EQ,     // c == b
    CASE_SIZE
};


static char const Meaningless[REL_SIZE][CASE_SIZE] = {
//    c < a      c == a     c >b       c==b
    { CMP_FALSE, CMP_VOID , CMP_FALSE, CMP_VOID },  // x == c
    { CMP_FALSE, CMP_FALSE, CMP_TRUE , CMP_VOID },  // x < c
    { CMP_FALSE, CMP_VOID , CMP_TRUE , CMP_TRUE },  // x <= c
};

#define NumBits( a )   ((a)&0x7f)

#ifdef FE_I64_MEANINGLESS
#define LOW_VAL         (0x8000000000000000I64)
#define HIGH_VAL        (0xffffffffffffffffUI64)
#else
#define LOW_VAL         (0x80000000)
#define HIGH_VAL        (0xfffffffful)
#endif

#define MAXSIZE        (sizeof( LARGEST_TYPE )*8)

cmp_result CheckMeaninglessCompare( rel_op rel, int op1_size, int result_size,
/**************************************************************************/
LARGEST_TYPE val, LARGEST_TYPE *low, LARGEST_TYPE *high )
// we're checking 'op1 cgop val' where val is a constant expression
{
    enum case_range     range;
    cmp_result          ret;

    if( NumSign(op1_size ) && NumSign(op1_size ) != NumSign(result_size) ) {
        if( NumBits( op1_size) < NumBits( result_size ) ) {
         // signed promoted to bigger unsigned num gets signed extended
        //  could have two ranges unsigned
            return( CMP_VOID ); // early return //TODO: could check == & !=
        } else if( NumBits( op1_size) == NumBits( result_size ) ) {
          // signed promoted to unsigned use unsigned range
          op1_size &= 0x7f;
        }
    }
    if( NumSign( result_size ) == 0 && NumBits( result_size ) == 16 ){
        val &= 0xffff; // num is truncated when compared
    }
    if( NumSign( op1_size ) ){
        *low = (LARGEST_TYPE)(LOW_VAL) >> MAXSIZE-NumBits( op1_size );
        *high = ~(*low);
    }else{
        *low = 0;
        *high = HIGH_VAL >> MAXSIZE-NumBits( op1_size );
    }
    if( val == *low ) {
        range = CASE_LOW_EQ;
    } else if( val == *high ) {
        range = CASE_HIGH_EQ;
    } else if( NumBits( op1_size ) < MAXSIZE ) { // can't be outside range and
        if( val < *low ) {                     // don't have to do unsigned compare
            range = CASE_LOW;
        } else if( val > *high ) {
            range = CASE_HIGH;
        } else {
            range = CASE_SIZE;
        }
    } else {
        range = CASE_SIZE;
    }
    if( range != CASE_SIZE ) {
        ret = Meaningless[rel][range];
    } else {
        ret = CMP_VOID;
    }
    return( ret );
}
