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
// IFCOTAN      : cotangent function for REAL argument
//


#include "fmath.h"

#include "ftnstd.h"
#include "ifenv.h"
#include "errcod.h"
#include "fmthcode.h"


#if !defined( __alternate_if__ )

single COTAN( single arg ) {
//==========================

// Return the cotangent of arg.

    single      tangent;

    tangent = tan( arg );
    if( tangent == 0.0 ) {
        return( __amath1err( FP_FUNC_COTAN | M_OVERFLOW | V_HUGEVAL, &arg ) );
    }
    return( 1.0 / tangent );
}

#endif


single XCOTAN( single *arg ) {
//============================

    return( COTAN( *arg ) );
}
