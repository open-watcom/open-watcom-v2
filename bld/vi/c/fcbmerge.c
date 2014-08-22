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
 * JoinFcbs - join two fcbs
 */
static vi_rc JoinFcbs( fcb *fcb1, fcb *fcb2 )
{
    unsigned    j, k;

    /*
     * see if we can merge them
     */
    if( fcb1->end_line != (fcb2->start_line - 1) ) {
        return( COULD_NOT_MERGE_FCBS );
    }
    j = FcbSize( fcb1 );
    k = FcbSize( fcb2 );
    if( j + k > (unsigned) MAX_IO_BUFFER ) {
        return( COULD_NOT_MERGE_FCBS );
    }

    /*
     * get fcb's if swapped
     */
    FetchFcb( fcb1 );
    fcb1->non_swappable = true;
    FetchFcb( fcb2 );
    fcb1->non_swappable = false;

    /*
     * update byte count and line numbers
     */
    fcb1->byte_cnt += fcb2->byte_cnt;
    fcb1->end_line = fcb2->end_line;

    /*
     * merge the two sets of lines
     */
    fcb1->lines.tail->next = fcb2->lines.head;
    fcb2->lines.head->prev = fcb1->lines.tail;
    fcb1->lines.tail = fcb2->lines.tail;
    return( ERR_NO_ERR );

} /* JoinFcbs */

/*
 * MergeFcbs - merge two fcbs, removing the second one
 */
vi_rc MergeFcbs( fcb_list *fcblist, fcb *fcb1, fcb *fcb2 )
{
    vi_rc   rc;

    rc = JoinFcbs( fcb1, fcb2 );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    DeleteLLItem( (ss **)&(fcblist->head), (ss **)&(fcblist->tail), (ss *)fcb2 );
    if( fcb2->globalmatch ) {
        fcb1->globalmatch = true;
    }
    FcbFree( fcb2 );
    return( ERR_NO_ERR );

} /* MergeFcbs */

/*
 * MergeAllFcbs - try merge process with all fcbs
 */
vi_rc MergeAllFcbs( fcb_list *fcblist )
{
    fcb     *cfcb;
    fcb     *nfcb;

    for( cfcb = fcblist->head; cfcb != NULL && cfcb->next != NULL; cfcb = nfcb ) {
        nfcb = cfcb->next;
        if( cfcb->in_memory && nfcb->in_memory ) {
            if( MergeFcbs( fcblist, cfcb, nfcb ) == ERR_NO_ERR ) {
                nfcb = cfcb;
            }
        }
    }
    return( ERR_NO_ERR );

} /* MergeAllFcbs */
