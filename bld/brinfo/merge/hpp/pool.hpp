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


#ifndef _POOL_HPP
#define _POOL_HPP

#define POOL_START      256
#define POOL_INC        256

//
// Pool         --Data allocation class (i.e. a carver).
//

enum    PoolIds {       // for debugging
    AvlLinkPool = 0x00,
    AvlStackPool,
    DeclPool,
    DefnPool,
    HashLinkPool,
    LListBlockPool,
    U32PairPool,
    ScopePool,
    StrLinkPool,
    TypePool,
    UsagePool
};

class Pool {
    private:
        static const unsigned   BLOCK_LEN;

    public:
        Pool( PoolIds id, size_t size, unsigned blockLen=BLOCK_LEN );
        ~Pool();

        void *Get();
        void Release( void * p );

        enum Severity {
            Low,        // Organize the free list
            Medium,     // Collect unused blocks
            High        // Delete unused blocks
        };
        void CleanUp( Severity s );

        unsigned        MemUsed();      // for debugging

    private:
        // Assignment of Pool's is not permitted.
        Pool( Pool const & )
        {
            // empty
        }
        Pool & operator=( Pool const & )
        {
            return *this;
        }

    private:
        uint_8                  *_array;
        void                    *_pfree;
        unsigned                _blockCount;    // for debugging
        PoolIds                 _idNum;         // for debugging

        uint_8                  *_freeBlocks;

        const unsigned          _blockLen;
        const size_t            _size;
};


#endif  // _POOL_HPP
