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
#include <drgetref.h>

#include "assure.h"
#include "module.h"
#include "gtcycle.h"

TreeCycleNode::TreeCycleNode( TreeWindow * prt, TreeCycleList * flatNode,
                              TreeRefList * flatRef )
                : TreeNode( prt )
                , _level( 0 )
                , _entered( FALSE )
                , _hasRef( FALSE )
//-----------------------------------------------------------------------
{
    _flatNodes =      (flatNode != NULL) ? flatNode : new TreeCycleList;
    _flatReferences = (flatRef != NULL) ? flatRef : new TreeRefList;

    _flatNodes->add( this );
}

TreeCycleNode::~TreeCycleNode()
//-----------------------------
{
    int i;
    for( i = _parents.count(); i > 0; i -= 1 ) {
        delete _parents[ i - 1 ];
    }
}

bool TreeCycleNode::isRelated( TreeNode * node )
//----------------------------------------------
{
    return (findNode( node ) >= 0);
}

int TreeCycleNode::findNode( TreeNode * node )
//--------------------------------------------
{
//    _MARK_( "TreeCycleNode::findNode( TreeNode * ) start" );

    for( int i = _flatNodes->count(); i > 0; i -= 1 ) {
        if( (*_flatNodes)[ i - 1 ]->getHandle() == node->getHandle() ) {
            return i - 1;
        }
    }

//    _MARK_( "TreeCycleNode::findNode( TreeNode * ) end" );
    return -1;
}

int TreeCycleNode::findNode( uint_32 handle )
//-------------------------------------------
{
//    _MARK_( "TreeCycleNode::findNode( uint_32 ) start" );

    for( int i = _flatNodes->count(); i > 0; i -= 1 ) {
        if( (*_flatNodes)[ i - 1 ]->getHandle() == handle ) {
            return i - 1;
        }
    }

//    _MARK_( "TreeCycleNode::findNode( uint_32 ) end" );

    return -1;
}

int TreeCycleNode::findParent( TreeNode * node )
//----------------------------------------------
{
//    _MARK_( "TreeCycleNode::findParent( TreeNode * ) start" );

    uint_32 handle = node->getHandle();
    for( int i = _parents.count(); i > 0; i -= 1 ) {
        TreeNode * prt = _parents[ i - 1 ]->getTo();
        if( prt->getHandle() == handle ) {
            return i - 1;
        }
    }

//    _MARK_( "TreeCycleNode::findParent( TreeNode * ) end" );

    return -1;
}

bool TreeCycleNode::isParent( TreeNode * node )
//---------------------------------------------
{
    return ( findParent( node ) >= 0 );
}

int TreeCycleNode::findChild( TreeNode * node )
//---------------------------------------------
{
//    _MARK_( "TreeCycleNode::findChild( TreeNode * ) start" );

    uint_32 handle = node->getHandle();
    for( int i = _children.count(); i > 0; i -= 1 ) {
        TreeNode * child = _children[ i - 1 ];
        if( child->getHandle() == handle ) {
            return i - 1;
        }
    }

//    _MARK_( "TreeCycleNode::findChild( TreeNode * ) end" );

    return -1;
}

bool TreeCycleNode::isChild( TreeNode * node )
//--------------------------------------------
{
    return ( findChild( node ) >= 0 );
}

TreePtr* TreeCycleNode::getPtr( TreeList_T list, int index )
//---------------------------------------------------------
{
    REQUIRE( list == ParentList, "TreeCycleNode::getPtr bad list" );
    list = list;        // to avoid a warning.

    return _parents[ index ];
}

TreeNode* TreeCycleNode::getNode( TreeList_T list, int index )
//-----------------------------------------------------------
{
    switch( list ) {
    case ParentList:
        return _parents[ index ]->getTo();
    case ChildList:
        return _children[ index ];
    case FlatList:
        if( index < _flatNodes->count() ) {
            return (*_flatNodes)[ index ];
        } else {
            return (*_flatReferences)[ index - _flatNodes->count() ];
        }
    default:
        REQUIRE( 0, "TreeCycleNode::getNode ack" );
        return NULL;                // keep compiler happy
    }
}

int TreeCycleNode::getCount( TreeList_T list )
//-------------------------------------------
{
    switch( list ) {
    case ParentList:
        return _parents.count();
    case ChildList:
        return _children.count();
    case FlatList:
        return _flatNodes->count() + _flatReferences->count();
    default:
        REQUIRE( 0, "TreeCycleNode::getCount ack" );
        return -1;
    }
}

TreeNode * TreeCycleNode::removeNode( TreeList_T list, TreeNode * node )
//----------------------------------------------------------------------
{
    int i;

    switch( list ) {
    case ParentList:
        for( i = _parents.count(); i > 0; i -= 1 ) {
            if( _parents[ i - 1 ]->getTo() == node ) {
                TreePtr * ptr = _parents.removeAt( i - 1 );
                TreeNode * nd = ptr->getTo();
                delete ptr;
                return nd;
            }
        }
        return NULL;

    case ChildList:
        return _children.removeAt( _children.indexOfSame( node ) );
    case FlatList:
        for( i = _flatNodes->count(); i > 0; i -= 1 ) {
            if( (*_flatNodes)[ i - 1 ] == node ) {
                return _flatNodes->removeAt( i - 1 );
            }
        }
        return NULL;

    default:
        REQUIRE( 0, "TreeCycleNode::removeNode ack" );
        return NULL;
    }
}

