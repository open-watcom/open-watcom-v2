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
#include <assert.h>
#include "vi.h"


/*
 * InsertLinesAtCursor - insert a set of lines at current pos. in file
 */
int InsertLinesAtCursor( fcb *fcbhead, fcb *fcbtail, undo_stack *us )
{
    fcb         *cfcb;
    linenum     e;
    int         i, lastLineLen;
    char        *source;
    line        *tLine;

    if( i = ModificationTest() ) {
        return( i );
    }

    /*
     * find the number of lines inserted
     */
    cfcb = fcbhead;
    e = 0;
    while( cfcb != NULL ) {
        e += (cfcb->end_line - cfcb->start_line + 1 );
        cfcb = cfcb->next;
    }

    // add chars from right of cursor to end of last line of buffer
    source = CurrentLine->data + CurrentColumn - 1;
    lastLineLen = fcbtail->line_tail->len;
    fcbtail->line_tail->len += CurrentLine->len - CurrentColumn + 1;

    fcbtail->byte_cnt += CurrentLine->len - CurrentColumn + 1;

    tLine = fcbtail->line_tail->prev;
    fcbtail->line_tail = MemReAlloc( fcbtail->line_tail, LINE_SIZE +
                                     fcbtail->line_tail->len + 1 );
    if( tLine ){
        tLine->next = fcbtail->line_tail;
    }
    strcpy( fcbtail->line_tail->data + lastLineLen, source );

    StartUndoGroup( us );

    // create new current line in work line
    CurrentLineReplaceUndoStart();
    GetCurrentLine();
    WorkLine->len = CurrentColumn + fcbhead->line_head->len - 1;
    strcpy( WorkLine->data + CurrentColumn - 1, fcbhead->line_head->data );

    // replace current line
    ReplaceCurrentLine();
    CurrentLineReplaceUndoEnd( TRUE );

    // remove first line of buffer
    FetchFcb( fcbhead );
    fcbhead->non_swappable = TRUE;
    fcbhead->start_line++;
    fcbhead->byte_cnt -= ( fcbhead->line_head->len + 1 );
    tLine = fcbhead->line_head;
    fcbhead->line_head = fcbhead->line_head->next;
    fcbhead->line_head->prev = NULL;
    MemFree( tLine );
    fcbhead->non_swappable = FALSE;

    // add rest of lines of buffer & done
    if( fcbhead->line_head) {
        InsertLines( CurrentLineNumber, fcbhead, fcbtail, us );
    }
    EndUndoGroup( us );

    // if are indeed linebased, move cursor as well
    if( !EditFlags.LineBased ) {
        GoToLineNoRelCurs( CurrentLineNumber + e - 1 );
        GoToColumnOnCurrentLine( lastLineLen + 1 );
    }

    return( ERR_NO_ERR );
} /* InsertLinesAtCursor */

/*
 * InsertLines - insert a set of lines after specified number in current file
 */
int InsertLines( linenum s, fcb *fcbhead, fcb *fcbtail, undo_stack *us )
{
    int         i;
    fcb         *sfcb,*cfcb;
    linenum     l,e;

    if( i = ModificationTest() ) {
        return( i );
    }
    if( s < 0 ) {
        return( ERR_NO_SUCH_LINE );
    }

    /*
     * find the number of lines inserted
     */
    cfcb = fcbhead;
    e = 0;
    while( cfcb != NULL ) {
        e += (cfcb->end_line - cfcb->start_line + 1 );
        cfcb = cfcb->next;
    }
    e += s;

    /*
     * see if there is a null fcb at the head; if so, ditch
     * the null fcb and then reset line ranges
     */
    if( CurrentFile->fcb_head->nullfcb ) {
        FreeEntireFcb( CurrentFile->fcb_head );
        CurrentFile->fcb_head = fcbhead;
        CurrentFile->fcb_tail = fcbtail;
        e = e - s;
        s = 0;
    /*
     * if we are to insert after line 0, then make this block of
     * fcbs the first set in the text
     */
    } else if( s == 0 ) {
        fcbtail->next = CurrentFile->fcb_head;
        CurrentFile->fcb_head->prev = fcbtail;
        CurrentFile->fcb_head = fcbhead;
    } else {
        /*
         * if we are inserting after the last fcb in the file,
         * make this block of fcbs the last set in the text
         */
        i = FindFcbWithLine( s+1, CurrentFile, &sfcb );
        if( i ) {
            if( i != ERR_NO_SUCH_LINE ) {
                return( i );
            }
            fcbhead->prev = CurrentFile->fcb_tail;
            CurrentFile->fcb_tail->next = fcbhead;
            CurrentFile->fcb_tail = fcbtail;
        /*
         * put the lines after the line to be have text inserted
         * into a new fcb, then insert the block of fcb's after
         * the fcb containing the line to have text inserted
         */
        } else {
            i = SplitFcbAtLine( s+1, CurrentFile, sfcb );
            if( i > 0 ) {
                return( i );
            }
            /*
             * line we want to split at is already the first line
             * in the fcb, so chain the new fcb block before
             * the fcb containg the line we want to split at
             */
            if( i == NO_SPLIT_CREATED_AT_START_LINE ) {
                if( sfcb->prev != NULL ) {
                    sfcb->prev->next = fcbhead;
                }
                fcbhead->prev = sfcb->prev;
                sfcb->prev = fcbtail;
                fcbtail->next = sfcb;
            /*
             * chain the new fcb block after the fcb that used
             * to containing the line we wanted to split at, and
             * before the new fcb containing the line we want
             * to split at
             */
            } else {
                if( i == NO_SPLIT_CREATED_AT_END_LINE ) {
//                  Die( "Impossible, can't be past last line");
                }
                if( sfcb->next != NULL ) {
                    sfcb->next->prev = fcbtail;
                }
                fcbtail->next = sfcb->next;
                sfcb->next = fcbhead;
                fcbhead->prev = sfcb;
            }
        }
    }

    /*
     * now, resequence line numbers and set proper file ptr
     */
    cfcb = fcbhead;
    while( cfcb != NULL ) {

        cfcb->f = CurrentFile;
        l = cfcb->end_line - cfcb->start_line;
        if( cfcb->prev != NULL ) {
            cfcb->start_line = cfcb->prev->end_line + 1;
        } else {
            cfcb->start_line = 1;
        }
        cfcb->end_line = cfcb->start_line + l;
        cfcb = cfcb->next;

    }

    /*
     * finish up: collect fcbs, point to corrent line, and
     * build undo for operation
     */
    i = CMergeAllFcbs();
    if( i ) {
        return( i );
    }
    StartUndoGroup( us );
    i = ValidateCurrentLine();
    if( i ) {
        return( i );
    }
    Modified( TRUE );
    UndoInsert( s+1,e, us );
    EndUndoGroup( us );

    return( ERR_NO_ERR );

} /* InsertLines */
