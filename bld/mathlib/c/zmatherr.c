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
#include "mathcode.h"
#undef __func__
#define __func__
#include "mathcode.h"
#include "cplx.h"

// z1 not used
_WMRTLINK complex __zmath1err( unsigned int err_info, complex *arg1 )
{
    return( __zmath2err( err_info, arg1, arg1 ) );
}

// z2 not used either... this module is not used!
_WMRTLINK complex __zmath2err( unsigned int err_info, complex *arg1, complex *arg2 )
{
    complex res;
    int     why;

    arg1 = arg1;
    arg2 = arg2;
    if(      err_info & M_DOMAIN   ) { why = DOMAIN;   }
    /* above is the only possible case...
    else if( err_info & M_SING     ) { why = SING;     }
    else if( err_info & M_OVERFLOW ) { why = OVERFLOW; }
    else if( err_info & M_UNDERFLOW) { why = UNDERFLOW;}
    else if( err_info & M_PLOSS    ) { why = PLOSS;    }
    else if( err_info & M_TLOSS    ) { why = TLOSS;    }*/
    __rterrmsg( why, MathFuncNames[ (err_info & FUNC_MASK) - 1 ] );
    if( err_info & V_ZERO ) {
        res.realpart = 0.0;
        res.imagpart = 0.0;
    } /* Also, above is the only case...
    else if( err_info & V_NEG_HUGEVAL ) {
        res.realpart = - HUGE_VAL;
        res.imagpart = - HUGE_VAL;
    } else if( err_info & V_ONE ) {
        res.realpart = 1.0;
        res.imagpart = 0.0;
    } else if( err_info & V_HUGEVAL ) {
        res.realpart = HUGE_VAL;
        res.imagpart = HUGE_VAL;
    } else { // PLOSS
        res = *arg2;
    }*/
    return( res );
}
