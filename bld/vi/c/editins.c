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
#ifdef _M_I86
    #include <i86.h>
#endif
#include "menu.h"
#include "win.h"
#include "rxsupp.h"

static bool     currLineRepUndo;
static bool     overStrike;
static bool     needNewUndoForLine;
static int      abbrevCnt;
static char     abbrevBuff[MAX_STR];

/*
 * startNewLineUndo - start current line undo stuff
 */
static void startNewLineUndo( void )
{
    if( needNewUndoForLine ) {
        CurrentLineReplaceUndoStart();
        currLineRepUndo = true;
        GetCurrentLine();
        needNewUndoForLine = false;
    }

} /* startNewLineUndo */

/*
 * doneWithCurrentLine - update current line, and update undo record
 */
static void doneWithCurrentLine( void )
{
    startNewLineUndo();
    ReplaceCurrentLine();
    if( currLineRepUndo ) {
        ConditionalCurrentLineReplaceUndoEnd();
        currLineRepUndo = false;
    } else {
        EndUndoGroup( UndoStack );
    }

} /* doneWithCurrentLine */

#define WHITE_SPACE( x ) ((x) == ' ' || (x) == '\t')

/*
 * trimWorkLine - remove trailing whitespace from work line
 */
static int trimWorkLine( void )
{
    int i, len;

    len = 0;
    if( EditFlags.CMode || EditFlags.RemoveSpaceTrailing ) {
        for( i = WorkLine->len - 1; i >= 0; --i ) {
            if( !WHITE_SPACE( WorkLine->data[i] ) ) {
                break;
            }
        }
        if( i == -1 ) {
            len = VirtualLineLen( WorkLine->data );
        }
        WorkLine->len = i + 1;
        WorkLine->data[i + 1] = 0;
    }
    return( len );

} /* trimWorkLine */

/*
 * DoneCurrentInsert - finished current insertion of text
 */
void DoneCurrentInsert( bool trim )
{
    if( EditFlags.InsertModeActive ) {
        EditFlags.InsertModeActive = false;
        if( trim ) {
            trimWorkLine();
        }
        if( CurrentPos.column > WorkLine->len ) {
            if( EditFlags.Modeless ) {
                GoToColumnOK( WorkLine->len + 1 );
            } else {
                GoToColumnOK( WorkLine->len );
            }
        }
        doneWithCurrentLine();
        EndUndoGroup( UndoStack );
        if( !EditFlags.Modeless ) {
            NewCursor( CurrentWindow, EditVars.NormalCursorType );
            SetWindowCursor();
        }
        EditFlags.EscapedInsertChar = false;
        EditFlags.NoReplaceSearchString = false;
    }

} /* DoneCurrentInsert */

/*
 * UpdateEditStatus - update current editing status
 */
void UpdateEditStatus( void )
{
    if( overStrike ) {
        UpdateCurrentStatus( CSTATUS_OVERSTRIKE );
        EditFlags.WasOverstrike = true;
        NewCursor( CurrentWindow, EditVars.OverstrikeCursorType );
    } else {
        UpdateCurrentStatus( CSTATUS_INSERT );
        EditFlags.WasOverstrike = false;
        NewCursor( CurrentWindow, EditVars.InsertCursorType );
    }
    SetWindowCursor();

} /* UpdateEditStatus */

/*
 * addChar - add character to working line
 */
static void addChar( char ch )
{
    char    overChar;
    int     i;

    if( WorkLine->len == 0 ) {
        WorkLine->data[0] = ch;
        WorkLine->data[1] = 0;
        WorkLine->len = 1;
        DisplayWorkLine( SSKillsFlags( ch ) );
        return;
    }

    overChar = WorkLine->data[CurrentPos.column - 1];
    DisplayWorkLine( SSKillsFlags( ch ) || SSKillsFlags( overChar ) );

    if( !overStrike ) {
        for( i = WorkLine->len; i >= CurrentPos.column - 1; i-- ) {
            WorkLine->data[i + 1] = WorkLine->data[i];
        }
        WorkLine->data[CurrentPos.column - 1] = ch;
        WorkLine->len++;
    } else {
        WorkLine->data[CurrentPos.column - 1] = ch;
        if( CurrentPos.column - 1 == WorkLine->len ) {
            WorkLine->len++;
            WorkLine->data[WorkLine->len] = 0;
        }
    }

} /* addChar */

/*
 * checkWrapMargin - see if we have moved past the wrap margin
 */
