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


#include "vi.h"
#include "win.h"
#include "sstyle.h"

static void WriteLongLineMarker( window_id wn, type_style *style, char_info *txt, char_info _FAR *scr, char old )
{
    char_info   info = {0, 0};

    if( wn != CurrentWindow || !EditFlags.MarkLongLines ) {
        return;
    }
    info.cinfo_attr = MAKE_ATTR( Windows[wn], style->background, style->foreground );
    if( EditVars.EndOfLineChar ) {
        info.cinfo_char = EditVars.EndOfLineChar;
    } else {
        info.cinfo_char = old;
    }
    WRITE_SCREEN_DATA( *txt, info );
    WRITE_SCREEN( *scr, info );
}

size_t strlen( const char *__s );

/*
 * displayLineInWindowGeneric - takes an ss_block directly
 */
static vi_rc displayLineInWindowGeneric( window_id wn, int c_line_no,
                                char *text, int start_col, ss_block *ss )
{
    wind                *w;
    char_info           *txt;
    window_id           *over;
    char                *tmp, *otmp;
    char_info           _FAR *scr;
    unsigned            oscr;
    int                 addr, start, end, a, spend;
    int                 cnt1, cnt2, startc, spl;
    char_info           blank = {0, 0};
    char_info           what = {0, 0};
#ifdef __VIO__
    unsigned            tbytes;
#endif
    bool                has_mouse, write_eol;
    int                 ss_i; // index into text, not ss[ss_i]

    if( EditFlags.Quiet ) {
        return( ERR_NO_ERR );
    }
    w = Windows[wn];
    otmp = NULL;
    write_eol = false;
    if( EditFlags.RealTabs ) {
        a = strlen( text );
        otmp = tmp = StaticAlloc();
        ExpandTabsInABuffer( text, a, tmp, EditVars.MaxLine + 1 );
    } else {
        tmp = text;
    }
    tmp += start_col;
    a = strlen( tmp );

    /*
     * find dimensions of line
     */
    if( w->has_border ) {
        if( c_line_no < 1 || c_line_no > w->height - 2 ) {
            if( EditFlags.RealTabs ) {
                StaticFree( otmp );
            }
            return( ERR_WIND_NO_SUCH_LINE );
        }
        start = 1;
        spl = c_line_no;
        spend = end = w->width - 1;
        if( a < end - 1 ) {
            end = a + 1;
        } else if( a >= end ) {
            write_eol = true;
        }
    } else {
        if( c_line_no < 1 || c_line_no > w->height ) {
            if( EditFlags.RealTabs ) {
                StaticFree( otmp );
            }
            return( ERR_WIND_NO_SUCH_LINE );
        }
        start = 0;
        spl = c_line_no - 1;
        spend = end = w->width;
        if( a < end ) {
            end = a;
        } else if( a > end ) {
            write_eol = true;
        }
    }
    startc = start;
    cnt1 = end - start;
    cnt2 = spend - end;

    c_line_no--;
    AccessWindow( w );

    /*
     * initialize
     */
    addr = startc + spl * w->width;
    txt = &(w->text[addr]);
    over = &(w->overlap[addr]);
    oscr = w->x1 + startc + (spl + w->y1) * EditVars.WindMaxWidth;
    scr = &Scrn[oscr];
#ifdef __VIO__
    tbytes = cnt1 + cnt2;
#endif

    ss_i = 0;
    what.cinfo_attr = MAKE_ATTR( w, SEType[ss->type].foreground, SEType[ss->type].background );
    blank.cinfo_char = ' ';

    has_mouse = DisplayMouse( false );

    /*
     * display line
     */
    if( w->overcnt[spl] ) {
        while( cnt1-- != 0 ) {
            what.cinfo_char = *tmp++;
            WRITE_SCREEN_DATA( *txt++, what );
            if( BAD_ID( *over ) ) {
                WRITE_SCREEN( *scr, what );
            }
            over++;
            scr++;
            if( ++ss_i > ss->end ) {
                ss++;
                what.cinfo_attr = MAKE_ATTR( w, SEType[ss->type].foreground, SEType[ss->type].background );
            }
        }
        if( write_eol && BAD_ID( *(over - 1) ) ) {
            WriteLongLineMarker( wn, &SEType[SE_EOFTEXT], txt - 1, scr - 1, *(tmp - 1) );
        } else {
            blank.cinfo_attr = MAKE_ATTR( w, SEType[ss->type].foreground, SEType[ss->type].background );
            while( cnt2-- != 0 ) {
                WRITE_SCREEN_DATA( *txt++, blank );
                if( BAD_ID( *over ) ) {
                    WRITE_SCREEN( *scr, blank );
                }
                over++;
                scr++;
            }
        }
    } else {
        while( cnt1-- != 0 ) {
            what.cinfo_char = *tmp++;
            WRITE_SCREEN_DATA( *txt++, what );
            WRITE_SCREEN( *scr++, what );
            if( ++ss_i > ss->end ) {
                ss++;
                what.cinfo_attr = MAKE_ATTR( w, SEType[ss->type].foreground, SEType[ss->type].background );
            }
        }
        if( write_eol ) {
            WriteLongLineMarker( wn, &SEType[SE_EOFTEXT], txt - 1, scr - 1, *(tmp - 1) );
        } else {
            blank.cinfo_attr = MAKE_ATTR( w, SEType[ss->type].foreground, SEType[ss->type].background );
            while( cnt2-- != 0 ) {
                WRITE_SCREEN_DATA( *txt++, blank );
                WRITE_SCREEN( *scr++, blank );
            }
        }
    }

#ifdef __VIO__
    MyVioShowBuf( oscr, tbytes );
#endif
    if( EditFlags.RealTabs ) {
        StaticFree( otmp );
    }
    ReleaseWindow( w );
    DisplayMouse( has_mouse );
    return( ERR_NO_ERR );

} /* displayLineInWindowGeneric */

