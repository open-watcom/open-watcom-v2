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


/*
 Description:
 ============
    This module provides the cover interface from the WATCOM world
    library functions to the WINDOWS functions. These functions
    assume that Display_window has been setup.
*/

#include "icgr.h"
#include <string.h>

static int Width;
static int Height;
static int Win_x_start;
static int Win_y_start;

static float X_start;
static float Y_start;
static float X_width;
static float Y_width;
static float Win_width;
static float Win_height;

static int Log_x;
static int Log_y;

static char Dummy_text[]="M";

static text_dir Text_path;

static WPI_COLOUR       Set_color;
static int      Set_fill_style;
static int      Set_pen_style;
static wl_style Set_wl_style;
static wl_width Set_wl_width;
static bool     Set_end_marker;     // for wide lines

static bool     Save_style_on = FALSE;
static int      Save_style;

static HPEN             Old_pen;
static HBRUSH           Old_brush;

extern void _wsetendmarker(
/*************************/
    bool    end_marker
) {
    Set_end_marker = end_marker;
}

extern bool _wsetstylewrap(
/*************************/
    bool    save_style
) {
    bool    old_save;

    old_save = Save_style_on;
    Save_style_on = save_style;
    Save_style = 0;

    return( old_save );
}

WPI_COLOUR get_palette_color(
/***************************/
/* takes a 0 origin colour index, and returns the RGB for the current chart */

    int                 index
) {
    if( index >= Palette_size ) {
        index = Palette_size - 1;
    }

    return( Curr_palette[index] );
}

void _winitwindow(
/****************/
/* this must be called once before using any of the other window functions */

    WPI_RECT            *area
) {
    int                 left, right, top, bottom;

    _wpi_getintrectvalues( *area, &left, &top, &right, &bottom );
    Width = right - left;
    Height = bottom - top;
    Win_x_start = left;
    Win_y_start = top;

#if 0
    /* if Windows 3.0 worked intelligently, the following code would
       also work. But 'GetDeviceCaps' for metafiles hangs! */
    if( GetDeviceCaps( Win_dc, TECHNOLOGY ) == DT_METAFILE ) {
        /* metafiles don't have arbitary coords... assume square pixels */
        Aspect_ratio = ( float ) Height / (float) Width;
    } else {
        Aspect_ratio = ((float)Height / GetDeviceCaps( Win_dc, LOGPIXELSY ) ) /
                    ( (float)Width / GetDeviceCaps( Win_dc, LOGPIXELSX ) );
    }
#else
    if( Is_metafile ) {
        /* BIG assumption with metafiles: use LAST real device's Aspect
           ratio and logical information */
    } else {
        Log_x = _wpi_devicecapableinch( Win_dc, LOGPIXELSX );
        Log_y = _wpi_devicecapableinch( Win_dc, LOGPIXELSY );
        Aspect_ratio = ((float)Height / Log_y) / ((float)Width / Log_x);
        Pixel_aspect_ratio = (float)Log_y / (float)Log_x;
    }
#endif
}

void WINEXP cgr_aspect_ratio(
/***************************/
/* returns last display aspect ratio. MAKE SURE that cgr_display_chart
   was called once before calling this routine */

    float __far         *aspect
) {
    *aspect = Aspect_ratio;
}

void _wsetwindow(
/***************/
/* setup the window */

    float               xl,
    float               yb,
    float               xr,
    float               yt
) {
    Win_width =  xr - xl;
    Win_height =  yt - yb;
    X_start = xl;
    X_width = 1 / Win_width * Width;
    Y_start = yt;
    Y_width = 1 / Win_height * Height;

}

extern void convert_pt(
/*********************/

    float               x,
    float               y,
    int                 *dx,
    int                 *dy
) {
    *dx =  ( x - X_start ) * X_width + Win_x_start + .5;
    *dy =  ( Y_start - y ) * Y_width + Win_y_start + .5;
    *dy = _wpi_convertheight( *dy, (Win_y_start + Height), Win_y_start );
}

