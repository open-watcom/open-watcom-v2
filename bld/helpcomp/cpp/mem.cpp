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
MEM:  memory functions and tracking
*/


#include <malloc.h>
#include "hcmem.h"

#ifdef TRACKER
extern "C" {
#include "trmem.h"
}
#include <io.h>
#include <fcntl.h>

static _trmem_hdl TrHdl;

#pragma initialize 40;


//
//  PrintLine   -- output function used by the memory tracker.
//

void PrintLine( void *parm, const char *buf, size_t len )
{
    parm = parm;
    write( STDOUT_FILENO, (void *) buf, (unsigned int) len );
}
#endif


//
//  Memory::Memory(), Memory::~Memory() -- Initialization and clean-up
//                     functions for the mem. tracker.
//

#ifdef TRACKER
Memory::Memory()
{
    TrHdl = _trmem_open( malloc, free, realloc, NULL, NULL, PrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 | _TRMEM_REALLOC_NULL |
            _TRMEM_FREE_NULL | _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
}

Memory::~Memory()
{
    _trmem_prt_list( TrHdl );
    _trmem_close( TrHdl );
}
#endif


//
//  new -- global allocator with hooks into the memory tracker.
//

void *operator new( size_t size )
{
    void *p;
#ifdef TRACKER
        p = _trmem_alloc( size, HCMemerr, TrHdl );
#else
        p = malloc( size );
#endif
    return p;
}


//
//  renew -- global realloc function with hooks into the memory tracker.
//

void *renew( void * p, size_t size )
{
#ifdef TRACKER
    p = _trmem_realloc( p, size, HCMemerr, TrHdl );
#else
    p = realloc( p, size );
#endif
    return p;
}


//
//  delete  -- global deallocator with hooks into the memory tracker.
//

void operator delete( void *p )
{
    if( p == NULL ) return;
#ifdef TRACKER
    _trmem_free( p, HCMemerr, TrHdl );
#else
    free( p );
#endif
}


//
// Pool::Pool   --Default constructor.
//

static const unsigned Pool::BLOCK_SIZE = 1024;

Pool::Pool( size_t size, unsigned b_size )
    : _size( size > sizeof(void*) ? size : sizeof(void*) ),
      _block( b_size ? b_size : BLOCK_SIZE )
{
    uint_8  *index;
    _array = new uint_8[ _block * _size + sizeof( uint_8 * ) ];

    for( index = _array; index < _array+(_block-1)*_size; index += _size ){
    *((void**)index) = (void *) (index + _size);
    }
    *((void**)index) = NULL;
    *((uint_8**)(index+_size)) = NULL;

    _pfree = (void *) _array;
}


//
// Pool::~Pool  --Destructor.
//

Pool::~Pool()
{
    uint_8  *temp;
    do{
    temp = _array;
    _array = *((uint_8 **) (_array + _block*_size));
    delete[] temp;
    } while( _array != NULL );
}


//
// Pool::get    --General purpose allocator.
//

void *Pool::get()
{
    void    *result;

    if( _pfree == NULL ){
    uint_8  *index;
    uint_8  *temp = new uint_8[ _block*_size + sizeof(uint_8 *) ];

    for( index=temp; index<temp+(_block-1)*_size; index+=_size ){
        *((void**) index) = (void *) (index + _size);
    }
    *((void**) index) = NULL;
    *((uint_8**) (index + _size)) = _array;

    _pfree = (void *) (_array = temp);
    }
    result = _pfree;
    _pfree = *((void **) _pfree);
    return result;
}


//
//  Pool::release   --General purpose de-allocator.
//

void Pool::release( void * p )
{
    *((void **) p) = _pfree;
    _pfree = p;
}
