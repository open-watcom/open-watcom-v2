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
* Description:  Help system file management.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#ifndef __UNIX__
    #include <direct.h>
#endif
#include "wio.h"
#include "watcom.h"
#include "uidef.h"
#include "stdui.h"
#include "help.h"
#include "helpmem.h"
#include "iopath.h"
#include "clibext.h"


help_file HelpFiles[MAX_HELP_FILES + 1] = {
    {NULL, 0 }
};

static HelpSrchPathItem         *srch_List;

void SetHelpFileDefExt( const char *name, char *buff )
{
    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];
    char        fname[_MAX_FNAME];
    char        ext[_MAX_EXT];

    _splitpath( name, drive, dir, fname, ext );
    if( *ext == '\0' ) {
        strcpy( ext, DEF_EXT );
    }
    _makepath( buff, drive, dir, fname, ext );
}

static void freeSearchList( void )
{
    unsigned    i;

    if( srch_List != NULL ) {
        for( i = 0; ; i++ ) {
            HelpMemFree( srch_List[i].info );
            if( srch_List[i].type == SRCHTYPE_EOL ) {
                break;
            }
        }
        HelpMemFree( srch_List );
        srch_List = NULL;
    }
}

static void freeHelpFiles( void )
{
    int         count;

    for( count = 0; HelpFiles[count].name != NULL && count < MAX_HELP_FILES; ++count ) {
        HelpMemFree( HelpFiles[count].name );
        HelpFiles[count].name = NULL;
    }
}

static bool search_for_file( char *fullpath, const char *fname, HelpSrchPathItem *where )
/***************************************************************************************/
{
    unsigned    i;

    if( where == NULL ) {
        if( !HelpAccess( fname, HELP_ACCESS_EXIST ) ) {
            strcpy( fullpath, fname );
            return( true );
        } else {
            return( false );
        }
    }
    /* check the current working directory */
    if( !HelpAccess( fname, HELP_ACCESS_EXIST ) ) {
        HelpGetCWD( fullpath, _MAX_PATH );
        fullpath += strlen( fullpath );
        if( !IS_PATH_SEP( fullpath[-1] ) ) {
            *fullpath++ = DIR_SEP;
        }
        strcpy( fullpath, fname );
        return( true );
    }
    for( i = 0; ; i++ ) {
        switch( where[i].type ) {
#ifndef __NETWARE_386__
        case SRCHTYPE_ENV:
            HelpSearchEnv( fname, where[i].info, fullpath );
            if( fullpath[0] != '\0' )
                return( true );
            break;
#endif
        case SRCHTYPE_PATH:
            strcpy( fullpath, where[i].info );
            strcat( fullpath, fname );
            if( !HelpAccess( fullpath, HELP_ACCESS_EXIST ) ) {
                return( true );
            }
            break;
        case SRCHTYPE_EOL:
            return( false );
        }
    }
}

static void initSearchList(
    HelpSrchPathItem *srchlist )    /* - list of places to look for help files */
{
    unsigned    i;
    int         count;

    freeSearchList();
    if( srchlist != NULL ) {
        count = 0;
        while( srchlist[count].type != SRCHTYPE_EOL )
            count++;
        count++;
        srch_List = HelpMemAlloc( count * sizeof( HelpSrchPathItem ) );
        for( i = 0; i < count ; ++i ) {
            srch_List[i].type = srchlist[i].type;
            if( srchlist[i].info != NULL ) {
                srch_List[i].info = HelpMemAlloc( strlen( srchlist[i].info ) + 1 );
                strcpy( srch_List[i].info, srchlist[i].info );
            } else {
                srch_List[i].info = NULL;
            }
        }
    }
}

static int do_init(                 /* INITIALIZATION FOR THE HELP PROCESS     */
    const char **helpfilenames,     /* - list of help file names               */
    HelpSrchPathItem *srchlist )    /* - list of places to look for help files */
{
    int         count;
    char        fullpath[_MAX_PATH];
    char        filename[_MAX_PATH];

    freeHelpFiles();
    for( count = 0; *helpfilenames != NULL && count < MAX_HELP_FILES; ++helpfilenames ) {
        SetHelpFileDefExt( *helpfilenames, filename );
        if( search_for_file( fullpath, filename, srchlist ) ) {
            HelpFiles[count].name = HelpMemAlloc( strlen( fullpath ) + 1 );
            strcpy( HelpFiles[count].name, fullpath );
            HelpFiles[count].f = 0;
            ++count;
        }
    }
    return( count );
}

int helpinit(                       /* INITIALIZATION FOR THE HELP PROCESS     */
    const char **helpfilenames,     /* - list of help file names               */
    HelpSrchPathItem *srchlist )    /* - list of places to look for help files */
{
    initSearchList( srchlist );
    return( do_init( helpfilenames, srchlist ) );

}

int help_reinit(                    /* RE-INITIALIZE AFTER CROSS FILE LINK */
    const char **helpfilenames )
{
    return( do_init( helpfilenames, srch_List ) );
}

void helpfini( void )
{
    freeHelpFiles();
    freeSearchList();
}
