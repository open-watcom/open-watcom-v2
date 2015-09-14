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
 * tempFileSetup - set up a temp file with data in it
 */
static void tempFileSetup( file **cfile, char *list[], int maxlist, int indent,
                           bool makelower )
{
    int         j, boff, i, k;
    char        dd[FILENAME_MAX];

    /*
     * allocate temporary file structures
     */
    *cfile = FileAlloc( NULL );

    /*
     * copy data into buffer and create fcbs
     */
    j = boff = 0;
    while( j < maxlist ) {
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
        j++;
    }

    CreateFcbData( *cfile, boff );
    (*cfile)->fcbs.tail->non_swappable = true;

} /* tempFileSetup */

/*
 * SelectItem - select item to set from a menu
 */
vi_rc SelectItem( selectitem *si )
{
    int                 j;
    file                *cfile;
    selflinedata        sfd;
    vi_rc               rc;

    tempFileSetup( &cfile, si->list, si->maxlist, 0, false );

    /*
     * get selected line
     */
    memset( &sfd, 0, sizeof( sfd ) );
    sfd.f = cfile;
    sfd.wi= si->wi;
    sfd.title = si->title;
    sfd.allow_rl = si->allowrl;
    sfd.hilite = si->hilite;
    sfd.show_lineno = si->show_lineno;
    sfd.retevents = si->retevents;
    sfd.event = si->event;
    sfd.cln = si->cln;
    sfd.eiw = si->eiw;
    sfd.is_menu = si->is_menu;
    rc = SelectLineInFile( &sfd );
    si->event = sfd.event;
    if( rc == ERR_NO_ERR ) {
        if( sfd.sl == -1 || sfd.sl == 0 ) {
            if( si->result != NULL ) {
                si->result[0] = 0;
            }
            si->num = -1;
        } else {
            j = (int) sfd.sl - 1;
            if( si->result != NULL ) {
                strcpy( si->result, si->list[j] );
            }
            si->num = j;
        }
    }

    /*
     * done, free memory
     */
    if( cfile != NULL ) {
        FreeEntireFile( cfile );
    }
    DCDisplayAllLines();
    return( rc );

} /* SelectItem */

/*
 * SelectItemAndValue - select item from list and give it a value
 */
vi_rc SelectItemAndValue( window_info *wi, char *title, char **list,
                        int maxlist, vi_rc (*updatertn)( const char *, char *, int * ),
                        int indent, char **vals, int valoff )
{
//    int                 j;
    file                *cfile;
    selflinedata        sfd;
    vi_rc               rc;

    tempFileSetup( &cfile, list, maxlist, indent, true );

    for( ;; ) {

        /*
         * go get selected line
         */
        memset( &sfd, 0, sizeof( sfd ) );
        sfd.f = cfile;
        sfd.wi = wi;
        sfd.title = title;
        sfd.checkres = updatertn;
        sfd.cln = 1;
        sfd.eiw = NO_WINDOW;
        sfd.vals = vals;
        sfd.valoff = valoff;
        rc = SelectLineInFile( &sfd );
        if( rc != ERR_NO_ERR ) {
            break;
        }
        if( sfd.sl == -1 ) {
            break;
        }
//        j = (int) sfd.sl - 1;

    }

    /*
     * done, free memory
     */
    if( cfile != NULL ) {
        FreeEntireFile( cfile );
    }
    DCDisplayAllLines();
    return( rc );

} /* SelectItemAndValue */
