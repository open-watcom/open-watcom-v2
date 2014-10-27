/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc.
*    Portions Copyright (c) 2014 Open Watcom contributors. 
*    All Rights Reserved.
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
* Description:  Error reporting convenience functions for complex arguments
*
****************************************************************************/


#include "variety.h"
#include "mathlib.h"
#include "_matherr.h"

_WMRTLINK complex __zmath1err( unsigned int err_info, complex *arg1 )
{
    return( __zmath2err( err_info, arg1, arg1 ) );
}

_WMRTLINK complex __zmath2err( unsigned int err_info, complex *arg1, complex *arg2 )
{
    complex res;
    int     why;

    arg1 = arg1;
    arg2 = arg2;
    if(      err_info & M_DOMAIN   ) { why = DOMAIN;   }
    
    __reporterrorsimple(why);
        
    res.realpart = 0.0;
    res.imagpart = 0.0;
    
    if( math_errhandling & MATH_ERRWATCOM ) {
        __rterrmsg( why, __rtmathfuncname( err_info & FP_FUNC_MASK ) );
        if( err_info & V_ZERO ) {
            res.realpart = 0.0;
            res.imagpart = 0.0;
        }
    }
    
    return( res );
}
