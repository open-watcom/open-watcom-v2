/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
 * tempFileSetup - set up a temp file with data in it
 */
static void tempFileSetup( file **cfile, char *list[], list_linenum maxlist, size_t indent, bool makelower )
{
    list_linenum    j;
    size_t          i;
    size_t          k;
    size_t          boff;
    char            dd[FILENAME_MAX];

    /*
     * allocate temporary file structures
     */
    *cfile = FileAlloc( NULL );

    /*
     * copy data into buffer and create fcbs
     */
    boff = 0;
    for( j = 0; j < maxlist; ++j ) {
        strcpy( dd, list[j] );
        if( makelower ) {
            FileLower( dd );
        }
        k = strlen( dd );
        if( k + 2 + indent + boff > MAX_IO_BUFFER ) {
            CreateFcbData( *cfile, boff );
            (*cfile)->fcbs.tail->non_swappable = true;
            boff = 0;
        }
        if( indent ) {
            for( i = 0; i < indent; i++ ) {
                ReadBuffer[boff + i] = ' ';
            }
        }
        memcpy( &ReadBuffer[boff + indent], dd, k );
        ReadBuffer[boff + k + indent] = CR;
        ReadBuffer[boff + k + indent + 1] = LF;
        boff += k + indent + 2;
    }

    CreateFcbData( *cfile, boff );
    (*cfile)->fcbs.tail->non_swappable = true;

} /* tempFileSetup */

/*
 * SelectItem - select item to set from a menu
 */
vi_rc SelectItem( selectitem *si )
{
    list_linenum        j;
    file                *cfile;
    selflinedata        sfd;
    vi_rc               rc;

    tempFileSetup( &cfile, si->list, si->maxlist, 0, false );

    /*
     * get selected line
     */
    sfd.is_menu = si->is_menu;
    sfd.show_lineno = si->show_lineno;
    sfd.has_scroll_gadgets = false;
    sfd.f = cfile;
    sfd.vals = NULL;
    sfd.valoff = 0;
    sfd.wi = si->wi;
    sfd.sl = 0;
    sfd.title = si->title;
    sfd.checkres = NULL;
    sfd.allowrl = si->allowrl;
    sfd.hi_list = si->hi_list;
    sfd.show_lineno = si->show_lineno;
    sfd.retevents = si->retevents;
    sfd.event = si->event;
    sfd.cln = si->cln;
    sfd.event_wid = si->event_wid;
    rc = SelectLineInFile( &sfd );
    si->event = sfd.event;
    if( rc == ERR_NO_ERR ) {
        if( sfd.sl == -1 || sfd.sl == 0 ) {
            if( si->result != NULL ) {
                si->result[0] = '\0';
            }
            si->num = -1;
        } else {
            j = sfd.sl - 1;
            if( si->result != NULL ) {
                strcpy( si->result, si->list[j] );
            }
            si->num = j;
        }
    }

    /*
     * done, free memory
     */
    FreeEntireFile( cfile );
    DCDisplayAllLines();
    return( rc );

} /* SelectItem */

/*
 * SelectItemAndValue - select item from list and give it a value
 */
vi_rc SelectItemAndValue( window_info *wi, char *title, char **list,
                        list_linenum maxlist, vi_rc (*updatertn)( const char *, char *, int * ),
                        size_t indent, char **vals, int valoff )
{
//    list_linenum        j;
    file                *cfile;
    selflinedata        sfd;
    vi_rc               rc;

    tempFileSetup( &cfile, list, maxlist, indent, true );

    for( ;; ) {
        /*
         * go get selected line
         */
        sfd.is_menu = false;
        sfd.show_lineno = false;
        sfd.has_scroll_gadgets = false;
        sfd.f = cfile;
        sfd.vals = vals;
        sfd.valoff = valoff;
        sfd.wi= wi;
        sfd.sl = 0;
        sfd.title = title;
        sfd.checkres = updatertn;
        sfd.allowrl = NULL;
        sfd.hi_list = NULL;
        sfd.retevents = NULL;
        sfd.event = VI_KEY( DUMMY );
        sfd.cln = 1;
        sfd.event_wid = NO_WINDOW;
        rc = SelectLineInFile( &sfd );
        if( rc != ERR_NO_ERR ) {
            break;
        }
        if( sfd.sl == -1 ) {
            break;
        }
//        j = sfd.sl - 1;
    }

    /*
     * done, free memory
     */
    FreeEntireFile( cfile );
    DCDisplayAllLines();
    return( rc );

} /* SelectItemAndValue */
