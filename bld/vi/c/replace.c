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

/*
 * ReplaceChar - replace an individual character
 */
vi_rc ReplaceChar( void )
{
    int         start, end, i, ai;
    char        *buff;
    bool        redrawAll;
    vi_rc       rc;
    vi_key      key;

    rc = ModificationTest();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    UpdateCurrentStatus( CSTATUS_REPLACECHAR );
    key = GetNextEvent( false );
    if( key == VI_KEY( ESC ) ) {
        return( ERR_NO_ERR );
    }
    if( CurrentLine->len == 0 ) {
        return( ERR_NO_CHAR_TO_REPLACE );
    }
    CurrentLineReplaceUndoStart();
    CurrentLineReplaceUndoEnd( key != VI_KEY( ENTER ) );

    if( key == VI_KEY( ENTER ) ) {

        buff = StaticAlloc();
        GetAutoIndentAmount( buff, 0, false );
        CurrentLine->data[CurrentPos.column - 1] = (char) 1;
        SaveCurrentFilePos();
        SplitUpLine( CurrentPos.line );
        RestoreCurrentFilePos();
        EndUndoGroup( UndoStack );
        GoToLineNoRelCurs( CurrentPos.line + 1 );
        GoToColumnOnCurrentLine( 1 );
        if( EditFlags.AutoIndent ) {
            ai = strlen( buff );
            if( ai + CurrentLine->len < EditVars.MaxLine ) {
                GetCurrentLine();
                i = 0;
                while( isspace( WorkLine->data[i] ) ) {
                    i++;
                }
                strcat( buff, &(WorkLine->data[i]) );
                i = strlen( buff );
                memcpy( WorkLine->data, buff, i + 1 );
                WorkLine->len = i;
                ReplaceCurrentLine();
            }
            rc = GoToColumn( ai + 1, CurrentLine->len );
            if( rc == ERR_NO_SUCH_COLUMN ) {
                GoToColumnOK( ai );
            }
        }
        StaticFree( buff );
        DCDisplayAllLines();

    } else {
        if( key == VI_KEY( TAB ) ) {
            key = '\t';
        }
        GetCurrentLine();
        start = CurrentPos.column - 1;
        end = start + (int) GetRepeatCount();
        if( end > WorkLine->len ) {
            end = WorkLine->len;
        }
        redrawAll = false;
        for( i = start; i < end; i++ ) {
            if( !redrawAll && SSKillsFlags( WorkLine->data[i] ) ) {
                redrawAll = true;
            }
            WorkLine->data[i] = (char) key;
        }
        redrawAll |= SSKillsFlags( key );
        DisplayWorkLine( redrawAll );
        ReplaceCurrentLine();
        GoToColumnOK( CurrentPos.column );

    }

    EditFlags.Dotable = true;
    return( ERR_NO_ERR );

} /* ReplaceChar */
