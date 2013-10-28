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
* Description:  Scrolling routines.
*
****************************************************************************/


#include "gdefn.h"
#if !defined( _DEFAULT_WINDOWS )
#include <malloc.h>
#include "gbios.h"
#include "stkavail.h"
#include "picdef.h"
#endif


#if defined( _DEFAULT_WINDOWS ) && defined( __OS2__ )

static void ScrollDC( WPI_PRES dc, short dx, short dy, WPI_RECT *scroll,
/*==================*/WPI_RECT *clip, HRGN rgn, WPI_RECT *update )

/* This function is defined in Windows but not OS/2. */
{
    HRGN        temprgn;
    WPI_RECT    rect;
    short       height, width;
    WPI_RECTDIM sx1, sy1, sx2, sy2;

    update = update;  // We don't care about these since we are not using it
    rgn = rgn;

    temprgn = _ClipRgn;
    _ClipRgn = _wpi_createrectrgn( dc, clip );
    _wpi_getclipbox( dc, &rect);
    _wpi_selectcliprgn( dc, _ClipRgn );
    _wpi_deletecliprgn( dc, temprgn );
    _wpi_getwrectvalues( *scroll, &sx1, &sy1, &sx2, &sy2 );
    width = _wpi_getwidthrect( *scroll );
    height = _wpi_getheightrect( *scroll );

    _wpi_bitblt( dc, sx1 + dx , sy1 + dy,
                 width, height,
                 dc, sx1, sy1,
                 SRCCOPY );

    temprgn = _ClipRgn;
    _ClipRgn = _wpi_createrectrgn( dc, &rect );
    _wpi_selectcliprgn( dc, _ClipRgn );
    _wpi_deletecliprgn( dc, temprgn );
}

#endif


#if !defined( _DEFAULT_WINDOWS )
static void GrShift( short src_y, short dst_y,
                     short dir, short rows, short xl, short xr )
//==============================================================

{
    short               width;
    short               size;
    short               plane_len;
    short               prev_action;
    char                *buf;
    gr_device _FARD     *dev_ptr;
    copy_fn             *get;
    copy_fn             *put;
    setup_fn            *setup;

    width = xr - xl + 1;
    size = _RoundUp( _RowLen( width ) );
    if( _stackavail() - size > 0x100 ) {
        buf = __alloca( size );
    } else {            /* not enough memory to proceed */
        _ErrorStatus = _GRINSUFFICIENTMEMORY;
        return;
    }
    prev_action = _setplotaction( _GPSET );

    _StartDevice();

    dev_ptr = _CurrState->deviceptr;
    get = dev_ptr->readrow;
    put = dev_ptr->pixcopy;
    setup = dev_ptr->setup;

    plane_len = size / _CurrState->vc.bitsperpixel;

    while( rows != 0 ) {
        ( *setup )( xl, src_y, 0 );
        ( *get )( buf, _Screen.mem, width, _Screen.bit_pos, 0 );
        ( *setup )( xl, dst_y, 0 );
        ( *put )( _Screen.mem, buf, width, _Screen.bit_pos << 8, plane_len );
        src_y += dir;
        dst_y += dir;
        --rows;
    }

    _ResetDevice();
    _setplotaction( prev_action );
}


static void TxtShift( short src_y, short dst_y,
                      short dir, short rows, short xl, short xr )
//===============================================================

{
    char __far          *p;
    short __far         *src;
    short __far         *dst;
    short               width;

    if( _CurrState->vc.mode == _TEXTMONO ) {
        p = MK_FP( _MonoSeg, _MonoOff );
    } else {
        p = MK_FP( _CgaSeg, _CgaOff );
    }
    p += _CurrActivePage * *(short __far *)_BIOS_data( CRT_LEN );
    src = (short __far *) p;
    src += src_y * _CurrState->vc.numtextcols + xl;
    dst = (short __far *) p;
    dst += dst_y * _CurrState->vc.numtextcols + xl;

    dir *= _CurrState->vc.numtextcols;
    width = ( xr - xl + 1 ) * 2;        // char & attr
    while( rows != 0 ) {
        _fmemcpy( dst, src, width );
        src += dir;
        dst += dir;
        --rows;
    }
}
#endif


void _ScrollWindow( short dir, short rows )
//=========================================

