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
//  wchash.h    Defines for the WATCOM Container Hash Table Class
//
//  Copyright by WATCOM International Corp. 1988-1993.  All rights reserved.
//

#ifndef _WCHASH_H_INCLUDED

#ifndef __cplusplus
#error wchash.h is for use with C++
#endif

#include <wcdefs.h>
#ifndef _WCEXCEPT_H_INCLUDED
#include <wcexcept.h>
#endif
#ifndef _WCLIST_H_INCLUDED
#include <wclist.h>
#endif
#ifndef _WCLISTIT_H_INCLUDED
#include <wclistit.h>
#endif
#include <wchbase.h>



//
// Macros to allow the user to find the size of objects which will be allocated
// and deallocated using their allocator and deallocator functions
//

#define WCValHashTableItemSize( Type )  sizeof( WCHashLink<Type> )
#define WCPtrHashTableItemSize( Type )  sizeof( WCHashLink<Type *> )
#define WCValHashSetItemSize( Type )    sizeof( WCHashLink<Type> )
#define WCPtrHashSetItemSize( Type )    sizeof( WCHashLink<Type *> )
#define WCValHashDictItemSize( Key, Value )     \
                        sizeof( WCHashLink<WCHashDictKeyVal<Key, Value> > )
#define WCPtrHashDictItemSize( Key, Value )     \
                        sizeof( WCHashLink<WCHashDictKeyVal<void *, void *> > )




//
// WCValHashTable - hash table for values, values do not need to be unique
//

template <class Type>
class WCValHashTable : public WCHashBase {
private:
    void * (* alloc_fn)( size_t );
    void (* dealloc_fn)( void *, size_t );

protected:
    typedef WCHashLink<Type> HashLink;

    // for PtrHash, hash_fn has the same prototype (ie Type is not really
    // <Type *> but just the Type which the PtrHash is templated over).  This
    // is accomplished by casting and base_get_bucket being virtual.  This
    // way the user can have an identical hash fn for both ValHash and PtrHash.
    unsigned (*hash_fn)( const Type & );

    // assignment operator base
    void base_assign( const WCValHashTable * orig );
    // copy constructor base
    void base_construct( const WCValHashTable * orig );

    // for WCHashBase::base_construct
    virtual BaseHashLink *base_copy_link( const BaseHashLink *orig ) {
        return( WCHashNew( &( (HashLink *)orig )->data ) );
    };

    // defines element equivalence, virtual, since pointer and dictionary
    // hash classes inherit from WCValHashTable, and have different
    // equivalence definitions
    // prototype is really base_equivalent(HashLink *, Type *)
    virtual int base_equivalent( BaseHashLink *elem1
                               , TTypePtr elem2 ) const {
        return( (const Type)( (HashLink *)elem1 )->data
                == *(const Type *)elem2 );
    };

    inline HashLink *base_find( const Type & elem, unsigned *bucket
                               , unsigned *index, find_type type ) const {
        return( (HashLink *)WCHashBase::base_find( &elem, bucket
                                                 , index, type ) );
    };

    // return the bucket an element hashes to
    virtual unsigned base_get_bucket_for_link( BaseHashLink *link ) const {
        return( base_get_bucket( &( (HashLink *)link )->data ) );
    };

    // parameter is really ( Type * elem )
    virtual unsigned base_get_bucket( TTypePtr elem ) const {
        return( hash_fn( *(const Type *)elem ) % num_buckets );
    }

    inline HashLink *base_remove_but_not_delete( const Type & elem ) {
        return( (HashLink *)WCHashBase::base_remove_but_not_delete( &elem ) );
    }

    inline HashLink *base_set_insert( const Type & elem ) {
        return( (HashLink *)WCHashBase::base_set_insert( &elem ) );
    };

    // similar to new and delete, but these will use user allocator and
    // deallocator functions if they were passed in
    virtual BaseHashLink * WCHashNew( TTypePtr elem );
    virtual void WCHashDelete( BaseHashLink *old_link );
public:
    inline WCValHashTable( unsigned (*fn)( const Type & )
                  , unsigned buckets = WC_DEFAULT_HASH_SIZE
                  ) : WCHashBase( buckets ), alloc_fn( 0 )
                   , dealloc_fn( 0 ), hash_fn( fn ) {};

