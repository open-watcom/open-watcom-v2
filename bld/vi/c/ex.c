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
#include <string.h>
#include "vi.h"
#include "ex.h"
#include "win.h"
#include "colors.h"

static window_info      exwInfo =
        { 0, BLACK, WHITE, { BRIGHT_WHITE, BLACK, 0 }, { WHITE, BLACK, 0 }, 0,24,79,24 };
/*
 * EnterExMode - start Ex emulation mode
 */
int EnterExMode( void )
{
    int         i,rc;
    window_id   clw;
    char        *st;
    char        *prompt;

    if( EditFlags.InputKeyMapMode ) {
        return( ERR_NO_ERR );
    }
    i = WindMaxHeight-1;
    exwInfo.y1 = exwInfo.y2 = i;
    exwInfo.x2 = WindMaxWidth-1;
    SetCursorOnScreen( i, 0 );
    EditFlags.ExMode = TRUE;
    EditFlags.LineDisplay = TRUE;
    EditFlags.ClockActive = FALSE;
    MyPrintf("\nEntering EX mode (type vi to return)\n");
    i = NewWindow2( &clw, &exwInfo );
    if( i ) {
        return( i );
    }
    st = MemAlloc( MaxLine );

    while( 1 ) {
        if( EditFlags.Appending ) {
            prompt = "";
        } else {
            prompt = ":";
        }
        rc = ReadStringInWindow( clw, 1, prompt, st, MaxLine, &CLHist );
        MyPrintf("\n");
        if( !rc ) {
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
        if( rc > 0 ) {
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
int ProcessEx( linenum n1, linenum n2, bool n2f, int dmt, int tkn,
                 char *data )
{
    int         rc = ERR_INVALID_COMMAND,i;
    char        word[MAX_STR],wordback[MAX_STR];
    linenum     addr,tlines;
    fcb         *cfcb,*s1fcb,*s2fcb;
    line        *cline;

    NextWord1( data, word );
    strcpy( wordback, word );
    if( GetAddress( word, &addr ) ) {
        addr = -1;
    }
    tlines = n2-n1+1;

    switch( tkn ) {
    case EX_T_APPEND:
        if( !EditFlags.ExMode ) {
            return( ERR_ONLY_VALID_IN_EX_MODE );
        }
        if( !n2f ) {
            rc = Append( n1, TRUE );
        }
        break;
    case EX_T_CHANGE:
        if( !EditFlags.ExMode ) {
            return( ERR_ONLY_VALID_IN_EX_MODE );
        }
        StartUndoGroup( UndoStack );
        rc = DeleteLineRange( n1, n2, 0 );
        if( rc ) {
            EndUndoGroup( UndoStack );
            break;
        }
        rc = Append( n1-1, FALSE );
        if( rc ) {
            EndUndoGroup( UndoStack );
            break;
        }
        break;
    case EX_T_COPY:
        if( addr < 0 || IsPastLastLine( addr ) ) {
            return( ERR_INVALID_ADDRESS );
        }
        i = GetCopyOfLineRange( n1,n2, &s1fcb, &s2fcb );
        if( i ) {
            break;
        }
        rc = InsertLines( addr, s1fcb, s2fcb, UndoStack );
        GoToLineNoRelCurs( addr );
        if( !rc ) {
            Message1( strCmmsg, tlines, "copied", addr );
        }
        break;
    case EX_T_INSERT:
        if( !EditFlags.ExMode ) {
            return( ERR_ONLY_VALID_IN_EX_MODE );
        }
        if( !n2f ) {
            rc = Append( n1-1, TRUE );
        }
        break;
    case EX_T_JOIN:
        if( SaveAndResetFilePos( n1 ) != ERR_NO_ERR ) {
            rc = ERR_NO_SUCH_LINE;
            break;
        }
        if( tlines == 1 ) {
            n2 = n1+1;
            tlines = 2;
        }
        SetRepeatCount( tlines-1 );
        rc = JoinCurrentLineToNext();
        RestoreCurrentFilePos();
        GoToLineNoRelCurs( n1 );
        if( !rc ) {
            Message1( "lines %l to %l joined", n1,n2 );
        }
        break;
    case EX_T_LIST:
        if( !EditFlags.ExMode ) {
            return( ERR_ONLY_VALID_IN_EX_MODE );
        }
        rc = CGimmeLinePtr( n1, &cfcb, &cline );
        while( !rc ) {
            if( EditFlags.LineNumbers ) {
                MyPrintf( "%M %s\n",n1, cline->data );
            } else {
                MyPrintf("%s\n",cline->data );
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
        if( rc ) {
            EndUndoGroup( UndoStack );
            break;
        }
        if( addr > n2 ) {
            addr -= tlines;
        } else if( addr >= n1 && addr <= n2 ) {
            addr = n1;
        }
        rc = InsertLines( addr, WorkSavebuf->first.fcb_head,
                            WorkSavebuf->fcb_tail, UndoStack );
        EndUndoGroup( UndoStack );
        GoToLineNoRelCurs( addr );
        if( !rc ) {
            Message1( strCmmsg, tlines, "moved", addr );
        }
        break;
    case EX_T_UNDO:
        if( dmt ) {
            rc = DoUndoUndo();
        } else {
            rc = DoUndo();
        }
        break;
    case EX_T_EQUALS:
        Message1( "%l", n1 );
        rc = ERR_NO_ERR;
        break;
    case EX_T_VERSION:
        rc = DoVersion();
        break;
    case EX_T_VISUAL:
        if( EditFlags.LineDisplay ) {
            ScreenPage( -1 );
            EditFlags.ExMode = FALSE;
            EditFlags.LineDisplay = FALSE;
            EditFlags.ClockActive = TRUE;
            ReDisplayScreen();
            DoVersion();
        }
        if( wordback[0] != 0 ) {
            rc = EditFile( wordback, dmt );
        } else {
            rc = ERR_NO_ERR;
        }
        break;
    }
    return( rc );

} /* ProcessEx */
