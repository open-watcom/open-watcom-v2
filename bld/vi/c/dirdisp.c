/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2019 The Open Watcom Contributors. All Rights Reserved.
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
#include "posix.h"
#include "win.h"
#ifdef __WIN__
    #include "utils.h"
    #include "color.h"
    #include "vifont.h"
#endif
#include <assert.h>


#define ROUND_TOPAGE_BEGIN(x)   (((x) / perPage) * perPage)
#define ROUND_TOLINE_BEGIN(x)   (((x) / maxJ) * maxJ)
#define ROUND_TOLINE_END(x)     (((x) / maxJ) * maxJ + maxJ - 1 )

static window_id    dir_wid = NO_WINDOW;
static list_linenum oldFilec = 0;
static list_linenum lastFilec = 0;
static list_linenum mouseFilec = -1;
static list_linenum maxJ;
static list_linenum perPage;
static list_linenum oldPage = -1;
static list_linenum cPage = 0;
static bool         hasWrapped = false;
static bool         isDone = false;
static bool         hasMouseHandler = false;

/*
 * FileCompleteMouseHandler - handle mouse events for file completion
 */
static bool FileCompleteMouseHandler( window_id wid, int win_x, int win_y )
{
    if( wid != dir_wid ) {
        return( false );
    }
    if( LastMouseEvent != VI_MOUSE_PRESS && LastMouseEvent != VI_MOUSE_DCLICK ) {
        return( false );
    }
    if( LastMouseEvent == VI_MOUSE_DCLICK ) {
        isDone = true;
    }
    if( !InsideWindow( wid, win_x, win_y ) ) {
        return( false );
    }

    mouseFilec = cPage + ( win_x - 1 ) / NAMEWIDTH + ( win_y - 1 ) * maxJ;

    return( true );

} /* FileCompleteMouseHandler */


/*
 * appendExtra - add extra to end
 */
static vi_rc appendExtra( char *data, size_t start, size_t max, direct_ent *fi, size_t len )
{
    size_t  i;
    vi_rc   rc;

    if( max > start + len )
        max = start + len;
    for( i = start; i < max; i++ ) {
        data[i] = fi->name[i - start];
    }
    if( IS_SUBDIR( fi ) ) {
        rc = ERR_NO_ERR;
    } else {
        rc = FILE_COMPLETE;
    }
    data[i] = '\0';
    return( rc );

} /* appendExtra */

/*
 * doFileComplete - complete file name
 */
