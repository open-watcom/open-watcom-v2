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


#ifndef __SETIMPL_H__
#define __SETIMPL_H__

// SETIMPL -- set implementation

class _SetImpl;
class _SetIterImpl;


/////////////////////////////////////////////////////////////////////////////
class _SetItemImpl {            // _SetItemImpl -- set item
/////////////////////////////////////////////////////////////////////////////

public:

void* _SetItemImpl::insertRing  // LINK IN ITEM AFTER ITEM
    ( _SetItemImpl*& hdr        // - ring header
    , _SetItemImpl* pred )      // - preceding item
;
static _SetItemImpl* _SetItemImpl::newItem // ALLOCATE NEW SET ITEM
    ( void* item )              // - item
;

protected:
    void* _item;                // set item
    _SetItemImpl* _next;        // next item

friend class _SetImpl;
friend class _SetIterImpl;

};


/////////////////////////////////////////////////////////////////////////////
class _SetImpl {                // _SetImpl -- set
/////////////////////////////////////////////////////////////////////////////
public:

_SetImpl::_SetImpl              // CONSTRUCTOR
    ( void )
;
_SetImpl::~_SetImpl             // DESTRUCTOR
    ( void )
;
int _SetImpl::empty             // TEST IF EMPTY SET
    ( void )
    const
;
void _SetImpl::erase            // ERASE (indexed)
    ( int index )               // - index of item
;
void _SetImpl::erase            // ERASE (item)
    ( void* old )               // - item
;
void* _SetImpl::first           // GET FIRST ITEM
    ( void )
    const
;
void* _SetImpl::insert          // INSERT AN ITEM
    ( void* item )              // - item
;
void* _SetImpl::insert          // INSERT AN ITEM
    ( void* item                // - item
    , void* pred )              // - preceeding item
;
void* _SetImpl::last            // GET LAST ITEM
    ( void )
    const
;

protected:
    _SetItemImpl* _header;      // set header


friend class _SetIterImpl;

};


/////////////////////////////////////////////////////////////////////////////
class _SetIterImpl {            // _SetIterImpl -- set iterator
/////////////////////////////////////////////////////////////////////////////
public:

_SetIterImpl::_SetIterImpl      // CONSTRUCTOR
    ( _SetImpl const & set )    // - set
;
void* _SetIterImpl::operator *  // GET CURRENT ELEMENT
    ( void )
    const
;
_SetIterImpl& _SetIterImpl::operator++ // MOVE AHEAD ONE ITEM
    ( void )
;
void _SetIterImpl::erase        // ERASE, MOVE AHEAD
    ( void )
;
int _SetIterImpl::index         // GET INDEX OF CURRENT ITEM
    ( void )
    const
;

protected:
    _SetImpl &_set;             // original set
    _SetItemImpl* _curr;        // current element
    int _index;                 // index of current item

};

#endif
