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


#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "cmdline.h"
#include "womp.h"
#include "genutil.h"
#include "memutil.h"
#include "cantype.h"
#include "namemgr.h"
#include "canaddr.h"
#include "cansymb.h"
#include "canmisc.h"
#include "objio.h"
#include "objprs.h"
#include "array.h"

STATIC char         inputFile[ _MAX_PATH ];
STATIC OBJ_RFILE    *fileIn;
STATIC char         outputFile[ _MAX_PATH ];
STATIC OBJ_WFILE    *fileOut;

STATIC void init( void ) {

    ArrInit();
    NameInit();
    FixInit();
    CanMInit();
    CanTInit();
    CanAInit();
    CanSInit();
    ObjRecInit();
    PObjInit();
}

STATIC void fini( void ) {

    PObjFini();
    ObjRecFini();
    CanSFini();
    CanAFini();
    CanTFini();
    CanMFini();
    FixFini();
    NameFini();
    ArrFini();
}

STATIC void prune( void ) {

    ObjRecFini();
    CanSFini();
    CanTFini();
    FixFini();
    NameFini();

    NameInit();
    FixInit();
    CanTInit();
    CanSInit();
    ObjRecInit();
}

static void cleanupHandler( void )
{
    char *p;

    if( outputFile[0] != '\0' ) {
        if( fileOut != NULL ) {
            ObjWriteClose( fileOut );
            fileOut = NULL;
        }
        for( p = outputFile; p != &outputFile[ _MAX_PATH ]; ++p ) {
            if( *p == '\0' ) {
                unlink( outputFile );
                break;
            }
            if( ! isprint( *p ) ) break;
        }
    }
    MsgFini();
    MemFini();
}

#pragma off (unreferenced);
void main( int argc, char *argv[] ) {
#pragma on (unreferenced);

    uint        i;
    uint        num_files;
    cmdline_t   *cmd;
    int         multi_module;
    pobj_lib_info pli;

    atexit( cleanupHandler );
    MemInit();
    MsgInit( argv[0] );
    cmd = CmdLineParse();
    init();
    while( cmd->action != NULL ) {
        ActionInit( cmd );
        num_files = cmd->action->num_files;
        i = 0;
        do {
            ActionInfile( cmd, inputFile, i );
            fileIn = ObjReadOpen( inputFile );
            if( fileIn == NULL ) {
                Fatal( MSG_UNABLE_TO_OPEN_FILE, inputFile );
            }
            if( cmd->need_output ) {
                ActionOutfile( cmd, outputFile, i );
                fileOut = ObjWriteOpen( outputFile );
                if( fileOut == NULL ) {
                    Fatal( MSG_UNABLE_TO_OPEN_FILE, outputFile );
                }
            } else {
                fileOut = NULL;
            }
            if( cmd->quiet == 0 ) {
                char    msgbuff[MAX_RESOURCE_SIZE];

                MsgGet( MSG_CONVERTING, msgbuff );
                PrtFmt( msgbuff, inputFile );
                PrtFmt( "\n" );
            }
            pli.is_lib = 0;
            do {
                multi_module = PObj( fileIn, fileOut, &pli );
                prune();
            } while( multi_module );
            ObjReadClose( fileIn );
            if( fileOut != NULL ) {
                ObjWriteClose( fileOut );
                ActionRename( cmd, inputFile, outputFile, i, pli.is_lib,
                    pli.page_len );
            }
            ++i;
        } while( i < num_files );
        ActionFini( cmd );
    }
    fini();
    MsgFini();
    MemFini();
    outputFile[0] = '\0';
    exit( EXIT_SUCCESS );
}

