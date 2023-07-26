/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdlib.h>
#include <ctype.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
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
#include "pathgrp2.h"

#include "clibint.h"
#include "clibext.h"


int main( int argc, char *argv[] )
{
#if !defined( __WATCOMC__ )
    _argc = argc;
    _argv = argv;
#else
    /* unused parameters */ (void)argv;
#endif

    if( argc < 2 || argc > 3 ) {
        HCWarning( USAGE );
        return( -1 );
    }

    // Parse the command line.
    int     cmd_len;
    char    *cmd_line;
    char    *pfilename;
    char    *cmd;
    bool    quiet = false;

    cmd_len = _bgetcmd( NULL, 0 ) + 1;
    cmd_line = new char[cmd_len];
    _bgetcmd( cmd_line, cmd_len );
    cmd = cmd_line;
    while( *cmd != '\0' && isspace( *cmd ) ) {
        cmd++;
    }
    pfilename = NULL;
    if( *cmd == '-' || *cmd == '/' ) {
        cmd++;
        if( (cmd[0] != 'q' && cmd[0] != 'Q') || !isspace( cmd[1] ) ) {
            delete[] cmd_line;
            HCWarning( USAGE );
            return( -1 );
        }
        quiet = true;
        cmd++;
        while( *cmd != '\0' && isspace( *cmd ) ) {
            cmd++;
        }
        if( *cmd == '\0' ) {
            delete[] cmd_line;
            HCWarning( USAGE );
            return( -1 );
        }
        pfilename = cmd;
    } else if( *cmd != '\0' ) {
        pfilename = cmd++;
        while( *cmd != '\0' && *cmd != '/' && *cmd != '-' ) {
            cmd++;
        }
        if( *cmd != '\0' ) {
            *cmd = '\0';
            cmd++;
            if( *cmd != 'q' && *cmd != 'Q' ) {
                delete[] cmd_line;
                HCWarning( USAGE );
                return( -1 );
            }
            cmd++;
            while( *cmd != '\0' && isspace( *cmd ) ) {
                cmd++;
            }
            if( *cmd != '\0' ) {
                delete[] cmd_line;
                HCWarning( USAGE );
                return( -1 );
            }
            quiet = true;
        }
    }

    SetQuiet( quiet );


    //  Parse the given filename.

    char        path[_MAX_PATH];
    pgroup2     pg;

    _fullpath( path, pfilename, _MAX_PATH );
    _splitpath2( path, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );

    if( CMPFEXT( pg.ext, PH_EXT ) || CMPFEXT( pg.ext, HLP_EXT ) ) {
        delete[] cmd_line;
        HCWarning( BAD_EXT );
        return( -1 );
    }
    if( pg.ext[0] == '\0' ) {
        _makepath( path, pg.drive, pg.dir, pg.fname, HPJ_EXT );
    }

    char    destpath[_MAX_PATH];
    _makepath( destpath, pg.drive, pg.dir, pg.fname, HLP_EXT );

    {
        InFile  input( path );
        if( input.bad() ) {
            HCWarning( FILE_ERR, pfilename );
            delete[] cmd_line;
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

            if( CMPFEXT( pg.ext, RTF_EXT ) ) {
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
            delete[] cmd_line;
            HCWarning( PROGRAM_STOPPED );
            return( -1 );
        }
    }
    delete[] cmd_line;
    mem_statistic();
    return( 0 );
}