static int points_to_pixel(
/*************************/
/* returns pixels in vert. dir */

    int                 pts
) {
    return( (pts * Log_y) / 72 );
}

float max_psize(
/**************/
/* wierd function which takes percentage size and returns it, or up
   to a max governed by the max point size */

    float               psize,
    int                 max_points
) {
    int                 max_pixel;

    max_pixel = points_to_pixel( max_points );

    if( psize * Height > max_pixel ) {
        return( (float)max_pixel / Height );
    }

    return( psize );
}

WPI_FONT _wtextinit(
/******************/
/* initial text, pass back font handle for _wtextout */

    text_def            *text
) {
    WPI_LOGFONT         font;
    WPI_FONT            font_hld;
    BOOL                is_bold;
    int                 size;
    int                 match_no;

    _wpi_getdeffm( font );
    if( text->style & TEXT_SIZEABLE ) {
        _wpi_setfontsizeable( &font, TRUE );
    }
    if( text->not_set ) {
        _wpi_setfontfacename( &font, NULL );
    } else {
        _wpi_setfontfacename( &font, text->face_name );
    }

#if 0
    size = text->size%100;
    match_no = ((text->size-(text->size%100))/100);;
#else
    size = text->size;
    match_no = 0;
#endif
    _wpi_setfontpointsize( &font, size, points_to_pixel( text->size ),
                match_no );

    _wpi_settextcolor( Win_dc, get_palette_color( text->color - 1 ) );
    is_bold = text->style & TEXT_BOLD;
    _wpi_setfontbold( &font, is_bold );

    /* due to some wierd, undocumented bug in Windows, italics will
       not print on some devices unless the style booleans below
       are set to -1 (255)! We found this out by looking at what
       commdlg.dll returned in the LOGFONT. I don't understand,
       but leave it this way */
    _wpi_setfontitalic( &font, text->style & TEXT_ITALIC );
    _wpi_setfontunderline( &font, text->style & TEXT_UNDERLINE );
    _wpi_setfontstrikeout( &font, text->style & TEXT_STRIKEOUT );
    if( text->not_set ) {
        _wpi_setfontcharset( &font, ANSI_CHARSET );
        _wpi_setfontpitch( &font, DEFAULT_PITCH | FF_ROMAN );
    } else {
        _wpi_setfontcharset( &font, text->char_set );
        _wpi_setfontpitch( &font, text->pitchfamily );
    }

    _wpi_setfontprecision( &font, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS );
    _wpi_setfontquality( &font, DRAFT_QUALITY );

    _wpi_setfontescapement( &font, 0 );
    _wpi_setfontorientation( &font, 0 );

    _wpi_createfont( font, font_hld );

    return( font_hld );
}

void _wtextdone(
/**************/
/* make sure that the font is not currently selected into the display */
    WPI_FONT            font
) {
    _wpi_deletefont( font );
}

static void textdim(
/******************/

    char                *text_line,
    text_def            *text,
    int                 *width,
    int                 *height
) {
    WPI_FONT            font_hld;
    WPI_FONT            old_font;

    if( text_line == NULL ) {
        text_line = Dummy_text;
    }

    font_hld = _wtextinit( text );

    old_font = _wpi_selectfont( Win_dc, font_hld );

    _wpi_gettextextent( Win_dc, text_line, strlen( text_line ), width, height );

    _wpi_getoldfont( Win_dc, old_font );

    _wtextdone( font_hld );
}

static void textdim_font(
/***********************/
    char                *text_line,
    WPI_FONT            font_hld,
    int                 *width,
    int                 *height
) {
    WPI_FONT            old_font;

    if( text_line == NULL ) {
        text_line = Dummy_text;
    }

    font_hld = font_hld;                // unused in Windows
    old_font = _wpi_selectfont( Win_dc, font_hld );
    _wpi_gettextextent( Win_dc, text_line, strlen( text_line ), width, height );
    _wpi_getoldfont( Win_dc, old_font );
}

