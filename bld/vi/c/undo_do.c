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

static char     usName[] = "undo stack";
static char     uusName[] = "undo-undo stack";

/*
 * validateUndo - make sure an undo has the correct number of open/closes
 */
static vi_rc validateUndo( undo *cundo )
{
    bool        done = false;
    int         depth = 0;

    /*
     * run through entries in this undo record, counting the
     * start/end groups.  if there are not an equal number, then
     * an error occurred while forming this undo and the whole
     * bloody stack is fried.
     */
    while( !done ) {
        switch( cundo->type ) {
        case START_UNDO_GROUP:
            depth--;
            if( depth == 0 ) {
                done = true;
            }
            break;

        case END_UNDO_GROUP:
            depth++;
            break;
        }
        cundo = cundo->next;
        if( cundo == NULL ) {
            break;
        }
    }

    if( !done ) {
        return( ERR_INVALID_UNDO );
    }
    return( ERR_NO_ERR );

} /* validateUndo */

/*
 * realUndo - perform an undo
 */
static vi_rc realUndo( undo_stack *stack, undo_stack *us )
{
    undo                *cundo, *tundo;
    bool                done = false;
    vi_rc               rc = ERR_NO_ERR;
    int                 col, depth = 0;
    linenum             lne, top;
    char                *name;
    linedel_flags       ldf;

    if( stack == NULL ) {
        return( ERR_NO_FILE );
    }

    if( stack->OpenUndo > 0 ) {
        return( ERR_OPEN_UNDO );
    }

    cundo = PopUndoStack( stack );
    if( cundo == NULL ) {
        if( stack == UndoUndoStack ) {
            return( ERR_NO_MORE_REDOS );
        }
        else {
            return( ERR_NO_MORE_UNDOS );
        }
    }
    if( validateUndo( cundo ) ) {
        PurgeUndoStack( UndoStack );
        PurgeUndoStack( UndoUndoStack );
        return( ERR_INVALID_UNDO );
    }
    tundo = cundo;
    StartUndoGroup( us );
    EditFlags.DisplayHold = true;

    ldf = 0;
    if( us == UndoUndoStack ) {
        ldf = USE_UNDO_UNDO;
    }

    /*
     * loop through all undos in this group
     */
    top = 0;
    lne = 0;
    col = 0;
    while( !done ) {
        switch( cundo->type ) {
        case START_UNDO_GROUP:
            depth--;
            if( depth == 0 ) {
                done = true;
            }
            if( cundo->data.sdata.depth == 1 ) {
                lne = cundo->data.sdata.p.line;
                top = cundo->data.sdata.top;
                col = cundo->data.sdata.p.column;
            }
            break;

        case END_UNDO_GROUP:
            depth++;
            break;

        case UNDO_INSERT_LINES:
            rc = DeleteLineRange( cundo->data.del_range.start,
                                  cundo->data.del_range.end, ldf );
            break;

        case UNDO_DELETE_FCBS:
            rc = InsertLines( cundo->data.fcbs.head->start_line,
                              &cundo->data.fcbs, us );
            break;
        }
        if( rc > ERR_NO_ERR ) {
            break;
        }
        cundo = cundo->next;
        if( cundo == NULL ) {
            break;
        }
    }
    if( !done ) {
        stack->OpenUndo = 0;
    }

    /*
     * finish up
     */
    EndUndoGroup( us );
    UndoFree( tundo, false );

    MergeAllFcbs( &CurrentFile->fcbs );
    EditFlags.DisplayHold = false;
    LeftTopPos.line = top;
    SetCurrentLineNumber( lne );
    CurrentPos.column = 1;
    CGimmeLinePtr( lne, &CurrentFcb, &CurrentLine );
    GoToColumnOK( col );
    UpdateStatusWindow();
    DCDisplayAllLines();
    if( rc == ERR_NO_ERR ) {
        if( stack == UndoStack ) {
            name = usName;
        } else {
            name = uusName;
        }
        if( stack->current < 0 ) {
            Message1( "%s is empty",name );
            if( stack == UndoStack ) {
                if( !stack->rolled ) {
                    Modified( false );
                }
            }
        } else {
            Message1( "%d items left on %s", stack->current + 1, name );
        }
        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
    }
    return( rc );

} /* realUndo */

/*
 * DoUndo - do an undo
 */
vi_rc DoUndo( void )
{
    return( realUndo( UndoStack, UndoUndoStack ) );

} /* DoUndo */

/*
 * DoUndoUndo - do an undo
 */
vi_rc DoUndoUndo( void )
{
    vi_rc   rc;

    EditFlags.UndoInProg = true;
    rc = realUndo( UndoUndoStack, UndoStack );
    EditFlags.UndoInProg = false;
    return( rc );

} /* DoUndoUndo */
