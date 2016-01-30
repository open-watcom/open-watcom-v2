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
* Description:  Default window sizes, window manipulation routines.
*
****************************************************************************/


#include "vi.h"
#include "win.h"
#include "mouse.h"
#include "myprtf.h"

static  bool    cursorNeedsDisplay = false;

/*
 * NewMessageWindow - create a new Message window
 */
vi_rc NewMessageWindow( void )
{
    if( !EditFlags.WindowsStarted ) {
        return( ERR_NO_ERR );
    }
    if( !BAD_ID( message_window_id ) ) {
        CloseAWindow( message_window_id );
        message_window_id = NO_WINDOW;
    }
    return( NewWindow2( &message_window_id, &messagew_info ) );

} /* NewMessageWindow */

#ifndef __WIN__
/*
 * NewWindow2 - build a new window, using window_info struct
 */
vi_rc NewWindow2( window_id *wid, window_info *wi )
{
    return( NewWindow( wid, wi->area.x1, wi->area.y1, wi->area.x2, wi->area.y2,
                       wi->has_border, wi->border_color1,
                       wi->border_color2, &wi->text_style ) );

} /* NewWindow2 */
#endif


#ifndef __WIN__
/*
 * Message1 - display message on line 1
 */
void Message1( const char *str, ... )
{
    va_list     al;
    char        tmp[MAX_STR];

    if( !EditFlags.EchoOn || BAD_ID( message_window_id ) ) {
        return;
    }
    ClearWindow( message_window_id );
    va_start( al, str );
    MyVSprintf( tmp, str, al );
    va_end( al );
    tmp[EditVars.WindMaxWidth - 1] = 0;

    if( !EditFlags.LineDisplay ) {
        DisplayLineInWindow( message_window_id, 1, tmp );
    } else {
        MyPrintf( "%s\n", tmp );
    }

} /* Message1 */

/*
 * Message1Box - display message on line 1 (differs from Message1 only on Windows)
 */
void Message1Box( const char *str, ... )
{
    va_list     al;
    char        tmp[MAX_STR];

    if( !EditFlags.EchoOn || BAD_ID( message_window_id ) ) {
        return;
    }
    ClearWindow( message_window_id );
    va_start( al, str );
    MyVSprintf( tmp, str, al );
    va_end( al );
    tmp[EditVars.WindMaxWidth - 1] = 0;

    if( !EditFlags.LineDisplay ) {
        DisplayLineInWindow( message_window_id, 1, tmp );
    } else {
        MyPrintf( "%s\n", tmp );
    }

} /* Message1 */

/*
 * Message2 - display message on line 2
 */
void Message2( const char *str, ... )
{
    va_list     al;
    char        tmp[MAX_STR];

    if( !EditFlags.EchoOn || BAD_ID( message_window_id ) ) {
        return;
    }
    va_start( al, str );
    MyVSprintf( tmp,str, al );
    va_end( al );
    tmp[EditVars.WindMaxWidth - 1] = 0;

    if( !EditFlags.LineDisplay ) {
        DisplayLineInWindow( message_window_id, 2, tmp );
    } else {
        MyPrintf( "%s\n", tmp );
    }

} /* Message2 */
#endif

/*
 * WPrintfLine - printf text on a window line
 */
vi_rc WPrintfLine( window_id wid, int line, char *str, ... )
{
    va_list     al;
    char        tmp[MAX_STR];

    va_start( al, str );
    MyVSprintf( tmp, str, al );
    va_end( al );
    tmp[EditVars.WindMaxWidth - 1] = 0;

    return( DisplayLineInWindow( wid, line, tmp ) );

} /* WPrintfLine */

/*
 * SetWindowCursor - flag cursor as needing display before next event
 */
void SetWindowCursor( void )
{
    cursorNeedsDisplay = true;
#ifndef __WIN__
    SetWindowCursorForReal();
#endif

} /* SetWindowCursor */

/*
 * SetWindowCursorForReal - set cursor in current window, if cursorNeedsDisp
 */
void SetWindowCursorForReal( void )
{
    if( !cursorNeedsDisplay || EditFlags.DisplayHold ) {
        return;
    }
    if( CurrentFile == NULL ) {
        HideCursor();
        return;
    }
#ifdef __WIN__
    // for windows assume tabs to be of lenght 1
    if( !EditFlags.RealTabs ){
        SetGenericWindowCursor( current_window_id, (int) (CurrentPos.line - LeftTopPos.line + 1),
                                VirtualColumnOnCurrentLine( CurrentPos.column ) - LeftTopPos.column );
    } else {

        SetGenericWindowCursor( current_window_id, (int) (CurrentPos.line - LeftTopPos.line + 1),
                                VirtualColumnOnCurrentLine( CurrentPos.column ) );
    }
#else
    SetGenericWindowCursor( current_window_id, (int) (CurrentPos.line - LeftTopPos.line + 1),
                            VirtualColumnOnCurrentLine( CurrentPos.column ) - LeftTopPos.column );
#endif

    cursorNeedsDisplay = false;

} /* SetWindowCursorForReal */

/*
 * DisplayExtraInfo - display info in extra window
 */
vi_rc DisplayExtraInfo( window_info *wi, window_id *wid, char _NEAR * _NEAR *data,
                      int numopts )
{
    int     j;
    vi_rc   rc;

    wi->area.y2 = wi->area.y1 + numopts + 1;

    rc = NewWindow2( wid, wi );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    WindowTitle( *wid, "Special Keys" );
    for( j = 0; j < numopts; j++ ) {
        DisplayLineInWindow( *wid, j + 1, data[j] );
    }
    return( ERR_NO_ERR );

} /* DisplayExtraInfo */

