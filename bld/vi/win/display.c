/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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


#include "vi.h"
#include "winaux.h"
#include "win.h"
#include "color.h"
#include "font.h"
#include "utils.h"
#include "sstyle.h"

static void funnyFix( RECT *rect, int x, window_id id, char *display, int len,
                      HDC hdc, int max_width, type_style *style, HBRUSH thisBrush );

void MyTabbedTextOut( HDC hdc, char **display, int len,
                      bool funny_italic, POINT *p, type_style *style, RECT *rect,
                      window_id id, char *otmp, int y );

bool                AllowDisplay = true;
static int          pageCnt;
static font_type    lastFont, thisFont;
static vi_color     lastFore, thisFore;
static vi_color     lastBack, thisBack;
static HBRUSH       thisBrush;


void ScreenPage( int page )
{
    pageCnt += page;
    AllowDisplay = ( pageCnt == 0 );
}

void WindowTitleAOI( window_id id, const char *title, bool active )
{
    active = active;
    if( !BAD_ID( id ) ) {
        SetWindowText( id, title );
    }
}

void WindowTitle( window_id id, const char *title )
{
    WindowTitleAOI( id, title, false );
}

void ClearWindow( window_id id )
{
    RECT        rect;
    window      *w;
    HDC         hdc;

    if( !AllowDisplay || BAD_ID( id ) ) {
        return;
    }
    w = WINDOW_FROM_ID( id );
    GetClientRect( id, &rect );
    hdc = TextGetDC( id, WIN_TEXT_STYLE( w ) );
    // should clear with SEType[SE_WHITESPACE].background for edit windows
    FillRect( hdc, &rect, ColorBrush( WIN_TEXT_BACKCOLOR( w ) ) );
    TextReleaseDC( id, hdc );
}

vi_rc DisplayLineInWindow( window_id id, int line, char *text )
{
    text = text;
    id = id;
    DCDisplaySomeLines( line - 1, line - 1 );
    return( ERR_NO_ERR );
}

void ShiftWindowUpDown( window_id id, int lines )
{
    int         change, height;
    window      *w;
    window_data *wd;
    RECT        clip_rect;

    if( lines == 0 || !AllowDisplay || BAD_ID( id ) ) {
        return;
    }
    w = WINDOW_FROM_ID( id );
    height = FontHeight( WIN_TEXT_FONT( w ) );
    change = -lines * height;
    DCScroll( -lines );

    MyHideCaret( id );
    wd = DATA_FROM_ID( id );
    // don't scroll extra bit bit at bottom
    // clip extra bit in case scrolling w/ positive change
    GetClientRect( id, &clip_rect );
    clip_rect.bottom = wd->extra.top;
    if( change > 0 ) {
        // dont scroll into extra bit
        ScrollWindow( id, 0, change, &clip_rect, &clip_rect );
    } else {
        ScrollWindow( id, 0, change, &clip_rect, NULL );
    }
    UpdateWindow( id );
    MyShowCaret( id );

    wd = wd;
    clip_rect.top = 0;

} /* ShiftWindowUpDown */

bool SetDrawingObjects( HDC hdc, type_style *style )
{
    static bool funny_italic = false;

    // setup font and colours for next string.
    thisFore = style->foreground;
    if( lastFore != thisFore ) {
        SetTextColor( hdc, ColorRGB( thisFore ) );
        lastFore = thisFore;
    }

    thisBack = style->background;
    if( lastBack != thisBack ) {
        SetBkColor( hdc, ColorRGB( thisBack ) );
        thisBrush = ColorBrush( style->background );
        lastBack = thisBack;
    }

    thisFont = style->font;
    if( lastFont != thisFont ) {
        SelectObject( hdc, FontHandle( thisFont ) );
        lastFont = thisFont;
        if( FontIsFunnyItalic( thisFont ) ) {
            funny_italic = true;
        } else {
            funny_italic = false;
        }
    }
    return( funny_italic );
}

#ifndef BITBLT_BUFFER_DISPLAY

static void funnyFix( RECT *rect, int x, window_id id, char *display, int len,
                      HDC hdc, int max_width, type_style *style, HBRUSH brush )
{
    // FunnyItalic so draw at bit at begining and end!
    RECT    smallrect;
    int     advance;
    int     width;

    smallrect.top = rect->top;
    smallrect.bottom = rect->bottom;

    // draw bit at the beginning
    smallrect.left = x;
    width = MyTextExtent( id, style, display, len );
    if( max_width > width )
        max_width = width;
    smallrect.right = max_width + x;
    FillRect( hdc, &smallrect, brush );

    // and at the end
    advance = x + width;
    smallrect.left = advance - max_width;
    if( smallrect.left < smallrect.right )
        smallrect.left = smallrect.right;
    smallrect.right = advance;
    FillRect( hdc, &smallrect, brush );
}