static void checkWrapMargin( void )
{
    int         i;
    int         width;
    int         pos;
    bool        old_ai;

    if( EditVars.WrapMargin != 0 ) {
        if( EditVars.WrapMargin < 0 ) {
            width = -EditVars.WrapMargin;
        } else {
            width = WindowAuxInfo( CurrentWindow, WIND_INFO_WIDTH ) - EditVars.WrapMargin;
        }
        if( CurrentPos.column > width ) {
            for( i = CurrentPos.column - 1; i >= 0; i-- ) {
                if( isspace( WorkLine->data[i] ) ) {
                    pos = CurrentPos.column - 1 - i;
                    CurrentPos.column = i + 2;
                    old_ai = EditFlags.AutoIndent;
                    EditFlags.AutoIndent = false;
                    IMEnter();
                    EditFlags.AutoIndent = old_ai;
                    GoToColumnOK( pos );
                    return;
                }
            }
        }
    }

} /* checkWrapMargin */

/*
 * insertChar - insert a char into the working line
 */
static vi_rc insertChar( bool add_to_abbrev, bool move_to_new_col )
{
    if( WorkLine->len == EditVars.MaxLine ) {
        return( ERR_NO_ERR );
    }
    addChar( LastEvent );
    if( move_to_new_col ) {
        GoToColumn( CurrentPos.column + 1, WorkLine->len + 1 );
    }
    if( abbrevCnt < sizeof( abbrevBuff ) && add_to_abbrev ) {
        abbrevBuff[abbrevCnt++] = LastEvent;
    }
    checkWrapMargin();
    return( ERR_NO_ERR );

} /* insertChar */

/*
 * IMChar - insert a character in insert mode
 */
vi_rc IMChar( void )
{
    if( CurrentFile == NULL ) {
        return( ERR_NO_ERR );
    }
    CurrentFile->need_autosave = true;

    startNewLineUndo();
    if( EditFlags.EscapedInsertChar ) {
        DisplayWorkLine( SSKillsFlags( LastEvent ) ||
                         SSKillsFlags( WorkLine->data[CurrentPos.column - 1] ) );
        WorkLine->data[CurrentPos.column - 1] = LastEvent;
        GoToColumn( CurrentPos.column + 1, WorkLine->len + 1 );
        EditFlags.EscapedInsertChar = false;
        return( ERR_NO_ERR );
    }

    return( insertChar( true, true ) );

} /* IMChar */

/*
 * IMEsc - handle ESC from insert mode
 */
vi_rc IMEsc( void )
{
    DoneCurrentInsert( true );
    return( ERR_NO_ERR );

} /* IMEsc */

/*
 * IMEnter - process the enter key in insert mode
 */
vi_rc IMEnter( void )
{
    char        *buff, *buffx;
    int         len, col, el;

    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }
    CurrentFile->need_autosave = true;

    startNewLineUndo();
    CheckAbbrev( abbrevBuff, &abbrevCnt );

    /*
     * truncate the working line
     */
    buff = StaticAlloc();
    buffx = StaticAlloc();
    el = WorkLine->len - CurrentPos.column + 1;
    if( el > 0 && WorkLine->len > 0 ) {
        memcpy( buff, &WorkLine->data[CurrentPos.column - 1], el + 1 );
        WorkLine->len -= el;
        WorkLine->data[CurrentPos.column - 1] = 0;
    } else {
        el = 0;
        buff[0] = 0;
    }

    len = trimWorkLine();

    /*
     * replace the current line with the working copy
     */
    ReplaceCurrentLine();
    if( currLineRepUndo ) {
        CurrentLineReplaceUndoEnd( false );
        currLineRepUndo = false;
    }

    /*
     * create a new line, insert leading spaces if needed
     * and copy in the truncation
     */
    if( EditFlags.AutoIndent ) {
        len = GetAutoIndentAmount( buffx, len, false );
        el += len;
        strcat( buffx, buff );
        AddNewLineAroundCurrent( buffx, el, INSERT_AFTER );
        col = len + 1;
    } else {
        AddNewLineAroundCurrent( buff, el, INSERT_AFTER );
        col = 1;
    }
    UndoInsert( CurrentPos.line + 1, CurrentPos.line + 1, UndoStack );

    /*
     * display the result
     */
    DCDisplayAllLines();
    GoToLineRelCurs( CurrentPos.line + 1 );
    GoToColumnOK( col );
    GetCurrentLine();
    StaticFree( buff );
    StaticFree( buffx );
    return( ERR_NO_ERR );

} /* IMEnter */

