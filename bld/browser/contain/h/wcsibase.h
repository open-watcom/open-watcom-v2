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
//  wcsibase.h  Base Class Definitions for the WATCOM Container Skip List
//              Iterator Classes
//
//  Copyright by WATCOM International Corp. 1988-1993.  All rights reserved.
//

#ifndef _WCSIBASE_H_INCLUDED

#ifndef __cplusplus
#error wcsibase.h is for use with C++
#endif



//
// The WCSkipListIterBase is the base iterator class for all skip list
// iterators.
//
// Objects of this type should never be instantiated: use the appropriate
// skip list iterator defined in wcskipit.h instead.
//
// WCIterExcept is used as a base class to provide exception handling
//

template<class Type>
class WCSkipListIterBase : public WCIterExcept {
private:
    typedef WCSkipListPtrs *    node_ptr;
    const WCSkipListBase<Type> *curr_skip_list;

protected:
    node_ptr                    curr;

private:
    int                         at_end;

    WCbool base_advance();

protected:
    inline WCSkipListNode<Type> *base_curr_node() const {
        if( curr != 0 ) {
            return( WCSkipListBase<Type>::base_whole_node( curr ) );
        } else {
            return( 0 );
        }
    };

public:
    inline WCSkipListIterBase() : curr_skip_list( 0 )
                               , curr( 0 ), at_end( 0 ) {};

    inline WCSkipListIterBase( const WCSkipListBase<Type>& skip_list )
                : curr_skip_list( &skip_list ), curr( 0 ), at_end( 0 ) {};

    inline ~WCSkipListIterBase() {};

    inline const WCSkipListBase<Type> *container() const {
        if( curr_skip_list == 0 ) {
            base_throw_undef_iter();
        }
        return( curr_skip_list );
    };

    Type current() const;

    inline void reset() {
        curr = 0;
        at_end = 0;
    };

    inline void reset( const WCSkipListBase<Type> &skip_list ) {
        curr_skip_list = &skip_list;
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
WCbool WCSkipListIterBase<Type>::base_advance() {
    if( ( curr_skip_list == 0 )||( at_end ) ) {
        at_end = 1;
        base_throw_undef_iter();
        return( FALSE );
    }
    if( curr == 0 ) {
        curr = curr_skip_list->header;
    }
    curr = curr->forward[ 0 ];
    if( curr == 0 ) {
        at_end = 1;
        return( FALSE );
    } else {
        return( TRUE );
    }
};


template <class Type>
Type WCSkipListIterBase<Type>::current() const {
    if( curr == 0 ) {
        base_throw_undef_item();
        Type temp;
        return( temp );
    }
    return( base_curr_node()->data );
};

#define _WCSIBASE_H_INCLUDED
#endif
