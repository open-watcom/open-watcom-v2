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


/*
 Description:
 ============
 These are the memory management routines used by the 3d pipelines. They
 allow allocations in arbritrary amounts without causing a global allocation
 each for each request. The drawback is that they do not allow deallocation
 until it is time to deallocate all the global memory that they use.
*/

#include "icgr.h"

/* Define the structures used internally to keep track of memory */

#define PIPE_MEM_BLOCK_SIZE     0x8000

typedef struct mem_block {
    struct mem_block *  next;
    long                free_offset;
    char                mem[ PIPE_MEM_BLOCK_SIZE ];
} mem_block;

typedef mem_block * mem_block_ptr;


/* Declaration of the pointer to the list of memory blocks */
mem_block_ptr  PipeMemHeader = NULL;


/* routines to allocate chunks of memory and to free it all */
static void add_new_mem_block(
/****************************/
    void
) {
    mem_block_ptr   new_block;

    _gnew( new_block, 1 );

    new_block->free_offset = 0;         // all memory is available
    new_block->next = PipeMemHeader;
    PipeMemHeader = new_block;
}

extern void * pipe_alloc(
/***********************/
    size_t          size
) {
    void *          mem;

    if (size > PIPE_MEM_BLOCK_SIZE) {
        return( NULL );
    }

    if (PipeMemHeader == NULL) {
        add_new_mem_block();
    }

    if (size > PIPE_MEM_BLOCK_SIZE - PipeMemHeader->free_offset) {
        add_new_mem_block();
    }

    mem = PipeMemHeader->mem + PipeMemHeader->free_offset;
    PipeMemHeader->free_offset += size;

    return( mem );
}

extern void pipe_free_all(
/************************/
    void
) {
    mem_block_ptr   old_block;
    mem_block_ptr   curr_block;

    curr_block = PipeMemHeader;
    while (curr_block != NULL) {
        old_block = curr_block;
        curr_block = curr_block->next;

        _gfree( old_block );
    }

    PipeMemHeader = NULL;
}
