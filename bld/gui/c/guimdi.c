/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "guiwind.h"
#include "guistr.h"
#include "guihook.h"
#include "guixmdi.h"
#include "guimdi.h"
#include "guixwind.h"

#include "clibext.h"


#define MDIWIN2ID(x)    (x + GUI_MDI_FIRST_WINDOW)
#define ID2MDIWIN(x)    (x - GUI_MDI_FIRST_WINDOW)

#define MAX_LENGTH      80

typedef struct {
    gui_window  *dlg_wnd;
    gui_ctl_id  list_id;
} dlg_init;

bool GUIMDI = false;

static gui_menu_struct MDIFirstSepMenu = {
    NULL, GUI_MDI_FIRST_SEPARATOR,  GUI_STYLE_MENU_SEPARATOR,   NULL,   GUI_NO_MENU
};

static gui_menu_struct MDISecondSepMenu = {
    NULL, GUI_MDI_SECOND_SEPARATOR, GUI_STYLE_MENU_SEPARATOR,   NULL,   GUI_NO_MENU
};

static gui_menu_struct MDIMoreMenu = {
    NULL, GUI_MDI_MORE_WINDOWS,     GUI_STYLE_MENU_ENABLED,     NULL,   GUI_NO_MENU
};

static gui_menu_struct MDIMenu[] = {
    {  NULL,    GUI_MDI_CASCADE,        GUI_STYLE_MENU_GRAYED,     NULL,    GUI_NO_MENU },
    {  NULL,    GUI_MDI_TILE_HORZ,      GUI_STYLE_MENU_GRAYED,     NULL,    GUI_NO_MENU },
    {  NULL,    GUI_MDI_TILE_VERT,      GUI_STYLE_MENU_GRAYED,     NULL,    GUI_NO_MENU },
    {  NULL,    GUI_MDI_ARRANGE_ICONS,  GUI_STYLE_MENU_GRAYED,     NULL,    GUI_NO_MENU },
};

static  char MenuHint[MAX_NUM_MDI_WINDOWS][MAX_LENGTH];

static  gui_ctl_id      GUIMDIMenuID    = 0;
static  gui_window      *Root           = NULL;
static  int             NumMDIWindows   = 0;
static  int             CurrMDIWindow   = -1;
static  gui_window      *MDIWindows[MAX_NUM_MDI_WINDOWS];

static  int             TotalWindows    = 0;
static  gui_window      **ChildWindows  = NULL;

static bool MDIMenuStructInitialized = false;

gui_window *GUIGetRoot( void )
{
    return( Root );
}

static int MDIGetWndIndex( gui_window *wnd )
{
    int     i;

    for( i = 0; i < MAX_NUM_MDI_WINDOWS; i++ ) {
        if( MDIWindows[i] == wnd ) {
            return( i );
        }
    }
    return( -1 );
}

static void EnableMDIMenus( gui_window *root, bool enable )
{
    GUIEnableMDIActions( enable );
    if( enable ) {
        if( GUIMDIMenuID != 0 ) {
            if( GUIGetMenuPopupCount( root, GUIMDIMenuID ) > 0 ) {
                GUIAppendMenuToPopup( root, GUIMDIMenuID, &MDISecondSepMenu, false );
            }
        }
    } else {
        GUIDeleteMenuItem( root, GUI_MDI_SECOND_SEPARATOR, false );
    }
}

static bool MDIAddMenu( gui_window *wnd, gui_window *parent_wnd, const gui_menu_items *menus )
{
    int         i;
    bool        has_items;
    bool        found_flag;
    gui_window  *root;

    if( GUIMDI && ( parent_wnd == NULL ) ) {
        found_flag = false;
        has_items = false;
        for( i = 0; i < menus->num_items; i++ ) {
            if( menus->menu[i].style & GUI_STYLE_MENU_MDIWINDOW ) {
                GUIMDIMenuID = menus->menu[i].id;
                found_flag = true;
                has_items = ( menus->menu[i].child.num_items > 0 );
                break;
            }
        }
        if( !found_flag ) {
            return( false );
        }
        GUIAddMDIActions( has_items, wnd );
        if( NumMDIWindows > 0 ) {
            root = GUIGetRootWindow();
            EnableMDIMenus( root, true );
        }
        return( true );
    }
    return( false );
}

static void MakeLabel( int index, char *name, char *label )
{
    if( GUIGetWindowText( MDIWindows[index], name, MAX_LENGTH - 3 ) == 0 ) {
        name[0] = '\0';
    }
    sprintf( label, "&%d %s", index + 1, name );
}


static void MakeHintText( int index, char *name )
{
    size_t  length;

    length = strlen( LIT( Window_Name_Hint ) );
    strncpy( MenuHint[index], LIT( Window_Name_Hint ), length );
    strcpy( MenuHint[index]+length, name );
}

