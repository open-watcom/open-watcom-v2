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
* Description:  Implementation of gcvt().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "xfloat.h"

#if defined(__WIDECHAR__)
_WMRTLINK CHAR_TYPE *_wgcvt( double value, int digits, CHAR_TYPE *buf )
{
    gcvt( value, digits, (char *)buf );
    return( _atouni( buf, (char *)buf ) );
}
#else
_WMRTLINK char *gcvt( double value, int digits, char *buf )
{
    int         i;
    CVT_INFO    cvt;
    long_double ld;
    auto char   stkbuf[64];     // See comment in ldcvt.c regarding stkbuf size
#ifdef _LONG_DOUBLE_
    double      double_value;

    /* convert this double into a long double */
    double_value = value;
    __iFDLD( &double_value, &ld );
#else
    ld.value = value;
#endif
    cvt.ndigits = digits;
    cvt.flags = G_FMT | NO_TRUNC;
    cvt.scale = 1;
    cvt.expwidth = 0;
    cvt.expchar  = 'E';
    __LDcvt( &ld, &cvt, stkbuf );
    i = 0;
    if( cvt.sign < 0 )  buf[i++] = '-';
    if( cvt.n1 != 0 ) {
        memcpy( &buf[i], &stkbuf[0], cvt.n1 );
        i += cvt.n1;
    }
    memset( &buf[i], '0', cvt.nz1 );
    i += cvt.nz1;
    if( cvt.n2 != 0 ) {
        memcpy( &buf[i], &stkbuf[cvt.n1], cvt.n2 );
        i += cvt.n2;
    }
    buf[i] = '\0';
    return( buf );
}
#endif