/*
 * ColumnInWindow - check if a column is in the current window
 */
bool ColumnInWindow( int col, int *diff )
{
    int text_cols;

    if( col < 1 ) {
        *diff = col - 1;
        return( false );
    }
    text_cols = WindowAuxInfo( current_window_id, WIND_INFO_TEXT_COLS );
    if( col > text_cols ) {
        *diff = col - text_cols;
        return( false );
    }
    return( true );

} /* ColumnInWindow */

/*
 * SetWindowSizes
 */
void SetWindowSizes( void )
{
    VarAddGlobalLong( "SW", EditVars.WindMaxWidth );
    VarAddGlobalLong( "SH", EditVars.WindMaxHeight );
    EditVars.SpinX = EditVars.WindMaxWidth - 15;
    EditVars.ClockX = EditVars.WindMaxWidth - 9;
    filecw_info.area.x2 = EditVars.WindMaxWidth - 5;
    filecw_info.area.y2 = EditVars.WindMaxHeight - 8;
    repcntw_info.area.y1 = EditVars.WindMaxHeight - 5;
    repcntw_info.area.y2 = EditVars.WindMaxHeight - 3;
#ifdef __WIN__
    editw_info.area.x2 = EditVars.WindMaxWidth - 1;
    editw_info.area.y2 = EditVars.WindMaxHeight - 3;
    cmdlinew_info.area.x1 = 2;
    cmdlinew_info.area.x2 = EditVars.WindMaxWidth - 3;
    cmdlinew_info.area.y1 = EditVars.WindMaxHeight - 7;
    cmdlinew_info.area.y2 = EditVars.WindMaxHeight - 5;
    messagew_info.area.x2 = EditVars.WindMaxWidth - 1;
    messagew_info.area.y1 = EditVars.WindMaxHeight - 2;
    messagew_info.area.y2 = EditVars.WindMaxHeight - 1;
#else
    editw_info.area.x2 = EditVars.WindMaxWidth - 1;
    editw_info.area.y2 = EditVars.WindMaxHeight - 2;
    cmdlinew_info.area.x1 = 0;
    cmdlinew_info.area.x2 = EditVars.WindMaxWidth - 1;
    cmdlinew_info.area.y1 = EditVars.WindMaxHeight - 1;
    cmdlinew_info.area.y2 = EditVars.WindMaxHeight - 1;
    messagew_info.area.x2 = EditVars.WindMaxWidth - 1;
    messagew_info.area.y1 = EditVars.WindMaxHeight - 1;
    messagew_info.area.y2 = EditVars.WindMaxHeight - 1;
#endif
    dirw_info.area.x2 = EditVars.WindMaxWidth - 12;
    dirw_info.area.y2 = EditVars.WindMaxHeight - 7;
    setw_info.area.y2 = EditVars.WindMaxHeight - 4;
    filelistw_info.area.x2 = EditVars.WindMaxWidth - 2;
    filelistw_info.area.y2 = EditVars.WindMaxHeight - 7;
    statusw_info.area.y1 = EditVars.WindMaxHeight - 2;
    statusw_info.area.y2 = EditVars.WindMaxHeight - 1;

} /* SetWindSizes */

/*
 * ResizeCurrentWindow - as it sounds
 */
vi_rc ResizeCurrentWindow( int x1, int y1, int x2, int y2 )
{
    int         text_lines;
    linenum     ln;
    vi_rc       rc;

    if( EditFlags.LineNumbers ) {
        if( EditFlags.LineNumsOnRight ) {
            rc = ResizeWindow( current_window_id, x1, y1, x2 + EditVars.LineNumWinWidth, y2, true );
        } else {
            rc = ResizeWindow( current_window_id, x1 - EditVars.LineNumWinWidth, y1, x2, y2, true );
        }
    } else {
        rc = ResizeWindow( current_window_id, x1, y1, x2, y2, true );
    }
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    text_lines = WindowAuxInfo( current_window_id, WIND_INFO_TEXT_LINES );
    if( CurrentPos.line >= LeftTopPos.line + text_lines ) {
        ln = LeftTopPos.line + text_lines - 1;
        GoToLineNoRelCurs( ln );
    }
    CheckLeftColumn();
    DCDisplayAllLines();
    SetWindowCursor();
    if( EditFlags.LineNumbers ) {
        CloseAWindow( curr_num_window_id );
        rc = LineNumbersSetup();
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }
    PositionVerticalScrollThumb( current_window_id, LeftTopPos.line,
                                 CurrentFile->fcbs.tail->end_line );

    return( ERR_NO_ERR );

} /* ResizeCurrentWindow */

/*
 * SetFileWindowTitle - set the title of the current window
 */
void SetFileWindowTitle( window_id wid, info *cinfo, bool hilite )
{
    char        *n;
    char        name[MAX_STR];

    if( CurrentFile->dup_count > 0 ) {
        MySprintf( name, "%s [%d]", CurrentFile->name, cinfo->DuplicateID );
        n = name;
    } else {
        n = CurrentFile->name;
    }
    WindowTitleAOI( wid, n, hilite );
#ifdef __WIN__
    SetWindowText( wid, n );
    UpdateFileTypeIcon( wid, n );
#endif

} /* SetFileWindowTitle */

/*
 * ResetAllWindows - reset all edit windows
 */
void ResetAllWindows( void )
{
    info        *cinfo;
    info        *oldcurr;

    oldcurr = CurrentInfo;
    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        SaveCurrentInfo();
        BringUpFile( cinfo, false );
        ResetWindow( &current_window_id );
    }

    if( oldcurr != NULL ) {
        SaveCurrentInfo();
        BringUpFile( oldcurr, true );
    }

} /* ResetAllWindows */
