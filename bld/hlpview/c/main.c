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
* Description:  Character mode help browser mainline.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __UNIX__
    #include <dirent.h>
#else
    #include <direct.h>
#endif
#include "watcom.h"
#include "uidef.h"
#include "stdui.h"
#include "help.h"
#include "helpmem.h"
#include "filelist.h"

#define DEF_EXTENSION   ".ihp"
#define FIRST_SRCH_PATH "./"

static HelpSrchPathItem searchList[] = {
    SRCHTYPE_PATH,      FIRST_SRCH_PATH,// this may be changed when a cross
                                        // file hyperlink is processed
    SRCHTYPE_ENV,       "WWINHELP",
    SRCHTYPE_ENV,       "PATH",
    SRCHTYPE_EOL,       NULL
};

static void showHelp( char *name )
{
    char        fname[_MAX_FNAME];

    _splitpath( name, NULL, NULL, fname, NULL );
    printf( "\nThe Watcom Help command line:\n" );
    printf( "\n" );
    printf( "               %s help_file [topic_name]\n", fname );
    printf( "\n" );
    printf( "- if help_file is specified without an extension \"%s\" is assumed\n", DEF_EXTENSION );
    printf( "- the topic_name parameter is optional\n" );
    printf( "- if topic_name is not specifed the default topic used.\n" );
    printf( "- if topic_name contains spaces then it must be enclosed in quotes\n" );
    printf( "\n" );
    printf( "The following help files are available:\n\n" );
    PrintHelpFiles( searchList );
}

static HelpSrchPathItem *checkFileName( char *name, char *buf )
{
    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];
    char        fname[_MAX_FNAME];
    char        ext[_MAX_EXT];
    char        path[_MAX_PATH];

    _splitpath( name, drive, dir, fname, ext );
    if( *ext == '\0' ) {
        strcpy( ext, DEF_EXTENSION );
    }
    _makepath( buf, NULL, NULL, fname, ext );
    if( *drive != '\0' || *dir != '\0' ) {
        _makepath( path, drive, dir, NULL, NULL );
        searchList[0].info = HelpMemAlloc( strlen( path ) + 1 );
        strcpy( searchList[0].info, path );
    }
    return( searchList );
}

static void freeSrchList( void )
{
    if( strcmp( searchList[0].info, FIRST_SRCH_PATH ) ) {
        HelpMemFree( searchList[0].info );
    }
}

int main( int argc, char *argv[] )
{
    char                *helpfiles[] = { NULL, NULL };
    char                *topic;
    char                filename[ _MAX_PATH ];
    HelpSrchPathItem    *srchlist;
    int                 rc;
    bool                err;

    HelpMemInit();
    err = FALSE;
    if( argc < 2 || !strcmp( argv[1], "?" ) || !strcmp( argv[1], "-?" )
        || !strcmp( argv[1], "/?" ) ) {
        showHelp( argv[0] );
        return( EXIT_SUCCESS );
    }
    if( argc > 3 ) {
        printf( "Too many arguments specified on the command line\n" );
        printf( "For help type %s -?\n", argv[0] );
        return( EXIT_FAILURE );
    }
    srchlist = checkFileName( argv[1], filename );
    strlwr( filename );
    helpfiles[ 0 ] = filename;
    if( argc == 3 ) {
        topic = HelpMemAlloc( strlen( argv[2] ) + 1);
        strcpy( topic, argv[2] );
    } else {
        topic = NULL;
    }
    if( !uistart() ) {
        printf( "ui failed\n" );
        return( EXIT_FAILURE );
    } else {
#if defined( __OS2__ ) || defined( __NT__ )
        initmouse( 2 );  /* the 0=mouseless,1=mouse,2=initialized mouse */
#elif !defined __UNIX__
        uiinitgmouse( 2 );  /* the 0=mouseless,1=mouse,2=initialized mouse */
        FlipCharacterMap();
#endif
        if( helpinit( helpfiles, srchlist ) ) {
            rc = showhelp( topic, NULL, HELPLANG_ENGLISH );
            if( rc == HELP_NO_SUBJECT ) {
                err = TRUE;
                uirestorebackground();
                printf( "Unable to find the topic \"%s\" in the help file \"%s\".\n",
                        topic, filename );
                HelpMemFree( topic );
                topic = NULL;
            }
        } else {
            err = TRUE;
            HelpMemFree( topic );
            topic = NULL;
            uirestorebackground();
            printf( "Unable to open the help file \"%s\".\n", filename );
//          printf( "Please check that you have specified the correct help file\n" );
//          printf( "and that the file is in a directory listed in your PATH\n" );
//          printf( "or the file is in the current directory\n" );
//          printf( "or you have specified the path where the file is located.\n" );
        }
        helpfini();
        freeSrchList();
        if( !err ) {
            uirestorebackground();
        }
        uifini();
    }
    HelpMemFini();
    return( EXIT_SUCCESS );
}
