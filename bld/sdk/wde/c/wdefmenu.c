/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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


#include "wdeglbl.h"
#include "wdemain.h"
#include "wderes.h"
#include "wdesdlg.h"
#include "wderibbn.h"
#include "wdecurr.h"
#include "wdegoto.h"
#include "wde.rh"
#include "wdecctl.h"
#include "wdecust.h"
#include "wdefmenu.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum {
    BASE = 1,
    DIALOG,
    CONTROL
} WdeLastObjectType;

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static bool              WdeLastDlgRestorable = 0;
static bool              WdeLastDlgIsParent   = 0;
static WdeLastObjectType WdeLastObject        = 0;
static WdeOrderMode      WdeLastOrderMode     = WdeSelect;

static void WdeEnableAllMenuItems( HMENU hmenu, bool enable )
{
    int count;

    count = GetMenuItemCount( hmenu );
    for( count--; count >= 0; count-- ) {
        if( enable ) {
            EnableMenuItem( hmenu, count, MF_ENABLED | MF_BYPOSITION );
        } else {
            EnableMenuItem( hmenu, count, MF_GRAYED | MF_BYPOSITION );
        }
    }
}

#if 0
static void WdeEnableTest( HMENU hmenu, bool enable )
{
    if( enable ) {
        EnableMenuItem( hmenu, IDM_TEST_MODE, MF_ENABLED | MF_BYCOMMAND );
    } else {
        EnableMenuItem( hmenu, IDM_TEST_MODE, MF_GRAYED | MF_BYCOMMAND );
    }
}
#endif

void WdeEnableSelectCustCntl( HMENU hmenu )
{
    if( WdeCustControlsLoaded() ) {
        EnableMenuItem( hmenu, IDM_SELCUST1, MF_ENABLED | MF_BYCOMMAND );
        EnableMenuItem( hmenu, IDM_SELCUST2, MF_ENABLED | MF_BYCOMMAND );
    } else {
        EnableMenuItem( hmenu, IDM_SELCUST1, MF_GRAYED | MF_BYCOMMAND );
        EnableMenuItem( hmenu, IDM_SELCUST2, MF_GRAYED | MF_BYCOMMAND );
    }
}

void WdeEnableCommonControlsMenu( HMENU hmenu )
{
    int i;

    i = WdeIsCurrentMDIWindowZoomed() ? 1 : 0;
    hmenu = GetSubMenu( hmenu, TOOLS_MENU + i );
    hmenu = GetSubMenu( hmenu, COMM_CTRL_MENU );

    WdeEnableAllMenuItems( hmenu, IsCommCtrlLoaded() );
}

void WdeEnableCustCntlTools( HMENU hmenu )
{
    if( WdeIsCurrentCustControlSet( 0 ) ) {
        EnableMenuItem( hmenu, IDM_CUSTOM1_TOOL, MF_ENABLED | MF_BYCOMMAND );
    } else {
        EnableMenuItem( hmenu, IDM_CUSTOM1_TOOL, MF_GRAYED | MF_BYCOMMAND );
    }

    if( WdeIsCurrentCustControlSet( 1 ) ) {
        EnableMenuItem( hmenu, IDM_CUSTOM2_TOOL, MF_ENABLED | MF_BYCOMMAND );
    } else {
        EnableMenuItem( hmenu, IDM_CUSTOM2_TOOL, MF_GRAYED | MF_BYCOMMAND );
    }
}

static void WdeCheckModeMenu( HMENU hmenu, WdeOrderMode mode, UINT flags )
{
    UINT    menu_id;

    menu_id = 0;

    switch( mode ) {
    case WdeSetOrder:
        menu_id = IDM_SET_ORDER;
        break;
    case WdeSetTabs:
        menu_id = IDM_SET_TABS;
        break;
    case WdeSetGroups:
        menu_id = IDM_SET_GROUPS;
        break;
    }

    if( menu_id ) {
        CheckMenuItem( hmenu, menu_id, flags );
        WdeSetRibbonItemState( menu_id, ( flags == MF_CHECKED ) );
    }
}

static void WdeEnableSelectDialogItem( HMENU hmenu )
{
    WdeResInfo  *info;
    UINT        cmd;

    info = WdeGetCurrentRes();

    if( WdeResInfoHasDialogs( info ) ) {
        cmd = MF_ENABLED;
    } else {
        cmd = MF_GRAYED;
    }

    EnableMenuItem( hmenu, IDM_SELECT_DIALOG, cmd | MF_BYCOMMAND );
    EnableMenuItem( hmenu, IDM_REMOVE_DIALOG, cmd | MF_BYCOMMAND );
}

