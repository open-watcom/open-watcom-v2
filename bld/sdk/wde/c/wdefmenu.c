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


#include <windows.h>

#include "wdeglbl.h"
#include "wdemain.h"
#include "wderes.h"
#include "wdesdlg.h"
#include "wderibbn.h"
#include "wdecurr.h"
#include "wdegoto.h"
#include "wde_rc.h"
#include "wdecctl.h"
#include "wdecust.h"
#include "wdefmenu.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum {
    BASE = 1
,   DIALOG
,   CONTROL
} WdeLastObjectType;

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static Bool              WdeLastDlgRestorable = 0;
static Bool              WdeLastDlgIsParent   = 0;
static WdeLastObjectType WdeLastObject        = 0;
static WdeOrderMode      WdeLastOrderMode     = WdeSelect;

static void WdeEnableAllMenuItems( HMENU menu, Bool enable )
{
    int count;

    count = GetMenuItemCount ( menu );
    for ( count--; count >= 0; count-- ) {
        if( enable ) {
            EnableMenuItem( menu, count, MF_ENABLED | MF_BYPOSITION);
        } else {
            EnableMenuItem( menu, count, MF_GRAYED  | MF_BYPOSITION);
        }
    }
}

#if 0
static void WdeEnableTest( HMENU menu, Bool enable )
{
    if( enable ) {
        EnableMenuItem( menu, IDM_TEST_MODE,  MF_ENABLED|MF_BYCOMMAND );
    } else {
        EnableMenuItem( menu, IDM_TEST_MODE,  MF_GRAYED|MF_BYCOMMAND );
    }
}
#endif

void WdeEnableSelectCustCntl( HMENU menu )
{
    if( WdeCustControlsLoaded() ) {
        EnableMenuItem( menu, IDM_SELCUST1,  MF_ENABLED|MF_BYCOMMAND );
        EnableMenuItem( menu, IDM_SELCUST2,  MF_ENABLED|MF_BYCOMMAND );
    } else {
        EnableMenuItem( menu, IDM_SELCUST1,  MF_GRAYED|MF_BYCOMMAND );
        EnableMenuItem( menu, IDM_SELCUST2,  MF_GRAYED|MF_BYCOMMAND );
    }
}

void WdeEnableCommonControlsMenu( HMENU menu )
{
    int         i;
    Bool        enable;

    i = ( WdeIsCurrentMDIWindowZoomed() ) ? 1 : 0;
    menu = GetSubMenu( menu, TOOLS_MENU + i );
    menu = GetSubMenu( menu, COMM_CTRL_MENU );

    enable = FALSE;
    if( WdeUsingCommonControls() ) {
        enable = TRUE;
    }

    WdeEnableAllMenuItems( menu, enable );
}

void WdeEnableCustCntlTools( HMENU menu )
{
    if( WdeIsCurrentCustControlSet( 0 ) ) {
        EnableMenuItem( menu, IDM_CUSTOM1_TOOL,  MF_ENABLED|MF_BYCOMMAND );
    } else {
        EnableMenuItem( menu, IDM_CUSTOM1_TOOL,  MF_GRAYED|MF_BYCOMMAND );
    }

    if( WdeIsCurrentCustControlSet( 1 ) ) {
        EnableMenuItem( menu, IDM_CUSTOM2_TOOL,  MF_ENABLED|MF_BYCOMMAND );
    } else {
        EnableMenuItem( menu, IDM_CUSTOM2_TOOL,  MF_GRAYED|MF_BYCOMMAND );
    }
}

static void WdeCheckModeMenu( HMENU menu, WdeOrderMode mode, UINT flags )
{
    UINT menu_id;

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
        CheckMenuItem( menu, menu_id, flags );
        WdeSetRibbonItemState( LOWORD(menu_id), ( flags == MF_CHECKED ) );
    }
}

static void WdeEnableSelectDialogItem( HMENU menu )
{
    WdeResInfo *info;
    UINT        cmd;

    info = WdeGetCurrentRes ();

    if ( WdeResInfoHasDialogs ( info ) ) {
        cmd = MF_ENABLED;
    } else {
        cmd = MF_GRAYED;
    }

    EnableMenuItem ( menu, IDM_SELECT_DIALOG, cmd | MF_BYCOMMAND );
    EnableMenuItem ( menu, IDM_REMOVE_DIALOG, cmd | MF_BYCOMMAND );
}

