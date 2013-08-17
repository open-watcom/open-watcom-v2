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
* Description:  Video mode set processing.
*
****************************************************************************/


#include <conio.h>
#include "gdefn.h"
#include "gbios.h"
#include "montypes.h"

#ifdef __QNX__

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/console.h>
#include <sys/dev.h>
#include <sys/kernel.h>

static struct _console_ctrl     *ConCtrl;
static unsigned                 old_hscroll;

static void HScrollOff()
{
    int                         h;

    if( ConCtrl == NULL ) {
        h = open( "//0/dev/con1", O_RDONLY );
        if( h != -1 ) {
            ConCtrl = console_open( h, O_RDWR );
            close( h );
        }
    }
    if( ConCtrl != NULL ) {
        old_hscroll = console_ctrl( ConCtrl, -1, CONSOLE_NOHSCROLL, CONSOLE_NOHSCROLL );
    }
}

static void HScrollRestore()
{
    if( ConCtrl != NULL ) {
        console_ctrl( ConCtrl, -1, old_hscroll, CONSOLE_NOHSCROLL );
    }
}

#endif // __QNX__


#if defined( _DEFAULT_WINDOWS )
    #undef HANDLE               // already defined by WPI
    #undef GLOBALHANDLE
    #include "win.h"
    #include <stdarg.h>
  #if defined( __OS2__ )
    #include "pmmenu.h"
  #endif

    WPI_PRES            _Mem_dc = NULL;
    HDC                 _Hdc;
    HBITMAP             _Mem_bmp = NULL;
    HWND                _CurrWin = NULL;
    extern HWND         _MainWindow;
  #if !defined( __OS2__ )
    extern HMENU        _SubMenuWindows;
  #else
    extern HWND         _GetWinMenuHandle();
  #endif
    extern void         _GetWindowNameAndCoords( char*, char*, int*, int*, int*, int*   );
    static LPWDATA      _NewGphWindow( HWND hwnd, ... );
    static short        _registergphclass( WPI_INST );

#else

    extern gr_device _FARD  _TextDevice, _GrCGA_4, _GrCGA_6, _GrHGC_11,
                            _GrEGA_13, _GrEGA_14, _GrEGA_15, _GrEGA_16,
                            _GrVGA_17, _GrVGA_18, _GrVGA_19;
  #if defined( _SUPERVGA )
    extern gr_device _FARD  _GrSVGA_100, _GrSVGA_102, _GrSVGA_103,
                            _GrSVGA_104, _GrSVGA_105;
  #endif

  #if defined( _SUPERVGA ) && defined( VERSION2 )
    extern gr_device _FARD  _GrVESA;
  #endif


typedef struct supported_video_mode {
    short           mode;
    gr_device _FARD *dev;
} SUPP_MODE;


static SUPP_MODE        VideoModes[] = {
    _TEXTBW40,          &_TextDevice,
    _TEXTC40,           &_TextDevice,
    _TEXTBW80,          &_TextDevice,
    _TEXTC80,           &_TextDevice,
    _MRES4COLOR,        &_GrCGA_4,
    _MRESNOCOLOR,       &_GrCGA_4,
    _HRESBW,            &_GrCGA_6,
    _TEXTMONO,          &_TextDevice,
    _HERCMONO,          &_GrHGC_11,
    _MRES16COLOR,       &_GrEGA_13,
    _HRES16COLOR,       &_GrEGA_14,
    _ERESNOCOLOR,       &_GrEGA_15,
    _ERESCOLOR,         &_GrEGA_16,
    _VRES2COLOR,        &_GrVGA_17,
    _VRES16COLOR,       &_GrVGA_18,
    _MRES256COLOR,      &_GrVGA_19,
  #if defined( _SUPERVGA )
    0x100,              &_GrSVGA_100,
    0x101,              &_GrSVGA_100,
    0x102,              &_GrSVGA_102,
    0x103,              &_GrSVGA_103,
    0x104,              &_GrSVGA_104,
    0x105,              &_GrSVGA_105,
  #endif
  #if defined( _SUPERVGA ) && defined( VERSION2 )
    -1,                 &_GrVESA
  #else
    -1,                 NULL
  #endif
};


static short            SelectMode( short );
#endif


