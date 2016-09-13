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
* Description:  FCB disk swapping routines.
*
****************************************************************************/


#include "vi.h"
#include "posix.h"
#include <fcntl.h>
#include "fcbmem.h"

#include "clibext.h"


#define SWAP_FILE_NAME  "swXXXXXX"

static vi_rc    swapFileOpen( void );
static vi_rc    getNewSwapFilePosition( long * );
static vi_rc    swapFileWrite( long *, int );

static char     swapFileName[sizeof( SWAP_FILE_NAME )];
static int      swapFileHandle = -1;

/*
 * SwapToDisk - swap an fcb to disk from memory
 */
vi_rc SwapToDisk( fcb *fb )
{
    int     len;
    vi_rc   rc;

    /*
     * set up data
     */
    rc = swapFileOpen();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    len = MakeWriteBlock( fb );

    /*
     * now write the buffer
     */
    rc = swapFileWrite( &(fb->offset), len );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    /*
     * finish up
     */
    fb->swapped = true;
    return( ERR_NO_ERR );

} /* SwapToDisk */

/*
 * SwapToMemoryFromDisk - swap an fcb to memory from disk
 */
vi_rc SwapToMemoryFromDisk( fcb *fb )
{
    int     len, expect;
    vi_rc   rc;

    /*
     * prepare swap file
     */
    rc = swapFileOpen();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    rc = FileSeek( swapFileHandle, fb->offset );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    /*
     * read in the buffer, create lines
     */
    expect = FcbSize( fb );
    len = read( swapFileHandle, ReadBuffer, expect );
    if( len != expect ) {
        return( ERR_SWAP_FILE_READ );
    }

    return( RestoreToNormalMemory( fb, len ) );

} /* SwapToMemoryFromDisk */

/*
 * swapFileOpen - do just that
 */
static vi_rc swapFileOpen( void )
{
    char    file[_MAX_PATH];

    if( swapFileHandle == -1 ) {
        MakeTmpPath( file, SWAP_FILE_NAME );
        swapFileHandle = mkstemp( file );
        if( swapFileHandle == -1 ) {
            return( ERR_SWAP_FILE_OPEN );
        }
        memcpy( swapFileName, file + strlen( file ) - ( sizeof( SWAP_FILE_NAME ) - 1 ), sizeof( SWAP_FILE_NAME ) );
    }
    return( ERR_NO_ERR );

} /* swapFileOpen */

/*
 * SwapFileClose - do just that
 */
void SwapFileClose( void )
{
    char    file[_MAX_PATH];

    if( swapFileHandle != -1 ) {
        close( swapFileHandle );
        swapFileHandle = -1;
        MakeTmpPath( file, swapFileName );
        remove( file );
    }

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
static vi_rc getNewSwapFilePosition( long *p )
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
static vi_rc swapFileWrite( long *pos, int size )
{
    int     i;
    vi_rc   rc;

    /*
     * get position to write data
     */
    if( *pos < 0 ) {
        rc = getNewSwapFilePosition( pos );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }
    rc = FileSeek( swapFileHandle, *pos );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    /*
     * write data
     */
    i = write( swapFileHandle, WriteBuffer, size );
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
        EditVars.MaxSwapBlocks = i;
        EditVars.MaxSwapBlocks /= (MAX_IO_BUFFER / 1024);
        SwapBlockArraySize = EditVars.MaxSwapBlocks >> 3;
        SwapBlocks = MemAlloc( SwapBlockArraySize + 1 );
        for( k = 0; k < SwapBlockArraySize; k++ ) {
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
