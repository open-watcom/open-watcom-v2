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
* Description:  Convert floating-point number to string representation.
*
****************************************************************************/


#include "ftnstd.h"
#include "target.h"
#include "xfloat.h"
#include <string.h>


void    R_F2E( extended val, char *buff, int fld_width, int digs, bool plus,
               int scale, int exp_width, char exp_chr ) {
//==========================================================================

// Convert floating point number to E format.

    int         i, j;
    int         width;
    long_double ld;
    CVT_INFO    cvt;
    auto char   stkbuf[CONVERSION_BUFFER+1];
#ifdef _LONG_DOUBLE_
    double      double_value;

    /* convert this double into a long double */
    double_value = val;
    __iFDLD( &double_value, &ld );
#else
    ld.value = val;
#endif
    cvt.flags    = E_FMT + F_DOT;
    cvt.ndigits  = digs;
    cvt.scale    = scale;
    cvt.expwidth = exp_width;
    cvt.expchar  = exp_chr;
    __LDcvt( &ld, &cvt, stkbuf );
    width = cvt.n1 + cvt.nz1 + cvt.n2 + cvt.nz2;
    if( cvt.sign < 0 || plus ) ++width;
    j = 0;
    if( width > fld_width ) { // if too big, try to get rid of optional chars
        if( stkbuf[0] == '0' ) {
            --width;
            j = 1;
        }
    }
    if( (cvt.expwidth > exp_width) ||
        (-scale >= digs) ||
        (scale >= digs + 2) ||
        (width > fld_width) ) {
        memset( buff, '*', fld_width );
    } else {
        i = fld_width - width;
        memset( buff, ' ', i );
        if( cvt.sign < 0 ) {
            buff[i++] = '-';
        } else if( plus ) {
            buff[i++] = '+';
        }
        for( ; j < cvt.n1; j++ ) {
            buff[i++] = stkbuf[j];
        }
        for( j = 0; j < cvt.nz1; j++ ) {
            buff[i++] = '0';
        }
        for( j = 0; j < cvt.n2; j++ ) {
            buff[i++] = stkbuf[cvt.n1+j];
        }
        for( j = 0; j < cvt.nz2; j++ ) {
            buff[i++] = '0';
        }
    }
}


void    R_F2F( extended val, char *buff, int fld_width, int digs, bool plus,
               int scale ) {
//==========================================================================

// Convert floating point number to F format.

    int         i;
    int         j = 0;
    int         width;
    long_double ld;
    CVT_INFO    cvt;
    auto char   stkbuf[34];
#ifdef _LONG_DOUBLE_
    double      double_value;

    /* convert this double into a long double */
    double_value = val;
    __iFDLD( &double_value, &ld );
#else
    ld.value = val;
#endif
    cvt.flags = F_FMT + F_DOT;
    cvt.ndigits = digs;
    cvt.scale = scale;
    cvt.expwidth = 0;
    cvt.expchar = 0;
    __LDcvt( &ld, &cvt, stkbuf );
    width = cvt.n1 + cvt.nz1 + cvt.n2 + cvt.nz2;
    if( cvt.sign < 0 || plus ) ++width;
    if( width > fld_width ) { // if too big, try to get rid of optional chars
        if( stkbuf[0] == '0' ) {
            --width;
            j = 1;
        }
    }
    if( width > fld_width ) {
        memset( buff, '*', fld_width );
    } else {
        i = fld_width - width;
        memset( buff, ' ', i );
        if( cvt.sign < 0 ) {
            buff[i++] = '-';
        } else if( plus ) {
            buff[i++] = '+';
        }
        if( cvt.n1 > 0 ) {
            memcpy( &buff[i], &stkbuf[j], cvt.n1 - j );
            i += cvt.n1 - j;
        }
        if( cvt.nz1 > 0 ) {
            memset( &buff[i], '0', cvt.nz1 );
            i += cvt.nz1;
        }
        if( cvt.n2 > 0 ) {
            memcpy( &buff[i], &stkbuf[cvt.n1], cvt.n2 );
            i += cvt.n2;
        }
        if( cvt.nz2 > 0 ) {
            memset( &buff[i], '0', cvt.nz2 );
            i += cvt.nz2;
        }
    }
}


void    SetMaxPrec( int precision ) {
//===================================

// Set maximum precision for formatting routines.

    precision = precision;
}