static void SetTextInfo( void )
/*=======================

   This routine initializes the graphics text defaults. */

{
    short               height;
    short               width;

    /* Make the graphics text look the same on all devices. Take the
       width to height visual ratio to be 5:8 and assume that the
       width to height ratio of the physical dimensions of the screen
       is 4 : 3. The size is about 1.5 the size of BIOS text.   */

    height = ( _CurrState->vc.numypixels * 3 ) /
                                ( _CurrState->vc.numtextrows * 2 );
    width = ( (long) height * _CurrState->vc.numxpixels * 15 ) /    // 3 * 5
                ( (long) _CurrState->vc.numypixels * 32 );          // 4 * 8
    _setcharsize( height, width );
    _settextorient( 1, 0 );
    _settextpath( _PATH_RIGHT );
    _setcharspacing( 0 );
    _settextalign( _NORMAL, _NORMAL );
}


static void _InitVariables( void )
/*================================

   This routine initializes all of the global variables used by the
   graphics functions. */

{
    _CurrPos.xcoord = 0;                        /* initial position */
    _CurrPos.ycoord = 0;
    _LogOrg.xcoord = _CurrPos.xcoord;
    _LogOrg.ycoord = _CurrPos.ycoord;
    _CurrPos_w.wx = 0.0;
    _CurrPos_w.wy = 0.0;

    _Window.invert = TRUE;          /* window coordinates defaults  */
    _Window.xleft = 0.0;
    _Window.ybottom = 0.0;
    _Window.xright = 1.0;
    _Window.ytop = 1.0;

    _CurrState->clip_def.xmin = 0;               /* graphics window */
    _CurrState->clip_def.xmax = _CurrState->vc.numxpixels - 1;
    _CurrState->clip_def.ymin = 0;
    _CurrState->clip_def.ymax = _CurrState->vc.numypixels - 1;
    _setclip( _GCLIPON );

    _TextPos.row = 0;                   /* BIOS text cursor position    */
    _TextPos.col = 0;
    _Tx_Row_Min = 0;                            /* text window */
    _Tx_Row_Max = _CurrState->vc.numtextrows - 1;
    _Tx_Col_Min = 0;
    _Tx_Col_Max = _CurrState->vc.numtextcols - 1;
    if( _GrMode ) {
        SetTextInfo();
    }

    memcpy( _FillMask, _DefMask, MASK_LEN );    /* solid fill */
    _HaveMask = 0;                              /* no fill mask set */
    _PaRf_x = 0;                                /* fill pattern    */
    _PaRf_y = 0;                                /* reference point */

    _CharAttr = _DEFAULT_ATTR;
    _CurrColor = ( _CurrState->vc.numcolors - 1 ) & 15;
#if defined( VERSION2 )
    if( _CurrState->vc.numcolors > 256 ) {
        _CharAttr = _WHITE;                     // white
        _CurrColor = _CurrState->pixel_mask;    // brightwhite
    }
#endif
    _CurrBkColor = 0;
    _CurrActivePage = _CurrVisualPage = 0;
    _CurrState->screen_seg = _CurrState->screen_seg_base;/* pg 0 scrn segment */
    _CurrState->screen_off = _CurrState->screen_off_base;/* pg 0 scrn offset */
    _LineStyle = SOLID_LINE;                    /* solid line */
    _StyleWrap = 0;                             /* line style continuation  */
    _PlotAct = 0;                               /* replace mode */
    _Transparent = 1;                           /* transparent mode */
    _Wrap = 1;                                  /* wrapping on */
}


_WCRTLINK short _WCI86FAR _CGRAPH _setvideomode( short req_mode )
/*================================================

   This routine sets the video mode if it is supported by the current
   hardware configuration. It returns the number of text rows if
   successful, otherwise it returns 0.  */

