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


// SETIMPL -- implementation for sets
//
//
// Set is implmented as a ring of set items, allocated on insertion.

#include "setimpl.h"


/////////////////////////////////////////////////////////////////////////////
//
// _SetItemImpl -- set item (internal)
//
/////////////////////////////////////////////////////////////////////////////


void* _SetItemImpl::insertRing  // LINK IN ITEM AFTER ITEM
    ( _SetItemImpl*& hdr        // - ring header
    , _SetItemImpl* pred )      // - preceding item
{
    if( pred == 0 ) {
        if( hdr == 0 ) {
            _next = this;
        } else {
            _next = hdr->_next;
            hdr->_next = this;
        }
        hdr = this;
    } else {
        _next = pred->_next;
        pred->_next = this;
        if( hdr == pred ) {
            hdr = this;
        }
    }
    return _item;
}


static _SetItemImpl* _SetItemImpl::newItem // ALLOCATE NEW SET ITEM
    ( void* item )              // - item
{
    _SetItemImpl* nw = new _SetItemImpl();
    nw->_item = item;
    return nw;
}


/////////////////////////////////////////////////////////////////////////////
//
// _SetImpl -- set
//
/////////////////////////////////////////////////////////////////////////////


_SetImpl::_SetImpl              // CONSTRUCTOR
    ( void )
    : _header( 0 )
{
}


_SetImpl::~_SetImpl             // DESTRUCTOR
    ( void )
{
    while( _header != 0 ) {
        erase( 0 );
    }
}


int _SetImpl::empty             // TEST IF EMPTY SET
    ( void )
    const
{
    return _header == 0;
}


void _SetImpl::erase            // ERASE (indexed)
    ( int index )               // - index of item
{
    _SetItemImpl* item = _header;
    _SetItemImpl* prev;
    if( item != 0 ) {
        for( ; ; --index ) {
            prev = item;
            item = item->_next;
            if( index == 0 ) {
                if( prev == item ) {
                    _header = 0;
                } else {
                    prev->_next = item->_next;
                    if( _header == item ) {
                        _header = prev;
                    }
                }
                delete item;
                break;
            }
            if( item == _header ) break;
        }
    }
}


void _SetImpl::erase            // ERASE (item)
    ( void* old )               // - item
{
    _SetItemImpl* item = _header;
    _SetItemImpl* prev;
    if( item != 0 ) {
        for( ; ; ) {
            prev = item;
            item = item->_next;
            if( old == item->_item ) {
                if( prev == item ) {
                    _header = 0;
                } else {
                    prev->_next = item->_next;
                    if( _header == item ) {
                        _header = prev;
                    }
                }
                delete item;
                break;
            }
            if( item == _header ) break;
        }
    }
}


void* _SetImpl::first           // GET FIRST ITEM
    ( void )
    const
{
    _SetItemImpl* item = _header;
    void* retn;
    if( item == 0 ) {
        retn = 0;
    } else {
        retn = item->_next->_item;
    }
    return retn;
}


void* _SetImpl::insert          // INSERT AN ITEM
    ( void* item )              // - item
{
    _SetItemImpl* nw = _SetItemImpl::newItem( item );
    return nw->insertRing( _header, 0 );
}


void* _SetImpl::insert          // INSERT AN ITEM
    ( void* item                // - item
    , void* pred )              // - preceeding item
{
    _SetItemImpl* pred_item;
    if( pred == 0 ) {
        pred_item = 0;
    } else {
        for( pred_item = _header; ; pred_item = pred_item->_next ) {
            if( pred_item->_item == item ) break;
            if( pred_item == _header ) {
                pred_item = 0;
                break;
            }
        }
    }
    _SetItemImpl* nw = _SetItemImpl::newItem( item );
    return nw->insertRing( _header, pred_item );
}


void* _SetImpl::last            // GET LAST ITEM
    ( void )
    const
{
    _SetItemImpl* item = _header;
    void* retn;
    if( item == 0 ) {
        retn = 0;
    } else {
        retn = item->_item;
    }
    return retn;
}


/////////////////////////////////////////////////////////////////////////////
//
// _SetIterImpl -- set iterator
//
/////////////////////////////////////////////////////////////////////////////


_SetIterImpl::_SetIterImpl      // CONSTRUCTOR
    ( _SetImpl const & set )    // - set
    : _set( (_SetImpl&)set )
{
    _curr = _set._header;
    if( _curr == 0 ) {
        _index = -1;
    } else {
        _curr = _curr->_next;
        _index = 0;
    }
}


void* _SetIterImpl::operator *  // GET CURRENT ELEMENT
    ( void )
    const
{
    void* retn;
    if( _curr == 0 ) {
        retn = 0;
    } else {
        retn = _curr->_item;
    }
    return retn;
}


_SetIterImpl& _SetIterImpl::operator++ // MOVE AHEAD ONE ITEM
    ( void )
{
    if( _curr != 0 ) {
        if( _curr == _set._header ) {
            _index = -1;
            _curr = 0;
        } else {
            ++ _index;
            _curr = _curr->_next;
        }
    }
    return *this;
}


void _SetIterImpl::erase        // ERASE, MOVE AHEAD
    ( void )
{
    int index = _index;
    if( index != -1 ) {
        ++(*this);
        _set.erase( index );
        if( _index != -1 ) {
            _index = index;
        }
    }
}


int _SetIterImpl::index         // GET INDEX OF CURRENT ITEM
    ( void )
    const
{
    return _index;
}
