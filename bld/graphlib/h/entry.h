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


// Alternate entry points for WATFOR-77

// Functions are in uppercase, do not have underscores on the end, and
// perform a jump to the appropriate function.

#if defined( __386__ )
  #define _ALT_ENABLED  0       // not necessary for 32-bit
#else
  #define _ALT_ENABLED  0       // disabled (currently WATFOR-77 doesn't have
#endif                          // enough memory to load the graphics library)


#if _ALT_ENABLED
  #define Entry( f1, f2 )       void f1( void ) {  \
                                   JMP ## f2();    \
                                }
#else
  #define Entry( f1, f2 )
#endif


#if _ALT_ENABLED

void JMP_arc();
void JMP_arc_w();
void JMP_arc_w_87();
void JMP_arc_wxy();
void JMP_clearscreen();
void JMP_displaycursor();
void JMP_ellipse();
void JMP_ellipse_w();
void JMP_ellipse_w_87();
void JMP_ellipse_wxy();
void JMP_floodfill();
void JMP_floodfill_w();
void JMP_floodfill_w_87();
void JMP_getactivepage();
void JMP_getarcinfo();
void JMP_getbkcolor();
void JMP_getcliprgn();
void JMP_getcolor();
void JMP_getcurrentposition();
void JMP_getcurrentposition_w();
void JMP_getfillmask();
void JMP_getfontinfo();
void JMP_getgtextextent();
void JMP_getgtextvector();
void JMP_getimage();
void JMP_getimage_w();
void JMP_getimage_w_87();
void JMP_getimage_wxy();
void JMP_getlinestyle();
void JMP_getlogcoord();
void JMP_getphyscoord();
void JMP_getpixel();
void JMP_getpixel_w();
void JMP_getpixel_w_87();
void JMP_getplotaction();
void JMP_gettextcolor();
void JMP_gettextcursor();
void JMP_gettextextent();
void JMP_gettextposition();
void JMP_gettextsettings();
void JMP_gettextwindow();
void JMP_getvideoconfig();
void JMP_getviewcoord_w();
void JMP_getviewcoord_w_87();
void JMP_getviewcoord_wxy();
void JMP_getvisualpage();
void JMP_getwindowcoord();
void JMP_grstatus();
void JMP_grtext();
void JMP_grtext_w();
void JMP_grtext_w_87();
void JMP_imagesize();
void JMP_imagesize_w();
void JMP_imagesize_w_87();
void JMP_imagesize_wxy();
void JMP_lineto();
void JMP_lineto_w();
void JMP_lineto_w_87();
void JMP_moveto();
void JMP_moveto_w();
void JMP_moveto_w_87();
void JMP_outgtext();
void JMP_outmem();
void JMP_outtext();
void JMP_pie();
void JMP_pie_w();
void JMP_pie_w_87();
void JMP_pie_wxy();
void JMP_polygon();
void JMP_polygon_w();
void JMP_polygon_wxy();
void JMP_putimage();
void JMP_putimage_w();
void JMP_putimage_w_87();
void JMP_rectangle();
void JMP_rectangle_w();
void JMP_rectangle_w_87();
void JMP_rectangle_wxy();
void JMP_registerfonts();
void JMP_remapallpalette();
void JMP_remappalette();
void JMP_scrolltextwindow();
void JMP_selectpalette();
void JMP_setactivepage();
void JMP_setbkcolor();
void JMP_setcharsize();
void JMP_setcharsize_w();
void JMP_setcharsize_w_87();
void JMP_setcharspacing();
void JMP_setcharspacing_w();
void JMP_setcharspacing_w_87();
void JMP_setclip();
void JMP_setcliprgn();
void JMP_setcolor();
void JMP_setfillmask();
void JMP_setfont();
void JMP_setgtextvector();
void JMP_setlinestyle();
void JMP_setlogorg();
void JMP_setpixel();
void JMP_setpixel_w();
void JMP_setpixel_w_87();
void JMP_setplotaction();
void JMP_setstylewrap();
void JMP_settextalign();
void JMP_settextcolor();
void JMP_settextcursor();
void JMP_settextorient();
void JMP_settextpath();
void JMP_settextposition();
void JMP_settextrows();
void JMP_settextwindow();
void JMP_settransparency();
void JMP_setvideomode();
void JMP_setvideomoderows();
void JMP_setviewport();
void JMP_setvisualpage();
void JMP_setwindow();
void JMP_setwindow_87();
void JMP_unregisterfonts();
void JMP_wrapon();
#if defined( _NEC_PC )
void JMP_kanjisize();
void JMP_getkanji();
void JMP_setkanji();
#endif

