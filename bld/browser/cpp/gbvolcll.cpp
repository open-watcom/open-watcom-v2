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


#include "busyntc.h"
#include "dbmgr.h"
#include "module.h"
#include "viewmgr.h"
#include "gtfunc.h"
#include "keysym.h"
#include "icons.h"

#include "gbvolcll.h"
#include "brwhelp.h"

GBVCallOutline::GBVCallOutline( const char * text )
                : Outline( text )
//-------------------------------------------------------------------------
{
    setIcon( GBOutline );
    show();
    load();
}

bool GBVCallOutline::contextHelp( bool is_active_win )
//----------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_CALL_TREE_VIEW );
    }
    return( TRUE );
}

bool GBVCallOutline::reallyClose()
//-----------------------------------
{
    viewManager()->viewDying( this );
    return TRUE;
}

bool GBVCallOutline::gettingFocus( WWindow * )
//-----------------------------------------------
{
    viewManager()->eventOccured( VEGettingFocus, this );
    return FALSE;
}

bool GBVCallOutline::losingFocus( WWindow * )
//----------------------------------------------
{
    viewManager()->eventOccured( VELosingFocus, this );
    return FALSE;
}

const char * GBVCallOutline::emptyText()
//--------------------------------------
{
    return "No calls found with current query";
}

void GBVCallOutline::load()
//----------------------------
// this code is cut-and-pasted from GTFUNC.CPP
// FIXME -- this should all be put back in one spot
{
    BusyNotice   busy( "Loading..." );
    TreeNodeList roots;
    int          i;

    _loadFilter->setSymType( KSTFunctions );
    TreeFuncNode::loadModule( NULL, dbManager()->module(), roots, _loadFilter );

    for( i = 0; i < roots.count(); i += 1 ) {
        TreeFuncNode * node = (TreeFuncNode *) roots[ i ];

        for( int j = 0; j < node->getCount( TreeNode::FlatList ); j += 1 ) {
            TreeFuncNode *pNode;
            pNode = (TreeFuncNode *) node->getNode( TreeNode::FlatList, j );
            if( pNode->getCount( TreeNode::ParentList ) == 0 ) {
                addLevel0( pNode );
            }
        }
    }

    for( i = 0; i < roots.count(); i += 1 ) {
        TreeFuncNode * node = (TreeFuncNode *) roots[ i ];

        node->deleteTree();
    }

    reset();
}

// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9

GBVCallOutline::~GBVCallOutline()
//-------------------------------------
{
}
