/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include "wpatch.h"


enum {
    MSG_USAGE_COUNT = 0
    #define pick(c,e,j) + 1
    #include "dusage.gh"
    #undef pick
};

static const char   *SyncString = NULL;

static char         *newName;

static char         *SrcPath;
static char         *TgtPath;

static void Usage( void )
{
    char msgbuf[MAX_RESOURCE_SIZE];
    int i;

    for( i = MSG_USAGE_BASE; i < MSG_USAGE_BASE + MSG_USAGE_COUNT; i++ ) {
        GetMsg( msgbuf, i );
        puts( msgbuf );
    }
    MsgFini();
    exit( EXIT_FAILURE );
}

static algorithm ParseArgs( int argc, char **argv )
{
    char        **arg;
    char        *curr;
    algorithm   alg;

    newName = NULL;
    if( argc < 4 ) {
        Usage();
    }
    OldSymName = NULL;
    NewSymName = NULL;
    CommentFile = NULL;
    newName = argv[1];
    Verbose = false;
    AppendPatchLevel = true;
    for( arg = argv + 4; (curr = *arg) != NULL; ++arg ) {
        if( *curr != '-' && *curr != '/' )
            Usage();
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
            Usage();
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


void FindRegionsAlg( algorithm alg )
{
    switch( alg ) {
    case ALG_NOTHING:
        FindRegions();
        break;
#ifdef USE_DBGINFO
    case ALG_ONLY_NEW:
    case ALG_BOTH:
        SymbolicDiff( alg, SrcPath, TgtPath );
        break;
#endif
    }
}


int main( int argc, char **argv )
{
    int         rc;
    algorithm   alg;

    if( !MsgInit() )
        return( EXIT_FAILURE );
    alg = ParseArgs( argc, argv );
    SrcPath = argv[1];
    TgtPath = argv[2];
    rc = DoBdiff( SrcPath, TgtPath, newName, argv[3], alg );
    MsgFini();
    return( rc );
}
