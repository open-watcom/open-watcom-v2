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
* Description:  Mainline for bpatch.
*
****************************************************************************/


#include "bdiff.h"
#include "msg.h"


static void PrintBanner( void )
{
    static bool banner_printed = false;

    if( !banner_printed ) {
        banner_printed = true;
        printf( banner1w( "BPATCH", _BPATCH_VERSION_ ) "\n" );
        printf( banner2 "\n" );
        printf( banner2a( "1990" ) "\n" );
        printf( banner3 "\n" );
        printf( banner3a "\n" );
    }
}

void Usage( const char *name )
{
    char msgbuf[MAX_RESOURCE_SIZE];
    int i;

    i = MSG_USAGE_FIRST;
    PrintBanner();
    GetMsg( msgbuf, i );
    printf( msgbuf, name );
    printf( "\n" );
    for( i = i + 1; i <= MSG_USAGE_LAST; i++ ) {
        GetMsg( msgbuf, i );
        if( msgbuf[ 0 ] == 0 ) break;
        printf( "\n" );
        printf( msgbuf );
    }
    MsgFini();
    exit( EXIT_FAILURE );
}

void main( int argc, char **argv )
{
    int         i;
    char        *target;
    bool        doprompt = true;
    bool        dobackup = true;
    bool        printlevel = false;
    char        *patchname = NULL;

    if( !MsgInit() ) exit( EXIT_FAILURE );
    if( argc < 2 ) Usage( argv[0] );
    for( i = 1; argv[ i ] != NULL; ++i ) {
        if( argv[ i ][ 0 ] == '-' ) {
            switch( tolower( argv[ i ][ 1 ] ) ) {
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
                Usage( argv[ 0 ] );
                break;
            }
        } else if( argv[ i ][ 0 ] == '?' ) {
            Usage( argv[ 0 ] );
        } else {
            if( patchname != NULL ) {
                if( doprompt ) {
                    PrintBanner();
                }
                PatchError( ERR_TWO_NAMES );
            }
            patchname = argv[ i ];
        }
    }
    PrintBanner();
    if( patchname == NULL ) {
        PatchError( ERR_NO_NAME );
    }
    DoPatch( patchname, doprompt, dobackup, printlevel, NULL );
    MsgFini();
    exit( EXIT_SUCCESS );
}
