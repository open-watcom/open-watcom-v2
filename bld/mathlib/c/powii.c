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
#include <stddef.h>
#include <limits.h>
#include "mathlib.h"
#include "ifprag.h"
#include "cplx.h"


_WMRTLINK long _IF_ipow( long base, long power )
/**********************************************/
{

    long    result;

    if( base == 0 ) {
        if( power <= 0 ) {
            double  dummy;

            dummy = 0.0;
//              used to be like this:
//              result = _matherr( DOMAIN, "ipow", NULL, NULL, 1.0 );
//              now it is like this:
            result = __math1err( FUNC_IPOW | M_DOMAIN | V_ONE, &dummy );
//              should be more like this:
//              result = __imath2err( FUNC_POW | M_DOMAIN | V_ZERO, &base, &power );
        } else {
            result = 0;
        }
    } else if( power < 0 ) {
        if( base == 1 ) {
            result = 1;
        } else if( base == -1 ) {
            if( power & 1 ) {
                result = -1;
            } else {
                result = 1;
            }
        } else {
            result = 0;
        }
    } else {
        result = 1;
        while( power > 0 ) {
            if( power & 1 ) {
                result *= base;
                --power;
            } else {
                base *= base;
                power /= 2;
            }
        }
    }
    return( result );
}


_WMRTLINK intstar4 _IF_powii( intstar4 base, intstar4 power )
/***********************************************************/
{
    return( _IF_ipow( base, power ) );
}
