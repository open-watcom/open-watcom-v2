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
// EXPCHK    : analyze floating point exponents
//

#include "ftnstd.h"

// If x is a real or double precision number, let xm be the magnitude of x.
// If xm is such that 10**d1 <= xm < 10**d2 the output is produced using
// 0PFw.d; otherwise 1PEw.dEe is used.
// We have chosen d1=3, d2=7, w=24, d=15 (double precision) or d=7 (real).


bool    ToFFormat( reallong val ) {
//=================================

// Check if floating point value can be displayed using F format.

    int         exp;

    exp = ((unsigned_16 *)(&val))[3] & 0x7ff0;
    if( exp == 0 ) return( TRUE );
    exp >>= 4;
    exp -= 0x03ff;      // unbias exponent
    // 2**19 < 10**7 < 2**20
    if( exp >= 20 ) return( FALSE );
    if( exp == 19 ) {
        // 1000000 is  0x412e848000000000
        if( ( ((unsigned_32 *)(&val))[1] & 0x7fffffff ) >= 0x412e8480 ) {
            return( FALSE );
        }
    }
    exp = -exp;
    if( exp >= 12 ) return( FALSE );
    return( TRUE );
}