float _ptextwidth(
/****************/
/* return text width in percentage screen width of text line. If text_line
   is NULL, assume 1 character */

    char                *text_line,
    text_def            *text
) {
    int                 pix_width;
    int                 pix_height;
    float               width;

    textdim( text_line, text, &pix_width, &pix_height );

    width = (float)pix_width / Width;

    return( _min( width, 1.0 ) );
}

float _ptextmaxwidth(
/*******************/

    int                 num_chars,
    text_def            *text
) {
    char                *ptr;
    float               width;

    _new( ptr, num_chars + 1 );

    memset( ptr, 'M', num_chars );
    ptr[num_chars] = '\0';

    width = _ptextwidth( ptr, text );

    _free( ptr );

    return( width );
}

float _ptextheight(
/*****************/
/* return height of 1 char */

    text_def            *text
) {
    int                 pix_width;
    int                 pix_height;
    float               height;

    textdim( NULL, text, &pix_width, &pix_height );

    height = (float)pix_height / Height;

    return( _min( height, 1.0 ) );
}

float _wtextheight(
/*****************/
/* return text height in window coords of text line */

    text_def            *text
) {
    int                 pix_width;
    int                 pix_height;
    float               height;

    textdim( NULL, text, &pix_width, &pix_height );

    height = (float)pix_height / Height * Win_height;

    return( _min( height, Win_height ) );
}

extern float _wtextheight_font(
/*****************************/
/* returns text height in window coords using the given font */
    char *          text,
    WPI_FONT        font
) {
    int             pix_height;
    int             pix_width;
    float           height;

    if( Text_path == TEXT_VERTICAL ) {
        textdim_font( NULL, font, &pix_width, &pix_height );
        pix_height *= strlen( text );
    } else {
        textdim_font( text, font, &pix_width, &pix_height );
    }

    height = (float)pix_height / Height * Win_height;

    return( _min( height, Win_height ) );
}

float _ptextheightsize(
/*********************/
    int                 pt_size,
    text_def            *text
) {
    int                 old_size;
    float               height;

    old_size = text->size;
    text->size = pt_size;
    height = _ptextheight( text );

    text->size = old_size;

    return( height );
}

float _wtextwidth(
/****************/
/* return text width in window coords of text line. If text_line
   is NULL, assume 1 character */

    char                *text_line,
    text_def            *text
) {
    int                 pix_width;
    int                 pix_height;
    float               width;

    textdim( text_line, text, &pix_width, &pix_height );

    width = (float)pix_width / Width * Win_width;

    return( _min( width, Win_width ) );
}

extern float _wtextwidth_font(
/****************************/
/* return text width in window coords of text line. If text_line
   is NULL, assume 1 character */
    char                *text_line,
    WPI_FONT            font
) {
    int                 pix_width;
    int                 pix_height;
    float               width;

    if( Text_path == TEXT_VERTICAL ) {
        textdim_font( NULL, font, &pix_width, &pix_height );
    } else {
        textdim_font( text_line, font, &pix_width, &pix_height );
    }

    width = (float)pix_width / Width * Win_width;

    return( _min( width, Win_width ) );
}

float _wtextmaxwidth(
/*******************/

    int                 num_chars,
    text_def            *text
) {
    char                *ptr;
    float               width;

    _new( ptr, num_chars + 1 );

    memset( ptr, 'M', num_chars );
    ptr[num_chars] = '\0';

    width = _wtextwidth( ptr, text );

    _free( ptr );

    return( width );
}


void _wsetpath(
/*************/
/* path for text */

    text_dir            dir
) {
    Text_path = dir;
}