static vi_rc doFileComplete( char *data, size_t start, size_t max, bool getnew, vi_key key )
{
    size_t          i;
    size_t          j;
    size_t          k;
    list_linenum    m;
    list_linenum    n;
    size_t          newstart;
    bool            newstartdef;
    char            fullmask[MAX_STR * 2];
    vi_rc           rc;
    int             c;

    newstartdef = false;
    newstart = 0;
    for( i = start; i > 0; --i ) {
        c = (unsigned char)data[i - 1];
        if( isspace( c ) )
            break;
        if( newstartdef )
            continue;
#ifdef __UNIX__
        if( c == FILE_SEP ) {
#else
        if( c = DRV_SEP || c == ALT_FILE_SEP || c == FILE_SEP ) {
#endif
            newstartdef = true;
            newstart = i;
        }
    }
    if( !newstartdef ) {
        newstart = i;
    }
    if( getnew ) {
        k = 0;
        for( j = i; j < start; j++ ) {
            fullmask[k++] = data[j];
        }
        lastFilec = -1;
        fullmask[k++] = '*';
        fullmask[k] = '\0';
        rc = GetSortDir( fullmask, false );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
        /*
         * remove any crap from the list
         */
        for( m = 0; m < DirFileCount; m++ ) {
            if( !IsTextFile( DirFiles[m]->name ) || (DirFiles[m]->name[0] == '.') ) {
                MemFree( DirFiles[m] );
                for( n = m + 1; n < DirFileCount; n++ ) {
                    DirFiles[n - 1] = DirFiles[n];
                }
                m--;
                DirFileCount--;
            }
        }
    }
    if( DirFileCount == 0 ) {
        return( ERR_FILE_NOT_FOUND );
    }
    if( DirFileCount == 1 ) {
        if( !BAD_ID( dir_wid ) ) {
            ClearWindow( dir_wid );
        }
        return( appendExtra( data, newstart, max, DirFiles[0], strlen( DirFiles[0]->name ) ) );
    }

    /*
     * okay, so we have multiple matches; add in the next match
     */
    oldFilec = lastFilec;
    switch( key ) {
    case VI_KEY( TAB ):
    case VI_KEY( RIGHT ):
        lastFilec++;
        break;
    case VI_KEY( SHIFT_TAB ):
    case VI_KEY( LEFT ):
        lastFilec--;
        break;
    case VI_KEY( DOWN ):
        lastFilec += maxJ;
        break;
    case VI_KEY( UP ):
        lastFilec -= maxJ;
        break;
    case VI_KEY( FAKEMOUSE ):
    case VI_KEY( MOUSEEVENT ):
        lastFilec = mouseFilec;
        break;
    }
    for( ; lastFilec >= DirFileCount; lastFilec -= DirFileCount ) {
        hasWrapped = true;
    }
    for( ; lastFilec < 0; lastFilec += DirFileCount ) {
        hasWrapped = true;
    }

    appendExtra( data, newstart, max, DirFiles[lastFilec], strlen( DirFiles[lastFilec]->name ) );

    return( ERR_NO_ERR );

} /* doFileComplete */

static void parseFileName( list_linenum i, char *buffer )
{
    MySprintf( buffer, "  %S", ( i < DirFileCount ) ? DirFiles[i]->name : SingleBlank );
    if( i < DirFileCount ) {
        if( IS_SUBDIR( DirFiles[i] ) ) {
            buffer[1] = FILE_SEP;
        }
    }
}

#ifdef __WIN__
static int calcColumns( window_id wid )
{
    RECT        rect;
    int         columns;
    window      *w;

    if( BAD_ID( wid ) )
        return( 0 );
    w = WINDOW_FROM_ID( wid );
    GetClientRect( wid, &rect );
    columns = rect.right - rect.left;
    columns = columns / ( NAMEWIDTH * FontAverageWidth( WIN_TEXT_FONT( w ) ) );
    return( columns );
}

void FileCompleteMouseClick( window_id wid, int x, int y, bool dclick )
{
    list_linenum    file;
    int             column_width;
    int             column_height;
    int             c;
    int             left_margin;
    int             columns;
    RECT            rect;
    window          *w;

    if( BAD_ID( wid ) )
        return;
    w = WINDOW_FROM_ID( wid );
    /* figure out which file_name the user clicked on */
    columns = calcColumns( wid );
    GetClientRect( wid, &rect );
    column_width = NAMEWIDTH * FontAverageWidth( WIN_TEXT_FONT( w ) );
    column_height = FontHeight( WIN_TEXT_FONT( w ) );
    left_margin = ( rect.right - rect.left - column_width * columns ) >> 1;
    if( x < left_margin || x + left_margin > rect.right ) {
        return;
    }
    c = ( x - left_margin ) / column_width;
    file = ( y / column_height ) * columns + c;
    file += cPage;
    if( dclick ) {
        isDone = true;
    }
    mouseFilec = file;
    KeyAdd( VI_KEY( FAKEMOUSE ) );
}

static void getBounds( list_linenum *start, list_linenum *end )
{
    list_linenum    first;
    list_linenum    last;

    if( oldFilec < lastFilec ) {
        first = oldFilec;
        last = lastFilec;
    } else {
        first = lastFilec;
        last = oldFilec;
    }
    *start = ROUND_TOLINE_BEGIN( first );
    *end = ROUND_TOLINE_END( last );
}

static void displayFiles( void )
{
    list_linenum    i;
    list_linenum    start;
    list_linenum    end;
    int             column;
    int             right_edge;
    int             left_edge;
    int             outer_bound;
    int             font_height;
    int             column_width;
    window          *w;
    RECT            rect;
    type_style      *style;
    char            buffer[FILENAME_MAX];

    if( BAD_ID( dir_wid ) )
        return;
    w = WINDOW_FROM_ID( dir_wid );

    if( hasWrapped ) {
        ClearWindow( dir_wid );
        hasWrapped = false;
    }

    font_height = FontHeight( WIN_TEXT_FONT( w ) );
    GetClientRect( dir_wid, &rect );
    column_width = NAMEWIDTH * FontAverageWidth( WIN_TEXT_FONT( w ) );
    outer_bound = rect.right;
    left_edge = rect.right - rect.left;
    left_edge -= maxJ * column_width;
    left_edge >>= 1;
    right_edge = rect.right - left_edge - 1;

    cPage = ROUND_TOPAGE_BEGIN( lastFilec );
    if( cPage != oldPage ) {
        start = cPage;
        end = cPage + perPage + maxJ;
    } else {
        getBounds( &start, &end );
    }

    // assert( start <= lastFilec && lastFilec <= end );
    rect.top = ( start / maxJ - cPage / maxJ ) * font_height;
    rect.bottom = rect.top + font_height;
    rect.left = 0;
    rect.right = left_edge;
    BlankRectIndirect( dir_wid, WIN_TEXT_BACKCOLOR( w ), &rect );
    column = 0;
    for( i = start; i <= end; i++ ) {
        parseFileName( i, buffer );
        buffer[NAMEWIDTH] = '\0';
        style = ( i == lastFilec ) ? WIN_HILIGHT_STYLE( w ) : WIN_TEXT_STYLE( w );
        rect.left = column * column_width + left_edge;
        rect.right = rect.left + column_width;
        BlankRectIndirect( dir_wid, style->background, &rect );
        WriteString( dir_wid, rect.left, rect.top, style, buffer );
        column = ( column + 1 ) % maxJ;
        if( column == 0 ) {
            /* blat out the rest of the row and continue on */
            rect.left = right_edge;
            rect.right = outer_bound;
            BlankRectIndirect( dir_wid, WIN_TEXT_BACKCOLOR( w ), &rect );
            rect.top = rect.bottom;
            rect.bottom = rect.top + font_height;
            rect.left = 0;
            rect.right = left_edge;
            BlankRectIndirect( dir_wid, WIN_TEXT_BACKCOLOR( w ), &rect );
        }
    }
    oldPage = cPage;
}

#else

/*
 * displayFiles - display files according to specified type
 *                (comment free code - my favourite kind!)
 */
static void displayFiles( void )
{
    char            tmp[FILENAME_MAX];
    char            tmp2[FILENAME_MAX];
    size_t          j;
    size_t          z;
    size_t          k;
    list_linenum    i;
    list_linenum    st;
    list_linenum    end;
    int             l;
    size_t          hilite;
    bool            hiliteon;

    st = 0;
    end = perPage;
    cPage = ROUND_TOPAGE_BEGIN( lastFilec );
    if( cPage > 0 ) {
        st += cPage;
        end += cPage;
    }
    if( hasWrapped ) {
        ClearWindow( dir_wid );
        hasWrapped = false;
    }

    l = 1;
    j = 0;
    hilite = 0;
    tmp[0] = '\0';
    hiliteon = false;
    for( i = st; i < end; i++ ) {
        if( i == lastFilec ) {
            hiliteon = true;
            hilite = j;
        }
        parseFileName( i, tmp2 );
        tmp2[NAMEWIDTH] = '\0';
        strcat( tmp, tmp2 );
        j++;
        if( j == maxJ || i == ( end - 1 ) ) {
            DisplayLineInWindow( dir_wid, l++, tmp );
            if( hiliteon ) {
                j = hilite * NAMEWIDTH;
                parseFileName( lastFilec, tmp2 );
                z = j + strlen( tmp2 );
                for( k = j; k < z; k++ ) {
                    SetCharInWindowWithColor( dir_wid, l - 1, k + 1, tmp2[k - j], &filecw_info.hilight_style );
                }
                hiliteon = false;
            }
            j = 0;
            tmp[0] = '\0';
        }
    }
    if( mouseFilec >= 0 ) {
        mouseFilec = -1;
    }

} /* displayFiles */
#endif

/*
 * StartFileComplete - handle file completion
 */
vi_rc StartFileComplete( char *data, size_t start, size_t max, vi_key what )
{
    vi_rc   rc;
    int     maxl;

    isDone = false;
    rc = doFileComplete( data, start, max, true, what );
    if( rc > ERR_NO_ERR || rc == FILE_COMPLETE ) {
        return( rc );
    }

    if( BAD_ID( dir_wid ) ) {
        // ensure uniform font before opening window
        if( filecw_info.text_style.font != filecw_info.hilight_style.font )
            filecw_info.hilight_style.font = filecw_info.text_style.font;

        rc = NewWindow2( &dir_wid, &filecw_info );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }
    WindowTitle( dir_wid, "File Completion List" );

#ifdef __WIN__
    maxJ = calcColumns( dir_wid );
#else
    maxJ = WindowAuxInfo( dir_wid, WIND_INFO_TEXT_COLS ) / NAMEWIDTH;
#endif
    maxl = WindowAuxInfo( dir_wid, WIND_INFO_TEXT_LINES );
    perPage = maxl * maxJ;
    oldPage = -1;
    displayFiles();
    PushMouseEventHandler( FileCompleteMouseHandler );
    hasMouseHandler = true;
    return( ERR_NO_ERR );

} /* StartFileComplete */

/*
 * ContinueFileComplete
 */
vi_rc ContinueFileComplete( char *data, size_t start, size_t max, vi_key what )
{
    vi_rc   rc;

    rc = doFileComplete( data, start, max, false, what );
    if( rc > ERR_NO_ERR || rc == FILE_COMPLETE ) {
        return( rc );
    }
    displayFiles();
    if( isDone ) {
        return( FILE_COMPLETE_ENTER );
    }
    return( ERR_NO_ERR );

} /* ContinueFileComplete */

/*
 * FinishFileComplete
 */
void FinishFileComplete( void )
{
    if( BAD_ID( dir_wid ) ) {
        return;
    }
    CloseAWindow( dir_wid );
    dir_wid = NO_WINDOW;
    if( hasMouseHandler ) {
        PopMouseEventHandler();
        hasMouseHandler = false;
    }

} /* FinishFileComplete */

/*
 * PauseFileComplete
 */
void PauseFileComplete( void )
{
    WindowTitle( dir_wid, NULL );
    if( hasMouseHandler ) {
        PopMouseEventHandler();
        hasMouseHandler = false;
    }

} /* PauseFileComplete */

