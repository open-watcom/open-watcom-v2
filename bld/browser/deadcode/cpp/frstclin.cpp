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


#include <wvlist.hpp>

#include "frstclin.h"
#include "gtbase.h"
#include "gtclass.h"
#include "projmodl.h"
#include "keysym.h"
#include "symbol.h"

ForestClassInterface::ForestClassInterface( ProjectModel * project )
        : _project( project )
{
    fillRoots();
}


ForestClassInterface::~ForestClassInterface()
/*******************************************/
{
    for( int i = _roots.count(); i > 0; i -= 1 ) {
        delete _roots[ i - 1 ];
    }
}

void ForestClassInterface::fillRoots( void )
/******************************************/
// Note : This is a direct clone from gtfunc.cpp of fillRoots()
//        except that NULL is passed as the parent window.
//
// <protected>
{
    KeySymbol       key;
    WVList          world;
    Symbol *        sym;
    TreeClassNode * rootNode;
    TreeClassList   roots;
    WVList          bases;
    bool            quit = FALSE;

    key.setSymType( DR_SEARCH_CLASSES );
    _project->findSymbols( &world, &key );

    while( world.count() > 0 ) {
        sym = (Symbol *) world.removeAt( world.count() - 1 );
        rootNode = new TreeClassNode( NULL, sym );
        delete sym;     // NYI - makes all these symbols then just delete them

        // if not related to a prev tree, its a new tree

        TreeClassList joinTo;

        if( rootNode->doParents( world, roots, joinTo ) ) {
            TreeClassNode * prevRoot = NULL;

            for( int i = joinTo.count(); i > 0; i -= 1 ) {
                if( !rootNode->isRelated( joinTo[ i - 1 ] ) ) {
                    rootNode->joinLattice( (ClassLattice *) joinTo[ i - 1 ] );
                    if( prevRoot != NULL ) {
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
                _roots.add( new TreeRoot( NULL, node->getNode( TreeNode::FlatList, j - 1 ) ) );
                break;
            }
        }
    }

    for( i = 0; i < roots.count(); i += 1 ) {
        TreeClassNode * node = (TreeClassNode *) roots[ i ];

        for( int j = 0; j < node->getCount( TreeNode::FlatList ); j += 1 ) {
            TreeClassNode *pNode;
            pNode = (TreeClassNode *) node->getNode( TreeNode::FlatList, j );
            if( pNode->getLevel() == 0 ) {
                _level0Nodes.add( pNode );
            }
        }
    }
}

void ForestClassInterface::getChildData( void * pParent,
                                            int index,
                                            void * & pChild,
                                            char * & pChildName,
                                            int & numChildren )
/***************************************************************/
// <public>
{
    if( pParent == NULL ) {
        if(( index < 0 ) || ( index > _roots.count() )) {
            pChild = NULL;
            pChildName = NULL;
            numChildren = 0;
        } else {
            pChild = _level0Nodes[ index ];
            pChildName =  _level0Nodes[ index ]->name();
            numChildren = ((TreeClassNode *)pChild)->getCount( TreeNode::ChildList );
        }
    } else {
        TreeNode * pNode;

        pNode = (TreeNode *) pParent;

        if(( index < 0 ) || ( index > pNode->getCount( TreeNode::ChildList ))) {
            pChild = NULL;
            pChildName = NULL;
            numChildren = 0;
        } else {
            pChild = pNode->getNode( TreeNode::ChildList, index );
            pChildName = pNode->getNode( TreeNode::ChildList, index )->name();
            numChildren = ((TreeClassNode *)pChild)->getCount( TreeNode::ChildList );
        }
    }
}

int ForestClassInterface::getNumChildren( void * pParent )
/************************************************************/
// Returns :
//      Number of roots if pParent == NULL
//      Number of children of pParent otherwise
//
// <public>
{
    if( pParent == NULL ) {
        return _level0Nodes.count();
    }

    TreeNode * pNode;

    pNode = (TreeNode *) pParent;

    return pNode->getCount( TreeNode::ChildList );
}
