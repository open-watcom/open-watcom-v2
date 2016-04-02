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
* Description:  run/compile-time subscripting
*
****************************************************************************/


#include "ftnstd.h"
#include "symdefs.h"
#include "subscr.h"


bool    DoSubscript( act_dim_list *dims, intstar4 *subscrs, intstar4 *res )
// Do subscript operation for EQUIVALENCE or DATA statements and
// NAMELIST-directed i/o at run-time.
{
    int         dims_no;
    intstar4    offset;
    intstar4    multiplier;
    intstar4    ss;
    intstar4    lo;
    intstar4    hi;
    intstar4    *bounds;

    *res = 0;
    dims_no = _DimCount( dims->dim_flags );
    bounds = &dims->subs_1_lo;
    multiplier = 1;
    offset = 0;
    for(;;) {
        ss = *subscrs;
        subscrs++;
        lo = *bounds;
        bounds++;
        hi = *bounds;
        bounds++;
        if( ss < lo ) return( FALSE );
        if( ss > hi ) return( FALSE );
        offset += ( ss - lo ) * multiplier;
        if( offset < 0 ) return( FALSE );
        if( offset > dims->num_elts ) return( FALSE );
        dims_no--;
        if( dims_no == 0 ) break;
        multiplier *= ( hi - lo + 1 );
    }
    *res = offset;
    return( TRUE );
}