static void InsertMenuForWindow( gui_window *root, int index, int position )
{
    char                name[MAX_LENGTH];
    char                label[MAX_LENGTH];
    gui_menu_struct     menu;

    MakeLabel( index, name, label );
    menu.child = NoMenu;
    menu.label = label;
    menu.id = MDIWIN2ID( index );
    menu.style = GUI_STYLE_MENU_ENABLED;
    if( index == CurrMDIWindow ) {
        menu.style |= GUI_STYLE_MENU_CHECKED;
    }
    MakeHintText( index, name );
    menu.hinttext = MenuHint[index];
    if( GUIMDIMenuID != 0 ) {
        GUIInsertMenuToPopup( root, GUIMDIMenuID, position, &menu, false );
    }
}

void MDIDeleteMenu( gui_ctl_id id )
{
    if( id == GUIMDIMenuID ) {
        GUIMDIMenuID = 0;
    }
}

void MDIResetMenus( gui_window *wnd, gui_window *parent_wnd, const gui_menu_items *menus )
{
    gui_window  *root;
    int         i;
    int         num_mdi_items;

    if( !MDIAddMenu( wnd, parent_wnd, menus ) ) {
        return;
    }
    root = GUIGetRootWindow();
    num_mdi_items = NumMDIWindows;
    if( num_mdi_items > MAX_NUM_MDI_WINDOWS )
        num_mdi_items = MAX_NUM_MDI_WINDOWS;
    for( i = 0; i < num_mdi_items; i++ ) {
        InsertMenuForWindow( root, i, -1 );
    }
    if( NumMDIWindows > MAX_NUM_MDI_WINDOWS ) {
        MDIMoreMenu.label = LIT( XMore_Windows );
        MDIMoreMenu.hinttext = LIT( More_Windows_Hint );
        GUIAppendMenuToPopup( root, GUIMDIMenuID, &MDIMoreMenu, false );
    }
}

bool GUIAPI GUIEnableMDIMenus( bool enable )
{
    int         i;
    gui_window  *root;
    int         num_mdi_items;

    root = GUIGetRootWindow();
    if( root != NULL ) {
        GUIEnableMDIActions( enable );
        num_mdi_items = NumMDIWindows;
        if( num_mdi_items > MAX_NUM_MDI_WINDOWS )
            num_mdi_items = MAX_NUM_MDI_WINDOWS;
        if( NumMDIWindows > MAX_NUM_MDI_WINDOWS ) {
            GUIEnableMenuItem( root, GUI_MDI_MORE_WINDOWS, enable, false );
        }
        for( i = 0; i < num_mdi_items; i++ ) {
            GUIEnableMenuItem( root, MDIWIN2ID( i ), enable, false );
        }
        return( true );
    }
    return( false );
}

void InitMDI( gui_window *wnd, gui_create_info *dlg_info )
{
    gui_window  *root;

    root = GUIGetRootWindow();
    MDIAddMenu( wnd, dlg_info->parent, &dlg_info->menus );
    if( GUIXInitMDI( wnd ) ) {
        if( dlg_info->parent && ( GUIGetParentWindow( dlg_info->parent ) != NULL ) ) {
            return;
        }
        if( CurrMDIWindow != -1 ) {
            GUICheckMenuItem( root, MDIWIN2ID( CurrMDIWindow ), false, false );
        }
        NumMDIWindows++;
        if( NumMDIWindows == 1 ) {
            EnableMDIMenus( root, true );
        }
        CurrMDIWindow = NumMDIWindows - 1;
        if( NumMDIWindows > MAX_NUM_MDI_WINDOWS ) {
            if( NumMDIWindows == MAX_NUM_MDI_WINDOWS + 1 ) {
                if( GUIMDIMenuID != 0 ) {
                    MDIMoreMenu.label = LIT( XMore_Windows );
                    MDIMoreMenu.hinttext = LIT( More_Windows_Hint );
                    GUIAppendMenuToPopup( root, GUIMDIMenuID, &MDIMoreMenu, false );
                }
            }
        } else {
            MDIWindows[CurrMDIWindow] = wnd;
            InsertMenuForWindow( root, CurrMDIWindow, -1 );
        }
    } else if( Root == NULL ) {
        Root = wnd;
    }
}

static void ChangeMenuTitle( gui_window *root, int index )
{
    char        name[MAX_LENGTH];
    char        label[MAX_LENGTH];

    MakeLabel( index, name, label );
    GUISetMenuText( root, MDIWIN2ID( index ), label, false );
    MakeHintText( index, name );
}

void ChangeTitle( gui_window *wnd )
{
    int         index;

    index = MDIGetWndIndex( wnd );
    if( index != -1 ) {
        ChangeMenuTitle( GUIGetRootWindow(), index );
    }
    if( index == CurrMDIWindow ) {
        GUIXChangeTitle( wnd );
    }
}

