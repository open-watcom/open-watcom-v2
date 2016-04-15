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


#include <string.h>
#include <string.hpp>

#include <drgetref.h>
#include <wpopmenu.hpp>
#include <wmenuitm.hpp>

#include "assure.h"
#include "browse.h"
#include "dbmgr.h"
#include "gtfunc.h"
#include "gtfnopt.h"
#include "keysym.h"
#include "mem.h"
#include "module.h"
#include "brwhelp.h"

const int PTRPOOLSIZE = 128;
const int NODEPOOLSIZE = 64;

#pragma warning 549 9           // sizeof contains compiler genned info.
MemoryPool TreeFuncPtr::_pool( sizeof( TreeFuncPtr ), "TreeFuncPtr",
                                PTRPOOLSIZE );
MemoryPool TreeFuncNode::_pool( sizeof( TreeFuncNode ), "TreeFuncNode",
                                NODEPOOLSIZE );
#pragma warning 549 3

/*---------------------------- TreeFuncPtr --------------------------*/

TreeFuncPtr::TreeFuncPtr( TreeFuncWindow * prt, TreeNode * from,
                          TreeNode * to, dr_ref_info ref )
                : TreePtr( from, to )
                , _reference( ref )
                , _parent( prt )
                , _numRefs( 1 )
//--------------------------------------------------------------
{
}

void * TreeFuncPtr::operator new( size_t )
//-----------------------------------------
{
    return( _pool.alloc() );
}

void TreeFuncPtr::operator delete( void * mem )
//---------------------------------------------
{
    _pool.free( mem );
}

TreePtr * TreeFuncPtr::makeClone( void )
//--------------------------------------
{
    TreeFuncPtr * ptr = new TreeFuncPtr;
    *ptr = *this;
    return ptr;
}

PaintInfo * TreeFuncPtr::getPaintInfo( void )
//-------------------------------------------
{
    OptionManager * mgr = WBRWinBase::optManager();

    return new PaintInfo( mgr->getFunctionOpts().numCalls( _numRefs ) );
}

/*---------------------------- TreeFuncNode --------------------------*/

TreeFuncNode::TreeFuncNode( TreeWindow * prt, dr_sym_type stp,
                            dr_handle drhdl, Module * mod, char * nm,
                            TreeCycleList * flatNode, TreeRefList * flatRef )
                :TreeCycleNode(prt, flatNode, flatRef )
                ,_symType(stp)
                ,_drhandle(drhdl)
                ,_module(mod)
                ,_name(nm)
                ,_decName(NULL)
//------------------------------------------------------------
{
    dr_handle   container;
    char *      name;
    String      accum;

    container = DRGetContaining( drhdl );
    if( container != DR_HANDLE_NUL ) {
        accum = strrev( _name );
        strrev( _name );

        while( container != DR_HANDLE_NUL ) {
            Symbol contSym( drhdl, NULL, mod, DRGetName( container ) );
            name = WBRStrDup( contSym.name() );
            accum += "::";
            accum += strrev( name );
            WBRFree( name );

            container = DRGetContaining( container );
        }

        _decName = WBRStrDup( (const char *)accum );
        strrev( _decName );
    }
}

TreeFuncNode::~TreeFuncNode()
{
    WBRFree( _name );
    WBRFree( _decName );
}

void * TreeFuncNode::operator new( size_t )
//-----------------------------------------
{
    return( _pool.alloc() );
}

void TreeFuncNode::operator delete( void * mem )
//---------------------------------------------
{
    _pool.free( mem );
}

PaintInfo * TreeFuncNode::getPaintInfo( void )
//--------------------------------------------
{
    OptionManager * mgr = WBRWinBase::optManager();

    return new PaintInfo( mgr->getFunctionOpts().repeated( hasReference() ) );
}

int CompFuncNode( const TreeFuncNode ** lhs, const TreeFuncNode ** rhs )
//----------------------------------------------------------------------
{
    return TreeNode::compareNodes( (const TreeNode*)(const TreeCycleNode*) *lhs,
                                   (const TreeNode*)(const TreeCycleNode*) *rhs );
}

int CompFuncPtr( const TreeFuncPtr ** lhs, const TreeFuncPtr ** rhs )
//-------------------------------------------------------------------
{
    return TreePtr::comparePtrs( (TreePtr*) *lhs, (TreePtr*) *rhs );
}

