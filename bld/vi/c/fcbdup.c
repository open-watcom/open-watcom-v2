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
 * duplicateFcb - do just that
 */
static void duplicateFcb( fcb *cfcb, fcb **dfcb )
{
    line        *cline, *nline;

    /*
     * get fcb and create a new one
     */
    FetchFcb( cfcb );
    cfcb->non_swappable = true;
    *dfcb = FcbAlloc( NULL );
    (*dfcb)->start_line = cfcb->start_line;
    (*dfcb)->end_line = cfcb->end_line;
    (*dfcb)->byte_cnt = cfcb->byte_cnt;
    (*dfcb)->lines.head = (*dfcb)->lines.tail = NULL;

    /*
     * copy all lines
     */
    for( cline = cfcb->lines.head; cline != NULL; cline = cline->next ) {
        nline = LineAlloc( cline->data, cline->len );
        AddLLItemAtEnd( (ss **)&((*dfcb)->lines.head), (ss **)&((*dfcb)->lines.tail),
            (ss *)nline );
    }

    cfcb->non_swappable = false;
    (*dfcb)->non_swappable = false;

} /* duplicateFcb */

/*
 * CreateDuplicateFcbList - duplicate a list of fcb's
 */
void CreateDuplicateFcbList( fcb *cfcb, fcb_list *fcblist )
{
    fcb     *xfcb;

    for( ; cfcb != NULL; cfcb = cfcb->next ) {
        duplicateFcb( cfcb, &xfcb );
        AddLLItemAtEnd( (ss **)&fcblist->head, (ss **)&fcblist->tail, (ss *)xfcb );
    }

} /* CreateDuplicateFcbList */
