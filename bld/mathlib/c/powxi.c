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
#include "cplx.h"
#include "mathcode.h"
#include "pdiv.h"


_WMRTLINK extended _IF_PowXI( extended base, intstar4 power )
//===========================================================
{
// Return base ** power where power could be <= 0.

    extended    result;

    if( base == 0.0 ) {
        if( power <= 0 ) {
            result = power;
            return( __math2err( FUNC_DPOWI | M_DOMAIN | V_ZERO, &base, &result ) );
        } else {
            return( 0.0 );
        }
    } else {
        if( power < 0 ) {
            power = -power;
            base = PDIV( 1.0, base );
        }
        result = 1.0;
        while( power > 0 ) {
            if( power % 2 == 0 ) {
                base *= base;
                power /= 2;
            } else {
                result *= base;
                --power;
            }
        }
        return( result );
    }
}
