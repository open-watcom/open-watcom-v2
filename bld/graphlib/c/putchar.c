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
* Description:  Text character output.
*
****************************************************************************/


#include "gdefn.h"
#if !defined( _DEFAULT_WINDOWS )
#include "gbios.h"
#include "filldef.h"
#endif


#if defined( _DEFAULT_WINDOWS )

void _PutChar( short row, short col, short ch )
//=============================================

// Display the given character on the screen. The point has already been
// clipped.

{
    WPI_PRES            dc;
    HFONT               font;
    HFONT               old_font;
    WPI_TEXTMETRIC      font_info;
    short               x, y, x2, y2, outy;
    short               char_height;
    short               font_height;
    grcolor             colour;
    short               prev_action;
    char                temp[2];
    WPI_COLOUR          old_bk, old_text;

    font = _GetSysMonoFnt();

    dc = _Mem_dc;
    old_font = _MySelectFont( dc, font );
    _wpi_gettextmetrics(dc, &font_info );

    char_height = _wpi_metricheight( font_info );
    font_height = _wpi_metricascent( font_info );

    // centre the character in given space
    x = col * _wpi_metricmaxcharwidth( font_info );
    y = row * char_height;

    if( ( _CharAttr & 0x80 ) && _CurrState->vc.numcolors != 256 ) {
        prev_action = _setplotaction( _GXOR );
    } else {
        _GrClear( x, y, x + _wpi_metricmaxcharwidth( font_info ), y + char_height );
    }
    colour = _CharAttr & ( _CurrState->vc.numcolors - 1 );

// Setup
    old_text = _wpi_settextcolor( dc, _Col2RGB( colour ) );
    old_bk   = _wpi_setbackcolour( dc, _CurrBkColor );

    temp[ 0 ] = ch;
    temp[ 1 ] = '\0';

    y2 = y + char_height;
    x2 = x + _wpi_metricmaxcharwidth( font_info );
    y = _wpi_cvth_y( y, _GetPresHeight() );
    y2 = _wpi_cvth_y( y2, _GetPresHeight() );
#if defined( __OS2__ )
    outy = y - font_height;
#else
    outy = y;
#endif
    _wpi_textout( dc, x, outy, temp, 1 );

// Cleanup
    if( ( _CharAttr & 0x80 ) && _CurrState->vc.numcolors != 256 ) {
        _setplotaction( prev_action );
    }
    _wpi_settextcolor( dc, old_text );
    _wpi_setbackcolour( dc, old_bk );
    _MyGetOldFont( dc, old_font );

// Invalidate the update region
    _MyInvalidate( x, y, x2, y2 );
}


#else

#define _HERC_HEIGHT    14
#define _DEFAULT_HEIGHT 8
#define _FONT_WIDTH     8


extern char _WCI86FAR   _HercFont[256][_HERC_HEIGHT];
extern char _WCI86FAR   _8x8BitMap;


void _PutChar( short row, short col, short ch )
//=============================================

// Display the given character on the screen. The point has already been
// clipped. In text mode, write directly to screen memory, in graphics
// modes, all characters are either drawn as 8x8 or 14x8 depending on
// the resolution.

{
    short               x, y;
    short               char_height;
    short               font_height;
    short               colour, space;
    short               prev_action;
    char far            *p;
    short far           *screen;
    char _WCI86FAR      *mask;
    gr_device _FARD     *dev_ptr;
    fill_fn near        *fill;

    if( IsTextMode ) {
        if( _CurrState->vc.mode == _TEXTMONO ) {
            p = MK_FP( _MonoSeg, _MonoOff );
        } else {
            p = MK_FP( _CgaSeg, _CgaOff );
        }
        p += _CurrActivePage * *(short far *)_BIOS_data( CRT_LEN );
        screen = (short far *) p;
        screen += row * _CurrState->vc.numtextcols + col;
        *screen = ( _CharAttr << 8 ) + ch;
    } else if( _IsDBCS ) {      // use BIOS for DBCS
        // set cursor position
        VideoInt( _BIOS_CURSOR_POSN, _CurrActivePage << 8, 0, ( row << 8 ) + col );
        // write character
        VideoInt( _BIOS_PUT_CHAR + ch, ( _CurrActivePage << 8 ) + _CharAttr, 1, 0 );
    } else {
        char_height = _CurrState->vc.numypixels / _CurrState->vc.numtextrows;
        if( char_height < _HERC_HEIGHT ) {
            font_height = _DEFAULT_HEIGHT;
            mask = &_8x8BitMap + _FONT_WIDTH * ch;
        } else {
            font_height = _HERC_HEIGHT;
            mask = &_HercFont[ ch ];
        }
        // centre the character in given space
        x = col * _FONT_WIDTH;
        y = row * char_height;
        space = char_height - font_height;
        if( space > 0 ) {
            y += space / 2;
        }
        if( ( _CharAttr & 0x80 ) && _CurrState->vc.numcolors != 256 ) {
            prev_action = _setplotaction( _GXOR );
        } else {
            _GrClear( x, y, x + _FONT_WIDTH - 1, y + char_height - 1 );
        }
        _StartDevice();
        dev_ptr = _CurrState->deviceptr;
        fill = dev_ptr->fill;
        colour = _CharAttr & ( _CurrState->vc.numcolors - 1 );
        if( space == 2 ) {      // duplicate top row
            ( *dev_ptr->setup )( x, y - 1, colour );
            ( *fill )( _Screen.mem, colour, *mask, _FONT_WIDTH, 0 );
        }
        for( row = 0; row < font_height; ++row, ++mask, ++y ) {
            ( *dev_ptr->setup )( x, y, colour );
            ( *fill )( _Screen.mem, colour, *mask, _FONT_WIDTH, 0 );
        }
        if( space == 2 ) {      // duplicate bottom row also
            ( *dev_ptr->setup )( x, y, colour );
            ( *fill )( _Screen.mem, colour, *(mask-1), _FONT_WIDTH, 0 );
        }
        _ResetDevice();
        if( ( _CharAttr & 0x80 ) && _CurrState->vc.numcolors != 256 ) {
            _setplotaction( prev_action );
        }
    }
}

#endif
