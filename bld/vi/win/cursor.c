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
* Description:  Cursor (caret) support.
*
****************************************************************************/


#include "vi.h"
#include "color.h"
#include "font.h"
#include "utils.h"
#include "win.h"
#include <assert.h>

static int              cursorHeight;
static int              cursorWidth;
static cursor_type      cursorType;
static UINT             oldBlinkTime;
static bool             haveOldBlinkTime;
static bool             caretDisplayed = FALSE;
static bool             caretKilled = FALSE;

extern int WinVirtualCursorPosition( char *, int );

/*
 * SetCursorBlinkRate - set the rate of blinking for the caret
 */
void SetCursorBlinkRate( int cbr )
{
    SetCaretBlinkTime( cbr );
    CursorBlinkRate = cbr;

} /* SetCursorBlinkRate */

/*
 * GoodbyeCursor - we are losing focus, so get rid of our cursor
 */
void GoodbyeCursor( HWND hwnd )
{
    if( haveOldBlinkTime ) {
        SetCaretBlinkTime( oldBlinkTime );
    }
    MyHideCaret( hwnd );
    DestroyCaret();

} /* GoodbyeCursor */

/*
 * NewCursor - create a new cursor for a window
 */
void NewCursor( window_id id, cursor_type ct )
{
    window      *w;
    int         height;
    int         width;

    if( BAD_ID( id ) ) {
        return;
    }
    w = WINDOW_FROM_ID( id );
    height = FontHeight( WIN_FONT( w ) );
    width = FontAverageWidth( WIN_FONT( w ) );
    height = (long) height * ct.height / 100L;
    width = (long) width * ct.width / 100L;
    MyHideCaret( id );
    DestroyCaret();
    cursorHeight = height;
    cursorWidth = width;
    if( !haveOldBlinkTime ) {
        oldBlinkTime = GetCaretBlinkTime();
        haveOldBlinkTime = TRUE;
    }
    CreateCaret( id, (HBITMAP)NULLHANDLE, cursorWidth, cursorHeight );
    SetCursorBlinkRate( CursorBlinkRate );
    MyShowCaret( id );
    cursorType = ct;

} /* NewCursor */

static int getCursorInfo( HWND hwnd, int row, int col, int *x, int *width )
{
    ss_block    *ss, *ss_start, *ss_prev;
    dc          dc_line;
    int         len;
    int         old_col;
    char        *str;
    int         funny = 0;

    col--;      // we like base 0
    row--;

    // this section checks if current line is valid.

    assert( hwnd == CurrentInfo->CurrentWindow );
    if( row < 0 || row >= CurrentInfo->dc_size ) {
        // not on screen -> not displayed
        *x = -10;
        *width = 0;
        return( 0 );
    }
    dc_line = DCFindLine( row, hwnd );

    if( dc_line->display != 0 ){
        // line has not been drawn yet. Can't set cursor.
        *x = -10;
        *width = 0;
        return( 0 );
    }

    assert( dc_line->valid );

    if( dc_line->start_col != LeftTopPos.column ) {
        // not in cache -> not on screen -> not displayed
        *x = -10;
        *width = 0;
        return( 0 );
    }
    ss_start = ss = dc_line->ss;


    // this bit adjusts col for real tabs

    if( EditFlags.RealTabs ){
        // this takes it from the current line!!!! %$#@!
        // what if row isn't the current line! oops!!!
        // luckily this works because who wants to know cursor info
        // for any line but the current one!

        // if( thisLine == CurrentLine ){
            int real_left = RealColumnOnCurrentLine( LeftTopPos.column + 1 );
            old_col = col;
            col = RealColumnOnCurrentLine( col + 1 ) - real_left;

            // kludge! - Real Cursor position refuses to say cursor is to right
            //           of last character like it ought to when Modeless
            if( CursorPositionOffRight( old_col + 1 ) &&
                (EditFlags.Modeless == FALSE) ) {
                 col++;
            }

        // } else {
            // int real_left = RealCursorPositionOnLine( thisLine, LeftColumn + 1 );
            // old_col = col;
            // col = RealCursorPositionOnLine( thisLine, col + 1 ) - real_left;
        // }
    }


    // this bit finds the block we want
    while( ss->end < col ) {
        ss++;
    }

    // handle cursor being off the right of text
    if( ss->end == BEYOND_TEXT ) {
        *width = FontAverageWidth( SEType[ss->type].font );
        if( ss == ss_start ) {
            *x = (*width) * col;
        } else {
            ss_prev = ss - 1;
            *x = ((*width) * (col - ss_prev->end - 1)) + ss_prev->offset;
        }
        return( 0 );
    }

    // setup to figure out where cursor is within text.
    if( ss != ss_start ) {
        ss_prev = ss - 1;
        str = dc_line->text + ss_prev->end + 1;
        len = col - ss_prev->end - 1;
    } else {
        str = dc_line->text;
        len = col;
    }

    // Magic Tabs positioning
    if( EditFlags.RealTabs ) {
        type_style      *this_style = &SEType[ss->type];
        int             no_tab = FALSE;
        int             avg_width = FontAverageWidth( SEType[ss->type].font );
        int             left, extent, end_tab;
        char            *cur_pos;
        char            *end_str;

        if( ss != ss_start ) {
            left = ss_prev->offset;
        } else {
            left = 0;
        }

        if( len > 0 ) {
            // Note: this will not work with 16bit chars or embedded chars
            // but niether will the drawing routines, so not to worry (yet)

            // Is there a tab in the current block ?
            end_str = cur_pos = str + len;
            while( *(--cur_pos) != '\t' ){
                if(cur_pos == str){
                    no_tab = TRUE;
                    break;
                }
            }

            // if so, figure out where the last tab stop is.
            if( no_tab == FALSE ) {

                // dist is the virtual curpos - the number of chars before
                // the first tab. this should be the tab boundry.
                int dist = (old_col + 1) - (end_str - cur_pos);
                // unless the end_str was also a tab, So we round down.
                left = (dist - (dist % HardTab) - LeftTopPos.column) * avg_width;

                cur_pos++;
            }

            // now get the extent of the leading chars ...
            extent = MyTextExtent( hwnd, this_style, cur_pos, end_str - cur_pos );
        } else {
            extent = 0;
            cur_pos = str;
            end_str = str;
        }


        // ... and find the position and width of the cursor.
        if( *end_str == '\t' ){
            // in strange case, tab may start before end of prev string
            end_tab = (old_col - LeftTopPos.column + 1) * avg_width;
            *x = min( left + extent, end_tab );
            *width = max( end_tab - *x, 1 );
            funny = 0;
        } else {
            *x = left + extent;
            *width = MyTextExtent( hwnd, this_style, cur_pos,
                                   end_str - cur_pos + 1 ) - extent;
            funny = (*width) / 2;
        }
    } else {
        type_style *this_style = &SEType[ss->type];

        *x = MyTextExtent( hwnd, this_style, str, len );
        *width = MyTextExtent( hwnd, this_style,  str, len + 1 ) - *x;
        if( ss != ss_start ) {
            *x += ss_prev->offset;
        }
        funny = (*width) / 2;
    }
    if( FontIsFunnyItalic( SEType[ss->type].font ) ) {
        return( funny );
    } else {
        return( 0 );
    }
}