{
#if defined( _DEFAULT_WINDOWS )
    WPI_INST            Inst;
    HWND                Win;
    WPI_PRES            Win_DC;
    WPI_RECT            rect;
    WPI_TEXTMETRIC      fntinfo;
    HMENU               menu;
    HFONT               old_font;
    LPWDATA             w;
    char*               name = "\0";
    char                dest[80];
    long                x, y;
    int                 x1, y1, x2, y2;
    short               clipy1, clipy2;
    WPI_RECTDIM         right, bottom, height;
  #if defined( __OS2__ )
    ULONG               winstyle;
    HWND                frame;
    MENUITEM            gphmenu;
  #endif
#else
    short               mode;
    short               prev_mode;
#ifdef __QNX__
    int                 wasTextMode;
#endif
    SUPP_MODE           *tab;
    gr_device _FARD     *prev_dev;
    gr_device _FARD     *dev_ptr;
#endif

    _ErrorStatus = _GROK;
    _InitState();
#if defined( _DEFAULT_WINDOWS )
    Win = 0;
  #if defined( __OS2__ )
    Inst.hab = WinQueryAnchorBlock( _MainWindow );
    Inst.mod_handle = NULL;
  #else
    Inst = GetWindowWord( _MainWindow, GWW_HINSTANCE );
  #endif
    _SetInst( &Inst );
    if( !_registergphclass( Inst ) ) {
        _ErrorStatus = _GRMODENOTSUPPORTED;
        return( 0 );
    }
    x = _wpi_getsystemmetrics( SM_CXSCREEN );
    y = _wpi_getsystemmetrics( SM_CYSCREEN );

    _SetPresHeight( y );

    if( req_mode == 0 ) {
        _ErrorStatus = _GRMODENOTSUPPORTED;
        return( 0 );
    } else if( _CurrWin ){
    // Make sure we don't create more than 1 window, and know when to close it
        if( req_mode == _DEFAULTMODE ) {
  #if defined( __OS2__ )
                w = _GetWindowData( _CurrWin );
                Win =  w->frame;
  #else
                Win = _CurrWin;
  #endif
                _wpi_destroywindow( Win );
                return( _CurrState->vc.numtextrows );
        } else {
            // if we are only changing moods, we will reinitialize everything
            _clearscreen( _GCLEARSCREEN );
            init_memdc_bk( x, y );
            init_all();
            _wpi_updatewindow( Win );
            return( _CurrState->vc.numtextrows );
        }
    }
    // Create a new window

    // Get the name and coordinates of the window
    name = "Graphics";
    _GetWindowNameAndCoords( name, dest, &x1, &x2, &y1, &y2 );

  #if defined( __OS2__ )
    winstyle = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX |
            FCF_VERTSCROLL | FCF_HORZSCROLL;
    frame = WinCreateStdWindow( _MainWindow,
                WS_VISIBLE | WS_CLIPSIBLINGS,
                &winstyle, "GraphWndClass", dest, 0, NULL, 0, &Win );
    if( frame == 0 ) return( FALSE );
    WinSetOwner( Win, _MainWindow );
    _OldFrameProc = WinSubclassWindow( frame, GraphFrameProc );
  #else
    Win = CreateWindow( "GraphWndClass", dest,
                        WS_CHILD |
                        WS_CAPTION | WS_THICKFRAME |
                        WS_CLIPSIBLINGS | WS_SYSMENU |
                        WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
                        0,  0,
                        0,  0,
                        _MainWindow, NULL,
                        Inst, NULL );
    if( !Win ) return( FALSE );
  #endif

    _CurrWin = Win;
    Win_DC = _wpi_getpres( Win );

    // Create the memory dc
    _Mem_dc = _wpi_createcompatiblepres( Win_DC, Inst, &_Hdc );
    if( !_Mem_dc ){
        _ErrorStatus = _GRMODENOTSUPPORTED;
        _wpi_destroywindow( _CurrWin );
        return( 0 );
    }

    // Create the bitmap to draw on
    _Mem_bmp = _wpi_createcompatiblebitmap( Win_DC, x,y );
    if( !_Mem_bmp ){
        _ErrorStatus = _GRMODENOTSUPPORTED;
        _wpi_destroywindow( _CurrWin );
        return( 0 );
    }

    _wpi_torgbmode( _Mem_dc );
    _wpi_torgbmode( Win_DC );

    _wpi_selectbitmap( _Mem_dc, _Mem_bmp );
    _wpi_releasepres( Win, Win_DC );

    _CurrState->vc.mode = Win;

    w = _NewGphWindow( Win, 9999, -1 );

    // initialize variables, memory device, etc.
    if( !_CreateSysMonoFnt( _Mem_dc ) ) {
        _ErrorStatus = _GRMODENOTSUPPORTED;
        return( 0 );
    }
    init_memdc_bk( x, y );
    init_all();
                        /* Initialize clipping regions */
    clipy1 = _wpi_cvth_y( _CurrState->clip_def.ymin, _GetPresHeight() );
    clipy2 = _wpi_cvth_y( _CurrState->clip_def.ymax, _GetPresHeight() );
    _wpi_setintwrectvalues( &rect, _CurrState->clip_def.xmin,
                                   clipy1,
                                   _CurrState->clip_def.xmax,
                                   clipy2 );
    _ClipRgn = _wpi_createrectrgn( _Mem_dc, &rect );
    _wpi_selectcliprgn( _Mem_dc, _ClipRgn );

  #if defined( __OS2__ )
    GpiDeleteSetId( _Mem_dc, LCID_DEFAULT );
    _CurFnt = LCID_DEFAULT;
  #else
    _CurFnt = GetStockObject( SYSTEM_FONT );
  #endif
    old_font = _MySelectFont( _Mem_dc, _CurFnt );
    _wpi_gettextmetrics( _Mem_dc, &fntinfo );

  #if defined( __OS2__ )
    w->frame = frame;
    menu = _GetWinMenuHandle();
  #else
    w->inst = Inst;
    menu = _SubMenuWindows;
  #endif

    _wpi_getclientrect( _MainWindow, &rect );
    height = _wpi_getheightrect( rect );

    // Calculate where to put the window and how big it should be
    x2 -= x1;
    y2 -= y1;
    x1 *= _wpi_metricmaxcharwidth( fntinfo );
    y1 *= _wpi_metricheight( fntinfo );
    x2 *= _wpi_metricmaxcharwidth( fntinfo );
    y2 *= _wpi_metricheight( fntinfo );
    w->xchar = _wpi_metricmaxcharwidth( fntinfo );
    w->ychar = _wpi_metricheight( fntinfo );
    _wpi_getrectvalues( rect, NULL, NULL, &right, &bottom );
    if( ( x1 < 0 ) || ( x1 >= right ) ) {
        x1 = 0;
    }
    if( ( y1 < 0 ) || ( y1 >= bottom ) ) {
        y1 = 0;
    }
    if( ( x2 <= 0 ) || ( x2 >= right ) ) {
        x2 = right - x1;
    }
    if( ( y2 <= 0 ) || ( y2 >= bottom ) ) {
        y2 = bottom - y1;
    }
  #if defined( __OS2__ )
    WinSendMsg( frame, WM_SETICON,
        MPFROMLONG( WinQuerySysPointer( HWND_DESKTOP, SPTR_APPICON, TRUE ) ), 0 );
    WinSetWindowPos( frame,
                     HWND_TOP,
                     x1,
                     y1,
                     x2,
                     y2,
                     SWP_SIZE | SWP_MOVE | SWP_ZORDER );

    gphmenu.iPosition = _MainWindowData->window_count - 1;
    gphmenu.afStyle = MIS_TEXT;
    gphmenu.afAttribute = 0;
    gphmenu.id = DID_WIND_STDIO + w->handles[0];
    gphmenu.hwndSubMenu = NULL;
    gphmenu.hItem = 0;
    if ( (MRESULT)MIT_ERROR == WinSendMsg( menu, ( ULONG )MM_INSERTITEM, MPFROMP( &gphmenu ), MPFROMP( dest ) ) ) abort();

  #else
    AppendMenu( menu, MF_ENABLED, MSG_WINDOWS+w->handles[0], dest );
    MoveWindow( Win, x1, y1, x2, y2, TRUE );
    ShowWindow( Win, SW_NORMAL );
    ShowScrollBar( Win, SB_BOTH, TRUE );
  #endif
    _MyGetOldFont( _Mem_dc, old_font );

    _wpi_updatewindow( Win );
    _MakeWindowActive( w );
#else
    mode = SelectMode( req_mode );
    if( mode == _DEFAULTMODE ) {
        _ErrorStatus = _GRERROR;
        return( 0 );
    }
    for( tab = VideoModes; ; ++tab ) {
        if( tab->mode == -1 ) {
#if defined( VERSION2 )
            if( ( mode > 0x105 ) && ( mode < 0x120 ) ) {
                dev_ptr=tab->dev;
                break;
            } else {
                _ErrorStatus = _GRINVALIDPARAMETER;
                return( 0 );
            }
#else
            _ErrorStatus = _GRINVALIDPARAMETER;
            return( 0 );
#endif
        } else if( tab->mode == mode ) {
            dev_ptr = tab->dev;
            break;
        }
    }

#ifdef __QNX__
    wasTextMode = IsTextMode;
#endif //__QNX__

    prev_dev  = _CurrState->deviceptr;
    prev_mode = _CurrState->vc.mode;
    if( prev_dev != NULL ) {
        _FiniDevice();
    }
    _CurrState->deviceptr = dev_ptr;
    if( !_InitDevice( mode ) ) {       // if invalid mode
        _CurrState->deviceptr = prev_dev;      // restore previous
        if( prev_dev != NULL ) {
            _InitDevice( prev_mode );
        }
        _ErrorStatus = _GRMODENOTSUPPORTED;
        return( 0 );
    } else {
        _GrMode = _CurrState->vc.bitsperpixel != 0;
        if( IsTextMode ) {
            _CursState = _GCURSORON;
        } else {
            _CursState = _GCURSOROFF;     /* cursor off in graphics mode  */
        }
        _GrCursor = _CursState;
        if( req_mode == _DEFAULTMODE ) {
            _SetRows( _DefTextRows );
        }
        _InitVariables();               /* initialize globals       */
        if( req_mode != _DEFAULTMODE ) {
            _PaletteInit();             // don't init palette if _DEFAULTMODE
        }
    }
#endif

#ifdef __QNX__
    if (req_mode == _DEFAULTMODE) {
        if (wasTextMode) {
            HScrollRestore();
        }
    } else if (IsTextMode) {
        HScrollOff();
    }

#endif

    return( _CurrState->vc.numtextrows );
}

