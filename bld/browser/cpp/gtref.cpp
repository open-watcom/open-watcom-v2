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


/*
 * defines RefNode -- used to represent a reference to another node
 */
#include <wstd.h>
#include "assure.h"
#include "gtref.h"
#include "optmgr.h"
#include "wbrwin.h"

PaintInfo * TreeRefPtr::getPaintInfo()
//------------------------------------
{
    OptionManager * mgr = WBRWinBase::optManager();

    return new PaintInfo( mgr->getFunctionOpts().numCalls( _numRefs ) );
}

TreePtr * TreeRefPtr::makeClone( void )
//-------------------------------------
{
    TreeRefPtr * ptr = new TreeRefPtr( getFrom(), getTo() );
    ptr->_numRefs = _numRefs;

    return ptr;
}

//----------------- TreeRefNode -------------------

TreeRefNode::TreeRefNode( TreeWindow * prt, TreeNode * ref, TreeNode * prnod )
    : TreeNode( prt )
    , _ref( ref )
//----------------------------------------------------------------------------
{
    _parents.add( new TreeRefPtr( this, prnod ) );

}

TreeRefNode::~TreeRefNode()
//-------------------------
{
    for( int i = _parents.count(); i > 0; i -= 1 ) {
        delete _parents[ i - 1 ];
    }
}

int TreeRefNode::getLevel( void ) const
//-------------------------------------
{
    //NYI -- take level out of the object

    TreePtrList& prts = (TreePtrList&) _parents;    //FIXME -- cast away const

    short level = -1;
    for( int i = prts.count(); i > 0; i -= 1 ) {
        level = (short) maxInt( level, prts[ i - 1 ]->getTo()->getLevel() + 1 );
    }
    return level;
}

TreePtr* TreeRefNode::getPtr( TreeList_T list, int index )
//--------------------------------------------------------
{
    REQUIRE( list == ParentList && index == 0, "TreeRefNode::getPtr bad" );
    list = list;        // to avoid a warning

    return _parents[ index ];
}

TreeNode* TreeRefNode::getNode( TreeList_T list, int index )
//----------------------------------------------------------
{
    switch( list ) {
    case ParentList:
        return _parents[ index ]->getTo();
    case FlatList:
        return _ref->getNode( list, index );
    default:
        return NULL;
    }
}

int TreeRefNode::getCount( TreeList_T list )
//------------------------------------------
{
    switch( list ) {
    case ParentList:
        return _parents.count();
    case FlatList:
        return _ref->getCount( list );
    default:
        return 0;
    }
}

void TreeRefNode::swapNode( TreeList_T list, TreeNode * from, TreeNode * to )
//---------------------------------------------------------------------------
{
    int i;

    switch( list ) {
    case ParentList:
        for( i = _parents.count(); i > 0; i -= 1 ) {
            if( _parents[ i - 1 ]->getTo() == from ) {
                _parents[ i - 1 ]->setTo( to );
                break;
            }
        }
        break;
    default:
        REQUIRE( 0, "TreeRefNode::swapNode -- bad list" );
    }
}

TreeNode * TreeRefNode::removeNode( TreeList_T list, TreeNode * node )
//--------------------------------------------------------------------
{
    REQUIRE( list == ParentList, "TreeRefNode::removeNode wrong list" );
    list=list;  // to avoid a warning

    for( int i = _parents.count(); i > 0; i -= 1 ) {
        if( _parents[ i -  1 ]->getTo() == node ) {
            TreePtr * ret = _parents.removeAt( i - 1 );
            delete ret;
            return node;
        }
    }

    return NULL;
}

void TreeRefNode::addPtr( TreeList_T list, TreePtr * ptr )
//--------------------------------------------------------
{
    REQUIRE( list == ParentList, "TreeRefNode::addPtr to wrong list" );
    list=list;  // to avoid a warning

    _parents.add( ptr );
}
