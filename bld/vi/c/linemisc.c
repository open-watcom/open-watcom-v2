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
#include <ctype.h>
#include <string.h>
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

int FindStartOfCurrentLine( void ) { return( FindStartOfALine( CurrentLine ) ); }

/*
 * GenericJoinCurrentLineToNext
 */
int GenericJoinCurrentLineToNext( bool remsp )
{
    line        *nline=CurrentLine;
    fcb         *nfcb=CurrentFcb;
    int         i,j,k;

    /*
     * get next line data
     */
    i = CGimmeNextLinePtr( &nfcb, &nline );
    if( i ) {
        return( i );
    }
    if( CurrentLine->len+nline->len +1 >= MaxLine ) {
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
        while( WorkLine->len > 0 && WorkLine->data[ WorkLine->len - 1 ] == ' ' ) {
            WorkLine->data[ WorkLine->len - 1 ] = 0;
            WorkLine->len--;
        }
        j = FindStartOfALine( nline )-1;
        if( !(j==0 && nline->data[0] == ' ') ) {
            if( WorkLine->len != 0 ) {
                WorkLine->data[WorkLine->len] = ' ';
                k = WorkLine->len+1;
            } else {
                k = 0;
            }
            for( i=j;i<=nline->len;i++ ) {
                WorkLine->data[k+i-j] = nline->data[i];
            }
        }
    } else {
        k = WorkLine->len;
        for( i=0;i<=nline->len;i++ ) {
            WorkLine->data[k+i] = nline->data[i];
        }
    }
    WorkLine->len = strlen( WorkLine->data );
    ReplaceCurrentLine();

    /*
     * delete next line
     */
    i = DeleteLineRange( CurrentLineNumber+1, CurrentLineNumber+1, 0 );
    if( i ) {
        return( i );
    }
    EndUndoGroup( UndoStack );
    if( remsp ) {
        if( k<2 ) {
            k = 2;
        }
        i = GoToColumn( k-1, CurrentLine->len );
        if( i ) {
            return( i );
        }
    }
    DCDisplayAllLines();
    return( ERR_NO_ERR );

} /* GenericJoinCurrentLineToNext */

/*
 * JoinCurrentLineToNext
 */
int JoinCurrentLineToNext( void )
{
    int rc,i,j;

    if( i = ModificationTest() ) {
        return( i );
    }
    i = (int) GetRepeatCount();
    StartUndoGroup( UndoStack );
    for( j=0;j<i;j++ ) {
        rc = GenericJoinCurrentLineToNext( TRUE );
        if( rc ) {
            break;
        }
    }
    EndUndoGroup( UndoStack );
    return( rc );

} /* JoinCurrentLineToNext */

#define MAX_FILE_STACK  5
static file *oldFile[MAX_FILE_STACK];
static linenum oldLineNo[MAX_FILE_STACK];
static linenum oldPageTop[MAX_FILE_STACK];
static int oldCol[MAX_FILE_STACK];
static int oldLeftCol[MAX_FILE_STACK];
static int stackDepth = -1;
/*
 * SaveCurrentFilePos
 */
void SaveCurrentFilePos( void )
{
    stackDepth++;
    oldFile[stackDepth] = CurrentFile;
    oldLineNo[stackDepth] = CurrentLineNumber;
    oldPageTop[stackDepth] = TopOfPage;
    oldCol[stackDepth] = CurrentColumn;
    oldLeftCol[stackDepth] = LeftColumn;

} /* SaveCurrentFilePos */

/*
 * RestoreCurrentFilePos
 */
void RestoreCurrentFilePos( void )
{
    int i;

    CurrentFile = oldFile[stackDepth];
    CurrentLineNumber = oldLineNo[stackDepth];
    TopOfPage = oldPageTop[stackDepth];

    if( CurrentFile != NULL ) {

        i = CGimmeLinePtr( CurrentLineNumber, &CurrentFcb, &CurrentLine );
        if(  i == ERR_NO_SUCH_LINE ) {
            if( CurrentFile->fcb_tail != NULL ) {
                CurrentLineNumber = CurrentFile->fcb_tail->end_line;
                CGimmeLinePtr( CurrentLineNumber, &CurrentFcb, &CurrentLine );
            }
        }
    } else {
        CurrentFcb = NULL;
        CurrentLine = NULL;
    }

    CurrentColumn = oldCol[stackDepth];
    LeftColumn = oldLeftCol[stackDepth];
    ValidateCurrentColumn();
    VarAddRandC();
    stackDepth--;

} /* RestoreCurrentFilePos */

/*
 * SaveAndResetFilePos - as it sounds
 */
int SaveAndResetFilePos( linenum n1 )
{
    int i;

    SaveCurrentFilePos();
    SetCurrentLineNumber( n1 );
    i = CGimmeLinePtr( n1, &CurrentFcb, &CurrentLine );
    if( i ) {
        RestoreCurrentFilePos();
    }
    return( i );

} /* SaveAndResetFilePos */