void BroughtToFront( gui_window *wnd )
{
    gui_window  *root;
    int         index;

    root = GUIGetRootWindow();
    index = MDIGetWndIndex( wnd );
    if( ( CurrMDIWindow != -1 ) && ( root != NULL ) ) {
        GUICheckMenuItem( root, MDIWIN2ID( CurrMDIWindow ), false, false );
    }
    CurrMDIWindow = index;
    if( ( CurrMDIWindow != -1 ) && ( root != NULL ) ) {
        GUICheckMenuItem( root, MDIWIN2ID( CurrMDIWindow ), true, false );
    }
}

gui_window *FindNextMDIMenuWindowNotInArray( gui_window *wnd, gui_window *avoid )
{
    gui_window  *start, *next, *parent_wnd;
    bool        done;

    done = false;
    start = next = wnd;
    parent_wnd = GUIGetParentWindow( wnd );
    while( !done ) {
        next = GUIGetNextWindow( next );
        if( next == NULL ) {
            next = GUIGetFirstSibling( start );
        }
        if( parent_wnd != GUIGetParentWindow( next ) ) {
            continue;
        }
        if( next == start ) {
            break;
        }
        if( ( next != avoid ) && MDIGetWndIndex( next ) == -1 ) {
            done = true;
        }
    }

    if( done ) {
        return( next );
    } else {
        return( NULL );
    }
}

/*
 * MDIDelete -- make adjustments need to reflect the fact the wnd was deleted
 */

void MDIDelete( gui_window *wnd )
{
    gui_window  *root;
    int         deleted_item;
    int         position;
    int         i;
    int         num_mdi_items;

    if( wnd == Root ) {
        Root = NULL;
    }
    if( Root == NULL ) {
        return;
    }

    root = GUIGetRootWindow();

    // This check will make sure that windows that are not children of
    // the root window are ignored
    if( GUIGetParentWindow( wnd ) != root ) {
        return;
    }

    deleted_item = MDIGetWndIndex( wnd );
    if( NumMDIWindows == ( MAX_NUM_MDI_WINDOWS + 1 ) ) {
        GUIDeleteMenuItem( root, GUI_MDI_MORE_WINDOWS, false );
    }
    NumMDIWindows--;
    if( deleted_item != -1 ) {
        if( deleted_item == NumMDIWindows ) {
            GUIDeleteMenuItem( root, MDIWIN2ID( deleted_item ), false );
            MDIWindows[deleted_item] = NULL;
        } else {
            // delete all MDI menu items from this index on
            num_mdi_items = NumMDIWindows;
            if( num_mdi_items > MAX_NUM_MDI_WINDOWS - 1 )
                num_mdi_items = MAX_NUM_MDI_WINDOWS - 1;
            for( i = deleted_item; i < num_mdi_items; i++ ) {
                GUIDeleteMenuItem( root, MDIWIN2ID( i ), false );
                MDIWindows[i] = MDIWindows[i + 1];
            }
            GUIDeleteMenuItem( root, MDIWIN2ID( num_mdi_items ), false );
            MDIWindows[num_mdi_items] = NULL;

            // re-add all menu items from index on
            position = GUIGetMenuPopupCount( root, GUIMDIMenuID );
            if( NumMDIWindows > MAX_NUM_MDI_WINDOWS ) {
                position--;
            }
            for( i = deleted_item; i < num_mdi_items; i++ ) {
                InsertMenuForWindow( root, i, position + ( i - deleted_item ) );
                if( CurrMDIWindow == i ) {
                    CurrMDIWindow--;
                    if( CurrMDIWindow < 0 ) {
                        CurrMDIWindow = -1;
                    } else {
                        GUICheckMenuItem( root, MDIWIN2ID( CurrMDIWindow ), true, false );
                    }
                }
            }

            // Fill in the last spot in the MDIWindows array and insert it
            // after the 8th element
            if( NumMDIWindows >= MAX_NUM_MDI_WINDOWS ) {
                MDIWindows[MAX_NUM_MDI_WINDOWS - 1] =
                    FindNextMDIMenuWindowNotInArray( MDIWindows[MAX_NUM_MDI_WINDOWS - 2], wnd );
                if( MDIWindows[MAX_NUM_MDI_WINDOWS - 1] != NULL ) {
                    position = GUIGetMenuPopupCount( root, GUIMDIMenuID ) - 1;
                    if( NumMDIWindows > MAX_NUM_MDI_WINDOWS ) {
                        position--;
                    }
                    InsertMenuForWindow( root, MAX_NUM_MDI_WINDOWS - 1, position );
                }
            }
        }
    }

    if( NumMDIWindows <= 0 ) {
        EnableMDIMenus( root, false );
        CurrMDIWindow = -1;
    }
}

