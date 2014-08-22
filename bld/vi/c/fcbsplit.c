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
 * SplitFcbAtLine - split a fcb at specified line (specified line goes in
 *                  new one) - line 1 causes new fcb to be created at start
 */
vi_rc SplitFcbAtLine( linenum lne, file *f, fcb *fb )
{
    linenum     sline;
    int         bytecnt = 0;
    line        *cl, *pl;
    fcb         *cfcb;

    /*
     * if at start line or end line + 1, no splitting possible
     */
    if( lne == fb->start_line ) {
        return( NO_SPLIT_CREATED_AT_START_LINE );
    }
    if( lne == fb->end_line + 1 ) {
        return( NO_SPLIT_CREATED_AT_END_LINE );
    }

    /*
     * check if we tried to split at line that is not in fcb;
     */
    if( lne > fb->end_line ) {
        return( ERR_NO_SUCH_LINE );
    }

    /*
     * get fcb to split, and make sure that it isn't swapped while
     * we use it
     */
    FetchFcb( fb );
    fb->non_swappable = true;

    /*
     * get position
     */
    cl = fb->lines.head;
    for( sline = fb->start_line; sline != lne; sline++ ) {
        bytecnt += cl->len + 1;
        cl = cl->next;
    }

    /*
     * add the new fcb
     */
    pl = cl->prev;
    cfcb = FcbAlloc( f );
    InsertLLItemAfter( (ss **)&(f->fcbs.tail), (ss *)fb, (ss *)cfcb );

    /*
     * reset line data for new fcb
     */
    cfcb->start_line = lne;
    cfcb->end_line = fb->end_line;
    cfcb->lines.head = cl;
    cfcb->lines.head->prev = NULL;
    cfcb->lines.tail = fb->lines.tail;
    cfcb->byte_cnt = fb->byte_cnt - bytecnt;

    /*
     * reset line data for original fcb
     */
    fb->end_line = lne - 1;
    fb->lines.tail = pl;
    fb->lines.tail->next = NULL;
    fb->byte_cnt = bytecnt;

    /*
     * check for locked fcb
     */
    if( fb->globalmatch ) {
        /*
         * make sure original one should stay locked
         */
        fb->globalmatch = false;
        for( cl = fb->lines.head; cl != NULL; cl = cl->next ) {
            if( cl->u.ld.globmatch ) {
                fb->globalmatch = true;
                break;
            }
        }
        /*
         * see if new one needs to be locked
         */
        for( cl = cfcb->lines.head; cl != NULL; cl = cl->next ) {
            if( cl->u.ld.globmatch ) {
                cfcb->globalmatch = true;
                break;
            }
        }
    }

    /*
     * release fcbs
     */
    fb->non_swappable = false;
    cfcb->non_swappable = false;
    return( ERR_NO_ERR );

} /* SplitFcbAtLine */

/*
 * CheckCurrentFcbCapacity - check if fcb has exceeded its capacity; if so,
 *                           split it
 */
vi_rc CheckCurrentFcbCapacity( void )
{
    int         bc, bl;
    line        *cl;
    linenum     l;
    vi_rc       rc;

    /*
     * check if fcb is full
     */
    if( FcbSize( CurrentFcb ) <= MAX_IO_BUFFER ) {
        return( ERR_NO_ERR );
    }
    FetchFcb( CurrentFcb );

    /*
     * can't take it, so split it
     */
    cl = CurrentFcb->lines.head;
    bl = CurrentFcb->byte_cnt / 2;
    l = CurrentFcb->start_line;
    for( bc = cl->len + 1; bc < bl; bc += cl->len + 1 ) {
        cl = cl->next;
        l++;
    }
    rc = SplitFcbAtLine( l, CurrentFile, CurrentFcb );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    /*
     * check if current line is in new fcb, if so, switch to new fcb;
     * as well, new fcb had better have the same display status as the old
     */
    CurrentFcb->next->on_display = CurrentFcb->on_display;
    if( CurrentPos.line > CurrentFcb->end_line ) {
        CurrentFcb = CurrentFcb->next;
        FetchFcb( CurrentFcb );
    }
    return( ERR_NO_ERR );

} /* CheckCurrentFcbCapacity */
