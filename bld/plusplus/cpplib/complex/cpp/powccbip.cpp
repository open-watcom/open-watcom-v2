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
#include <complex>
#include "cplx.h"

_WPRTLINK Complex pow( const Complex &base, int power ) {
/******************************************************/
    dcomplex result;
    result = _IF_C16PowI( base.real(), base.imag(), power );
    return Complex( result.realpart, result.imagpart );
}

#ifdef blah  // the way it was ...
    Complex     cresult;
    int         abs_power;
    Complex     base_2nd;       // "base" squared
    Complex     base_4th;       // "base" to the 4th power

    abs_power = abs( power );
    if( abs_power >= 2 ) {
        base_2nd = base * base;
        if( abs_power >= 4 ) {
            base_4th = base_2nd * base_2nd;
        }
    }
    switch( abs_power ) {
      case 0:
        return Complex( 1, 0 );
      case 1:
        cresult = base;
        break;
      case 2:
        cresult = base_2nd;
        break;
      case 3:
        cresult = base_2nd * base;
        break;
      case 4:
        cresult = base_4th;
        break;
      case 5:
        cresult = base_4th * base;
        break;
      case 6:
        cresult = base_4th * base_2nd;
        break;
      case 7:
        cresult = base_4th * base_2nd * base;
        break;
      case 8:
        cresult = base_4th * base_4th;
        break;
      default:
        return( pow( base, (double) power ) );
    }
    if( power < 0 ) {
        cresult = 1.0 / cresult;
    }
    return( cresult );
#endif
