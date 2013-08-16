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
* Description:  Low level pixel reading routine.
*
****************************************************************************/


#include "gdefn.h"


short _L1GetDot( short x, short y )
/*=================================

    This function returns the color of the screen pixel at (x,y)    */

{
    short               color;
#if defined( _DEFAULT_WINDOWS )
    WPI_COLOUR          RGB_color;
#else
    gr_device _FARD     *dev_ptr;
    getdot_fn           *getdot;
#endif

    if( _L1OutCode( x, y ) == 0 ) {             /* check if inside viewport */
#if defined( _DEFAULT_WINDOWS )
        y = _wpi_cvth_y( y, _GetPresHeight() );
        RGB_color = _wpi_getpixel( _Mem_dc, x, y );
        color = _RGB2Col( RGB_color );          // Convert back to our color
#else
        _StartDevice();

        dev_ptr = _CurrState->deviceptr;
        ( *dev_ptr->setup )( x, y, 0 );
        getdot = dev_ptr->getdot;
        color = ( *getdot )( _Screen.mem, _Screen.bit_pos );

        _ResetDevice();
#endif
    } else {
        _ErrorStatus = _GRNOOUTPUT;
        color = -1;
    }
    return( color );
}
