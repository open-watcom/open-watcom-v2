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
* Description:  Main module for Windows help compiler.
*
****************************************************************************/


#include "hcmem.h"
#include "hlpdir.h"
#include "system.h"
#include "font.h"
#include "context.h"
#include "ctxomap.h"
#include "ttlbtree.h"
#include "topic.h"
#include "phrase.h"
#include "bmx.h"
#include "hpjread.h"
#include "parsing.h"
#include "hcerrors.h"
#include <stdlib.h>
#include <ctype.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include "clibext.h"


// Extension of a .HLP file.
static char const   HlpExt[] = ".hlp";


int main( int argc, char *argv[] )
{
#ifndef __WATCOMC__
    _argv = argv;
    _argc = argc;
#else
    argv = argv;
#endif

    if( argc < 2 || argc > 3 ) {
        HCWarning( USAGE );
        return( -1 );
    }

    // Parse the command line.
    char    cmdline[_MAX_PATH];
    char    *pfilename, *temp;
    bool    quiet = false;

    getcmd( cmdline );
    temp = cmdline;
    pfilename = NULL;
    while( *temp != '\0' && isspace( *temp ) ) {
        temp++;
    }
    if( *temp == '-' || *temp == '/' ) {
        temp++;
        if( (*temp != 'q' && *temp != 'Q') || !isspace( *(temp+1) ) ) {
            HCWarning( USAGE );
            return( -1 );
        } else {
            quiet = true;
            temp++;
            while( *temp != '\0' && isspace( *temp ) ) {
                temp++;
            }
            if( *temp == '\0' ) {
                HCWarning( USAGE );
                return( -1 );
            } else {
                pfilename = temp;
            }
        }
    } else if( *temp != '\0' ) {
        pfilename = temp++;
        while( *temp != '\0' && *temp != '/' && *temp != '-' ) {
            temp++;
        }
        if( *temp != '\0' ) {
            *temp = '\0';
            temp++;
            if( *temp != 'q' && *temp != 'Q' ) {
                HCWarning( USAGE );
                return( -1 );
            } else {
                temp++;
                while( *temp != '\0' && isspace( *temp ) ) {
                    temp++;
                }
                if( *temp != '\0' ) {
                    HCWarning( USAGE );
                    return( -1 );
                } else {
                    quiet = true;
                }
            }
        }
    }

    SetQuiet( quiet );


    //  Parse the given filename.

    char    path[_MAX_PATH];
    char    drive[_MAX_DRIVE];
    char    dir[_MAX_DIR];
    char    fname[_MAX_FNAME];
    char    ext[_MAX_EXT];

    _fullpath( path, pfilename, _MAX_PATH );
    _splitpath( path, drive, dir, fname, ext );

    if( stricmp( ext, PhExt ) == 0 || stricmp( ext, HlpExt ) == 0 ) {
        HCWarning( BAD_EXT );
        return( -1 );
    }
    if( ext[0] == '\0' ) {
        _makepath( path, drive, dir, fname, HpjExt );
    }

    char    destpath[_MAX_PATH];
    _makepath( destpath, drive, dir, fname, HlpExt );

    {
        InFile  input( path );
        if( input.bad() ) {
            HCWarning( FILE_ERR, pfilename );
            return( -1 );
        }

        //  Set up and start the help compiler.

        try {
            HFSDirectory    helpfile( destpath );
            HFFont          fontfile( &helpfile );
            HFContext       contfile( &helpfile );
            HFSystem        sysfile( &helpfile, &contfile );
            HFCtxomap       ctxfile( &helpfile, &contfile );
            HFTtlbtree      ttlfile( &helpfile );
            HFKwbtree       keyfile( &helpfile );
            HFBitmaps       bitfiles( &helpfile );

            Pointers        my_files = {
                                NULL,
                                NULL,
                                &sysfile,
                                &fontfile,
                                &contfile,
                                &ctxfile,
                                &keyfile,
                                &ttlfile,
                                &bitfiles,
            };

            if( stricmp( ext, RtfExt ) == 0 ) {
                my_files._topFile = new HFTopic( &helpfile );
                RTFparser   rtfhandler( &my_files, &input );
                rtfhandler.Go();
            } else {
                HPJReader   projfile( &helpfile, &my_files, &input );
                projfile.parseFile();
            }

            helpfile.dump();
            if( my_files._topFile != NULL ) {
                delete my_files._topFile;
            }
            if( my_files._phrFile != NULL ) {
                delete my_files._phrFile;
            }
        } catch( HCException ) {
            HCWarning( PROGRAM_STOPPED );
            return( -1 );
        }
    }
    mem_statistic();
    return( 0 );
}
