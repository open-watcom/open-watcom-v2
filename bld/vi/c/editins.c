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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _M_I86
#include <i86.h>
#endif
#include "vi.h"
#include "keys.h"
#include "source.h"
#include "menu.h"
#include "win.h"

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
        currLineRepUndo = TRUE;
        GetCurrentLine();
        needNewUndoForLine = FALSE;
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
        currLineRepUndo = FALSE;
    } else {
        EndUndoGroup( UndoStack );
    }

} /* doneWithCurrentLine */

/*
 * DoneCurrentInsert - finished current insertion of text
 */
void DoneCurrentInsert( bool trim )
{
    if( EditFlags.InsertModeActive ) {
        EditFlags.InsertModeActive = FALSE;
        if( trim ) {
            trimWorkLine();
        }
        if( CurrentColumn > WorkLine->len ) {
            if( EditFlags.Modeless ) {
                GoToColumnOK( WorkLine->len+1 );
            } else {
                GoToColumnOK( WorkLine->len );
            }
        }
        doneWithCurrentLine();
        EndUndoGroup( UndoStack );
        if( !EditFlags.Modeless ) {
            NewCursor( CurrentWindow, NormalCursorType );
            SetWindowCursor();
        }
        EditFlags.EscapedInsertChar = FALSE;
        EditFlags.NoReplaceSearchString = FALSE;
    }
} /* DoneCurrentInsert */

/*
 * UpdateEditStatus - update current editing status
 */
