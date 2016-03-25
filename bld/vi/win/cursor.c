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
static bool             caretDisplayed = false;
static bool             caretKilled = false;

extern int WinVirtualCursorPosition( char *, int );

/*
 * SetCursorBlinkRate - set the rate of blinking for the caret
 */
void SetCursorBlinkRate( int cbr )
{
    SetCaretBlinkTime( cbr );
    EditVars.CursorBlinkRate = cbr;

} /* SetCursorBlinkRate */

/*
 * GoodbyeCursor - we are losing focus, so get rid of our cursor
 */
void GoodbyeCursor( window_id wid )
{
    if( haveOldBlinkTime ) {
        SetCaretBlinkTime( oldBlinkTime );
    }
    MyHideCaret( wid );
    DestroyCaret();

} /* GoodbyeCursor */

/*
 * NewCursor - create a new cursor for a window
 */
void NewCursor( window_id wid, cursor_type ct )
{
    window      *w;
    int         height;
    int         width;

    if( BAD_ID( wid ) ) {
        return;
    }
    w = WINDOW_FROM_ID( wid );
    height = FontHeight( WIN_TEXT_FONT( w ) );
    width = FontAverageWidth( WIN_TEXT_FONT( w ) );
    height = (long) height * ct.height / 100L;
    width = (long) width * ct.width / 100L;
    MyHideCaret( wid );
    DestroyCaret();
    cursorHeight = height;
    cursorWidth = width;
    if( !haveOldBlinkTime ) {
        oldBlinkTime = GetCaretBlinkTime();
        haveOldBlinkTime = true;
    }
    CreateCaret( wid, (HBITMAP)NULLHANDLE, cursorWidth, cursorHeight );
    SetCursorBlinkRate( EditVars.CursorBlinkRate );
    MyShowCaret( wid );
    cursorType = ct;

} /* NewCursor */