Entry1( _SETVIDEOMODE, _setvideomode ) // alternate entry-point


#if !defined( _DEFAULT_WINDOWS )

static short SelectMode( short req_mode )
/*=======================================

    Return the actual mode based on the value of req_mode. */

{
    short               mode;
    short               monitor;

    if( req_mode == _MAXRESMODE ) {
        monitor = _SysMonType() & 0xff;     // only look at active monitor
        switch( monitor ) {
        case MT_CGA_COLOUR:
            mode = _HRESBW;
            break;
        case MT_EGA_ENHANCED:
            mode = _ERESCOLOR;
            break;
        case MT_EGA_COLOUR:
            mode = _HRES16COLOR;
            break;
        case MT_EGA_MONO:
            mode = _ERESNOCOLOR;
            break;
        case MT_VGA_MONO:
        case MT_VGA_COLOUR:
        case MT_SVGA_MONO:
        case MT_SVGA_COLOUR:
            mode = _VRES16COLOR;
            break;
        case MT_MCGA_DIGITAL:
        case MT_MCGA_MONO:
        case MT_MCGA_COLOUR:
            mode = _VRES2COLOR;
            break;
        case MT_HERC:
        case MT_HERCPLUS:
        case MT_HERCINCL:
            mode = _HERCMONO;
            break;
        default:
            mode = _DEFAULTMODE;
        }
    } else if( req_mode == _MAXCOLORMODE ) {
        monitor = _SysMonType() & 0xff;     // only look at active monitor
        switch( monitor ) {
        case MT_CGA_COLOUR:
            mode = _MRES4COLOR;
            break;
        case MT_EGA_ENHANCED:
            if( EGA_Memory() != 0 ) {       // more than 64K
                mode = _ERESCOLOR;
                break;
            }   // else - fall through
        case MT_EGA_COLOUR:
            mode = _HRES16COLOR;
            break;
        case MT_EGA_MONO:
            mode = _ERESNOCOLOR;
            break;
        case MT_VGA_MONO:
        case MT_VGA_COLOUR:
        case MT_MCGA_DIGITAL:
        case MT_MCGA_MONO:
        case MT_MCGA_COLOUR:
        case MT_SVGA_MONO:
        case MT_SVGA_COLOUR:
            mode = _MRES256COLOR;
            break;
        case MT_HERC:
        case MT_HERCPLUS:
        case MT_HERCINCL:
            mode = _HERCMONO;
            break;
        default:
            mode = _DEFAULTMODE;
        }
    } else if( req_mode == _DEFAULTMODE ) {
        mode = _DefMode;
    } else {
        mode = req_mode;
    }
    return( mode );
}

