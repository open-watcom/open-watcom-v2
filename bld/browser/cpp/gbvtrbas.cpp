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


#include "browse.h"
#include "gbvtrbas.h"
#include "view.h"
#include "menuids.h"
#include "menumgr.h"
#include "symbol.h"

static MIMenuID WantTreeMenus[] = {
    MIMenuID( MMDetail, DMDetail ),
    MIMenuID( MMDetail, DMDefinition ),
    MIMenuID( MMDetail, DMReferences ),
    MIMenuID( MMDetail, DMUsers ),

    MIMenuID( MMLocate, LMFindSelected ),
    MIMenuID( MMTree,   TMRoots ),
    MIMenuID( MMTree,   TMArrangeAll ),
    MIMenuID( MMTree,   TMExpandOne ),
    MIMenuID( MMTree,   TMExpandBranch ),
    MIMenuID( MMTree,   TMExpandAll ),
    MIMenuID( MMTree,   TMCollapseBranch ),
    MIMenuID( MMTree,   TMCollapseAll ),
};

#define NumWantTreeMenus ( sizeof( WantTreeMenus ) / sizeof( MIMenuID ) )

GlobalViewTree::GlobalViewTree( MenuManager * mgr )
        : _menuManager( mgr )
//------------------------------
{
}

GlobalViewTree::~GlobalViewTree()
//-------------------------------
{
}

void GlobalViewTree::setMenus( MenuManager * mgr )
//------------------------------------------------
{
    int i;

    for( i = 0; i < NumWantTreeMenus; i += 1 ) {
        mgr->registerForMenu( this, WantTreeMenus[ i ] );
    }

    _menuManager = mgr;
    selChange();
}

void GlobalViewTree::unsetMenus( MenuManager * mgr )
//--------------------------------------------------
{
    int i;

    for( i = 0; i < NumWantTreeMenus; i += 1 ) {
        mgr->unRegister( WantTreeMenus[ i ] );
    }
}

void GlobalViewTree::menuSelected( const MIMenuID & id )
//------------------------------------------------------
{
    switch( id.mainID() ) {
    case MMDetail:
        switch( id.subID() ) {
        case DMDetail:
            treeViewEvent( ShowDetail );
            break;
        case DMDefinition:
            treeViewEvent( ShowDefinition );
            break;
        case DMReferences:
            treeViewEvent( ShowReferences );
            break;
        case DMUsers:
            treeViewEvent( ShowUsers );
            break;
        }
        break;
    case MMLocate:
        switch( id.subID() ) {
        case LMFindSelected:
            treeViewEvent( ScrollToSelected );
            break;
        }
        break;
    case MMTree:
        switch( id.subID() ) {
        case TMRoots:
            treeViewEvent( SelectRoots );
            break;
        case TMArrangeAll:
            treeViewEvent( Arrange );
            break;
        case TMExpandOne:
            treeViewEvent( ExpandOne );
            break;
        case TMExpandBranch:
            treeViewEvent( ExpandBranch );
            break;
        case TMExpandAll:
            treeViewEvent( ExpandAll );
            break;
        case TMCollapseBranch:
            treeViewEvent( CollapseBranch );
            break;
        case TMCollapseAll:
            treeViewEvent( CollapseAll );
            break;
        }
        break;
    }
    selChange();
}

bool GlobalViewTree::key( WKeyCode key, WKeyState state )
//-------------------------------------------------------
{
    bool ret = TRUE;

    switch( key ) {
    case WKeyCtrlKPAsterisk:
        treeViewEvent( ExpandAll );
        break;
    case WKeyCtrl8:
        if( state & WKeyStateShift ) {
            treeViewEvent( ExpandAll );
        } else {
            ret = FALSE;
        }
        break;
    case WKeyAsterisk:
//    case WKeyKPAsterisk:
        if( state & WKeyStateCtrl ) {
            treeViewEvent( ExpandAll );
        } else {
            treeViewEvent( ExpandBranch );
        }
        break;
    case WKeyPlus:
//    case WKeyKPPlus:
        treeViewEvent( ExpandOne );
        break;
    case WKeyCtrlMinus:
    case WKeyCtrlKPMinus:
        treeViewEvent( CollapseAll );
        break;
    case WKeyMinus:
//    case WKeyKPMinus:
        treeViewEvent( CollapseBranch );
        break;
    default:
        ret = FALSE;
    }
    selChange();

    return ret;
}

void GlobalViewTree::selChange()
//------------------------------
{
    ExpandState st = state();

    _menuManager->enableMenu( MIMenuID( MMDetail, DMDetail ),
                              (st != NoSymbol) );
    _menuManager->enableMenu( MIMenuID( MMDetail, DMDefinition ),
                              (st != NoSymbol) && browseTop->canEdit() );
    _menuManager->enableMenu( MIMenuID( MMDetail, DMReferences ),
                              (st != NoSymbol) );
    _menuManager->enableMenu( MIMenuID( MMDetail, DMUsers ),
                              (st != NoSymbol) );
    _menuManager->enableMenu( MIMenuID( MMLocate, LMFindSelected ),
                              (st != NoSymbol) );
    _menuManager->enableMenu( MIMenuID( MMTree, TMExpandBranch ),
                              st != NoSymbol );
    _menuManager->enableMenu( MIMenuID( MMTree, TMCollapseBranch ),
                              (st != NoSymbol ) );

    _menuManager->enableMenu( MIMenuID( MMTree, TMExpandOne ),
                              (st == Collapsed ) );
}

void GlobalViewTree::event( ViewEvent ve, View * view )
//-----------------------------------------------------
{
    ve = ve; view = view;
    // NYI
}

ViewEvent GlobalViewTree::wantEvents()
//------------------------------------
{
    return VENoEvent;
}
