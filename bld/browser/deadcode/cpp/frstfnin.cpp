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

#include "frstfnin.h"
#include "gtbase.h"
#include "gtfunc.h"
#include "projmodl.h"
#include "module.h"

ForestFunctionInterface::ForestFunctionInterface( ProjectModel * project )
        : _project( project )
{
    fillRoots();
}


ForestFunctionInterface::~ForestFunctionInterface()
/*************************************************/
{
    for( int i = _roots.count(); i > 0; i -= 1 ) {
        delete _roots[ i - 1 ];
    }
}

void ForestFunctionInterface::fillRoots( void )
/*********************************************/
// Note : This is a clone of gtfunc.cpp of fillRoots()
//        except that NULL is passed as the parent window.
//        As well, we think of a "root" as any node at level 0.
//        ie. a DAG can have more than one "root"
//
// <protected>
{
    WVList& modules = _project->modules();
    TreeNodeList roots;

    for( int i = modules.count(); i > 0; i -= 1 ) {
        Module * mod = (Module *) modules[ i - 1 ];

        if( mod->enabled() ) {
            mod->setModule();
            TreeFuncNode::loadModule( NULL, mod, roots );
        }
    }

    for( i = roots.count(); i > 0; i -= 1 ) {
        TreeFuncNode * node = (TreeFuncNode *) roots[ i - 1 ];
        for( int j = node->getCount( TreeNode::FlatList ); j > 0; j -= 1 ) {
            if( node->getNode( TreeNode::FlatList, j - 1 )->getLevel() == 0 ) {
                _roots.add( new TreeRoot( NULL, node->getNode( TreeNode::FlatList, j - 1 ) ) );
                break;
            }
        }
    }

    for( i = 0; i < roots.count(); i += 1 ) {
        TreeFuncNode * node = (TreeFuncNode *) roots[ i ];

        for( int j = 0; j < node->getCount( TreeNode::FlatList ); j += 1 ) {
            TreeFuncNode *pNode;
            pNode = (TreeFuncNode *) node->getNode( TreeNode::FlatList, j );
            if( pNode->getLevel() == 0 ) {
                _level0Nodes.add( pNode );
            }
        }
    }
}

void ForestFunctionInterface::getChildData( void * pParent,
                                            int index,
                                            void * & pChild,
                                            char * & pChildName,
                                            int & numChildren )
/***************************************************************/
// <public>
{
    if( pParent == NULL ) {
        if(( index < 0 ) || ( index > _level0Nodes.count() )) {
            pChild = NULL;
            pChildName = NULL;
            numChildren = 0;
        } else {
            pChild = _level0Nodes[ index ];
            pChildName =  _level0Nodes[ index ]->name();
            numChildren = ((TreeFuncNode *)pChild)->getCount( TreeNode::ChildList );
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
            numChildren = ((TreeFuncNode *)pChild)->getCount( TreeNode::ChildList );
        }
    }
}

int ForestFunctionInterface::getNumChildren( void * pParent )
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