void MyTabbedTextOut( HDC hdc,
                      char **display,        // a reference to a string
                      int len,               // number of chars to display
                      bool funny_italic,     // fix up begin and end ?
                      POINT *p,              // reference to current position
                      type_style *style,     // current style
                      RECT *rect,
                      window_id id,
                      char *otmp,
                      int y )
{
    if( EditFlags.RealTabs ) {
        char    *tstring = *display;
        char    *string_end = tstring + len;
        int     tlen;
        RECT    new;

        while( tstring < string_end ) {
            tlen = 0;
            while( *tstring != '\t' ){
                // BAD! Kevin.P.
                // I think this portion of code is 8-bit dependant.
                // Should call getNext() or something from TabHell
                if( tstring == string_end ) {
                    break;
                }
                tstring++;
                tlen++;
            }
            if( funny_italic ) {
                // FunnyItalic so draw at bit at begining and end!
                funnyFix( rect, p->x, id, *display, tlen,
                          hdc, FontMaxWidth( thisFont ), style, thisBrush );
            }
            TextOut( hdc, 0, 0, *display, tlen );
            *display = tstring;
            if( tstring >= string_end ) {
                break;
            }

            tlen = 0;
            while( *tstring == '\t' ) {
                if( tstring == string_end ) {
                    break;
                }
                tstring++;
                tlen++;
            }
            if( tlen == 0 ) {
                break;
            }
            *display = tstring;

            GetCurrentPositionEx( hdc, p );
            new.left = p->x;
            new.right = (WinVirtualCursorPosition( otmp, tstring - otmp ) -
                         LeftTopPos.column) * FontAverageWidth( thisFont );
            new.top = rect->top;
            new.bottom = rect->bottom;
            FillRect(hdc, &new, thisBrush );
            MoveToEx( hdc, new.right, y, NULL );
            GetCurrentPositionEx( hdc, p );
        }
    } else {
        if( funny_italic ) {
            // FunnyItalic so draw at bit at begining and end!
            funnyFix( rect, p->x, id, *display, len,
                      hdc, FontMaxWidth( thisFont ), style, thisBrush );
        }
        TextOut( hdc, 0, 0, *display, len );
        *display += len;
    }
}