    inline WCValHashTable( unsigned (*fn)( const Type & )
                  , unsigned buckets
                  , void * (*user_alloc)( size_t )
                  , void (*user_dealloc)( void *, size_t )
                  ) : WCHashBase( buckets ), alloc_fn( user_alloc )
                    , dealloc_fn( user_dealloc ), hash_fn( fn ) {};

    inline WCValHashTable( const WCValHashTable &orig ) {
        if( orig.num_buckets > 0 ) {
            hash_array = new WCIsvSList<BaseHashLink>[ orig.num_buckets ];
        } else {
            hash_array = 0;
        }
        base_construct( &orig );
    };

    virtual ~WCValHashTable ();

    inline unsigned buckets () const {
        return( num_buckets );
    };

    inline void clear () {
        WCHashBase::clear();
    };

    inline WCbool contains( const Type & elem ) const {
        unsigned bucket, index;
        return( base_find( elem, &bucket, &index, FIND_FIRST ) != 0 );
    };

    inline unsigned entries() const {
        return( num_entries );
    };


    WCbool find( const Type &search, Type &return_val ) const;

    void forAll( void (*)(Type, void*), void * ) const;

    inline WCbool insert( const Type & elem ) {
        return( WCHashBase::insert( &elem ) );
    };

    inline WCbool isEmpty () const {
        return( 0 == num_entries );
    }

    inline unsigned occurrencesOf( const Type &elem ) const {
        return( WCHashBase::occurrencesOf( &elem ) );
    };

    WCbool remove( const Type &elem );

    inline unsigned removeAll( const Type &elem ) {
        return( WCHashBase::removeAll( &elem ) );
    };

    inline void resize( unsigned buckets ) {
        WCHashBase::resize( buckets );
    };

    inline WCValHashTable &operator=( const WCValHashTable & orig ) {
        base_assign( &orig );
        return( *this );
    };

    inline int operator==( const WCValHashTable &rhs ) const {
        return( this == &rhs );
    };
};


template <class Type>
void WCValHashTable<Type>::base_assign( const WCValHashTable * orig ) {
    if( this != orig ) {
        clear();
        delete [] hash_array;
        if( orig->num_buckets > 0 ) {
            hash_array = new WCIsvSList<BaseHashLink>[ orig->num_buckets ];
        } else {
            hash_array = 0;
        }
        base_construct( orig );
    }
};


template <class Type>
void WCValHashTable<Type>::base_construct( const WCValHashTable * orig ) {
    alloc_fn = orig->alloc_fn;
    dealloc_fn = orig->dealloc_fn;
    hash_fn = orig->hash_fn;
    WCHashBase::base_construct( orig );
};


template <class Type>
WCSLink *WCValHashTable<Type>::WCHashNew( TTypePtr elem ) {
    HashLink *new_link;

    if( alloc_fn ) {
        // call the user specified allocator function to get the memory
        new_link = (HashLink *)alloc_fn( sizeof( HashLink ) );
    } else {
        new_link = (HashLink *)new char[ sizeof( HashLink ) ];
    }
    if( new_link ) {
        // use the placement syntax to call WCHashLink's copy constructor
        // with the memory allocated above
        new( new_link ) HashLink( *(const Type *)elem );
    }
    return( new_link );
};

template <class Type>
void WCValHashTable<Type>::WCHashDelete( BaseHashLink *old_base_link ) {
    HashLink *old_link = (HashLink *)old_base_link;

    if( old_link ) {
        old_link->~HashLink();
        if( dealloc_fn ) {
            // call the user specified deallocator functor to free the memory
            dealloc_fn( old_link, sizeof( HashLink ) );
        } else {
            delete (void *)old_link;
        }
    }
};

template <class Type>
WCValHashTable<Type>::~WCValHashTable () {
    if( num_entries > 0 ) {
        base_throw_not_empty();
    }
    clear();
    delete [] hash_array;
};


template <class Type>
WCbool WCValHashTable<Type>::find( const Type &search
                                 , Type &return_val ) const {
    unsigned bucket, index;

    HashLink *link = base_find( search, &bucket, &index, FIND_FIRST );
    if( link ) {
        return_val = link->data;
        return( TRUE );
    }
    return( FALSE );
};