#pragma aux JMP_arc                     = 0xe9 offset _arc;
#pragma aux JMP_arc_w                   = 0xe9 offset _arc_w;
#pragma aux JMP_arc_w_87                = 0xe9 offset _arc_w_87;
#pragma aux JMP_arc_wxy                 = 0xe9 offset _arc_wxy;
#pragma aux JMP_clearscreen             = 0xe9 offset _clearscreen;
#pragma aux JMP_displaycursor           = 0xe9 offset _displaycursor;
#pragma aux JMP_ellipse                 = 0xe9 offset _ellipse;
#pragma aux JMP_ellipse_w               = 0xe9 offset _ellipse_w;
#pragma aux JMP_ellipse_w_87            = 0xe9 offset _ellipse_w_87;
#pragma aux JMP_ellipse_wxy             = 0xe9 offset _ellipse_wxy;
#pragma aux JMP_floodfill               = 0xe9 offset _floodfill;
#pragma aux JMP_floodfill_w             = 0xe9 offset _floodfill_w;
#pragma aux JMP_floodfill_w_87          = 0xe9 offset _floodfill_w_87;
#pragma aux JMP_getactivepage           = 0xe9 offset _getactivepage;
#pragma aux JMP_getarcinfo              = 0xe9 offset _getarcinfo;
#pragma aux JMP_getbkcolor              = 0xe9 offset _getbkcolor;
#pragma aux JMP_getcliprgn              = 0xe9 offset _getcliprgn;
#pragma aux JMP_getcolor                = 0xe9 offset _getcolor;
#pragma aux JMP_getcurrentposition      = 0xe9 offset _getcurrentposition;
#pragma aux JMP_getcurrentposition_w    = 0xe9 offset _getcurrentposition_w;
#pragma aux JMP_getfillmask             = 0xe9 offset _getfillmask;
#pragma aux JMP_getfontinfo             = 0xe9 offset _getfontinfo;
#pragma aux JMP_getgtextextent          = 0xe9 offset _getgtextextent;
#pragma aux JMP_getgtextvector          = 0xe9 offset _getgtextvector;
#pragma aux JMP_getimage                = 0xe9 offset _getimage;
#pragma aux JMP_getimage_w              = 0xe9 offset _getimage_w;
#pragma aux JMP_getimage_w_87           = 0xe9 offset _getimage_w_87;
#pragma aux JMP_getimage_wxy            = 0xe9 offset _getimage_wxy;
#pragma aux JMP_getlinestyle            = 0xe9 offset _getlinestyle;
#pragma aux JMP_getlogcoord             = 0xe9 offset _getlogcoord;
#pragma aux JMP_getphyscoord            = 0xe9 offset _getphyscoord;
#pragma aux JMP_getpixel                = 0xe9 offset _getpixel;
#pragma aux JMP_getpixel_w              = 0xe9 offset _getpixel_w;
#pragma aux JMP_getpixel_w_87           = 0xe9 offset _getpixel_w_87;
#pragma aux JMP_getplotaction           = 0xe9 offset _getplotaction;
#pragma aux JMP_gettextcolor            = 0xe9 offset _gettextcolor;
#pragma aux JMP_gettextcursor           = 0xe9 offset _gettextcursor;
#pragma aux JMP_gettextextent           = 0xe9 offset _gettextextent;
#pragma aux JMP_gettextposition         = 0xe9 offset _gettextposition;
#pragma aux JMP_gettextsettings         = 0xe9 offset _gettextsettings;
#pragma aux JMP_gettextwindow           = 0xe9 offset _gettextwindow;
#pragma aux JMP_getvideoconfig          = 0xe9 offset _getvideoconfig;
#pragma aux JMP_getviewcoord_w          = 0xe9 offset _getviewcoord_w;
#pragma aux JMP_getviewcoord_w_87       = 0xe9 offset _getviewcoord_w_87;
#pragma aux JMP_getviewcoord_wxy        = 0xe9 offset _getviewcoord_wxy;
#pragma aux JMP_getvisualpage           = 0xe9 offset _getvisualpage;
#pragma aux JMP_getwindowcoord          = 0xe9 offset _getwindowcoord;
#pragma aux JMP_grstatus                = 0xe9 offset _grstatus;
#pragma aux JMP_grtext                  = 0xe9 offset _grtext;
#pragma aux JMP_grtext_w                = 0xe9 offset _grtext_w;
#pragma aux JMP_grtext_w_87             = 0xe9 offset _grtext_w_87;
#pragma aux JMP_imagesize               = 0xe9 offset _imagesize;
#pragma aux JMP_imagesize_w             = 0xe9 offset _imagesize_w;
#pragma aux JMP_imagesize_w_87          = 0xe9 offset _imagesize_w_87;
#pragma aux JMP_imagesize_wxy           = 0xe9 offset _imagesize_wxy;
#pragma aux JMP_lineto                  = 0xe9 offset _lineto;
#pragma aux JMP_lineto_w                = 0xe9 offset _lineto_w;
#pragma aux JMP_lineto_w_87             = 0xe9 offset _lineto_w_87;
#pragma aux JMP_moveto                  = 0xe9 offset _moveto;
#pragma aux JMP_moveto_w                = 0xe9 offset _moveto_w;
#pragma aux JMP_moveto_w_87             = 0xe9 offset _moveto_w_87;
#pragma aux JMP_outgtext                = 0xe9 offset _outgtext;
#pragma aux JMP_outmem                  = 0xe9 offset _outmem;
#pragma aux JMP_outtext                 = 0xe9 offset _outtext;
#pragma aux JMP_pie                     = 0xe9 offset _pie;
#pragma aux JMP_pie_w                   = 0xe9 offset _pie_w;
#pragma aux JMP_pie_w_87                = 0xe9 offset _pie_w_87;
#pragma aux JMP_pie_wxy                 = 0xe9 offset _pie_wxy;
#pragma aux JMP_polygon                 = 0xe9 offset _polygon;
#pragma aux JMP_polygon_w               = 0xe9 offset _polygon_w;
#pragma aux JMP_polygon_wxy             = 0xe9 offset _polygon_wxy;
#pragma aux JMP_putimage                = 0xe9 offset _putimage;
#pragma aux JMP_putimage_w              = 0xe9 offset _putimage_w;
#pragma aux JMP_putimage_w_87           = 0xe9 offset _putimage_w_87;
#pragma aux JMP_rectangle               = 0xe9 offset _rectangle;
#pragma aux JMP_rectangle_w             = 0xe9 offset _rectangle_w;
#pragma aux JMP_rectangle_w_87          = 0xe9 offset _rectangle_w_87;
#pragma aux JMP_rectangle_wxy           = 0xe9 offset _rectangle_wxy;
#pragma aux JMP_registerfonts           = 0xe9 offset _registerfonts;
#pragma aux JMP_remapallpalette         = 0xe9 offset _remapallpalette;
#pragma aux JMP_remappalette            = 0xe9 offset _remappalette;
#pragma aux JMP_scrolltextwindow        = 0xe9 offset _scrolltextwindow;
#pragma aux JMP_selectpalette           = 0xe9 offset _selectpalette;
#pragma aux JMP_setactivepage           = 0xe9 offset _setactivepage;
#pragma aux JMP_setbkcolor              = 0xe9 offset _setbkcolor;
#pragma aux JMP_setcharsize             = 0xe9 offset _setcharsize;
#pragma aux JMP_setcharsize_w           = 0xe9 offset _setcharsize_w;
#pragma aux JMP_setcharsize_w_87        = 0xe9 offset _setcharsize_w_87;
#pragma aux JMP_setcharspacing          = 0xe9 offset _setcharspacing;
#pragma aux JMP_setcharspacing_w        = 0xe9 offset _setcharspacing_w;
#pragma aux JMP_setcharspacing_w_87     = 0xe9 offset _setcharspacing_w_87;
#pragma aux JMP_setclip                 = 0xe9 offset _setclip;
#pragma aux JMP_setcliprgn              = 0xe9 offset _setcliprgn;
#pragma aux JMP_setcolor                = 0xe9 offset _setcolor;
#pragma aux JMP_setfillmask             = 0xe9 offset _setfillmask;
#pragma aux JMP_setfont                 = 0xe9 offset _setfont;
#pragma aux JMP_setgtextvector          = 0xe9 offset _setgtextvector;
#pragma aux JMP_setlinestyle            = 0xe9 offset _setlinestyle;
#pragma aux JMP_setlogorg               = 0xe9 offset _setlogorg;
#pragma aux JMP_setpixel                = 0xe9 offset _setpixel;
#pragma aux JMP_setpixel_w              = 0xe9 offset _setpixel_w;
#pragma aux JMP_setpixel_w_87           = 0xe9 offset _setpixel_w_87;
#pragma aux JMP_setplotaction           = 0xe9 offset _setplotaction;
#pragma aux JMP_setstylewrap            = 0xe9 offset _setstylewrap;
#pragma aux JMP_settextalign            = 0xe9 offset _settextalign;
#pragma aux JMP_settextcolor            = 0xe9 offset _settextcolor;
#pragma aux JMP_settextcursor           = 0xe9 offset _settextcursor;
#pragma aux JMP_settextorient           = 0xe9 offset _settextorient;
#pragma aux JMP_settextpath             = 0xe9 offset _settextpath;
#pragma aux JMP_settextposition         = 0xe9 offset _settextposition;
#pragma aux JMP_settextrows             = 0xe9 offset _settextrows;
#pragma aux JMP_settextwindow           = 0xe9 offset _settextwindow;
#pragma aux JMP_settransparency         = 0xe9 offset _settransparency;
#pragma aux JMP_setvideomode            = 0xe9 offset _setvideomode;
#pragma aux JMP_setvideomoderows        = 0xe9 offset _setvideomoderows;
#pragma aux JMP_setviewport             = 0xe9 offset _setviewport;
#pragma aux JMP_setvisualpage           = 0xe9 offset _setvisualpage;
#pragma aux JMP_setwindow               = 0xe9 offset _setwindow;
#pragma aux JMP_setwindow_87            = 0xe9 offset _setwindow_87;
#pragma aux JMP_unregisterfonts         = 0xe9 offset _unregisterfonts;
#pragma aux JMP_wrapon                  = 0xe9 offset _wrapon;
#if defined( _NEC_PC )
#pragma aux JMP_kanjisize               = 0xe9 offset _kanjisize;
#pragma aux JMP_getkanji                = 0xe9 offset _getkanji;
#pragma aux JMP_setkanji                = 0xe9 offset _setkanji;
#endif

