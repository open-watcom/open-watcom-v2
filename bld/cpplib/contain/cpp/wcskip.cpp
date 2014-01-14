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


#include "variety.h"
#include <stdlib.h>
#include <wcskip.h>



const int WCSkipBitsInRandom = 15;

_WPRTLINK unsigned WCSkipNonTempBase::randomsLeft = WCSkipBitsInRandom / 2;
_WPRTLINK unsigned WCSkipNonTempBase::randomBits = rand();



//
//
//

_WPRTLINK WCSkipNonTempBase::WCSkipNonTempBase( unsigned prob, unsigned max_ptrs,
                                      WCbool duplicates )
                  : alloc_fn( 0 ), dealloc_fn( 0 ), level( 0 ),
                    allowDuplicates( duplicates ), num_entries( 0 ) {
    base_init( prob, max_ptrs );
}



//
//
//

_WPRTLINK WCSkipNonTempBase::WCSkipNonTempBase( unsigned prob, unsigned max_ptrs,
                                      void * (*user_alloc)( size_t ),
                                      void (*user_dealloc)( void *, size_t ),
                                      WCbool duplicates )
                  : alloc_fn( user_alloc ), dealloc_fn( user_dealloc ),
                    level( 0 ), allowDuplicates( duplicates ),
                    num_entries( 0 ) {
    base_init( prob, max_ptrs );
}



//
//
//

_WPRTLINK void WCSkipNonTempBase::base_assign( const WCSkipNonTempBase * orig ) {
    if( this != orig ) {
        clear();
        max_ptrs_in_node = orig->max_ptrs_in_node;
        base_init_header();
        base_construct( orig );
    };
};



//
//
//

_WPRTLINK void WCSkipNonTempBase::base_construct( const WCSkipNonTempBase * orig ) {
    alloc_fn = orig->alloc_fn;
    dealloc_fn = orig->dealloc_fn;
    max_ptrs_in_node = orig->max_ptrs_in_node;
    probability = orig->probability;
    WCExcept::base_construct( orig );
    node_ptr curr = orig->header->forward[ 0 ];
    while( curr != 0 && base_insert_copy( curr ) ) {
        curr = curr->forward[ 0 ];
    }
}



//
//
//

_WPRTLINK WCSkipListPtrs * WCSkipNonTempBase::base_find( TTypePtr elem ) const {
    node_ptr curr;
    node_ptr next;
    curr = header;
    int k = level;
    while( k >= 0 ) {
        next = curr->forward[ k ];
        if( next == 0 || !base_less( next, elem ) ) {
            k--;
        } else {
            curr = next;
        }
    }
    if( next != 0 && base_equiv( next, elem ) ) {
        return( next );
    }
    return( 0 );
}



//
// Find an element in the skip list, updating the update array for the
// insert and remove functions
//

_WPRTLINK WCSkipListPtrs * WCSkipNonTempBase::base_find_with_update( TTypePtr elem,
                                                node_ptr update[] ) const {
    node_ptr curr;
    node_ptr next;
    curr = header;
    int k = level;
    while( k >= 0 ) {
        next = curr->forward[ k ];
        if( next == 0 || !base_less( next, elem ) ) {
            update[ k ] = curr;
            k--;
        } else {
            curr = next;
        }
    }
    if( next != 0 && base_equiv( next, elem ) ) {
        return( next );
    }
    return( 0 );
}



//
//
//

void WCSkipNonTempBase::base_init( unsigned prob, unsigned max_ptrs ) {
    if( max_ptrs > WCSKIPLIST_MAX_PTRS ) {
        max_ptrs = WCSKIPLIST_MAX_PTRS;
    } else if( max_ptrs < 1 ) {
        max_ptrs = 1;
    }
    max_ptrs_in_node = max_ptrs;
    if( prob != WCSKIPLIST_PROB_QUARTER && prob != WCSKIPLIST_PROB_HALF ) {
        prob = WCSKIPLIST_PROB_QUARTER;
    }
    probability = prob;
    base_init_header();
}



//
//
//

_WPRTLINK WCSkipListPtrs * WCSkipNonTempBase::base_insert( TTypePtr elem ) {
    int k;
    node_ptr curr;
    node_ptr found;
    node_ptr new_node;
    node_ptr update[ WCSKIPLIST_MAX_PTRS ];

    // find where to insert elem
    found = base_find_with_update( elem, update );

    if( !allowDuplicates && found != 0 ) {
        // duplicates are not allowed for sets and dictionaries
        base_throw_not_unique();
        return( 0 );
    }

    // insert elem after all elements less than elem
    k = base_random_level();
    if( k > level ) {
        // Make sure the new node has a maximum level more than one than the
        // current maximum level in the skip list.
        k = ++level;
        update[ k ] = header;
    }

    // allocate a new node, initailized with elem
    new_node = base_new_node( elem, k );
    if( new_node == 0 ) {
        base_throw_out_of_memory();
        return( 0 );
    }

    // link new_node into the skip list
    do {
        curr = update[ k ];
        new_node->forward[ k ] = curr->forward[ k ];
        curr->forward[ k ] = new_node;
    } while( --k >= 0 );
    num_entries++;
    return( new_node );
}



//
//
//

_WPRTLINK unsigned WCSkipNonTempBase::base_occurrencesOf( TTypePtr elem ) const {
    unsigned count = 0;
    node_ptr curr = base_find( elem );

    if( curr != 0 ) {
        do {
            count++;
            curr = curr->forward[ 0 ];
        } while( curr != 0 && base_equiv( curr, elem ) );
    }
    return( count );
};



//
//
//