template <class Type>
void WCValHashTable<Type>::forAll( register void (*user_fn)(Type, void*)
                                 , void *data ) const {
    WCIsvSListIter<BaseHashLink> iter;
    HashLink *link;

    for( int i = 0; i < num_buckets; i++ ) {
        iter.reset( hash_array[ i ] );
        while( ( link = (HashLink *)++iter ) != 0 ) {
            user_fn( link->data, data );
        }
    }
};


template <class Type>
WCbool WCValHashTable<Type>::remove( const Type &elem ) {
    HashLink *link = base_remove_but_not_delete( elem );
    if( link ) {
        WCHashDelete( link );
    }
    return( link != 0 );
};


//
// WCPtrHashTable - hash table for pointers, values pointed to do not need
// to be unique
//
// Implementation note:
// WCPtrHashTable inherits from WCValHashTable templated over <void *>.  This
// saves most of the hash table code being generated for pointer hash tables
// templated over different types, speeding up compile time, and reducing
// code size.
//

template <class Type>
class WCPtrHashTable : public WCValHashTable<void *> {
protected:
    // the real type of what is stored in the hash table
    typedef Type * __Type_Ptr;
    // all pointers are stored as pointers to void so that all pointer hashes
    // inherit from WCValHashTable templated over <void *>
    typedef void * __Stored_Ptr;
    // the hashing function which the user passes in, and what is called
    typedef unsigned (*_HashFnType)( const Type & );
    // the hashing function is stored by WCValHashTable, and this type
    // matches the type WCValHashTable stores
    typedef unsigned (*_StorageHashFnType)( const __Stored_Ptr & );
    // the user function passed to the forAll member function is passed
    // to WCValHashTable< void * >::forAll using this cast type
    typedef void (*_ForAllFnCast)( void *, void * );

    // equivalence definition for WCPtrHashTable, two pointers are equivalent
    // if the values pointed to are equivalent
    // prototype is really base_equivalent(HashLink *, Type * *)
    virtual int base_equivalent( BaseHashLink *elem1
                               , TTypePtr elem2 ) const {
        return( *(const Type *)( (HashLink *)elem1 )->data
                == * *(const Type * *)elem2 );
    };

    // determine the bucket elem hashes to
    // parameter is really ( Type * * elem )
    virtual unsigned base_get_bucket( TTypePtr elem ) const {
        return( ( (_HashFnType)hash_fn )( * *(Type * *)elem ) % num_buckets );
    }

public:
    inline WCPtrHashTable( _HashFnType fn
              , unsigned buckets = WC_DEFAULT_HASH_SIZE
              ) : WCValHashTable( (_StorageHashFnType)fn, buckets ) {};

    inline WCPtrHashTable( _HashFnType fn
                  , unsigned buckets
                  , void * (*user_alloc)( size_t )
                  , void (*user_dealloc)( void *, size_t )
                  ) : WCValHashTable( (_StorageHashFnType)fn, buckets
                                    , user_alloc, user_dealloc ) {};

    inline WCPtrHashTable( const WCPtrHashTable &orig )
                : WCValHashTable( orig.hash_fn, orig.num_buckets ) {
        base_construct( &orig );
    };

    virtual ~WCPtrHashTable() {};

    void clearAndDestroy();

    inline WCbool contains( const Type *elem ) const {
        return( WCValHashTable::contains( (const __Type_Ptr)elem ) );
    };

    Type *find( const Type *elem ) const;


    void forAll( void (*fn)(Type *, void*), void *data ) const {
        WCValHashTable::forAll( (_ForAllFnCast)fn, data );
    };

    inline WCbool insert( Type *elem ) {
        return( WCValHashTable::insert( elem ) );
    };

    inline unsigned occurrencesOf( const Type *elem ) const {
        return( WCValHashTable::occurrencesOf( (const __Type_Ptr)elem ) );
    };

    Type *remove( const Type *elem );

    inline unsigned removeAll( const Type *elem ) {
        return( WCValHashTable::removeAll( (const __Type_Ptr)elem ) );
    };

