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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "vi.h"
#include "keys.h"
#include "rxsupp.h"
#ifdef __WIN__
#include "winrtns.h"
#endif

/*
 * GetCurrentTag - get current tag word and hunt for it
 */
int GetCurrentTag( void )
{
    int         i;
    char        tag[MAX_STR];

    i = GimmeCurrentWord( tag, sizeof( tag ) - 1, FALSE );
    if( i ) {
        return( i );
    }
    i = TagHunt( tag );
    return( i );

} /* GetCurrentTag */

/*
 * TagHunt - hunt for a specified tag
 */
int TagHunt( char *str )
{
    char        buff[MAX_STR],file[_MAX_PATH];
    int         num,rc=ERR_NO_ERR;

    rc = LocateTag( str, file, buff );
    if( !rc ) {

        PushFileStack();
        rc = EditFile( file, FALSE );
        if( rc == ERR_NO_ERR ) {
            if( buff[0] != '/' ) {
                num = atoi( buff );
                rc = GoToLineNoRelCurs( num );
            } else {
                rc = FindTag( buff );
                if( rc < 0 ) {
                    strcpy( buff, str );
                    ColorFind( buff, 0 );
                    rc = ERR_TAG_NOT_FOUND;
                }
            }
        } else {
            PopFileStack();
        }

    }

    if( rc == ERR_TAG_NOT_FOUND ) {
        Error( GetErrorMsg(rc), str );
        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
    }
    return( rc );

} /* TagHunt */

/*
 * FindTag - locate a given tag
 */
int FindTag( char *tag )
{
    extern char near META[];
    int         rc,omag;
    char        *oldms;

    omag = EditFlags.Magic;
    EditFlags.Magic = FALSE;
    oldms = Majick;
    Majick = &META[3];

    rc = ColorFind( tag, 0 );

    Majick = oldms;
    EditFlags.Magic = omag;
    return( rc );

} /* FindTag */

#ifndef __WIN__
/*
 * PickATag - pick a tag
 */
int PickATag( int clist, char **list, char *tagname )
{
    window_info tw;
    int         i;
    bool        show_lineno;
    selectitem  si;
    int         rc;
    char        title[MAX_STR];

    memcpy( &tw, &dirw_info, sizeof( window_info ) );
    tw.x1 = 12;
    tw.x2 = WindMaxWidth-12;
    i = tw.y2 - tw.y1+1;
    if( tw.has_border ) {
        i -= 2;
    }
    if( clist < i ) {
        tw.y2 -= ( i-clist );
    }
    if( clist > i ) {
        show_lineno = TRUE;
    } else {
        show_lineno = FALSE;
    }
    MySprintf( title, "Pick A File For Tag \"%s\"", tagname );

    memset( &si, 0, sizeof( si ) );
    si.wi = &tw;
    si.title = title;
    si.list = list;
    si.maxlist = clist;
    si.num = 0;
    si.retevents = NULL;
    si.event = -1;
    si.show_lineno = show_lineno;
    si.cln = 1;
    si.eiw = NO_WINDOW;

    rc = SelectItem( &si );
    if( rc ) {
        return( -1 );
    }
    return( si.num );

} /* PickATag */
#endif

/*
 * selectTag - select a tag from a list of possible tags
 */
static int selectTag( FILE *f, char *str, char *buff, char *fname )
{
    int         tagcnt;
    char        **taglist;
    int         i;
    int         whichtag;
    char        tag[MAX_STR];

    tagcnt = 0;
    taglist = NULL;

    while( 1 ) {
        RemoveLeadingSpaces( buff );
        taglist = MemReAlloc( taglist, sizeof( char * ) * (tagcnt+1) );
        AddString( &taglist[tagcnt], buff );
        i = 0;
        while( !isspace( taglist[tagcnt][i] ) ) {
            i++;
        }
        taglist[tagcnt][i] = 0;
        tagcnt++;
        if( fgets( buff,MAX_STR,f ) == NULL )  {
            break;
        }
        if( NextWord1( buff,tag ) <= 0 ) {
            continue;
        }
        if( EditFlags.IgnoreTagCase ) {
            i = stricmp( str,tag );
        } else {
            i = strcmp( str,tag );
        }
        if( i ) {
            break;
        }
    }
    fclose( f );
    if( EditFlags.TagPrompt && EditFlags.WindowsStarted && tagcnt > 1 ) {
        whichtag = PickATag( tagcnt, taglist, str );
        if( whichtag < 0 ) {
            return( DO_NOT_CLEAR_MESSAGE_WINDOW );
        }
    } else {
        whichtag = 0;
    }
    taglist[ whichtag ][ strlen( taglist[ whichtag ] ) ] = ' ';
    strcpy( buff, taglist[ whichtag ] );
    MemFreeList( tagcnt, taglist );

    if( NextWord1( buff, fname ) <= 0 ) {
        return( ERR_INVALID_TAG_FOUND );
    }
    buff[strlen(buff)-1] = 0;
    RemoveLeadingSpaces( buff );
    if( buff[0] == 0 ) {
        return( ERR_INVALID_TAG_FOUND );
    }
    return( ERR_NO_ERR );

} /* selectTag */


#define TAGFILE    "\\tags"

/*
 * SearchForTags - search up the directory tree to see if there are any
 *                 tagfiles kicking around
 */
FILE *SearchForTags(void)
{
    char  path[_MAX_PATH];
    char *eop;

    if (CurrentFile && CurrentFile->name) {
        _fullpath(path, CurrentFile->name, _MAX_PATH);

        /*
         * Remove trailing filename.
         */
        eop = strrchr(path, '\\');
        if (eop) {
            *eop = 0x00;
        }
    } else {
        GetCWD2(&path, _MAX_PATH);
    }

    eop = &path[strlen(path) - 1];

    while (eop >= path) {
        strcpy((eop + 1), TAGFILE);

        if (!access(path, F_OK)) {
            return fopen(path, "r");
        }

        while ((eop >= path) && (*eop != '\\')) {
            --eop;
        }

        if (eop >= path) {
            *eop-- = 0x00;
        }
    } /* while */

    return NULL;
} /* SearchForTags() */


/*
 * LocateTag - locate a tag in the tag file
 */
int LocateTag( char *str, char *fname, char *buff )
{
    char        tag[MAX_STR];
    int         i;
    FILE        *f;

    /*
     * get file and buffer
     */
    f = GetFromEnvAndOpen(TagFileName);
    if (!f) {
        if (EditFlags.SearchForTagfile) {
            f = SearchForTags();
        }

        if (!f) {
            return ERR_FILE_NOT_FOUND;
        }
    }

    /*
     * loop until tag found
     */
    while( TRUE ) {

        if( fgets( buff,MAX_STR,f ) == NULL )  {
            fclose( f );
            return( ERR_TAG_NOT_FOUND );
        }
        if( NextWord1( buff,tag ) <= 0 ) {
            continue;
        }
        if( EditFlags.IgnoreTagCase ) {
            i = stricmp( str,tag );
        } else {
            i = strcmp( str,tag );
            if( i < 0 ) {
                fclose( f );
                return( ERR_TAG_NOT_FOUND );
            }
        }
        if( !i ) {
            return( selectTag( f, str, buff, fname ) );
        }
    }
} /* LocateTag */