void TreeCycleNode::addPtr( TreeList_T list, TreePtr * ptr )
//----------------------------------------------------------
{
    switch( list ) {
    case ParentList:
        _parents.add( ptr );
        break;

    case ChildList:
        _children.add( ptr->getFrom() );
        break;

    default:
        REQUIRE( 0, "TreeCycleNode::addPtr ack" );
    }
}

void TreeCycleNode::swapNode( TreeList_T list, TreeNode * from,
                              TreeNode * to )
//-------------------------------------------------------------
{
    int index;

    switch( list ) {
    case ParentList:
        for( index = _parents.count(); index > 0; index -= 1 ) {
            if( _parents[ index - 1 ]->getTo() == from ) break;
        }

        REQUIRE( index > 0, "TreeCycleNode::swapNode -- unknown from" );
        _parents[ index - 1 ]->setTo( to );
        break;
    case ChildList:
        index = _children.indexOfSame( from );
        REQUIRE( index >= 0, "TreeCycleNode::swapNode -- unknown from" );
        _children.replaceAt( index, to );
        break;
    default:
        REQUIRE( 0, "TreeCycleNode::getCount ack" );
    }
}

void TreeCycleNode::deleteTree( void )
//-----------------------------------
{
//    _MARK_( "TreeCycleNode::deleteTree start" );

    int             i;
    TreeCycleList * nodeList = _flatNodes;
    TreeRefList *   refList = _flatReferences;

    for( i = nodeList->count(); i > 0; i -= 1 ) {
        delete (*nodeList)[ i - 1 ];
    }

    for( i = refList->count(); i > 0; i -= 1 ) {
        delete (*refList)[ i - 1 ];
    }

    delete nodeList;
    delete refList;

//    _MARK_( "TreeCycleNode::deleteTree start" );
}

void TreeCycleNode::breakCycles( void )
//-------------------------------------
{
//    _MARK_( "TreeCycleNode::breakCycles start" );

    short         level = 0;
    TreeCycleList seen;

    for( int i = _flatNodes->count(); i > 0; i -= 1 ) {
        TreeCycleNode * node = (*_flatNodes)[ i - 1 ];
        if( node->_level == 0 ) {
            findComponents( seen, level );
        }
    }

//    _MARK_( "TreeCycleNode::breakCycles end" );
}

int TreeCycleNode::findComponents( TreeCycleList& seen, short& level )
//--------------------------------------------------------------------
{
//    _MARK_( "TreeCycleNode::findComponents start" );

    int minLevel;
    int testMin;
    int myIndex = seen.count();

    level += 1;
    _level = level;
    seen.add( this );

    minLevel = level;

    for( int i = _children.count(); i > 0; i -= 1 ) {
        if( _children[ i - 1 ]->isReference() ) {
            continue;   // <-- wierd flow!
        }
        TreeCycleNode * node = (TreeCycleNode *) _children[ i - 1 ];
        testMin = node->_level > 0 ? node->_level
                               : node->findComponents( seen, level );

        minLevel = minInt( minLevel, testMin );
    }

    if( minLevel == _level ) {      // found a strongly-connected component
        if( seen.count() == myIndex + 1 ) {
            _level = 0xf00;         // EC#
            seen.removeAt( myIndex );
        } else {
            bool broken = FALSE;
            for( int j = seen.count(); j > myIndex; j -= 1 ) {
                int childIndex = seen[ j - 1 ]->findChild( this );

                if( childIndex >= 0
                    && !seen[ j - 1 ]->_children[ childIndex ]->isReference()
                    && !broken ) {

                    broken = TRUE;

                    TreeRefNode * newRef;

                    newRef = new TreeRefNode( _parent, this, seen[ j - 1 ] );
                    seen[ j - 1 ]->_children.replaceAt( childIndex, newRef );
                    _flatReferences->add( newRef );
                    _hasRef = TRUE;

                    TreePtr * killIt = _parents.removeAt(
                                                findParent( seen[ j - 1 ] ) );
                    REQUIRE( killIt != NULL, "TreeCycleNode killing NULL ack" );
                    delete killIt;
                }

                seen[ j - 1 ]->_level = 0; // reset to be done once more
                seen.removeAt( j - 1 );
            }
        }
    }

//    _MARK_( "TreeCycleNode::findComponents end" );

    return minLevel;
}