#endif


#if defined( _DEFAULT_WINDOWS )

WPI_COLOUR              Color[ 256 ] = {
    0x00000000, 0x00800000, 0x00008000, 0x00808000, 0x00000080,
    0x00800080, 0x00008080, 0x00bfbfbf, 0x00808080, 0x00ff0000,
    0x0000ff00, 0x00ffff00, 0x000000ff, 0x00ff00ff, 0x0000ffff,
    0x00ffffff,
    0x00000000, 0x00050505, 0x00080808, 0x000b0b0b, 0x000e0e0e,
    0x00111111, 0x00141414, 0x00181818, 0x001c1c1c, 0x00202020,
    0x00242424, 0x00282828, 0x002d2d2d, 0x00323232, 0x00383838,
    0x003f3f3f, 0x003f0000, 0x003f0010, 0x003f001f, 0x003f002f,
    0x003f003f, 0x002f003f, 0x001f003f, 0x0010003f, 0x0000003f,
    0x0000103f, 0x00001f3f, 0x00002f3f, 0x00003f3f, 0x00003f2f,
    0x00003f1f, 0x00003f10, 0x00003f00, 0x00103f00, 0x001f3f00,
    0x002f3f00, 0x003f3f00, 0x003f2f00, 0x003f1f00, 0x003f1000,
    0x003f1f1f, 0x003f1f27, 0x003f1f2f, 0x003f1f37, 0x003f1f3f,
    0x00371f3f, 0x002f1f3f, 0x00271f3f, 0x001f1f3f, 0x001f273f,
    0x001f2f3f, 0x001f373f, 0x001f3f3f, 0x001f3f37, 0x001f3f2f,
    0x001f3f27, 0x001f3f1f, 0x00273f1f, 0x002f3f1f, 0x00373f1f,
    0x003f3f1f, 0x003f371f, 0x003f2f1f, 0x003f271f, 0x003f2d2d,
    0x003f2d31, 0x003f2d36, 0x003f2d3a, 0x003f2d3f, 0x003a2d3f,
    0x00362d3f, 0x00312d3f, 0x002d2d3f, 0x002d313f, 0x002d363f,
    0x002d3a3f, 0x002d3f3f, 0x002d3f3a, 0x002d3f36, 0x002d3f31,
    0x002d3f2d, 0x00313f2d, 0x00363f2d, 0x003a3f2d, 0x003f3f2d,
    0x003f3a2d, 0x003f362d, 0x003f312d, 0x001c0000, 0x001c0007,
    0x001c000e, 0x001c0015, 0x001c001c, 0x0015001c, 0x000e001c,
    0x0007001c, 0x0000001c, 0x0000071c, 0x00000e1c, 0x0000151c,
    0x00001c1c, 0x00001c15, 0x00001c0e, 0x00001c07, 0x00001c00,
    0x00071c00, 0x000e1c00, 0x00151c00, 0x001c1c00, 0x001c1500,
    0x001c0e00, 0x001c0700, 0x001c0e0e, 0x001c0e11, 0x001c0e15,
    0x001c0e18, 0x001c0e1c, 0x00180e1c, 0x00150e1c, 0x00110e1c,
    0x000e0e1c, 0x000e111c, 0x000e151c, 0x000e181c, 0x000e1c1c,
    0x000e1c18, 0x000e1c15, 0x000e1c11, 0x000e1c0e, 0x00111c0e,
    0x00151c0e, 0x00181c0e, 0x001c1c0e, 0x001c180e, 0x001c150e,
    0x001c110e, 0x001c1414, 0x001c1416, 0x001c1418, 0x001c141a,
    0x001c141c, 0x001a141c, 0x0018141c, 0x0016141c, 0x0014141c,
    0x0014161c, 0x0014181c, 0x00141a1c, 0x00141c1c, 0x00141c1a,
    0x00141c18, 0x00141c16, 0x00141c14, 0x00161c14, 0x00181c14,
    0x001a1c14, 0x001c1c14, 0x001c1a14, 0x001c1814, 0x001c1614,
    0x00100000, 0x00100004, 0x00100008, 0x0010000c, 0x00100010,
    0x000c0010, 0x00080010, 0x00040010, 0x00000010, 0x00000410,
    0x00000810, 0x00000c10, 0x00001010, 0x0000100c, 0x00001008,
    0x00001004, 0x00001000, 0x00041000, 0x00081000, 0x000c1000,
    0x00101000, 0x00100c00, 0x00100800, 0x00100400, 0x00100808,
    0x0010080a, 0x0010080c, 0x0010080e, 0x00100810, 0x000e0810,
    0x000c0810, 0x000a0810, 0x00080810, 0x00080a10, 0x00080c10,
    0x00080e10, 0x00081010, 0x0008100e, 0x0008100c, 0x0008100a,
    0x00081008, 0x000a1008, 0x000c1008, 0x000e1008, 0x00101008,
    0x00100e08, 0x00100c08, 0x00100a08, 0x00100b0b, 0x00100b0c,
    0x00100b0d, 0x00100b0f, 0x00100b10, 0x000f0b10, 0x000d0b10,
    0x000c0b10, 0x000b0b10, 0x000b0c10, 0x000b0d10, 0x000b0f10,
    0x000b1010, 0x000b100f, 0x000b100d, 0x000b100c, 0x000b100b,
    0x000c100b, 0x000d100b, 0x000f100b, 0x0010100b, 0x00100f0b,
    0x00100d0b, 0x00100c0b, 0x00100c0b, 0x00100c0b, 0x00100c0b,
    0x00100c0b, 0x00100c0b, 0x00000000, 0x00000000, 0x00000000
};


