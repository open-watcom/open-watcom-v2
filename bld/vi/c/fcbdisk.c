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


#include <stdio.h>
#include <stdlib.h>
#include "posix.h"
#include <fcntl.h>
#include "vi.h"
#include "fcbmem.h"

static char swapFileName[L_tmpnam];

static int swapFileOpen( void );
static int getNewSwapFilePosition( long * );
static int swapFileWrite( long *, int );

/*
 * SwapToDisk - swap an fcb to disk from memory
 */
int SwapToDisk( fcb *fb )
{
    int i,len;

    /*
     * set up data
     */
    i = swapFileOpen();
    if( i ) {
        return( i );
    }
    len = MakeWriteBlock( fb );

    /*
     * now write the buffer
     */
    i = swapFileWrite( &(fb->offset), len );
    if( i ) {
        return( i );
    }

    /*
     * finish up
     */
    fb->swapped = TRUE;
    return( ERR_NO_ERR );

} /* SwapToDisk */

/*
 * SwapToMemoryFromDisk - swap an fcb to memory from disk
 */
int SwapToMemoryFromDisk( fcb *fb )
{
    int len,i,expect;

    /*
     * prepare swap file
     */
    i = swapFileOpen();
    if( i ) {
        return( i );
    }
    i = FileSeek( SwapFileHandle, fb->offset );
    if( i ) {
        return( i );
    }

    /*
     * read in the buffer, create lines
     */
    expect = FcbSize( fb );
    len = read( SwapFileHandle, ReadBuffer, expect );
    if( len != expect ) {
        return( ERR_SWAP_FILE_READ );
    }

    return( RestoreToNormalMemory( fb, len ) );

} /* SwapToMemoryFromDisk */

/*
 * swapFileOpen - do just that
 */
static int swapFileOpen( void )
{
    int i;

    if( SwapFileHandle >= 0 ) {
        return( ERR_NO_ERR );
    }

    i = TmpFileOpen( swapFileName, &SwapFileHandle );
    if( i ) {
        return( ERR_SWAP_FILE_OPEN );
    }

    return( ERR_NO_ERR );

} /* swapFileOpen */

/*
 * SwapFileClose - do just that
 */
void SwapFileClose( void )
{
    TmpFileClose( SwapFileHandle, swapFileName );

} /* SwapFileClose */

/*
 * GiveBackSwapBlock - return a block to the swap file
 */
void GiveBackSwapBlock( long offset )
{
    GiveBackBlock( offset, SwapBlocks );
    SwapBlocksInUse--;
} /* GiveBackSwapBlock */

/*
 * getNewSwapFilePosition - find free space in swap file
 */
static int getNewSwapFilePosition( long *p )
{
    if( !GetNewBlock( p, SwapBlocks, SwapBlockArraySize ) ) {
        return( ERR_SWAP_FILE_FULL );
    }
    SwapBlocksInUse++;
    return( ERR_NO_ERR );
} /* getNewSwapFilePosition */

/*
 * swapFileWrite - write data to swap file
 */
static int swapFileWrite( long *pos, int size )
{
    int i;

    /*
     * get position to write data
     */
    if( *pos < 0 ) {
        i = getNewSwapFilePosition( pos );
        if( i ) {
            return( i );
        }
    }
    i = FileSeek( SwapFileHandle, *pos );
    if( i ) {
        return( i );
    }

    /*
     * write data
     */
    i = write( SwapFileHandle, WriteBuffer, size );
    if( i != size ) {
        return( ERR_SWAP_FILE_WRITE );
    }
    return( ERR_NO_ERR );

} /* swapFileWrite */

/*
 * SwapBlockInit
 */
void SwapBlockInit( int i )
{
    int k;

    if( SwapBlocks == NULL ) {
        MaxSwapBlocks = i;
        MaxSwapBlocks /= (MAX_IO_BUFFER/1024);
        SwapBlockArraySize = MaxSwapBlocks >> 3;
        SwapBlocks = MemAlloc( SwapBlockArraySize+1 );
        for( k=0;k< SwapBlockArraySize;k++ ) {
            SwapBlocks[k] = 0xff;
        }
    }

} /* SwapBlockInit */


/*
 * SwapBlockFini
 */
void SwapBlockFini( void )
{
    if( SwapBlocks != NULL ) {
        MemFree( SwapBlocks );
    }
} /* SwapBlockFini */
