/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Routines to set number of text rows.
*
****************************************************************************/


#include <conio.h>
#include "gdefn.h"
#include "realmod.h"
#include "gbios.h"


#if !defined( _DEFAULT_WINDOWS )

#define TXT_CURS_8x8        0x0607
#define TXT_CURS_8x14       0x0B0C
#define TXT_CURS_EMU        0x0600

#define VIDEOINT_CHARGEN_SET_BLOCK  0x1103

#define VIDEOINT_SET_TXT_FONT_8x14  0x1111
#define VIDEOINT_SET_TXT_FONT_8x8   0x1112
#define VIDEOINT_SET_GR_FONT_8x14   0x1122
#define VIDEOINT_SET_GR_FONT_8x8    0x1123
#define VIDEOINT_SET_GR_FONT_8x16   0x1124

#define VIDEOINT_SCAN_200   0x1200
#define VIDEOINT_SCAN_350   0x1201
#define VIDEOINT_SCAN_400   0x1202

#endif

#if defined( _DEFAULT_WINDOWS )

short _SetRows( short rows )
/*==========================

    This function has no effect on Windows.  It only re-initializes
    the variable states. */
{
    _ErrorStatus = _GROK;
    _InitState();           // read the current machine state
    rows = rows;
    _clearscreen( _GCLEARSCREEN );
    _Tx_Row_Min = 0;                            // text window is now
    _Tx_Col_Min = 0;                            // the full screen
    _Tx_Row_Max = _CurrState->vc.numtextrows - 1;
    _Tx_Col_Max = _CurrState->vc.numtextcols - 1;
    _TextPos.row = 0;                           // set mode function
    _TextPos.col = 0;                           // sets position to 0,0
    return( 0 );
}

#else

static void GrModeRows( short rows )
//==================================

{
    short           set_font;

    switch( _CurrState->vc.mode ) {
    case _ERESNOCOLOR:
    case _ERESCOLOR:
        switch( rows ) {
        case 25:
            set_font = VIDEOINT_SET_GR_FONT_8x14;  // 8x14
            break;
        case _MAXTEXTROWS:
            rows = 43;
            /* fall through */
        case 43:
            set_font = VIDEOINT_SET_GR_FONT_8x8;   // 8x8
            break;
        default:
            _ErrorStatus = _GRINVALIDPARAMETER;
            return;
        }
        break;
    case _VRES2COLOR:
    case _VRES16COLOR:
        switch( rows ) {
        case 30:
            set_font = VIDEOINT_SET_GR_FONT_8x16;  // 8x16
            break;
        case 34:
            set_font = VIDEOINT_SET_GR_FONT_8x14;  // 8x14
            break;
        case _MAXTEXTROWS:
            rows = 60;
            /* fall through */
        case 60:
            set_font = VIDEOINT_SET_GR_FONT_8x8;   // 8x8
            break;
        default:
            _ErrorStatus = _GRINVALIDPARAMETER;
            return;
        }
        break;
    // 25 rows for all other graphics modes
    default:
        if (rows != 25) {
            _ErrorStatus = _GRERROR;
        }
        return;
    }
    VideoInt( VIDEOINT_SET_MODE + GetVideoMode(), 0, 0, 0 );
    // Load pointer to character set
    if( _CurrState->vc.adapter == _EGA ) {
        VideoInt( set_font, 0, 0, rows - 1 );   // do this only for the EGA
    } else {
        VideoInt( set_font, 0, 0, rows );
    }
    _GrCursor = 0;                          // cursor is off
}


static void Load_25( void )
/*=========================
 * When we want to go back to a 25-row display, just do a set mode
 * and update a few variables.
 */
{
    VideoInt( VIDEOINT_SET_MODE + GetVideoMode(), 0, 0, 0 );
    BIOSData( BDATA_VIDEO_INFO_0, unsigned char ) &= ~0x01;              // 43 line mode cursor emulation off
    VideoInt( VIDEOINT_CURSOR_SIZE, 0, 0x0607, 0 );    // reset the cursor
    _GrCursor = 1;                                  // cursor is on
}


