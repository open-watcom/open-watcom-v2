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

_WMRTLINK extern double _IF_dsinh( double );
#if defined(_M_IX86)
  #pragma aux (if_rtn) _IF_sinh "IF@SINH";
  #pragma aux (if_rtn) _IF_dsinh "IF@DSINH";
#endif

_WMRTLINK float _IF_sinh( float x )
/*********************/
    {
        return( _IF_dsinh( x ) );
    }

_WMRTLINK double (sinh)( double x )
/***********************/
    {
        return( _IF_dsinh( x ) );
    }

_WMRTLINK double _IF_dsinh( double x )
/*************************/
    {
        unsigned int err_code;
        double z;

        z = fabs( x );
        if( z > 709.782712893384 ) {            /* if argument is too large */
//            z = ( x < 0.0 ) ? - HUGE_VAL : HUGE_VAL;
//            z = _matherr( OVERFLOW, "sinh", &x, &x, z );
            if( x < 0.0 ) {
                err_code = FUNC_SINH | M_OVERFLOW | V_NEG_HUGEVAL;
            } else {
                err_code = FUNC_SINH | M_OVERFLOW | V_HUGEVAL;
            }
            z = __math1err( err_code, &x );
        } else {
/*          if( z <= ldexp( 1.0, -26 ) ) { */
            if( z <= 1.49011611938476580e-008 ) { /* if x is small */
                z = x;
            } else {
                z = exp( x );
                z = (z - PDIV( 1.0 , z )) / 2.0;
            }
        }
        return( z );
    }
