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


#include <string.h>
#include "heapwalk.h"

static void     **MemBlocks;
static DWORD    MemBlocksCnt;

static char     **StringTbl;

static char     *BufEnd;
static DWORD    BytesLeft;

#define MEM_BLOCK_SIZE          ( 6 * 1024 )

static BOOL AllocNewBuf( void )
{
    MemBlocks = MemReAlloc( MemBlocks, ( MemBlocksCnt + 1 ) * sizeof( void * ) );
    if( MemBlocks == NULL )
        return( FALSE );
    MemBlocks[ MemBlocksCnt ] = MemAlloc( MEM_BLOCK_SIZE );
    if( MemBlocks[ MemBlocksCnt ] == NULL )
        return( FALSE );
    BufEnd = MemBlocks[ MemBlocksCnt ];
    BytesLeft = MEM_BLOCK_SIZE;
    return( TRUE );
}

static BOOL AddString( msg_id msgid )
{
    int         len;

    if( BytesLeft < RCSTR_MAX_LEN ) {
        if( !AllocNewBuf() ) {
            return( FALSE );
        }
    }
    len = LoadString( Instance, msgid, BufEnd, RCSTR_MAX_LEN );
    if( len < 0 )
        len = 0;
    StringTbl[msgid] = BufEnd;
    BufEnd[len++] = '\0';
    BufEnd += len;
    BytesLeft -= len;
    return( TRUE );
}

BOOL InitStringTable( void )
{
    DWORD       i;
    HRSRC       rchdl;
    HGLOBAL     rcmemhdl;
    WORD        numstrings;

    rchdl = FindResource( Instance, "NUMBER_OF_STRINGS", RT_RCDATA );
    if( rchdl == NULL )
        return( FALSE );
    rcmemhdl = LoadResource( Instance, rchdl );
    if( rcmemhdl == NULL )
        return( FALSE );
    numstrings = *(WORD *)LockResource( rcmemhdl );
    FreeResource( rcmemhdl );
    StringTbl = MemAlloc( numstrings * sizeof( char * ) );
    if( StringTbl == NULL )
        return( FALSE );
    for( i = 0; i < numstrings; i++ ) {
        if( !AddString( i ) ) {
            return( FALSE );
        }
    }
    return( TRUE );
}


/*
 * GetRCString
 */

const char *HWGetRCString( UINT msgid )
{
    return( StringTbl[msgid] );
}

char *HWAllocRCString( UINT id )
{
    return( StringTbl[id] );
}

void HWFreeRCString( char *str )
{
    // the strings are not really allocated so don't free them
    str = str;
}

#if 0
int HWCopyRCString( UINT id, char *buf, int bufsize )
{
    strncpy( buf, StringTbl[ id ], bufsize );
    return( strlen( StringTbl[ id ] ) );
}
#endif
