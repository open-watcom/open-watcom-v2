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
* Description:  Help system file management.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifndef __UNIX__
    #include <direct.h>
#endif
#include "help.h"
#include "helpmem.h"
#include "iopath.h"
#include "pathgrp2.h"

#include "clibext.h"


#define MAX_HELP_FILES  10

static help_file_info   helpFiles[MAX_HELP_FILES + 1] = {
    { NULL, NULL, 0 }
};

static HelpSrchPathItem         *srch_List;

void SetHelpFileDefExt( const char *name, char *buff )
{
    pgroup2      pg;

    _splitpath2( name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( pg.ext[0] == '\0' )
        pg.ext = DEF_EXT;
    _makepath( buff, pg.drive, pg.dir, pg.fname, pg.ext );
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

    for( count = 0; count < MAX_HELP_FILES; ++count ) {
        if( helpFiles[count].name == NULL )
            break;
        HelpMemFree( helpFiles[count].name );
        helpFiles[count].name = NULL;
    }
}

static bool search_for_file( char *fullpath, const char *fname, HelpSrchPathItem *where )
/***************************************************************************************/
{
    unsigned    i;

    if( where == NULL ) {
        if( !HelpFileAccess( fname ) ) {
            strcpy( fullpath, fname );
            return( true );
        } else {
            return( false );
        }
    }
    /* check the current working directory */
    if( !HelpFileAccess( fname ) ) {
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
            if( !HelpFileAccess( fullpath ) ) {
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
    int         i;
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
            srch_List[i].info = HelpDupStr( srchlist[i].info );
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
    count = 0;
    for( ; *helpfilenames != NULL; ++helpfilenames ) {
        SetHelpFileDefExt( *helpfilenames, filename );
        if( search_for_file( fullpath, filename, srchlist ) ) {
            helpFiles[count].name = HelpDupStr( fullpath );
            helpFiles[count].fp = NULL;
            ++count;
            if( count >= MAX_HELP_FILES ) {
                break;
            }
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

help_file_info  *HelpFileInfo( void )
{
    return( helpFiles );
}