/*
 * IMBackSpace - process the backspace key in insert mode
 */
vi_rc IMBackSpace( void )
{
    char        killedChar, overChar;
    bool        mv_right;
    bool        stay_at_end;
    int         i;

    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }

    startNewLineUndo();
    if( abbrevCnt > 0 ) {
        abbrevCnt--;
    }
    if( CurrentPos.column == 1 ) {

        if( !EditFlags.WrapBackSpace ) {
            return( ERR_NO_ERR );
        }
        if( CurrentPos.line ==1 ) {
            return( ERR_NO_ERR );
        }
        stay_at_end = ( WorkLine->len == 0 );
        doneWithCurrentLine();
        abbrevCnt = 0;
        GoToLineRelCurs( CurrentPos.line - 1 );
        GoToColumnOnCurrentLine( CurrentLine->len );
        mv_right = ( CurrentLine->len != 0 );
        GenericJoinCurrentLineToNext( false );
        if( mv_right && !stay_at_end ) {
            GoToColumnOnCurrentLine( CurrentPos.column + 1 );
        }
        if( stay_at_end ) {
            GoToColumnOK( CurrentLine->len + 1 );
        }
        CurrentLineReplaceUndoStart();
        currLineRepUndo = true;
        GetCurrentLine();
        return( ERR_NO_ERR );
    }
    killedChar = WorkLine->data[CurrentPos.column - 2];
    overChar = WorkLine->data[CurrentPos.column - 1];
    for( i = CurrentPos.column - 1; i <= WorkLine->len + 1; i++ ) {
        WorkLine->data[i - 1] = WorkLine->data[i];
    }
    WorkLine->len--;
    GoToColumn( CurrentPos.column - 1, WorkLine->len + 1 );
    DisplayWorkLine( SSKillsFlags( killedChar ) || SSKillsFlags( overChar ) );
    return( ERR_NO_ERR );

} /* IMBackSpace */

/*
 * IMDelete - handle DEL key pressed in insert mode
 */
vi_rc IMDelete( void )
{
    int wlen;

    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }

    startNewLineUndo();
    wlen = WorkLine->len + 1;
    if( wlen == 0 ) {
        wlen = CurrentLine->len + 1;
    }
    if( EditFlags.Modeless && CurrentPos.column == wlen && CurrentLine->next ) {
        /* go to beginning of next line */
        GoToLineRelCurs( CurrentPos.line + 1 );
        GoToColumnOK( 1 );
        GetCurrentLine();
    } else {
        GoToColumn( CurrentPos.column + 1, wlen );
        if( CurrentPos.column != wlen - 1 || abbrevCnt == 0 ) {
            abbrevCnt++;        /* gets subtracted by IMBackSpace */
        }
    }
    return( IMBackSpace() );

} /* IMDelete */

/*
 * IMDeleteML - delete char iff no selection
 */
vi_rc IMDeleteML( void )
{
    if( !SelRgn.selected ) {
        return( IMDelete() );
    } else {
        return( ERR_NO_ERR );
    }

} /* IMDeleteML */

/*
 * IMBackSpaceML - backspace iff no selection
 */
vi_rc IMBackSpaceML( void )
{
    if( !SelRgn.selected ) {
        return( IMBackSpace() );
    } else {
        return( ERR_NO_ERR );
    }

} /* IMBackSpaceML */

/*
 * IMMouseEvent - handle a mouse event in insert mode
 */
vi_rc IMMouseEvent( void )
{
    if( LastMouseEvent == MOUSE_MOVE
        || LastMouseEvent == MOUSE_RELEASE
        || LastMouseEvent == MOUSE_RELEASE_R ) {
        return( ERR_NO_ERR );
    }
    EditFlags.ReturnToInsertMode = true;
    DoneCurrentInsert( true );
    AddCurrentMouseEvent();
    return( ERR_NO_ERR );

} /* IMMouseEvent */

/*
 * IMCursorKey - handle cursor keys in insert mode
 */
