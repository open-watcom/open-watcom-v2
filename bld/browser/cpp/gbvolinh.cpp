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
#include "gtclass.h"
#include "icons.h"
#include "keysym.h"
#include "module.h"
#include "viewmgr.h"

#include "gbvolinh.h"
#include "brwhelp.h"

GBVInheritOutline::GBVInheritOutline( const char * text )
                : Outline( text )
//-------------------------------------------------------------------------
{
    setIcon( GBOutline );
    show();
    load();
}

bool GBVInheritOutline::contextHelp( bool is_active_win )
//-------------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_INHERITANCE_VIEW );
    }
    return( TRUE );
}

bool GBVInheritOutline::reallyClose()
//-----------------------------------
{
    viewManager()->viewDying( this );
    return TRUE;
}

bool GBVInheritOutline::gettingFocus( WWindow * )
//-----------------------------------------------
{
    viewManager()->eventOccured( VEGettingFocus, this );
    return FALSE;
}

bool GBVInheritOutline::losingFocus( WWindow * )
//----------------------------------------------
{
    viewManager()->eventOccured( VELosingFocus, this );
    return FALSE;
}

const char * GBVInheritOutline::emptyText()
//-----------------------------------------
{
    return "No classes found with current query";
}

void GBVInheritOutline::load()
//----------------------------
// this code is cut-and-pasted from GTCLASS.CPP
// FIXME -- this should all be put back in one spot
{
    BusyNotice      busy( "Loading..." );
    WVList          world;
    Symbol *        sym;
    TreeClassNode * rootNode;
    TreeClassList   roots;
    WVList          bases;
    int             i;
    bool            quit = FALSE;

    _loadFilter->setSymType( KSTClasses );
    dbManager()->module()->findSymbols( &world, _loadFilter );

    while( world.count() > 0 ) {
        sym = (Symbol *) world.removeAt( world.count() - 1 );
        rootNode = new TreeClassNode( NULL, sym );
        delete sym;

        // if not related to a prev tree, its a new tree

        TreeClassList joinTo;

        if( rootNode->doParents( world, roots, joinTo ) ) {
            TreeClassNode * prevRoot = NULL;

            for( i = joinTo.count(); i > 0; i -= 1 ) {

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
                        roots.removeSame( prevRoot );
                    }

                    prevRoot = joinTo[ i - 1 ];
                }
            }
        } else {
            roots.add( rootNode );
        }
    }

    for( i = roots.count(); i > 0; i -= 1 ) {
        TreeClassNode* node = roots[ i - 1 ];
        node->setAllDeriveds();
        node->normalizeLevels();

        for( int j = node->getCount( TreeNode::FlatList ); j > 0; j -= 1 ) {
            TreeClassNode *pNode;
            pNode = (TreeClassNode *) node->getNode( TreeNode::FlatList, j - 1 );

            if( pNode->getCount( TreeNode::ParentList ) == 0 ) {
                addLevel0( pNode );
            }
        }
    }


    for( i = 0; i < roots.count(); i += 1 ) {
        TreeClassNode * node = (TreeClassNode *) roots[ i ];
        node->deleteTree();
    }

    reset();
}

// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9

GBVInheritOutline::~GBVInheritOutline()
//-------------------------------------
{
}

