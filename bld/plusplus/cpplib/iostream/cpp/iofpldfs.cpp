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


#error not used anymore (will be deleted!)
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %     Copyright (C) 1992, by WATCOM International Inc.  All rights    %
// %     reserved.  No part of this software may be reproduced or        %
// %     used in any form or by any means - graphic, electronic or       %
// %     mechanical, including photocopying, recording, taping or        %
// %     information storage and retrieval systems - except with the     %
// %     written permission of WATCOM International Inc.                 %
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//  Modified    By              Reason
//  ========    ==              ======
//  93/05/26    Greg Bentz      pull floating point out of istream/ostream
//  93/10/08    Greg Bentz      make LDFloatToString set scale_factor to 1
//                              for _Ftos when 'G' format
//  93/10/25    Raymond Tang    Split into separate files.
//  94/04/06    Greg Bentz      combine header files
//  95/06/19    Greg Bentz      indirect call to math library
//                              *** OBSOLETE ***

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <float.h>
#include <stdlib.h>
#include <iostream.h>
#endif
#include "ioutil.h"
#include "iofhdr.h"

char *__LDFloatToString( long double &f, int precision,
/**********************************************************/
    ios::fmtflags format_flags, int *length_ptr ) {
// Convert a "long double" floating-point value to a string.
// If "ios::fixed" or "ios::scientific" is specified, use that form.
// Otherwise, use "scientific" only if the exponent is < -4 or > precision.
// Perform rounding, if necessary.

    char         *buffer;
    int           dec;
    int           sign;
    ios::fmtflags notation;
    int           exponent;
    int           fixed;
    int           round_digit;
    int           size;
    int           length;
    int           remainder;
    char *ptr;
    char *end_digits;
    char *res_buffer;
    char *bufptr;
    char *decptr;
    char *src;
    int           newlen;

    buffer   = __EFG_fcvt( f, LDBL_DIG, &dec, &sign );
    exponent = dec - 1;
    notation = (format_flags & ios::floatfield);
    if( notation == 0  ||  notation == ios::floatfield ) {
        fixed = (exponent >= -4  &&  exponent <= precision);
    } else {
        fixed = (notation == ios::fixed);
    }

    // Round the number at the digit following the precision digit:
    length      = ::strlen( buffer );
    round_digit = fixed ? precision + dec
                        : precision + 1;
    if( round_digit >= 0
            &&  round_digit < length
            &&  buffer[round_digit] >= '5' ) {
        for( ;; ) {
            if( --round_digit < 0 ) {
                ::memmove( buffer + 1, buffer, length - 1 );
                buffer[0] = '1';
                dec++;
                exponent++;
                break;
            }
            if( buffer[round_digit] == '9' ) {
                buffer[round_digit] = '0';
            } else {
                buffer[round_digit]++;
                break;
            }
        }
    }

    if( fixed ) {
        newlen = precision + 1 + ((dec > 0) ? dec : 1);
    } else {
        newlen  = precision + 4;         // d.pppppE+ee
        newlen += (abs( exponent ) < 100) ? 2 : 3;
    }
    if( sign || format_flags & ios::showpos ) {
        newlen++;
    }
    res_buffer = new char[newlen+1];
    if( res_buffer == NULL ) {
        return( NULL );
    }
    ::memset( res_buffer, '0', newlen );
    res_buffer[newlen] = '\0';
    bufptr             = res_buffer;
    if( sign ) {
        *(bufptr++) = '-';
    } else if( format_flags & ios::showpos ) {
        *(bufptr++) = '+';
    }
    if( fixed ) {
        if( dec <= 0 ) {                // 0.something
            bufptr++;
            decptr  = bufptr++;
            bufptr += -dec;
            src     = buffer;
        } else {
            size = (dec < length) ? dec : length;
            ::memcpy( bufptr, buffer, size );
            bufptr += size;
            decptr  = bufptr++;
            src     = buffer + size;
        }

        // Put in the decimal point and the following (precision) digits:
        *decptr = '.';
        size = (__huge_ptr_int)(buffer + length - src); // what's left?
        if( size > precision ) {
            size = precision;
        }
        if( size > 0 ) {
            ::memcpy( bufptr, src, size );
        }
        bufptr     = decptr + precision + 1;
        *bufptr    = '\0';
        end_digits = bufptr;
    } else {
        *(bufptr++) = buffer[0];
        *(bufptr++) = '.';
        size        = length - 1;               // how many digits left?
        if( size > precision ) {
            size = precision;
        }
        if( size > 0 ) {
            ::memcpy( bufptr, buffer + 1, size );
        }
        bufptr += precision;
        end_digits  = bufptr;
        *(bufptr++) = (char)((format_flags & ios::uppercase) ? 'E' : 'e');
        *(bufptr++) = (char)((exponent >= 0) ? '+' : '-');
        if( exponent < 0 ) {
            exponent = -exponent;
        }
        if( exponent < 10 ) {
            ++bufptr;                           // want 2 digits
        }
        itoa( exponent, bufptr, 10 );
        bufptr = ::strchr( bufptr, '\0' );
    }

    // If "showpoint" is not specified, then we must trim trailing zeros
    // and the decimal point:
    if( (format_flags & ios::showpoint) == 0 ) {
        ptr = end_digits;
        while( *--ptr == '0' ) {
        }
        if( *ptr != '.' ) {
            ++ptr;           // point at chars to be tossed
        }
        remainder = (__huge_ptr_int)(bufptr - end_digits);
        if( remainder > 0 ) {
            ::memcpy( ptr, end_digits, remainder );
        }
        newlen -= (end_digits - ptr);
    }
    *length_ptr = newlen;
    return( res_buffer );
}
