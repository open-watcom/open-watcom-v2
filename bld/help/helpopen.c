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


#include <stdlib.h>
#include <unistd.h>

#define PATH_SEPARATOR '/'

#include <string.h>

#if !defined(__QNX__) && !defined(UNIX)
    #include <direct.h>
#endif

#include "uidef.h"
#include "stdui.h"
#include "help.h"
#include "helpmem.h"

#if defined(UNIX)
    #include "clibext.h"
#endif

#ifdef PEN
#include "penride.h"
#endif


help_file HelpFiles[MAX_HELP_FILES] = {
    {NULL, 0 }
};

static HelpSrchPathItem         *srch_List;

static void freeSearchList( void ) {
    unsigned    i;

    if( srch_List != NULL ) {
        i = 0;
        for( ;; i++ ) {
            HelpMemFree( srch_List[i].info );
            if( srch_List[i].type == SRCHTYPE_EOL ) break;
        }
        HelpMemFree( srch_List );
        srch_List = NULL;
    }
}

static bool search_for_file( char *fullpath, char *fname,
                                 HelpSrchPathItem *where )
/**************************************************************/
{
    unsigned            i;

    if( where == NULL ) {
        if( !HelpAccess( fname, HELP_ACCESS_EXIST ) ) {
            strcpy( fullpath, fname );
            return( TRUE );
        } else {
            return( FALSE );
        }
    }
    /* check the current working directory */
    if( !HelpAccess( fname, HELP_ACCESS_EXIST ) ) {
        HelpGetCWD( fullpath, _MAX_PATH );
        i = strlen( fullpath );
        fullpath[i] = PATH_SEPARATOR;
        fullpath[i+1] = '\0';
        strcat( fullpath, fname );
        return( TRUE );
    }
    for( i=0; ; i++ ) {
        switch( where[i].type ) {
        #ifndef __NETWARE_386__
        case SRCHTYPE_ENV:
            HelpSearchEnv( fname, where[i].info, fullpath );
            if( fullpath[0] != '\0' ) return( TRUE );
            break;
        #endif
        case SRCHTYPE_PATH:
            strcpy( fullpath, where[i].info );
            strcat( fullpath, fname );
            if( !HelpAccess( fullpath, HELP_ACCESS_EXIST ) ) {
                return( TRUE );
            }
            break;
        case SRCHTYPE_EOL:
            return( FALSE );
            break;
        }
    }
}
int do_init(                    /* INITIALIZATION FOR THE HELP PROCESS    */
    char **helpfilenames,         /* - list of help file names              */
    HelpSrchPathItem *srchlist )  /* - list of places to look for help files */
{
    int                 count;
    char                **fname;
    char                fullpath[_MAX_PATH];

    count = 0;
    while( HelpFiles[count].name != NULL && count < MAX_HELP_FILES - 1 ) {
        HelpMemFree( HelpFiles[count].name );
        HelpFiles[count].name = NULL;
        count += 1;
    }
    fname = helpfilenames;
    count = 0;
    while( *fname != NULL  &&  count < MAX_HELP_FILES-1 ) {
        if( search_for_file( fullpath, *fname, srchlist ) ) {
            HelpFiles[count].name = HelpMemAlloc( strlen(fullpath) + 1 );
            strcpy( HelpFiles[count].name, fullpath );
            HelpFiles[count].f = 0;
            count += 1;
        }
        ++fname;
    }
    HelpFiles[count].name = NULL;
    HelpFiles[count].f = 0;
    return( count );
}

int helpinit(                   /* INITIALIZATION FOR THE HELP PROCESS    */
    char **helpfilenames,         /* - list of help file names              */
    HelpSrchPathItem *srchlist )  /* - list of places to look for help files */
{
    unsigned    i;
    int         cnt;

    if( srchlist != NULL ) {
        cnt = 0;
        while( srchlist[cnt].type != SRCHTYPE_EOL ) cnt++;
        cnt++;
        srch_List = HelpMemAlloc( cnt * sizeof( HelpSrchPathItem ) );
        for( i = 0; i < cnt ; ++i ) {
            srch_List[i].type = srchlist[i].type;
            if( srchlist[i].info != NULL ) {
                srch_List[i].info = HelpMemAlloc( strlen( srchlist[i].info )
                                                  + 1 );
                strcpy( srch_List[i].info, srchlist[i].info );
            } else {
                srch_List[i].info = NULL;
            }
        }
    } else {
        freeSearchList();
    }
    cnt = do_init( helpfilenames, srchlist );
    return( cnt );

}

int help_reinit(                /* RE-INITIALIZE AFTER CROSS FILE LINK */
    char **helpfilenames )
{
    int         cnt;
    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];
    char        fname[_MAX_FNAME];
    char        ext[_MAX_EXT];


    _splitpath( helpfilenames[0], drive, dir, fname, ext );
    if( *ext == '\0' ) {
        _makepath( helpfilenames[0], drive, dir, fname, ".ihp" );
    }
    cnt = do_init( helpfilenames, srch_List );
    return( cnt );
}

void helpfini( void )
{
    int         count;

    for( count=0; count < MAX_HELP_FILES-1; ++count ){
        if( HelpFiles[count].name == NULL ) break;
        HelpMemFree( HelpFiles[count].name );
        HelpFiles[count].name = NULL;
    }
    freeSearchList();
}

