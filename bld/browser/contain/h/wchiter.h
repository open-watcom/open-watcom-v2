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
//  wchiter.h    Definitions for the WATCOM Container Hash Iterator Classes
//
//  Copyright by WATCOM International Corp. 1988-1993.  All rights reserved.
//

#ifndef _WCHITER_H_INCLUDED

#ifndef __cplusplus
#error wchiter.h is for use with C++
#endif

#include <wcdefs.h>
#ifndef _WCEXCEPT_H_INCLUDED
#include <wcexcept.h>
#endif
#ifndef _WCHASH_H_INCLUDED
#include <wchash.h>
#endif



//
// The WCHashIterBase is the base class for all hash iterator classes
//
// WCIterExcept is used as a base class to provide exception handling
//
//

class WCHashIterBase : public WCIterExcept {
private:
    typedef WCSLink BaseHashLink;

protected:
    const WCHashBase *          curr_hash;
    WCIsvSListIter<BaseHashLink> bucket_iter;
    unsigned                    curr_bucket;
    int                         at_end;

    WCbool base_advance();

    inline WCbool base_hash_valid() const {
        return( ( curr_hash != 0 )&&( curr_hash->num_buckets > 0 ) );
    };

public:
    inline WCHashIterBase() : curr_hash( 0 ), bucket_iter()
                                , curr_bucket( 0 ), at_end( 0 ) {};

    inline WCHashIterBase( const WCHashBase& hash )
                : curr_hash( &hash ), bucket_iter() {
        reset();
    };

    inline virtual ~WCHashIterBase() = 0;

    void reset();
};


WCHashIterBase::~WCHashIterBase() {};



//
// The WCValHashTableIter is the iterator for the WCValHashTable class
//
// WCIterExcept is used as a base class to provide exception handling
//

template<class Type>
class WCValHashTableIter : public WCHashIterBase {
protected:
    typedef WCHashLink<Type> HashLink;

    // get the current hashlink object, or 0 if current undefined
    WCHashLink<Type> *base_curr_hashlink() const;

public:
    inline WCValHashTableIter() {};

    inline WCValHashTableIter( const WCValHashTable<Type>& hash )
                : WCHashIterBase( hash ) {};

    inline virtual ~WCValHashTableIter() {};

    inline const WCValHashTable<Type> *container() const {
        if( curr_hash == 0 ) {
            base_throw_undef_iter();
        }
        return( (const WCValHashTable<Type> *)curr_hash );
    };

    Type current() const;

    inline void reset() {
        WCHashIterBase::reset();
    };

    inline void reset( const WCValHashTable<Type> &hash ) {
        curr_hash = &hash;
        reset();
    };

    inline WCbool operator++() {
        return( base_advance() );
    }

    inline WCbool operator()() {
        return( base_advance() );
    }
};


template <class Type>
WCHashLink<Type> *WCValHashTableIter<Type>::base_curr_hashlink() const {
    HashLink *link = 0;
    if( base_hash_valid() ) {
        link = (HashLink *)bucket_iter.current();
    }
    return( link );
};


template <class Type>
Type WCValHashTableIter<Type>::current() const {
    HashLink *link = base_curr_hashlink();
    if( link == 0 ) {
        // bucket_iter is off end or no hash to iterate over
        base_throw_undef_item();
        Type temp;
        return( temp );
    }
    return( link->data );
};




//
// The WCPtrHashTableIter is the iterator for the WCPtrHashTable class.
//

template<class Type>
class WCPtrHashTableIter : public WCValHashTableIter<void *> {
public:
    inline WCPtrHashTableIter() {};

    inline WCPtrHashTableIter( const WCPtrHashTable<Type>& hash )
                : WCValHashTableIter( hash ) {};

    inline virtual ~WCPtrHashTableIter() {};

    inline const WCPtrHashTable<Type> *container() const {
        return( (const WCPtrHashTable<Type> *)WCValHashTableIter::container() );
    };

    inline Type *current() const {
        return( (Type *)WCValHashTableIter::current() );
    };

    inline void reset() {
        WCValHashTableIter::reset();
    };

    inline void reset( const WCPtrHashTable<Type> &hash ) {
        WCValHashTableIter::reset( hash );
    };
};




//
// The WCValHashSetIter is the iterator for the WCValHashSet class.
//

template<class Type>
class WCValHashSetIter : public WCValHashTableIter<Type> {
public:
    inline WCValHashSetIter() {};