    inline WCPtrHashTable &operator=( const WCPtrHashTable & orig ) {
        base_assign( &orig );
        return( *this );
    };
};


template <class Type>
void WCPtrHashTable<Type>::clearAndDestroy() {
    HashLink *link;
    for( unsigned i = 0; i < buckets(); i++ ) {
        while( ( link = (HashLink *)hash_array[ i ].get() ) != 0 ) {
            delete( (Type *)link->data );
            WCHashDelete( link );
        }
    }
    num_entries = 0;
}


template <class Type>
Type *WCPtrHashTable<Type>::find( const Type *elem ) const {
    unsigned bucket, index;

    HashLink *link = base_find( (const __Type_Ptr)elem
                              , &bucket, &index, FIND_FIRST );
    if( link ) {
        return( (Type *)link->data );
    } else {
        return( 0 );
    }
};


template <class Type>
Type *WCPtrHashTable<Type>::remove( const Type *elem ) {
    HashLink *link = base_remove_but_not_delete(
                                                (const __Type_Ptr)elem );
    if( link != 0 ) {
        Type *ret_ptr = (Type *)link->data;
        WCHashDelete( link );
        return( ret_ptr );
    } else {
        return( 0 );
    }
}




//
// WCValHashSet - hash table for values, values must be unique
//

template <class Type>
class WCValHashSet : public WCValHashTable<Type> {
private:
    // not necessary for a set, contains and remove can be used instead
    unsigned occurrencesOf( const Type &elem ) const;
    unsigned removeAll( const Type &elem );
public:
    inline WCValHashSet( unsigned (*fn)( const Type & )
                         , unsigned buckets = WC_DEFAULT_HASH_SIZE
                         ) : WCValHashTable( fn, buckets ) {};

    inline WCValHashSet( unsigned (*fn)( const Type & )
                       , unsigned buckets
                       , void * (*user_alloc)( size_t )
                       , void (*user_dealloc)( void *, size_t )
                       ) : WCValHashTable( fn, buckets
                                         , user_alloc, user_dealloc ) {};

    inline WCValHashSet( const WCValHashSet &orig
                ) : WCValHashTable( orig.hash_fn, orig.num_buckets ) {
        base_construct( &orig );
    };

    virtual ~WCValHashSet() {};

    inline WCValHashSet &operator=( const WCValHashSet & orig ) {
        base_assign( &orig );
        return( *this );
    };

    inline WCbool insert( const Type & elem ) {
        return( base_set_insert( elem ) != 0 );
    };
};



//
// WCPtrHashSet - hash table for pointers values, values pointed to must
//                be unique
//

template <class Type>
class WCPtrHashSet : public WCPtrHashTable<Type> {
private:
    // not necessary for a set, contains and remove can be used instead
    unsigned occurrencesOf( const Type *elem ) const;
    unsigned removeAll( const Type *elem );

public:
    inline WCPtrHashSet( unsigned (*fn)( const Type & )
                         , unsigned buckets = WC_DEFAULT_HASH_SIZE
                         ) : WCPtrHashTable( fn, buckets ) {};

    inline WCPtrHashSet( unsigned (*fn)( const Type & )
                       , unsigned buckets
                       , void * (*user_alloc)( size_t )
                       , void (*user_dealloc)( void *, size_t )
                       ) : WCPtrHashTable( fn, buckets
                                         , user_alloc, user_dealloc ) {};

    inline WCPtrHashSet( const WCPtrHashSet &orig
                ) : WCPtrHashTable( (_HashFnType)orig.hash_fn
                                  , orig.num_buckets ) {
        base_construct( &orig );
    };

    virtual ~WCPtrHashSet() {};

    inline WCPtrHashSet &operator=( const WCPtrHashSet & orig ) {
        base_assign( &orig );
        return( *this );
    };

    inline WCbool insert( Type * elem ) {
        return( base_set_insert( elem ) != 0 );
    };
};




//
// WCValHashDict - hash dictionary for Keys and Values.  Keys must be unique
// Lookup is done using the Key, and both the Key and Value are stored.
// The Key can be viewed as a handle to the Value.
//