static void Load_VGA( short set_scan, short set_font, short cursor )
/*==================================================================
 * This routines loads an alphanumeric font for the VGA.
 */
{
    VideoInt( set_scan, 0x0030, 0, 0 );             // becomes effective at next set mode
    VideoInt( VIDEOINT_SET_MODE + GetVideoMode(), 0, 0, 0 );
    VideoInt( VIDEOINT_VIDEO_PAGE, 0, 0, 0 );       // set active page to 0
    VideoInt( set_font, 0, 0, 0 );                  // load character set
    VideoInt( VIDEOINT_CURSOR_SIZE, 0, cursor, 0 ); // reset the cursor
    _GrCursor = 1;                                  // cursor is on
}


static void Load_EGA( short rows, short set_font, short cursor )
/*==============================================================
 * This routines loads an alphanumeric font for the EGA.
 */
{
    VideoInt( VIDEOINT_VIDEO_PAGE, 0, 0, 0 );       // set active page to 0
    VideoInt( VIDEOINT_SET_MODE + GetVideoMode(), 0, 0, 0 );
    VideoInt( set_font, 0, 0, 0 );                  // load pointer to character set in block 0
    if( rows == 43 ) {                              // cursor emulation
        BIOSData( BDATA_VIDEO_INFO_0, unsigned char ) |= 1; // 43 line mode cursor emulation on
    } else {
        outpw( 0x03D4, 0x1414 );                    // reset underline location to none
    }
    VideoInt( VIDEOINT_CURSOR_SIZE, 0, cursor, 0 );    // reset the cursor
    _GrCursor = 1;                                  // cursor is on
}


static void Load_MCGA( short rows, short set_font, short cursor )
/*===============================================================
 * This routines loads an alphanumeric font for the MCGA.
 */
{
    VideoInt( VIDEOINT_VIDEO_PAGE, 0, 0, 0 );               // set active page to 0
    VideoInt( VIDEOINT_SET_MODE + GetVideoMode(), 0, 0, 0 );
    _fmemset( _MK_FP( _EgaSeg, _EgaOff ), 0, 0x2000 );      // must do for MCGA 40 rows
    VideoInt( set_font & 0xFF0F, 0, 0, 0 );                 // load character set
    VideoInt( VIDEOINT_CHARGEN_SET_BLOCK, 0, 0, 0 );
    VideoInt( VIDEOINT_CURSOR_SIZE, 0, cursor, 0 );         // reset the cursor
    outpw( 0x03D4, ( cursor & 0xFF00 ) + 0x09 );            // # double scan lines
    BIOSData( BDATA_VIDEO_ROWS, unsigned char ) = rows - 1; // # of rows
    // # of vertical points per character
    BIOSData( BDATA_POINT_HEIGHT, unsigned short ) = 2 * ( cursor & 0xFF + 1 );
    _GrCursor = 1;                                          // cursor is on
}


static void TextModeRows( short rows )
//====================================