vi_rc IMCursorKey( void )
{
    int         wlen;
    event       *ev;
    int         type;

    wlen = WorkLine->len + 1;
    if( wlen == 0 ) {
        wlen = CurrentLine->len + 1;
    }

    /*
     * handle movement on the same line
     */
    switch( LastEvent ) {
    case VI_KEY( HOME ):
        GoToColumn( 1, wlen );
        abbrevCnt = 0;
        return( ERR_NO_ERR );
    case VI_KEY( END ):
        GoToColumnOK( wlen );
        abbrevCnt = 0;
        return( ERR_NO_ERR );
    case VI_KEY( LEFT ):
        GoToColumn( CurrentPos.column - 1, wlen );
        abbrevCnt = 0;
        return( ERR_NO_ERR );
    case VI_KEY( RIGHT ):
        GoToColumn( CurrentPos.column + 1, wlen );
        abbrevCnt = 0;
        return( ERR_NO_ERR );
    }

    /*
     * handle movement that leaves the current line
     */
    ev = &EventList[LastEvent];
    if( EditFlags.Modeless ) {
        type = ev->alt_b.type;
    } else {
        type = ev->b.type;
    }

    if( needNewUndoForLine ) {
        if( type == EVENT_REL_MOVE || type == EVENT_ABS_MOVE ) {
            DoMove( ev );
        } else {
            if( EditFlags.Modeless ) {
                ev->alt_rtn.old();
            } else {
                ev->rtn.old();
            }
        }
        return( ERR_NO_ERR );
    }
    if( CurrentPos.column > WorkLine->len ) {
        GoToColumnOK( WorkLine->len + 1 );
    }
    doneWithCurrentLine();
    if( type == EVENT_REL_MOVE || type == EVENT_ABS_MOVE ) {
        DoMove( ev );
    } else {
        if( EditFlags.Modeless ) {
            ev->alt_rtn.old();
        } else {
            ev->rtn.old();
        }
    }
    needNewUndoForLine = true;
    abbrevCnt = 0;
    return( ERR_NO_ERR );

} /* IMCursorKey */

/*
 * IMMenuKey - process menu keys from insert mode
 */
vi_rc IMMenuKey( void )
{
    if( IsMenuHotKey( LastEvent ) ) {
        DoneCurrentInsert( true );
        KeyAdd( LastEvent );
        EditFlags.ReturnToInsertMode = true;
    }
    return( ERR_NO_ERR );

} /* IMMenuKey */

/*
 * IMSpace - handle a space in insert mode
 */
vi_rc IMSpace( void )
{
    startNewLineUndo();
    CheckAbbrev( abbrevBuff, &abbrevCnt );
    return( insertChar( false, true ) );

} /* IMSpace */

/*
 * IMTabs - handle tabs in insert mode
 */
vi_rc IMTabs( void )
{
    char        *buff;
    bool        back;
    int         cp, vc, tc, add;
    int         i, j;
    int         len;

    startNewLineUndo();
    CheckAbbrev( abbrevBuff, &abbrevCnt );
    switch( LastEvent ) {
    case VI_KEY( TAB ):
        if( EditFlags.RealTabs ) {
            if( WorkLine->len + 1 >= EditVars.MaxLine ) {
                break;
            }
            addChar( '\t' );
            GoToColumn( CurrentPos.column + 1, WorkLine->len + 1 );
            checkWrapMargin();
            break;
        }
        /* fall through if not real tabs */
    case VI_KEY( CTRL_T ):
    case VI_KEY( SHIFT_TAB ):
    case VI_KEY( CTRL_D ):
        /*
         * get position of cursor on virtual line
         */
        vc = VirtualColumnOnCurrentLine( CurrentPos.column );
        if( CurrentPos.column - 1 == WorkLine->len && !EditFlags.Modeless ) {
            add = 1;
        } else {
            add = 0;
        }
        j = 0;
        back = false;
        switch( LastEvent ) {
        case VI_KEY( SHIFT_TAB ):
            j = ShiftTab( vc, EditVars.TabAmount );
            back = true;
            break;
        case VI_KEY( CTRL_D ):
            j = ShiftTab( vc, EditVars.ShiftWidth );
            back = true;
            break;
        case VI_KEY( TAB ):
            j = Tab( vc, EditVars.TabAmount );
            break;
        case VI_KEY( CTRL_T ):
            j = Tab( vc, EditVars.ShiftWidth );
            break;
        }
        if( back && (vc - j < 1) ) {
            break;
        } else if( VirtualLineLen( WorkLine->data ) + j >= EditVars.MaxLine ) {
            break;
        }

        /*
         * create a real version of the line
         */
        buff = StaticAlloc();
        ExpandTabsInABufferUpToColumn( CurrentPos.column - 1, WorkLine->data, WorkLine->len, buff, EditVars.MaxLine );
        len = strlen( buff );

        /*
         * put in/suck out the tab
         */
        tc = vc - 1;
        if( back ) {
            for( i = tc; i <= len + 1; i++ ) {
                buff[i - j] = buff[i];
            }
            len -= j;
        } else {
            for( i = len; i >= tc; i-- ) {
                buff[i + j] = buff[i];
            }
            for( i = 0; i < j; i++ ) {
                buff[tc + i] = ' ';
            }
            len += j;
        }

        /*
         * put tabs back in
         */
        if( back ) {
            cp = vc - j;
        } else {
            cp = vc + j;
        }
        if( EditFlags.RealTabs ) {
            ConvertSpacesToTabsUpToColumn( cp, buff, len, WorkLine->data, EditVars.MaxLine );
        } else {
            strcpy( WorkLine->data, buff );
        }
        WorkLine->len = strlen( WorkLine->data );
        StaticFree( buff );
        cp = RealColumnOnCurrentLine( cp ) + add;
        GoToColumn( cp, WorkLine->len + 1 );
        DisplayWorkLine( false );
        break;
    }
    return( ERR_NO_ERR );

} /* IMTabs */

