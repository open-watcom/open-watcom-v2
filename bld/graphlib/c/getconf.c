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
* Description:  Get current video mode information.
*
****************************************************************************/


#include <conio.h>
#include "gdefn.h"
#include "gbios.h"


#if !defined( _DEFAULT_WINDOWS )

static short            _AdapTab[ 18 ] = {
    _NODISPLAY,     _MDPA,          _CGA,           _EGA,
    _EGA,           _EGA,           _UNKNOWN,       _VGA,
    _VGA,           _UNKNOWN,       _MCGA,          _MCGA,
    _MCGA,          _HERCULES,      _HERCULES,      _HERCULES,
    _SVGA,          _SVGA
};

static short            _MonTab[ 18 ] = {
    _NODISPLAY,     _MONO,          _COLOR,         _ENHANCED,
    _COLOR,         _MONO,          _UNKNOWN,       _ANALOGMONO,
    _ANALOGCOLOR,   _UNKNOWN,       _COLOR,         _ANALOGMONO,
    _ANALOGCOLOR,   _MONO,          _MONO,          _MONO,
    _ANALOGMONO,    _ANALOGCOLOR
};

static short            _MemoryTab[ 8 ] = {
    0, 16, 16, 64, 256, -1, 256, 256
};

#endif

_WCRTLINK struct videoconfig _WCI86FAR *_WCI86FAR _CGRAPH
        _getvideoconfig( struct videoconfig _WCI86FAR *config )
/*=================================================================*/ 
/* This function returns a structure containing information about the
   current video mode. */

{
    _InitState();
    *config = _CurrState->vc;
    return( config );
}

Entry( _GETVIDEOCONFIG, _getvideoconfig ) // alternate entry-point


void _CalcNumPages( void )
/*==================

   This routine updates the number of alphanumeric video pages after a
   new _setvideomode() or _setvideomoderows().
   Note: real page size is = 2 * rows * cols, but 1024 = ( 1 << 10 )
         So use some magic to line up the start of the video page with a
         1K boundary.   */

{
    short           product;
    short           buf_size;
    short           pg_size;
    short           pages;

    product = _CurrState->vc.numtextrows * _CurrState->vc.numtextcols;
    pg_size = product & 0xFE00;         /* total # of chars / 512   */
    if( pg_size < product ) {               /* closest 1/2K boundary    */
        pg_size += 512;
    }
    /* Multiply by 2 to get closest K-byte boundary.*/
    *(short far *)_BIOS_data( CRT_LEN ) = pg_size << 1;
    /* Adjust # of video pages for the given memory.*/
    if( _CurrState->vc.memory == 64 ) {
        buf_size = 16;                      /* in K bytes   */
    } else {
        buf_size = 32;                      /* in K bytes   */
    }
    pages = buf_size / ( pg_size >> 9 );
    if( pages > 8 ) {           // BIOS cannot support more than 8 pages
        pages = 8;
    }
    _CurrState->vc.numvideopages = pages;
}


void _GetState( void )
/*====================

    This routine initializes each field of the video state structure.
    It is assumed that the current mode is a text mode. */