{
    if( rows == _MAXTEXTROWS ) {
        switch( _CurrState->vc.adapter ) {
        case _MCGA:
        case _VGA:
        case _SVGA:
            rows = 50;
            break;
        case _EGA:
            if( _CurrState->vc.monitor == _COLOR ) {
                rows = 25;
            } else {        // enhanced colour
                rows = 43;
            }
            break;
        default:
            return;     // can't be changed
        }
    }
    if( rows == 25 ) {
        Load_25();
        return;
    }
    switch( _CurrState->vc.adapter ) {      // find a text mode
    case _VGA:
    case _SVGA:
        switch( rows ) {
        case 28:
            Load_VGA( VIDEOINT_SCAN_400, VIDEOINT_SET_TXT_FONT_8x14, TXT_CURS_8x14 );
            break;
        case 43:
            Load_VGA( VIDEOINT_SCAN_350, VIDEOINT_SET_TXT_FONT_8x8, TXT_CURS_8x8 );
            break;
        case 50:
            Load_VGA( VIDEOINT_SCAN_400, VIDEOINT_SET_TXT_FONT_8x8, TXT_CURS_8x8 );
            break;
        default:
            _ErrorStatus = _GRINVALIDPARAMETER;
        }
        break;
    case _EGA:
        if( _CurrState->vc.monitor == _COLOR ) {
            switch( rows ) {            // 200 scan lines
            case 14:
                Load_EGA( rows, VIDEOINT_SET_TXT_FONT_8x14, TXT_CURS_8x14 );
                break;
            default:
                _ErrorStatus = _GRINVALIDPARAMETER;
            }
        } else {                        // EGA Mono or Enhanced
            switch( rows ) {
            case 43:                    // 350 scan lines
                Load_EGA( rows, VIDEOINT_SET_TXT_FONT_8x8, TXT_CURS_EMU );
                break;
            default:
                _ErrorStatus = _GRINVALIDPARAMETER;
            }
        }
        break;
    case _MCGA:
        switch( rows ) {
        case 40:
            Load_MCGA( rows, VIDEOINT_SET_TXT_FONT_8x8, 0x0404 );
            break;
        case 50:
            Load_MCGA( rows, VIDEOINT_SET_TXT_FONT_8x8, 0x0303 );
            break;
        default:
            _ErrorStatus = _GRINVALIDPARAMETER;
        }
        break;
    default:
        _ErrorStatus = _GRERROR;
    }
}

short _SetRows( short rows )
/*==========================

   This function sets the number of text rows in the current mode. It only
   affects the _VGA, _EGA, and _MCGA adapters.  */

{
    _ErrorStatus = _GROK;
    _InitState();           // read the current machine state
    if( _GrMode ) {
        GrModeRows( rows );
    } else {
        TextModeRows( rows );
    }
    if( _ErrorStatus != _GROK ) {
        return( 0 );
    } else {
        rows = BIOSData( BDATA_VIDEO_ROWS, unsigned char ) + 1;   // 0 for Hercules
        if( rows == 1 )
            rows = 25;
        _CurrState->vc.numtextrows = rows;
        if( !_GrMode ) {
            _CalcNumPages();                        // update the video configuration
        }
        _Tx_Row_Min = 0;                            // text window is now
        _Tx_Col_Min = 0;                            // the full screen
        _Tx_Row_Max = _CurrState->vc.numtextrows - 1;
        _Tx_Col_Max = _CurrState->vc.numtextcols - 1;
        _TextPos.row = 0;                           // set mode function
        _TextPos.col = 0;                           // sets position to 0,0
        _CurrVisualPage = 0;
        _CurrActivePage = 0;
        VideoInt( VIDEOINT_VIDEO_PAGE, 0, 0, 0 );   // set to page 0
        return( _CurrState->vc.numtextrows );
    }
}

#endif

_WCRTLINK short _WCI86FAR _CGRAPH _settextrows( short rows )
/*===========================================

   This function sets the number of text rows in the current mode. It only
   affects the _VGA, _EGA, and _MCGA adapters.  */

{
    short               set_rows;

    set_rows = _SetRows( rows );
    #if !defined( _DEFAULT_WINDOWS )
        _PaletteInit();   // need to reset palette after changing mode
    #endif
    return( set_rows );
}

Entry1( _SETTEXTROWS, _settextrows ) // alternate entry-point



_WCRTLINK short _WCI86FAR _CGRAPH _setvideomoderows( short mode, short rows )
/*============================================================

   This routine sets the video mode if it is supported by the current
   hardware configuration, then sets the number of text rows requested. It
   returns the number of text rows if successful, otherwise it returns 0.   */

{
    if( _setvideomode( mode ) ) {
        return( _settextrows( rows ) );
    }
    return( 0 );
}

Entry1( _SETVIDEOMODEROWS, _setvideomoderows ) // alternate entry-point
