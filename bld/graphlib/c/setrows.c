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
* Description:  Routines to set number of text rows.
*
****************************************************************************/


#include <conio.h>
#include "gdefn.h"
#include "gbios.h"


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

#define TX_CURS_8x8     0x0607
#define TX_CURS_8x14    0x0B0C
#define TX_CURS_EMU     0x0600

#define TX_FONT_8x8     0x1112
#define TX_FONT_8x14    0x1111

#define GR_FONT_8x8     0x1123
#define GR_FONT_8x14    0x1122
#define GR_FONT_8x16    0x1124

#define SCAN_200        0x1200
#define SCAN_350        0x1201
#define SCAN_400        0x1202


static void             Load_25( void );
static void             Load_VGA( short, short, short );
static void             Load_EGA( short, short, short );
static void             Load_MCGA( short, short, short );


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
            Load_VGA( SCAN_400, TX_FONT_8x14, TX_CURS_8x14 );
            break;
        case 43:
            Load_VGA( SCAN_350, TX_FONT_8x8, TX_CURS_8x8 );
            break;
        case 50:
            Load_VGA( SCAN_400, TX_FONT_8x8, TX_CURS_8x8 );
            break;
        default:
            _ErrorStatus = _GRINVALIDPARAMETER;
        }
        break;
    case _EGA:
        if( _CurrState->vc.monitor == _COLOR ) {
            switch( rows ) {            // 200 scan lines
            case 14:
                Load_EGA( rows, TX_FONT_8x14, TX_CURS_8x14 );
                break;
            default:
                _ErrorStatus = _GRINVALIDPARAMETER;
            }
        } else {                        // EGA Mono or Enhanced
            switch( rows ) {
            case 43:                    // 350 scan lines
                Load_EGA( rows, TX_FONT_8x8, TX_CURS_EMU );
                break;
            default:
                _ErrorStatus = _GRINVALIDPARAMETER;
            }
        }
        break;
    case _MCGA:
        switch( rows ) {
        case 40:
            Load_MCGA( rows, TX_FONT_8x8, 0x0404 );
            break;
        case 50:
            Load_MCGA( rows, TX_FONT_8x8, 0x0303 );
            break;
        default:
            _ErrorStatus = _GRINVALIDPARAMETER;
        }
        break;
    default:
        _ErrorStatus = _GRERROR;
    }
}


static void GrModeRows( short rows )
//==================================

{
    short           font;

    switch( _CurrState->vc.mode ) {
    case _ERESNOCOLOR:
    case _ERESCOLOR:
        switch( rows ) {
        case 25:
            font = GR_FONT_8x14;  // 8x14
            break;
        case _MAXTEXTROWS:
            rows = 43;
        case 43:
            font = GR_FONT_8x8;   // 8x8
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
            font = GR_FONT_8x16;  // 8x16
            break;
        case 34:
            font = GR_FONT_8x14;  // 8x14
            break;
        case _MAXTEXTROWS:
            rows = 60;
        case 60:
            font = GR_FONT_8x8;   // 8x8
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
    VideoInt( _BIOS_SET_MODE + GetVideoMode(), 0, 0, 0 );
    // Load pointer to character set
    if( _CurrState->vc.adapter == _EGA ) {
        VideoInt( font, 0, 0, rows - 1 );   // do this only for the EGA
    } else {
        VideoInt( font, 0, 0, rows );
    }
    _GrCursor = 0;                          // cursor is off
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
        rows = *(char far *)_BIOS_data( ROWS ) + 1;     // 0 for Hercules
        if( rows == 1 ) rows = 25;
        _CurrState->vc.numtextrows = rows;
        if( !_GrMode ) {
            _CalcNumPages();              // update the video configuration
        }
        _Tx_Row_Min = 0;                            // text window is now
        _Tx_Col_Min = 0;                            // the full screen
        _Tx_Row_Max = _CurrState->vc.numtextrows - 1;
        _Tx_Col_Max = _CurrState->vc.numtextcols - 1;
        _TextPos.row = 0;                           // set mode function
        _TextPos.col = 0;                           // sets position to 0,0
        _CurrVisualPage = 0;
        _CurrActivePage = 0;
        VideoInt( _BIOS_VIDEO_PAGE, 0, 0, 0 );      // set to page 0
        return( _CurrState->vc.numtextrows );
    }
}


static void Load_25( void )
/*===================

   When we want to go back to a 25-row display, just do a set mode
   and update a few variables. */

{
    VideoInt( _BIOS_SET_MODE + GetVideoMode(), 0, 0, 0 );
    *(char far *)_BIOS_data( INFO ) &= 0xFE;  // cursor emulation off
    VideoInt( _BIOS_CURSOR_SIZE, 0, 0x0607, 0 );    // reset the cursor
    _GrCursor = 1;                                  // cursor is on
}


static void Load_VGA( short scan, short font, short cursor )
/*==========================================================

   This routines loads an alphanumeric font for the VGA.    */

{
    VideoInt( scan, 0x0030, 0, 0 );   // becomes effective at next set mode
    VideoInt( _BIOS_SET_MODE + GetVideoMode(), 0, 0, 0 );
    VideoInt( _BIOS_VIDEO_PAGE, 0, 0, 0 );          // set active page to 0
    VideoInt( font, 0, 0, 0 );                      // load character set
    VideoInt( _BIOS_CURSOR_SIZE, 0, cursor, 0 );    // reset the cursor
    _GrCursor = 1;                                  // cursor is on
}


static void Load_EGA( short rows, short font, short cursor )
/*==========================================================

   This routines loads an alphanumeric font for the EGA.    */

{
    VideoInt( _BIOS_VIDEO_PAGE, 0, 0, 0 );          // set active page to 0
    VideoInt( _BIOS_SET_MODE + GetVideoMode(), 0, 0, 0 );
    VideoInt( font, 0, 0, 0 );                      // load pointer to character set in block 0
    if( rows == 43 ) {                              // cursor emulation
        *(char far *)_BIOS_data( INFO ) |= 1;     // 43 rows only
    } else {
        outpw( 0x03D4, 0x1414 );        // reset underline location to none
    }
    VideoInt( _BIOS_CURSOR_SIZE, 0, cursor, 0 );    // reset the cursor
    _GrCursor = 1;                                  // cursor is on
}


static void Load_MCGA( short rows, short font, short cursor )
/*===========================================================

   This routines loads an alphanumeric font for the MCGA.   */

{
    VideoInt( _BIOS_VIDEO_PAGE, 0, 0, 0 );         // set active page to 0
    VideoInt( _BIOS_SET_MODE + GetVideoMode(), 0, 0, 0 );
    _fmemset( MK_FP( _EgaSeg, _EgaOff ), 0, 0x2000 );  // must do for MCGA 40 rows
    VideoInt( font & 0xFF0F, 0, 0, 0 );             // load character set
    VideoInt( 0x1103, 0, 0, 0 );
    VideoInt( _BIOS_CURSOR_SIZE, 0, cursor, 0 );    // reset the cursor
    outpw( 0x03D4, ( cursor & 0xFF00 ) + 0x09 );    // # double scan lines
    *(char far *)_BIOS_data( ROWS ) = rows - 1;       // # of rows
    // # of vertical points per character
    *(short far *)_BIOS_data( POINTS ) = 2 * ( cursor & 0xFF + 1 );
    _GrCursor = 1;                                          // cursor is on
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

Entry( _SETTEXTROWS, _settextrows ) // alternate entry-point



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

Entry( _SETVIDEOMODEROWS, _setvideomoderows ) // alternate entry-point
