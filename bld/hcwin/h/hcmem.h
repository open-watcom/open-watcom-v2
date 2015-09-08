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


//
//  HCMEM.H -- Some memory tracking/handling for Watcom .HLP compiler.
//
//

#ifndef HCMEM_H
#define HCMEM_H

#include "hcerrors.h"


//
//  Memory  -- Dummy class to initialize and cleanup the memory tracker.
//

struct Memory
{
    Memory();
    ~Memory();
};


extern void mem_statistic();

//
//  New global allocators/deallocators to hook into the memory tracker.
//

extern void *operator new( size_t size );
extern void operator delete( void *p );
extern void *renew( void *p, size_t size );


//
// Pool     --Data allocation class.
//        DO NOT USE before initializing the memory tracker!!!
//        (E.g., don't make this a static class member or global
//                 variable.  Static local variable is okay, though.)
//

class Pool
{
    char    *_array;
    void    *_pfree;

    const unsigned  _block;
    const size_t    _size;

    // Assignment of Pool's is not permitted.
    Pool( Pool const & ) : _block( 0 ), _size( 0 ) {};
    Pool &  operator=( Pool const & ) { return *this; };

protected:
    static const unsigned BLOCK_SIZE;

public:
    Pool( size_t size, unsigned b_size = BLOCK_SIZE );
    ~Pool();

    void *get();
    void release( void * p );
};


//
//  Buffer  -- a template array class to encapsulate free-store
//         allocation.  (Useful in case of exceptions.)
//

template<class T> class Buffer
{
    T       *_data;

    // Assignment of Buffer's is not allowed. (I could do it,
    // but I don't need to and it would involve storing size information).
    Buffer( Buffer<T> const & ) {};
    Buffer<T> & operator=( Buffer<T> const & ) { return *this; };

public:
    Buffer( size_t size );
    ~Buffer();

    operator T *() { return _data; };
    T &operator[]( size_t index ) { return _data[index]; };
    void *resize( size_t size );
};

template<class T>
inline Buffer<T>::Buffer( size_t size )
{
    if( size == 0 ) {
        _data = NULL;
    } else {
        _data = new T[size];
    }
}

template<class T>
inline Buffer<T>::~Buffer()
{
    delete[] _data;
}

template<class T>
void *Buffer<T>::resize( size_t size )
{
    if( size == 0 ) {
        if( _data )
            delete[] _data;
        _data = NULL;
    } else {
        _data = (T*)renew( _data, size * sizeof( T ) );
    }
    return _data;
}

#endif