void _wtextout(
/*************/

    char                *text,
    float               x,
    float               y,
    int                 hor_align,
    int                 ver_align,
    void                *wfont
) {
    WORD                horiz_flags;
    WORD                vert_flags;
    WPI_FONT            old_font;
    int                 dx;
    int                 dy;
    WPI_TEXTMETRIC      font_info;
    int                 len;
    int                 bk_mode;
    int                 px1;
    int                 py1;
    int                 px2;
    int                 py2;
    int                 width;
    int                 height;
    WPI_FONT            font;

    font = (WPI_FONT) wfont;

    bk_mode = _wpi_getbackmode( Win_dc );
    _wpi_setbackmode( Win_dc, TRANSPARENT );

    len = strlen( text );

    convert_pt( x, y, &dx, &dy );

    old_font = _wpi_selectfont( Win_dc, font );

    if( Text_path == TEXT_VERTICAL ) {
        _wpi_gettextmetrics( Win_dc, &font_info );
        width = _wpi_metricmaxcharwidth( font_info );
        height = len * _wpi_metricheight( font_info );
    } else {
        _wpi_gettextextent( Win_dc, text, len, &width, &height );
    }
    px1 = px2 = dx;
    py1 = py2 = dy;

    switch( hor_align ) {

    case TEXT_H_LEFT:
        horiz_flags = TA_LEFT;
        px2 += width;
        break;

    case TEXT_H_CENTER:
        horiz_flags = TA_CENTER;
        px1 -= width / 2;
        px2 += width / 2;
        break;

    case TEXT_H_RIGHT:
        horiz_flags = TA_RIGHT;
        px2 -= width;
        break;
    }

    switch( ver_align ) {

    case TEXT_V_TOP:
        py2 += height * WPI_VERT_MULT;
        break;

    case TEXT_V_CENTER:
        py1 -= height / 2 * WPI_VERT_MULT;
        py2 += height / 2 * WPI_VERT_MULT;
        break;

    case TEXT_V_BOTTOM:
        py2 -= height * WPI_VERT_MULT;
        break;
    }
    rgn_rectangle( px1, py1, px2, py2 );

    if( Text_path == TEXT_HORIZONTAL ) {
        /* normal right path */
        switch( ver_align ) {

        case TEXT_V_TOP:
#ifdef PLAT_OS2
            /* OS/2 has problems with TA_LEFT && TA_TOP aligned text */
            /* This is a messy solution until we actually figure out */
            /* what the problem is.                                  */
            vert_flags = TA_BOTTOM;
            _wpi_gettextmetrics( Win_dc, &font_info );
            dy += _wpi_metricheight( font_info ) * WPI_VERT_MULT;
#else
            vert_flags = TA_TOP;
#endif
            break;

        case TEXT_V_CENTER:
#ifdef PLAT_OS2
            vert_flags = TA_HALF;
#else
            /* OS/2 has a much cleverer way of doing this */
            /* but Windows apparently doesn't             */
            vert_flags = TA_TOP;
            _wpi_gettextmetrics( Win_dc, &font_info );
            dy -= _wpi_metricheight( font_info ) / 2 * WPI_VERT_MULT;
#endif
            break;

        case TEXT_V_BOTTOM:
            vert_flags = TA_BOTTOM;
            break;
        }

        _wpi_settextalign( Win_dc, horiz_flags, vert_flags );
        _wpi_textout( Win_dc, dx, dy, text, len );

    } else {
        /* must display text vertically... do it by hand */

        vert_flags = TA_TOP;
        _wpi_settextalign( Win_dc, horiz_flags, vert_flags );

        switch( ver_align ) {

        case TEXT_V_BOTTOM:
            dy -= _wpi_metricheight( font_info ) * len * WPI_VERT_MULT;
            break;

        case TEXT_V_CENTER:
            dy -= (_wpi_metricheight( font_info ) * len) / 2 * WPI_VERT_MULT;
            break;
        }
        _wpi_settextalign( Win_dc, horiz_flags, vert_flags );

        for( ; len > 0; --len, dy += _wpi_metricheight( font_info )
                                * WPI_VERT_MULT, ++text ) {
            _wpi_textout( Win_dc, dx, dy, text, 1 );
        }
    }
    _wpi_getoldfont( Win_dc, old_font );
    _wpi_setbackmode( Win_dc, bk_mode );
}

