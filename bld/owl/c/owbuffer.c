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


#include "owlpriv.h"

#ifndef NDEBUG
#define NUM_BINS                (16)
#define INITIAL_BIN_SIZE        ((8*1024)/16)
#else
#define NUM_BINS                16
#define INITIAL_BIN_SIZE        (8*1024)
#endif

// This is the crazy buffering scheme of the week: we keep 16 bins
// of a certain size hanging off of our buffer. When these fill up,
// we collapse all of them into a new bin, of exactly
// ( buffer->bin_size * NUM_BINS ) bytes in size, and use this new
// size for all our future bins.

typedef struct owl_buffer {
    owl_offset          location;
    owl_offset          size;
    owl_offset          bin_size;
    owl_file_handle     file;
    char                *bins[ NUM_BINS ];
} owl_buffer;

static void binInit( owl_buffer *buffer ) {
