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
 * InsertLinesAtCursor - insert a set of lines at current pos. in file
 */
vi_rc InsertLinesAtCursor( fcb_list *fcblist, undo_stack *us )
{
    fcb         *cfcb;
    linenum     e;
    int         lastLineLen;
    char        *source;
    line        *tLine;
    vi_rc       rc;

    rc = ModificationTest();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    /*
     * find the number of lines inserted
     */
    e = 0;
    for( cfcb = fcblist->head; cfcb != NULL; cfcb = cfcb->next ) {
        e += (cfcb->end_line - cfcb->start_line + 1);
    }

    // add chars from right of cursor to end of last line of buffer
    source = CurrentLine->data + CurrentPos.column - 1;
    lastLineLen = fcblist->tail->lines.tail->len;
    fcblist->tail->lines.tail->len += CurrentLine->len - CurrentPos.column + 1;

    fcblist->tail->byte_cnt += CurrentLine->len - CurrentPos.column + 1;

    tLine = fcblist->tail->lines.tail->prev;
    fcblist->tail->lines.tail = MemReAlloc( fcblist->tail->lines.tail,
                    sizeof( line ) + fcblist->tail->lines.tail->len + 1 );
    if( tLine ){
        tLine->next = fcblist->tail->lines.tail;
    }
    strcpy( fcblist->tail->lines.tail->data + lastLineLen, source );

    StartUndoGroup( us );

    // create new current line in work line
    CurrentLineReplaceUndoStart();
    GetCurrentLine();
    WorkLine->len = CurrentPos.column + fcblist->head->lines.head->len - 1;
    strcpy( WorkLine->data + CurrentPos.column - 1, fcblist->head->lines.head->data );

    // replace current line
    ReplaceCurrentLine();
    CurrentLineReplaceUndoEnd( true );

    // remove first line of buffer
    FetchFcb( fcblist->head );
    fcblist->head->non_swappable = true;
    fcblist->head->start_line++;
    fcblist->head->byte_cnt -= fcblist->head->lines.head->len + 1;
    tLine = fcblist->head->lines.head;
    fcblist->head->lines.head = fcblist->head->lines.head->next;
    fcblist->head->lines.head->prev = NULL;
    MemFree( tLine );
    fcblist->head->non_swappable = false;

    // add rest of lines of buffer & done
    if( fcblist->head->lines.head) {
        InsertLines( CurrentPos.line, fcblist, us );
    }
    EndUndoGroup( us );

    // if are indeed linebased, move cursor as well
    if( !EditFlags.LineBased ) {
        GoToLineNoRelCurs( CurrentPos.line + e - 1 );
        GoToColumnOnCurrentLine( lastLineLen + 1 );
    }

    return( ERR_NO_ERR );

} /* InsertLinesAtCursor */

/*
 * InsertLines - insert a set of lines after specified number in current file
 */
vi_rc InsertLines( linenum s, fcb_list *fcblist, undo_stack *us )
{
    fcb         *sfcb, *cfcb;
    linenum     l, e;
    vi_rc       rc;

    rc = ModificationTest();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    if( s < 0 ) {
        return( ERR_NO_SUCH_LINE );
    }

    /*
     * find the number of lines inserted
     */
    e = 0;
    for( cfcb = fcblist->head; cfcb != NULL; cfcb = cfcb->next ) {
        e += cfcb->end_line - cfcb->start_line + 1;
    }
    e += s;

    /*
     * see if there is a null fcb at the head; if so, ditch
     * the null fcb and then reset line ranges
     */
    if( CurrentFile->fcbs.head->nullfcb ) {
        FreeEntireFcb( CurrentFile->fcbs.head );
        CurrentFile->fcbs = *fcblist;
        e = e - s;
        s = 0;
    /*
     * if we are to insert after line 0, then make this block of
     * fcbs the first set in the text
     */
    } else if( s == 0 ) {
        fcblist->tail->next = CurrentFile->fcbs.head;
        CurrentFile->fcbs.head->prev = fcblist->tail;
        CurrentFile->fcbs.head = fcblist->head;
    } else {
        /*
         * if we are inserting after the last fcb in the file,
         * make this block of fcbs the last set in the text
         */
        rc = FindFcbWithLine( s + 1, CurrentFile, &sfcb );
        if( rc != ERR_NO_ERR ) {
            if( rc != ERR_NO_SUCH_LINE ) {
                return( rc );
            }
            fcblist->head->prev = CurrentFile->fcbs.tail;
            CurrentFile->fcbs.tail->next = fcblist->head;
            CurrentFile->fcbs.tail = fcblist->tail;
        /*
         * put the lines after the line to be have text inserted
         * into a new fcb, then insert the block of fcb's after
         * the fcb containing the line to have text inserted
         */
        } else {
            rc = SplitFcbAtLine( s + 1, CurrentFile, sfcb );
            if( rc > ERR_NO_ERR ) {
                return( rc );
            }
            /*
             * line we want to split at is already the first line
             * in the fcb, so chain the new fcb block before
             * the fcb containg the line we want to split at
             */
            if( rc == NO_SPLIT_CREATED_AT_START_LINE ) {
                if( sfcb->prev != NULL ) {
                    sfcb->prev->next = fcblist->head;
                }
                fcblist->head->prev = sfcb->prev;
                sfcb->prev = fcblist->tail;
                fcblist->tail->next = sfcb;
            /*
             * chain the new fcb block after the fcb that used
             * to containing the line we wanted to split at, and
             * before the new fcb containing the line we want
             * to split at
             */
            } else {
                if( rc == NO_SPLIT_CREATED_AT_END_LINE ) {
//                  Die( "Impossible, can't be past last line");
                }
                if( sfcb->next != NULL ) {
                    sfcb->next->prev = fcblist->tail;
                }
                fcblist->tail->next = sfcb->next;
                sfcb->next = fcblist->head;
                fcblist->head->prev = sfcb;
            }
        }
    }

    /*
     * now, resequence line numbers and set proper file ptr
     */
    for( cfcb = fcblist->head; cfcb != NULL; cfcb = cfcb->next ) {
        cfcb->f = CurrentFile;
        l = cfcb->end_line - cfcb->start_line;
        if( cfcb->prev != NULL ) {
            cfcb->start_line = cfcb->prev->end_line + 1;
        } else {
            cfcb->start_line = 1;
        }
        cfcb->end_line = cfcb->start_line + l;
    }

    /*
     * finish up: collect fcbs, point to corrent line, and
     * build undo for operation
     */
    rc = MergeAllFcbs( &CurrentFile->fcbs );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    StartUndoGroup( us );
    rc = ValidateCurrentLine();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    Modified( true );
    UndoInsert( s + 1, e, us );
    EndUndoGroup( us );

    return( ERR_NO_ERR );

} /* InsertLines */
