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


/***************************************************************************
 * this module is used to convert numbers from DOUBLE
 * to a ten-byte floating byte format ( tbyte )
 * the tbyte value is just stored as an array of chars
 *
 * ( note that tbytes DO use little-endian format - so that the order chars
 * are flipped around - the char containing the sign bit & 1st part of the
 * exponent field comes last, etc. )
 *
 * this module will not be necessary when (if) our compiler begins to support
 * ten byte floating point numbers itself
 *
 * note that for now, the ten byte format will NOT really have any more
 * precision than the current double format ( 8 byte ) -- the last few
 * bits of the mantissa will just be left as zero
 ***************************************************************************/

#include <float.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "watcom.h"

#define TBYTE_MAX_EXP 16383
#define LDBL_EXP_DIG 11
#define LDBL_MANTISSA_DIG 52
#define INBYTES 8
#define OUTBYTES 10


static void swap( char *a, char *b )
/**********************************/
{
    char tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
    return;
}

static void little_to_big_endian( char *data, uint_8 bytecount )
/**************************************************************/
{
    uint_8 i;

    for( i = 0; i < bytecount / 2; i++ ) {
        swap( &data[ i ], &data[ bytecount - 1 - i ] );
    }
}

static void copy_mantissa( char *input, char *output, uint_8 no_of_bits )
/***********************************************************************/
{
    #define     FIRST_BIT 0x80

                /* input_bit & output_bit are bit-masks -
                 * 1 bit is on at a time, moving left to right in the byte
                 */
    uint_8      input_bit = 0x08;
    uint_8      output_bit = 0x40;


    char        *input_byte = input + 1;
    char        *output_byte = output + 2;
    uint_8      bitcount;

    for( bitcount = 0; bitcount < no_of_bits; bitcount++ ) {
        if( (*input_byte) & input_bit ) {
            /* if the input bit is set, turn on the corresponding output bit */
            (*output_byte) |= output_bit;
        }
        input_bit >>=1;
        if( !input_bit ) {
            input_bit = FIRST_BIT;
            input_byte++;
        }
        output_bit >>=1;
        if( !output_bit ) {
            output_bit = FIRST_BIT;
            output_byte++;
        }
    }
    return;
}

void double2tbyte( double data, char *output )
/********************************************/
{
    double              save;
    char                *input;
    char                *tmp;
    unsigned short int  *tmp2;
    unsigned long int   sign, exponent;

    save = data;
    input = (char *)&data;

    exponent = *((unsigned short int *)(input+INBYTES-2));
    sign = exponent & 0x8000; /* get the 1st bit */
    exponent &= 0x7fff; /* get rid of sign bit */
    exponent >>= ( 15 - LDBL_EXP_DIG );
    /* now use this to kill off those digits in input */

    if( OUTBYTES == 10 && save != 0.0 ) {
        /* we need to add a 1 for the integer part */
        tmp = output + OUTBYTES - 3;
        (*tmp) |= 0x80; /* put a 1 in the integer bit */
    }

    /* now put the sign bit & exponent into the output */
    exponent -= LDBL_MAX_EXP;   /* unbiased */
    exponent += TBYTE_MAX_EXP;  /* biased for output type */

    tmp2 = (unsigned short int*)(output + OUTBYTES - 2);
    (*tmp2) |= exponent;

    if( sign ) {
        tmp = output + OUTBYTES - 1;
        (*tmp) |= 0x80; /* sign bit - leftmost bit of last byte */
    }

    /*=============================================================*/

    /* step 1 is done.
     * now flip both input & output into big-endian format & copy
     * the mantissa over bit by bit
     */

    little_to_big_endian( input, 8 );
    little_to_big_endian( output, 10 );

    copy_mantissa( input, output, LDBL_MANTISSA_DIG );

    little_to_big_endian( input, 8 ); /* same both ways */
    little_to_big_endian( output, 10 );

    return;
}
