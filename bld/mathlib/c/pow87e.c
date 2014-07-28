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
#include "mathlib.h"


double __pow87_err( double x, double y, unsigned char code )
/**********************************************************/
{
    unsigned int err_code;

    if( code == 0 ) {           /* x == 0.0  */
        if( y > 0.0 )
            return( 0.0 );
        if( y < 0.0 ) {
            err_code = FP_FUNC_POW | M_DOMAIN | V_HUGEVAL; /* 0.0 ** -ve */
        } else {
            err_code = FP_FUNC_POW | M_DOMAIN | V_ONE;     /* 0.0 ** 0.0 */
        }
    } else if( code == 1 ) {    /* negative ** fraction  */
        err_code = FP_FUNC_POW | M_DOMAIN | V_ZERO;        /* -ve ** frac*/
    } else {                    /* code == 2 */
        if( y <= 0.0 )
            return( 0.0 );
        if( x > 0.0 ) {
            err_code = FP_FUNC_POW | M_OVERFLOW | V_HUGEVAL;
        } else {
            err_code = FP_FUNC_POW | M_OVERFLOW | V_NEG_HUGEVAL;
        }
    }
    return( __math2err( err_code, &x, &y ) );
}