template<class Key, class Value>
class WCValHashDict : public WCValHashSet<WCHashDictKeyVal<Key, Value> > {
protected:
    // the type stored by WCValHashSet
    typedef WCHashDictKeyVal<Key, Value> KeyVal;
    // the prototype of the user's hash function
    typedef unsigned (*_HashFnType)( const Key & );
    // the prototype of the hash function stored by WCValHashSet
    typedef unsigned (*_StorageHashFnType)( const KeyVal & );

    // element equivalence definition (used by base classes), two elements
    // are equivalent if their keys are equivalent
    // prototype is really base_equivalent(HashLink *, KeyVal *)
    virtual int base_equivalent( BaseHashLink *elem1, TTypePtr elem2 ) const {
        return( (const Key)( (HashLink *)elem1 )->data.key
                == ( (const KeyVal *)elem2)->key );
    };

    // equivalence definition for hash dictionaries
    virtual int base_dict_equivalent( const Key key1, const Key key2 ) const {
        return( key1 == key2 );
    };

    // find an key-value element with a matching key
    HashLink *base_dict_find( const Key &search
                            , unsigned *bucket, unsigned *index ) const;

    // return the bucket which elem hashes to (used by base classes)
    // parameter is really ( KeyVal * elem )
    virtual unsigned base_get_bucket( TTypePtr elem ) const {
        return( base_get_dict_bucket( ( (const KeyVal *)elem )->key ) );
    }

    // return the bucket which key hashes to
    inline virtual unsigned base_get_dict_bucket( const Key & key ) const {
        return( ((_HashFnType)hash_fn)( key ) % num_buckets );
    }

public:
    inline WCValHashDict( _HashFnType fn
                        , unsigned buckets = WC_DEFAULT_HASH_SIZE
                        ) : WCValHashSet( (_StorageHashFnType)fn, buckets ) {};

    inline WCValHashDict( _HashFnType fn
                        , unsigned buckets
                        , void * (*user_alloc)( size_t )
                        , void (*user_dealloc)( void *, size_t )
                        ) : WCValHashSet( (_StorageHashFnType)fn, buckets
                                        , user_alloc, user_dealloc ) {};

    inline WCValHashDict( const WCValHashDict &orig
                ) : WCValHashSet( orig ) {};

    virtual ~WCValHashDict() {};

    inline WCbool contains( const Key & key ) const {
        unsigned bucket, index;
        return( base_dict_find( key, &bucket, &index ) != 0 );
    };

    WCbool find( const Key &search, Value &return_val ) const;

    WCbool findKeyAndValue( const Key &search, Key &ret_key
                          , Value &ret_value ) const;

    void forAll( void (*)(Key, Value, void*), void * ) const;

    WCbool insert( const Key & key, const Value & value ) {
        KeyVal key_and_val( key, value );
        return( WCValHashSet::insert( key_and_val ) );
    }

    WCbool remove( const Key &elem );

    inline WCValHashDict &operator=( const WCValHashDict & orig ) {
        base_assign( &orig );
        return( *this );
    };

    Value & operator[]( const Key & key );

    const Value & operator[]( const Key & key ) const;

    friend class WCValHashDictIter;
};


template <class Key, class Value>
WCHashLink<WCHashDictKeyVal<Key, Value> > *WCValHashDict<Key, Value>
                ::base_dict_find( const Key & elem, unsigned *bucket
                           , unsigned *ret_index ) const {
    if( num_buckets == 0 ) {
        return( 0 );
    }
    int index = 0;
    *bucket = base_get_dict_bucket( elem );
    WCIsvSListIter<BaseHashLink> iter( hash_array[ *bucket ] );
    HashLink *link;

    while( ( link = (HashLink *)++iter ) != 0 ) {
        if( base_dict_equivalent( link->data.key, elem ) ) {
            *ret_index = index;
            return( link );
        }
        index++;
    }
    return( 0 );
};


template <class Key, class Value>
WCbool WCValHashDict<Key, Value>::find( const Key &search
                                      , Value &return_val ) const {
    unsigned bucket, index;

    HashLink *link = base_dict_find( search, &bucket, &index );
    if( link ) {
        return_val = link->data.value;
        return( TRUE );
    }
    return( FALSE );
};


