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
* Description:  Hyperbolic tangent routine.
*
****************************************************************************/


#include "variety.h"
#include <math.h>
#include "mathlib.h"
#include "ifprag.h"
#include "pdiv.h"


_WMRTLINK float _IF_tanh( float x )
/*********************************/
{
    return( _IF_dtanh( x ) );
}

_WMRTLINK double (tanh)( double x )
/*********************************/
{
    return( _IF_dtanh( x ) );
}

_WMRTLINK double _IF_dtanh( double x )
/************************************/
{
    double  z;

    z = fabs( x );
    if( z >= 64.0 ) {
        if( x < 0.0 ) {
            z = -1.0;
        } else {
            z = 1.0;
        }
    } else {
/*          if( z <= ldexp( 1.0, -26 ) ) { */
        if( z <= 1.49011611938476580e-008 ) { /* if x is small */
            z = x;
        } else {
            z = exp( (-2.0) * x );
            z = PDIV( (1.0 - z), (1.0 + z) );
        }
    }
    return( z );
}