_WPRTLINK int WCSkipNonTempBase::base_random_level() {
    int level = 0;
    int b;
    do {
        b = randomBits&probability;
        if( !b ) {
            level++;
        }
        randomBits >>= 2;
        if( --randomsLeft == 0 ) {
            randomBits = rand();
            randomsLeft = WCSkipBitsInRandom/2;
        }
    } while( !b );
    return( ( level > max_ptrs_in_node - 1 ) ? max_ptrs_in_node - 1 : level );
}



//
//
//

_WPRTLINK unsigned WCSkipNonTempBase::base_removeAll( TTypePtr elem ) {
    int k;
    int m;
    int i;
    node_ptr curr;
    node_ptr next;
    node_ptr delete_node;
    unsigned count = 0;
    node_ptr update[ WCSKIPLIST_MAX_PTRS ];

    delete_node = base_find_with_update( elem, update );
    if( delete_node ) {
        // found first node to be deleted (delete_node)
        node_ptr last_delete_node = delete_node;

        // find the last node to be deleted
        for(;;) {
            next = last_delete_node->forward[ 0 ];
            if( next == 0 || !base_equiv( next, elem ) ) break;
            last_delete_node = next;
        }

        // relink the skip list removing all elements equal to elem
        m = level;
        for( k = 0;
             ( ( k <= m ) &&( ( curr = update[ k ] )->forward[ k ] != 0 )
             &&( base_equiv( curr->forward[ k ], elem ) ) );
             k++ ) {

            // make the update pointers to be pointers to the first node to
            // be deleted at each level, for base_delete_node, below
            update[ k ] = update[ k ]->forward[ k ];

            next = curr->forward[ k ];
            while( next != 0 && base_equiv( next, elem ) ) {
                next = next->forward[ k ];
            };
            curr->forward[ k ] = next;
        }

        // now use the update pointers as pointers to the first node to
        // be deleted (or the first node after the nodes being deleted) at
        // each level, so that we can figure out how many pointers to pass
        // to the base_delete_node function
        node_ptr after_delete_nodes = last_delete_node->forward[ 0 ];
        do {
            count++;
            next = delete_node->forward[ 0 ];
            for( i = 0; ( ( i < k )&&( update[ i ] == delete_node ) ); i++ ) {
                update[ i ] = delete_node->forward[ i ];
            }
            base_delete_node( delete_node, i - 1 );
            delete_node = next;
        } while( delete_node != after_delete_nodes );

        // update the number of levels used in the skip list
        while( header->forward[ m ] == 0 && m > 0 ) {
            m--;
        }
        level = m;
        num_entries -= count;
    }
    return( count );
}



//
//
//

_WPRTLINK WCSkipListPtrs *WCSkipNonTempBase::base_remove_but_not_delete( TTypePtr elem,
                                                      int &num_ptrs_in_node ) {
    int k;
    int m;
    node_ptr found;
    node_ptr curr;
    node_ptr update[ WCSKIPLIST_MAX_PTRS ];

    found = base_find_with_update( elem, update );

    if( !found ) {
        return( 0 );
    }
    m = level;
    // found is the element in the skip list which is being removed
    for( k = 0;
         ( k <= m )&&( ( curr = update[ k ] )->forward[ k ] == found );
         k++ ) {
        curr->forward[ k ] = found->forward[ k ];
    }

    // the above loop terminated when k was one higher than the number of
    // levels to relink, and the number of levels to relink is the number
    // of levels in the node being removed
    num_ptrs_in_node = k - 1;

    // update the highest level used in the skip list
    while( m > 0 && header->forward[ m ] == 0 ) {
        m--;
    }
    level = m;
    num_entries--;
    return( found );
}



//
// In order to be able to pass the number of pointers to the base_delete_node
// function, the following algorithm, was used:
//
// for( k = 0; k <= level; k++ ) {
//    curr = header node
//    while( curr->next != 0 ) {
//        if( curr->next is excactly level k ) {
//            delete it, without relinking
//        }
//    }
// }
//
// This algorighm was chosen since it does not require use of the
// update array, and requires no relinking.
//

_WPRTLINK void WCSkipNonTempBase::clear() {
    node_ptr curr;
    node_ptr next;
    node_ptr delete_node;
    unsigned k = 0;

    if( dealloc_fn ) {
        // This loop is used to find the number of ptrs in the elements while
        // they are being cleared.  If there is no user deallocation fn,
        // there is no need to know how many pointers are in each element.
        //
        // This part of the algorthm does not work at the highest level
        // of pointers, since we look at the pointer one level up, the loop
        // later in this function clears the highest level.
        for( k = 0; k < level; k++ ) {
            curr = header;
            next = curr->forward[ k ];

            while( next != 0 ) {
                if( curr->forward[ k + 1 ] != next ) {
                    delete_node = next;
                    next = next->forward[ k ];
                    base_delete_node( delete_node, k );
                } else {
                    curr = next;
                    next = curr->forward[ k ];
                }
            }
        }
    }
    // clear the pointers at the highest level,
    // *OR* if no dealloc_fn, pass 1 as number of pointers to
    // base_delete_node, regardless of how many pointers there really are.
    if( k <= level ) {
        curr = header->forward[ k ];
        while( curr != 0 ) {
            next = curr->forward[ k ];
            base_delete_node( curr, k );
            curr = next;
        }
    }
    base_init_header();
    level = 0;
    num_entries = 0;
}



//
//
//

_WPRTLINK WCbool WCSkipNonTempBase::remove( TTypePtr elem ) {
    int num_ptrs_in_node;

    node_ptr node = base_remove_but_not_delete( elem, num_ptrs_in_node );
    if( node ) {
        base_delete_node( node, num_ptrs_in_node );
        return( true );
    }
    return( false );
}
