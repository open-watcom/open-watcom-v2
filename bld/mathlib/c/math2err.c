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
#include <math.h>
#include "mathcode.h"
#include "mathlib.h"
#include "_matherr.h"


_WMRTLINK double __math1err( unsigned int err_info, double *arg1 )
{
    return( __math2err( err_info, arg1, arg1 ) );
}

_WMRTLINK double __math2err( unsigned int err_info, double *arg1, double *arg2 )
{
    int                 why;
    struct exception    exc;

    exc.arg1 = *arg1;
    exc.arg2 = *arg2;
    if(      err_info & M_DOMAIN   ) { why = DOMAIN;   }
    else if( err_info & M_SING     ) { why = SING;     }
    else if( err_info & M_OVERFLOW ) { why = OVERFLOW; }
    else if( err_info & M_UNDERFLOW) { why = UNDERFLOW;}
    else if( err_info & M_PLOSS    ) { why = PLOSS;    }
    else if( err_info & M_TLOSS    ) { why = TLOSS;    }
    exc.type = why;
    exc.name = __rtmathfuncname( err_info & FUNC_MASK );
    if(      err_info & V_NEG_HUGEVAL ) { exc.retval = - HUGE_VAL; }
    else if( err_info & V_ZERO        ) { exc.retval = 0.0;        }
    else if( err_info & V_ONE         ) { exc.retval = 1.0;        }
    else if( err_info & V_HUGEVAL     ) { exc.retval = HUGE_VAL;   }
    else  /* PLOSS from sin,cos,tan */  { exc.retval = *arg2;      }

    return( _matherr( &exc ) );
}
