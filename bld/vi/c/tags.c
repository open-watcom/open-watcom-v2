/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Support for ctags style tags.
*
****************************************************************************/


#include "vi.h"
#include "posix.h"
#include "rxsupp.h"
#ifdef __WIN__
    #include "winrtns.h"
#endif
#include "myio.h"

#include "clibext.h"


extern char _NEAR   META[];

/*
 * GetCurrentTag - get current tag word and hunt for it
 */
vi_rc GetCurrentTag( void )
{
    vi_rc       rc;
    char        tag[MAX_STR];

    rc = GimmeCurrentWord( tag, sizeof( tag ), false );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    rc = TagHunt( tag );
    return( rc );

} /* GetCurrentTag */

/*
 * TagHunt - hunt for a specified tag
 */
vi_rc TagHunt( const char *str )
{
    char        buff[MAX_STR], file[FILENAME_MAX];
    int         num;
    vi_rc       rc;

    rc = LocateTag( str, file, buff, sizeof( buff ) );
    if( rc == ERR_NO_ERR ) {

        PushFileStack();
        rc = EditFile( file, false );
        if( rc == ERR_NO_ERR ) {
            if( buff[0] != '/' ) {
                num = atoi( buff );
                rc = GoToLineNoRelCurs( num );
            } else {
                rc = FindTag( buff );
                if( rc < ERR_NO_ERR ) {
                    ColorFind( str, 0 );
                    rc = ERR_TAG_NOT_FOUND;
                }
            }
        } else {
            PopFileStack();
        }

    }

    if( rc == ERR_TAG_NOT_FOUND ) {
        Error( GetErrorMsg( rc ), str );
        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
    }
    return( rc );

} /* TagHunt */

/*
 * FindTag - locate a given tag
 */
vi_rc FindTag( const char *tag )
{
    vi_rc               rc;

    RegExpAttrSave( -1, &META[3] );
    rc = ColorFind( tag, 0 );
    RegExpAttrRestore();
    return( rc );

} /* FindTag */

#ifndef __WIN__
/*
 * PickATag - pick a tag
 */
static list_linenum PickATag( list_linenum tag_count, char **tag_list, const char *tagname )
{
    window_info     wi;
    list_linenum    i;
    bool            show_lineno;
    selectitem      si;
    vi_rc           rc;
    char            title[MAX_STR];

    memcpy( &wi, &dirw_info, sizeof( window_info ) );
    wi.area.x1 = 12;
    wi.area.x2 = EditVars.WindMaxWidth - 12;
    i = wi.area.y2 - wi.area.y1 + BORDERDIFF( wi );
    if( tag_count < i ) {
        wi.area.y2 -= (windim)( i - tag_count );
    }
    show_lineno = ( tag_count > i );
    MySprintf( title, "Pick A File For Tag \"%s\"", tagname );

    si.is_menu = false;
    si.show_lineno = show_lineno;
    si.wi = &wi;
    si.title = title;
    si.list = tag_list;
    si.maxlist = tag_count;
    si.result = NULL;
    si.num = 0;
    si.allowrl = NULL;
    si.hi_list = NULL;
    si.retevents = NULL;
    si.event = VI_KEY( DUMMY );
    si.cln = 1;
    si.event_wid = NO_WINDOW;
    rc = SelectItem( &si );
    if( rc != ERR_NO_ERR ) {
        return( -1 );
    }
    return( si.num );

} /* PickATag */
#endif

/*
 * selectTag - select a tag from a list of possible tags
 */
