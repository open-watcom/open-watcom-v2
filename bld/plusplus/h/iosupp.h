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

// IOSUPP.H -- i/o support
//
// 91/06/04 -- J.W.Welch        -- defined
// 92/12/29 -- B.J. Stecher     -- QNX support

#ifndef __IOSUPP_H__
#define __IOSUPP_H__

#include "ftype.h"


// TEMP FILE typedefs
typedef unsigned long           DISK_ADDR;      // disk address
typedef unsigned                DISK_OFFSET;    // offset in disk block
#define TMPBLOCK_BSIZE          (4096)          // - block size for temp file

// PROTOTYPES:

bool IoSuppCloseFile(           // CLOSE FILE IF OPENED
    FILE **file_ptr )           // - addr( file pointer )
;
#if 0
void IoSuppConsoleChr           // PRINT CHARACTER TO CONSOLE
    ( char chr )                // - output character
;
void IoSuppConsoleFlush(        // FLUSH CONSOLE BUFFER
    void )
;
void IoSuppConsoleLine(         // PRINT LINE TO CONSOLE
    char const * line )         // - output line
;
void IoSuppConsoleStr(          // PRINT STRING TO CONSOLE
    char const * line )         // - output line
;
#endif
char *IoSuppFullPath(           // GET FULL PATH OF FILE NAME (ALWAYS USE RET VALUE)
    char *name,                 // - input file name
    char *buff,                 // - output buffer
    unsigned size )             // - output buffer size
;
bool IoSuppOpenSrc(             // OPEN A SOURCE FILE (PRIMARY,HEADER)
    const char *file_name,      // - supplied file name
    src_file_type typ )         // - what type of file is it?
;
char *IoSuppOutFileName(        // BUILD AN OUTPUT NAME FROM SOURCE NAME
    out_file_type typ )         // - extension
;
void IoSuppSetBuffering(        // SET FULL BUFFERING FOR AN OPEN FILE
    FILE *fp,                   // - opened file
    size_t buf_size )           // - buffer size
;
void IoSuppSetLineBuffering(    // SET LINE BUFFERING FOR AN OPEN FILE
    FILE *fp,                   // - opened file
    size_t buf_size )           // - buffer size
;
DISK_ADDR IoSuppTempNextBlock(  // GET NEXT BLOCK NUMBER
    unsigned num_blocks )       // - number of blocks allocated
;
void IoSuppTempRead(            // READ FROM TEMPORARY FILE
    DISK_ADDR   block_num,      // - block within temp file
    size_t      block_size,     // - size of blocks
    void        *data )         // - buffer to read
;
void IoSuppTempWrite(           // WRITE TO TEMPORARY FILE
    DISK_ADDR   block_num,      // - block within temp file
    size_t      block_size,     // - size of blocks
    void        *data )         // - buffer to write
;
#endif