/*
 * IMEscapeNextChar - handle ^Q and ^V in insert mode
 */
vi_rc IMEscapeNextChar( void )
{
    vi_rc   rc;

    startNewLineUndo();
    LastEvent = '^';
    rc = insertChar( false, false );
    EditFlags.EscapedInsertChar = true;
    return( rc );

} /* IMEscapeNextChar */

/*
 * IMInsert - handle INS key pressed in insert mode
 */
vi_rc IMInsert( void )
{
    overStrike = !overStrike;
    UpdateEditStatus();
    return( ERR_NO_ERR );

} /* IMInsert */

/*
 * tempMatch - show a temporary match
 */
static void tempMatch( i_mark *pos )
{
    SaveCurrentFilePos();
    GoToLineNoRelCurs( pos->line );
    GoToColumnOK( pos->column );
#ifdef __WIN__
    DCDisplayAllLines();
    DCUpdate();

    SetWindowCursorForReal();
    MyDelay( 150 );
    RestoreCurrentFilePos();

    DCDisplayAllLines();
    DCUpdate();
#else
    MyDelay( 150 );
    RestoreCurrentFilePos();
    DCDisplayAllLines();
#endif

} /* tempMatch */

/*
 * IMCloseBracket - handle a ')' being entered in insert mode
 */
vi_rc IMCloseBracket( void )
{
    vi_rc       rc;
    i_mark      pos;

    startNewLineUndo();
    insertChar( true, false );
    if( EditFlags.ShowMatch ) {

        ReplaceCurrentLine();
        rc = FindMatch( &pos );
        if( rc == ERR_NO_ERR ) {
            tempMatch( &pos );
        }
        GetCurrentLine();

    }
    GoToColumn( CurrentPos.column + 1, WorkLine->len + 1 );
    return( ERR_NO_ERR );

} /* IMCloseBracket */

/*
 * getBracketLoc - find a matching '(' for a ')'
 */
static vi_rc getBracketLoc( i_mark *pos )
{
    vi_rc       rc;
    char        tmp[3];
    int         len;
//    linenum     lne;

    tmp[0] = '\\';
    tmp[1] = ')';
    tmp[2] = 0;
//    lne = CurrentPos.line;
    RegExpAttrSave( -1, NULL );
    rc = GetFind( tmp, pos, &len, FINDFL_BACKWARDS | FINDFL_NOERROR | FINDFL_NOCHANGE );
    RegExpAttrRestore();
    if( pos->line != CurrentPos.line ) {
        return( ERR_FIND_NOT_FOUND );
    }
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    /*
     * find the matching '('
     */
    CurrentPos = *pos;
    CGimmeLinePtr( CurrentPos.line, &CurrentFcb, &CurrentLine );
    rc = FindMatch( pos );
    return( rc );

} /* getBracketLoc */

/*
 * findMatchingBrace find '{' for a '}'
 */
