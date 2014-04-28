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


#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__

#include <stddef.h>
#include <string.h>

#if DEBUG
#  include "debuglog.h"
#endif

//
// MemoryPool provides a simple memory pool.
//
// Memory is grabbed in hunks and then alloc() parcels out bits from a
// hunk when it is called.
//
// free() puts memory into a free list of blocks which are used from
// alloc() in preference to bits of hunks.
//
// All hunks are freed when the object is destroyed or ragnarok() is called.
//

class MemoryPool {

public:
                    MemoryPool( const char * owner );
                    MemoryPool( size_t elemSize, const char * owner,
                                int elemsPerBlock = 10 );
                    ~MemoryPool();

            void *  alloc();
            void    free( void * );
            void    ragnarok();         // free entire pool
            void    setSize( size_t elemSize, int elemsPerBlock = 10 );

protected:
            void        grow();

            size_t      _blockSize;
            size_t      _elemSize;
            int         _elemsPerBlock;
            char *      _lastElement;
            char *      _currElement;
            char *      _currBlock;
            void *      _freeList;


            #if DEBUG
            const char *    _poolOwner; // name of owner
            int             _numAllocs;
            int             _numFrees;
    static  DebuggingLog    _log;
            #endif
};

#endif // __MEMPOOL_H__
