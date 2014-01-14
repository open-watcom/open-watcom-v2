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
#include <wclist.h>
#include <wclistit.h>
#include <iostream.h>



//
//  This defines the list base next operation.  If there are no elements
//  in the list, NULL is returned.  NULL is also returned if the current
//  position is at the end of the list.
//

_WPRTLINK WCSLink * WCIsvListBase::base_next( const WCSLink * curr, WCbool reset ) const {
    if( reset ) {
        curr = tail;
        if( curr == 0 ) {
            return( 0 );
        }
    } else if( curr == tail ) {
        return( 0 );
    }
    return( curr->link );
};



//
//  This defines the single list base insert operation.
//

_WPRTLINK void WCIsvListBase::base_insert( WCSLink * datum ) {
    if( tail == 0 ) {
        datum->link = datum;
        tail = datum;
    } else {
        datum->link = tail->link;
        tail->link = datum;
    };
    entry_count++;
};



//
//  This defines the list base index check.  If exceptions have been enabled,
//  a range check is thrown.  If not enabled, a reasonable valid value is
//  is picked for the index.
//  If the list is empty, and the index_range or empty_container exceptions
//  have not been enabled, then a value 0 or -1 is returned.
//

_WPRTLINK int WCIsvListBase::base_index_check( int index ) const {
    int entry_index = entry_count - 1;
    if( index < 0 || index > entry_index ) {
        if( entry_count == 0 ) {
            base_throw_empty_container();
        }
        base_throw_index_range();
        if( index < 0 ) {
            index = 0;
        } else {
            index = entry_index;
        };
    }
    return( index );
};



//
//  This defines the list base find operation for single link lists.
//  This routine assumes base_index_check returns <= 0 on an empty list
//  If there are no elements in the list, NULL is returned.
//

_WPRTLINK WCSLink * WCIsvListBase::base_bfind( int index ) const {
    index = base_index_check( index );
    WCSLink * ret_val = tail;
    while( index-- > 0 ) {
        ret_val = ret_val->link;
    }
    return( ret_val );
};



//
//  This defines the list base find operation for single link lists.
//  If there are no elements in the list, NULL is returned.
//

_WPRTLINK WCSLink * WCIsvListBase::base_sfind( int index ) const {
    WCSLink * prev = base_bfind( index );
    if( prev == 0 ) {
        return( 0 );
    }
    return( prev->link );
};



//
//  This defines the list base find operation for double link lists.
//  If there are no elements in the list, NULL is returned.
//

_WPRTLINK WCDLink * WCIsvListBase::base_dfind( int index ) const {
    int entry_index = entry_count - 1;
    if( index <= (entry_index / 2) + 1 ) {
        return( (WCDLink *)base_sfind( index ) );
    }
    WCDLink * ret_val = (WCDLink *)tail;
    index = base_index_check( index );
    index = entry_index - index;
    // if no elements in list index <= 0 here
    while( index-- > 0 ) {
        ret_val = (WCDLink *)ret_val->prev.link;
    }
    return( ret_val );
};



//
//  This defines the list base get operation.  If there are no elements
//  in the list, NULL is returned.
//

_WPRTLINK WCSLink * WCIsvListBase::base_sget( int index ) {
    WCSLink * ret_prev = base_bfind( index );
    if( ret_prev == 0 ) {       // no element in list, base_bfind can throw
        return( 0 );            // exception
    };
    WCSLink * ret_val = ret_prev->link;
    if( ret_val == ret_prev ) {
        tail = 0;
    } else {
        ret_prev->link = ret_val->link;
        if( ret_val == tail ) {
            tail = ret_prev;
        }
    };
    entry_count--;
    return( ret_val );
};



//
//  This defines the double linked list base get operation.
//  If there are no elements in the list, NULL is returned.
//

_WPRTLINK WCDLink * WCIsvListBase::base_dget( int index ) {
    WCDLink * ret_val = base_dfind( index );
    if( ret_val == 0 ) {
        return( 0 );
    }
    WCDLink * ret_prev = (WCDLink *)ret_val->prev.link;
    if( ret_val == ret_prev ) {
        tail = 0;
    } else {
        ret_prev->link = ret_val->link;
        ((WCDLink *)ret_val->link)->prev.link = ret_prev;
        if( ret_val == tail ) {
            tail = ret_prev;
        }
    };
    entry_count--;
    return( ret_val );
};



//
//  This defines the list base index operation.  If there are any
//  elements matching the supplied data, the index of the first
//  matching element is returned.  If the element is not found,
//  -1 is returned.
//
//  Note that this is a list item pointer comparison, and is intended to be
//  used only by intrusive lists.  Value lists must check the data stored
//  within the link.
//

_WPRTLINK int WCIsvListBase::base_index( const WCSLink * datum ) const {
    WCSLink * rover;
    int index = 0;

    rover = base_next( rover, true );
    while( rover != 0 ) {
        if( rover == datum ) {
            return( index );
        }
        rover = base_next( rover, false );
        index++;
    }
    return( -1 );
};



//
//  This defines the list base clear operation.  It disconnects the list
//  elements from the list.  The list object will still exist after
//  this routine is finished.  The list element objects are not cleared.
//