void TreeFuncNode::sortPrtKids()
//------------------------------
{
    _parents.sort( (TComp) CompFuncPtr );
    _children.sort( (TComp) CompFuncNode );
}

Symbol * TreeFuncNode::makeSymbol( void )
//---------------------------------------
{
    char * name = WBRStrDup( _name );
    return Symbol::defineSymbol( _symType, _drhandle, DR_HANDLE_NUL, _module, name );
}

char * TreeFuncNode::name()
//-------------------------
{
    return (_decName) ? _decName : _name;
}


void TreeFuncNode::loadModule( TreeFuncWindow * prt, Module * mod,
                               TreeNodeList& roots, KeySymbol * key )
//-------------------------------------------------------------------
{
    FuncSearchData data;

    data.mod =  mod;
    data.roots = &roots;
    data.parentWin = prt;
    data.key = key;

    #if DEBUG
    data.funcsFound = 0;
    #endif

    DRReferencedSymbols( DR_SYM_FUNCTION, &data, TreeFuncHook );

    for( int i = roots.count(); i > 0; i -= 1 ) {
        ((TreeCycleNode *)roots[ i - 1 ])->breakCycles();
        ((TreeCycleNode *)roots[ i - 1 ])->assignLevels();
    }
}

bool TreeFuncNode::TreeFuncHook( dr_handle owner, dr_ref_info * ref,
                                 char* ownerName, void * info )