static vi_rc findMatchingBrace( i_mark *pos1 )
{
    vi_rc       rc;
    i_mark      pos2;

    rc = FindMatch( pos1 );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    SaveCurrentFilePos();
    CurrentPos = *pos1;
    CGimmeLinePtr( CurrentPos.line, &CurrentFcb, &CurrentLine );

    rc = getBracketLoc( &pos2 );
    RestoreCurrentFilePos();
    if( rc == ERR_NO_ERR ) {
        *pos1 = pos2;
    }
    return( ERR_NO_ERR );

} /* findMatchingBrace */

/*
 * IMCloseBrace - handle '}' in insert mode
 */
vi_rc IMCloseBrace( void )
{
    int         i, j;
    int         ts;
    fcb         *cfcb;
    line        *cline;
    vi_rc       rc;
    int         newcol;
    i_mark      pos;

    startNewLineUndo();
    insertChar( true, false );
    newcol = CurrentPos.column + 1;
    if( EditFlags.ShowMatch ) {
        ReplaceCurrentLine();
        rc = FindMatch( &pos );
        if( rc == ERR_NO_ERR ) {
            tempMatch( &pos );
        }
        GetCurrentLine();
    }
    if( EditFlags.CMode ) {
        i = 0;
        while( isspace( WorkLine->data[i] ) ) {
            i++;
        }
        if( WorkLine->data[i] == '}' ) {
            /*
             * added a {, so:
             *   find matching }
             *   find out indentation of that line
             *   shift current line over to that indentation
             *   set current indentation to that
             */

            ReplaceCurrentLine();
            rc = findMatchingBrace( &pos );
            if( rc == ERR_NO_ERR ) {
                newcol = VirtualColumnOnCurrentLine( CurrentPos.column );
                CGimmeLinePtr( pos.line, &cfcb, &cline );
                i = FindStartOfALine( cline );
                i = GetVirtualCursorPosition( cline->data, i );
                j = i - VirtualColumnOnCurrentLine( CurrentPos.column );
                ts = EditVars.ShiftWidth;
                if( j > 0 ) {
                    EditVars.ShiftWidth = j;
                    Shift( CurrentPos.line, CurrentPos.line, '>', false );
                } else if( j < 0 ) {
                    EditVars.ShiftWidth = -j;
                    Shift( CurrentPos.line, CurrentPos.line, '<', false );
                }
                EditVars.ShiftWidth = ts;
                newcol = 1 + RealColumnOnCurrentLine( j + newcol );
            }
            GetCurrentLine();
        }
    }
    GoToColumn( newcol, WorkLine->len + 1 );
    return( ERR_NO_ERR );

} /* IMCloseBrace */

/*
 * continueInsertText - continue in insert mode after mouse events
 */
static void continueInsertText( int col, bool overstrike )
{
    overStrike = overstrike;
    abbrevCnt = 0;
    if( !EditFlags.Modeless ) {
        UpdateEditStatus();
    }
    EditFlags.Dotable = true;
    EditFlags.NoReplaceSearchString = true;
    EditFlags.InsertModeActive = true;
    if( col > 1 && CurrentLine->len == 0 ) {
        col = 1;
    }
    GoToColumnOK( col );
    GetCurrentLine();

} /* continueInsertText */

/*
 * stdInsert - standard insert on a line
 */
static vi_rc stdInsert( int col, bool overstrike )
{
    vi_rc   rc;

    rc = ModificationTest();
    if( rc == ERR_NO_ERR ) {
        StartUndoGroup( UndoStack );
        CurrentLineReplaceUndoStart();
        currLineRepUndo = true;
        continueInsertText( col, overstrike );
    }
    return( rc );

} /* stdInsert */

/*
 * DeleteAndInsertText - delete text range, then insert at beginning
 */
vi_rc DeleteAndInsertText( int scol, int ecol )
{
    int     startcol;
    vi_rc   rc;

    StartUndoGroup( UndoStack );
    CurrentLineReplaceUndoStart();
    currLineRepUndo = true;
    if( ecol >= 0 ) {
        if( CurrentLine->len > 0 ) {
            rc = DeleteBlockFromCurrentLine( scol, ecol, false );
            if( rc == ERR_NO_ERR ) {
                startcol = CurrentPos.column;
                if( scol > ecol ) {
                    startcol = ecol + 1;
                }
                if( startcol > WorkLine->len ) {
                    startcol = WorkLine->len + 1;
                }
                DisplayWorkLine( true );
                ReplaceCurrentLine();
                rc = GoToColumnOK( startcol );
            }
            if( rc != ERR_NO_ERR ) {
                CurrentLineReplaceUndoCancel();
                EndUndoGroup( UndoStack );
                return( rc );
            }
        } else {
            ReplaceCurrentLine();
        }
    }
    continueInsertText( CurrentPos.column, false );
    return( ERR_NO_ERR );

} /* DeleteAndInsertText */