template <class Key, class Value>
WCbool WCValHashDict<Key, Value>::findKeyAndValue( const Key &search
                              , Key &return_key, Value &return_val ) const {
    unsigned bucket, index;

    HashLink *link = base_dict_find( search, &bucket, &index );
    if( link ) {
        return_key = link->data.key;
        return_val = link->data.value;
        return( TRUE );
    }
    return( FALSE );
};


template <class Key, class Value>
void WCValHashDict<Key, Value>::forAll( register void (*user_fn)(Key, Value
                                                                , void*)
                                      , void *data ) const {
    WCIsvSListIter<BaseHashLink> iter;
    HashLink *link;

    for( int i = 0; i < num_buckets; i++ ) {
        iter.reset( hash_array[ i ] );
        while( ( link = (HashLink *)++iter ) != 0 ) {
            user_fn( link->data.key, link->data.value, data );
        }
    }
};


template <class Key, class Value>
WCbool WCValHashDict<Key, Value>::remove( const Key &elem ) {
    unsigned bucket, index;

    if( base_dict_find( elem, &bucket, &index ) ) {
        HashLink *link = (HashLink *)hash_array[ bucket ].get( index );
        WCHashDelete( link );
        num_entries--;
        return( TRUE );
    } else {
        return( FALSE );
    }
};


template <class Key, class Value>
Value & WCValHashDict<Key, Value>::operator[]( const Key & key ) {
    unsigned bucket, index;

    HashLink *link = base_dict_find( key, &bucket, &index );
    if( link != 0 ) {
        return( link->data.value );
    } else {
        KeyVal key_and_val( key );
        link = base_set_insert( key_and_val );
        if( link ) {
            return( link->data.value );
        } else {
            // insert failed because allocation failed and out_of_memory
            // exception disabled
            return( *(Value *)0 );
        }
    }
};



template <class Key, class Value>
const Value & WCValHashDict<Key, Value>::operator[]( const Key & key ) const {
    unsigned bucket, index;

    HashLink *link = base_dict_find( key, &bucket, &index );
    if( link != 0 ) {
        return( link->data.value );
    } else {
        base_throw_index_range();
        // key not found, and index_range is disabled
        return( *( const Value *)0 );
    }
};




//
// WCPtrHashDict - hash dictionary for Keys and Values.  Only the pointers
// to the Keys and Values are copied into the dictionary.  Keys must be unique
// Lookup is done using the Key, and both the Key and Value are stored.
// The Key can be viewed as a handle to the Value.
//
// Implementation note:
// WCPtrHashDict inherits from WCValHashDict templated over <void *, void *>.
// This saves most of the hash dictionary code being generated for pointer
// hash dictionaries templated over different types, speeding up compile time,
// and reducing code size.
//

template<class Key, class Value>
class WCPtrHashDict : public WCValHashDict<void *, void *> {
protected:
    // the real type that is stored in the hash dictionary
    typedef WCHashDictKeyVal<Key *, Value *> KeyVal;
    // all pointers are stored as pointers to void so that all pointer hashes
    // inherit from WCValHashDict templated over <void *, void *>
    typedef WCHashDictKeyVal<void *, void *> StoredKeyVal;
    // the hashing function which the user passes in, and what is called
    typedef unsigned (*_HashFnType)( const Key & );
    // the hashing function is stored by WCValHashDict, and this type
    // matches the type WCValHashDict stores
    typedef unsigned (*_StorageHashFnType)( void * const & );
    // the user function passed to the forAll member function is passed
    // to WCValHashDict<void *, void *>::forAll using this cast
    typedef void (*_ForAllFnCast)( void *, void *, void * );
    typedef Key *Key_Ptr;

    // equivalence definition for pointer hash dictionaries
    // prototype is really base_equivalent(HashLink *, KeyVal *)
    virtual int base_equivalent( BaseHashLink *elem1
                               , TTypePtr elem2 ) const {
        return( *(const Key *)( (HashLink *)elem1 )->data.key
                == *(const Key *)( (KeyVal *)elem2 )->key );
    };

    // find an key-value element with a matching key
    virtual int base_dict_equivalent( void *const key1
                                    , void *const key2 ) const {
        return( *(const Key *)key1 == *(const Key *)key2 );
    };

