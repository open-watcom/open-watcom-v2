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


#ifndef __DWIO_H__
#define __DWIO_H__

// DWIO.H -- C++ definitions related to code-generation files
//
// 92/09/21 -- Greg Bentz       -- cloned from cgio.h

#include <unistd.h>

#include "dwiobuff.h"

// STRUCTURES:

typedef struct dwfile           DWFILE;         // control for a virtual file

struct dwfile                   // DWFILE -- control for a virtual file
{   DWFILE *next;               // - next virtual file
    DISK_ADDR first;            // - first block
    DWIOBUFF *buffer;           // - current buffer in memory
};

// PROTOTYPES:

void DwioCloseInputFile(        // CLOSE VIRTUAL FILE AFTER INPUT PHASE
    DWFILE *ctl )               // - control for file
;
void DwioCloseOutputFile(       // CLOSE VIRTUAL FILE AFTER OUTPUT PHASE
    DWFILE *ctl )               // - control for file
;
DWFILE *DwioCreateFile(         // CREATE A DW VIRTUAL FILE
    void )
;
void DwioFini(                  // FINALIZE FOR DW-IO
    void )
;
void DwioFreeFile(              // FREE A VIRTUAL FILE
    DWFILE *ctl )               // - control for file
;
void DwioInit(                  // INITIALIZE FOR DW-IO
    void )
;
void DwioOpenInput(             // OPEN VIRTUAL FILE FOR INPUT
    DWFILE *ctl )               // - control for file
;
void *DwioRead(                 // READ A BYTE STREAM
    DWFILE *ctl,                // - control for the file
    size_t *size )              // - in  -> last size consumed
                                // - out -> new space avail
;
void DwioSeek(                  // POSITION TO BYTE OFFSET IN FILE
    DWFILE *ctl,                // - control for the file
    size_t offset )             // - offset to seek to
;
void DwioWrite(                 // WRITE BYTE STREAM TO VIRTUAL FILE
    DWFILE *ctl,                // - control for the file
    void *data,                 // - data to be written
    size_t size )               // - data size
;

#endif