void UpdateEditStatus( void )
{
    if( overStrike ) {
        UpdateCurrentStatus( CSTATUS_OVERSTRIKE );
        EditFlags.WasOverstrike = TRUE;
        NewCursor( CurrentWindow, OverstrikeCursorType );
    } else {
        UpdateCurrentStatus( CSTATUS_INSERT );
        EditFlags.WasOverstrike = FALSE;
        NewCursor( CurrentWindow, InsertCursorType );
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
        WorkLine->data[ 0 ] = ch;
        WorkLine->data[ 1 ] = 0;
        WorkLine->len = 1;
        DisplayWorkLine( SSKillsFlags( ch ) );
        return;
    }

    overChar = WorkLine->data[ CurrentColumn - 1 ];
    DisplayWorkLine( SSKillsFlags( ch ) || SSKillsFlags( overChar ) );

    if( !overStrike ) {
        for( i = WorkLine->len; i >= CurrentColumn - 1; i-- ) {
            WorkLine->data[ i + 1 ] = WorkLine->data[ i ];
        }
        WorkLine->data[ CurrentColumn - 1 ] = ch;
        WorkLine->len++;
    } else {
        WorkLine->data[ CurrentColumn - 1] = ch;
        if( CurrentColumn - 1 == WorkLine->len ) {
            WorkLine->len++;
            WorkLine->data[ WorkLine->len ] = 0;
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

    if( WrapMargin != 0 ) {
        if( WrapMargin < 0 ) {
            width = -WrapMargin;
        } else {
            width = WindowAuxInfo( CurrentWindow, WIND_INFO_WIDTH ) - WrapMargin;
        }
        if( CurrentColumn > width ) {
            for( i = CurrentColumn - 1;i>=0;i-- ) {
                if( isspace( WorkLine->data[ i ] ) ) {
                    pos = CurrentColumn -1 - i;
                    CurrentColumn = i+2;
                    old_ai = EditFlags.AutoIndent;
                    EditFlags.AutoIndent = FALSE;
                    IMEnter();
                    EditFlags.AutoIndent = old_ai;
                    GoToColumnOK( pos );
                    return;
                }
            }
        }
    }

} /* checkWrapMargin */

#define WHITE_SPACE( x ) ( (x) == ' ' || (x) == '\t' )

/*
 * trimWorkLine - remove trailing whitespace from work line
 */
static int trimWorkLine( void )
{
    int i,len;

    len = 0;
    if( EditFlags.CMode ) {
        i = WorkLine->len - 1;
        while( i>=0 && WHITE_SPACE( WorkLine->data[i] ) ) {
            i--;
        }
        if( i == -1 ) {
            len = RealLineLen( WorkLine->data );
        }
        WorkLine->len = i+1;
        WorkLine->data[i+1] = 0;
    }
    return( len );

} /* trimWorkLine */

/*
 * insertChar - insert a char into the working line
 */
static int insertChar( bool add_to_abbrev, bool move_to_new_col )
{
    if( WorkLine->len == MaxLine ) {
        return( ERR_NO_ERR );
    }
    addChar( LastEvent );
    if( move_to_new_col ) {
        GoToColumn( CurrentColumn+1, WorkLine->len+1 );
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
int IMChar( void )
{
    if( CurrentFile == NULL ) {
        return( ERR_NO_ERR );
    }
    CurrentFile->need_autosave = TRUE;

    startNewLineUndo();
    if( EditFlags.EscapedInsertChar ) {
        DisplayWorkLine( SSKillsFlags( LastEvent ) ||
                         SSKillsFlags( WorkLine->data[CurrentColumn-1] ) );
        WorkLine->data[CurrentColumn-1] = LastEvent;
        GoToColumn( CurrentColumn +1, WorkLine->len+1 );
        EditFlags.EscapedInsertChar = FALSE;
        return( ERR_NO_ERR );
    }

    return( insertChar( TRUE, TRUE ) );

} /* IMChar */

/*
 * IMEsc - handle ESC from insert mode
 */
int IMEsc( void )
{
    DoneCurrentInsert( TRUE );
    return( ERR_NO_ERR );

} /* IMEsc */

/*
 * IMEnter - process the enter key in insert mode
 */
int IMEnter( void )
{
    char        *buff,*buffx;
    int         len,col,el;

    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }
    CurrentFile->need_autosave = TRUE;

    startNewLineUndo();
    CheckAbbrev( abbrevBuff, &abbrevCnt );
    abbrevCnt = 0;

    /*
     * truncate the working line
     */
    buff = StaticAlloc();
    buffx = StaticAlloc();
    el = WorkLine->len - CurrentColumn+1;
    if( el > 0 && WorkLine->len > 0 ) {
        memcpy( buff, &WorkLine->data[CurrentColumn-1], el + 1 );
        WorkLine->len -= el;
        WorkLine->data[CurrentColumn-1] = 0;
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
        CurrentLineReplaceUndoEnd( FALSE );
        currLineRepUndo = FALSE;
    }

    /*
     * create a new line, insert leading spaces if needed
     * and copy in the truncation
     */
    if( EditFlags.AutoIndent ) {
        len = GetAutoIndentAmount( buffx, len, FALSE );
        el += len;
        strcat( buffx, buff );
        AddNewLineAroundCurrent( buffx,el, INSERT_AFTER );
        col = len+1;
    } else {
        AddNewLineAroundCurrent( buff,el, INSERT_AFTER );
        col = 1;
    }
    UndoInsert( CurrentLineNumber+1, CurrentLineNumber+1, UndoStack );

    /*
     * display the result
     */
    DCDisplayAllLines();
    GoToLineRelCurs( CurrentLineNumber + 1 );
    GoToColumnOK( col );
    GetCurrentLine();
    StaticFree( buff );
    StaticFree( buffx );
    return( ERR_NO_ERR );

} /* IMEnter */

/*
 * IMBackSpace - process the backspace key in insert mode
 */
int IMBackSpace( void )
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
    if( CurrentColumn == 1 ) {

        if( !EditFlags.WrapBackSpace ) {
            return( ERR_NO_ERR );
        }
        if( CurrentLineNumber ==1 ) {
            return( ERR_NO_ERR );
        }
        stay_at_end = FALSE;
        if( WorkLine->len == 0 ) {
            stay_at_end = TRUE;
        }
        doneWithCurrentLine();
        abbrevCnt = 0;
        GoToLineRelCurs( CurrentLineNumber - 1 );
        GoToColumnOnCurrentLine( CurrentLine->len );
        mv_right = TRUE;
        if( CurrentLine->len == 0 ) {
            mv_right = FALSE;
        }
        GenericJoinCurrentLineToNext( FALSE );
        if( mv_right && !stay_at_end ) {
            GoToColumnOnCurrentLine( CurrentColumn + 1 );
        }
        if( stay_at_end ) {
            GoToColumnOK( CurrentLine->len+1 );
        }
        CurrentLineReplaceUndoStart();
        currLineRepUndo = TRUE;
        GetCurrentLine();
        return( ERR_NO_ERR );
    }
    killedChar = WorkLine->data[ CurrentColumn - 2 ];
    overChar = WorkLine->data[ CurrentColumn - 1 ];
    for( i = CurrentColumn-1; i <= WorkLine->len+1; i++ ) {
        WorkLine->data[i-1] = WorkLine->data[i];
    }
    WorkLine->len--;
    GoToColumn( CurrentColumn - 1, WorkLine->len+1 );
    DisplayWorkLine( SSKillsFlags( killedChar ) || SSKillsFlags( overChar ) );
    return( ERR_NO_ERR );

} /* IMBackSpace */

/*
 * IMDelete - handle DEL key pressed in insert mode
 */
int IMDelete( void )
{
    int wlen;

    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }

    startNewLineUndo();
    wlen = WorkLine->len+1;
    if( wlen == 0 ) {
        wlen = CurrentLine->len+1;
    }
    if( EditFlags.Modeless && CurrentColumn == wlen && CurrentLine->next ) {
        /* go to beginning of next line
        */
        GoToLineRelCurs( CurrentLineNumber + 1 );
        GoToColumnOK( 1 );
        GetCurrentLine();
    } else {
        GoToColumn( CurrentColumn + 1, wlen );
        if( CurrentColumn != wlen-1 || abbrevCnt == 0 ) {
            abbrevCnt++;        /* gets subtracted by IMBackSpace */
        }
    }
    return( IMBackSpace() );

} /* IMDelete */

/*
 * IMDeleteML - delete char iff no selection
 */
int IMDeleteML( void )
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
int IMBackSpaceML( void )
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
int IMMouseEvent( void )
{
    if( LastMouseEvent == MOUSE_MOVE
        || LastMouseEvent == MOUSE_RELEASE
        || LastMouseEvent == MOUSE_RELEASE_R ) {
        return( ERR_NO_ERR );
    }
    EditFlags.ReturnToInsertMode = TRUE;
    DoneCurrentInsert( TRUE );
    AddCurrentMouseEvent();
    return( ERR_NO_ERR );

} /* IMMouseEvent */

/*
 * IMCursorKey - handle cursor keys in insert mode
 */
int IMCursorKey( void )
{
    int         wlen;
    event       *ev;
    int         type;

    wlen = WorkLine->len+1;
    if( wlen == 0 ) {
        wlen = CurrentLine->len+1;
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
        GoToColumnOK( wlen);
        abbrevCnt = 0;
        return( ERR_NO_ERR );
    case VI_KEY( LEFT ):
        GoToColumn( CurrentColumn - 1, wlen);
        abbrevCnt = 0;
        return( ERR_NO_ERR );
    case VI_KEY( RIGHT ):
        GoToColumn( CurrentColumn + 1, wlen);
        abbrevCnt = 0;
        return( ERR_NO_ERR );
    }

    /*
     * handle movement that leaves the current line
     */
    ev = &EventList[ LastEvent ];
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
    if( CurrentColumn > WorkLine->len ) {
        GoToColumnOK( WorkLine->len+1 );
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
    needNewUndoForLine = TRUE;
    abbrevCnt = 0;
    return( ERR_NO_ERR );

} /* IMCursorKey */

/*
 * IMMenuKey - process menu keys from insert mode
 */
int IMMenuKey( void )
{
    if( IsMenuHotKey( LastEvent - VI_KEY( ALT_A ) + 'A' ) ) {
        DoneCurrentInsert( TRUE );
        KeyAdd( LastEvent );
        EditFlags.ReturnToInsertMode = TRUE;
    }
    return( ERR_NO_ERR );

} /* IMMenuKey */

/*
 * IMSpace - handle a space in insert mode
 */
int IMSpace( void )
{
    startNewLineUndo();
    CheckAbbrev( abbrevBuff, &abbrevCnt );
    abbrevCnt = 0;
    return( insertChar( FALSE, TRUE ) );

} /* IMSpace */

/*
 * IMTabs - handle tabs in insert mode
 */
int IMTabs( void )
{
    char        *buff;
    bool        back;
    int         cp,vc,tc,add;
    int         i,j;
    int         len;

    startNewLineUndo();
    CheckAbbrev( abbrevBuff, &abbrevCnt );
    abbrevCnt = 0;
    switch( LastEvent ) {
    case VI_KEY( TAB ):
        if( EditFlags.RealTabs ) {
            if( WorkLine->len + 1 >= MaxLine ) {
                break;
            }
            addChar( '\t' );
            GoToColumn( CurrentColumn +1, WorkLine->len+1 );
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
        vc = VirtualCursorPosition();
        if( CurrentColumn-1 == WorkLine->len && !EditFlags.Modeless ) {
            add=1;
        } else {
            add=0;
        }
        switch( LastEvent ) {
        case VI_KEY( SHIFT_TAB ):
            j = ShiftTab( vc, TabAmount );
            back = TRUE;
            break;
        case VI_KEY( CTRL_D ):
            j = ShiftTab( vc, ShiftWidth );
            back = TRUE;
            break;
        case VI_KEY( TAB ):
            j = Tab( vc, TabAmount );
            back = FALSE;
            break;
        case VI_KEY( CTRL_T ):
            j = Tab( vc, ShiftWidth );
            back = FALSE;
            break;
        }
        if( back && (vc - j < 1) ) {
            break;
        } else if( RealLineLen( WorkLine->data) + j >= MaxLine ) {
            break;
        }

        /*
         * create a real version of the line
         */
        buff = StaticAlloc();
        ExpandTabsInABufferUpToColumn( CurrentColumn-1, WorkLine->data,
                                WorkLine->len, buff, MaxLine );
        len = strlen( buff );

        /*
         * put in/suck out the tab
         */
        tc = vc-1;
        if( back ) {
            for( i=tc; i<=len+1; i++ ) {
                buff[i-j] = buff[i];
            }
            len -= j;
        } else {
            for( i=len; i>=tc; i-- ) {
                buff[i+j] = buff[i];
            }
            for( i=0; i<j; i++ ) {
                buff[tc+i] = ' ';
            }
            len += j;
        }

        /*
         * put tabs back in
         */
        if( back ) {
            cp = vc-j;
        } else {
            cp = vc+j;
        }
        if( EditFlags.RealTabs ) {
            ConvertSpacesToTabsUpToColumn( cp, buff, len, WorkLine->data, MaxLine );
        } else {
            strcpy( WorkLine->data, buff );
        }
        WorkLine->len = strlen( WorkLine->data );
        StaticFree( buff );
        cp = RealCursorPosition( cp )+add;
        GoToColumn( cp, WorkLine->len+1 );
        DisplayWorkLine( FALSE );
        break;
    }
    return( ERR_NO_ERR );

} /* IMTabs */

/*
 * IMEscapeNextChar - handle ^Q and ^V in insert mode
 */
int IMEscapeNextChar( void )
{
    int rc;

    startNewLineUndo();
    LastEvent = '^';
    rc = insertChar( FALSE, FALSE );
    EditFlags.EscapedInsertChar = TRUE;
    return( rc );

} /* IMEscapeNextChar */

/*
 * IMInsert - handle INS key pressed in insert mode
 */
int IMInsert( void )
{
    if( overStrike ) {
        overStrike = FALSE;
    } else {
        overStrike = TRUE;
    }
    UpdateEditStatus();
    return( ERR_NO_ERR );

} /* IMInsert */

/*
 * tempMatch - show a temporary match
 */
static void tempMatch( linenum mline, int mcol )
{
    SaveCurrentFilePos();
    GoToLineNoRelCurs( mline );
    GoToColumnOK( mcol );
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
int IMCloseBracket( void )
{
    int         rc,mcol;
    linenum     mline;

    startNewLineUndo();
    insertChar( TRUE, FALSE );
    if( EditFlags.ShowMatch ) {

        ReplaceCurrentLine();
        rc = FindMatch( &mline, &mcol );
        if( !rc ) {
            tempMatch( mline, mcol );
        }
        GetCurrentLine();

    }
    GoToColumn( CurrentColumn+1, WorkLine->len+1 );
    return( ERR_NO_ERR );

} /* IMCloseBracket */

/*
 * getBracketLoc - find a matching '(' for a ')'
 */
static int getBracketLoc( linenum *mline, int *mcol )
{
    int         rc;
    char        tmp[3];
    int         len;
    linenum     lne;
    bool        oldmagic = EditFlags.Magic;

    EditFlags.Magic = TRUE;
    tmp[0] = '\\';
    tmp[1] = ')';
    tmp[2] = 0;
    lne = CurrentLineNumber;
    rc = GetFind( tmp, mline, mcol, &len, FINDFL_BACKWARDS|FINDFL_NOERROR);
    if( *mline != CurrentLineNumber ) {
        EditFlags.Magic = oldmagic;
        return( ERR_FIND_NOT_FOUND );
    }
    if( rc ) {
        EditFlags.Magic = oldmagic;
        return( rc );
    }

    /*
     * find the matching '('
     */
    CurrentLineNumber = *mline;
    CurrentColumn = *mcol;
    CGimmeLinePtr( CurrentLineNumber, &CurrentFcb, &CurrentLine );
    rc = FindMatch( mline, mcol );
    EditFlags.Magic = oldmagic;
    return( rc );

} /* getBracketLoc */

/*
 * findMatchingBrace find '{' for a '}'
 */
static int findMatchingBrace( linenum *mline, int *mcol  )
{
    int         rc;
    int         col;
    linenum     sline;

    rc = FindMatch( mline, mcol );
    if( rc ) {
        return( rc );
    }
    SaveCurrentFilePos();
    CurrentLineNumber = *mline;
    CurrentColumn = *mcol;
    CGimmeLinePtr( CurrentLineNumber, &CurrentFcb, &CurrentLine );

    rc = getBracketLoc( &sline, &col );
    RestoreCurrentFilePos();
    if( !rc ) {
        *mline = sline;
        *mcol = col;
    }
    return( ERR_NO_ERR );

} /* findMatchingBrace */

/*
 * IMCloseBrace - handle '}' in insert mode
 */
int IMCloseBrace( void )
{
    int         i,j;
    int         ts;
    fcb         *cfcb;
    line        *cline;
    int         rc;
    int         newcol;
    linenum     mline;
    int         mcol;

    startNewLineUndo();
    insertChar( TRUE, FALSE );
    newcol = CurrentColumn+1;
    if( EditFlags.ShowMatch ) {
        ReplaceCurrentLine();
        rc = FindMatch( &mline, &mcol );
        if( !rc ) {
            tempMatch( mline, mcol );
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
            rc = findMatchingBrace( &mline, &mcol );
            if( !rc ) {
                newcol = VirtualCursorPosition();
                CGimmeLinePtr( mline, &cfcb, &cline );
                i = FindStartOfALine( cline );
                i = GetVirtualCursorPosition( cline->data, i );
                j = i - VirtualCursorPosition2( CurrentColumn );
                ts = ShiftWidth;
                if( j > 0 ) {
                    ShiftWidth = j;
                    Shift( CurrentLineNumber, CurrentLineNumber, '>', FALSE );
                } else if( j < 0 ) {
                    ShiftWidth = -j;
                    Shift( CurrentLineNumber, CurrentLineNumber, '<', FALSE );
                }
                ShiftWidth = ts;
                newcol = 1+RealCursorPosition( j + newcol );
            }
            GetCurrentLine();
        }
    }
    GoToColumn( newcol, WorkLine->len+1 );
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
    EditFlags.Dotable = TRUE;
    EditFlags.NoReplaceSearchString = TRUE;
    EditFlags.InsertModeActive = TRUE;
    if( col > 1 && CurrentLine->len == 0 ) {
        col = 1;
    }
    GoToColumnOK( col );
    GetCurrentLine();

} /* continueInsertText */

/*
 * stdInsert - standard insert on a line
 */
static int stdInsert( int col, bool overstrike )
{
    int rc;

    if( rc = ModificationTest() ) {
        return( rc );
    }
    StartUndoGroup( UndoStack );
    CurrentLineReplaceUndoStart();
    currLineRepUndo = TRUE;
    continueInsertText( col, overstrike );
    return( ERR_NO_ERR );

} /* stdInsert */

/*
 * DeleteAndInsertText - delete text range, then insert at beginning
 */
int DeleteAndInsertText( int scol, int ecol )
{
    int rc,startcol;

    StartUndoGroup( UndoStack );
    CurrentLineReplaceUndoStart();
    currLineRepUndo = TRUE;
    if( ecol >= 0 ) {
        if( CurrentLine->len > 0 ) {
            rc = DeleteBlockFromCurrentLine( scol, ecol, FALSE );
            if( !rc ) {
                startcol = CurrentColumn;
                if( scol > ecol ) {
                    startcol = ecol+1;
                }
                if( startcol > WorkLine->len ) {
                    startcol = WorkLine->len+1;
                }
                DisplayWorkLine( TRUE );
                ReplaceCurrentLine();
                rc=GoToColumnOK( startcol );
            }
            if( rc ) {
                CurrentLineReplaceUndoCancel();
                EndUndoGroup( UndoStack );
                return( rc );
            }
        } else {
            ReplaceCurrentLine();
        }
    }
    continueInsertText( CurrentColumn, FALSE );
    return( ERR_NO_ERR );

} /* DeleteAndInsertText */

/*
 * insertTextOnOtherLine - open up a different line
 */
static int insertTextOnOtherLine( insert_dir type )
{
    char        *buffx;
    int         i,j;
    linenum     a,b;
    bool        above_line = FALSE;

    if( i = ModificationTest() ) {
        return( i );
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
    a = b = CurrentLineNumber+1;
    if( type == INSERT_BEFORE ) {
        a--;
        b--;
        above_line = TRUE;
    }

    /*
     * set up for undo
     */
    StartUndoGroup( UndoStack );
    Modified( TRUE );
    StartUndoGroup( UndoStack );
    UndoInsert( a, a, UndoStack );
    currLineRepUndo = FALSE;

    /*
     * add extra line, and spaces if needed.
     */
    if( EditFlags.AutoIndent ) {
        buffx = StaticAlloc();
        i = GetAutoIndentAmount( buffx, 0, above_line );
        AddNewLineAroundCurrent( buffx,i, type );
        StaticFree( buffx );
        j = i+1;
    } else {
        AddNewLineAroundCurrent( NULL,0, type );
        j = 1;
    }
    GoToLineRelCurs( b );
    GoToColumn( j,CurrentLine->len+1 );
    DCDisplayAllLines();
    continueInsertText( CurrentColumn, FALSE );
    return( ERR_NO_ERR );

} /* insertTextOnOtherLine */

int InsertTextOnNextLine( void )
{
    return( insertTextOnOtherLine( INSERT_AFTER ) );
}

int InsertTextOnPreviousLine( void )
{
    return( insertTextOnOtherLine( INSERT_BEFORE ) );
}

int InsertTextAtCursor( void )
{
    return( stdInsert( CurrentColumn, FALSE ) );
}

int InsertTextAfterCursor( void )
{
    return( stdInsert( CurrentColumn+1, FALSE ) );
}

int InsertTextAtLineStart( void )
{
    if( CurrentFile != NULL ) {
        return( stdInsert( FindStartOfCurrentLine(), FALSE ) );
    }
    return( ERR_NO_ERR );
}

int InsertTextAtLineEnd( void )
{
    if( CurrentFile != NULL ) {
        return( stdInsert( CurrentLine->len+1, FALSE ) );
    }
    return( ERR_NO_ERR );
}

/*
 * DoReplaceText - go into overstrike mode
 */
int DoReplaceText( void )
{
    int         rc;

    rc = stdInsert( CurrentColumn, TRUE );
    return( rc );

} /* DoReplaceText */

/*
 * InsertLikeLast - go into insert mode, the same mode as last time
 */
int InsertLikeLast( void )
{
    bool        overstrike;
    int         rc;

    if( EditFlags.WasOverstrike ) {
        overstrike = TRUE;
    } else {
        overstrike = FALSE;
    }
    rc = stdInsert( CurrentColumn, overstrike );
    return( rc );

} /* InsertLikeLast */

typedef struct mode {
    struct mode *prev;
    bool        wasinsert:1;
    bool        wasoverstrike:1;
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
    DoneCurrentInsert( TRUE );

} /* PushMode */

/*
 * PopMode - restore to previous mode
 */
int PopMode( void )
{
    mode        *cmode;
    int         rc;

    rc = ERR_NO_ERR;
    if( modeTail == NULL ) {
        return( ERR_NO_ERR );
    }
    cmode = modeTail;
    modeTail = cmode->prev;

    DoneCurrentInsert( TRUE );
    if( cmode->wasinsert ) {
        rc = stdInsert( CurrentColumn, cmode->wasoverstrike );
    }
    MemFree( cmode );
    return( rc );

} /* PopMode */