    // return the bucket which elem hashes to (used by base classes)
    // parameter is really ( KeyVal * elem )
    virtual unsigned base_get_bucket( TTypePtr elem ) const {
        return( base_get_dict_bucket( ( (StoredKeyVal *)elem)->key ) );
    }

    // return the bucket which key hashes to
    inline virtual unsigned base_get_dict_bucket( void * const & key ) const {
        return( ((_HashFnType)hash_fn)( *(Key *)key ) % num_buckets );
    }

public:
    inline WCPtrHashDict( _HashFnType fn
                        , unsigned buckets = WC_DEFAULT_HASH_SIZE
                        ) : WCValHashDict( (_StorageHashFnType)fn, buckets ) {};

    inline WCPtrHashDict( _HashFnType fn
                        , unsigned buckets
                        , void * (*user_alloc)( size_t )
                        , void (*user_dealloc)( void *, size_t )
                        ) : WCValHashDict( (_StorageHashFnType)fn, buckets
                                         , user_alloc, user_dealloc ) {};

    inline WCPtrHashDict( const WCPtrHashDict &orig
                ) : WCValHashDict( orig ) {};

    virtual ~WCPtrHashDict() {};

    void clearAndDestroy();

    inline WCbool contains( const Key * key ) const {
        return( WCValHashDict::contains( (const Key_Ptr)key ) );
    };

    Value *find( const Key * ) const;

    Value *findKeyAndValue( const Key * search, Key * &ret_key ) const;

    inline void forAll( void (*user_fn)(Key *, Value *, void*)
                      , void *data ) const {
        WCValHashDict::forAll( (_ForAllFnCast)user_fn, data );
    };

    inline WCbool insert( Key * key, Value * value ) {
        return( WCValHashDict::insert( (const Key_Ptr)key
                                     , (Value * const)value ) );
    };

    Value *remove( const Key * key );

    inline WCPtrHashDict &operator=( const WCPtrHashDict & orig ) {
        base_assign( &orig );
        return( *this );
    };

    inline Value * & operator[]( const Key * key ) {
        return( (Value * &)WCValHashDict::operator[]( (const Key_Ptr)key ) );
    };

    inline Value * const & operator[]( const Key * key ) const {
        return( (Value * const &)WCValHashDict
                                        ::operator[]( (const Key_Ptr)key ) );
    };

};


template <class Key, class Value>
void WCPtrHashDict<Key, Value>::clearAndDestroy() {
    HashLink *link;
    for( unsigned i = 0; i < buckets(); i++ ) {
        while( ( link = (HashLink *)hash_array[ i ].get() ) != 0 ) {
            delete( (Key *)link->data.key );
            delete( (Value *)link->data.value );
            WCHashDelete( link );
        }
    }
    num_entries = 0;
}


template <class Key, class Value>
Value *WCPtrHashDict<Key, Value>::find( const Key * search ) const {
    unsigned bucket, index;

    HashLink *link = base_dict_find( (const Key_Ptr)search, &bucket, &index );
    if( link ) {
        return( (Value *)link->data.value );
    } else {
        return( 0 );
    }
};


template <class Key, class Value>
Value *WCPtrHashDict<Key, Value>::findKeyAndValue( const Key * search
                                                 , Key * &ret_key ) const {
    unsigned bucket, index;

    HashLink *link = base_dict_find( (const Key_Ptr)search, &bucket, &index );
    if( link ) {
        ret_key = (Key *)link->data.key;
        return( (Value *)link->data.value );
    } else {
        return( 0 );
    }
};


template <class Key, class Value>
Value *WCPtrHashDict<Key, Value>::remove( const Key *elem ) {
    unsigned bucket, index;

    if( base_dict_find( (const Key_Ptr)elem, &bucket, &index ) ) {
        HashLink *link = (HashLink *)hash_array[ bucket ].get( index );
        Value *ret_ptr = (Value *)link->data.value;
        WCHashDelete( link );
        num_entries--;
        return( ret_ptr );
    } else {
        return( 0 );
    }
}


#define _WCHASH_H_INCLUDED
#endif