static void WdeEnableAlignTools( HMENU menu, int offset )
{
    LIST   *list;
    UINT    cmd;
    int     count;
    OBJPTR  obj;

    list = WdeGetCurrObjectList();

    if( list ) {
        count = ListCount( list );
    } else {
        count = 0;
    }

    if( count > 1 ) {
        cmd = MF_ENABLED;
    } else if( count == 1 ) {
        obj = WdeGetCurrObject();
        if( obj && !ListFindElt( list, obj ) ) {
            cmd = MF_ENABLED;
        } else {
            cmd = MF_GRAYED;
        }
    } else {
        cmd = MF_GRAYED;
    }

    ListFree( list );

    menu = GetSubMenu( menu, EDIT_MENU + offset );

    EnableMenuItem( menu, ALIGN_SUBMENU,    cmd | MF_BYPOSITION );
    EnableMenuItem( menu, SAMESIZE_SUBMENU, cmd | MF_BYPOSITION );
}

static void WdeSetDialogObjectDialogMenu( HMENU menu, int offset,
                                          Bool restorable, Bool res_has_hash,
                                          WdeOrderMode mode )
{
    WdeEnableAllMenuItems( GetSubMenu(menu, DIALOG_MENU+offset), TRUE );

    if( restorable ) {
        EnableMenuItem( menu, IDM_DIALOG_RESTORE,  MF_ENABLED|MF_BYCOMMAND );
    } else {
        EnableMenuItem( menu, IDM_DIALOG_RESTORE,  MF_GRAYED|MF_BYCOMMAND );
    }

    if( res_has_hash ) {
        EnableMenuItem( menu, IDM_WRITE_SYMBOLS, MF_ENABLED|MF_BYCOMMAND );
    } else {
        EnableMenuItem( menu, IDM_WRITE_SYMBOLS, MF_GRAYED|MF_BYCOMMAND );
    }

    if( mode != WdeLastOrderMode ) {
        WdeCheckModeMenu( menu, WdeLastOrderMode, MF_UNCHECKED );
        WdeCheckModeMenu( menu, mode, MF_CHECKED );
        WdeLastOrderMode = mode;
    }
}

#if 0
static void WdeEnableGotoMenu( HMENU menu )
{
    Bool        visible;
    UINT        cmd;

    cmd = MF_GRAYED;
    visible = WdeIsCurrentObjectVisible();
    if( !visible ) {
        cmd = MF_ENABLED;
    }
    EnableMenuItem( menu, IDM_GOTO_OBJECT, cmd | MF_BYCOMMAND );
}
#endif

void WdeSetTestModeMenu( Bool testing )
{
    HMENU menu;
    int   i;

    i = ( WdeIsCurrentMDIWindowZoomed() ) ? 1 : 0;

    menu = WdeGetMenuHandle( );

    WdeEnableAllMenuItems( menu, !testing );

    WdeEnableAllMenuItems( GetSubMenu(menu, DIALOG_MENU+i), !testing );

    if( testing ) {
        EnableMenuItem( menu, DIALOG_MENU + i, MF_ENABLED | MF_BYPOSITION);
        EnableMenuItem( menu, IDM_TEST_MODE, MF_ENABLED );
        CheckMenuItem( menu, IDM_TEST_MODE, MF_CHECKED );
    } else {
        CheckMenuItem( menu, IDM_TEST_MODE, MF_UNCHECKED );
    }

    WdeSetRibbonItemState( IDM_TEST_MODE, testing );

    DrawMenuBar( WdeGetMainWindowHandle () );
}