extern void _wsetlinewidth(
/*************************/
    line_width      width
) {
    switch( width ) {
    case WIDTH_SINGLE:
        Set_wl_width = LW_SINGLE;
        break;
    case WIDTH_NARROW:
        Set_wl_width = LW_NARROW;
        break;
    case WIDTH_MEDIUM:
        Set_wl_width = LW_MEDIUM;
        break;
    }
}

void _wsetlinestyle(
/******************/

    line_style          style
) {
    switch( style ) {

    case LINE_NONE:
        Set_pen_style = PS_NULL;
        Set_wl_style = LS_NONE;
        break;

    case LINE_SOLID:
        Set_pen_style = PS_SOLID;
        Set_wl_style = LS_SOLID;
        break;

    case LINE_DASHED:
        Set_pen_style = PS_DASH;
        Set_wl_style = LS_DASH;
        break;

    case LINE_DOTTED:
        Set_pen_style = PS_DOT;
        Set_wl_style = LS_DOT;
        break;

    case LINE_MIXED:
        Set_pen_style = PS_DASHDOT;
        Set_wl_style = LS_DASH_DOT;
        break;
    }
}

void _wsetfillstyle(
/******************/

    fill_style          style
) {
    Set_fill_style = style;
}

void APIENTRY cgr_delete_brush(
/*****************************/
    HBRUSH                      brush_hld
) {
    _wpi_deletebrush( brush_hld );
}

HBRUSH WINEXP cgr_make_brush(
/***************************/

    WPI_COLOUR                  color,
    fill_style                  fill
) {
    LOGBRUSH                    brush;
    HBRUSH                      new_brush;

    _wpi_setlogbrushcolour( &brush, color );
    if( fill == FILL_SOLID ) {
        _wpi_setlogbrushsolid( &brush );
    } else if( fill == FILL_HOLLOW ) {
        _wpi_setlogbrushhollow( &brush );
    } else {
        _wpi_setlogbrushstyle( &brush, BS_HATCHED );
        switch( fill ) {

        case FILL_HATCH_LD:
            _wpi_setlogbrushsymbol( &brush, HS_BDIAGONAL );
            break;

        case FILL_HATCH_RD:
            _wpi_setlogbrushsymbol( &brush, HS_FDIAGONAL );
            break;

        case FILL_HATCH_HC:
            _wpi_setlogbrushsymbol( &brush, HS_CROSS );
            break;

        case FILL_HATCH_DC:
            _wpi_setlogbrushsymbol( &brush, HS_DIAGCROSS );
            break;

        case FILL_HATCH_H:
            _wpi_setlogbrushsymbol( &brush, HS_HORIZONTAL );
            break;

        case FILL_HATCH_V:
            _wpi_setlogbrushsymbol( &brush, HS_VERTICAL );
            break;
        }
    }

    new_brush = _wpi_createbrush( &brush );
    return( new_brush );
}

void _wsetcolor(
/**************/

    int         color
) {
    if( color < 0 ) {
        /* indicates that real BLACK is desired */
        Set_color = _wpi_getrgb( 0, 0, 0 );
    } else {
        Set_color = get_palette_color( color );
    }
}

void _wsetrgbcolor(
/*****************/
    WPI_COLOUR  rgb
) {
    Set_color = rgb;
}

