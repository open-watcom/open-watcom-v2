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


#ifndef __DWIOBUFF_H__
#define __DWIOBUFF_H__

// DWIOBUFF.H -- description of buffer for C++ I/O to dwarf file
//
// 93/09/20 -- Greg Bentz       -- defined

#include "iosupp.h"

#define DWBLOCK_FACTOR  ( 2 )
#define DWBLOCK_IOSIZE  ( DWBLOCK_FACTOR * TMPBLOCK_BSIZE )
#define DWBLOCK_EXTRA   ( sizeof( DWIOBUFF_EXTRA ) )
#define DWBLOCK_BSIZE   ( DWBLOCK_IOSIZE - DWBLOCK_EXTRA )

typedef struct dwiobuff DWIOBUFF;
struct dwiobuff                 // DWIOBUFF -- buffer in memory
{   DWIOBUFF *next;             // - next in the ring
    DISK_ADDR disk_addr;        // - block number in IC file
    DISK_ADDR next_addr;        // - next block number in IC file
    DISK_ADDR prev_addr;        // - prev block number in IC file
    DISK_OFFSET current_offset; // - free offset
    unsigned block_num;         // - block number in chain of blocks
    unsigned reading;           // - number of items being read from block
    unsigned written : 1;       // - block has been written (not dirty)
    unsigned writing : 1;       // - being written to now
    char data[DWBLOCK_IOSIZE];  // - buffer in memory
};

// info that must be saved to disk when a buffer is freed
typedef struct dwiobuff_extra DWIOBUFF_EXTRA;
struct dwiobuff_extra {
    DISK_ADDR   next_addr;
    DISK_ADDR   prev_addr;
    unsigned    block_num;
};

// PROTOTYPES :

void DwioBuffFini(              // BUFFERING COMPLETION
    void )
;
void DwioBuffInit(              // BUFFERING INITIALIZATION
    void )
;
void DwioBuffRdClose(           // RELEASE BUFFER AFTER READING
    DWIOBUFF *ctl )             // - buffer control
;
DWIOBUFF *DwioBuffRdOpen(       // GET BUFFER FOR READING
    DISK_ADDR block )           // - disk address
;
DWIOBUFF *DwioBuffRead(         // READ A RECORD
    DWIOBUFF *ctl,              // - buffer control
    void **record,              // - record to be read
    size_t *size )              // - in  -> last size consumed
                                // - out -> new space avail
;
DWIOBUFF *DwioBuffSeek(         // POSITION TO SPECIFIED OFFSET FROM START
    DWIOBUFF *ctl,              // - current buffer control
    DISK_ADDR block,            // - starting disk address
    size_t offset )             // - where to position to
;
void DwioBuffWrClose(           // RELEASE BUFFER AFTER WRITING
    DWIOBUFF *ctl )             // - buffer control
;
DWIOBUFF *DwioBuffWrOpen(       // GET BUFFER FOR WRITING
    void )
;
DWIOBUFF *DwioBuffWrite(        // WRITE A RECORD
    DWIOBUFF *ctl,              // - buffer control
    void *record,               // - record to be written
    size_t size )               // - record size
;

#endif
