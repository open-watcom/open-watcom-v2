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

#define _BlockSize( x )         ( ( 1 << ( x ) ) * INITIAL_BLOCK_SIZE )
#define _BinSize( x )           ( _BlockSize( x ) * BLOCKS_PER_BIN )

static void binInit( owl_buffer *buffer ) {
//*****************************************

    owl_mem_block       *block;
    owl_offset          start_addr;
    unsigned            i, j;

    start_addr = 0;
    for( i = 0; i < BINS_PER_BUFFER; i++ ) {
        for( j = 0; j < BLOCKS_PER_BIN; j++ ) {
            block = &buffer->blocks[ i ][ j ];
            block->start_addr = start_addr;
            block->mem = NULL;
            start_addr += _BlockSize( i );
        }
    }
    block = &buffer->blocks[ 0 ][ 0 ];
    block->mem = _ClientAlloc( buffer->file, _BlockSize( 0 ) );
}

static void nextBlock( owl_buffer *buffer ) {
//*******************************************

    owl_mem_block       *block;

    assert( buffer->curr_addr == _BlockSize( buffer->curr_bin ) );
    buffer->curr_block++;
    if( buffer->curr_block == BLOCKS_PER_BIN ) {
        buffer->curr_block = 0;
        buffer->curr_bin++;
        if( buffer->curr_bin == BINS_PER_BUFFER ) {
            // error - buffer full!
            buffer->curr_bin = 0;
        }
    }
    block = &buffer->blocks[ buffer->curr_bin ][ buffer->curr_block ];
    block->mem = _ClientAlloc( buffer->file, _BlockSize( buffer->curr_bin ) );
    buffer->curr_addr = 0;
}

static owl_offset fillBlock( owl_buffer *buffer, const char *src, owl_offset size ) {
//***********************************************************************************

    owl_offset          bytes_left;
    owl_offset          bytes_to_write;
    owl_mem_block       *block;


    block = &buffer->blocks[ buffer->curr_bin ][ buffer->curr_block ];
    bytes_left = _BlockSize( buffer->curr_bin ) - buffer->curr_addr;
    bytes_to_write = min( size, bytes_left );
    memcpy( block->mem + buffer->curr_addr, src, bytes_to_write );
    buffer->curr_addr += bytes_to_write;
    if( bytes_to_write < size ) {
        nextBlock( buffer );
    }
    return( bytes_to_write );
}

void OWLENTRY OWLBufferPatch( owl_buffer *buffer, owl_offset location, owl_offset value, unsigned bit_mask ) {
//************************************************************************************************************

    owl_mem_block       *block;
    unsigned            i, j;
    owl_offset          *patch_pos;
    owl_offset          patch_value;

    // this is obviously very cheesy and special purpose - given that ( location % 4 ) == 0
    // we add the 32-bit value to whatever is at that particular location. ACK! Because this
    // shouldn't be called too often we take the slow cheesy route here and just whip through
    // bins until we find the magic location. I apologize for this whole routine.

    assert( ( location % 4 ) == 0 );
    assert( location < OWLBufferOffset( buffer ) );
    for( i = 0; i <= buffer->curr_bin; i++ ) {
        block = &buffer->blocks[ i ][ 0 ];
        assert( location >= block->start_addr );
        if( location < ( block->start_addr + _BinSize( i ) ) ) {
            for( j = 0; j < BLOCKS_PER_BIN; j++ ) {
                block = &buffer->blocks[ i ][ j ];
                if( location < ( block->start_addr + _BlockSize( i ) ) ) {
                    patch_pos = (owl_offset *)( &block->mem[ location - block->start_addr ] );
                    patch_value = ( *patch_pos + value ) & bit_mask;
                    *patch_pos = ( *patch_pos & ~bit_mask ) | patch_value;
                    return;
                }
            }
            assert( 0 );        // ACK! what happened?
        }
    }
    assert( 0 );
}

void OWLENTRY OWLBufferWrite( owl_buffer *buffer, const char *src, owl_offset size ) {
//************************************************************************************


    owl_offset          bytes_written;

    while( size != 0 ) {
        bytes_written = fillBlock( buffer, src, size );
        size -= bytes_written;
        src += bytes_written;
    }
}

void OWLENTRY OWLBufferEmit( owl_buffer *buffer ) {
//*************************************************

    unsigned            i, j;
    owl_offset          size;
    owl_mem_block       *block;

    block = &buffer->blocks[ 0 ][ 0 ];

    // write all the full bins out one block at a time
    for( i = 0; i < buffer->curr_bin; i++ ) {
        size = _BlockSize( i );
        for( j = 0; j < BLOCKS_PER_BIN; j++ ) {
            _ClientWrite( buffer->file, block->mem, size );
            block++;
        }
    }

    // now write all full blocks in last bin out
    size = _BlockSize( buffer->curr_bin );
    for( j = 0; j < buffer->curr_block; j++ ) {
        _ClientWrite( buffer->file, block->mem, size );
        block++;
    }

    // and lastly the (potentially) empty block which is the current block
    _ClientWrite( buffer->file, block->mem, buffer->curr_addr );
}

owl_offset OWLENTRY OWLBufferOffset( owl_buffer *buffer ) {
//*********************************************************

    owl_mem_block       *block;

    block = &buffer->blocks[ buffer->curr_bin ][ buffer->curr_block ];
    return( block->start_addr + buffer->curr_addr );
}

owl_buffer * OWLENTRY OWLBufferInit( owl_file_handle file ) {
//***********************************************************

    owl_buffer          *buffer;

    buffer = _ClientAlloc( file, sizeof( owl_buffer ) );
    buffer->curr_addr = 0;
    buffer->curr_bin = 0;
    buffer->curr_block = 0;
    buffer->file = file;
    binInit( buffer );
    return( buffer );
}

void OWLENTRY OWLBufferFini( owl_buffer *buffer ) {
//*************************************************

    owl_mem_block       *block;
    unsigned            i, j;

    for( i = 0; i < BINS_PER_BUFFER; i++ ) {
        for( j = 0; j < BLOCKS_PER_BIN; j++ ) {
            block = &buffer->blocks[ i ][ j ];
            if( block->mem != NULL ) {
                _ClientFree( buffer->file, block->mem );
            }
        }
    }
    _ClientFree( buffer->file, buffer );
}