static void get_obj_settings(
/***************************/
/* set brush and pen based on fill type, for normal objects (polys, pies)  */

    int                 fill_type,
    HPEN                *pen,
    HBRUSH              *brush
) {

    switch( fill_type ) {

    case FILL_BORDER:           // border only: interior not touched (pen)
        *pen = _wpi_createpen( Set_pen_style, 1, Set_color );
        *brush = _wpi_createnullbrush();
        Old_pen = _wpi_selectpen( Win_dc, *pen );
        Old_brush = _wpi_selectbrush( Win_dc, *brush );
        break;

    case FILL_INTERIOR:         // interior only: border not touched (brush)
        /* Windows has a nasty bug. NULL_PEN generates a 'non-written'
           border. 'Width' = 0 doesn't help either. With NULL_PEN,
           behaviour depends on the primitive: rectangles goof, polygons
           are OK. Anyway, gist is that we gotta live with it! */
        //*pen = CreatePen( PS_SOLID, 0, _wpi_getrgb( 0, 0, 0 ) );
        *pen = _wpi_createnullpen();
        *brush = cgr_make_brush( Set_color, Set_fill_style );
        Old_pen = _wpi_selectpen( Win_dc, *pen );
        Old_brush = _wpi_selectbrush( Win_dc, *brush );
        break;

    case FILL_BORDER_CLEAR:             // border WITH interior erased to bkgd (pen)
        *pen = _wpi_createpen( Set_pen_style, 1, Set_color );
        *brush = cgr_make_brush( GetBkColor( Win_dc ), FILL_SOLID );
        Old_pen = _wpi_selectpen( Win_dc, *pen );
        Old_brush = _wpi_selectbrush( Win_dc, *brush );
        break;

    case FILL_BORDER_FILL:              // border and interior (pen & brush)
        *pen = _wpi_createpen( Set_pen_style, 1, Set_color );
        *brush = cgr_make_brush( Set_color, Set_fill_style );
        Old_pen = _wpi_selectpen( Win_dc, *pen );
        Old_brush = _wpi_selectbrush( Win_dc, *brush );
        break;
    }
}

static void del_obj_settings(
/***************************/

    int                 fill_type,
    HPEN                pen,
    HBRUSH              brush
) {
    if (Old_pen) {
        _wpi_getoldpen( Win_dc, Old_pen );
    }

    if (Old_brush) {
        _wpi_getoldbrush( Win_dc, Old_brush );
    }

    switch( fill_type ) {

    case FILL_BORDER:           // border only: interior not touched (pen)
        _wpi_deletepen( pen );
        _wpi_deletenullbrush( brush );
        break;

    case FILL_INTERIOR:         // interior only: border not touched (brush)
        _wpi_deletenullpen( pen );
        _wpi_deletebrush( brush );
        break;

    case FILL_BORDER_CLEAR:             // border WITH interior erased to bkgd (pen)
    case FILL_BORDER_FILL:              // border and interior (pen & brush)
        _wpi_deletepen( pen );
        _wpi_deletebrush( brush );
        break;
    }
}

void _wpie(
/*********/

    int                 fill_type,
    float               x1,
    float               y1,
    float               x2,
    float               y2,
    float               x3,
    float               y3,
    float               x4,
    float               y4
) {
    HPEN                pen;
    HBRUSH              brush;
    int                 px1, py1;
    int                 px2, py2;
    int                 px3, py3;
    int                 px4, py4;

    get_obj_settings( fill_type, &pen, &brush );

    convert_pt( x1, y1, &px1, &py1 );
    convert_pt( x2, y2, &px2, &py2 );
    convert_pt( x3, y3, &px3, &py3 );
    convert_pt( x4, y4, &px4, &py4 );

    _wpi_pie( Win_dc, px1, py1, px2, py2, px3, py3, px4, py4 );
    rgn_pie( px1, py1, px2, py2, px3, py3, px4, py4 );

    del_obj_settings( fill_type, pen, brush );
}

void _wmoveto(
/************/

    float               x,
    float               y
) {
    int                 px;
    int                 py;
    WPI_POINT           pt;

    convert_pt( x, y, &px, &py );

    pt.x = px;
    pt.y = py;
    _wpi_moveto( Win_dc, &pt );
}

