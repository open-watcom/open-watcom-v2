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
#include "ex.h"
#include "win.h"

static window_info      exwInfo =
    { 0, BLACK, WHITE, { BRIGHT_WHITE, BLACK, 0 }, { WHITE, BLACK, 0 }, 0, 24, 79, 24 };

/*
 * EnterExMode - start Ex emulation mode
 */
vi_rc EnterExMode( void )
{
    int         i;
    window_id   clw;
    char        *st;
    char        *prompt;
    vi_rc       rc;
    bool        ret;

    if( EditFlags.InputKeyMapMode ) {
        return( ERR_NO_ERR );
    }
    i = EditVars.WindMaxHeight - 1;
    exwInfo.y1 = exwInfo.y2 = i;
    exwInfo.x2 = EditVars.WindMaxWidth - 1;
    SetPosToMessageLine();
    EditFlags.ExMode = true;
    EditFlags.LineDisplay = true;
    EditFlags.ClockActive = false;
    MyPrintf( "\nEntering EX mode (type vi to return)\n" );
    rc = NewWindow2( &clw, &exwInfo );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    st = MemAlloc( EditVars.MaxLine );

    for( ;; ) {
        if( EditFlags.Appending ) {
            prompt = "";
        } else {
            prompt = ":";
        }
        ret = ReadStringInWindow( clw, 1, prompt, st, EditVars.MaxLine, &EditVars.CLHist );
        MyPrintf( "\n" );
        if( !ret ) {
            continue;
        }
        ScreenPage( 1 );
        if( EditFlags.Appending ) {
            AppendAnother( st );
            ScreenPage( -1 );
            continue;
        }
        rc = RunCommandLine( st );
        if( !EditFlags.ExMode ) {
            break;
        }
        if( rc > ERR_NO_ERR ) {
            Error( GetErrorMsg( rc ) );
        }
        ScreenPage( -1 );
    }
    MemFree( st );
    return( ERR_NO_ERR );

} /* EnterExMode */

static char strCmmsg[] = "%l lines %s after line %l";
/*
 * ProcessEx - process an ex command
 */
vi_rc ProcessEx( linenum n1, linenum n2, bool n2f, int tkn, const char *data )
{
    vi_rc       rc = ERR_INVALID_COMMAND, i;
    char        word[MAX_STR];
    linenum     addr, tlines;
    fcb         *cfcb;
    line        *cline;
    fcb_list    fcblist;

    GetNextWord1( data, word );
    data = word;
    if( GetAddress( &data, &addr ) != ERR_NO_ERR ) {
        addr = -1;
    }
    tlines = n2 - n1 + 1;

    switch( tkn ) {
    case EX_T_APPEND:
        if( !EditFlags.ExMode ) {
            return( ERR_ONLY_VALID_IN_EX_MODE );
        }
        if( !n2f ) {
            rc = Append( n1, true );
        }
        break;
    case EX_T_CHANGE:
        if( !EditFlags.ExMode ) {
            return( ERR_ONLY_VALID_IN_EX_MODE );
        }
        StartUndoGroup( UndoStack );
        rc = DeleteLineRange( n1, n2, 0 );
        if( rc != ERR_NO_ERR ) {
            EndUndoGroup( UndoStack );
            break;
        }
        rc = Append( n1 - 1, false );
        if( rc != ERR_NO_ERR ) {
            EndUndoGroup( UndoStack );
            break;
        }
        break;
    case EX_T_COPY:
        if( addr < 0 || IsPastLastLine( addr ) ) {
            return( ERR_INVALID_ADDRESS );
        }
        i = GetCopyOfLineRange( n1, n2, &fcblist );
        if( i ) {
            break;
        }
        rc = InsertLines( addr, &fcblist, UndoStack );
        GoToLineNoRelCurs( addr );
        if( rc == ERR_NO_ERR ) {
            Message1( strCmmsg, tlines, "copied", addr );
        }
        break;
    case EX_T_INSERT:
        if( !EditFlags.ExMode ) {
            return( ERR_ONLY_VALID_IN_EX_MODE );
        }
        if( !n2f ) {
            rc = Append( n1 - 1, true );
        }
        break;
    case EX_T_JOIN:
        if( SaveAndResetFilePos( n1 ) != ERR_NO_ERR ) {
            rc = ERR_NO_SUCH_LINE;
            break;
        }
        if( tlines == 1 ) {
            n2 = n1 + 1;
            tlines = 2;
        }
        SetRepeatCount( tlines - 1 );
        rc = JoinCurrentLineToNext();
        RestoreCurrentFilePos();
        GoToLineNoRelCurs( n1 );
        if( rc == ERR_NO_ERR ) {
            Message1( "lines %l to %l joined", n1, n2 );
        }
        break;
    case EX_T_LIST:
        if( !EditFlags.ExMode ) {
            return( ERR_ONLY_VALID_IN_EX_MODE );
        }
        rc = CGimmeLinePtr( n1, &cfcb, &cline );
        while( !rc ) {
            if( EditFlags.LineNumbers ) {
                MyPrintf( "%M %s\n", n1, cline->data );
            } else {
                MyPrintf( "%s\n", cline->data );
            }
            if( n1 >= n2 ) {
                break;
            }
            n1++;
            rc = CGimmeNextLinePtr( &cfcb, &cline );
        }
        break;
    case EX_T_MARK:
        rc = SetGenericMark( n1, 1, word[0] );
        break;
    case EX_T_MOVE:
        if( addr < 0 || IsPastLastLine( addr ) ) {
            return( ERR_INVALID_ADDRESS );
        }
        SavebufNumber = WORK_SAVEBUF;
        StartUndoGroup( UndoStack );
        rc = DeleteLineRange( n1, n2, SAVEBUF_FLAG );

        if( SavebufNumber != WORK_SAVEBUF ) {
            /* if this changes, the command will fail
             * this could be caused by checking out a read-only file
             * so fix the deleted text and give an error message
             */
            DoUndo();
            return( ERR_INVALID_COMMAND );
        }
        if( rc != ERR_NO_ERR ) {
            EndUndoGroup( UndoStack );
            break;
        }
        if( addr > n2 ) {
            addr -= tlines;
        } else if( addr >= n1 && addr <= n2 ) {
            addr = n1;
        }
        rc = InsertLines( addr, &WorkSavebuf->u.fcbs, UndoStack );
        EndUndoGroup( UndoStack );
        GoToLineNoRelCurs( addr );
        if( rc == ERR_NO_ERR ) {
            Message1( strCmmsg, tlines, "moved", addr );
        }
        break;
    case EX_T_UNDO:
        rc = DoUndo();
        break;
    case EX_T_UNDO_DMT:
        rc = DoUndoUndo();
        break;
    case EX_T_EQUALS:
        Message1( "%l", n1 );
        rc = ERR_NO_ERR;
        break;
    case EX_T_VERSION:
        rc = DoVersion();
        break;
    case EX_T_VISUAL:
    case EX_T_VISUAL_DMT:
        if( EditFlags.LineDisplay ) {
            ScreenPage( -1 );
            EditFlags.ExMode = false;
            EditFlags.LineDisplay = false;
            EditFlags.ClockActive = true;
            ReDisplayScreen();
            DoVersion();
        }
        if( word[0] != 0 ) {
            rc = EditFile( word, ( tkn == EX_T_VISUAL_DMT ) );
        } else {
            rc = ERR_NO_ERR;
        }
        break;
    }
    return( rc );

} /* ProcessEx */
