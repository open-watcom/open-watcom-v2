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


#include "gdefn.h"


short _WCI86FAR _CGRAPH _getplotaction( void )
/*=======================================

   This function queries the plotting action for line drawing, area
   filling, and image retreival. */

{
    switch( _PlotAct ) {
    default :
    case 0 :
        return( _GPSET );
    case 1 :
        return( _GXOR );
    case 2 :
        return( _GAND );
    case 3 :
        return( _GOR );
    }
}

Entry( _GETPLOTACTION, _getplotaction ) // alternate entry-point


short _WCI86FAR _CGRAPH _setplotaction( short action )
/*===============================================

   This routine sets the plotting action for line drawing, area filling,
   and image retreival. */

{
    short               prev;           /* previous mode */

    prev = _getplotaction();
    switch( action ) {
    case _GXOR :
        _PlotAct = 1;
        break;
    case _GAND :
        _PlotAct = 2;
        break;
    case _GOR :
        _PlotAct = 3;
        break;
    case _GPSET :
    case _GPRESET :
    default :
        _PlotAct = 0;
    }
#if defined( _DEFAULT_WINDOWS )
    _wpi_setrop2( _Mem_dc, _MapPlotAct() );
#endif
    return( prev );
}

Entry( _SETPLOTACTION, _setplotaction ) // alternate entry-point