void _wlineto(
/************/

    float               x,
    float               y,
    BOOL                fill_last
) {
    int                 px, py;

    convert_pt( x, y, &px, &py );
    _wline( px, py, fill_last, TRUE, FALSE );
}

void _wline(
/**********/

    int                 px,
    int                 py,
    BOOL                fill_last,
    BOOL                rgn_on,
    BOOL                line_with_two_boxes
) {
    WPI_POINT           initial_pos;

    _wpi_getcurrpos( Win_dc, &initial_pos );

#if 0
    if( Save_style_on ) {
        wide_line_to_style( Win_dc, px, py, Set_wl_width, Set_wl_style,
                            Set_color, &Save_style, Set_end_marker );
    } else {
        wide_line_to( Win_dc, px, py, Set_wl_width, Set_wl_style, Set_color );
    }

    if( rgn_on ) {
        if( line_with_two_boxes ) {
            rgn_line_boxes( initial_pos.x, initial_pos.y, px, py );
        } else {
            rgn_line( initial_pos.x, initial_pos.y, px, py );
        }
    }
#else
    rgn_on = rgn_on;
    line_with_two_boxes = line_with_two_boxes;
    {
        WPI_POINT       pt;

        pt.x = px;
        pt.y = py;
        _wpi_lineto( Win_dc, &pt );
    }

#endif

#ifdef PLAT_WIN
    if( fill_last ) {
        _wpi_setpixel( Win_dc, px, py, Set_color );
    }
#else
    /* in OS/2, the last pixel is always filled. Don't need to do this
       crap. */
    fill_last = fill_last;
#endif
}

void _wpolygon(
/*************/

    int                 fill_type,
    int                 num_pts,
    wcoord              *pts
) {
    _wpolygon_rgn( fill_type, num_pts, pts, TRUE );
}

void _wpolygon_rgn(
/*****************/

    int                 fill_type,
    int                 num_pts,
    wcoord              *pts,
    BOOL                rgn_on
) {
    HPEN                pen;
    HBRUSH              brush;
    WPI_POINT           *p_pts;
    int                 i;

    _new( p_pts, num_pts );

    for( i = 0; i < num_pts; ++i ) {
        /* assumption: sizeof(int)==sizeof(short) */
        convert_pt( pts[i].xcoord, pts[i].ycoord,
                    (int *) &p_pts[i].x, (int *) &p_pts[i].y );
    }

    get_obj_settings( fill_type, &pen, &brush );

    _wpi_polygon( Win_dc, p_pts, num_pts );

    if( rgn_on ) {
        rgn_polygon( p_pts, num_pts );
    }

    del_obj_settings( fill_type, pen, brush );

    _free( p_pts );
}

void _wellipse(
/*************/

    int                 fill_type,
    float               x1,
    float               y1,
    float               x2,
    float               y2
) {
    int                 px1, py1;
    int                 px2, py2;

    convert_pt( x1, y1, &px1, &py1 );
    convert_pt( x2, y2, &px2, &py2 );
    _world_ellipse( fill_type, px1, py1, px2, py2 );
}

void _world_ellipse(
/******************/

    int                 fill_type,
    int                 px1,
    int                 py1,
    int                 px2,
    int                 py2
) {
    HPEN                pen;
    HBRUSH              brush;

    get_obj_settings( fill_type, &pen, &brush );

    _wpi_ellipse( Win_dc, px1, py1, px2, py2 );

    if( fill_type == FILL_BORDER ) {
        rgn_ellipse_border( px1, py1, px2, py2 );
    } else {
        rgn_ellipse( px1, py1, px2, py2 );
    }

    del_obj_settings( fill_type, pen, brush );
}

void _wdot(
/*********/

    float               x,
    float               y
) {
    int                 px, py;

    convert_pt( x, y, &px, &py );
    _wdot_world( px, py );
}