static int getCursorInfo( window_id wid, int row, int col, int *x, int *width )
{
    ss_block    *ss, *ss_start, *ss_prev;
    dc_line     *dcline;
    int         len;
    int         old_col = 0;
    char        *str;
    int         funny = 0;

    col--;      // we like base 0
    row--;

    // this section checks if current line is valid.

    assert( wid == CurrentInfo->current_window_id );
    if( row < 0 || row >= CurrentInfo->dc_size ) {
        // not on screen -> not displayed
        *x = -10;
        *width = 0;
        return( 0 );
    }
    dcline = DCFindLine( row, wid );

    if( dcline->display != 0 ){
        // line has not been drawn yet. Can't set cursor.
        *x = -10;
        *width = 0;
        return( 0 );
    }

    assert( dcline->valid );

    if( dcline->start_col != LeftTopPos.column ) {
        // not in cache -> not on screen -> not displayed
        *x = -10;
        *width = 0;
        return( 0 );
    }
    ss_start = ss = dcline->ss;
    ss_prev = NULL;


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
            if( CursorPositionOffRight( old_col + 1 ) && !EditFlags.Modeless ) {
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
        str = dcline->text + ss_prev->end + 1;
        len = col - ss_prev->end - 1;
    } else {
        str = dcline->text;
        len = col;
    }

    // Magic Tabs positioning
    if( EditFlags.RealTabs ) {
        type_style      *this_style = &SEType[ss->type];
        bool            no_tab = false;
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
                    no_tab = true;
                    break;
                }
            }

            // if so, figure out where the last tab stop is.
            if( !no_tab ) {

                // dist is the virtual curpos - the number of chars before
                // the first tab. this should be the tab boundry.
                int dist = (old_col + 1) - (end_str - cur_pos);
                // unless the end_str was also a tab, So we round down.
                left = (dist - (dist % EditVars.HardTab) - LeftTopPos.column) * avg_width;

                cur_pos++;
            }

            // now get the extent of the leading chars ...
            extent = MyTextExtent( wid, this_style, cur_pos, end_str - cur_pos );
        } else {
            extent = 0;
            cur_pos = str;
            end_str = str;
        }


        // ... and find the position and width of the cursor.
        if( *end_str == '\t' ){
            // in strange case, tab may start before end of prev string
            end_tab = (old_col - LeftTopPos.column + 1) * avg_width;
            *x = left + extent;
            if( *x > end_tab )
                *x = end_tab;
            *width = end_tab - *x;
            if( *width < 1 )
                *width = 1;
            funny = 0;
        } else {
            *x = left + extent;
            *width = MyTextExtent( wid, this_style, cur_pos,
                                   end_str - cur_pos + 1 ) - extent;
            funny = (*width) / 2;
        }
    } else {
        type_style *this_style = &SEType[ss->type];

        *x = MyTextExtent( wid, this_style, str, len );
        *width = MyTextExtent( wid, this_style,  str, len + 1 ) - *x;
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
    getCursorInfo( current_window_id, CurrentPos.line - LeftTopPos.line + 1,
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

    if( BAD_ID( current_window_id ) ) {
        return;
    }

    if( EditFlags.Quiet || EditFlags.NoSetCursor ) {
        return;
    }

    funny = getCursorInfo( current_window_id, row, col, &x, &width );
    w = WINDOW_FROM_ID( current_window_id );
    y = row * FontHeight( WIN_TEXT_FONT( w ) ) - cursorHeight;
    width = (long) width * cursorType.width / 100L;
    if( cursorWidth != width ) {
        MyHideCaret( current_window_id );
        DestroyCaret();
        CreateCaret( current_window_id, (HBITMAP)NULLHANDLE, width, cursorHeight );
        cursorWidth = width;
    }
    // adjust position for italic sillyness
    SetCaretPos( x - funny, y );
    MyShowCaret( current_window_id );

} /* setCursorOnScreen */

/*
 * SetCursorOnLine - set cursor at specified column in single line text string
 */
void SetCursorOnLine( window_id wid, int col, char *str, type_style *style )
{
    window      *w;
    int         x, y;
    int         width, height;

    if( BAD_ID( wid ) ) {
        return;
    }
    w = WINDOW_FROM_ID( wid );
    // y = FontHeight( WIN_TEXT_FONT( w ) ) - cursorHeight;

    x = MyTextExtent( wid, style, str, col - 1 );
    width = MyTextExtent( wid, style, str, col ) - x;

    /* adjust so that Insert cursor is 0 width
     * Also make the overstrike cursor the height of the insert cursor.
     */
    width = (long)width * cursorType.width / 100L;
    height = EditVars.InsertCursorType.height;
    y = FontHeight( WIN_TEXT_FONT( w ) ) - height;

    MyHideCaret( wid );
    DestroyCaret();
    // CreateCaret( wid, (HBITMAP)NULLHANDLE, width, cursorHeight );
    CreateCaret( wid, (HBITMAP)NULLHANDLE, width, height );
    SetCaretPos( x, y );
    MyShowCaret( wid );

} /* SetCursorOnLine */

void SetGenericWindowCursor( window_id wid, int row, int col )
{
    // setCursorOnScreen calls functions which are not generic
    // ie) they only work on the current window! Therefore
    // this routine does not do what the name implies!!!
    wid = wid;

    setCursorOnScreen( row, col );

} /* SetGenericWindowCursor */

/*
 * ResetEditWindowCursor - display cursor on setfocus to a buffer
 */
void ResetEditWindowCursor( window_id wid )
{
    if( !EditFlags.Modeless && !EditFlags.InsertModeActive ) {
        NewCursor( wid, EditVars.NormalCursorType );
    } else {
        if( EditFlags.WasOverstrike ) {
            NewCursor( wid, EditVars.OverstrikeCursorType );
        } else {
            NewCursor( wid, EditVars.InsertCursorType );
        }
    }

    // position cursor in edit window
    SetWindowCursor();

} /* ResetEditWindowCursor */

/*
 * MyShowCaret - ShowCaret w/o additive effects
 */
void MyShowCaret( window_id wid )
{
    if( !caretDisplayed && !caretKilled ) {
        ShowCaret( wid );
        caretDisplayed = true;
    }

} /* MyShowCaret */

/*
 * MyHideCaret - HideCaret w/o additive effects
 */
void MyHideCaret( window_id wid )
{
    if( caretDisplayed ) {
        HideCaret( wid );
        caretDisplayed = false;
    }

} /* MyHideCaret */

void MyKillCaret( window_id wid )
{
    if( !caretKilled ) {
        MyHideCaret( wid );
        HideCaret( wid );
        caretKilled = true;
    }
}

void MyRaiseCaret( window_id wid )
{
    if( caretKilled ) {
        ShowCaret( wid );
        caretKilled = false;
        MyShowCaret( wid );
    }
}