/*
 * DisplayLineInWindowWithColor - do just that
 */
vi_rc DisplayLineInWindowWithColor( window_id wn, int c_line_no, char *text, type_style *ts, int start_col )
{
    ss_block    ss;

    SEType[SE_UNUSED].foreground = ts->foreground;
    SEType[SE_UNUSED].background = ts->background;
    SEType[SE_UNUSED].font = FONT_DEFAULT;
    ss.type = SE_UNUSED;
    ss.end = BEYOND_TEXT;

    return( displayLineInWindowGeneric( wn, c_line_no, text, start_col, &ss ) );
}

/*
 * DisplayLineInWindowWithSyntaxStyle - display wrt syntax lang. settings
 */
vi_rc DisplayLineInWindowWithSyntaxStyle( window_id wn, int c_line_no, line *line,
                                        linenum line_no, char *text, int start_col,
                                        unsigned int junk )
{
    static ss_block     ss[MAX_SS_BLOCKS];
    int                 dummy;
    bool                saveRealTabs;
    int                 a;
    vi_rc               rc;
    char                *tmp;
//    dc_line             *c_line;

    junk = junk;
    if( EditFlags.RealTabs ) {
        a = strlen( text );
        tmp = StaticAlloc();
        ExpandTabsInABuffer( text, a, tmp, EditVars.MaxLine + 1 );
    } else {
        tmp = text;
    }

    // this code commented out cause it doesn't quite work.
    // it should be close considering it mirrors the WINDOWS version.

    // get the laguage flags state previous to this line
    // c_line = DCFindLine( c_line_no - 1, wn );
    // SSGetLanguageFlags( &(c_line->flags) );

    // parse the line (generate new flags as well)
    ss[0].end = BEYOND_TEXT;
    SSDifBlock( ss, tmp, start_col, line, line_no, &dummy );

    // prevent displayLineInWindowGeneric from expanding tabs - blech
    saveRealTabs = EditFlags.RealTabs;
    EditFlags.RealTabs = false;

    // display the thing
    rc = displayLineInWindowGeneric( wn, c_line_no, tmp, start_col, ss );
    EditFlags.RealTabs = saveRealTabs;

    // now say that it has been displayed and the flags are OK
    // DCValidateLine( c_line, start_col, tmp );

    if( EditFlags.RealTabs ) {
        StaticFree( tmp );
    }
    return( rc );
}

/*
 * DisplayCrossLineInWindow - do just that, using given colors
 */