int PixelFromColumnOnCurrentLine( int vcol )
{
    int         x, w;
    getCursorInfo( CurrentWindow, CurrentPos.line - LeftTopPos.line + 1,
                   vcol - LeftTopPos.column, &x, &w );

    return( x );
}

/*
 * setCursorOnScreen - set cursor at specified row and column in edit window
 */
static void setCursorOnScreen( int row, int col )
{
    window      *w;
    int         x, y;
    int         width;
    int         funny;

    if( BAD_ID( CurrentWindow ) ) {
        return;
    }

    if( EditFlags.Quiet || EditFlags.NoSetCursor ) {
        return;
    }

    funny = getCursorInfo( CurrentWindow, row, col, &x, &width );
    w = WINDOW_FROM_ID( CurrentWindow );
    y = row * FontHeight( WIN_FONT( w ) ) - cursorHeight;
    width = (long) width * cursorType.width / 100L;
    if( cursorWidth != width ) {
        MyHideCaret( CurrentWindow );
        DestroyCaret();
        CreateCaret( CurrentWindow, (HBITMAP)NULLHANDLE, width, cursorHeight );
        cursorWidth = width;
    }
    // adjust position for italic sillyness
    SetCaretPos( x - funny, y );
    MyShowCaret( CurrentWindow );

} /* setCursorOnScreen */

/*
 * SetCursorOnLine - set cursor at specified column in single line text string
 */
void SetCursorOnLine( window_id id, int col, char *str, type_style *style )
{
    window      *w;
    int         x, y;
    int         width, height;

    if( BAD_ID( id ) ) {
        return;
    }
    w = WINDOW_FROM_ID( id );
    // y = FontHeight( WIN_FONT( w ) ) - cursorHeight;

    x = MyTextExtent( id, style, str, col - 1 );
    width = MyTextExtent( id, style, str, col ) - x;

    /* adjust so that Insert cursor is 0 width
     * Also make the overstrike cursor the height of the insert cursor.
     */
    width = (long) width * cursorType.width / 100L;
    height = InsertCursorType.height;
    y = FontHeight( WIN_FONT( w ) ) - height;

    MyHideCaret( id );
    DestroyCaret();
    // CreateCaret( id, (HBITMAP)NULLHANDLE, width, cursorHeight );
    CreateCaret( id, (HBITMAP)NULLHANDLE, width, height );
    SetCaretPos( x, y );
    MyShowCaret( id );

} /* SetCursorOnLine */

void SetGenericWindowCursor( window_id id, int row, int col )
{
    // setCursorOnScreen calls functions which are not generic
    // ie) they only work on the current window! Therefore
    // this routine does not do what the name implies!!!
    id = id;

    setCursorOnScreen( row, col );

} /* SetGenericWindowCursor */

/*
 * ResetEditWindowCursor - display cursor on setfocus to a buffer
 */
void ResetEditWindowCursor( window_id id )
{
    if( !EditFlags.Modeless && !EditFlags.InsertModeActive ) {
        NewCursor( id, NormalCursorType );
    } else {
        if( EditFlags.WasOverstrike ) {
            NewCursor( id, OverstrikeCursorType );
        } else {
            NewCursor( id, InsertCursorType );
        }
    }

    // position cursor in edit window
    SetWindowCursor();

} /* ResetEditWindowCursor */

/*
 * MyShowCaret - ShowCaret w/o additive effects
 */
void MyShowCaret( window_id id )
{
    if( !caretDisplayed && !caretKilled ) {
        ShowCaret( id );
        caretDisplayed = TRUE;
    }

} /* MyShowCaret */

/*
 * MyHideCaret - HideCaret w/o additive effects
 */
void MyHideCaret( window_id id )
{
    if( caretDisplayed ) {
        HideCaret( id );
        caretDisplayed = FALSE;
    }

} /* MyHideCaret */

void MyKillCaret( window_id id )
{
    if( !caretKilled ) {
        MyHideCaret( id );
        HideCaret( id );
        caretKilled = TRUE;
    }
}

void MyRaiseCaret( window_id id )
{
    if( caretKilled ) {
        ShowCaret( id );
        caretKilled = FALSE;
        MyShowCaret( id );
    }
}