//  Scroll the text window by the given number of rows in the given
//  direction.

{
#if defined( _DEFAULT_WINDOWS )
    WPI_PRES            dc;
    HFONT               font;
    HFONT               old_font;
    WPI_RECT            Rect1, Rect2;
    WPI_TEXTMETRIC      font_info;
    short               y1;
    short               y2;
#else
    short               total_rows;
#endif
    short               char_height;
    short               char_width;
    short               y_top;
    short               y_bot;
    short               x_left;
    short               x_right;

#if defined( _DEFAULT_WINDOWS )
// get the font's information
    font = _GetSysMonoFnt();
    dc = _Mem_dc;
    old_font = _MySelectFont( dc, font );
    _wpi_gettextmetrics(dc, &font_info );

// Calculate how much to move
    char_height = _wpi_metricheight( font_info );
    char_width =  _wpi_metricmaxcharwidth( font_info );
    y_top = _Tx_Row_Min * char_height;
    y_bot = _Tx_Row_Max * char_height + char_height - 1;
    x_left = _Tx_Col_Min * char_width;
    x_right = _Tx_Col_Max * char_width + char_width - 1;
    rows *= char_height;
    y1 = _wpi_cvth_y( y_top, _GetPresHeight() );
    y2 = _wpi_cvth_y( y_bot, _GetPresHeight() );

    if( rows != 0 ) {
        _wpi_setrectvalues( &Rect1, x_left, y1, x_right, y2 );
        _wpi_setrectvalues( &Rect2, x_left, 0, x_right + 1, _GetPresHeight() );
        if( dir != _SCROLL_DOWN ) {
            rows = -rows;
        }
#if defined( __OS2__ )
        rows = -rows;
#endif
        ScrollDC( dc, 0, rows, &Rect1, &Rect2, NULL, NULL );
#if defined( __OS2__ )
        rows = -rows;
#endif
    }

    if( dir == _SCROLL_DOWN ) {
        _GrClear( x_left, y_top, x_right, y_top + rows - 1 );
    } else {
        _GrClear( x_left, y_bot - ( rows - 1 ), x_right, y_bot );
    }
    _MyGetOldFont( dc, old_font );

// Update the window
    _MyInvalidate( x_left, y1, x_right, y2 );
    _RefreshWindow();

#else
    if( IsTextMode ) {
        total_rows = _Tx_Row_Max - _Tx_Row_Min + 1;
        if( rows != 0 ) {
            if( dir == _SCROLL_DOWN ) {
                TxtShift( _Tx_Row_Max - rows, _Tx_Row_Max, -1, total_rows - rows, _Tx_Col_Min, _Tx_Col_Max );
            } else {
                TxtShift( _Tx_Row_Min + rows, _Tx_Row_Min, +1, total_rows - rows, _Tx_Col_Min, _Tx_Col_Max );
            }
        }
        if( dir == _SCROLL_DOWN ) {
            _TxtClear( _Tx_Row_Min, _Tx_Col_Min, _Tx_Row_Min + rows - 1, _Tx_Col_Max );
        } else {
            _TxtClear( _Tx_Row_Max - ( rows - 1 ), _Tx_Col_Min, _Tx_Row_Max, _Tx_Col_Max );
        }
    } else {
        char_height = _CurrState->vc.numypixels / _CurrState->vc.numtextrows;
        char_width = _CurrState->vc.numxpixels / _CurrState->vc.numtextcols;
        y_top = _Tx_Row_Min * char_height;
        y_bot = _Tx_Row_Max * char_height + char_height - 1;
        x_left = _Tx_Col_Min * char_width;
        x_right = _Tx_Col_Max * char_width + char_width - 1;
        total_rows = ( y_bot - y_top + 1 );
        rows *= char_height;
        if( rows != 0 ) {
            if( dir == _SCROLL_DOWN ) {
                GrShift( y_bot - rows, y_bot, -1, total_rows - rows, x_left, x_right );
            } else {
                GrShift( y_top + rows, y_top, +1, total_rows - rows, x_left, x_right );
            }
        }
        if( dir == _SCROLL_DOWN ) {
            _GrClear( x_left, y_top, x_right, y_top + rows - 1 );
        } else {
            _GrClear( x_left, y_bot - ( rows - 1 ), x_right, y_bot );
        }
    }
#endif
}
