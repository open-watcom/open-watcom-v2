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
// IFGAMMA      : gamma function for REAL argument
//

#include "fmath.h"
#include <float.h>

#include "ftnstd.h"
#include "ifenv.h"
#include "ifqgamma.h"


static  const extended __FAR    xinf = { FLT_MAX };
static  const single __FAR      xmin = { FLT_MIN };
static  const single __FAR      big = { 34.844 };


single GAMMA( single arg ) {
//==========================

    if( fabs( arg ) <= xmin ) {
        if( arg <= 0.0 ) {
            return( -xinf );
        } else {
            return( xinf );
        }
    } else if( fabs( arg ) >= big ) {
        return( xinf );
    }
    return( __gamma( arg, &xinf ) );
}

single XGAMMA( single *arg ) {
//============================

    return( GAMMA( *arg ) );
}
