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
#include <direct.h>
#include <uidef.h>
#include "stdui.h"
#include "help.h"
#include "helpmem.h"
#include "trmemcvr.h"
#include "filelist.h"
#include <unistd.h>
#include <fcntl.h>

#define DEF_EXTENSION   ".IHP"
#define FIRST_SRCH_PATH ".\\"

static HelpSrchPathItem searchList[] = {
    SRCHTYPE_PATH,      FIRST_SRCH_PATH,// this may be changed when a cross
                                        // file hyperlink is processed
    SRCHTYPE_ENV,       "WWINHELP",
    SRCHTYPE_ENV,       "PATH",
    SRCHTYPE_EOL,       NULL
};

static int      memFHdl;

static void memInit( void ) {
#ifdef TRMEM
    memFHdl= open( "MEMERR", O_WRONLY | O_TRUNC | O_CREAT | O_TEXT,
                    S_IRWXO | S_IRWXG | S_IRWXU );
    TRMemOpen();
    TRMemRedirect( memFHdl );
#else
    memFHdl = memFHdl;
#endif
}

static void memFini( void ) {
#ifdef TRMEM
//    TRMemPrtList();
    TRMemClose();
    if( tell( memFHdl ) != 0 ) {
        printf( "**************************\n" );
        printf( "* A memory error ocurred *\n" );
        printf( "**************************\n" );
    }
    close( memFHdl );
#endif
}


static void showHelp( char *name ) {

    char        fname[_MAX_FNAME];

    _splitpath( name, NULL, NULL, fname, NULL );
    printf( "\nThe WATCOM Help command line:\n" );
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

static HelpSrchPathItem *checkFileName( char *name, char *buf ) {

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

static void freeSrchList( void ) {

    if( freeSrchList != NULL && searchList[0].info != FIRST_SRCH_PATH ) {
        HelpMemFree( searchList[0].info );
    }
}

void main( int argc, char *argv[] ) {

    char                *helpfiles[] = { NULL, NULL };
    char                *topic;
    char                filename[ _MAX_PATH ];
    HelpSrchPathItem    *srchlist;
    int                 rc;
    bool                err;

    memInit();
    err = FALSE;
    if( argc < 2 || !strcmp( argv[1], "?" ) || !strcmp( argv[1], "-?" )
        || !strcmp( argv[1], "/?" ) ) {
        showHelp( argv[0] );
        return;
    }
    if( argc > 3 ) {
        printf( "Too many arguments specified on the command line\n" );
        printf( "For help type %s -?\n", argv[0] );
        return;
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
        return;
    } else {
    #ifndef __OS2__
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
        if( !err ) uirestorebackground();
        uifini();
    }
    memFini();
}
