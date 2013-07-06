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
* Description:  Converts a float value to string representation for stream
*               output.
*
****************************************************************************/

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <iostream>
#endif
#include "ioutil.h"
#include "lock.h"
#include "iofhdr.h"


#if 0
// it will be enabled as soon as long double will be supported
enum {
    FLOAT_OVERHEAD  = 8,            // 2 signs, decimal, E, 4-digit exponent
    MAX_PREC        = LDBL_DIG,
    LONGEST_FLOAT   = LDBL_DIG + FLOAT_OVERHEAD
};
#else
enum {
    FLOAT_OVERHEAD  = 7,            // 2 signs, decimal, E, 3-digit exponent
    MAX_PREC        = DBL_DIG,
    LONGEST_FLOAT   = DBL_DIG + FLOAT_OVERHEAD
};
#endif

namespace std {

  // Write a "long double" floating-point value.

  ostream &ostream::__outfloat( long double const &f ) {

    int                 digit_offset;
    int                 precision;
    char                buf[ LONGEST_FLOAT * 2 ];
    long_double         ld;
    int                 i;
    int                 len;
    char                *x;
    CVT_INFO            cvt;
    auto char           stkbuf[ LONGEST_FLOAT + 1 ];
    std::ios::fmtflags  format_flags;
#ifdef _LONG_DOUBLE_
    double              double_value;

    /* convert this double into a long double */
    double_value = f;
    __EFG__FDLD( &double_value, &ld );
#else
    ld.value = f;
#endif
    __lock_it( __i_lock );
    precision = this->precision();
    if( precision > MAX_PREC )
        precision = MAX_PREC;

    format_flags = this->flags();
    if(( format_flags & ( std::ios::scientific | ios::fixed )) == std::ios::scientific ) {
        cvt.flags = E_FMT;
        cvt.scale = 1;
    } else if(( format_flags & ( std::ios::scientific | ios::fixed )) == std::ios::fixed ) {
        cvt.flags = F_FMT;
        cvt.scale = 0;
    } else {
        cvt.flags = G_FMT;
        cvt.scale = 1;
        if( precision == 0 ) {
            precision = 1;
        }
    }
    if( format_flags & std::ios::showpoint ) {
        cvt.flags |= F_DOT;
    }
    cvt.ndigits = precision;
    cvt.expchar = ( format_flags & std::ios::uppercase ) ? 'E' : 'e';
    cvt.expwidth = 0;
    __EFG_LDcvt( &ld, &cvt, stkbuf );
    // put all the pieces together
    len = cvt.n1 + cvt.nz1 + cvt.n2 + cvt.nz2 + 1;
    if( cvt.sign < 0 ) {
        ++len;
    } else if( format_flags & std::ios::showpos ) {
        ++len;
    }
    if( len > sizeof( buf ) ) {
        x = new char[ len + 1 ];
    } else {
        x = buf;
    }
    i = 0;
    digit_offset = 1;
    if( cvt.sign < 0 ) {
        x[i++] = '-';
    } else if( format_flags & std::ios::showpos ) {
        x[i++] = '+';
    } else {
        digit_offset = 0;
    }
    if( cvt.n1 != 0 ) {
        ::memcpy( &x[i], &stkbuf[0], cvt.n1 );
        i += cvt.n1;
    }
    if( cvt.nz1 != 0 ) {
        ::memset( &x[i], '0', cvt.nz1 );
        i += cvt.nz1;
    }
    if( cvt.n2 != 0 ) {
        ::memcpy( &x[i], &stkbuf[cvt.n1], cvt.n2 );
        i += cvt.n2;
    }
    if( cvt.nz2 != 0 ) {
        ::memset( &x[i], '0', cvt.nz2 );
        i += cvt.nz2;
    }
    x[i] = '\0';
                
    if( opfx() ) {
        setstate( __WATCOM_ios::writeitem( *this, x, ::strlen( x ), digit_offset ) );
        osfx();
    }
    if( len > sizeof( buf ) ) {
        delete x;
    }
    return( *this );
  }
}   /* end namespace std */

