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
#include "ifprag.h"
#include "mathcode.h"
#include "pdiv.h"

_WMRTLINK extern double _IF_dcosh( double );

#if defined(_M_IX86)
  #pragma aux (if_rtn) _IF_cosh "IF@COSH";
  #pragma aux (if_rtn) _IF_dcosh "IF@DCOSH";
#endif

_WMRTLINK float _IF_cosh( float x )
/*********************/
    {
        return( _IF_dcosh( x ) );
    }

_WMRTLINK double (cosh)( double x )
/***********************/
    {
        return( _IF_dcosh( x ) );
    }

_WMRTLINK double _IF_dcosh( double x )
/*************************/
    {
        double z;

        z = fabs( x );
        if( z > 709.782712893384 ) {            /* if argument is too large */
//            z = _matherr( OVERFLOW, "cosh", &x, &x, HUGE_VAL );
            z = __math1err( FUNC_COSH | M_OVERFLOW | V_HUGEVAL, &x );
        } else {
/*          if( z <= ldexp( 1.0, -26 ) ) { */
            if( z <= 1.49011611938476580e-008 ) { /* if x is small */
                z = 1.0;
            } else {
                z = exp( x );
                z = (z + PDIV( 1.0 , z) ) / 2.0;
            }
        }
        return( z );
    }
