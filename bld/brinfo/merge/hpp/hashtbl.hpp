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
    HASHTBL.HPP
    -----------
    Multi-purpose container classes (hash tables and singly linked lists)
    for the browse/merge DLL.

    HashTable<T> : Very specialized hash-table class used throughout the DLL.
    LList<T> : Basic, generic, non-intrusive singly linked list.
*/

#ifndef _HASHTBL_HPP
#define _HASHTBL_HPP

#ifndef NUM_BUCKETS
#define NUM_BUCKETS     251     // a nice prime number
#endif

#include "pool.hpp"

/*  Turn off memory tracking to redefine "new" and "delete"
*/
#undef new
#undef delete


/*  Hashable:
        Any class must inherit from Hashable to be stored in a HashTable.
*/
struct Hashable {
    virtual ~Hashable() {}

    uint_32     index;  // general-purpose id number
    Hashable    *next;
};

inline uint_32 indexOf( Hashable *p ){
    return p?p->index:(uint_32)0;
}



/*  HashTableBase:
      Guts of the HashTable template class.
*/

class HashTableBase {
    protected:
        HashTableBase( int num_buckets );

    public:
        ~HashTableBase();

        // Empty the table, with or without deleting the inserted entries.
        void    Clear();
        void    ClearAndDelete();

        // Access functions.
        int     NumBuckets() { return _numBuckets; }
        int     Count() { return _numEntries; }

    protected:
        // Functions which are overridden in the derived template classes.
        void            Insert( Hashable *hashdata );
        Hashable *      Lookup( uint_32 id );
        Hashable *      Remove( uint_32 id );

    protected:
        int             Hash( uint_32 id );

    protected:
        Hashable        **_table;
        int             _numEntries;
        const int       _numBuckets;
};


/*  HashTable<T>:
      T must inherit from Hashable.
*/

template<class T>
class HashTable : public HashTableBase {
    public:
        HashTable( int num_buckets = NUM_BUCKETS )
            : HashTableBase(num_buckets) {}

        void    Insert( T *hashdata )
            { HashTableBase::Insert(hashdata); }
        T *     Lookup( uint_32 id )
            { return (T *)HashTableBase::Lookup(id); }
        T *     Remove( uint_32 id )
            { return (T *)HashTableBase::Remove(id); }
};


/*  LListBase:
      Guts of the LList template class.
*/

class LListBase {
    protected:
        LListBase();

    public:
        ~LListBase();

        // Empty the list.  Does not delete pointed-to entries.
        void    Clear();

        // Access function.
        int     Count() { return _numEntries; }

    protected:
        // Functions overriden in the derived template classes.
        void    Append( void *data );
        void    Push( void *data ) { Append( data ); }
        void *  Pop();
        void *  First();
        void *  Next();

    protected:
        // To save memory, the pointers are stored in bunches.
        struct Block {
            Block       *next;
            Block       *prev;
            void        *data[32];

            static Pool blockPool;
            void        *operator new( size_t ) { return blockPool.Get(); }
            void        *operator new( size_t, const WChar *, const WChar *,
                                       WULong ) { return blockPool.Get(); }
            void        operator delete( void *p ) { blockPool.Release(p); }
        };

        int             _tail;
        int             _current;
        Block *         _headBlock;
        Block *         _tailBlock;
        Block *         _currentBlock;
        int             _numEntries;
};


/*  LList<T>:
      A basic non-instrusive singly linked list class.
*/

template<class T>
class LList : public LListBase {
    public:
        LList() : LListBase() {}

        void    Append( T *data )
            { LListBase::Append( data ); }
        void    Push( T *data )
            { LListBase::Push( data ); }
        T *     Pop()
            { return (T *) LListBase::Pop(); }
        T *     First()
            { return (T *) LListBase::First(); }
        T *     Next()
            { return (T *) LListBase::Next(); }
};


/*  Restart memory tracking
*/
#include "wnew.hpp"

#endif // _HASHTBL_HPP