static void WdeEnableAlignTools( HMENU hmenu, int offset )
{
    LIST    *list;
    UINT    cmd;
    int     count;
    OBJPTR  obj;

    list = WdeGetCurrObjectList();

    if( list != NULL ) {
        count = ListCount( list );
    } else {
        count = 0;
    }

    if( count > 1 ) {
        cmd = MF_ENABLED;
    } else if( count == 1 ) {
        obj = WdeGetCurrObject();
        if( obj != NULL && !ListFindElt( list, obj ) ) {
            cmd = MF_ENABLED;
        } else {
            cmd = MF_GRAYED;
        }
    } else {
        cmd = MF_GRAYED;
    }

    ListFree( list );

    hmenu = GetSubMenu( hmenu, EDIT_MENU + offset );

    EnableMenuItem( hmenu, ALIGN_SUBMENU, cmd | MF_BYPOSITION );
    EnableMenuItem( hmenu, SAMESIZE_SUBMENU, cmd | MF_BYPOSITION );
    EnableMenuItem( hmenu, SPACE_SUBMENU, cmd | MF_BYPOSITION );
}

static void WdeSetDialogObjectDialogMenu( HMENU hmenu, int offset,
                                          bool restorable, bool res_has_hash,
                                          WdeOrderMode mode )
{
    WdeEnableAllMenuItems( GetSubMenu( hmenu, DIALOG_MENU + offset ), TRUE );

    if( restorable ) {
        EnableMenuItem( hmenu, IDM_DIALOG_RESTORE, MF_ENABLED | MF_BYCOMMAND );
    } else {
        EnableMenuItem( hmenu, IDM_DIALOG_RESTORE, MF_GRAYED | MF_BYCOMMAND );
    }

    if( res_has_hash ) {
        EnableMenuItem( hmenu, IDM_WRITE_SYMBOLS, MF_ENABLED | MF_BYCOMMAND );
    } else {
        EnableMenuItem( hmenu, IDM_WRITE_SYMBOLS, MF_GRAYED | MF_BYCOMMAND );
    }

    if( mode != WdeLastOrderMode ) {
        WdeCheckModeMenu( hmenu, WdeLastOrderMode, MF_UNCHECKED );
        WdeCheckModeMenu( hmenu, mode, MF_CHECKED );
        WdeLastOrderMode = mode;
    }
}

#if 0
static void WdeEnableGotoMenu( HMENU hmenu )
{
    bool        visible;
    UINT        cmd;

    cmd = MF_GRAYED;
    visible = WdeIsCurrentObjectVisible();
    if( !visible ) {
        cmd = MF_ENABLED;
    }
    EnableMenuItem( hmenu, IDM_GOTO_OBJECT, cmd | MF_BYCOMMAND );
}
#endif

void WdeSetTestModeMenu( bool testing )
{
    HMENU   hmenu;
    int     i;

    i = WdeIsCurrentMDIWindowZoomed() ? 1 : 0;

    hmenu = WdeGetMenuHandle();
    WdeEnableAllMenuItems( hmenu, !testing );
    WdeEnableAllMenuItems( GetSubMenu( hmenu, DIALOG_MENU + i ), !testing );

    if( testing ) {
        EnableMenuItem( hmenu, DIALOG_MENU + i, MF_ENABLED | MF_BYPOSITION );
        EnableMenuItem( hmenu, IDM_TEST_MODE, MF_ENABLED );
        CheckMenuItem( hmenu, IDM_TEST_MODE, MF_CHECKED );
    } else {
        CheckMenuItem( hmenu, IDM_TEST_MODE, MF_UNCHECKED );
    }

    WdeSetRibbonItemState( IDM_TEST_MODE, testing );

    DrawMenuBar( WdeGetMainWindowHandle() );
}

