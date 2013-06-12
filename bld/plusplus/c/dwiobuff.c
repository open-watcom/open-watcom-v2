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


#include "plusplus.h"

#include <sys/types.h>
#include "wio.h"

#include "memmgr.h"
#include "iosupp.h"
#include "dwiobuff.h"
#include "ring.h"
#include "toggle.h"
#include "dbg.h"


static DWIOBUFF *activeBufs;    // active buffers
static DWIOBUFF *freeBufs;      // free buffers
static DWIOBUFF *beingWritten;  // buffer being written


static void deactivateBuffer(   // DEACTIVATE A BUFFER FROM ACTIVE RING
    DWIOBUFF *ctl )             // - buffer control
{
    RingPrune( &activeBufs, ctl );
    RingAppend( &freeBufs, ctl );
}


static void activateBuffer(     // ACTIVATE A BUFFER FROM FREE RING
    DWIOBUFF *ctl )             // - buffer control
{
    RingPrune( &freeBufs, ctl );
    RingAppend( &activeBufs, ctl );
}


static DWIOBUFF *allocateBuffer(// ALLOCATE A NEW BUFFER
    void )
{
    DWIOBUFF *ctl;              // - new buffer

    ctl = CMemAlloc( sizeof( *ctl ) );
    ctl->next = NULL;
    ctl->disk_addr = 0;
    ctl->next_addr = 0;
    ctl->prev_addr = 0;
    ctl->current_offset = 0;
    ctl->block_num = 0;
    ctl->reading = 0;
    ctl->written = FALSE;
    ctl->writing = FALSE;
    RingAppend( &activeBufs, ctl );
    return ctl;
}


static void *pointXfer(         // POINT AT TRANSFER POINT
    DWIOBUFF *ctl )             // - buffer control
{
    return (char*)ctl->data + ctl->current_offset;
}


static DWIOBUFF *findWrBuffer(  // FIND A BUFFER FOR WRITING
    void )
{
    DWIOBUFF *ctl;              // - buffer control

    ctl = allocateBuffer();
    ctl->disk_addr = IoSuppTempNextBlock( DWBLOCK_FACTOR );
    ctl->writing = TRUE;
    ctl->written = FALSE;
    return ctl;
}


static DWIOBUFF *locateRdBuffer(// LOCATE A READ BUFFER IN A RING
    DWIOBUFF *ring,             // - ring of buffers
    DISK_ADDR disk_addr )       // - disk block for buffer
{
    DWIOBUFF *ctl;              // - buffer control (current buffer)
    DWIOBUFF *found;            // - buffer control (located buffer)

    found = NULL;
    RingIterBeg( ring, ctl ) {
        if( ctl->disk_addr == disk_addr ) {
            found = ctl;
            break;
        }
    } RingIterEnd( ctl )
    return found;
}


static void savePrevNext( DWIOBUFF *ctl )
{
    DWIOBUFF_EXTRA *p;

    p = (DWIOBUFF_EXTRA *) ( ctl->data + DWBLOCK_BSIZE );
    p->prev_addr = ctl->prev_addr;
    p->next_addr = ctl->next_addr;
    p->block_num = ctl->block_num;
}


static void extractPrevNext( DWIOBUFF *ctl )
{
    DWIOBUFF_EXTRA *p;

    p = (DWIOBUFF_EXTRA *) ( ctl->data + DWBLOCK_BSIZE );
    ctl->prev_addr = p->prev_addr;
    ctl->next_addr = p->next_addr;
    ctl->block_num = p->block_num;
}


static DWIOBUFF *findBuffer(    // FIND A SPECIFIC BUFFER
    DISK_ADDR block )           // - disk block
{
    DWIOBUFF *ctl;              // - buffer control

    ctl = locateRdBuffer( activeBufs, block );
    if( ctl == NULL ) {
        ctl = locateRdBuffer( freeBufs, block );
        if( ctl == NULL ) {
            ctl = allocateBuffer();
            IoSuppTempRead( block
                          , TMPBLOCK_BSIZE * DWBLOCK_FACTOR
                          , ctl->data );
            extractPrevNext( ctl );
            ctl->disk_addr = block;
            ctl->written = TRUE;
        } else {
            activateBuffer( ctl );
        }
    }
    return ctl;
}


static DWIOBUFF *findRdBuffer(  // FIND A BUFFER FOR READING
    DISK_ADDR block )           // - disk block
{
    DWIOBUFF *ctl;
    ctl = findBuffer( block );
    ++ctl->reading;
    ctl->current_offset = 0;
    return ctl;
}


static DWIOBUFF *findReWrBuffer(// FIND BUFFER FOR RE-WRITE
    DISK_ADDR block )           // - disk block
{
    DWIOBUFF *ctl;
    ctl = findBuffer( block );
    ctl->writing = TRUE;
    return ctl;
}


static void finishWrBuffer(     // COMPLETE WRITE-USE OF A BUFFER
    DWIOBUFF *ctl )             // - buffer control
{
    ctl->writing = FALSE;
    deactivateBuffer( ctl );
}


static void finishRdBuffer(     // COMPLETE READ-USE OF A BUFFER
    DWIOBUFF *ctl )             // - buffer control
{
    -- ctl->reading;
    if( ( ctl->reading == 0 ) && ! ctl->writing ) {
        deactivateBuffer( ctl );
    }
}


