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
#include <math.h>
#include <limits.h>
#include "ifprag.h"
#include "rtdata.h"
#include "cplx.h"
#include "mathcode.h"


_WMRTLINK float _IF_powi( float base, long power )
/************************************************/
{
    return( _IF_dpowi( base, power ) );
}

_WMRTLINK double _IF_dpowi( double base, long power )
/***************************************************/
{
    double    result;

#if defined(_M_IX86)
    if( _RWD_real87 )
        return( pow( base, power ) );
#endif
    if( base == 0.0 ) {
        if( power <= 0 ) {
            result = power;
            result = __math2err( FUNC_DPOWI | M_DOMAIN | V_ZERO,
                                     &base, &result  );
        } else {
            result = 0.0;
        }
    } else {
        if( power < 0 ) {
            power = -power;
            base = 1 / base;
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
    }
    return( result );
}

_WMRTLINK double _IF_PowRI( double base, intstar4 power )
/*******************************************************/
{
    return( _IF_dpowi( base, power ) );
}