void WdeSetControlObjectMenu ( bool dlg_is_parent, bool dlg_restorable,
                               bool res_has_hash, WdeOrderMode mode )
{
    HMENU   hmenu;
    int     i;

    i = WdeIsCurrentMDIWindowZoomed() ? 1 : 0;

    hmenu = WdeGetMenuHandle();

    if ( WdeLastObject != CONTROL || mode != WdeLastOrderMode ||
         WdeLastDlgIsParent != dlg_is_parent ) {
        if( dlg_is_parent ) {
            WdeSetDialogObjectDialogMenu( hmenu, i, dlg_restorable, res_has_hash, mode );
        } else {
            if( mode != WdeLastOrderMode ) {
                WdeCheckModeMenu( hmenu, WdeLastOrderMode, MF_UNCHECKED );
                WdeCheckModeMenu( hmenu, mode, MF_CHECKED );
                WdeLastOrderMode = mode;
            }
            WdeEnableAllMenuItems( GetSubMenu( hmenu, DIALOG_MENU + i ), FALSE );
            EnableMenuItem( hmenu, IDM_DIALOG_NEW, MF_ENABLED );
        }

        WdeEnableAllMenuItems( GetSubMenu( hmenu, EDIT_MENU + i ), TRUE );

        WdeLastObject = CONTROL;
        WdeLastDlgIsParent = dlg_is_parent;
    }

    WdeEnableSelectDialogItem( hmenu );
    WdeEnableAlignTools( hmenu, i );
    //WdeEnableTest( hmenu, mode == WdeSelect );
    //WdeEnableGotoMenu( hmenu );

#if 0
    DrawMenuBar( WdeGetMainWindowHandle() );
#endif
}

void WdeSetDialogObjectMenu ( bool restorable, bool res_has_hash, WdeOrderMode mode )
{
    HMENU   hmenu;
    int     i;

    i = WdeIsCurrentMDIWindowZoomed() ? 1 : 0;

    hmenu = WdeGetMenuHandle();

    if( WdeLastObject != DIALOG || WdeLastDlgRestorable != restorable ||
        mode != WdeLastOrderMode ) {

        WdeSetDialogObjectDialogMenu( hmenu, i, restorable, res_has_hash, mode );
        WdeEnableAllMenuItems( GetSubMenu( hmenu, EDIT_MENU + i ), TRUE );
        WdeLastObject = DIALOG;
        WdeLastDlgRestorable = restorable;
    }

    WdeEnableSelectDialogItem( hmenu );
    WdeEnableAlignTools( hmenu, i );
    //WdeEnableTest( hmenu, mode == WdeSelect );
    //WdeEnableGotoMenu( hmenu );

#if 0
    DrawMenuBar( WdeGetMainWindowHandle() );
#endif
}

void WdeSetBaseObjectMenu( bool has_hash )
{
    HMENU   hmenu;
    int     i;

    i = WdeIsCurrentMDIWindowZoomed() ? 1 : 0;

    hmenu = WdeGetMenuHandle();

    if( WdeLastObject != BASE ) {
        WdeEnableAllMenuItems( GetSubMenu( hmenu, EDIT_MENU + i ), FALSE );
        EnableMenuItem( hmenu, IDM_OPTIONS, MF_ENABLED | MF_BYCOMMAND );

        WdeEnableAllMenuItems( GetSubMenu( hmenu, DIALOG_MENU + i ), FALSE );
        EnableMenuItem( hmenu, IDM_DIALOG_NEW, MF_ENABLED | MF_BYCOMMAND );

        if( has_hash ) {
            EnableMenuItem( hmenu, IDM_WRITE_SYMBOLS, MF_ENABLED | MF_BYCOMMAND );
        } else {
            EnableMenuItem( hmenu, IDM_WRITE_SYMBOLS, MF_GRAYED | MF_BYCOMMAND );
        }

        WdeLastObject = BASE;
    }

    WdeEnableSelectDialogItem( hmenu );
    //WdeEnableGotoMenu( hmenu );

    hmenu = GetSubMenu( hmenu, EDIT_MENU + i );
    EnableMenuItem( hmenu, SYMBOLS_SUBMENU, MF_ENABLED | MF_BYPOSITION );

#if 0
    DrawMenuBar( WdeGetMainWindowHandle() );
#endif
}

void WdeEnablePasteItem( HMENU hmenu )
{
    UINT        cmd;
    WdeResInfo  *rinfo;

    cmd = MF_GRAYED;
    rinfo = WdeGetCurrentRes();
    if( rinfo != NULL && rinfo->editting ) {
        InitState( rinfo->forms_win );
        if( FMPasteValid() ) {
            cmd = MF_ENABLED;
        }
    }

    EnableMenuItem( hmenu, IDM_PASTEOBJECT, cmd | MF_BYCOMMAND );
}

bool WdeIsCurrentMDIWindowZoomed( void )
{
    WdeResInfo *info;

    info = WdeGetCurrentRes();

    if( info != NULL && info->res_win != NULL ) {
        return( IsZoomed( info->res_win ) != 0 );
    } else {
        return( FALSE );
    }
}
