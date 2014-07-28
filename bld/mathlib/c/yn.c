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
* Description:  Implementation of yn() Bessel function.
*
****************************************************************************/


#include "variety.h"
#include "mathlib.h"
#include "pdiv.h"


_WMRTLINK double yn( int n, double x )
/**************************/
{
    int     j;
    double  by, bym, byp, tox;

    if( x < 0.0 ) {
        return __math1err( FP_FUNC_YN | M_DOMAIN | V_NEG_HUGEVAL, &x );
    }
    bym = y0( x );
    if( n == 0 )
        return( bym );
    tox = PDIV( 2.0, x );
    by = y1( x );
    for( j = 1; j < n; j++ ) {
        byp = j * tox * by - bym;
        bym = by;
        by = byp;
    }
    return( by );
}
