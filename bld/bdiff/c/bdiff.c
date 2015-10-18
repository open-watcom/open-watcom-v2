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
* Description:  Mainline for binary diff utility.
*
****************************************************************************/


#include "bdiff.h"
#include "msg.h"
#include "diff.h"

char    *OldSymName;
char    *NewSymName;

bool    AppendPatchLevel;
bool    Verbose;

byte    *PatchFile;
byte    *OldFile;
byte    *NewFile;

char    *CommentFile;

int     OldCorrection;
int     NewCorrection;

static const char   *SyncString = NULL;

static char         *newName;

void Usage( const char *name )
{
    char msgbuf[MAX_RESOURCE_SIZE];
    int i;

    i = MSG_USAGE_FIRST;
    GetMsg( msgbuf, i );
    printf( msgbuf, name );
    for( i = i + 1; i <= MSG_USAGE_LAST; i++ ) {
        GetMsg( msgbuf, i );
        if( msgbuf[0] == '\0' )
            break;
        puts( msgbuf );
    }
    MsgFini();
    exit( EXIT_FAILURE );
}

algorithm ParseArgs( int argc, char **argv )
{
    char        **arg;
    char        *curr;
    algorithm   alg;

    newName = NULL;
    if( argc < 4 ) {
        Usage( argv[0] );
    }
    OldSymName = NULL;
    NewSymName = NULL;
    CommentFile = NULL;
    newName = argv[1];
    Verbose = false;
    AppendPatchLevel = true;
    for( arg = argv + 4; (curr = *arg) != NULL; ++arg ) {
        if( *curr != '-' && *curr != '/' )
            Usage( argv[0] );
        ++curr;
        switch( tolower( curr[0] ) ) {
        case 's':
            SyncString = strdup( curr + 1 );
            break;
        case 'p':
            newName = curr + 1;
            break;
        case 'c':
            CommentFile = curr + 1;
            break;
        case 'v':
            Verbose = true;
            break;
        case 'l':
            AppendPatchLevel = false;
            break;
        case 'd':
            if( tolower( curr[1] ) == 'o' ) {
                OldSymName = curr + 2;
                break;
            } else if( tolower( curr[1] ) == 'n' ) {
                NewSymName = curr + 2;
                break;
            }
            /* fall through */
        default:
            Usage( argv[0] );
            break;
        }
    }
    alg = ALG_NOTHING;
#ifdef USE_DBGINFO
    if( NewSymName && access( NewSymName, R_OK ) != -1 ) {
        alg = ALG_ONLY_NEW;
        if( OldSymName && access( OldSymName, R_OK ) != -1 ) {
            alg = ALG_BOTH;
        }
    }
#endif
    return( alg );
}


void main( int argc, char **argv )
{
    long        savings;
    foff        buffsize;
    algorithm   alg;

    if( !MsgInit() )
        exit( EXIT_FAILURE );
    alg = ParseArgs( argc, argv );

    EndOld = FileSize( argv[1], &OldCorrection );
    EndNew = FileSize( argv[2], &NewCorrection );

    buffsize = ( EndOld > EndNew ) ? ( EndOld ) : ( EndNew );
    buffsize += sizeof( PATCH_LEVEL );
    OldFile = ReadIn( argv[1], buffsize, EndOld );
    NewFile = ReadIn( argv[2], buffsize, EndNew );

    ScanSyncString( SyncString );

    switch( alg ) {
    case ALG_NOTHING:
        FindRegions();
        break;
#ifdef USE_DBGINFO
    case ALG_ONLY_NEW:
    case ALG_BOTH:
        SymbolicDiff( alg, argv[1], argv[2] );
        break;
#endif
    }

    if( NumHoles == 0 && DiffSize == 0 && EndOld == EndNew ) {
        printf( "Patch file not created - files are identical\n" );
        MsgFini();
        exit( EXIT_SUCCESS );
    }
    MakeHoleArray();
    SortHoleArray();
    ProcessHoleArray( 0 );
    savings = HolesToDiffs();
    WritePatchFile( argv[3], newName );
    FreeHoleArray();
    VerifyCorrect( argv[2] );

    print_stats( savings );

    MsgFini();
    exit( EXIT_SUCCESS );
}

