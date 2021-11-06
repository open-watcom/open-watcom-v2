/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Ensure that important structure sizes stay in sync
*
****************************************************************************/


#include "variety.h"
#include <malloc.h>
#include "heap.h"


// This file is compiled to ensure that important structures don't change.
// The following sizes are important to maintaining the alignment of the heap
#define SIZEOF_FREELIST     (TAG_SIZE * 3)
#if defined( __OS2__ ) && !defined( _M_I86 )
// 32-bit OS/2 has one more for high memory flags - was 8
#define SIZEOF_HEAPBLK      (TAG_SIZE * 9 + SIZEOF_FREELIST)
#else
#define SIZEOF_HEAPBLK      (TAG_SIZE * 8 + SIZEOF_FREELIST)
#endif
#define SIZEOF_HEAPSTART    (SIZEOF_HEAPBLK + SIZEOF_FREELIST)
#define SIZEOF_HEAPEND      (TAG_SIZE + SIZEOF_FREELIST)
#ifdef __DOS_EXT__
#define SIZEOF_DPMI_HDR     (sizeof( unsigned long ) + TAG_SIZE)
#endif

// declare an two arrays for each structure
// one array to test if the structure has grown and one to see if the
// structure has shrunk
char __freelist_shrunk[sizeof( freelist ) - SIZEOF_FREELIST + 1];
char __freelist_bigger[SIZEOF_FREELIST - sizeof( freelist ) + 1];
char __heapblk_shrunk[sizeof( heapblk ) - SIZEOF_HEAPBLK + 1];
char __heapblk_bigger[SIZEOF_HEAPBLK - sizeof( heapblk ) + 1];
char __heapstart_shrunk[sizeof( heapstart ) - SIZEOF_HEAPSTART + 1];
char __heapstart_bigger[SIZEOF_HEAPSTART - sizeof( heapstart ) + 1];
char __heapend_shrunk[sizeof( heapend ) - SIZEOF_HEAPEND + 1];
char __heapend_bigger[SIZEOF_HEAPEND - sizeof( heapend ) + 1];
#ifdef __DOS_EXT__
char __dpmi_hdr_shrunk[sizeof( dpmi_hdr ) - SIZEOF_DPMI_HDR + 1];
char __dpmi_hdr_bigger[SIZEOF_DPMI_HDR - sizeof( dpmi_hdr ) + 1];
#endif