void DisplayCrossLineInWindow( window_id wn, int line )
{
    wind                *w;
    window_id           *over;
    char_info           *txt;
    char_info           _FAR *scr;
    unsigned            oscr;
    int                 addr, i;
    char_info           what = {0, 0};

    if( EditFlags.Quiet ) {
        return;
    }
    w = Windows[wn];

    /*
     * find dimensions of line
     */
    if( !w->has_border ) {
        return;
    }
    if( line < 1 || line > w->height - 2 ) {
        return;
    }
    line--;

    /*
     * initialize
     */
    AccessWindow( w );
    addr = 1 + (1 + line) * w->width;
    txt = &(w->text[addr]);
    over = &(w->overlap[addr]);
    oscr = w->x1 + (1 + line + w->y1) * EditVars.WindMaxWidth;
    scr = &Scrn[oscr];
    what.cinfo_attr = MAKE_ATTR( w, w->border_color1, w->border_color2 );
    what.cinfo_char = WindowBordersNG[WB_LEFTT];

    WRITE_SCREEN_DATA( *txt++, what );
    if( BAD_ID( *over ) ) {
        WRITE_SCREEN( *scr, what );
    }
    over++;
    scr++;

    what.cinfo_char = WindowBordersNG[WB_TOPBOTTOM];
    for( i = w->x1 + 1; i < w->x2; i++ ) {
        WRITE_SCREEN_DATA( *txt++, what );
        if( BAD_ID( *over ) ) {
            WRITE_SCREEN( *scr, what );
        }
        over++;
        scr++;
    }

    if( line != w->height - 3 && line != 0 ) {
        what.cinfo_char = WindowBordersNG[WB_RIGHTT];
        WRITE_SCREEN_DATA( *txt, what );
        if( BAD_ID( *over ) ) {
            WRITE_SCREEN( *scr, what );
        }
    }

#ifdef __VIO__
    MyVioShowBuf( oscr, w->width );
#endif
    ReleaseWindow( w );

} /* DisplayCrossLineInWindow */

/*
 * changeColorOfDisplayLine - do just that, using given colors and column range
 */
static void changeColorOfDisplayLine( int line, int scol, int ecol, type_style *s )
{
    wind                *w;
    window_id           *over;
    char_info           _FAR *scr;
    unsigned            oscr;
    char_info           what = {0, 0};
#ifdef __VIO__
    unsigned            onscr;
#endif
    viattr_t            attr;
    int                  end, spend, cnt1, cnt2, sscol, spl;

    if( EditFlags.Quiet ) {
        return;
    }
    w = Windows[CurrentWindow];
    AccessWindow( w );

    /*
     * find dimensions of line
     */
    if( w->has_border ) {
        if( line < 1 || line > w->height - 2 ) {
            ReleaseWindow( w );
            return;
        }
        spl = line;
        sscol = 1 + scol;
        spend = end = ecol + 2;
        if( spend > w->width - 1 ) {
            spend = end = w->width - 1;
        }
        if( sscol < 1 ) {
            sscol = 1;
        }
    } else {
        if( line < 1 || line > w->height ) {
            ReleaseWindow( w );
            return;
        }
        spl = line - 1;
        sscol = scol;
        spend = end = ecol + 1;
        if( spend > w->width ) {
            spend = end = w->width;
        }
        if( sscol < 0 ) {
            sscol = 0;
        }
    }
    cnt1 = end - sscol;
    cnt2 = spend - end;

    line--;

    /*
     * initialize
     */
    oscr = w->x1 + sscol + (spl + w->y1) * EditVars.WindMaxWidth;
    scr = &Scrn[oscr];
#ifdef __VIO__
    onscr = 0;
#endif
    attr = MAKE_ATTR( w, s->foreground, s->background );
    what.cinfo_attr = attr;

    /*
     * display line
     */
    if( w->overcnt[spl] ) {
        over = w->overlap + sscol + spl * w->width;
        while( cnt1-- != 0 ) {
            if( BAD_ID( *over ) ) {
                what.cinfo_char = scr->cinfo_char;
                WRITE_SCREEN( *scr, what );
#ifdef __VIO__
                onscr++;
#endif
            }
            over++;
            scr++;
        }
        while( cnt2-- != 0 ) {
            if( BAD_ID( *over ) ) {
                what.cinfo_char = scr->cinfo_char;
                WRITE_SCREEN( *scr, what );
#ifdef __VIO__
                onscr++;
#endif
            }
            over++;
            scr++;
        }
    } else {
        while( cnt1-- != 0 ) {
            what.cinfo_char = scr->cinfo_char;
            WRITE_SCREEN( *scr++, what );
#ifdef __VIO__
            onscr++;
#endif
        }
        while( cnt2-- != 0 ) {
            what.cinfo_char = scr->cinfo_char;
            WRITE_SCREEN( *scr++, what );
#ifdef __VIO__
            onscr++;
#endif
        }
    }
#ifdef __VIO__
    MyVioShowBuf( oscr, onscr );
#endif
    ReleaseWindow( w );

} /* changeColorOfDisplayLine */

