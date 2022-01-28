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
* Description:  Mainline for bpatch.
*
****************************************************************************/


#include "bdiff.h"
#include "wpatchio.h"
#include "msg.h"


enum {
    MSG_USAGE_COUNT = 0
    #define pick(c,e,j) + 1
#if defined( BPATCH )
    #include "ppusage.gh"
#elif defined( BDUMP )
    #include "pdusage.gh"
#endif
    #undef pick
};

static void PrintBanner( void )
{
    static bool banner_printed = false;

    if( !banner_printed ) {
        banner_printed = true;
#if defined( BPATCH )
        puts( banner1w( "BPATCH", _BPATCH_VERSION_ ) );
#elif defined( BDUMP )
        puts( banner1w( "BDUMP", _BPATCH_VERSION_ ) );
#endif
        puts( banner2 );
        puts( banner2a( 1990 ) );
        puts( banner3 );
        puts( banner3a );
    }
}

static void Usage( void )
{
    char msgbuf[MAX_RESOURCE_SIZE];
    int i;

    for( i = MSG_USAGE_BASE; i < MSG_USAGE_BASE + MSG_USAGE_COUNT; i++ ) {
        GetMsg( msgbuf, i );
        puts( msgbuf );
    }
}

int main( int argc, char **argv )
{
    int         i;
    char        *target;
    bool        doprompt = true;
    bool        dobackup = true;
    bool        printlevel = false;
    bool        printusage = false;
    char        *patchname = NULL;
    int         rc;
    int         err;

    rc = EXIT_FAILURE;
    if( MsgInit() ) {
        err = 0;
        if( argc < 2 ) {
            printusage = true;
        } else {
            for( i = 1; argv[i] != NULL; ++i ) {
                if( argv[i][0] == '-' ) {
                    switch( tolower( argv[i][1] ) ) {
                    case 'p':
                        doprompt = false;
                        break;
                    case 'b':
                        dobackup = false;
                        break;
                    case 'f':           /* specify full pathname of file to patch */
                        ++i;
                        target = argv[i];
                        break;
                    case 'q':
                        printlevel = true;
                        break;
                    default:
                        printusage = true;
                        break;
                    }
                } else if( argv[i][0] == '?' ) {
                    printusage = true;
                } else {
                    if( patchname != NULL ) {
                        err = ERR_TWO_NAMES;
                    }
                    patchname = argv[i];
                }
            }
            if( patchname == NULL && !printusage ) {
                err = ERR_NO_NAME;
            }
        }
        PrintBanner();
        if( err ) {
            PatchError( err );
        } else if( printusage ) {
            Usage();
        } else {
            DoPatch( patchname, doprompt, dobackup, printlevel, NULL );
            rc = EXIT_SUCCESS;
        }
        MsgFini();
    }
    return( rc );
}
