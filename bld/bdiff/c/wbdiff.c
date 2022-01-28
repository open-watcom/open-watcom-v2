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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "bdiff.h"
#include "diff.h"
#include "wpatch.h"
#include "msg.h"


byte    *PatchBuffer;
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

int DoBdiff( const char *srcPath, const char *tgtPath, const char *new_name, const char *name, algorithm alg )
{
    long        savings;
    foff        buffsize;

    EndOld = FileSize( srcPath, &OldCorrection );
    EndNew = FileSize( tgtPath, &NewCorrection );

    buffsize = ( EndOld > EndNew ) ? ( EndOld ) : ( EndNew );
    buffsize += sizeof( PATCH_LEVEL );
    OldFile = ReadIn( srcPath, buffsize, EndOld );
    NewFile = ReadIn( tgtPath, buffsize, EndNew );

    ScanSyncString( SyncString );

    FindRegionsAlg( alg );

    if( NumHoles == 0 && DiffSize == 0 && EndOld == EndNew ) {
        puts( "Patch file not created - files are identical" );
        return( 1 );
    }
    MakeHoleArray();
    SortHoleArray();
    ProcessHoleArray( 0 );
    savings = HolesToDiffs();
    WritePatchFile( name, new_name );
    FreeHoleArray();
    VerifyCorrect( tgtPath );

    print_stats( savings );

    return( 0 );
}
