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


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "vi.h"
#include "keys.h"
#include "win.h"
#include "mouse.h"
#include "source.h"
#ifdef __WIN__
#include "winvi.h"
#endif

static  bool    cursorNeedsDisplay = FALSE;

/*
 * NewMessageWindow - create a new Message window
 */
int NewMessageWindow( void )
{
    if( !EditFlags.WindowsStarted ) {
        return( ERR_NO_ERR );
    }
    if( MessageWindow != NO_WINDOW ) {
        CloseAWindow( MessageWindow );
        MessageWindow = NO_WINDOW;
    }
    return( NewWindow2( &MessageWindow, &messagew_info ) );

} /* NewMessageWindow */

#ifndef __WIN__
/*
 * NewWindow2 - build a new window, using window_info struct
 */
int NewWindow2( window_id *wn, window_info *wi )
{
    return( NewWindow( wn, wi->x1, wi->y1, wi->x2, wi->y2,
                   wi->has_border, wi->border_color1,
                   wi->border_color2,  &wi->text ) );

} /* NewWindow2 */
#endif


#ifndef __WIN__
/*
 * Message1 - display message on line 1
 */
void Message1( char *str, ... )
{
    va_list     al;
    char        tmp[MAX_STR];

    if( !EditFlags.EchoOn || MessageWindow < 0 ) {
        return;
    }
    ClearWindow( MessageWindow );
    va_start( al, str );
    MyVSprintf( tmp,str, al );
    va_end( al );
    tmp[WindMaxWidth-1] = 0;

    if( !EditFlags.LineDisplay ) {
        DisplayLineInWindow( MessageWindow, 1, tmp );
    } else {
        MyPrintf("%s\n", tmp );
    }

} /* Message1 */

/*
 * Message2 - display message on line 2
 */
void Message2( char *str, ... )
{
    va_list     al;
    char        tmp[MAX_STR];

    if( !EditFlags.EchoOn || MessageWindow < 0 ) {
        return;
    }
    va_start( al, str );
    MyVSprintf( tmp,str, al );
    va_end( al );
    tmp[WindMaxWidth-1] = 0;

    if( !EditFlags.LineDisplay ) {
        DisplayLineInWindow( MessageWindow, 2, tmp );
    } else {
        MyPrintf("%s\n", tmp );
    }

} /* Message2 */
#endif

/*
 * WPrintfLine - printf text on a window line
 */
int WPrintfLine( window_id w, int line, char *str, ... )
{
    va_list     al;
    char        tmp[MAX_STR];

    va_start( al, str );
    MyVSprintf( tmp,str, al );
    va_end( al );
    tmp[WindMaxWidth-1] = 0;

    return( DisplayLineInWindow( w, line, tmp ) );

} /* WPrintfLine */

/*
 * SetWindowCursor - flag cursor as needing display before next event
 */
void SetWindowCursor( void )
{
    cursorNeedsDisplay = TRUE;
#ifndef __WIN__
    SetWindowCursorForReal();
#endif
}

/*
 * SetWindowCursorForReal - set cursor in current window, if cursorNeedsDisp
 */
void SetWindowCursorForReal( void )
{
    if( !cursorNeedsDisplay || EditFlags.DisplayHold ) {
        return;
    }
    if( CurrentFile == NULL ) {
        SetCursorOnScreen( -1, -1 );
        return;
    }
#ifndef __WIN__
    SetGenericWindowCursor( CurrentWindow, (int) (CurrentLineNumber-TopOfPage+1),
                    VirtualCursorPosition()-LeftColumn );
#else
    // for windows assume tabs to be of lenght 1
    if( !EditFlags.RealTabs ){
        SetGenericWindowCursor( CurrentWindow, (int) (CurrentLineNumber-TopOfPage+1),
                        VirtualCursorPosition()-LeftColumn );
    } else {

        SetGenericWindowCursor( CurrentWindow, (int) (CurrentLineNumber-TopOfPage+1),
                                VirtualCursorPosition() );
    }
#endif

    cursorNeedsDisplay = FALSE;

} /* SetWindowCursorForReal */

/*
 * DisplayExtraInfo - display info in extra window
 */
int DisplayExtraInfo( window_info *wi, window_id *wn, char _NEAR * _NEAR * data,
                int numopts )
{
    int i,j;

    wi->y2 = wi->y1+numopts+1;

    i = NewWindow2( wn, wi );
    if( i ) {
        return( i );
    }
    WindowTitle( *wn, "Special Keys" );
    for( j=0;j<numopts;j++ ) {
        DisplayLineInWindow( *wn, j+1, data[j] );
    }
    return( ERR_NO_ERR );

} /* DisplayExtraInfo */

/*
 * ColumnInWindow - check if a column is in the current window
 */
