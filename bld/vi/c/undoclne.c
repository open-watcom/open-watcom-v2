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

static line     *lineSave;
static linenum  cLine, pageTop;
static int      cCol;

/*
 * CurrentLineReplaceUndoStart - set up undo for replacement of current line
 */
void CurrentLineReplaceUndoStart( void )
{

    if( !EditFlags.Undo || UndoStack == NULL ) {
        return;
    }
    cLine = CurrentPos.line;
    cCol = CurrentPos.column;
    pageTop = LeftTopPos.line;
    lineSave = LineAlloc( CurrentLine->data, CurrentLine->len );
    lineSave->u.ld.mark = CurrentLine->u.ld.mark;

} /* CurrentLineReplaceUndoStart */

/*
 * singleLineFcb - create fcb from lineSave
 */
static fcb *singleLineFcb( void )
{
    fcb *cfcb;

    cfcb = FcbAlloc( CurrentFile );
    if( cfcb == NULL ) {
        return( NULL );
    }
    cfcb->lines.head = cfcb->lines.tail = lineSave;
    cfcb->byte_cnt = lineSave->len + 1;
    return( cfcb );

} /* singleLineFcb */


/*
 * CurrentLineReplaceUndoCancel - cancel undoing of current line replacement
 */
void CurrentLineReplaceUndoCancel( void )
{
    if( !EditFlags.Undo || UndoStack == NULL ) {
        return;
    }
    MemFree( lineSave );

} /* CurrentLineReplaceUndoCancel */

/*
 * CurrentLineReplaceUndoEnd - actually add the undo
 */
void CurrentLineReplaceUndoEnd( bool endgrp )
{
    fcb         *cfcb, *nfcb;
    undo        *top, *delrec;
    fcb_list    fcblist;

    if( !EditFlags.Undo || UndoStack == NULL ) {
        return;
    }
    /*
     * see if we can merge this with the last undo record
     * (provided we are in an open undo group)
     *
     * we need the following undo sequence:
     * END_UNDO_GROUP
     * UNDO_INSERT_LINES
     *      - must have end line one less than the current line
     * UNDO_DELETE_FCBS
     *      - must have last line to insert being two less
     *        than the current (since then the undo for
     *        the current would be on the line ONE less than
     *        the current);
     */
    if( endgrp && UndoStack->OpenUndo > 0 ) {
        top = UndoStack->stack[UndoStack->current];
        if( top != NULL && top->type == END_UNDO_GROUP ) {
            top = top->next;
            if( top != NULL && top->type == UNDO_INSERT_LINES ) {
                if( top->data.del_range.end == CurrentPos.line - 1 ) {
                    delrec = top;
                    top = top->next;
                    if( top != NULL && top->type == UNDO_DELETE_FCBS ) {
                        cfcb = top->data.fcbs.tail;
                        if( cfcb->end_line == CurrentPos.line - 2 ) {
                            /*
                             * FINALLY, we can add it. either
                             * add to current fcb or add a new
                             * fcb
                             */
                            if( (FcbSize( cfcb ) + lineSave->len + 4) <=
                                MAX_IO_BUFFER ) {
                                FetchFcb( cfcb );
                                InsertLLItemAfter( (ss **)&cfcb->lines.tail,
                                    (ss *)cfcb->lines.tail, (ss *)lineSave );
                                cfcb->byte_cnt += lineSave->len + 1;
                                cfcb->end_line++;
                            } else {
                                nfcb = singleLineFcb();
                                nfcb->start_line = nfcb->end_line = cfcb->end_line + 1;
                                InsertLLItemAfter( (ss **)&(top->data.fcbs.tail),
                                    (ss *)cfcb, (ss *)nfcb );
                                nfcb->non_swappable = false;
                            }
                            delrec->data.del_range.end++;
                            Modified( true );
                            return;

                        }
                    }
                }
            }
        }
    }

    /*
     * create an fcb with a single line
     */
    cfcb = singleLineFcb();

    /*
     * build undo action
     */
    fcblist.head = cfcb;
    fcblist.tail = cfcb;
    StartUndoGroupWithPosition( UndoStack, cLine, pageTop, cCol );
    UndoDeleteFcbs( CurrentPos.line - 1, &fcblist, UndoStack );
    UndoInsert( CurrentPos.line, CurrentPos.line, UndoStack );
    if( endgrp ) {
        EndUndoGroup( UndoStack );
    }
    Modified( true );
    cfcb->non_swappable = false;

} /* CurrentLineReplaceUndoEnd */

/*
 * ConditionalCurrentLineReplaceUndoEnd - do the above if there are changes
 */
void ConditionalCurrentLineReplaceUndoEnd( void )
{
    if( lineSave->len == CurrentLine->len ) {
        if( !memcmp( lineSave->data, CurrentLine->data, lineSave->len ) ) {
            CurrentLineReplaceUndoCancel();
            return;
        }
    }
    CurrentLineReplaceUndoEnd( true );

} /* ConditionalCurrentLineReplaceUndoEnd */
