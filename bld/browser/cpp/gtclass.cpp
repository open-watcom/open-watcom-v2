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


#include <wmenu.hpp>
#include <wmenuitm.hpp>

#include "assure.h"
#include "dbmgr.h"
#include "gtclass.h"
#include "gtclsopt.h"
#include "keysym.h"
#include "module.h"
#include "optmgr.h"
#include "symbol.h"
#include "util.h"
#include "brwhelp.h"

const int PTRPOOLSIZE = 128;
const int NODEPOOLSIZE = 64;

#pragma warning 549 9           // sizeof contains compiler genned info.
MemoryPool TreeClassPtr::_pool( sizeof( TreeClassPtr ), "TreeClassPtr",
                                    PTRPOOLSIZE );
MemoryPool TreeClassNode::_pool( sizeof( TreeClassNode ), "TreeClassNode",
                                    NODEPOOLSIZE );
#pragma warning 549 3

TreeClassPtr::TreeClassPtr( TreeClassWindow * prt, TreeClassNode * from,
                            TreeClassNode * to, dr_access acc,
                            dr_virtuality virt )
                : DerivationPtr( to, acc, virt )
                , TreePtr( from, to )
                , _parent( prt )
//----------------------------------------------------------------------
{
}

void * TreeClassPtr::operator new( size_t )
//-----------------------------------------
{
    return( _pool.alloc() );
}

void TreeClassPtr::operator delete( void * mem )
//---------------------------------------------
{
    _pool.free( mem );
}

void TreeClassPtr::adjustTo( ClassLattice * cls )
//-----------------------------------------------
{
    setTo( (TreeNode *)(TreeClassNode *) cls );
    DerivationPtr::adjustTo( cls );
}

TreePtr * TreeClassPtr::makeClone()
//----------------------------------
{
    TreeClassPtr * ptr = new TreeClassPtr;

    *ptr = *this;
    return ptr;
}

PaintInfo * TreeClassPtr::getPaintInfo( void )
//--------------------------------------------
{
    OptionManager * mgr = WBRWinBase::optManager();

    return new PaintInfo( mgr->getClassOpts().value( _access, _virtuality ) );
}

TreeClassNode::TreeClassNode( TreeWindow * prt, Symbol * sym )
            : TreeNode( prt )
            , ClassLattice( sym, TRUE )
//------------------------------------------------------------
{
};

TreeClassNode::TreeClassNode( TreeWindow * prt, dr_handle hdl, Module * mod,
                              char * nm, ClassList * l, dr_access acc,
                              dr_virtuality v, int lvl )
                : ClassLattice( hdl, mod, nm, l, acc, v, TRUE, lvl )
                , TreeNode( prt )
//--------------------------------------------------------------------------
{
}

void * TreeClassNode::operator new( size_t )
//------------------------------------------
{
    return( _pool.alloc() );
}

void TreeClassNode::operator delete( void * mem )
//-----------------------------------------------
{
    _pool.free( mem );
}

int TreeClassNode::getLevel( void ) const
//---------------------------------------
{
    return ClassLattice::_level;
}

PaintInfo * TreeClassNode::getPaintInfo( void )
//---------------------------------------------
{
    Color       clr = ColorBlack;
    WLineStyle  stl = LS_PEN_SOLID;
    unsigned    thk = 1;

    return new PaintInfo( clr, thk, stl );
}

bool TreeClassNode::isRelated( TreeNode * node )
//----------------------------------------------
{
    bool ret = (findClass( *_flatClasses,
                       (ClassLattice *)(TreeClassNode *) node ) >= 0);

    return ret;
}

ClassLattice *  TreeClassNode::newLattice(  dr_handle hdl, Module *mod,
                                            char *nm, ClassList *l,
                                            dr_access acc, dr_virtuality v,
                                            int lvl )
//-------------------------------------------------------------------------
{
    return (ClassLattice *) new TreeClassNode( _parent, hdl, mod, nm, l, acc,
                                                v, lvl );
}

DerivationPtr * TreeClassNode::newPtr( ClassLattice * cls,
                            dr_access acc, dr_virtuality virt )
//-------------------------------------------------------------
{
    return new TreeClassPtr( (TreeClassWindow *)_parent, this,
                             (TreeClassNode *) cls, acc, virt );
}

bool TreeClassNode::doParents( WVList& world, TreeClassList & roots,
                               TreeClassList& joinTo )
