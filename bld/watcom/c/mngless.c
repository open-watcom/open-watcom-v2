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
* Description:  Check meaningless compare (type range problems).
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


cmp_result CheckMeaninglessCompare( rel_op rel, int op1_size, int result_size,
               int isBitField, signed_64 val, signed_64 *low, signed_64 *high )
/******************************************************************************/
// we're checking 'op1 cgop val' where val is a constant expression
{
    enum case_range     range;
    cmp_result          ret;
    signed_64           LOW_VAL = I64Val( 0x80000000, 0 );
    signed_64           HIGH_VAL = I64Val( 0xFFFFFFFF, 0xFFFFFFFF );
    signed_64           tmp;
    int                 shift;

    if( NumSign( op1_size ) && NumSign( op1_size ) != NumSign( result_size ) ) {
        if( NumBits( op1_size) == NumBits( result_size ) ) {
            // signed promoted to unsigned use unsigned range
            op1_size = NumBits( op1_size );
        } else if( !isBitField && NumBits( op1_size ) < NumBits( result_size ) ) {
            // signed promoted to bigger unsigned num gets signed extended
            // could have two ranges unsigned
            return( CMP_VOID ); // early return //TODO: could check == & !=
        }
    }
    shift = CMPMAXBITSIZE - NumBits( result_size );
    if( NumSign( result_size ) == 0 && shift > 0 ) {
        U64ShiftR( &HIGH_VAL, shift, &tmp );
        U64And( &val, &tmp, &val );
    }
    shift = CMPMAXBITSIZE - NumBits( op1_size );
    if( NumSign( op1_size ) ) {
        I64ShiftR( &LOW_VAL, shift, low );
        U64Not( low, high );
    } else {
        U64Set( low, 0, 0 );
        U64ShiftR( &HIGH_VAL, shift, high );
    }
    if( I64Cmp( &val, low ) == 0 ) {
        range = CASE_LOW_EQ;
    } else if( I64Cmp( &val, high ) == 0 ) {
        range = CASE_HIGH_EQ;
    } else if( shift > 0 ) {                // can't be outside range and
        if( I64Cmp( &val, low ) < 0 ) {     // don't have to do unsigned compare
            range = CASE_LOW;
        } else if( I64Cmp( &val, high ) > 0 ) {
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