void TreeCycleNode::assignLevels( void )
//--------------------------------------
{
//    _MARK_( "TreeCycleNode::assignLevels start" );

    TreeCycleListList roots;        // roots of search trees
    TreeCycleList *   seen;         // nodes seen in current tree
    TreeCycleRelList  relatives;    // nodes last tree was related to
    int           i;

    for( i = _flatNodes->count(); i > 0; i -= 1 ) {
        (*_flatNodes)[ i - 1 ]->_entered = FALSE;

        #if DEBUG
        #define MAGIC_NUMBER 21212
        (*_flatNodes)[ i - 1 ]->_level = MAGIC_NUMBER;
        #endif
    }

    for( i = _flatNodes->count(); i > 0; i -= 1 ) {
        if( !(*_flatNodes)[ i - 1 ]->_entered ) {
            seen = new TreeCycleList;

            (*_flatNodes)[ i - 1 ]->levelAdjust( *seen, relatives, 0 );

            roots.add( joinTrees( roots, seen, relatives ) );

            for( int j = relatives.count(); j > 0; j -= 1 ) {
                delete relatives[ j - 1 ];
            }
            relatives.reset();
        }
    }

    int minLevel = _level;

    for( i = _flatNodes->count(); i > 0; i -= 1 ) {
        #if DEBUG
        REQUIRE( (*_flatNodes)[ i - 1 ]->_level != MAGIC_NUMBER, "TreeCycleNode::assignLevels()" );
        #endif

        minLevel = minInt( minLevel, (*_flatNodes)[ i - 1 ]->_level );
    }

    for( i = _flatNodes->count(); i > 0; i -= 1 ) {
        (*_flatNodes)[ i - 1 ]->_level -= minLevel;
    }

    for( i = roots.count(); i > 0; i -= 1 ) {
        delete roots[ i - 1 ];
    }

//    _MARK_( "TreeCycleNode::assignLevels end" );
}

void TreeCycleNode::levelAdjust( TreeCycleList & seen,
                                 TreeCycleRelList & relatives,
                                 int seenAt )
//------------------------------------------------------------
{
//    _MARK_( "TreeCycleNode::levelAdjust start" );

    seen.add( this );
    _level = seenAt;
    _entered = TRUE;

    #if DEBUG
    char * myName = name();
    char * pName;

    TreeCycleList * snL = &seen;
    TreeCycleRelList * relL = &relatives;
    #endif

    for( int i = _parents.count(); i > 0; i -= 1 ) {
        TreeCycleNode * prt = (TreeCycleNode *)_parents[ i - 1 ]->getTo();

        #if DEBUG
        pName = prt->name();
        #endif

        if( prt->_entered ) {
            TreeCycleRelative * rel = new TreeCycleRelative;
            rel->node = prt;
            rel->levelAt = (short)( seenAt - 1 );
            relatives.add( rel );
        } else {
            prt->levelAdjust( seen, relatives, seenAt - 1 );
        }
    }

//    _MARK_( "TreeCycleNode::levelAdjust end" );
}

TreeCycleList * TreeCycleNode::joinTrees( TreeCycleListList & roots,
                    TreeCycleList * nodes, TreeCycleRelList & relatives )
//-----------------------------------------------------------------------
{
//    _MARK_( "TreeCycleNode::joinTrees start" );

    int  maxDelta = 0;
    bool deltaSet;
    int  tstDelta;

    #if DEBUG
    char * myName = name();
    char * rName;
    char * rrName;
    #endif

    for( int i = roots.count(); i > 0 && relatives.count() > 0; i -= 1 ) {
        deltaSet = FALSE;
        for( int j = relatives.count(); j > 0; j -= 1 ) {
            TreeCycleRelative * relNode = relatives[ j - 1 ];

            #if DEBUG
            rrName = relNode->node->name();
            #endif

            for( int k = roots[ i - 1 ]->count(); k > 0; k -= 1 ) {
                TreeCycleNode * rootNode = (*roots[ i - 1 ])[ k - 1 ];

                #if DEBUG
                rName = rootNode->name();
                #endif

                if( rootNode->getHandle() == relNode->node->getHandle() ) {
                    tstDelta = rootNode->_level - relNode->levelAt;
                    delete relatives.removeAt( j - 1 );

                    if( !deltaSet ) {
                        deltaSet = TRUE;
                        maxDelta = tstDelta;
                    } else {
                        maxDelta = maxInt( maxDelta, tstDelta );
                    }

                    break;  // out of root-node loop
                }
            }
        }

        // now all the connections to the current tree have been
        // explored: delta the current tree, add the current root's
        // node to our nodes, then make this the new root

        if( deltaSet ) {
            for( int nodeCtr = nodes->count(); nodeCtr > 0; nodeCtr -= 1 ) {
                (*nodes)[ nodeCtr - 1 ]->_level += maxDelta;
                roots[ i - 1 ]->add( (*nodes)[ nodeCtr - 1 ] );
            }
            for( int relCtr = relatives.count(); relCtr > 0; relCtr -= 1 ) {
                relatives[ relCtr - 1 ]->levelAt += maxDelta;
            }

            delete nodes;
            nodes = roots[ i - 1 ];
            roots.removeAt( i - 1 );
        }
    }

//    _MARK_( "TreeCycleNode::joinTrees end" );

    return nodes;
}