_WPRTLINK void WCIsvListBase::base_clear( void ) {
    tail = 0;
    entry_count = 0;
};



//
//  This defines the list base destroy operation.  It clears the list
//  elements from the list.  The list object will still exist after
//  this routine is finished.  Since we are going to destroy every
//  element, do not worry about maintaining links.
//
//  Data objects pointed to within a list element are not cleared
//  unless the destruction of the element has been provided for
//  by the user.
//

_WPRTLINK void WCIsvListBase::base_destroy( void ) {
    WCSLink * rover;
    WCSLink * next_link;

    rover = base_next( rover, true );
    while( rover != 0 ) {
        next_link = base_next( rover, false );
        WCListDelete( rover );
        rover = next_link;
    }
    base_clear();
};



//
//  This defines the list base destructor.  If there are any problems, it
//  checks for a user supplied indication of an exception handler, and if
//  found, throws an exception.
//
//  Exceptions are cleared before throwing since it is an error to throw
//  an exception while stack unwinding from a destructor.
//
//  If the list is not empty, and no exception can be thrown, the list is
//  cleared.
//

WCIsvListBase::~WCIsvListBase() {
    if( tail != 0 ) {
        base_throw_not_empty();
        // we can't do a destroy here (list elements may not be newed)
        // base_destroy is done in WCValListBase
    }
};



//
//  This defines the routine that advances through a list for an iterator
//  class.
//

_WPRTLINK WCSLink * WCListIterBase::base_advance( int adv_amount ) {
    if( ( curr_list == 0 )||( outside_list&after_last ) ) {
        // no list is associated, or we are after the end of the list already
        base_throw_undef_iter();
        return( 0 );
    }
    if( adv_amount < 1 ) {
        base_throw_iter_range();
        adv_amount = 1;
    }
    WCSLink * list_item = curr_item;
    WCSLink * list_tail = curr_list->tail;
    // if we had iterated to the last element in the list already
    if( list_item == list_tail ) {
        curr_item = 0;
        // now we are after the last element in the list
        outside_list = after_last;
        if( adv_amount > 1 ) {
            base_throw_iter_range();
        }
        return( 0 );
    }
    if( list_item == 0 ) {
        // we were before the first element in the list
        list_item = list_tail;
        outside_list = 0;
    }
    while( adv_amount-- > 0 ) {
        list_item = list_item->link;
        if( list_item == list_tail ) {
            // we are at the last element in the list
            if( adv_amount > 0 ) {
                // we are iterating past the end of the list
                outside_list = after_last;
                curr_item = 0;
                if( adv_amount > 1 ) {
                    // we are iterating more than one element past the end
                    // of the list
                    base_throw_iter_range();
                }
                return( 0 );
            }
        }
    }
    curr_item = list_item;
    return( list_item );
}



//
//  This defines the routine that retreats through a list for an iterator
//  class.
//

_WPRTLINK WCDLink * WCListIterBase::base_retreat( int adv_amount ) {
    // if there was no list associated or we were already before the first
    // element
    if( ( curr_list == 0 )||( outside_list&before_first ) ) {
        base_throw_undef_iter();
        return( 0 );
    }
    if( adv_amount < 1 ) {
        base_throw_iter_range();
        adv_amount = 1;
    }
    WCSLink * list_item = curr_item;
    WCSLink * list_tail = curr_list->tail;
    if( ( list_tail == 0 )
      ||( list_item == list_tail->link ) ) {
        // the list is empty or we were at the first element in the list
        curr_item = 0;
        // now we are before the first element in the list
        outside_list = before_first;
        if( adv_amount > 1 ) {
            base_throw_iter_range();
        }
        return( 0 );
    }
    if( list_item == 0 ) {
        // was after_last and list is non empty
        // new one after end (the beginning)
        list_item = list_tail->link;
        outside_list = 0;
    }
    while( adv_amount-- > 0 ) {
        list_item = ((WCDLink *)list_item)->prev.link;
        if( list_item == list_tail->link ) {
            // we are at the first element in the list
            if( adv_amount > 0 ) {
                // retreat before the first element in the list
                outside_list = before_first;
                curr_item = 0;
                if( adv_amount > 1 ) {
                    base_throw_iter_range();
                }
                return( 0 );
            }
        }
    }
    curr_item = list_item;
    return( (WCDLink *)list_item );
}



//
//  This defines the routine that hits the tail pointer with a new value
//  so that inserts/appends can easily be done in the middle of a list.
//  A zero is returned if there are no items on the list, or the tail is
//  equal to the current item.
//

_WPRTLINK WCSLink * WCListIterBase::base_tail_hit( WCSLink * new_tail ) {
    WCSLink * list_tail = curr_list->tail;
    if( list_tail == new_tail || new_tail == 0 ) {
        list_tail = 0;
    } else {
        curr_list->tail = new_tail;
    }
    return( list_tail );
}



//
//  This defines the routine that hits the tail pointer with a new value
//  so that inserts/appends can easily be done in the middle of a list.
//  A zero is returned if there are no items on the list, or the tail is
//  equal to the current item.
//

_WPRTLINK void WCListIterBase::base_tail_unhit( WCSLink * old_tail ) {
    if( old_tail != 0 ) {
        curr_list->tail = old_tail;
    }
}
