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
* Description:  Description of buffer for C++ I/O to intermediate-code file.
*
****************************************************************************/


#ifndef __CGIOBUFF_H__
#define __CGIOBUFF_H__

#include "iosupp.h"

#define CGIO_CRIPPLE_LIMIT      5000

typedef struct cgiobuff CGIOBUFF;
struct cgiobuff {               // CGIOBUFF -- buffer in memory
    CGIOBUFF    *next;          // - next in the ring
    CGIOBUFF    *reuse_next;    // - links for reuse doubly-linked ring
    CGIOBUFF    *reuse_prev;    // - ...
    DISK_ADDR   disk_addr;      // - block number in IC file
    DISK_OFFSET free_offset;    // - free offset
    unsigned    reading;        // - number of items being read from block
    unsigned    written : 1;    // - block has been written (not dirty)
    unsigned    writing : 1;    // - being written to now
    unsigned    active : 1;     // - block is active
    char        data[TMPBLOCK_BSIZE]; // - buffer in memory
#ifndef NDEBUG

    unsigned    check;          // - consistency check
#endif
};

#if _INTEL_CPU
typedef uint_8                  CGINTEROP;  // opcode size
#elif _CPU == _AXP
typedef unsigned                CGINTEROP;  // opcode size
#else
#error bad _CPU
#endif

typedef struct cgfile_ins CGFILE_INS;
struct cgfile_ins               // CGFILE_INS -- location of instruction
{   DISK_ADDR block;            // - file block
    DISK_OFFSET offset;         // - offset in the block
};

#define CGINTER_BLOCKING        (sizeof(unsigned))

// structure is written out to a file
#include <pushpck1.h>

// having 'value' first means it will be aligned in cases
// were it is a singleton
typedef struct cginter          CGINTER;    // intermediate text
struct cginter {                // CGINTER -- intermediate-code instruction
    CGVALUE     value;          // - value
    CGINTEROP   opcode;         // - opcode for text
};

#include <poppck.h>

typedef void *CGIRELOCFN( void * );

// PROTOTYPES :

void CgioBuffFini(              // BUFFERING COMPLETION
    void )
;
void CgioBuffInit(              // BUFFERING INITIALIZATION
    void )
;
void CgioBuffRdClose(           // RELEASE BUFFER AFTER READING
    CGIOBUFF *ctl )             // - buffer control
;
CGIOBUFF *CgioBuffRdOpen(       // GET BUFFER FOR READING
    DISK_ADDR block )           // - disk address
;
CGIOBUFF *CgioBuffSeek(         // SEEK TO POSITION
    CGIOBUFF *ctl,              // - buffer control
    CGFILE_INS *posn,           // - position to seek to
    CGINTER **ins )             // - cursor to update
;
CGFILE_INS CgioBuffLastRead(    // RETURN POSITION OF LAST READ
    CGIOBUFF *ctl,              // - buffer control
    CGINTER *ins )              // - current cursor
;
CGFILE_INS CgioBuffLastWrite(   // RETURN POSITION OF LAST WRITE
    CGIOBUFF *ctl )             // - buffer control
;
CGIOBUFF *CgioBuffReadIC(       // READ A RECORD
    CGIOBUFF *ctl,              // - buffer control
    CGINTER **ins )             // - cursor to update
;
CGIOBUFF *CgioBuffReadICUntilOpcode(       // READ A RECORD UNTIL OPCODE IS FOUND
    CGIOBUFF *ctl,              // - buffer control
    CGINTER **ins,              // - cursor to update
    unsigned opcode )           // - opcode to find
;
CGIOBUFF *CgioBuffReadICMask(   // READ A RECORD UNTIL OPCODE IN SET IS FOUND
    CGIOBUFF *ctl,              // - buffer control
    CGINTER **ins,              // - cursor to update
    unsigned mask )             // - control mask for opcodes
;
CGIOBUFF *CgioBuffReadICMaskCount(      // READ A RECORD UNTIL OPCODE IN SET IS FOUND
    CGIOBUFF *ctl,              // - buffer control
    CGINTER **ins,              // - cursor to update
    unsigned mask,              // - control mask for opcodes to return
    unsigned count_mask,        // - control mask for opcodes to count
    unsigned *count )           // - counter to update
;
void CgioBuffWrClose(           // RELEASE BUFFER AFTER WRITING
    CGIOBUFF *ctl )             // - buffer control
;
CGIOBUFF *CgioBuffWrOpen(       // GET BUFFER FOR WRITING
    void )
;
CGIOBUFF *CgioBuffWriteIC(      // WRITE AN IC RECORD
    CGIOBUFF *ctl,              // - buffer control
    CGINTER *ins )              // - IC to write
;
CGINTER *CgioBuffPCHRead(       // READ FROM PCH AND WRITE INTO BUFFER
    CGINTER *buff,              // - alternate buffer for PCHReadLoc
    CGIOBUFF **pctl )           // - addr( buffer control )
;
void CgioBuffZap(               // ZAP A WRITTEN AREA OF A BUFFER
    CGFILE_INS zap,             // - zap location
    CGINTER *ins )              // - instr to be written
;

#include "ioptypes.h"
extern ic_op_type ICOpTypes[];

#endif
