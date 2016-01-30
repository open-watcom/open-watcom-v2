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
* Description:  Support for ctags style tags.
*
****************************************************************************/


#include "vi.h"
#include "posix.h"
#include "rxsupp.h"
#ifdef __WIN__
    #include "winrtns.h"
#endif

#include "clibext.h"


#define isWSorCtrlZ( x )    (isspace( x ) || (x == 0x1A))

extern char _NEAR   META[];

/*
 * GetCurrentTag - get current tag word and hunt for it
 */
vi_rc GetCurrentTag( void )
{
    vi_rc       rc;
    char        tag[MAX_STR];

    rc = GimmeCurrentWord( tag, sizeof( tag ) - 1, false );
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

    rc = LocateTag( str, file, buff );
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
static int PickATag( int clist, char **list, const char *tagname )
{
    window_info tw;
    int         i;
    bool        show_lineno;
    selectitem  si;
    vi_rc       rc;
    char        title[MAX_STR];

    memcpy( &tw, &dirw_info, sizeof( window_info ) );
    tw.area.x1 = 12;
    tw.area.x2 = EditVars.WindMaxWidth - 12;
    i = tw.area.y2 - tw.area.y1 + 1;
    if( tw.has_border ) {
        i -= 2;
    }
    if( clist < i ) {
        tw.area.y2 -= i - clist;
    }
    show_lineno = ( clist > i );
    MySprintf( title, "Pick A File For Tag \"%s\"", tagname );

    memset( &si, 0, sizeof( si ) );
    si.wi = &tw;
    si.title = title;
    si.list = list;
    si.maxlist = clist;
    si.num = 0;
    si.retevents = NULL;
    si.event = VI_KEY( DUMMY );
    si.show_lineno = show_lineno;
    si.cln = 1;
    si.eiw = NO_WINDOW;

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
static vi_rc selectTag( FILE *f, const char *str, char *buff, char *fname )
{
    int         tagcnt;
    char        **taglist;
    int         i;
    int         whichtag;
    char        tag[MAX_STR];
    char        *p;

    tagcnt = 0;
    taglist = NULL;
    p = GetNextWord1( buff, tag );
    for( ;; ) {
        p = SkipLeadingSpaces( p );
        taglist = MemReAlloc( taglist, sizeof( char * ) * ( tagcnt + 1 ) );
        taglist[tagcnt] = DupString( p );
        i = 0;
        while( !isspace( taglist[tagcnt][i] ) ) {
            i++;
        }
        taglist[tagcnt][i] = 0;
        tagcnt++;
        if( fgets( buff, MAX_STR, f ) == NULL )  {
            break;
        }
        for( i = strlen( buff ); i && isWSorCtrlZ( buff[i - 1] ); --i ) {
            buff[i - 1] = '\0';
        }
        p = GetNextWord1( buff, tag );
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
    fclose( f );
    if( EditFlags.TagPrompt && EditFlags.WindowsStarted && tagcnt > 1 ) {
        whichtag = PickATag( tagcnt, taglist, str );
        if( whichtag < 0 ) {
            return( DO_NOT_CLEAR_MESSAGE_WINDOW );
        }
    } else {
        whichtag = 0;
    }
    taglist[whichtag][strlen( taglist[whichtag] )] = ' ';
    strcpy( buff, taglist[whichtag] );
    MemFreeList( tagcnt, taglist );
    p = GetNextWord1( buff, fname );
    if( *fname == '\0' ) {
        return( ERR_INVALID_TAG_FOUND );
    }
    p = SkipLeadingSpaces( p );
    if( p[0] == 0 ) {
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
    char    path[FILENAME_MAX];
    char    *eop;

    if( CurrentFile && CurrentFile->name ) {
        _fullpath(path, CurrentFile->name, FILENAME_MAX);

        /*
         * Remove trailing filename.
         */
        eop = strrchr(path, '\\');
        if (eop) {
            *eop = 0x00;
        }
    } else {
        GetCWD2( path, FILENAME_MAX );
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
            *eop-- = 0x00;
        }
    } /* while */

    return( NULL );

} /* SearchForTags() */

/*
 * LocateTag - locate a tag in the tag file
 */
vi_rc LocateTag( const char *str, char *fname, char *buff )
{
    char        tag[MAX_STR];
    int         i;
    FILE        *f;

    /*
     * get file and buffer
     */
    f = GetFromEnvAndOpen( EditVars.TagFileName );
    if( f == NULL ) {
        if( EditFlags.SearchForTagfile ) {
            f = SearchForTags();
        }

        if( f == NULL ) {
            return( ERR_FILE_NOT_FOUND );
        }
    }

    /*
     * loop until tag found
     */
    for( ;; ) {
        if( fgets( buff, MAX_STR, f ) == NULL )  {
            fclose( f );
            return( ERR_TAG_NOT_FOUND );
        }
        for( i = strlen( buff ); i && isWSorCtrlZ( buff[i - 1] ); --i ) {
            buff[i - 1] = '\0';
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
                fclose( f );
                return( ERR_TAG_NOT_FOUND );
            }
        }
        if( i == 0 ) {
            return( selectTag( f, str, buff, fname ) );
        }
    }

} /* LocateTag */