bool ColumnInWindow( int col, int *diff )
{
    int text_cols;
    int xcol;

    if( col < 1 ) {
        *diff = col - 1;
        return( FALSE );
    }
    xcol = xcol;
    text_cols = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_COLS );
    if( col > text_cols ) {
        *diff = col - text_cols;
        return( FALSE );
    }
    return( TRUE );
} /* ColumnInWindow */

/*
 * SetWindowSizes
 */
void SetWindowSizes( void )
{
    VarAddGlobalLong( "SW", WindMaxWidth );
    VarAddGlobalLong( "SH", WindMaxHeight );
    SpinX = WindMaxWidth-15;
    ClockX = WindMaxWidth-9;
    filecw_info.x2 = WindMaxWidth-5;
    filecw_info.y2 = WindMaxHeight-8;
    repcntw_info.y1 = WindMaxHeight-5;
    repcntw_info.y2 = WindMaxHeight-3;
#ifdef __WIN__
    editw_info.x2 = WindMaxWidth-1;
    editw_info.y2 = WindMaxHeight-3;
    cmdlinew_info.x1 = 2;
    cmdlinew_info.x2 = WindMaxWidth-3;
    cmdlinew_info.y1 = WindMaxHeight-7;
    cmdlinew_info.y2 = WindMaxHeight-5;
    messagew_info.x2 = WindMaxWidth-1;
    messagew_info.y1 = WindMaxHeight-2;
    messagew_info.y2 = WindMaxHeight-1;
#else
    editw_info.x2 = WindMaxWidth-1;
    editw_info.y2 = WindMaxHeight-2;
    cmdlinew_info.x1 = 0;
    cmdlinew_info.x2 = WindMaxWidth-1;
    cmdlinew_info.y1 = WindMaxHeight-1;
    cmdlinew_info.y2 = WindMaxHeight-1;
    messagew_info.x2 = WindMaxWidth-1;
    messagew_info.y1 = WindMaxHeight-1;
    messagew_info.y2 = WindMaxHeight-1;
#endif
    dirw_info.x2 = WindMaxWidth-14;
    dirw_info.y2 = WindMaxHeight-7;
    setw_info.y2 = WindMaxHeight-4;
    filelistw_info.x2 = WindMaxWidth-2;
    filelistw_info.y2 = WindMaxHeight-7;
    statusw_info.y1 = WindMaxHeight-2;
    statusw_info.y2 = WindMaxHeight-1;

} /* SetWindSizes */

/*
 * CurrentWindowResize - as it sounds
 */
int CurrentWindowResize( int x1, int y1, int x2, int y2 )
{
    int         i,text_lines;
    linenum     ln;

    if( EditFlags.LineNumbers ) {
        if( EditFlags.LineNumsOnRight ) {
            i = ResizeWindow( CurrentWindow,x1,y1,x2+LineNumWinWidth,y2,TRUE );
        } else {
            i = ResizeWindow( CurrentWindow,x1-LineNumWinWidth,y1,x2,y2,TRUE );
        }
    } else {
        i = ResizeWindow( CurrentWindow,x1,y1,x2,y2,TRUE );
    }
    if( i ) {
        return( i );
    }
    text_lines = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES );
    if( CurrentLineNumber >= TopOfPage+text_lines ) {
        ln = TopOfPage+text_lines-1;
        GoToLineNoRelCurs( ln );
    }
    CheckLeftColumn();
    DCDisplayAllLines();
    SetWindowCursor();
    if( EditFlags.LineNumbers ) {
        CloseAWindow( CurrNumWindow );
        i = LineNumbersSetup();
        if( i ) {
            return( i );
        }
    }
    PositionVerticalScrollThumb( CurrentWindow, TopOfPage,
                CurrentFile->fcb_tail->end_line );

    return( ERR_NO_ERR );

} /* CurrentWindowResize */

/*
 * SetFileWindowTitle - set the title of the current window
 */
void SetFileWindowTitle( window_id cw, info *cinfo, bool hilite )
{
    char        *n;
    char        name[MAX_STR];

    if( CurrentFile->dup_count > 0 ) {
        MySprintf( name,"%s [%d]", CurrentFile->name, cinfo->DuplicateID );
        n = name;
    } else {
        n = CurrentFile->name;
    }
    WindowTitleAOI( cw, n, hilite );
    #ifdef __WIN__
        SetWindowText( cw, n );
    #endif

} /* SetFileWindowTitle */

/*
 * ResetAllWindows - reset all edit windows
 */
void ResetAllWindows( void )
{
    info        *cinfo;
    info        *oldcurr;

    cinfo = InfoHead;
    oldcurr = CurrentInfo;

    while( cinfo != NULL ) {
        SaveCurrentInfo();
        BringUpFile( cinfo, FALSE );
        ResetWindow( &CurrentWindow );
        cinfo = cinfo->next;
    }

    if( oldcurr != NULL ) {
        SaveCurrentInfo();
        BringUpFile( oldcurr, TRUE );
    }

} /* ResetAllWindows */