void _PaletteInit( void )
/*=======================
    This function initializes the palette for windows.  It initializes
    the first 16 colors to specific values.  Depending on how many colors
    Windows is capable of handling, it initializes the color to Windows'
    system palette colors.  And the rest become black. */
{
    short               i, j;
    short               r, g, b;
#if defined( __WINDOWS__ )
    WPI_COLOUR          rgb;

    _wpi_setrop2( _Mem_dc, R2_XORPEN );
#endif

    i = j = 0;
    while( ( i < 16 ) && ( j < 16 ) ) {
#if defined( __WINDOWS__ )
        rgb = _wpi_getrgb( Color[j] & 0x000000ff,
                         ( Color[j] & 0x0000ff00 ) >> 8,
                         ( Color[j] & 0x00ff0000 ) >> 16 );

        rgb = _wpi_palettergb( _Mem_dc, _wpi_getrvalue( rgb ),
                                        _wpi_getgvalue( rgb ),
                                        _wpi_getbvalue( rgb ) );
        _wpi_setpixel( _Mem_dc, 0, 0, rgb );
        rgb = _wpi_getpixel( _Mem_dc, 0, 0 );
        _Set_RGB_COLOR( i, rgb );
        _wpi_setpixel( _Mem_dc, 0, 0, rgb );
#else
        _Set_RGB_COLOR( i, _wpi_getrgb( Color[j] & 0x000000ff,
                                      ( Color[j] & 0x0000ff00 ) >> 8,
                                      ( Color[j] & 0x00ff0000 ) >> 16 ) );
#endif
        ++i;
        ++j;
    }
#if defined( __WINDOWS__ )
    _wpi_setrop2( _Mem_dc, R2_COPYPEN );
#endif
    while( ( i < _CurrState->vc.numcolors ) && ( j < 240) ) {
        r = Color[j] & 0x000000ff;
        r = r * 4.0625f;
        g = ( Color[j] & 0x0000ff00 ) >> 8;
        g = g * 4.0625f;
        b = ( Color[j] & 0x00ff0000 ) >> 16 ;
        b = b * 4.0625f;
        _Set_RGB_COLOR( i, _wpi_getrgb( r, g, b ) );
        ++i;
        ++j;
    }
    while( i < _CurrState->vc.numcolors ) {
        _Set_RGB_COLOR( i, 0L );
        ++i;
    }
}