/*
 * HiliteAColumnRange - color a specified word on a line
 */
void HiliteAColumnRange( linenum line, int scol, int ecol )
{
    int s, e;

    s = VirtualColumnOnCurrentLine( scol );
    e = VirtualColumnOnCurrentLine( ecol + 1 ) - 1;
    if( scol == 0 ) {
        s = 0;
    }
    changeColorOfDisplayLine( (int)(line - LeftTopPos.line + 1),
        s - LeftTopPos.column, e - LeftTopPos.column, &editw_info.hilight_style );

} /* HiliteAColumnRange */

/*
 * ColorAColumnRange - color a specified word on a line
 */
void ColorAColumnRange( int row, int scol, int ecol, type_style *style )
{
    int s, e, t;

    s = VirtualColumnOnCurrentLine( scol );
    e = VirtualColumnOnCurrentLine( ecol );
    if( s > e ) {
        t = s;
        s = e;
        e = t;
    }

    s--;
    e--;
    changeColorOfDisplayLine( row, s - LeftTopPos.column, e - LeftTopPos.column, style );

} /* ColorAColumnRange */

/*
 * SetCharInWindowWithColor - do just that, using given colors
 */
vi_rc SetCharInWindowWithColor( window_id wn, int line, int col, char text, type_style *style )
{
    wind                *w;
    int                 addr, start, spl;
    char_info           tmp = {0, 0};
    bool                has_mouse;
    unsigned            oscr;

    if( EditFlags.Quiet ) {
        return( ERR_NO_ERR );
    }
    w = Windows[wn];

    /*
     * find dimensions of line
     */
    if( w->has_border ) {
        if( line < 1 || line > w->height - 2 ) {
            return( ERR_WIND_NO_SUCH_LINE );
        }
        start = 1;
        spl = line;
    } else {
        if( line < 1 || line > w->height ) {
            return( ERR_WIND_NO_SUCH_LINE );
        }
        start = 0;
        spl = line - 1;
    }
    if( col < 1 || col > w->width ) {
        return( ERR_WIND_NO_SUCH_COLUMN );
    }
    line--;
    col--;

    /*
     * initialize
     */
    AccessWindow( w );
    addr = col + start + spl * w->width;
    oscr = w->x1 + start + col + (spl + w->y1) * EditVars.WindMaxWidth;
    tmp.cinfo_attr = MAKE_ATTR( w, style->foreground, style->background );
    tmp.cinfo_char = text;

    /*
     * display char
     */
    has_mouse = DisplayMouse( false );
    WRITE_SCREEN_DATA( w->text[addr], tmp );
    if( BAD_ID( w->overlap[addr] ) ) {
        WRITE_SCREEN( Scrn[oscr], tmp );
    }
#ifdef __VIO__
    MyVioShowBuf( oscr, 1 );
#endif

    ReleaseWindow( w );
    DisplayMouse( has_mouse );
    return( ERR_NO_ERR );

} /* SetCharInWindowWithColor */

/*
 * DisplayLineInWindow - do as it sounds, use default colors
 */
vi_rc DisplayLineInWindow( window_id wn, int c_line_no, char *text )
{
    ss_block    ss;
    wind        *w;

    w = Windows[wn];
    SEType[SE_UNUSED].foreground = w->text_color;
    SEType[SE_UNUSED].background = w->background_color;
    SEType[SE_UNUSED].font = FONT_DEFAULT;
    ss.type = SE_UNUSED;
    ss.end = BEYOND_TEXT;
    return( displayLineInWindowGeneric( wn, c_line_no, text, 0, &ss ) );

} /* DisplayLineInWindow */