//-------------------------------------------------------------------
{
    bool joinable = FALSE;
    loadBases();

    for( int i = _bases.count(); i > 0; i -= 1 ) {
        TreeClassNode * baseNode = (TreeClassNode*)_bases[ i - 1 ]->_class;
        bool            thisNodeJoinable = FALSE;
        int             j;

        // remove a base from the world -- should be done for all bases
        for( j = world.count(); j > 0; j -= 1 ) {
            if( ((Symbol *)world[ j - 1 ])->getHandle() ==
                baseNode->getHandle() ) {
                delete world.removeAt( j - 1 );
                break;
            }
        }

        // look to see if we are related to another root
        for( j = roots.count(); j > 0; j -= 1 ) {
            TreeClassNode * rootNode = roots[ j - 1 ];
            if( rootNode != this &&
                rootNode->isRelated( (TreeClassNode *) baseNode ) &&
                !rootNode->isRelated( this ) ) {

                joinable = TRUE;
                thisNodeJoinable = TRUE;
                joinTo.add( rootNode );
                break;
            }
        }

        if( !baseNode->_basesLoaded && !thisNodeJoinable ) {
            joinable = joinable | baseNode->doParents( world, roots, joinTo );
        }
    }

    return joinable;
}

TreePtr* TreeClassNode::getPtr( TreeList_T list, int index )
//----------------------------------------------------------
{
    switch( list ) {
    case ParentList:
        return (TreePtr*)(TreeClassPtr*)_bases[ index ];
    case ChildList:
    case TreeNode::FlatList:
    default:
        REQUIRE( 0, "TreeClassNode::getPtr - bad list." );
        return NULL;
    }
}

TreeNode* TreeClassNode::getNode( TreeList_T list, int index )
//------------------------------------------------------------
{
    switch( list ) {
    case ParentList:
        return (TreeNode*)(TreeClassNode*)((TreeClassPtr *)_bases[ index ])->getTo();
    case ChildList:
        return (TreeNode*)(TreeClassNode*)((TreeClassPtr *)_deriveds[ index ])->getTo();
    case TreeNode::FlatList:
        return (TreeNode*)(TreeClassNode*)(*_flatClasses)[ index ];
    default:
        REQUIRE( 0, "TreeClassNode::getPtr - bad list." );
        return NULL;
    }
}

int TreeClassNode::getCount( TreeList_T list )
//--------------------------------------------
{
    switch( list ) {
    case ParentList:
        return _bases.count();
    case ChildList:
        return _deriveds.count();
    case TreeNode::FlatList:
        return (*_flatClasses).count();
    default:
        REQUIRE( 0, "TreeClassNode::getPtr - bad list." );
        return NULL;
    }
}

TreeNode * TreeClassNode::removeNode( TreeList_T list, TreeNode * node )
//----------------------------------------------------------------------
{
    int index;

    switch( list ) {
    case ParentList:
        for( index = _bases.count(); index > 0; index -= 1 ) {
            if( ((TreeClassPtr*)_bases[ index - 1 ])->getTo() == node ) {
                TreeClassPtr * ptr = (TreeClassPtr*)_bases.removeAt( index - 1 );
                TreeNode *     nd = ptr->getTo();
                delete ptr;
                return nd;
            }
        }
        return NULL;

    case ChildList:
        for( index = _deriveds.count(); index > 0; index -= 1 ) {
            if( ((TreeClassPtr*)_deriveds[ index - 1 ])->getTo() == node ) {
                TreeClassPtr * ptr = (TreeClassPtr *)_deriveds.removeAt( index - 1 );
                TreeNode *     nd = ptr->getTo();
                delete ptr;
                return nd;
            }
        }
        return NULL;

    case TreeNode::FlatList:
        for( index = _flatClasses->count(); index > 0; index -= 1 ) {
            if( ((TreeNode *)(TreeClassNode *)(*_flatClasses)[ index - 1 ]) == node ) {
                return (TreeClassNode *)_flatClasses->removeAt( index - 1 );
            }
        }
        return NULL;
    default:
        REQUIRE( 0, "TreeClassNode::removeNode - bad list." );
        return NULL;
    }
}

void TreeClassNode::addPtr( TreeList_T list, TreePtr * ptr )
//----------------------------------------------------------
{
    switch( list ) {
    case ParentList:
        _bases.add( (TreeClassPtr*)ptr);
        break;

    case ChildList:
        TreeClassPtr * p;
        TreeNode *     sv;

        p = (TreeClassPtr*)ptr->makeClone();
        sv = p->getTo();
        p->setTo( p->getFrom() );
        p->setFrom( sv );

        _deriveds.add( p );
        break;

    default:
        REQUIRE( 0, "TreeClassNode::addPtr - bad list." );
    }
}

void TreeClassNode::swapNode( TreeList_T list,
                              TreeNode * from, TreeNode * to )
