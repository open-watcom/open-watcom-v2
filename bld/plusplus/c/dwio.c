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
#include "errdefns.h"
#include "dwio.h"
#include "memmgr.h"
#include "ring.h"
#include "dwiobuff.h"
#include "carve.h"


static DWFILE *dw_file_ring;    // ring of virtual files


void DwioInit(                  // INITIALIZE FOR DW-IO
    void )
{
    dw_file_ring = NULL;
    DwioBuffInit();
}


void DwioFini(                  // FINALIZE FOR DW-IO
    void )
{
    DwioBuffFini();
    RingFree( &dw_file_ring );
}


DWFILE *DwioCreateFile(         // CREATE A DW VIRTUAL FILE
    void )
{
    DWFILE *file_element;       // - allocated file element
    DWIOBUFF *buff_element;     // - buffer element

    file_element = RingAlloc( &dw_file_ring, sizeof( DWFILE ) );
    buff_element = DwioBuffWrOpen();
    file_element->buffer = buff_element;
    file_element->first = buff_element->disk_addr;
    return( file_element );
}


void DwioWrite(                 // WRITE BYTE STREAM TO VIRTUAL FILE
    DWFILE *ctl,                // - control for the file
    void *data,                 // - data to be written
    size_t size )               // - data size
{
    ctl->buffer = DwioBuffWrite( ctl->buffer, data, size );
}


void DwioCloseOutputFile(       // CLOSE VIRTUAL FILE AFTER OUTPUT PHASE
    DWFILE *ctl )               // - control for file
{
    DwioBuffWrClose( ctl->buffer );
    ctl->buffer = NULL;
}


void DwioOpenInput(             // OPEN VIRTUAL FILE FOR INPUT
    DWFILE *ctl )               // - control for file
{
    ctl->buffer = DwioBuffRdOpen( ctl->first );
}


void *DwioRead(                 // READ A BYTE STREAM
    DWFILE *ctl,                // - control for the file
    size_t *size )              // - in  -> last size consumed
                                // - out -> new space avail
{
    void *ptr;
    ctl->buffer = DwioBuffRead( ctl->buffer, &ptr, size );
    return ptr;
}


void DwioCloseInputFile(        // CLOSE VIRTUAL FILE AFTER INPUT PHASE
    DWFILE *ctl )               // - control for file
{
    DwioBuffRdClose( ctl->buffer );
    ctl->buffer = NULL;
}


void DwioSeek(                  // POSITION TO BYTE OFFSET IN FILE
    DWFILE *ctl,                // - control for the file
    size_t offset )             // - offset to seek to
{
    ctl->buffer = DwioBuffSeek( ctl->buffer, ctl->first, offset );
}


void DwioFreeFile(              // FREE A VIRTUAL FILE
    DWFILE *ctl )               // - control for file
{
    RingDealloc( &dw_file_ring, ctl );
}
