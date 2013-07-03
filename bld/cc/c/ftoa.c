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
* Description:  convert float binary format to string representation
*
****************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include "cvars.h"

#define MAX_DIGIT 21

static char     buf[80];

char *ftoa( FLOATVAL *flt )
{
#ifdef __WATCOMC__
    CVT_INFO    cvt;
    char        mant[MAX_DIGIT + 1];

    cvt.flags = G_FMT + F_CVT + NO_TRUNC + LONG_DOUBLE;
    cvt.scale = 1;
    cvt.ndigits = MAX_DIGIT;
    cvt.expwidth = 0;
    cvt.expchar  = 0;
    __LDcvt( &flt->ld, &cvt, mant );
    if( !isdigit( *mant ) ) {
        /* special magical thingy (nan, inf, ...) */
        strcpy( buf, mant );
        return( buf );
    }
    if( *mant != '0' )
        --cvt.decimal_place;
    sprintf( buf, "%c%c.%sE%+1d", 
        ( cvt.sign ) ? '-' : '+', *mant, mant + 1, cvt.decimal_place );
#else
  #ifdef _LONG_DOUBLE_
    double      dbl;
    long_double ld;
    ld = flt->ld;
    __iLDFD( &ld, &dbl );
    sprintf( buf, "%.19e", dbl );
  #else
    sprintf( buf, "%.19e", flt->ld );
  #endif
#endif
    return( buf );
}
