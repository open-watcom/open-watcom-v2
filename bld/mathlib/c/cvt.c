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


#include "variety.h"
#include <stdio.h>
#include <string.h>
#include "xfloat.h"


char *__cvt( double value,
             int    ndigits,
             int    *dec,
             int    *sign,
             int    fmt,
             char   *buf )
{
    CVT_INFO    cvt;
    long_double ld;
#ifdef _LONG_DOUBLE_
    double      double_value;

    /* convert this double into a long double */
    double_value = value;
    __iFDLD( &double_value, &ld );
#else
    ld.value = value;
#endif
    if( fmt == 'F' ) {          // fcvt passes in 'F'
        cvt.flags = F_FMT + F_CVT;
        cvt.scale = 0;
    } else {                    // ecvt passes in 'G'
        cvt.flags = G_FMT + F_CVT;
        cvt.scale = 1;
    }
    cvt.ndigits = ndigits;
    cvt.expwidth = 0;
    cvt.expchar  = fmt;
    __LDcvt( &ld, &cvt, buf );
    *dec = cvt.decimal_place;
    *sign = cvt.sign;
    return( buf );
}
