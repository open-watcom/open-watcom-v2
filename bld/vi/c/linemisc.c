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
#include "source.h"

/*
 * FindStartOfALine - do just that
 */
int FindStartOfALine( line *cline )
{
    int i = 0;

    while( isspace( cline->data[i] ) ) {
        i++;
    }
    if( cline->data[i] == 0 ) {
        return( 1 );
    }
    return( i + 1 );

} /* FindStartOfALine */

int FindStartOfCurrentLine( void )
{
    return( FindStartOfALine( CurrentLine ) );
}

/*
 * GenericJoinCurrentLineToNext
 */
vi_rc GenericJoinCurrentLineToNext( bool remsp )
{
    line        *nline = CurrentLine;
    fcb         *nfcb = CurrentFcb;
    int         i, j, k;
    vi_rc       rc;

    /*
     * get next line data
     */
    rc = CGimmeNextLinePtr( &nfcb, &nline );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    if( CurrentLine->len + nline->len + 1 >= MaxLine ) {
        return( ERR_LINE_FULL );
    }

    /*
     * now, copy in the data
     */
    StartUndoGroup( UndoStack );
    CurrentLineReplaceUndoStart();
    CurrentLineReplaceUndoEnd( TRUE );
    GetCurrentLine();

    if( remsp ) {
        while( WorkLine->len > 0 && WorkLine->data[WorkLine->len - 1] == ' ' ) {
            WorkLine->data[WorkLine->len - 1] = 0;
            WorkLine->len--;
        }
        j = FindStartOfALine( nline ) - 1;
        if( !(j == 0 && nline->data[0] == ' ') ) {
            if( WorkLine->len != 0 ) {
                WorkLine->data[WorkLine->len] = ' ';
                k = WorkLine->len + 1;
            } else {
                k = 0;
            }
            for( i = j; i <= nline->len; i++ ) {
                WorkLine->data[k + i - j] = nline->data[i];
            }
        }
    } else {
        k = WorkLine->len;
        for( i = 0; i <= nline->len; i++ ) {
            WorkLine->data[k + i] = nline->data[i];
        }
    }
    WorkLine->len = strlen( WorkLine->data );
    ReplaceCurrentLine();

    /*
     * delete next line
     */
    rc = DeleteLineRange( CurrentPos.line + 1, CurrentPos.line + 1, 0 );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    EndUndoGroup( UndoStack );
    if( remsp ) {
        if( k < 2 ) {
            k = 2;
        }
        rc = GoToColumn( k - 1, CurrentLine->len );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }
    DCDisplayAllLines();
    return( ERR_NO_ERR );

} /* GenericJoinCurrentLineToNext */

/*
 * JoinCurrentLineToNext
 */
vi_rc JoinCurrentLineToNext( void )
{
    int     i, j;
    vi_rc   rc;

    if( rc = ModificationTest() ) {
        return( rc );
    }
    i = (int) GetRepeatCount();
    StartUndoGroup( UndoStack );
    for( j = 0; j < i; j++ ) {
        rc = GenericJoinCurrentLineToNext( TRUE );
        if( rc != ERR_NO_ERR ) {
            break;
        }
    }
    EndUndoGroup( UndoStack );
    return( rc );

} /* JoinCurrentLineToNext */

#define MAX_FILE_STACK  5
static file     *oldFile[MAX_FILE_STACK];
static i_mark   oldCurrentPos[MAX_FILE_STACK];
static i_mark   oldLeftTopPos[MAX_FILE_STACK];
static int      stackDepth = -1;

/*
 * SaveCurrentFilePos
 */
void SaveCurrentFilePos( void )
{
    stackDepth++;
    oldFile[stackDepth] = CurrentFile;
    oldCurrentPos[stackDepth] = CurrentPos;
    oldLeftTopPos[stackDepth] = LeftTopPos;
}

/*
 * RestoreCurrentFilePos
 */
void RestoreCurrentFilePos( void )
{
    vi_rc   rc;

    CurrentFile = oldFile[stackDepth];
    CurrentPos = oldCurrentPos[stackDepth];
    LeftTopPos = oldLeftTopPos[stackDepth];

    if( CurrentFile != NULL ) {
        rc = CGimmeLinePtr( CurrentPos.line, &CurrentFcb, &CurrentLine );
        if( rc == ERR_NO_SUCH_LINE ) {
            if( CurrentFile->fcbs.tail != NULL ) {
                CurrentPos.line = CurrentFile->fcbs.tail->end_line;
                CGimmeLinePtr( CurrentPos.line, &CurrentFcb, &CurrentLine );
            }
        }
    } else {
        CurrentFcb = NULL;
        CurrentLine = NULL;
    }

    ValidateCurrentColumn();
    VarAddRandC();
    stackDepth--;

} /* RestoreCurrentFilePos */

/*
 * SaveAndResetFilePos - as it sounds
 */
vi_rc SaveAndResetFilePos( linenum n1 )
{
    vi_rc   rc;

    SaveCurrentFilePos();
    SetCurrentLineNumber( n1 );
    rc = CGimmeLinePtr( n1, &CurrentFcb, &CurrentLine );
    if( rc != ERR_NO_ERR ) {
        RestoreCurrentFilePos();
    }
    return( rc );

} /* SaveAndResetFilePos */