static void init_memdc_bk( long x, long y )
//=========================================
{
    WPI_RECT    rect;
    HBRUSH      brush;

    _wpi_setrectvalues( &rect, 0, 0, x, y );
    brush = _wpi_createsolidbrush( 0x00000000 );
    _wpi_fillrect( _Mem_dc, &rect, 0, brush );
    _wpi_deletebrush( brush );
}


static void init_all( void )
/*==========================
    This function initializes everything for windows.*/
{
    /* initialize the refresh coordinates */
    _BitBlt_Coord.xcoord = 0;
    _BitBlt_Coord.ycoord = 0;

    /* initialize the drawing modes for our memory dc */
    SetBkMode( _Mem_dc, TRANSPARENT );
    _wpi_setrop2( _Mem_dc, R2_COPYPEN );

    _GrMode = 1;                /* must be graphics for windows */
    _GetState();
    _CursState = _GCURSOROFF;     /* cursor off in graphics mode  */
    _GrCursor = _CursState;
    _InitVariables();                       /* initialize globals       */
}


static LPWDATA _NewGphWindow( HWND hwnd, ... )
/*============================================

    Create a new graphic window */
{
    LPWDATA     w;
    va_list     al;

    /*
     * allocate window data area
     */
    va_start( al, hwnd );
    w = _AnotherWindowData( hwnd, al );

    /*
     * set up data
     */
  #if defined( __OS2__ )
    w->text_color = CLR_WHITE;
    w->background_color = CLR_BLACK;
  #else
    w->text_color = BRIGHT_WHITE;
    w->background_color = BLACK;
    w->brush = CreateSolidBrush( _ColorMap[BLACK] );
  #endif
    w->LineHead = w->LineTail = NULL;
    w->gphwin = TRUE;

    return( w );

} /* _NewWindow */


