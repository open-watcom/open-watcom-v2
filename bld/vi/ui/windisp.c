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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "vi.h"
#include "win.h"
#include "sstyle.h"

static void WriteLongLineMarker( window_id wn, type_style *style,
        char_info *txt, char_info _FAR *scr, char old )
{
    char_info   info;

    if( wn != CurrentWindow || !EditFlags.MarkLongLines ) return;
    info.attr = MAKE_ATTR( Windows[ wn ], style->background, style->foreground );
    if( EndOfLineChar ) {
        info.ch = EndOfLineChar;
    } else {
        info.ch = old;
    }
    WRITE_SCREEN_DATA( *txt, info );
    WRITE_SCREEN( *scr, info );
}

size_t strlen( const char *__s );
/*
 * displayLineInWindowGeneric - takes an ss_block directly
 */
int displayLineInWindowGeneric( window_id wn, int c_line_no,
                                char *text, int start_col, ss_block *ss )
{
    wind                *w;
    char_info           *txt;
    char                *over,*tmp,*otmp;
    char_info           _FAR *scr;
    int                 addr,start,end,a,spend;
    int                 cnt1,cnt2,startc,spl;
    char_info           blank,what;
#ifdef __VIO__
    unsigned            oscr;
    unsigned            tbytes;
#endif
    bool                has_mouse, write_eol;
    unsigned            ss_i; // index into text, not ss[ss_i]

    if( EditFlags.Quiet ) {
        return( ERR_NO_ERR );
    }
    w = Windows[ wn ];

    write_eol = FALSE;
    if( EditFlags.RealTabs ) {
        a = strlen( text );
        otmp = tmp = StaticAlloc();
        ExpandTabsInABuffer( text, a, tmp, MaxLinem1 + 2 );
    } else {
        tmp = text;
    }
    tmp += start_col;
    a = strlen( tmp );

    /*
     * find dimensions of line
     */
    if( w->has_border ) {
        if( c_line_no < 1 || c_line_no > w->height-2 ) {
            if( EditFlags.RealTabs ) {
                StaticFree( otmp );
            }
            return( ERR_WIND_NO_SUCH_LINE );
        }
        start = 1;
        spl = c_line_no;
        spend = end = w->width-1;
        if( a < end-1 ) {
            end = a+1;
        } else if( a >= end ) {
            write_eol = TRUE;
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
            write_eol = TRUE;
        }
    }
    startc = start;
    cnt1 = end-start;
    cnt2 = spend-end;

    c_line_no--;
    w = AccessWindow( wn );

    /*
     * initialize
     */
    addr = startc + spl * w->width;
    txt = (char_info *) &(w->text[ sizeof(char_info)*addr ]);
    scr = (char_info _FAR *) &Scrn[ (w->x1+startc +
                (spl+w->y1)*WindMaxWidth) *sizeof(char_info) ];
#ifdef __VIO__
    oscr = (unsigned) ((char *) scr - Scrn);
    tbytes = cnt1+cnt2;
#endif

    ss_i = 0;
    what.attr = MAKE_ATTR( w, SEType[ ss->type ].foreground,
                              SEType[ ss->type ].background );
    blank.ch = ' ';

    has_mouse = DisplayMouse( FALSE );

    /*
     * display line
     */
    if( w->overcnt[ spl ] ) {
        over = &(w->overlap[ addr ]);
        while( cnt1-- != 0 ) {
            what.ch = (*tmp);
            WRITE_SCREEN_DATA( *txt, what );
            if( *over++ == NO_CHAR ) {
                WRITE_SCREEN( *scr, what );
            }
            tmp++;
            scr++;
            txt++;
            if( ++ss_i > ss->end ) {
                ss++;
                what.attr = MAKE_ATTR( w, SEType[ ss->type ].foreground,
                                          SEType[ ss->type ].background );
            }
        }
        if( write_eol && *(over-1) == NO_CHAR ) {
            WriteLongLineMarker( wn, &( SEType[ SE_EOFTEXT ] ),
                                 txt-1, scr-1, *(tmp-1) );
        } else {
            blank.attr = MAKE_ATTR( w, SEType[ ss->type ].foreground,
                                       SEType[ ss->type ].background );
            while( cnt2-- != 0 ) {
                WRITE_SCREEN_DATA( *txt, blank );
                if( *over++ == NO_CHAR ) {
                    WRITE_SCREEN( *scr, blank );
                }
                scr++;
                txt++;
            }
        }
    } else {
        while( cnt1-- != 0 ) {
            what.ch = (*tmp);
            WRITE_SCREEN_DATA( *txt, what );
            WRITE_SCREEN( *scr, what );
            scr++;
            txt++;
            tmp++;
            if( ++ss_i > ss->end ) {
                ss++;
                what.attr = MAKE_ATTR( w, SEType[ ss->type ].foreground,
                                          SEType[ ss->type ].background );
            }
        }
        if( write_eol ) {
            WriteLongLineMarker( wn, &( SEType[ SE_EOFTEXT ] ),
                                 txt-1, scr-1, *(tmp-1) );
        } else {
            blank.attr = MAKE_ATTR( w, SEType[ ss->type ].foreground,
                                       SEType[ ss->type ].background );
            while( cnt2-- != 0 ) {
                WRITE_SCREEN_DATA( *txt, blank );
                WRITE_SCREEN( *scr, blank );
                txt++;
                scr++;
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
int DisplayLineInWindowWithColor( window_id wn, int c_line_no,
        char *text, type_style *ts, int start_col )
{
    ss_block    ss;

    SEType[ SE_UNUSED ].foreground = ts->foreground;
    SEType[ SE_UNUSED ].background = ts->background;
    SEType[ SE_UNUSED ].font = 0;
    ss.type = SE_UNUSED;
    ss.end = BEYOND_TEXT;

    return( displayLineInWindowGeneric( wn, c_line_no, text, start_col, &ss ) );
}

/*
 * DisplayLineInWindowWithSyntaxStyle - display wrt syntax lang. settings
 */
int DisplayLineInWindowWithSyntaxStyle( window_id wn, int c_line_no,
   line *line, linenum line_no, char *text, int start_col,
   unsigned int junk )
{
    static ss_block     ss[ MAX_SS_BLOCKS ];
    int         dummy;
    bool        saveRealTabs;
    int         a, rc;
    char        *tmp;
    // dc               c_line;

    junk = junk;
    if( EditFlags.RealTabs ) {
        a = strlen( text );
        tmp = StaticAlloc();
        ExpandTabsInABuffer( text, a, tmp, MaxLinem1 + 2 );
    } else {
        tmp = text;
    }

    // this code commented out cause it doesn't quite work.
    // it should be close considering it mirrors the WINDOWS version.

    // get the laguage flags state previous to this line
    // c_line = DCFindLine( c_line_no - 1, wn );
    // SSGetLanguageFlags( &( c_line->flags ) );

    // parse the line (generate new flags as well)
    ss[ 0 ].end = BEYOND_TEXT;
    SSDifBlock( ss, tmp, start_col, line, line_no, &dummy );

    // prevent displayLineInWindowGeneric from expanding tabs - blech
    saveRealTabs = EditFlags.RealTabs;
    EditFlags.RealTabs = FALSE;

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
    char                *over;
    char_info           *txt;
    char_info           _FAR *scr;
#ifdef __VIO__
    unsigned            oscr;
#endif
    int                 addr,i;
    char_info           what;

    if( EditFlags.Quiet ) {
        return;
    }
    w = Windows[ wn ];

    /*
     * find dimensions of line
     */
    if( !w->has_border ) {
        return;
    }
    if( line < 1 || line > w->height-2 ) {
        return;
    }
    line--;

    /*
     * initialize
     */
    w = AccessWindow( wn );
    addr = 1 + (1+line) * w->width;
    txt = (char_info *) &(w->text[ sizeof(char_info)*addr ]);
    scr = (char_info _FAR *) &Scrn[ (w->x1 + (1+line+w->y1)*WindMaxWidth) *
                                sizeof(char_info) ];
#ifdef __VIO__
    oscr = (unsigned) ((unsigned char _FAR *) scr-Scrn);
#endif
    what.attr = MAKE_ATTR( w, w->border_color1, w->border_color2 );
    what.ch = WindowBordersNG[ WB_LEFTT ];

    over = &(w->overlap[ addr ]);
    if( *over++ == NO_CHAR ) {
        WRITE_SCREEN( *scr, what );
    }
    WRITE_SCREEN_DATA( *txt, what );
    txt++;
    scr++;

    what.ch = WindowBordersNG[ WB_TOPBOTTOM ];
    for( i=w->x1+1;i<w->x2;i++ ) {
        if( *over++ == NO_CHAR ) {
            WRITE_SCREEN( *scr, what );
        }
        WRITE_SCREEN_DATA( *txt, what );
        txt++;
        scr++;
    }

    if( line != w->height-3 && line != 0 ) {
        what.ch = WindowBordersNG[ WB_RIGHTT ];
        WRITE_SCREEN_DATA( *txt, what );
        if( *over == NO_CHAR )  {
            WRITE_SCREEN( *scr, what );
        }
        scr++;
        txt++;
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
    char                *over;
    char_info           _FAR *scr;
    char_info           what;
#ifdef __VIO__
    unsigned            oscr;
    unsigned            onscr;
#endif
    int                 attr,t,end,spend,cnt1,cnt2,sscol,spl;

    if( EditFlags.Quiet ) {
        return;
    }
    w = AccessWindow( CurrentWindow );

    /*
     * find dimensions of line
     */
    if( w->has_border ) {
        if( line < 1 || line > w->height-2 ) {
            ReleaseWindow( w );
            return;
        }
        spl = line;
        sscol = 1+scol;
        spend = end = ecol+2;
        if( spend > w->width-1 ) {
            spend = end = w->width-1;
        }
        if( sscol < 1 ) {
            sscol = 1;
        }
    } else {
        if( line < 1 || line > w->height ) {
            ReleaseWindow( w );
            return;
        }
        spl = line-1;
        sscol = scol;
        spend = end = ecol+1;
        if( spend > w->width ) {
            spend = end = w->width;
        }
        if( sscol < 0 ) {
            sscol = 0;
        }
    }
    cnt1 = end-sscol;
    cnt2 = spend-end;

    line--;

    /*
     * initialize
     */
    t = sscol + spl * w->width;
    scr = (char_info _FAR *) &Scrn[ (w->x1+sscol + (spl+w->y1)*WindMaxWidth)
                                *sizeof(char_info) ];
#ifdef __VIO__
    oscr = (unsigned) ((char _FAR *) scr - Scrn);
    onscr = 0;
#endif
    attr = MAKE_ATTR( w, s->foreground, s->background );
    what.attr = attr;

    /*
     * display line
     */
    if( w->overcnt[ spl ] ) {
        over = w->overlap + t;
        while( cnt1-- != 0 ) {
            if( *over++ == NO_CHAR ) {
                what.ch = (*scr).ch;
                WRITE_SCREEN( *scr, what );
#ifdef __VIO__
                onscr++;
#endif
            }
            scr++;
        }
        while( cnt2-- != 0 ) {
            if( *over++ == NO_CHAR ) {
                what.ch = (*scr).ch;
                WRITE_SCREEN( *scr, what );
#ifdef __VIO__
                onscr++;
#endif
            }
            scr++;
        }
    } else {
        while( cnt1-- != 0 ) {
            what.ch = (*scr).ch;
            WRITE_SCREEN( *scr, what );
#ifdef __VIO__
            onscr++;
#endif
            scr++;
        }
        while( cnt2-- != 0 ) {
            what.ch = (*scr).ch;
            WRITE_SCREEN( *scr, what );
#ifdef __VIO__
            onscr++;
#endif
            scr++;
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
    int s,e;

    s = VirtualCursorPosition2( scol );
    e = VirtualCursorPosition2( ecol + 1 ) - 1;
    if( scol == 0 ) {
        s = 0;
    }
    changeColorOfDisplayLine( (int) (line-TopOfPage+1),
            s - LeftColumn, e - LeftColumn, &editw_info.hilight );

} /* HiliteAColumnRange */

/*
 * ColorAColumnRange - color a specified word on a line
 */
void ColorAColumnRange( int row, int scol, int ecol, type_style *style )
{
    int s,e,t;

    s = VirtualCursorPosition2( scol );
    e = VirtualCursorPosition2( ecol );
    if( s > e ) {
        t = s;
        s = e;
        e = t;
    }

    s--;
    e--;
    changeColorOfDisplayLine( row, s - LeftColumn, e - LeftColumn, style );

} /* ColorAColumnRange */

/*
 * SetCharInWindowWithColor - do just that, using given colors
 */
int SetCharInWindowWithColor( window_id wn, int line, int col, char text, type_style *style )
{
    wind                *w;
    char                *over;
    int                 attr,addr,start,spl;
    char_info           tmp;
    char_info           *txt;
    char_info           _FAR *scr;
    bool                has_mouse;

    if( EditFlags.Quiet ) {
        return( ERR_NO_ERR );
    }
    w = Windows[ wn ];

    /*
     * find dimensions of line
     */
    if( w->has_border ) {
        if( line < 1 || line > w->height-2 ) {
            return( ERR_WIND_NO_SUCH_LINE );
        }
        start = 1;
        spl = line;
    } else {
        if( line < 1 || line > w->height ) {
            return( ERR_WIND_NO_SUCH_LINE );
        }
        start = 0;
        spl = line-1;
    }
    if( col < 1 || col > w->width ) {
        return( ERR_WIND_NO_SUCH_COLUMN );
    }
    line--;
    col--;

    /*
     * initialize
     */
    w = AccessWindow( wn );
    addr = col + start + spl * w->width;
    txt = (char_info *) &(w->text[ sizeof(char_info)*addr ]);
    scr = (char_info _FAR *) &Scrn[ (w->x1+start + col + (spl+w->y1)*
                        WindMaxWidth) *sizeof(char_info) ];
    attr = MAKE_ATTR( w, style->foreground, style->background );

    /*
     * display char
     */
    has_mouse = DisplayMouse( FALSE );
    over = &(w->overlap[ addr ]);
    tmp.attr = attr;
    tmp.ch = text;
    WRITE_SCREEN_DATA( *txt, tmp );
    if( *over == NO_CHAR ) {
        WRITE_SCREEN( *scr, tmp );
    }
#ifdef __VIO__
    MyVioShowBuf( (unsigned)((char *) scr-Scrn), 1 );
#endif

    ReleaseWindow( w );
    DisplayMouse( has_mouse );
    return( ERR_NO_ERR );

} /* SetCharInWindowWithColor */

/*
 * DisplayLineInWindow - do as it sounds, use default colors
 */
int DisplayLineInWindow( window_id wn, int c_line_no, char *text )
{
    ss_block    ss;
    SEType[ SE_UNUSED ].foreground = Windows[ wn ]->text_color;
    SEType[ SE_UNUSED ].background = Windows[ wn ]->background_color;
    SEType[ SE_UNUSED ].font = 0;
    ss.type = SE_UNUSED;
    ss.end = BEYOND_TEXT;
    return( displayLineInWindowGeneric( wn, c_line_no, text, 0, &ss ) );
} /* DisplayLineInWindow */
