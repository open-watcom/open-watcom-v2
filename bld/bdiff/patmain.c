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


#include "bdiff.h"

extern  PATCH_RET_CODE  DoPatch( char *, int, int, int, char * );
extern  int             GetMsg( char *, int );
extern  void            Message( int, ... );
extern  void            MsgPrintf( int resourceid, va_list arglist );

static void PrintBanner()
{
    static int  banner_printed = 0;

    if( !banner_printed ) {
        banner_printed = 1;
        printf( banner1w( "BPATCH", _BPATCH_VERSION_ ) "\n" );
        printf( banner2( "1990" ) "\n" );
        printf( banner3 "\n" );
    }
}

void Usage( char *name )
{
    char msgbuf[80];
    int i;

    PrintBanner();
    GetMsg( msgbuf, MSG_USAGE_LN_1 );
    printf( msgbuf, name );
    printf( "\n" );
    for( i = MSG_USAGE_LN_2; i <= MSG_USAGE_LAST; i++ ) {
        GetMsg( msgbuf, i );
        if( msgbuf[ 0 ] == 0 ) break;
        printf( "\n" );
        printf( msgbuf );
    }
    MsgFini();
    exit( EXIT_FAILURE );
}

static void Err( int format, va_list args )
{
    char        msgbuf[80];

    GetMsg( msgbuf, MSG_ERROR );
    printf( msgbuf );
    MsgPrintf( format, args);
}

void PatchError( int format, ... )
{
    va_list     args;

    va_start( args, format );
    Err( format, args );
    printf( "\n" );
    va_end( args );
    MsgFini();
    exit( EXIT_FAILURE );
}

void FilePatchError( int format, ... )
{
    va_list     args;
    int         err;

    va_start( args, format );
    err = errno;
    Err( format, args );
    printf( ": %s\n", strerror( err ) );
    va_end( args );
    MsgFini();
    exit( EXIT_FAILURE );
}

void main( int argc, char **argv )
{
    int         i;
    char        *target;
    int         doprompt = 1;
    int         dobackup = 1;
    int         printlevel = 0;
    char        *patchname = NULL;

    if( !MsgInit() ) exit( EXIT_FAILURE );
    if( argc < 2 ) Usage( argv[0] );
    for( i = 1; argv[ i ] != NULL; ++i ) {
        if( argv[ i ][ 0 ] == '-' ) {
            switch( tolower( argv[ i ][ 1 ] ) ) {
            case 'p':
                doprompt = 0;
                break;
            case 'b':
                dobackup = 0;
                break;
            case 'f':           /* specify full pathname of file to patch */
                ++i;
                target = argv[i];
                break;
            case 'q':
                printlevel = 1;
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