void WdeSetControlObjectMenu ( Bool dlg_is_parent, Bool dlg_restorable,
                               Bool res_has_hash, WdeOrderMode mode )
{
    HMENU menu;
    int   i;

    i = ( WdeIsCurrentMDIWindowZoomed() ) ? 1 : 0;

    menu = WdeGetMenuHandle ( );

    if ( ( WdeLastObject != CONTROL ) || ( mode != WdeLastOrderMode ) ||
         ( WdeLastDlgIsParent != dlg_is_parent ) )  {
        if ( dlg_is_parent ) {
            WdeSetDialogObjectDialogMenu ( menu, i, dlg_restorable,
                                           res_has_hash, mode );
        } else {
            if( mode != WdeLastOrderMode ) {
                WdeCheckModeMenu ( menu, WdeLastOrderMode, MF_UNCHECKED );
                WdeCheckModeMenu ( menu, mode, MF_CHECKED );
                WdeLastOrderMode = mode;
            }
            WdeEnableAllMenuItems ( GetSubMenu(menu, DIALOG_MENU+i), FALSE );
            EnableMenuItem ( menu, IDM_DIALOG_NEW, MF_ENABLED );
        }

        WdeEnableAllMenuItems( GetSubMenu(menu, EDIT_MENU+i), TRUE );

        WdeLastObject = CONTROL;
        WdeLastDlgIsParent = dlg_is_parent;
    }

    WdeEnableSelectDialogItem( menu );
    WdeEnableAlignTools( menu, i );
    //WdeEnableTest( menu, ( mode == WdeSelect ) );
    //WdeEnableGotoMenu( menu );

#if 0
    DrawMenuBar( WdeGetMainWindowHandle() );
#endif
}

void WdeSetDialogObjectMenu ( Bool restorable, Bool res_has_hash,
                              WdeOrderMode mode  )
{
    HMENU menu;
    int   i;

    i = ( WdeIsCurrentMDIWindowZoomed() ) ? 1 : 0;

    menu = WdeGetMenuHandle ();

    if( ( WdeLastObject != DIALOG ) ||
        ( WdeLastDlgRestorable != restorable ) ||
        ( mode != WdeLastOrderMode ) )  {

        WdeSetDialogObjectDialogMenu( menu, i, restorable, res_has_hash,
                                      mode );
        WdeEnableAllMenuItems( GetSubMenu(menu, EDIT_MENU+i), TRUE );
        WdeLastObject = DIALOG;
        WdeLastDlgRestorable = restorable;
    }

    WdeEnableSelectDialogItem( menu );
    WdeEnableAlignTools( menu, i );
    //WdeEnableTest( menu, ( mode == WdeSelect ) );
    //WdeEnableGotoMenu( menu );

#if 0
    DrawMenuBar ( WdeGetMainWindowHandle () );
#endif
}

void WdeSetBaseObjectMenu ( Bool has_hash )
{
    HMENU menu;
    int   i;

    i = ( WdeIsCurrentMDIWindowZoomed() ) ? 1 : 0;

    menu = WdeGetMenuHandle ( );

    if ( WdeLastObject != BASE ) {
        WdeEnableAllMenuItems ( GetSubMenu(menu, EDIT_MENU+i), FALSE );
        EnableMenuItem ( menu, IDM_OPTIONS, MF_ENABLED | MF_BYCOMMAND );

        WdeEnableAllMenuItems ( GetSubMenu(menu, DIALOG_MENU+i), FALSE );
        EnableMenuItem ( menu, IDM_DIALOG_NEW, MF_ENABLED | MF_BYCOMMAND );

        if ( has_hash ) {
            EnableMenuItem ( menu, IDM_WRITE_SYMBOLS, MF_ENABLED | MF_BYCOMMAND );
        } else {
            EnableMenuItem ( menu, IDM_WRITE_SYMBOLS, MF_GRAYED | MF_BYCOMMAND );
        }

        WdeLastObject = BASE;
    }

    WdeEnableSelectDialogItem( menu );
    //WdeEnableGotoMenu( menu );

    menu = GetSubMenu( menu, EDIT_MENU + i );
    EnableMenuItem( menu, SYMBOLS_SUBMENU, MF_ENABLED | MF_BYPOSITION );

#if 0
    DrawMenuBar ( WdeGetMainWindowHandle () );
#endif
}

void WdeEnablePasteItem ( HMENU menu )
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

    EnableMenuItem ( menu, IDM_PASTEOBJECT, cmd | MF_BYCOMMAND );
}

Bool WdeIsCurrentMDIWindowZoomed ( void )
{
    WdeResInfo *info;

    info = WdeGetCurrentRes ();

    if ( info && ( info->res_win != NULL ) ) {
        return ( IsZoomed ( info->res_win ) );
    } else {
        return ( FALSE );
    }
}