DWIOBUFF *DwioBuffWrOpen(       // GET BUFFER FOR WRITING
    void )
{
    return findWrBuffer();
}


DWIOBUFF *DwioBuffWrite(        // WRITE A RECORD
    DWIOBUFF *ctl,              // - buffer control
    void *record,               // - record to be written
    size_t size )               // - record size
{
    DWIOBUFF *next;             // - next buffer control
    size_t len;                 // - amount to write

    for(;;) {
        len = min( size, DWBLOCK_BSIZE - ctl->current_offset );
        memcpy( pointXfer( ctl ), record, len );
        ctl->written = FALSE;
        ctl->current_offset += len;
        record = (void *)((char *)record + len);
        size -= len;
        if( size == 0 ) break;
        if( ctl->next_addr == 0 ) {
            next = findWrBuffer();
            ctl->next_addr = next->disk_addr;
            next->prev_addr = ctl->disk_addr;
            next->block_num = ctl->block_num + 1;
        } else {
            next = findReWrBuffer( ctl->next_addr );
        }
        next->current_offset = 0;
        finishWrBuffer( ctl );
        ctl = next;
    }
    return ctl;
}


DWIOBUFF *DwioBuffSeek(         // POSITION TO SPECIFIED OFFSET FROM START
    DWIOBUFF *ctl,              // - current buffer control
    DISK_ADDR block,            // - starting disk address
    size_t offset )             // - where to position to
{
    DWIOBUFF *next;             // - next buffer control
    size_t required_block;
    size_t required_offset;

    required_block = offset / DWBLOCK_BSIZE;
    required_offset = offset % DWBLOCK_BSIZE;
    while( ctl->block_num != required_block ) {
        if( ctl->block_num < required_block ) {
            if( ctl->next_addr == 0 ) {
                next = findWrBuffer();
                ctl->next_addr = next->disk_addr;
                next->prev_addr = ctl->disk_addr;
                next->block_num = ctl->block_num + 1;
            } else {
                next = findReWrBuffer( ctl->next_addr );
            }
        } else {
            if( (ctl->block_num - required_block) > required_block ) {
                next = findReWrBuffer( block );
            } else {
                if( ctl->prev_addr == 0 ) {
                    DbgStmt( CFatal( "dwiobuff: attempt to seek off start of file" ) );
                } else {
                    next = findReWrBuffer( ctl->prev_addr );
                }
            }
        }
        finishWrBuffer( ctl );
        ctl = next;
    }
    ctl->current_offset = required_offset;
    return( ctl );
}


void DwioBuffWrClose(           // RELEASE BUFFER AFTER WRITING
    DWIOBUFF *ctl )             // - buffer control
{
    finishWrBuffer( ctl );
}


DWIOBUFF *DwioBuffRdOpen(       // GET BUFFER FOR READING
    DISK_ADDR block )           // - disk address
{
    DWIOBUFF *ctl;

    ctl = findRdBuffer( block );
    ctl->current_offset = 0;
    return ctl;
}


DWIOBUFF *DwioBuffRead(         // READ A RECORD
    DWIOBUFF *ctl,              // - buffer control
    void **record,              // - record to be read
    size_t *size )              // - in  -> last size consumed
                                // - out -> new space avail
{
    char     *ptr;              // - current pointer
    size_t   new_size;          // - space still available in buffer
    DWIOBUFF *next;             // - buffer control (next buffer)

    ctl->current_offset += *size;
    ptr = (char *)ctl->data + ctl->current_offset;
    new_size = DWBLOCK_BSIZE - ctl->current_offset;
    if( new_size == 0 ) {
        next = findRdBuffer( ctl->next_addr );
        ptr = (char *)next->data;
        finishRdBuffer( ctl );
        ctl = next;
        new_size = (ctl->data+DWBLOCK_BSIZE) - ptr;
    }
    *size = new_size;
    *record = (void *)ptr;
    return ctl;
}


void DwioBuffRdClose(           // RELEASE BUFFER AFTER READING
    DWIOBUFF *ctl )             // - buffer control
{
    finishRdBuffer( ctl );
}

static void dwioBuffReleaseMemory(  // RELEASE MEMORY
    void )
{
    DWIOBUFF *buf;              // - current buffer

    for(;;) {
        buf = RingPop( &freeBufs );
        if( buf == NULL ) break;
        if( ! buf->written
         && ! CompFlags.compile_failed ) {
            savePrevNext( buf );
            beingWritten = buf;
            IoSuppTempWrite( buf->disk_addr
                           , TMPBLOCK_BSIZE * DWBLOCK_FACTOR
                           , buf->data );
            beingWritten = NULL;
        }
        CMemFree( buf );
    }
}

void DwioBuffInit(              // BUFFERING INITIALIZATION
    void )
{
    beingWritten = NULL;
    CMemRegisterCleanup( dwioBuffReleaseMemory );
}

void DwioBuffFini(              // BUFFERING COMPLETION
    void )
{
    if( NULL != beingWritten ) {
        CMemFreePtr( &beingWritten );
    }
    RingFree( &freeBufs );
}