{
#if defined( _DEFAULT_WINDOWS )
    HFONT               font;
    HFONT               oldfont;
    WPI_TEXTMETRIC      info;
    WPI_PRES            pres;
    int                 Width, Height, NumColors;
  #if defined( __WINDOWS__ )
    HWND                dwin;
  #endif
#else
    short               display;
    short               rows;
#endif

    _CurrState->vc.numxpixels = 0;
    _CurrState->vc.numypixels = 0;
    _CurrState->vc.bitsperpixel = 0;
#if defined( __QNX__ )
    // The QNX "bios emulator" didn't support all of the int10
    // functions used by the graphics lib.
    _CurrState->misc_info = NO_BIOS;
#else
    _CurrState->misc_info = 0;
#endif
#if defined( _DEFAULT_WINDOWS )
    _CurrState->vc.numxpixels = _wpi_getsystemmetrics( SM_CXSCREEN );
    _CurrState->vc.numypixels = _wpi_getsystemmetrics( SM_CYSCREEN );
    _CurrState->vc.adapter = _VGA;  //initialize it so that it works with windows
    if( !_Mem_dc ) {
        Height = 16;
        Width = 8;
    } else {
        font = _GetSysMonoFnt();
        oldfont = _MySelectFont( _Mem_dc, font );
        _wpi_gettextmetrics( _Mem_dc, &info );
        Height = _wpi_metricheight( info );
        Width = _wpi_metricmaxcharwidth( info );
        _MyGetOldFont( _Mem_dc, oldfont );
    }
    _CurrState->vc.numtextrows = _CurrState->vc.numypixels / Height;
    _CurrState->vc.numtextcols = _CurrState->vc.numxpixels / Width;
    #if defined( __OS2__ )
        pres = _wpi_getscreenpres();
    #else
        dwin = GetDesktopWindow();
        pres = GetDC( dwin );
    #endif
    NumColors = _wpi_getdevicecaps( pres, NUMCOLORS );
    #if defined( __OS2__ )
        WinReleasePS( pres );
    #else
        ReleaseDC( dwin, pres );
    #endif
    if( NumColors > 256 ) NumColors = 256;
    else if( NumColors > 16 ) NumColors = 16;
    _CurrState->vc.numcolors = NumColors;
    _CurrState->vc.numvideopages = 1;
#else
    rows = *(char far *)_BIOS_data( ROWS ) + 1;     // 0 for Hercules
    if( rows == 1 ) rows = 25;
    _CurrState->vc.numtextrows = rows;
    _CurrState->vc.numtextcols = *(short far *)_BIOS_data( CRT_COLS );
    _CurrState->vc.numcolors = 32;
    _CurrState->vc.mode = GetVideoMode();
    display = _SysMonType() & 0x00FF;
    _CurrState->vc.adapter = _AdapTab[ display ];
    _CurrState->vc.monitor = _MonTab[ display ];
    _CurrState->vc.memory = _MemoryTab[ _CurrState->vc.adapter ];
    if( _CurrState->vc.memory == -1 ) {     // EGA adapter
        _CurrState->vc.memory = 64 * ( 1 + ( EGA_Memory() & 0x00ff ) );
    }
    if( _GrMode || _CurrState->vc.adapter < _MCGA ) {
        _CurrState->vc.numvideopages = 8;
    } else {
        _CalcNumPages();
    }
    _CursorShape = *(short far *)_BIOS_data( CURSOR_MODE );
#endif
}


void _InitState( void )
/*=====================

   This routine reads the current BIOS state and initializes the variables
   relevant to the BIOS text routines.  */

{
    unsigned short      pos;

    if( _StartUp ) {        // if first time through
        _StartUp = 0;
#if defined( _DEFAULT_WINDOWS )
        pos = pos;
        _CurrState->vc.mode = 0;
#else
        _InitSegments();
#endif
        _GetState();
#if defined( _DEFAULT_WINDOWS )
        _TextPos.row = 0;               // default position for window
        _TextPos.col = 0;
#else
        _DefMode = _CurrState->vc.mode;
        _DefTextRows = _CurrState->vc.numtextrows;
        pos = *(short far *)_BIOS_data( CURSOR_POSN );
        _TextPos.row = pos >> 8;        /* default cursor position  */
        _TextPos.col = pos & 0xFF;
#endif
        _Tx_Row_Min = 0;                /* full screen text window  */
        _Tx_Row_Max = _CurrState->vc.numtextrows - 1;
        _Tx_Col_Min = 0;
        _Tx_Col_Max = _CurrState->vc.numtextcols - 1;
    }
}


void _GrInit( short x, short y, short stride, grcolor col, short bpp,
              short pag, short seg, int off, short siz, short mis )
//=================================================================

//  Initialize fields of the videoinfo structure for a graphics mode.

{
#if defined( VERSION2 )
    int i;
#endif
    _GrMode = TRUE;
    _GetState();    // initialize text mode fields
    _CurrState->vc.numxpixels    = x;
    _CurrState->vc.numypixels    = y;
    _CurrState->vc.numcolors     = col;
#if defined( VERSION2 )
    _CurrState->pixel_mask       = col - 1;
#endif
    _CurrState->vc.bitsperpixel  = bpp;
    _CurrState->vc.numvideopages = pag;
    _CurrState->screen_seg_base  = seg;
    _CurrState->screen_off_base  = off;
    _CurrState->page_size        = siz;
    _CurrState->misc_info        |= mis;
    _CurrState->screen_seg       = seg;     /* page 0 screen segment */
    _CurrState->screen_off       = off;     /* page 0 screen offset */
    _CurrState->stride           = stride;
#if defined( VERSION2 )
    for( i = 0; i < 16; i++ ) {
        _coltbl[i] = _VGA_Colours[i];
    }
#endif
}
