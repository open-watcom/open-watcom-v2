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


// Non-Templated code for base implementation of WATCOM Hash Container
// classes

#include <wchash.h>
#include <wchiter.h>


void WCHashBase::base_construct( const WCHashBase * orig ) {
    WCExcept::base_construct( orig );
    num_buckets = orig->num_buckets;
    num_entries = 0;
    if( num_buckets > 0 ) {
        if( !hash_array ) {
            // hash_array must be allocated or NULL before
            // base_construct is called
            num_buckets = 0;
            base_throw_out_of_memory();
        } else {
            WCIsvSListIter<BaseHashLink> iter;
            BaseHashLink *link;
            BaseHashLink *new_link;
            // copy elements, making sure num_entries is always correct
            for( int i = 0; i < num_buckets; i++ ) {
                iter.reset( orig->hash_array[ i ] );
                while( ( link = ++iter ) != 0 ) {
                    new_link = base_copy_link( link );
                    if( new_link == 0 ) {
                        base_throw_out_of_memory();
                        return;
                    }
                    hash_array[ i ].append( new_link );
                    num_entries++;
                }
            }
        }
    }
};


//
// Attempt to find an element equivalent to elem.  find_type should be
// FIND_FIRST except when searching for the next element when an equivalent
// element was already found.  If an element is found then the HashLink
// containing the equivalent found element is returned, bucket is assigned
// the bucket elem hashes into, and ret_index is assigned the index within
// bucket of the found element.  0 is returned if the element is not found.
// To find more than one equivalent element, on successive searches, pass
// the search element, the bucket and index returned by the previous search,
// and NEXT_MULT_FIND.
//

WCSLink *WCHashBase::base_find( TTypePtr elem
                                , unsigned *bucket, unsigned *ret_index
                                , find_type type ) const {
    if( num_buckets == 0 ) {
        return( 0 );
    }

    int index;
    if( type == FIND_FIRST ) {
        *bucket = base_get_bucket( elem );
        index = 0;
    }
    WCIsvSListIter<BaseHashLink> iter( hash_array[ *bucket ] );
    if( type == NEXT_MULT_FIND ) {
        index = *ret_index;
        // advance the iterator to a previously found element
        if( ( index > 0 )
          &&( !( iter += index ) ) ) {
            return( 0 );
        }
    };

    BaseHashLink *link;
    while( ( link = ++iter ) != 0 ) {
        if( base_equivalent( link, elem ) ) {
            *ret_index = index;
            return( link );
        }
        index++;
    }
    return( 0 );
};


//
// initialization common to the constructors
//

void WCHashBase::base_init( unsigned buckets ) {
    num_entries = 0;
    if( buckets == 0 ) {
        buckets = 1;
    }
    hash_array = new WCIsvSList<BaseHashLink>[ buckets ];
    if( hash_array == 0 ) {
        buckets = 0;
    }
    num_buckets = buckets;
};


WCSLink *WCHashBase::base_remove_but_not_delete( TTypePtr elem ) {
    unsigned bucket, index;

    if( base_find( elem, &bucket, &index, FIND_FIRST ) ) {
        BaseHashLink *link = (BaseHashLink *)hash_array[ bucket ].get( index );
        num_entries--;
        return( link );
    } else {
        return( 0 );
    }
};


//
// The insert function for HashSets
//

WCSLink * WCHashBase::base_set_insert( TTypePtr elem ) {
    unsigned bucket, index;

    if( base_find( elem, &bucket, &index, FIND_FIRST ) == 0 ) {
        if( num_buckets == 0 ) {
            base_throw_out_of_memory();
            return( 0 );
        }
        BaseHashLink *link = WCHashNew( elem );
        if( link == 0 ) {
            base_throw_out_of_memory();
            return( 0 );
        }
        hash_array[ bucket ].append( link );
        num_entries++;
        return( link );
    } else {
        // find succeeded: an equivalent element was previously in the hash
        // set
        base_throw_not_unique();
        return( 0 );
    }
};


WCHashBase::~WCHashBase() {};


unsigned WCHashBase::bitHash( const void * ptr, size_t size ) {
    char *curr = (char *)ptr;
    long count = *curr;

    for( size_t i = 1; i < size; i++ ) {
        count += 37 * count + *curr;
        curr++;
    }
    return( (unsigned)( ( ( count * 19L ) + 12451L ) % 8882693L ) );
};


void WCHashBase::clear() {
    BaseHashLink *link;
    for( unsigned i = 0; i < num_buckets; i++ ) {
        while( ( link = hash_array[ i ].get() ) != 0 ) {
            WCHashDelete( link );
        }
    }
    num_entries = 0;
}


WCbool WCHashBase::insert( TTypePtr elem ) {
    if( num_buckets == 0 ) {
        base_throw_out_of_memory();
        return( FALSE );
    }
    BaseHashLink *link = WCHashNew( elem );
    if( link == 0 ) {
        base_throw_out_of_memory();
        return( FALSE );
    }
    hash_array[ base_get_bucket( elem ) ].append( link );
    num_entries++;
    return( TRUE );
}


unsigned WCHashBase::occurrencesOf( TTypePtr elem ) const {
    unsigned bucket, index, count = 0;

    if( base_find( elem, &bucket, &index, FIND_FIRST ) ) {
        do {
            count++;
            index++;
        } while( base_find( elem, &bucket, &index, NEXT_MULT_FIND ) );
    }
    return( count );
}


unsigned WCHashBase::removeAll( TTypePtr elem ) {
    unsigned bucket, index, count = 0;

    if( base_find( elem, &bucket, &index, FIND_FIRST ) ) {
        BaseHashLink *link;
        do {
            link = hash_array[ bucket ].get( index );
            WCHashDelete( link );
            count++;
        } while( base_find( elem, &bucket, &index, NEXT_MULT_FIND ) );
    }
    num_entries -= count;
    return( count );
}


void WCHashBase::resize( unsigned buckets ) {
    if( buckets > 0 ) {
        WCIsvSList<BaseHashLink> *new_hash_array;

        new_hash_array = new WCIsvSList<BaseHashLink>[ buckets ];
        if( new_hash_array == 0 ) {
            base_throw_out_of_memory();
            return;
        }

        unsigned old_buckets = num_buckets;
        num_buckets = buckets;

        BaseHashLink *link;

        // we need to rehash all the values on a resize
        for( int i = 0; i < old_buckets; i++ ) {
            while( ( link = hash_array[ i ].get() ) != 0 ) {
                new_hash_array[ base_get_bucket_for_link( link ) ]
                                                        .append( link );
            }
        }

        delete [] hash_array;
        hash_array = new_hash_array;
    } else {
        base_throw_zero_buckets();
    }
}


//
// Hash iterator base functions
//

WCbool WCHashIterBase::base_advance() {
    if( ( curr_hash == 0 )||( at_end ) ) {
        at_end = 1;
        base_throw_undef_iter();
        return( FALSE );
    }
    if( curr_hash->num_buckets == 0 ) {
        at_end = 1;
        return( FALSE );
    }
    while( 1 ) {
        if( ++bucket_iter != 0 ) {
            return( TRUE );
        }
        curr_bucket++;
      if( curr_bucket >= curr_hash->num_buckets ) break;
        bucket_iter.reset( curr_hash->hash_array[ curr_bucket ] );
    };
    at_end = 1;
    return( FALSE );
};


void WCHashIterBase::reset() {
    if( ( curr_hash != 0 )
      &&( curr_hash->num_buckets != 0 ) ) {
        bucket_iter.reset( curr_hash->hash_array[ 0 ] );
    }
    curr_bucket = 0;
    at_end = 0;
};