int DisplayLineInWindowWithSyntaxStyle( window_id id, int c_line_no,
    line *line, linenum line_no, char *text, int start_col, HDC hdc )
{
    char        *display, *old;
    char        *tmp, *otmp;
    dc_line     *c_line;
    RECT        rect;
    int         height, len;
    int         x, y, indent;
    bool        changed;
    int         ssDifIndex;
    ss_block    *ss_cache, *ss_step;
    int         lastPos;
    type_style  *style;
    POINT       p;
    bool        funny_italic = false;
    int         prev_col;

    if( !AllowDisplay || BAD_ID( id ) ) {
        return( ERR_NO_ERR );
    }

    /* all font heights should be the same
     *   - note this may not quite be true for bold fonts but so what!
     */
    height = FontHeight( SEType[SE_WHITESPACE].font );
    y = (c_line_no - 1) * height;
    GetClientRect( id, &rect );
    rect.top = y;
    rect.bottom = y + height;

    // set up tabs for drawing
    if( !EditFlags.RealTabs ) {
#if defined( __WATCOMC__) && !defined( __ALPHA__ )
        len = _inline_strlen( text );
#else
        len = strlen( text );
#endif
        otmp = tmp = StaticAlloc();
        ExpandTabsInABuffer( text, len, tmp, EditVars.MaxLine + 1 );
    } else {
        // leave the tabs alone ...
        // let tabbedTextExtent and tabbedTextOut do the rest.
        otmp = tmp = text;
    }

    // check out common text

    prev_col = start_col;
    if( EditFlags.RealTabs ){
        start_col = WinRealCursorPosition( otmp, start_col + 1 ) -1;
    }

    tmp += start_col;
    display = tmp;

    // this section of code makes the ss blocks for this line.
    // it also compares the new blocks to the ones which existed
    // so that we can draw less ( although its not very good at it )
    x = 0;
    c_line = DCFindLine( c_line_no - 1, id );
    SSGetLanguageFlags( &(c_line->flags) );
    ss_cache = c_line->ss;
    indent = 0;
    changed = true;
    ss_step = NULL;
    if( c_line->valid && c_line->start_col == start_col ) {
        /* do not redraw whatever is in common */
        old = c_line->text;

        SSDifBlock( ss_cache, otmp, start_col, line, line_no, &ssDifIndex );

        while( *old == *display ) {
            if( *old == 0 || indent == ssDifIndex ) {
                break;
            }
            old++;
            display++;
            indent++;
        }
        if( *old == *display && indent < ssDifIndex ) {
            changed = false;
        } else {
            // jump ss_step to first block we are actually going to use
            ss_step = ss_cache;
            while( ss_step->end < indent ) {
                ss_step++;
            }
            // grap pixel offset of where to start next block
            x = 0;
            if( ss_step != ss_cache ) {
                x = (ss_step - 1)->offset;
            }
        }
    } else {
        SSDifBlock( ss_cache, otmp, start_col, line, line_no, &ssDifIndex );
        // start at beginning of line
        ss_step = ss_cache;
        x = 0;
    }

    // this section of code performs the drawing of the current line
    // to the display, by interpreting the ss_blocks.
    // The function MyTabbedTextOut prints one ss_block at a time,
    // and updates the position variables. The function
    // SetDrawingObjects updates the font colors and brush for the
    // current ss_block if they have changed.
    if( changed ) {

        lastPos = indent - 1;
        MoveToEx( hdc, x, y, NULL );
        p.x = x;
        SetTextAlign( hdc, TA_UPDATECP );
        lastFont  = -1;
        lastBack  = -1;
        lastFore  = -1;

        // check if we are at the last block OR the remainder
        // of the line is "BEYOND_TEXT" *sigh*
        while( ss_step->end != BEYOND_TEXT ) {

            // setup font and colors for next string.
            style = &SEType[ss_step->type];
            funny_italic = SetDrawingObjects( hdc, style );
            len = ss_step->end - lastPos;

            // MyTabbedTextOut is long and used in 2 places but needs so
            // many arguments maybe it should be inline.
            MyTabbedTextOut( hdc, &display, len, funny_italic,
                             &p, style, &rect, id, otmp, y );

            // save pixel offset where next block is to start
            GetCurrentPositionEx( hdc, &p );
            ss_step->offset = p.x;

            if( p.x > rect.right ) {
                // gone off the edge of the display!
                // put in dummy offsets for the rest of the blocks
                // and exit
                ss_step++;
                for( ;; ) {
                    if( ss_step->end == BEYOND_TEXT ) {
                        break;
                    }
                    ss_step->offset = 10000;
                    ss_step++;
                }
                ss_step->offset = 10000;
                DCValidateLine( c_line, prev_col, tmp );
                if( !EditFlags.RealTabs ) {
                    StaticFree( otmp );
                }
                return( ERR_NO_ERR );
            }

            // advance to the next block
            ss_step++;
            lastPos += len;
        }

        // now "beyond text" but there still could be more "text"
        // if there is, display the rest of it!
        style = &SEType[ss_step->type];
        funny_italic = SetDrawingObjects( hdc, style );
        len = strlen( display );

        if( *display != '\0' ) {
            MyTabbedTextOut( hdc, &display, len, funny_italic,
                             &p, style, &rect, id, otmp, y );
        }

        // if the previous line was longer than this one, blot it out.
        GetCurrentPositionEx( hdc, &p );
        rect.left += p.x;
        FillRect( hdc, &rect, thisBrush );

        // and set a dummy offset for this last block
        ss_step->offset = 10000;

        // line is now completely drawn. Validate it.
        DCValidateLine( c_line, prev_col, tmp );
    }

    if( !EditFlags.RealTabs ) {
        StaticFree( otmp );
    }
    return( ERR_NO_ERR );
}

#else