/*
 * DlgInit -- callback function for GUIEnumChildWindows to fill list box
 *            with names of MDI windows
 */

static void DlgInit( gui_window *wnd, void *param )
{
    char        buffer[MAX_LENGTH];
    dlg_init    *info;

    info = (dlg_init *)param;
    TotalWindows++;
    ChildWindows[TotalWindows - 1] = wnd;
    if( GUIGetWindowText( wnd, buffer, sizeof( buffer ) ) != 0 ) {
        GUIAddText( info->dlg_wnd, info->list_id, buffer );
    } else {
        GUIAddText( info->dlg_wnd, info->list_id, "" );
    }
    if( wnd == GUICurrWnd ) {
        GUISetCurrSelect( info->dlg_wnd, info->list_id, TotalWindows - 1 );
    }
}

/*
 * DlgCount -- callback function for GUIEnumChildWindows to count the number
 *             of MDI child windows
 */

static void DlgCount( gui_window *wnd, void *param )
{
    /* unused parameters */ (void)param; (void)wnd;

    TotalWindows++;
}

static void IconCount( gui_window *wnd, void *param )
{
    /* unused parameters */ (void)param;

    if( GUIIsMinimized( wnd ) ) {
        TotalWindows++;
    }
}

/*
 * GUIGetNumChildWindows -- return the number of MDI child windows
 */

int GUIGetNumChildWindows( void )
{
    TotalWindows = 0;
    GUIEnumChildWindows( GUIGetRootWindow(), &DlgCount, NULL );
    return( TotalWindows );
}

int GUIGetNumIconicWindows( void )
{
    TotalWindows = 0;
    GUIEnumChildWindows( GUIGetRootWindow(), &IconCount, NULL );
    return( TotalWindows );
}

/*
 * PickInit -- callback procedure to GUIDlgPick function
 */

static void PickInit( gui_window *wnd, gui_ctl_id list_id )
{
    gui_window  *root;
    int         num_windows;
    dlg_init    info;

    root = GUIGetRootWindow();
    num_windows = GUIGetNumChildWindows();
    ChildWindows = (gui_window **)GUIMemAlloc( sizeof( gui_window * ) * num_windows );
    info.dlg_wnd = wnd;
    info.list_id = list_id;
    TotalWindows = 0;
    GUIEnumChildWindows( root, &DlgInit, &info );
}

void GUIMDIMoreWindows( void )
{
    int         choice;
    gui_window  *wnd;

    if( GUIDlgPick( LIT( Select_Window ), &PickInit, &choice ) ) {
        if( ( choice >= 0 ) && ( choice < TotalWindows ) ) {
            wnd = ChildWindows[choice];
            if( GUIIsMinimized( wnd ) ) {
                GUIRestoreWindow( wnd );
            }
            GUIBringToFront( wnd );
        }
    }
    GUIMemFree( ChildWindows );
    ChildWindows = NULL;
    TotalWindows = 0;
}

gui_window *GUIMDIGetWindow( gui_ctl_id id )
{
    int         index;

    index = ID2MDIWIN( id );
    if( index < MAX_NUM_MDI_WINDOWS ) {
        return( MDIWindows[index] );
    } else {
        return( NULL );
    }
}

static void InitMDIMenuStruct( void )
{
    MDIMenu[0].label = LIT( XCascade );
    MDIMenu[0].hinttext = LIT( Cascade_Hint );
    MDIMenu[1].label = LIT( Tile_XHorz );
    MDIMenu[1].hinttext = LIT( Tile_Horz_Hint );
    MDIMenu[2].label = LIT( Tile_XVert );
    MDIMenu[2].hinttext = LIT( Tile_Vert_Hint );
    MDIMenu[3].label = LIT( XArrange_Icons );
    MDIMenu[3].hinttext = LIT( Arrange_Icons_Hint );
}

void EnableMDIActions( bool enable )
{
    GUIEnableMenuItem( Root, GUI_MDI_CASCADE, enable, false );
    GUIEnableMenuItem( Root, GUI_MDI_TILE_HORZ, enable, false );
    GUIEnableMenuItem( Root, GUI_MDI_TILE_VERT, enable, false );
    GUIEnableMenuItem( Root, GUI_MDI_ARRANGE_ICONS, enable, false );
}

void AddMDIActions( bool has_items, gui_window *wnd )
{
    int         i;

    if( !MDIMenuStructInitialized ) {
        InitMDIMenuStruct();
        MDIMenuStructInitialized = true;
    }

    if( has_items ) {
        GUIAppendMenuToPopup( wnd, GUIMDIMenuID, &MDIFirstSepMenu, false );
    }

    for( i = 0; i < GUI_ARRAY_SIZE( MDIMenu ); i++ ) {
        GUIAppendMenuToPopup( wnd, GUIMDIMenuID, &MDIMenu[i], false );
    }
}
