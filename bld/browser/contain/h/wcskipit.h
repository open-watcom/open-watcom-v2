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
//  wcskipit.h  Definitions for the WATCOM Container Skip List Iterator
//              Classes
//
//  Copyright by WATCOM International Corp. 1988-1993.  All rights reserved.
//

#ifndef _WCSKIPIT_H_INCLUDED

#ifndef __cplusplus
#error wcskipit.h is for use with C++
#endif

#include <wcdefs.h>
#ifndef _WCEXCEPT_H_INCLUDED
#include <wcexcept.h>
#endif
#ifndef _WCSKIP_H_INCLUDED
#include <wcskip.h>
#endif
#include <wcsibase.h>




//
// The WCValSkipListIter is the iterator for the WCValSkipList class
//

template<class Type>
class WCValSkipListIter : public WCSkipListIterBase<Type> {
public:
    inline WCValSkipListIter() {};

    inline WCValSkipListIter( const WCValSkipList<Type>& skip_list )
                : WCSkipListIterBase( skip_list ) {};

    inline ~WCValSkipListIter() {};

    inline const WCValSkipList<Type> *container() const {
        return (const WCValSkipList<Type> *)WCSkipListIterBase::container();
    };

    inline void reset() {
        WCSkipListIterBase::reset();
    };

    inline void reset( const WCValSkipList<Type> &skip_list ) {
        WCSkipListIterBase::reset( skip_list );
    };
};



//
// The WCPtrSkipListIter is the iterator for the WCPtrSkipList class.
//

template<class Type>
class WCPtrSkipListIter : public WCSkipListIterBase<void *> {
public:
    inline WCPtrSkipListIter() {};

    inline WCPtrSkipListIter( const WCPtrSkipList<Type>& skip_list )
                : WCSkipListIterBase( skip_list ) {};

    inline ~WCPtrSkipListIter() {};

    inline const WCPtrSkipList<Type> *container() const {
        return( (const WCPtrSkipList<Type> *)WCSkipListIterBase::container() );
    };

    inline Type *current() const {
        return( (Type *)WCSkipListIterBase::current() );
    };

    inline void reset() {
        WCSkipListIterBase::reset();
    };

    inline void reset( const WCPtrSkipList<Type> &skip_list ) {
        WCSkipListIterBase::reset( skip_list );
    };
};




//
// The WCValSkipListSetIter is the iterator for the WCValSkipListSet class
//

template<class Type>
class WCValSkipListSetIter : public WCSkipListIterBase<Type> {
public:
    inline WCValSkipListSetIter() {};

    inline WCValSkipListSetIter( const WCValSkipListSet<Type>& skip_list )
                : WCSkipListIterBase( skip_list ) {};

    inline ~WCValSkipListSetIter() {};

    inline const WCValSkipListSet<Type> *container() const {
        return (const WCValSkipListSet<Type> *)WCSkipListIterBase::container();
    };

    inline void reset() {
        WCSkipListIterBase::reset();
    };

    inline void reset( const WCValSkipListSet<Type> &skip_list ) {
        WCSkipListIterBase::reset( skip_list );
    };
};



//
// The WCPtrSkipListSetIter is the iterator for the WCPtrSkipListSet class.
//

template<class Type>
class WCPtrSkipListSetIter : public WCSkipListIterBase<void *> {
public:
    inline WCPtrSkipListSetIter() {};

    inline WCPtrSkipListSetIter( const WCPtrSkipListSet<Type>& skip_list )
                : WCSkipListIterBase( skip_list ) {};

    inline ~WCPtrSkipListSetIter() {};

    inline const WCPtrSkipListSet<Type> *container() const {
        return( (const WCPtrSkipListSet<Type> *)WCSkipListIterBase
                                                        ::container() );
    };

    inline Type *current() const {
        return( (Type *)WCSkipListIterBase::current() );
    };

    inline void reset() {
        WCSkipListIterBase::reset();
    };

    inline void reset( const WCPtrSkipListSet<Type> &skip_list ) {
        WCSkipListIterBase::reset( skip_list );
    };
};




//
// The WCValSkipListDictIter is the iterator for the WCValSkipListDict class.
//
// private inheritance is used to hide the current member function.
//

template<class Key, class Value>
class WCValSkipListDictIter
        : private WCSkipListIterBase<WCSkipListDictKeyVal<Key, Value> > {
public:
    inline WCValSkipListDictIter() {};

    inline WCValSkipListDictIter( const WCValSkipListDict<Key, Value>& hash )
                : WCSkipListIterBase( hash ) {};

    inline ~WCValSkipListDictIter() {};

    inline const WCValSkipListDict<Key, Value> *container() const {
        return( (const WCValSkipListDict<Key, Value> *)WCSkipListIterBase
                        ::container() );
    };

    inline wciter_state exceptions() const {
        return( WCSkipListIterBase::exceptions() );
    };

    inline wciter_state exceptions( wciter_state const set_flags ) {
        return( WCSkipListIterBase::exceptions( set_flags ) );
    };

    Key key() const;

    inline void reset() {
        WCSkipListIterBase::reset();
    };

    inline void reset( const WCValSkipListDict<Key, Value> &hash ) {
        WCSkipListIterBase::reset( hash );
    };

    Value value() const;

    inline WCbool operator++() {
        return( WCSkipListIterBase::operator++() );
    };

    inline WCbool operator()() {
        return( WCSkipListIterBase::operator()() );
    };
};


template <class Key, class Value>
Key WCValSkipListDictIter<Key, Value>::key() const {
    if( curr == 0 ) {
        base_throw_undef_item();
        Key temp;
        return( temp );
    }
    return( base_curr_node()->data.key );
};


template <class Key, class Value>
Value WCValSkipListDictIter<Key, Value>::value() const {
    if( curr == 0 ) {
        base_throw_undef_item();
        Value temp;
        return( temp );
    }
    return( base_curr_node()->data.value );
};




//
// The WCPtrSkipListDictIter is the iterator for the WCPtrSkipListDict class.
//

template<class Key, class Value>
class WCPtrSkipListDictIter
        : public WCValSkipListDictIter<void *, void *> {
public:
    inline WCPtrSkipListDictIter() {};

    inline WCPtrSkipListDictIter( const WCPtrSkipListDict<Key, Value>& hash )
                : WCValSkipListDictIter( hash ) {};

    inline ~WCPtrSkipListDictIter() {};

    inline const WCPtrSkipListDict<Key, Value> *container() const {
        return( (const WCPtrSkipListDict<Key, Value> *)WCValSkipListDictIter
                                                        ::container() );
    };

    inline Key * key() const {
        return( (Key *)WCValSkipListDictIter::key() );
    };

    inline void reset() {
        WCValSkipListDictIter::reset();
    };

    inline void reset( const WCPtrSkipListDict<Key, Value> &hash ) {
        WCValSkipListDictIter::reset( hash );
    };

    Value *value() const {
        return( (Value *)WCValSkipListDictIter::value() );
    };
};

#define _WCSKIPIT_H_INCLUDED
#endif
