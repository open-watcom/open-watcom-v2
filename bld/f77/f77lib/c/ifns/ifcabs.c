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
// IFCABS       : absolute value for COMPLEX argument
//

#include "fmath.h"

#include "ftnstd.h"
#include "ifenv.h"


single  CABS( single rp, single ip ) {
//====================================

// Return the absolute value of arg.

    rp = fabs( rp );
    ip = fabs( ip );
    if( rp > ip ) {
        ip = ip / rp;
        return( rp * sqrt( 1.0 + ip * ip ) );
    } else if( ip == 0.0 ) {
        return( rp );
    } else {
        rp = rp / ip;
        return( ip * sqrt( 1.0 + rp * rp ) );
    }
}

single  XCABS( complex *arg ) {
//=============================

    return( CABS( arg->realpart, arg->imagpart ) );
}