static vi_rc selectTag( FILE *fp, const char *str, char *fname, char *buff, int max_len )
{
    list_linenum    tag_count;
    char            **tag_list;
    int             i;
    list_linenum    whichtag;
    char            tag[MAX_STR];
    const char      *p;
    char            *p1;

    tag_count = 0;
    tag_list = NULL;
    p = GetNextWord1( buff, tag );
    for( ;; ) {
        tag_list = _MemReallocList( tag_list, tag_count + 1 );
        p1 = tag_list[tag_count] = DupString( p );
        SKIP_NOSPACES( p1 );
        p1 = strchr( tag_list[tag_count], ' ' );
        if( p1 != tag_list[tag_count] )
            *p1 = '\0';
        tag_count++;
        if( (p = myfgets( buff, max_len, fp )) == NULL )  {
            break;
        }
        p = GetNextWord1( p, tag );
        if( *tag == '\0' ) {
            continue;
        }
        if( EditFlags.IgnoreTagCase ) {
            i = stricmp( str, tag );
        } else {
            i = strcmp( str, tag );
        }
        if( i != 0 ) {
            break;
        }
    }
    if( EditFlags.TagPrompt && EditFlags.WindowsStarted && tag_count > 1 ) {
        whichtag = PickATag( tag_count, tag_list, str );
        if( whichtag < 0 ) {
            return( DO_NOT_CLEAR_MESSAGE_WINDOW );
        }
    } else {
        whichtag = 0;
    }
    tag_list[whichtag][strlen( tag_list[whichtag] )] = ' ';
    strcpy( buff, tag_list[whichtag] );
    MemFreeList( tag_count, tag_list );
    p = GetNextWord1( buff, fname );
    if( *fname == '\0' ) {
        return( ERR_INVALID_TAG_FOUND );
    }
    if( p[0] == '\0' ) {
        return( ERR_INVALID_TAG_FOUND );
    }
    strcpy( buff, p );;
    return( ERR_NO_ERR );

} /* selectTag */


#define TAGFILE    "\\tags"

/*
 * SearchForTags - search up the directory tree to see if there are any
 *                 tagfiles kicking around
 */
static FILE *SearchForTags( void )
{
    char    path[_MAX_PATH];
    char    *eop;

    if( CurrentFile && CurrentFile->name ) {
        _fullpath(path, CurrentFile->name, FILENAME_MAX);

        /*
         * Remove trailing filename.
         */
        eop = strrchr(path, '\\');
        if( eop != NULL ) {
            *eop = '\0';
        }
    } else {
        GetCWD2( path, sizeof( path ) );
    }

    eop = &path[strlen( path ) - 1];

    while( eop >= path ) {
        strcpy( eop + 1, TAGFILE );

        if( !access( path, F_OK ) ) {
            return fopen( path, "r" );
        }

        while( eop >= path && *eop != '\\' ) {
            --eop;
        }

        if( eop >= path ) {
            *eop-- = '\0';
        }
    } /* while */

    return( NULL );

} /* SearchForTags() */

/*
 * LocateTag - locate a tag in the tag file
 */
vi_rc LocateTag( const char *str, char *fname, char *buff, int max_len )
{
    char        tag[MAX_STR];
    int         i;
    FILE        *fp;
    vi_rc       rc;

    /*
     * get file and buffer
     */
    fp = GetFromEnvAndOpen( EditVars.TagFileName );
    if( fp == NULL ) {
        if( EditFlags.SearchForTagfile ) {
            fp = SearchForTags();
        }

        if( fp == NULL ) {
            return( ERR_FILE_NOT_FOUND );
        }
    }

    /*
     * loop until tag found
     */
    for( ;; ) {
        if( myfgets( buff, max_len, fp ) == NULL )  {
            rc = ERR_TAG_NOT_FOUND;
            break;
        }
        GetNextWord1( buff, tag );
        if( *tag == '\0' ) {
            continue;
        }
        if( EditFlags.IgnoreTagCase ) {
            i = stricmp( str, tag );
        } else {
            i = strcmp( str, tag );
            if( i < 0 ) {
                rc = ERR_TAG_NOT_FOUND;
                break;
            }
        }
        if( i == 0 ) {
            rc = selectTag( fp, str, fname, buff, max_len );
            break;
        }
    }
    fclose( fp );
    return( rc );

} /* LocateTag */
