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
* Description:  Low level pixel drawing routine.
*
****************************************************************************/


#include "gdefn.h"
#if !defined( _DEFAULT_WINDOWS )
#include "dotfunc.h"
#endif


short _L1PutDot( short x, short y )
/*=================================

    This function writes a pixel to the screen and returns the previous
    pixel value at that position, or -1 if the pixel is outside the
    viewport.   */

{
    short               prev_color;
#if defined( _DEFAULT_WINDOWS )
    WPI_COLOUR          color, RGB_Old;
    WPI_PRES            dc;
#else
    gr_device _FARD     *dev_ptr;
    get_dot_fn near     *getdot;
    put_dot_fn near     *putdot;

#endif

    if( _L1OutCode( x, y ) == 0 ) {             /* check if inside viewport */
#if defined( _DEFAULT_WINDOWS )
        dc = _Mem_dc;
        y = _wpi_cvth_y( y, _GetPresHeight() );
        RGB_Old = _wpi_getpixel( dc, x, y );    /* get the old color */
        prev_color = _RGB2Col( RGB_Old );       /* change it to our color index */
        color = _Col2RGB( _CurrColor );
        _wpi_setpixel( dc, x, y, color );               /* set the pixel */

// Update the window
        _MyInvalidate( x, y, x, y );
        _RefreshWindow();
#else
        _StartDevice();
        dev_ptr = _CurrState->deviceptr;
        ( *dev_ptr->setup )( x, y, _CurrColor );
        getdot = dev_ptr->getdot;
        prev_color = ( *getdot )( _Screen.mem, 0, _Screen.bit_pos );
        putdot = dev_ptr->plot[ _PlotAct ];
        ( *putdot )( _Screen.mem, _Screen.colour, _Screen.mask );
        _ResetDevice();
#endif
    } else {
        _ErrorStatus = _GRNOOUTPUT;
        prev_color = -1;
    }
    return( prev_color );
}
