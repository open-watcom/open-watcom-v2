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
// IFDLGAMA     : log of gamma function for DOUBLE PRECISION argument
//

#include "fmath.h"
#include <float.h>

#include "ftnstd.h"
#include "ifenv.h"

extern  extended          __lgamma(extended,const extended __FAR *);

static  const extended __FAR  xinf  = { DBL_MAX };
static  const double __FAR  big   = { 1.28118e305 };

double  DLGAMA( double arg ) {
//============================

    if( fabs( arg ) >= big ) return( xinf );
    if( arg == 0.0 ) return( xinf );
    return( __lgamma( arg, &xinf ) );
}

double XDLGAMA( double *arg ) {
//=============================

    return( DLGAMA( *arg ) );
}
