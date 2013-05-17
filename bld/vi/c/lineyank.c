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
 * YankLineRange - yank a specified line range in current file
 */
vi_rc YankLineRange( linenum s, linenum e )
{
    vi_rc       rc;
    fcb_list    fcblist;

    rc = GetCopyOfLineRange( s, e, &fcblist );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    AddFcbsToSavebuf( &fcblist, FALSE );
    LineYankMessage( s, e );
    return( ERR_NO_ERR );

} /* YankLineRange */

/*
 * GetCopyOfLineRange - yank a specified line range in current file
 */
vi_rc GetCopyOfLineRange( linenum s, linenum e, fcb_list *fcblist )
{
    file        *cfile;
    linenum     ll;
    fcb         *sfcb, *efcb;
    vi_rc       rc;
    vi_rc       rc1;
    vi_rc       rc2;

    /*
     * check line range
     */
    if( EditFlags.Modeless == FALSE ) {
        UnselectRegion();
    }
    if( s < 1 ) {
        return( ERR_NO_SUCH_LINE );
    }
    if( s > e ) {
        ll = s;
        s = e;
        e = ll;
    }

    /*
     * find start and ending fcbs
     */
    rc = FindFcbWithLine( s, CurrentFile, &sfcb );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    rc = FindFcbWithLine( e + 1, CurrentFile, &efcb );
    if( rc != ERR_NO_ERR ) {
        if( rc != ERR_NO_SUCH_LINE ) {
            return( rc );
        }
        if( IsPastLastLine( e ) ) {
            return( ERR_NO_SUCH_LINE );
        }
        rc = FindFcbWithLine( e, CurrentFile, &efcb );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }

    /*
     * duplicate this list
     */
    fcblist->tail = efcb->next;
    efcb->next = NULL;
    cfile = FileAlloc( NULL );
    cfile->fcbs.head = NULL;
    cfile->fcbs.tail = NULL;
    CreateDuplicateFcbList( sfcb, &cfile->fcbs );
    efcb->next = fcblist->tail;

    /*
     * split head and tail fcbs
     */
    rc1 = SplitFcbAtLine( s, cfile, cfile->fcbs.head );
    if( rc1 > 0 ) {
        return( rc1 );
    }
    rc = FindFcbWithLine( e + 1, cfile, &efcb );
    if( rc != ERR_NO_ERR ) {
        if( rc != ERR_NO_SUCH_LINE )  {
            return( rc );
        }
        rc = FindFcbWithLine( e, cfile, &efcb );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }
    rc2 = SplitFcbAtLine( e + 1, cfile, efcb );
    if( rc2 > 0 ) {
        return( rc2 );
    }

    /*
     * select appropriate part of split fcbs
     */
    if( rc1 == NO_SPLIT_CREATED_AT_START_LINE ) {
        fcblist->head = cfile->fcbs.head;
    } else {
        fcblist->head = cfile->fcbs.head->next;
        FreeEntireFcb( cfile->fcbs.head );
    }
    if( rc2 != NO_SPLIT_CREATED_AT_START_LINE ) {
        fcblist->tail = efcb;
    } else {
        fcblist->tail = efcb->prev;
        FreeEntireFcb( efcb );
        fcblist->tail->next = NULL;
    }

    /*
     * tidy up and return
     */
    if( fcblist->tail->next != NULL ) {
        FreeEntireFcb( fcblist->tail->next );
    }
    fcblist->head->prev = fcblist->tail->next = NULL;
    for( sfcb = fcblist->head; sfcb != NULL; sfcb = sfcb->next ) {
        sfcb->f = NULL;
    }
    FileFree( cfile );
    return( ERR_NO_ERR );

} /* GetCopyOfLineRange */

/*
 * LineYankMessage - display line deleted message
 */
void LineYankMessage( linenum s, linenum e )
{
#ifdef __WIN__
    if( LastSavebuf == 0 ) {
        Message1( "%l lines copied into the clipboard", e - s + 1 );
    } else {
#endif
        Message1( "%l lines yanked into buffer %c", e - s + 1, LastSavebuf );
#ifdef __WIN__
    }
#endif

} /* LineYankMessage */
