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
#include "diff.h"

byte    *PatchFile;
byte    *OldFile;
byte    *NewFile;

bool    AppendPatchLevel;
bool    Verbose;

char    *OldSymName;
char    *NewSymName;

char    *CommentFile;

int     OldCorrection;
int     NewCorrection;

static const char   *SyncString = NULL;
static region       *SimilarRegions;
static region       *DiffRegions;
static region       *HoleRegions;
static foff         SimilarSize;
static foff         NumDiffs;
static foff         HolesInRegion;
static foff         HoleCount[3];
static foff         HoleHeaders;

static const char   *newName;

int DoBdiff( const char *srcPath, const char *tgtPath, const char *name )
{
    long        savings;
    foff        buffsize;
    int         i;
//    if( !MsgInit() ) exit( EXIT_FAILURE );
    /* initialize static variables each time */
    SimilarRegions = NULL;
    DiffRegions = NULL;
    HoleRegions = NULL;
    SimilarSize = 0;
    NumHoles = 0;
    NumDiffs = 0;
    DiffSize = 0;
    HolesInRegion = 0;
    HoleHeaders = 0;
    for( i = 0; i < 3; i += 1 ) {
        HoleCount[i] = 0;
    }

    init_diff();

    newName = name;
    EndOld = FileSize( srcPath, &OldCorrection );
    EndNew = FileSize( tgtPath, &NewCorrection );

    buffsize = ( EndOld > EndNew ) ? ( EndOld ) : ( EndNew );
    buffsize += sizeof( PATCH_LEVEL );
    OldFile = ReadIn( srcPath, buffsize, EndOld );
    NewFile = ReadIn( tgtPath, buffsize, EndNew );

    ScanSyncString( SyncString );

    FindRegions();

    if( NumHoles == 0 && DiffSize == 0 && EndOld == EndNew ) {
        printf( "Patch file not created - files are identical\n" );
        MsgFini();
        exit( EXIT_SUCCESS );
    }
    MakeHoleArray();
    SortHoleArray();
    ProcessHoleArray( 0 );
    savings = HolesToDiffs();
    WritePatchFile( "", newName );
    FreeHoleArray();
    VerifyCorrect( tgtPath );

    print_stats( savings );

    MsgFini();
    return ( EXIT_SUCCESS );
}