static short _registergphclass( WPI_INST inst )
/*=============================================

    This function registers the graphics windows' class.*/
{
    WNDCLASS            wc;
    short               rc;
    static short        _Startup = 1;

    if( _Startup ){
        _Startup = 0;

        _wpi_setclassstyle( &wc, NULL );
        _wpi_setclassproc( &wc, GraphWndProc );
        _wpi_setclassextra( &wc, 0 );
        _wpi_setclassinst( &wc, inst );
        _wpi_setclassbackground( &wc, _wpi_createsolidbrush( 0x00000000 ) );
        _wpi_setclassname( &wc, "GraphWndClass" );
    #if defined( __WINDOWS__ )
        wc.cbClsExtra = 0;
        wc.lpszMenuName =  NULL;
        wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
        wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    #endif

        rc = _wpi_registerclass( &wc );
        if( !rc ) return( FALSE );
    }
    return( TRUE );
}


#else

void _PaletteInit( void )
//=======================

{
    int                 i;

    // For VGA like adapters, map the overscan register (border) to black and
    // reset the palette so the 1st 16 colors use the 1st 16 palette registers
    if( _CurrState->vc.adapter >= _MCGA &&
        ( _CurrState->vc.mode != 7 && _CurrState->vc.mode != 15 ) ) {
        VideoInt( _BIOS_SET_PALETTE + 0x10, 16, 0, 0 ); // map 16 to black
        VideoInt( _BIOS_SET_PALETTE + 0x01, 16 << 8, 0, 0 ); // overscan = 16
        for( i = 0; i < 16; ++i ) {
            VideoInt( _BIOS_SET_PALETTE + 0x00, ( i << 8 ) | i, 0, 0 );
        }
        if( _CurrState->vc.numcolors == 2 ) {
            _remappalette( 0, _BLACK );
            _remappalette( 1, _BRIGHTWHITE );
        } else if( _CurrState->vc.numcolors == 4 ) {
            _remappalette( 0, _BLACK );
            _remappalette( 1, _LIGHTCYAN );
            _remappalette( 2, _LIGHTMAGENTA );
            _remappalette( 3, _BRIGHTWHITE );
#if defined( VERSION2 )
        } else if( ( _CurrState->vc.numcolors >= 16 ) && ( _CurrState->vc.numcolors <= 256 ) ) {
#else
        } else if( _CurrState->vc.numcolors >= 16 ) {
#endif
            _RemapNum( _VGA_Colours, 16 );
//          for( i = 0; i < 16; ++i ) {
//              _remappalette( i, _VGA_Colours[ i ] );
//          }
        }
    }
}

#endif