#pragma aux _ARC "*";
#pragma aux _ARC_W "*";
#pragma aux _ARC_W_87 "*";
#pragma aux _ARC_WXY "*";
#pragma aux _CLEARSCREEN "*";
#pragma aux _DISPLAYCURSOR "*";
#pragma aux _ELLIPSE "*";
#pragma aux _ELLIPSE_W "*";
#pragma aux _ELLIPSE_W_87 "*";
#pragma aux _ELLIPSE_WXY "*";
#pragma aux _FLOODFILL "*";
#pragma aux _FLOODFILL_W "*";
#pragma aux _FLOODFILL_W_87 "*";
#pragma aux _GETACTIVEPAGE "*";
#pragma aux _GETARCINFO "*";
#pragma aux _GETBKCOLOR "*";
#pragma aux _GETCLIPRGN "*";
#pragma aux _GETCOLOR "*";
#pragma aux _GETCURRENTPOSITION "*";
#pragma aux _GETCURRENTPOSITION_W "*";
#pragma aux _GETFILLMASK "*";
#pragma aux _GETFONTINFO "*";
#pragma aux _GETGTEXTEXTENT "*";
#pragma aux _GETGTEXTVECTOR "*";
#pragma aux _GETIMAGE "*";
#pragma aux _GETIMAGE_W "*";
#pragma aux _GETIMAGE_W_87 "*";
#pragma aux _GETIMAGE_WXY "*";
#pragma aux _GETLINESTYLE "*";
#pragma aux _GETLOGCOORD "*";
#pragma aux _GETPHYSCOORD "*";
#pragma aux _GETPIXEL "*";
#pragma aux _GETPIXEL_W "*";
#pragma aux _GETPIXEL_W_87 "*";
#pragma aux _GETPLOTACTION "*";
#pragma aux _GETTEXTCOLOR "*";
#pragma aux _GETTEXTCURSOR "*";
#pragma aux _GETTEXTEXTENT "*";
#pragma aux _GETTEXTPOSITION "*";
#pragma aux _GETTEXTSETTINGS "*";
#pragma aux _GETTEXTWINDOW "*";
#pragma aux _GETVIDEOCONFIG "*";
#pragma aux _GETVIEWCOORD_W "*";
#pragma aux _GETVIEWCOORD_W_87 "*";
#pragma aux _GETVIEWCOORD_WXY "*";
#pragma aux _GETVISUALPAGE "*";
#pragma aux _GETWINDOWCOORD "*";
#pragma aux _GRSTATUS "*";
#pragma aux _GRTEXT "*";
#pragma aux _GRTEXT_W "*";
#pragma aux _GRTEXT_W_87 "*";
#pragma aux _IMAGESIZE "*";
#pragma aux _IMAGESIZE_W "*";
#pragma aux _IMAGESIZE_W_87 "*";
#pragma aux _IMAGESIZE_WXY "*";
#pragma aux _LINETO "*";
#pragma aux _LINETO_W "*";
#pragma aux _LINETO_W_87 "*";
#pragma aux _MOVETO "*";
#pragma aux _MOVETO_W "*";
#pragma aux _MOVETO_W_87 "*";
#pragma aux _OUTGTEXT "*";
#pragma aux _OUTMEM "*";
#pragma aux _OUTTEXT "*";
#pragma aux _PIE "*";
#pragma aux _PIE_W "*";
#pragma aux _PIE_W_87 "*";
#pragma aux _PIE_WXY "*";
#pragma aux _POLYGON "*";
#pragma aux _POLYGON_W "*";
#pragma aux _POLYGON_WXY "*";
#pragma aux _PUTIMAGE "*";
#pragma aux _PUTIMAGE_W "*";
#pragma aux _PUTIMAGE_W_87 "*";
#pragma aux _RECTANGLE "*";
#pragma aux _RECTANGLE_W "*";
#pragma aux _RECTANGLE_W_87 "*";
#pragma aux _RECTANGLE_WXY "*";
#pragma aux _REGISTERFONTS "*";
#pragma aux _REMAPALLPALETTE "*";
#pragma aux _REMAPPALETTE "*";
#pragma aux _SCROLLTEXTWINDOW "*";
#pragma aux _SELECTPALETTE "*";
#pragma aux _SETACTIVEPAGE "*";
#pragma aux _SETBKCOLOR "*";
#pragma aux _SETCHARSIZE "*";
#pragma aux _SETCHARSIZE_W "*";
#pragma aux _SETCHARSIZE_W_87 "*";
#pragma aux _SETCHARSPACING "*";
#pragma aux _SETCHARSPACING_W "*";
#pragma aux _SETCHARSPACING_W_87 "*";
#pragma aux _SETCLIP "*";
#pragma aux _SETCLIPRGN "*";
#pragma aux _SETCOLOR "*";
#pragma aux _SETFILLMASK "*";
#pragma aux _SETFONT "*";
#pragma aux _SETGTEXTVECTOR "*";
#pragma aux _SETLINESTYLE "*";
#pragma aux _SETLOGORG "*";
#pragma aux _SETPIXEL "*";
#pragma aux _SETPIXEL_W "*";
#pragma aux _SETPIXEL_W_87 "*";
#pragma aux _SETPLOTACTION "*";
#pragma aux _SETSTYLEWRAP "*";
#pragma aux _SETTEXTALIGN "*";
#pragma aux _SETTEXTCOLOR "*";
#pragma aux _SETTEXTCURSOR "*";
#pragma aux _SETTEXTORIENT "*";
#pragma aux _SETTEXTPATH "*";
#pragma aux _SETTEXTPOSITION "*";
#pragma aux _SETTEXTROWS "*";
#pragma aux _SETTEXTWINDOW "*";
#pragma aux _SETTRANSPARENCY "*";
#pragma aux _SETVIDEOMODE "*";
#pragma aux _SETVIDEOMODEROWS "*";
#pragma aux _SETVIEWPORT "*";
#pragma aux _SETVISUALPAGE "*";
#pragma aux _SETWINDOW "*";
#pragma aux _SETWINDOW_87 "*";
#pragma aux _UNREGISTERFONTS "*";
#pragma aux _WRAPON "*";
#if defined( _NEC_PC )
#pragma aux _KANJISIZE "*";
#pragma aux _GETKANJI "*";
#pragma aux _SETKANJI "*";
#endif

#endif
