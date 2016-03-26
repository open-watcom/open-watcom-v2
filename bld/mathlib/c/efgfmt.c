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
* Description:  Implementation of _EFG_Format(). This routine is the
*               interface between clib and mathlib.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "myvalist.h"
#include "xfloat.h"
#include "farsupp.h"
#include "printf.h"
#include "rtcntrl.h"
#include "fltsupp.h"


/* NB: This code assumes ASCII character encoding. Probably not unreasonable. */


VISIBILITY FAR_STRING _EFG_Format( char *buffer, my_va_list *args, SPECS __SLIB *specs )
{
    int         digits;
    int         fmt;
    CVT_INFO    cvt;
    double      double_value;
    long_double ld;

    cvt.expchar = specs->_character;    /* 'e', 'g' exponent character */
    digits = specs->_prec;
    fmt = specs->_character & 0x5F;
    if( fmt == 'G' ) {
        if( digits == 0 )  digits = 1;  /* 27-oct-88 */
        cvt.expchar -= 2;               /* change exponent to 'e' or 'E' */
        cvt.flags = G_FMT;
        cvt.scale = 1;
    } else if( fmt == 'E' ) {
        cvt.flags = E_FMT;
        cvt.scale = 1;
    } else {
        cvt.flags = F_FMT;
        cvt.scale = 0;
    }
    if( !(specs->_character & 0x20) ) { /* test for 'E', 'F', or 'G' */
        cvt.flags |= IN_CAPS;           /* INF/NAN needs to be uppercase */
    }
    if( specs->_flags & SPF_ALT ) {
        cvt.flags |= F_DOT;
    }
    if( (specs->_flags & SPF_LONG_DOUBLE) && !_LDisDouble() ) {
        ld = va_arg( args->v, long_double );
        cvt.flags |= LONG_DOUBLE;
    } else {
        double_value = va_arg( args->v, double );

#ifdef _LONG_DOUBLE_
        /* convert this double into a long double */
        __iFDLD( &double_value, &ld );
#else
        ld.value = double_value;
#endif
    }
    if( digits == -1 ) {
        digits = 6;
    }
    cvt.ndigits = digits;
    cvt.expwidth = 2;   // Only relevant for E format
    __LDcvt( &ld, &cvt, buffer + 1 );
    specs->_n1  = cvt.n1;
    specs->_nz1 = cvt.nz1;
    specs->_n2  = cvt.n2;
    specs->_nz2 = cvt.nz2;
    if( cvt.flags & IS_INF_NAN ) {
        specs->_pad_char = ' '; /* No zero padding for inf/nan! */
    }
    if( cvt.sign < 0 ) {
        buffer[specs->_n0++] = '-';
    } else if( specs->_flags & SPF_FORCE_SIGN ) {
        buffer[specs->_n0++] = '+';
    } else if( specs->_flags & SPF_BLANK ) {
        buffer[specs->_n0++] = ' ';
    }

    return( buffer );
} /* _EFG_Format() */

#if defined( __MAKE_DLL_MATHLIB )
_type_EFG_Format *__get_EFG_Format( void )
{
    return( &_EFG_Format );
}
#endif