//------------------------------------------------------------------
// this function really needs to be split up
{
    int             parentIndex;
    int             parentRoot;
    TreeFuncNode *  parent = NULL;
    int             childIndex;
    int             childRoot;
    TreeFuncNode *  child = NULL;
    FuncSearchData* data = (FuncSearchData *) info;
    char *          depName;
    bool            accept;

    #if DEBUG
    data->funcsFound += 1;

    if( !(data->funcsFound % 10) ) {
        WString stat;
        stat.printf( "Loading - %d found", data->funcsFound );
        browseTop->statusText( stat.gets() );
    }
    #endif

    // find if the symbol matches the filter

    depName = DRGetName( ref->dependent );
    accept = data->key->matches( ref->dependent, depName );
    WBRFree( depName ); // OPTIMIZE -- could use this later

    if( !accept ) {
        return true;    // <---- early return -- discard symbol, keep looking
    }


    for( int i = data->roots->count(); i > 0; i -= 1 ) {
        TreeFuncNode * rt = (TreeFuncNode*) (*data->roots)[ i - 1 ];
        parentIndex = rt->findNode( owner );
        if( !parent ) {
            if( parentIndex >= 0 ) {
                parent = (TreeFuncNode *)(*rt->_flatNodes)[ parentIndex ];
                parentRoot = i - 1;
            }
        }

        if( !child ) {
            childIndex = rt->findNode( ref->dependent );
            if( childIndex >= 0 ) {
                child = (TreeFuncNode *)(*rt->_flatNodes)[ childIndex ];
                childRoot = i - 1;
            }
        }

        if( child != NULL && parent != NULL ) {
            break;
        }
    }

    if( child != NULL && parent != NULL ) {
        WBRFree( ownerName );

        if( parentRoot == childRoot ) {
            int prtIdx = child->findParent( parent );
            if( prtIdx < 0 ) {
                if( ref->dependent == owner ) {     // direct recursion -- a->a
                    bool inReferences = false;

                    for( int refIdx = 0; refIdx < child->_flatReferences->count(); refIdx += 1 ) {
                        if( (*child->_flatReferences)[ refIdx ]->getHandle() == owner ) {
                            TreeNode * node = (*child->_flatReferences)[ refIdx ];
                            for( int ref2idx = 0; ref2idx < node->getCount( ParentList ); ref2idx += 1 ) {
                                if( node->getNode( ParentList, ref2idx )->getHandle() == owner ) {
                                    TreePtr * ptr = node->getPtr( ParentList, ref2idx );
                                    ((TreeRefPtr*)ptr)->incRefs();
                                    inReferences = true;
                                    break;
                                }
                            }
                        }
                        if( inReferences ) {
                            break;
                        }
                    }

                    if( !inReferences ) {
                        TreeRefNode * newRef;
                        newRef = new TreeRefNode( data->parentWin, parent, parent );
                        parent->_children.add( newRef );
                        parent->_hasRef = true;

                        parent->_flatReferences->add( newRef );
                    }
                } else {
                    parent->_children.add( child );
                    child->_parents.add(
                        new TreeFuncPtr( data->parentWin, child, parent, *ref ) );
                }
            } else {
                TreePtr * ptr = child->getPtr( ParentList, prtIdx );
                ((TreeFuncPtr *)ptr)->incRefs();
            }
        } else {
            int     i;

            for( i = child->_flatNodes->count(); i > 0; i -= 1 ) {
                parent->_flatNodes->add( (*child->_flatNodes)[ i - 1 ] );
            }

            data->roots->removeAt( childRoot );
            parent->_children.add( child );
            child->_parents.add(
                    new TreeFuncPtr( data->parentWin, child, parent, *ref ) );

            TreeCycleList * fList = child->_flatNodes;
            TreeRefList * rList = child->_flatReferences;

            for( i = fList->count(); i > 0; i -= 1 ) {
                TreeFuncNode * node = (TreeFuncNode *) (*fList)[ i - 1 ];
                node->_flatNodes = parent->_flatNodes;
                node->_flatReferences = parent->_flatReferences;
            }

            for( i = rList->count(); i > 0; i -= 1 ) {
                parent->_flatReferences->add( (*rList)[ i - 1 ] );
            }

            delete fList;
            delete rList;
        }
    } else {
        if( child != NULL ) {
            parent = new TreeFuncNode( data->parentWin, DRGetSymType( owner ),
                                       owner, data->mod, ownerName,
                                       child->_flatNodes,
                                       child->_flatReferences );

            parent->_children.add( child );
            child->_parents.add(
                    new TreeFuncPtr( data->parentWin, child, parent, *ref ) );
        } else {
            if( parent != NULL ) {
                WBRFree( ownerName );

                child = new TreeFuncNode( data->parentWin,
                                          DRGetSymType( ref->dependent ),
                                          ref->dependent, data->mod,
                                          DRGetName( ref->dependent ),
                                          parent->_flatNodes,
                                          parent->_flatReferences );
                parent->_children.add( child );
                child->_parents.add(
                    new TreeFuncPtr( data->parentWin, child, parent, *ref ) );

            } else {    // child == parent == NULL

                parent = new TreeFuncNode( data->parentWin,
                                           DRGetSymType( owner ),
                                           owner, data->mod, ownerName );

                if( ref->dependent == owner ) {
                    TreeRefNode * newRef;
                    newRef = new TreeRefNode( data->parentWin, parent, parent );
                    parent->_children.add( newRef );
                    parent->_hasRef = true;

                    parent->_flatReferences->add( newRef );
                } else {
                    child = new TreeFuncNode( data->parentWin,
                                              DRGetSymType( ref->dependent ),
                                              ref->dependent, data->mod,
                                              DRGetName( ref->dependent ),
                                              parent->_flatNodes,
                                              parent->_flatReferences );

                    child->_parents.add(
                        new TreeFuncPtr( data->parentWin, child, parent, *ref ) );
                    parent->_children.add( child );
                }

                data->roots->add( parent );
            }
        }
    }

    return true;
}

/*-------------------------- TreeFuncWindow ----------------------------*/

TreeFuncWindow::TreeFuncWindow( char * title )
                    : TreeWindow( title )
//--------------------------------------------------------------------------
{
    show();
}

bool TreeFuncWindow::contextHelp( bool is_active_window )
//-------------------------------------------------------
{
    if( is_active_window ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_CALL_TREE_VIEW );
    }
    return( true );
}

const char * TreeFuncWindow::emptyText()
//--------------------------------------
{
    return "No calls found with current query";
}

void TreeFuncWindow::fillRoots( void )
//------------------------------------
{
    TreeNodeList roots;
    int          i;

    _loadFilter->setSymType( KSTFunctions );
    TreeFuncNode::loadModule( this, dbManager()->module(), roots, _loadFilter );

    for( i = roots.count(); i > 0; i -= 1 ) {
        TreeFuncNode * node = (TreeFuncNode *) roots[ i - 1 ];
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

TreeFuncWindow::~TreeFuncWindow()
//-------------------------------
{
}