/*
 * insertTextOnOtherLine - open up a different line
 */
static vi_rc insertTextOnOtherLine( insert_dir type )
{
    char        *buffx;
    int         i, j;
    linenum     a, b;
    bool        above_line = false;
    vi_rc       rc;

    rc = ModificationTest();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    /*
     * special case: no data in file
     */
    if( CurrentFcb->nullfcb ) {
        return( InsertTextAfterCursor() );
    }

    /*
     * get line deletion and undo crap
     */
    a = b = CurrentPos.line + 1;
    if( type == INSERT_BEFORE ) {
        a--;
        b--;
        above_line = true;
    }

    /*
     * set up for undo
     */
    StartUndoGroup( UndoStack );
    Modified( true );
    StartUndoGroup( UndoStack );
    UndoInsert( a, a, UndoStack );
    currLineRepUndo = false;

    /*
     * add extra line, and spaces if needed.
     */
    if( EditFlags.AutoIndent ) {
        buffx = StaticAlloc();
        i = GetAutoIndentAmount( buffx, 0, above_line );
        AddNewLineAroundCurrent( buffx, i, type );
        StaticFree( buffx );
        j = i + 1;
    } else {
        AddNewLineAroundCurrent( NULL, 0, type );
        j = 1;
    }
    GoToLineRelCurs( b );
    GoToColumn( j, CurrentLine->len + 1 );
    DCDisplayAllLines();
    continueInsertText( CurrentPos.column, false );
    return( ERR_NO_ERR );

} /* insertTextOnOtherLine */

vi_rc InsertTextOnNextLine( void )
{
    return( insertTextOnOtherLine( INSERT_AFTER ) );
}

vi_rc InsertTextOnPreviousLine( void )
{
    return( insertTextOnOtherLine( INSERT_BEFORE ) );
}

vi_rc InsertTextAtCursor( void )
{
    return( stdInsert( CurrentPos.column, false ) );
}

vi_rc InsertTextAfterCursor( void )
{
    return( stdInsert( CurrentPos.column + 1, false ) );
}

vi_rc InsertTextAtLineStart( void )
{
    if( CurrentFile != NULL ) {
        return( stdInsert( FindStartOfCurrentLine(), false ) );
    }
    return( ERR_NO_ERR );
}

vi_rc InsertTextAtLineEnd( void )
{
    if( CurrentFile != NULL ) {
        return( stdInsert( CurrentLine->len + 1, false ) );
    }
    return( ERR_NO_ERR );
}

/*
 * DoReplaceText - go into overstrike mode
 */
vi_rc DoReplaceText( void )
{
    vi_rc       rc;

    rc = stdInsert( CurrentPos.column, true );
    return( rc );

} /* DoReplaceText */

/*
 * InsertLikeLast - go into insert mode, the same mode as last time
 */
vi_rc InsertLikeLast( void )
{
    vi_rc       rc;

    rc = stdInsert( CurrentPos.column, EditFlags.WasOverstrike );
    return( rc );

} /* InsertLikeLast */

typedef struct mode {
    struct mode *prev;
    bool        wasinsert       : 1;
    bool        wasoverstrike   : 1;
} mode;

static mode     *modeTail;

/*
 * PushMode - push our current mode
 */
void PushMode( void )
{
    mode        *cmode;

    cmode = MemAlloc( sizeof( mode ) );
    cmode->prev = modeTail;
    modeTail = cmode;

    cmode->wasinsert = EditFlags.InsertModeActive;
    cmode->wasoverstrike = EditFlags.WasOverstrike;
    DoneCurrentInsert( true );

} /* PushMode */

/*
 * PopMode - restore to previous mode
 */
vi_rc PopMode( void )
{
    mode        *cmode;
    vi_rc       rc;

    rc = ERR_NO_ERR;
    if( modeTail == NULL ) {
        return( ERR_NO_ERR );
    }
    cmode = modeTail;
    modeTail = cmode->prev;

    DoneCurrentInsert( true );
    if( cmode->wasinsert ) {
        rc = stdInsert( CurrentPos.column, cmode->wasoverstrike );
    }
    MemFree( cmode );
    return( rc );

} /* PopMode */