void _wdot_world(
/***************/

    int                 px,
    int                 py
) {
    HPEN                pen;
    HBRUSH              brush;

    get_obj_settings( FILL_BORDER_FILL, &pen, &brush );

    _wpi_rectangle( Win_dc, px, py, ( px + 2 ), ( py + 2 ) );

    del_obj_settings( FILL_BORDER_FILL, pen, brush );
}

void wrgn_rectangle(
/******************/

    float               x1,
    float               y1,
    float               x2,
    float               y2
) {
    int                 px1, py1;
    int                 px2, py2;

    convert_pt( x1, y1, &px1, &py1 );
    convert_pt( x2, y2, &px2, &py2 );

    rgn_rectangle( px1, py1, px2, py2 );
}

void wrgn_polygon(
/****************/

    int                 num_pts,
    wcoord              *pts
) {
    WPI_POINT           *p_pts;
    int                 i;

    _new( p_pts, num_pts );

    for( i = 0; i < num_pts; ++i ) {
        /* assumption: sizeof(int)==sizeof(short) */
        convert_pt( pts[i].xcoord, pts[i].ycoord,
                    (int *) &p_pts[i].x, (int *) &p_pts[i].y );
    }

    rgn_polygon( p_pts, num_pts );

    _free( p_pts );
}

void wrgn_set_add(
/****************/

    wcoord              *pt
) {
    WPI_POINT           win_pt;

    /* assumption: sizeof(int)==sizeof(short) */
    convert_pt( pt->xcoord, pt->ycoord, (int *) &win_pt.x, (int *) &win_pt.y );
    rgn_set_add( &win_pt );
}

void _warc(
/*********/
/* NOTE: this primitive WILL NOT have an associated region */

    int                 fill_type,
    float               x1,
    float               y1,
    float               x2,
    float               y2,
    float               x3,
    float               y3,
    float               x4,
    float               y4
) {
    HPEN                pen;
    HBRUSH              brush;
    int                 px1, py1;
    int                 px2, py2;
    int                 px3, py3;
    int                 px4, py4;

    get_obj_settings( fill_type, &pen, &brush );

    convert_pt( x1, y1, &px1, &py1 );
    convert_pt( x2, y2, &px2, &py2 );
    convert_pt( x3, y3, &px3, &py3 );
    convert_pt( x4, y4, &px4, &py4 );

    _wpi_arc( Win_dc, px1, py1, px2, py2, px3, py3, px4, py4 );

    del_obj_settings( fill_type, pen, brush );
}

void _wrectangle(
/***************/
/* NOTE: this primitive WILL NOT have an associated region */

    int                 fill_type,
    float               x1,
    float               y1,
    float               x2,
    float               y2
) {
    HPEN                pen;
    HBRUSH              brush;
    int                 px1, py1;
    int                 px2, py2;

    get_obj_settings( fill_type, &pen, &brush );

    convert_pt( x1, y1, &px1, &py1 );
    convert_pt( x2, y2, &px2, &py2 );

    _wpi_rectangle( Win_dc, px1, py1, px2, py2 );

    del_obj_settings( fill_type, pen, brush );
}

extern bool _wrectvisible(
/************************/
    float               x1,
    float               y1,
    float               x2,
    float               y2
) {
    int                 px1, py1;
    int                 px2, py2;
    WPI_RECT            rect;
    int                 left, right, top, bottom;

    convert_pt( x1, y1, &px1, &py1 );
    convert_pt( x2, y2, &px2, &py2 );

    left = _min( px1, px2 );
    right = _max( px1, px2 );
    top = _min( py1, py2 );
    bottom = _max( py1, py2 );
    _wpi_setintrectvalues( &rect, left, top, right, bottom );

    return( RectVisible( Win_dc, &rect ) );
}

extern int _left_coord(
/*********************/
) {
    return( Win_x_start );
}

extern int _bottom_coord(
/***********************/
) {
    return( Win_y_start + Height );
}

extern int _right_coord(
/**********************/
) {
    return( Win_x_start + Width );
}

extern int _top_coord(
/********************/
) {
    return( Win_y_start );
}