//------------------------------------------------------------
{
    int index;

    switch( list ) {
    case ParentList:
        for( index = _bases.count(); index > 0; index -= 1 ) {
            if( ((TreeClassPtr*)_bases[ index - 1 ])->getTo() == from ) break;
        }

        REQUIRE( index > 0, "TreeClassNode::swapNode -- unknown from" );
        ((TreeClassPtr*)_bases[ index - 1 ])->setTo( to );
        break;
    case ChildList:
        for( index = _deriveds.count(); index > 0; index -= 1 ) {
            if( ((TreeClassPtr*)_deriveds[ index - 1 ])->getTo() == from ) break;
        }

        REQUIRE( index > 0, "TreeClassNode::swapNode -- unknown from" );
        ((TreeClassPtr*)_deriveds[ index - 1 ])->setTo( to );
        break;
    default:
        REQUIRE( 0, "TreeClassNode::swapNode - bad list." );
    }
}

static int CompDerivationPtr( const DerivationPtr ** lhs,
                              const DerivationPtr ** rhs )
//--------------------------------------------------------
{
    const TreeNode * lhN = (const TreeNode *) ((TreeClassPtr*)(*lhs))->getTo();
    const TreeNode * rhN = (const TreeNode *) ((TreeClassPtr*)(*rhs))->getTo();

    return TreeNode::compareNodes( lhN, rhN );
}

void TreeClassNode::sortPrtKids()
//-------------------------------
{
    _bases.sort( (TComp) CompDerivationPtr );
    _deriveds.sort( (TComp) CompDerivationPtr );
}

void TreeClassNode::setAllDeriveds( void )
//----------------------------------------
{
    for( int j = _flatClasses->count(); j > 0; j -= 1 ) {
        (*_flatClasses)[ j - 1 ]->setDeriveds();
    }
}

/*-------------------------- TreeClassWindow ----------------------------*/

TreeClassWindow::TreeClassWindow( char * title )
                    : TreeWindow( title )
//----------------------------------------------
{
    show();

    _loadFilter->setSymType( KSTClasses );
    _findFilter->setSymType( KSTClasses );
}

bool TreeClassWindow::contextHelp( bool is_active_window )
//--------------------------------------------------------
{
    if( is_active_window ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_INHERITANCE_VIEW );
    }
    return( TRUE );
}

const char * TreeClassWindow::emptyText()
//---------------------------------------
{
    return "No classes found with current query";
}

void TreeClassWindow::fillRoots( void )
//-------------------------------------
// OPTIMIZE -- this finds all class, creates symbols for them,
//             then creates TreeClassNodes from these and deletes the
//             symbols.  Would be better if didn't create / delete syms.
{
    WVList          world;
    Symbol *        sym;
    TreeClassNode * rootNode;
    TreeClassList   roots;
    WVList          bases;
    bool            quit = FALSE;

    _loadFilter->setSymType( KSTClasses );
    dbManager()->module()->findSymbols( &world, _loadFilter );

    while( world.count() > 0 ) {
        sym = (Symbol *) world.removeAt( world.count() - 1 );
        rootNode = new TreeClassNode( this, sym );
        delete sym;

        // if not related to a prev tree, its a new tree

        TreeClassList joinTo;

        if( rootNode->doParents( world, roots, joinTo ) ) {
            TreeClassNode * prevRoot = NULL;

            for( int i = joinTo.count(); i > 0; i -= 1 ) {

                // make sure that they really are related by checking
                // both.  If both aren't checked, then a partially
                // loaded child might think it is already related
                // to its parent

                if( rootNode->isRelated( joinTo[ i - 1 ] )
                    && joinTo[ i - 1 ]->isRelated( rootNode ) ) {

                    // no action, they are already joined

                } else {
                    rootNode->joinLattice( (ClassLattice *) joinTo[ i - 1 ] );
                    if( prevRoot != NULL ) {
                        REQUIRE( prevRoot->isRelated( joinTo[ i - 1 ] ),
                                 "TreeClassWindow::fillRoots ack nak" );
                        roots.removeSame( prevRoot );
                    }

                    prevRoot = joinTo[ i - 1 ];
                }
            }
        } else {
            roots.add( rootNode );
        }
    }

    for( int i = roots.count(); i > 0; i -= 1 ) {
        TreeClassNode* node = roots[ i - 1 ];
        node->setAllDeriveds();
        node->normalizeLevels();

        for( int j = node->getCount( TreeNode::FlatList ); j > 0; j -= 1 ) {
            if( node->getNode( TreeNode::FlatList, j - 1 )->getLevel() == 0 ) {
                _roots.add( new TreeRoot( this, node->getNode( TreeNode::FlatList, j - 1 ) ) );
                break;
            }
        }
    }
}

// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9

TreeClassWindow::~TreeClassWindow()
//---------------------------------
{
}
