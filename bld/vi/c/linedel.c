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
#include "vi.h"

/*
 * UpdateLineNumbers - modify the start and end lines in specifed fcb and
 *                     in all fcb's after
 */
void UpdateLineNumbers( linenum amt, fcb *cfcb  )
{
    while( cfcb != NULL ) {
        cfcb->start_line += amt;
        cfcb->end_line += amt;
        cfcb = cfcb->next;
    }

} /* UpdateLineNumbers */

/*
 * DeleteLineRange - delete a specified line range in current file
 */
int DeleteLineRange( linenum s, linenum e, linedel_flags flags )
{
    int         i,j,k;
    linenum     diff,ll;
    fcb         *s1fcb,*sfcb,*e1fcb,*efcb,*cfcb;
    undo_stack  *us;

    /*
     * check line range
     */
    UnselectRegion();
    if( i = ModificationTest() ) {
        return( i );
    }
    if( s > e ) {
        ll = s;
        s = e;
        e = ll;
    }
    if( s<1 ) {
        return( ERR_NO_SUCH_LINE );
    }
    i = CFindLastLine( &ll );
    if( i ) {
        return( i );
    }
    if( e > ll ) {
        return( ERR_NO_SUCH_LINE );
    }

    /*
     * split fcb with start
     */
    i=FindFcbWithLine( s, CurrentFile, &sfcb );
    if( i ) {
        return( i );
    }
    k = SplitFcbAtLine( s, CurrentFile, sfcb );
    if( k > 0 ) {
        return( k );
    }

    /*
     * split fcb with end line
     */
    i = FindFcbWithLine( e+1, CurrentFile, &efcb );
    if( i ) {
        if( i != ERR_NO_SUCH_LINE ) {
            return( i );
        }
        if( e > ll ) {
            return( ERR_NO_SUCH_LINE );
        }
        i = FindFcbWithLine( e, CurrentFile, &efcb );
        if( i ) {
            return( i );
        }
    }
    if( (j=SplitFcbAtLine( e+1, CurrentFile, efcb )) > 0 ) {
        return( j );
    }

    /*
     * get pointers to middle fcbs (will use these for save buffers
     * and undos).
     */
    if( k== NO_SPLIT_CREATED_AT_START_LINE ) {
        s1fcb = sfcb;
    } else {
        s1fcb = sfcb->next;
    }
    if( j != NO_SPLIT_CREATED_AT_START_LINE ) {
        e1fcb = efcb;
    } else {
        e1fcb = efcb->prev;
    }

    /*
     * point sfcb to the fcb containing the line before the first line
     * deleted, and point efcb to the fcb containing the line after the last
     * line deleted; then chain sfcb and efcb together
     */
    if( k == NO_SPLIT_CREATED_AT_START_LINE ) {
        sfcb = sfcb->prev;
    }
    if( j != NO_SPLIT_CREATED_AT_START_LINE ) {
        efcb = efcb->next;
    }
    if( sfcb != NULL ) {
        sfcb->next = efcb;
    }
    if( efcb != NULL ) {
        efcb->prev = sfcb;
    }
    diff = s-e-1;

    /*
     * when this happens, all data is gone and we need a new
     * null fcb
     */
    if(  sfcb == NULL && efcb == NULL ) {
        cfcb = FcbAlloc( CurrentFile );
        CreateNullLine( cfcb );
        cfcb->non_swappable = FALSE;
        CurrentFile->fcb_head = CurrentFile->fcb_tail = cfcb;
    /*
     * when this happens, we have lost the head elements, so
     * reset the head ptr and renumber
     */
    } else if( sfcb == NULL ) {
        CurrentFile->fcb_head = efcb;
        UpdateLineNumbers( diff, efcb );
    /*
     * when this happens, we have lost the tail elements,
     * so reset the tail ptr. no renumbering required
     */
    } else if( efcb == NULL ) {
        CurrentFile->fcb_tail = sfcb;
    /*
     * deleted somewhere inside, so update line numbers
     * from fcb at end of range on; then try to merge fcb at start
     * of deleted range to the fcb at the end of the deleted range
     */
    } else {
        UpdateLineNumbers( diff, efcb );
        i = CMergeFcbs( sfcb,efcb );
        if( i > 0 ) {
            return( i );
        }
    }

    /*
     * check if we need to duplicate these fcbs to a save buffer
     */
    if( (flags & SAVEBUF_FLAG) && !EditFlags.GlobalInProgress ) {
        AddFcbsToSavebuf( s1fcb, e1fcb, TRUE );
    }

    /*
     * finish up: fix up line number and build undo action
     */
    if( flags & USE_UNDO_UNDO ) {
        us = UndoUndoStack;
    } else {
        us = UndoStack;
    }
    StartUndoGroup( us );
    if( CurrentLineNumber >= s ) {
        if( CurrentLineNumber <= e ) {
            i = SetCurrentLine( s );
            if( i ) {
                if( i==ERR_NO_SUCH_LINE ) {
                    i = SetCurrentLine( s-1 );
                }
                if( i ) {
                    return( i );
                }
            }
        } else {
            i = SetCurrentLine( s+CurrentLineNumber-e-1 );
            if( i ) {
                return( i );
            }
        }
    }
    Modified( TRUE );
    s1fcb->prev = e1fcb->next = NULL;
    if( EditFlags.GlobalInProgress ) {
        for( cfcb=s1fcb;cfcb != NULL;cfcb=cfcb->next) {
            cfcb->globalmatch = FALSE;
        }
    }
    UndoDeleteFcbs( s-1, s1fcb, e1fcb, us );
    EndUndoGroup( us );
    PatchDeleteUndo( us );

    return( ERR_NO_ERR );

} /* DeleteLineRange */

/*
 * LineDeleteMessage - display line deleted message
 */
void LineDeleteMessage( linenum s, linenum e )
{

    if( EditFlags.GlobalInProgress ) {
        return;
    }
    #ifdef __WIN__
        if( LastSavebuf == 0 ) {
            Message1( "%l lines deleted into the clipboard",e-s+1 );
        } else
    #endif
    Message1( "%l lines%s%c",e-s+1, MSG_DELETEDINTOBUFFER, LastSavebuf );

} /* LineDeleteMessage */
