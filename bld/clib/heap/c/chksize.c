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


#include "variety.h"
#include <malloc.h>
#include "heap.h"

// This file is compiled to ensure that important structures don't change.
// The following sizes are important to maintaining the alignment of the heap
#define SIZEOF_FREELIST     (TAG_SIZE*3)
#define SIZEOF_FREELISTP    SIZEOF_FREELIST
#define SIZEOF_HEAPBLK      (TAG_SIZE*8+SIZEOF_FREELIST)
#define SIZEOF_HEAPBLKP     SIZEOF_HEAPBLK
#define SIZEOF_MINIHEAPBLKP SIZEOF_HEAPBLK
#define SIZEOF_HEAPSTART    (SIZEOF_HEAPBLK + SIZEOF_FREELIST)
#define SIZEOF_HEAPEND      (TAG_SIZE + SIZEOF_FREELIST)
#ifdef __DOS_EXT__
#define SIZEOF_DPMI_HDR     (sizeof(unsigned long) + TAG_SIZE)
#endif

// declare an two arrays for each structure
// one array to test if the structure has grown and one to see if the
// structure has shrunk
char __freelist_shrunk[ sizeof( struct freelist ) - SIZEOF_FREELIST + 1];
char __freelist_bigger[ SIZEOF_FREELIST - sizeof( struct freelist ) + 1];
char __freelistp_shrunk[ sizeof( struct freelistp ) - SIZEOF_FREELISTP + 1];
char __freelistp_bigger[ SIZEOF_FREELISTP - sizeof( struct freelistp ) + 1];
char __heapblk_shrunk[ sizeof( struct heapblk ) - SIZEOF_HEAPBLK + 1];
char __heapblk_bigger[ SIZEOF_HEAPBLK - sizeof( struct heapblk ) + 1];
char __heapblkp_shrunk[ sizeof( struct heapblkp ) - SIZEOF_HEAPBLKP + 1];
char __heapblkp_bigger[ SIZEOF_HEAPBLKP - sizeof( struct heapblkp ) + 1];
char __miniheapblkp_shrunk[ sizeof( struct miniheapblkp ) - SIZEOF_MINIHEAPBLKP + 1];
char __miniheapblkp_bigger[ SIZEOF_MINIHEAPBLKP - sizeof( struct miniheapblkp ) + 1];
char __heapstart_shrunk[ sizeof( struct heapstart ) - SIZEOF_HEAPSTART + 1];
char __heapstart_bigger[ SIZEOF_HEAPSTART - sizeof( struct heapstart ) + 1];
char __heapend_shrunk[ sizeof( struct heapend ) - SIZEOF_HEAPEND + 1];
char __heapend_bigger[ SIZEOF_HEAPEND - sizeof( struct heapend ) + 1];
#ifdef __DOS_EXT__
char __dpmi_hdr_shrunk[ sizeof( struct dpmi_hdr ) - SIZEOF_DPMI_HDR + 1];
char __dpmi_hdr_bigger[ SIZEOF_DPMI_HDR - sizeof( struct dpmi_hdr ) + 1];
#endif
