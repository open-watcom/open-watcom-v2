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
#include "winaux.h"
#include "win.h"
#include <assert.h>

int LineLength( linenum l )
{
    line        *line;
    fcb         *fcb;
    vi_rc       rc;

    rc = CGimmeLinePtr( l, &fcb, &line );
    if( rc != ERR_NO_ERR ) {
        return( 0 );
    }
    return( line->len );
}

vi_rc GetLineRange( range *result, long count, linenum line )
{
    result->line_based = TRUE;
    result->start.line = line;
    line += count - 1;
    CFindLastLine( &result->end.line );
    if( line <= result->end.line ) {
        result->end.line = line;
    }
    return( ERR_NO_ERR );
}

vi_rc Delete( range *r )
{
    vi_rc       rc;

    // need to perform the actual delete
    if( r->line_based ) {
        rc = DeleteLineRange( r->start.line, r->end.line, SAVEBUF_FLAG );
        DCDisplayAllLines();
        LineDeleteMessage( r->start.line, r->end.line );
    } else if( r->start.line != r->end.line ) {
        rc = Cut( r->start.line, r->start.column,
                  r->end.line, r->end.column, TRUE );
        DCDisplayAllLines();
    } else {
        GoToLineNoRelCurs( r->start.line );
        rc = DeleteRangeOnCurrentLine( r->start.column, r->end.column, TRUE );
    }
    // move cursor to tl corner of region
    if( rc == ERR_NO_ERR ) {
        GoToLineNoRelCurs( r->start.line );
        r->start.column += 1;
        if( r->start.column > CurrentLine->len ) {
            if( EditFlags.Modeless ) {
                r->start.column = CurrentLine->len + 1;
            } else {
                r->start.column = CurrentLine->len;
            }
        }
        GoToColumnOnCurrentLine( r->start.column );
        EditFlags.Dotable = TRUE;
    } else {
        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
    }
    return( rc );

} /* Delete */

vi_rc DeleteLines( void )
{
    range       r;

    GetLineRange( &r, GetRepeatCount(), CurrentPos.line );
    return( Delete( &r ) );

} /* DeleteLines */

vi_rc Yank( range *r )
{
    vi_rc       rc;

    if( r->line_based ) {
        rc = YankLineRange( r->start.line, r->end.line );
    } else if( r->start.line == r->end.line ) {
        assert( CurrentPos.line == r->start.line );
        AddLineToSavebuf( CurrentLine->data, r->start.column, r->end.column );
#ifdef __WIN__
        if( LastSavebuf == 0 ) {
            Message1( "%d characters copied into the clipboard",
                      r->end.column - r->start.column + 1 );
        } else {
#endif
            Message1( "%d %s yanked into buffer %c",
                      r->end.column - r->start.column + 1,
                      MSG_CHARACTERS, LastSavebuf );
#ifdef __WIN__
        }
#endif
        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
    } else {
        rc = Cut( r->start.line, r->start.column,
                  r->end.line, r->end.column, FALSE );
    }
    if( rc != ERR_NO_ERR ) {
        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
    }
    return( rc );

} /* Yank */

vi_rc YankLines( void )
{
    range       r;

    GetLineRange( &r, GetRepeatCount(), CurrentPos.line );
    return( Yank( &r ) );

} /* YankLines */

vi_rc Change( range *r )
{
    int         scol, ecol;
    int         tmp, vecol;
    vi_rc       rc;
    vi_key      key;

    /*
     * change line ranges
     */
    if( r->start.line != r->end.line ) {
        StartUndoGroup( UndoStack );
        if( !r->line_based ) {
            rc = Cut( r->start.line, r->start.column,
                      r->end.line, r->end.column, TRUE );
            r->end.column = -1;
            scol = -1;
            ecol = -1;
        } else {
            if( r->start.line == CurrentPos.line ) {
                r->start.line++;
            } else {
                r->end.line--;
            }
            if( r->start.line <= r->end.line ) {
                rc = DeleteLineRange( r->start.line, r->end.line, 0 );
                if( rc != ERR_NO_ERR ) {
                    EndUndoGroup( UndoStack );
                    return( rc );
                }
            }
            scol = FindStartOfCurrentLine() - 1;
            ecol = CurrentLine->len - 1;
        }
        DCDisplayAllLines();
        rc = DeleteAndInsertText( scol, ecol );
        EndUndoGroup( UndoStack );
        return( rc );
    }

    /*
     * change text on current line
     */
    rc = ERR_NO_ERR;
    GoToLineNoRelCurs( r->start.line );
    ecol = r->end.column;
    scol = r->start.column;
#ifdef __WIN__
    vecol = vecol;
//    GetCurrentLine();
    strcpy( WorkLine->data, CurrentLine->data );
    tmp = WorkLine->data[ecol];
    WorkLine->data[ecol] = '$';
#else
    vecol = VirtualColumnOnCurrentLine( ecol + 1 );
    vecol--;
    ExpandTabsInABuffer( CurrentLine->data, CurrentLine->len, WorkLine->data, MaxLine );
    WorkLine->len = strlen( WorkLine->data );
    tmp = WorkLine->data[vecol];
    WorkLine->data[vecol] = '$';
#endif
    if( WorkLine->len == 0 ) {
        WorkLine->data[1] = 0;
    }
    EditFlags.InsertModeActive = TRUE;
    GoToColumn( scol + 1, CurrentLine->len );
    EditFlags.InsertModeActive = FALSE;
    DisplayWorkLine( TRUE );
    UnselectRegion();
    DCUpdate();
#ifndef __WIN__
    HiliteAColumnRange( CurrentPos.line, scol, ecol );
#endif

    /*
     * now, get ready to do change
     */
    key = GetNextEvent( FALSE );
#ifdef __WIN__
    WorkLine->data[ecol] = tmp;
#else
    WorkLine->data[vecol] = tmp;
#endif
    DisplayWorkLine( TRUE );
    if( key == VI_KEY( ESC ) && !EditFlags.ChangeLikeVI ) {
        WorkLine->len = -1;
        GoToColumn( scol + 1, CurrentLine->len );
    } else {
        KeyAdd( key );
        rc = DeleteAndInsertText( scol, ecol );
    }
    return( rc );

} /* Change */