    inline WCValHashSetIter( const WCValHashSet<Type>& hash )
                : WCValHashTableIter( hash ) {};

    inline virtual ~WCValHashSetIter() {};

    inline const WCValHashSet<Type> *container() const {
        return( (const WCValHashSet<Type> *)WCValHashTableIter::container() );
    };

    inline void reset() {
        WCValHashTableIter::reset();
    };

    inline void reset( const WCValHashSet<Type> &hash ) {
        WCValHashTableIter::reset( hash );
    };
};




//
// The WCPtrHashSetIter is the iterator for the WCPtrHashSet class.
//

template<class Type>
class WCPtrHashSetIter : public WCPtrHashTableIter<Type> {
public:
    inline WCPtrHashSetIter() {};

    inline WCPtrHashSetIter( const WCPtrHashSet<Type>& hash )
                : WCPtrHashTableIter( hash ) {};

    inline virtual ~WCPtrHashSetIter() {};

    inline const WCPtrHashSet<Type> *container() const {
        return( (const WCPtrHashSet<Type> *)WCPtrHashTableIter::container() );
    };

    inline void reset() {
        WCPtrHashTableIter::reset();
    };

    inline void reset( const WCPtrHashSet<Type> &hash ) {
        WCPtrHashTableIter::reset( hash );
    };
};




//
// The WCValHashDictIter is the iterator for the WCValHashDict class.
//
// This class inherits privately to hide the current member function.
//

template<class Key, class Value>
class WCValHashDictIter
        : private WCValHashTableIter<WCHashDictKeyVal<Key, Value > > {
public:
    inline WCValHashDictIter() {};

    inline WCValHashDictIter( const WCValHashDict<Key, Value>& hash )
                : WCValHashTableIter( hash ) {};

    inline virtual ~WCValHashDictIter() {};

    inline const WCValHashDict<Key, Value> *container() const {
        return( (const WCValHashDict<Key, Value> *)WCValHashTableIter
                                                        ::container() );
    };

    inline wciter_state exceptions() const {
        return( WCValHashTableIter::exceptions() );
    };

    inline wciter_state exceptions( wciter_state const set_flags ) {
        return( WCValHashTableIter::exceptions( set_flags ) );
    };

    Key key() const;

    inline void reset() {
        WCValHashTableIter::reset();
    };

    inline void reset( const WCValHashDict<Key, Value> &hash ) {
        WCValHashTableIter::reset( hash );
    };

    Value value() const;

    inline WCbool operator++() {
        return( WCValHashTableIter::operator++() );
    };

    inline WCbool operator()() {
        return( WCValHashTableIter::operator()() );
    };
};


template <class Key, class Value>
Key WCValHashDictIter<Key, Value>::key() const {
    HashLink *link = base_curr_hashlink();
    if( link == 0 ) {
        // bucket_iter is off end or no hash to iterate over
        base_throw_undef_item();
        Key temp;
        return( temp );
    }
    return( link->data.key );
};


template <class Key, class Value>
Value WCValHashDictIter<Key, Value>::value() const {
    HashLink *link = base_curr_hashlink();
    if( link == 0 ) {
        // bucket_iter is off end or no hash to iterate over
        base_throw_undef_item();
        Value temp;
        return( temp );
    }
    return( link->data.value );
};




//
// The WCPtrHashDictIter is the iterator for the WCPtrHashDict class.
//

template<class Key, class Value>
class WCPtrHashDictIter
        : public WCValHashDictIter<void *, void *> {
public:
    inline WCPtrHashDictIter() {};

    inline WCPtrHashDictIter( const WCPtrHashDict<Key, Value>& hash )
                : WCValHashDictIter( hash ) {};

    inline virtual ~WCPtrHashDictIter() {};

    inline const WCPtrHashDict<Key, Value> *container() const {
        return( (const WCPtrHashDict<Key, Value> *)WCValHashDictIter
                                                        ::container() );
    };

    inline Key * key() const {
        return( (Key *)WCValHashDictIter::key() );
    };

    inline void reset() {
        WCValHashDictIter::reset();
    };

    inline void reset( const WCPtrHashDict<Key, Value> &hash ) {
        WCValHashDictIter::reset( hash );
    };

    Value *value() const {
        return( (Value *)WCValHashDictIter::value() );
    };
};


#define _WCHITER_H_INCLUDED
#endif