// unfortunately bitblting each line is considerably slower on standard
// vga, and at best only comparable to direct TextOut on Window accelerators
int DisplayLineInWindowWithSyntaxStyle( window_id id, int c_line_no,
    line *line, linenum line_no, char *text, int start_col,
    HDC hdc_wnd, HDC hdc_mem )
{
    char        *display, *old;
    char        *tmp, *otmp;
    dc_line     *c_line;
    RECT        rect;
    int         width, height, len;
    int         x, y, indent;
    bool        changed;
    int         ssDifIndex;
    ss_block    *ss_cache, *ss_step;
    int         lastPos;
    font_type   lastFont, thisFont;
    vi_color    lastFore, thisFore;
    vi_color    lastBack, thisBack;
    type_style  *style;
    POINT       p;

    if( !AllowDisplay || BAD_ID( id ) ) {
        return( ERR_NO_ERR );
    }

    // all font heights should be the same
    height = FontHeight( SEType[SE_WHITESPACE].font );
    width = WindowAuxInfo( CurrentWindow, WIND_INFO_WIDTH );
    y = (c_line_no - 1) * height;
    GetClientRect( id, &rect );
    rect.top = y;
    rect.bottom = y + height;

    if( EditFlags.RealTabs ) {
#if defined( __WATCOMC__) && !defined( __ALPHA__ )
        len = _inline_strlen( text );
#else
        len = strlen( text );
#endif
        otmp = tmp = StaticAlloc();
        ExpandTabsInABuffer( text, len, tmp, EditVars.MaxLine + 1 );
    } else {
        otmp = tmp = text;
    }
    tmp += start_col;
    display = tmp;
    x = 0;
    c_line = DCFindLine( c_line_no - 1, id );
    SSGetLanguageFlags( &(c_line->flags) );
    ss_cache = c_line->ss;
    indent = 0;
    changed = true;
    if( c_line->valid && c_line->start_col == start_col ) {
        // do not redraw whatever is in common
        old = c_line->text;
        SSDifBlock( ss_cache, otmp, start_col, line, line_no, &ssDifIndex );
        while( *old == *display ) {
            if( *old == 0 || indent == ssDifIndex ) {
                break;
            }
            old++;
            display++;
            indent++;
        }
        if( *old == *display && indent < ssDifIndex ) {
            changed = false;
        } else {
            // jump ss_step to first block we are actually going to use
            ss_step = ss_cache;
            while( ss_step->end < indent ) {
                ss_step++;
            }
            // grap pixel offset of where to start next block
            x = 0;
            if( ss_step != ss_cache ) {
                x = (ss_step - 1)->offset;
            }
        }
    } else {
        SSDifBlock( ss_cache, otmp, start_col, line, line_no, &ssDifIndex );
        // start at beginning of line
        ss_step = ss_cache;
        x = 0;
    }
    if( changed ) {

        PatBlt( hdc_mem, 0, 0, 10000, 10000, PATCOPY );

        lastPos = indent - 1;
        MoveToEx( hdc_mem, x, 0, NULL );
        SetTextAlign( hdc_mem, TA_UPDATECP );
        lastFore = lastBack = lastFont = -1;
        while( ss_step->end != BEYOND_TEXT ) {
            style = &SEType[ss_step->type];
            thisFore = style->foreground;
            if( lastFore != thisFore ) {
                SetTextColor( hdc_mem, ColorRGB( thisFore ) );
                lastFore = thisFore;
            }
            thisBack = style->background;
            if( lastBack != thisBack ) {
                SetBkColor( hdc_mem, ColorRGB( thisBack ) );
                lastBack = thisBack;
            }
            thisFont = style->font;
            if( lastFont != thisFont ) {
                SelectObject( hdc_mem, FontHandle( thisFont ) );
                lastFont = thisFont;
            }
            len = ss_step->end - lastPos;
            TextOut( hdc_mem, 0, 0, display, len );
            // save pixel offset where next block is to start
            // ss_step->offset = LOWORD( GetCurrentPosition( hdc_mem ) );
            GetCurrentPositionEx( hdc, &p );            // BBB - Jan 6, 1994
            ss_step->offset = p.x;
            lastPos += len;
            display += len;
            ss_step++;
        }
        style = &SEType[ss_step->type];
        if( *display != '\0' ) {
            SetTextColor( hdc_mem, ColorRGB( style->foreground ) );
            SetBkColor( hdc_mem, ColorRGB( style->background ) );
            SelectObject( hdc_mem, FontHandle( style->font ) );
            TextOut( hdc_mem, 0, 0, display, strlen( display ) );
        }
        ss_step->offset = 10000;

        BitBlt( hdc_wnd, 0, y, width, height, hdc_mem, 0, 0, SRCCOPY );

        DCValidateLine( c_line, start_col, tmp );
    }

    if( EditFlags.RealTabs ) {
        StaticFree( otmp );
    }
    return( ERR_NO_ERR );
}

#endif
