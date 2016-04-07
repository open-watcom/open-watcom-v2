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


//
// URAND        : uniform random number generator
//

#include "ftnstd.h"
#include "fmath.h"
#include "ftnapi.h"

#include <stddef.h>

static  intstar4        a = { 2147437301 };
static  intstar4        m = { -2147483648 };
static  intstar4        c = { 453816693 };


single  __fortran URAND( intstar4 *xn ) {
//=====================================

// Uniform random number generator based on techniques described
// in "The Art of Computer Programming", Vol.2, Knuth.
//
//       Xn+1 = a*Xn + c mod m
// where
//       Xn is the seed supplied by the caller
//       m = 2**31
//       a = 2147437301
//       c = 453816693
//
// 'a' satisfies the following:
//
//       max( sm, m/10 ) <  a  < m - sm, where sm = square root of m
//       a mod 8 = 5
//
// 'c' is computed as follows:
//
//       c = idnint( 2d0**31 * ( .5d0 - dsqrt( 3d0 ) / 6d0 ) ) + 1
//
// Note: Integer arithmetic is automatically done modulo 2**31.
//

    *xn = a * *xn + c;
    if( *xn < 0 ) {
        *xn = *xn + m;
    }
    return( *xn / pow( 2.0, 31 ) );
}
