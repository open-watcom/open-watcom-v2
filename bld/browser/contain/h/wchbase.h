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
//  wchbase.h    Definitions for the base classes used by
//               the WATCOM Container Hash Classes
//
//  Copyright by WATCOM International Corp. 1988-1993.  All rights reserved.
//

#ifndef _WCHBASE_H_INCLUDED

#ifndef __cplusplus
#error wchbase.h is for use with C++
#endif



//
// The default number of buckets in a hash table.  If a second parameter is
// passed to the hash table constructor, this value will not be used
//

const unsigned WC_DEFAULT_HASH_SIZE = 101;




//
// Hash implementation object:
// A singly linked list element for storing the values in the hash table
//

template <class Type>
class WCHashLink : public WCSLink {
public:
    Type data;

    // used by WCHashNew to be able to call a constructor on user allocator
    // allocated memory
    inline void * operator new( size_t, void * ptr ){
        return( ptr );
    }
    inline WCHashLink( const Type & datum ) : data( datum ) {};
    inline ~WCHashLink() {};
};



//
// Hash Dictionary implementation object:
// Combines the Key and Value into one object
//

template <class Key, class Value>
class WCHashDictKeyVal{
public:
    Key key;
    Value value;

    inline WCHashDictKeyVal( const WCHashDictKeyVal &orig )
            : key( orig.key ), value( orig.value ) {};
    inline WCHashDictKeyVal( const Key &new_key, const Value &new_value )
            : key( new_key ), value( new_value ) {};
    inline WCHashDictKeyVal( const Key &new_key ) : key( new_key ) {};
    inline WCHashDictKeyVal() {};
    inline ~WCHashDictKeyVal() {};

    // this operator is NEVER used, but necessary for compilation
    // (needed by WCValHashSet, inherited by WCValHashDict)
    inline int operator==( const WCHashDictKeyVal & ) const {
        return( 0 );
    };
};




//
// Provide base functionality for WATCOM container hash tables, sets and
// dictionaries.
//
// WCExcept provides exception handling.
//
// This class is an abstract class so that objects of this type cannot be
// created.
//


class WCHashBase : public WCExcept {
protected:
    WCIsvSList<WCSLink> *hash_array;
    unsigned num_buckets;
    unsigned num_entries;

    // link base non-templated class
    typedef WCSLink BaseHashLink;
    // pointer to element of templated type
    typedef const void *TTypePtr;

    enum find_type {            // enumerations for base_find
        FIND_FIRST,             // find first matching element
        NEXT_MULT_FIND };       // find next matching element in bucket

    // copy constructor base
    void base_construct( const WCHashBase * orig );

    // for copy constructor
    virtual BaseHashLink *base_copy_link( const BaseHashLink *orig ) = 0;

    virtual int base_equivalent( BaseHashLink *elem1
                               , TTypePtr elem2 ) const = 0;

    virtual unsigned base_get_bucket( TTypePtr elem ) const = 0;

    // for the resize member function
    virtual unsigned base_get_bucket_for_link( BaseHashLink *link ) const = 0;

    BaseHashLink *base_find( TTypePtr elem, unsigned *bucket
                           , unsigned *index, find_type type ) const;

    // common initialization code for the constructors
    void base_init( unsigned buckets );

    BaseHashLink *base_remove_but_not_delete( TTypePtr elem );

    // the insert function for WCValHashSet and WCPtrHashSet
    BaseHashLink *base_set_insert( TTypePtr elem );

    virtual BaseHashLink * WCHashNew( TTypePtr elem ) = 0;

    virtual void WCHashDelete( BaseHashLink *old_link ) = 0;


    inline WCHashBase() : hash_array( 0 ), num_buckets( 0 )
                        , num_entries( 0 ) {};

    inline WCHashBase( unsigned buckets ) {
        base_init( buckets );
    };

    virtual ~WCHashBase() = 0;

    void clear ();

    WCbool insert( TTypePtr elem );

    unsigned occurrencesOf( TTypePtr elem ) const;

    unsigned removeAll( TTypePtr elem );

    void resize( unsigned buckets );

public:
    static unsigned bitHash( const void * data, size_t size );

    friend class WCHashIterBase;
};


#define _WCHBASE_H_INCLUDED
#endif
