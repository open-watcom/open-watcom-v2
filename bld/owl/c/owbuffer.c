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
//*****************************************

    int                 i;

    for( i = 0; i < NUM_BINS; i++ ) {
        buffer->bins[ i ] = NULL;
    }
}

static void binFini( owl_buffer *buffer ) {
//*****************************************

    int                 i;

    for( i = 0; i < NUM_BINS; i++ ) {
        if( buffer->bins[ i ] == NULL ) break;
        _ClientFree( buffer->file, buffer->bins[ i ] );
    }
}

static void bufferCollapse( owl_buffer *buffer ) {
//************************************************

    char                *bin;
    char                *dst;
    unsigned            i;

    assert( buffer->size == ( buffer->bin_size * NUM_BINS ) );
    bin = _ClientAlloc( buffer->file, buffer->size );
    for( dst = bin, i = 0; i < NUM_BINS; i++ ) {
        memcpy( dst, buffer->bins[ i ], buffer->bin_size );
        _ClientFree( buffer->file, buffer->bins[ i ] );
        buffer->bins[ i ] = NULL;
        dst += buffer->bin_size;
    }
    buffer->bins[ 0 ] = bin;
    buffer->bin_size = buffer->size;
}

static owl_offset bufferBinBytesLeft( owl_buffer *buffer ) {
//**********************************************************

    return( buffer->bin_size - ( buffer->location % buffer->bin_size ) );
}

static int bufferFull( owl_buffer *buffer ) {
//*******************************************

    assert( buffer->size <= ( buffer->bin_size * NUM_BINS ) );
    return( buffer->size == ( buffer->bin_size * NUM_BINS ) );
}

static void bufferFill( owl_buffer *buffer, const char *data, owl_offset len ) {
//******************************************************************************

    unsigned            index;
    char                *location;

    assert( bufferBinBytesLeft( buffer ) >= len );
    index = buffer->location / buffer->bin_size;
    if( buffer->bins[ index ] == NULL ) {
        buffer->bins[ index ] = _ClientAlloc( buffer->file, buffer->bin_size );
    }
    location = &buffer->bins[ index ][ buffer->location % buffer->bin_size ];
    if( ( buffer->location + len ) > buffer->size ) {
        buffer->size = buffer->location + len;
    }
    if( data != NULL ) {
        memcpy( location, data, len );
    } else {
        memset( location, 0, len );
    }
}

static void bufferWrite( owl_buffer *buffer, const char *data, owl_offset num_bytes ) {
//*************************************************************************************

    owl_offset          bytes_remaining;
    owl_offset          chunk_size;

    assert( buffer->location <= buffer->size );
    bytes_remaining = num_bytes;
    while( bytes_remaining ) {
        if( bufferFull( buffer ) ) {
            bufferCollapse( buffer );
        }
        chunk_size = bufferBinBytesLeft( buffer );
        if( bytes_remaining < chunk_size ) {
            chunk_size = bytes_remaining;
        }
        bufferFill( buffer, data, chunk_size );
        bytes_remaining -= chunk_size;
        if( data != NULL ) {
            data += chunk_size;
        }
        buffer->location += chunk_size;
    }
}

static void bufferPad( owl_buffer *buffer, owl_offset to ) {
//**********************************************************

    owl_offset          old_location;

    if( to > buffer->size ) {
        old_location = buffer->location;
        buffer->location = buffer->size;
        bufferWrite( buffer, NULL, to - buffer->size );
        buffer->location = old_location;
    }
}

owl_buffer * OWLENTRY OWLBufferInit( owl_file_handle file ) {
//***********************************************************

    owl_buffer          *buffer;

    buffer = _ClientAlloc( file, sizeof( owl_buffer ) );
    buffer->location = 0;
    buffer->size = 0;
    buffer->bin_size = INITIAL_BIN_SIZE;
    buffer->file = file;
    binInit( buffer );
    return( buffer );
}

void OWLENTRY OWLBufferFini( owl_buffer *buffer ) {
//*************************************************

    binFini( buffer );
    _ClientFree( buffer->file, buffer );
}

void OWLENTRY OWLBufferWrite( owl_buffer *buffer, const char *src, owl_offset len ) {
//***********************************************************************************

    if( buffer->location > buffer->size ) {
        bufferPad( buffer, buffer->location );
    }
    bufferWrite( buffer, src, len );
}

void OWLENTRY OWLBufferRead( owl_buffer *buffer, owl_offset location, char *dst, owl_offset len ) {
//*************************************************************************************************

    owl_offset          bytes_remaining;
    owl_offset          chunk_size;
    unsigned            index;

    assert( ( location + len ) <= buffer->size );
    bytes_remaining = len;
    while( bytes_remaining ) {
        index = location / buffer->bin_size;
        if( index > NUM_BINS ) {
            // reading past the end of the damn buffer
            memset( dst, 0, bytes_remaining );
            break;
        }
        chunk_size = min( buffer->bin_size - location % buffer->bin_size, bytes_remaining );
        memcpy( dst, &buffer->bins[ index ][ location % buffer->bin_size ], chunk_size );
        location += chunk_size;
        bytes_remaining -= chunk_size;
        dst += chunk_size;
    }
}

owl_offset OWLENTRY OWLBufferTell( owl_buffer *buffer ) {
//*******************************************************

    return( buffer->location );
}

void OWLENTRY OWLBufferSeek( owl_buffer *buffer, owl_offset location ) {
//**********************************************************************

    buffer->location = location;
}

owl_offset OWLENTRY OWLBufferSize( owl_buffer *buffer ) {
//*******************************************************

    if( buffer == NULL ) return( 0 );
    return( buffer->size );
}

void OWLENTRY OWLBufferEmit( owl_buffer *buffer ) {
//*************************************************

    unsigned            i;
    unsigned            last_bin;

    last_bin = buffer->size / buffer->bin_size;
    for( i = 0; i < last_bin; i++ ) {
        _ClientWrite( buffer->file, buffer->bins[ i ], buffer->bin_size );
    }
    _ClientWrite( buffer->file, buffer->bins[ i ], buffer->size % buffer->bin_size );
}