/*
 * doPush - shove/suck tab spaces in text
 */
static vi_rc doPush( range *r, bool shove )
{
    vi_rc       rc;

    /*
     * get the line range
     */
    if( r->start.line == r->end.line && !r->line_based ) {
        rc = ERR_INVALID_LINE_RANGE;
    } else {
        rc = Shift( r->start.line, r->end.line, shove ? '>' : '<', TRUE );
        if( rc <= ERR_NO_ERR ) {
#if 0
            GoToLineNoRelCurs( r->start.line );
#endif
            GoToColumnOnCurrentLine( FindStartOfCurrentLine() );
            EditFlags.Dotable = TRUE;
        }
    }
    return( rc );

} /* doPush */

vi_rc StartShove( range *r )
{
    UpdateCurrentStatus( CSTATUS_SHIFT_RIGHT );
    return( doPush( r, TRUE ) );

} /* StartShove */

vi_rc StartSuck( range *r )
{
    UpdateCurrentStatus( CSTATUS_SHIFT_LEFT );
    return( doPush( r, FALSE ) );

} /* StartSuck */

/*
 * changeOneLine: change the case for each character on the given line
 * from the starting column given by start_col (base 0) to the column
 * denoted by end_col (base 0).
 */
static vi_rc changeOneLine( linenum line_num, int start_col, int end_col )
{
    line        *line;
    fcb         *fcb;
    int         num_cols, i;
    char        *s;
    vi_rc       rc;

    rc = CGimmeLinePtr( line_num, &fcb, &line );
    if( rc == ERR_NO_ERR ) {
        assert( end_col < line->len && end_col >= start_col );
        num_cols = end_col - start_col + 1;
        s = &line->data[start_col];
        for( i = 0; i < num_cols; i++ ) {
            if( isupper( *s ) ) {
                *s = tolower( *s );
            } else {
                *s = toupper( *s );
            }
            s++;
        }
    }
    return( rc );
}

static vi_rc changeToEndOfLine( linenum line, int start )
{
    int         len;
    vi_rc       rc;

    rc = ERR_NO_ERR;
    len = LineLength( line );
    if( len ) {
        rc = changeOneLine( line, start, len - 1 );
    }
    return( rc );
}

vi_rc ChangeCase( range *r )
{
    linenum     curr;
    vi_rc       rc;
    long        total;
    char        *msg;

    UndoReplaceLines( r->start.line, r->end.line );
    if( r->start.line == r->end.line ) {
        rc = changeOneLine( r->start.line, r->start.column, r->end.column );
        msg = MSG_CHARACTERS;
        total = r->end.column - r->start.column + 1;
    } else {
        rc = changeToEndOfLine( r->start.line, r->start.column );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
        for( curr = r->start.line + 1; curr < r->end.line; curr++ ) {
            rc = changeToEndOfLine( curr, 0 );
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
        }
        rc = changeOneLine( r->end.line, 0, r->end.column );
        msg = MSG_LINES;
        total = r->end.line - r->start.line + 1;
    }
    EditFlags.Dotable = TRUE;
    DCDisplayAllLines();
    Message1( "case toggled for %l %s", total, msg );
    Modified( TRUE );
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* ChangeCase */

vi_rc Filter( range *r )
{
    vi_rc       rc;
    char        cmd[MAX_STR];

    rc = PromptForString( "Command: ", cmd, sizeof( cmd ), &FilterHist );
    if( rc == ERR_NO_ERR ) {
        rc = DoGenericFilter( r->start.line, r->end.line, cmd );
    } else {
        if( rc == NO_VALUE_ENTERED ) {
            rc = ERR_NO_ERR;
        }
    }
    return( rc );

} /* Filter */
