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

/*
 * CMergeFcbs - merge two fcbs, removing the second one
 */
int CMergeFcbs( fcb *fcb1, fcb *fcb2 )
{
    int i;

    i = JoinFcbs( fcb1, fcb2 );
    if( i ) {
        return( i );
    }
    DeleteLLItem( &(CurrentFile->fcb_head),&(CurrentFile->fcb_tail), fcb2 );

    if( fcb2->globalmatch ) {
        fcb1->globalmatch = TRUE;
    }
    FcbFree( fcb2 );

    return( ERR_NO_ERR );

} /* CMergeFcbs */

/*
 * CMergeAllFcbs - try merge process with all fcbs
 */
int CMergeAllFcbs( void )
{
    fcb *cfcb;
    int i;

    cfcb = CurrentFile->fcb_head;
    while( cfcb != NULL ) {
        if( cfcb->next == NULL ) {
            break;
        }
        if( !cfcb->in_memory || !cfcb->next->in_memory ) {
            cfcb=cfcb->next;
            continue;
        }
        i = CMergeFcbs( cfcb, cfcb->next );
        if( i ) {
            if( i == COULD_NOT_MERGE_FCBS ) {
                cfcb = cfcb->next;
            }
            else return( i );
        }
    }

    return( ERR_NO_ERR );

} /* CMergeAllFcbs */

/*
 * JoinFcbs - join two fcbs
 */
int JoinFcbs( fcb *fcb1, fcb *fcb2 )
{
    unsigned    j,k;

    /*
     * see if we can merge them
     */
    if( fcb1->end_line != (fcb2->start_line -1) ) {
        return( COULD_NOT_MERGE_FCBS );
    }
    j = FcbSize( fcb1 );
    k = FcbSize( fcb2 );
    if( j+k > (unsigned) MAX_IO_BUFFER ) {
        return( COULD_NOT_MERGE_FCBS );
    }

    /*
     * get fcb's if swapped
     */
    FetchFcb( fcb1 );
    fcb1->non_swappable = TRUE;
    FetchFcb( fcb2 );
    fcb1->non_swappable = FALSE;

    /*
     * update byte count and line numbers
     */
    fcb1->byte_cnt += fcb2->byte_cnt;
    fcb1->end_line = fcb2->end_line;

    /*
     * merge the two sets of lines
     */
    fcb1->line_tail->next = fcb2->line_head;
    fcb2->line_head->prev = fcb1->line_tail;
    fcb1->line_tail = fcb2->line_tail;
    return( ERR_NO_ERR );

} /* JoinFcbs */
