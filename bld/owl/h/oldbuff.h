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


typedef struct owl_mem_block    owl_mem_block;
typedef struct owl_block_list   owl_block_list;
typedef struct owl_buffer       owl_buffer;
typedef struct owl_buffer       *owl_buffer_handle;

#define BLOCKS_PER_BIN          16
#define BINS_PER_BUFFER         16
#define INITIAL_BLOCK_SIZE      4096

struct owl_mem_block {
    owl_offset          start_addr;
    char                *mem;
};

struct owl_buffer {
    owl_offset          curr_addr;      // offset within block of next byte
    unsigned            curr_bin;       // bin in which block below resides
    unsigned            curr_block;     // block in which to put next byte
    owl_file_handle     file;
    owl_mem_block       blocks[ BINS_PER_BUFFER ][ BLOCKS_PER_BIN ];
};

extern owl_buffer_handle        OWLENTRY OWLBufferInit( owl_file_handle file );
extern void                     OWLENTRY OWLBufferFini( owl_buffer_handle buffer );

extern void                     OWLENTRY OWLBufferWrite( owl_buffer_handle buffer, const char *src, owl_offset size );
extern void                     OWLENTRY OWLBufferPatch( owl_buffer_handle buffer, owl_offset location, owl_offset value, unsigned bitMask );
extern void                     OWLENTRY OWLBufferRead( owl_buffer_handle buffer, owl_offset addr, char *dest, owl_offset size );
extern void                     OWLENTRY OWLBufferEmit( owl_buffer_handle buffer );
extern owl_offset               OWLENTRY OWLBufferOffset( owl_buffer_handle buffer );
