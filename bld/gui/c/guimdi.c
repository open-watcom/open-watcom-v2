/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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

static gui_menu_struct MDISecondSepMenu[] = {
    { NULL, GUI_MDI_SECOND_SEPARATOR, GUI_STYLE_MENU_SEPARATOR, NULL }
};

static  char MenuHint[MAX_NUM_MDI_WINDOWS][MAX_LENGTH];

static gui_menu_struct MDIMoreMenu[] = {
    { NULL, GUI_MDI_MORE_WINDOWS, GUI_STYLE_MENU_ENABLED, NULL }
};

static  gui_ctl_id      GUIMDIMenuID    = 0;
static  gui_window      *Root           = NULL;
static  int             NumMDIWindows   = 0;
static  int             CurrMDIWindow   = -1;
static  gui_window      *MDIWindows[MAX_NUM_MDI_WINDOWS];

static  int             TotalWindows    = 0;
static  gui_window      **ChildWindows  = NULL;

static gui_menu_struct MDIMenu[] = {
    {  NULL,    GUI_MDI_CASCADE,        GUI_STYLE_MENU_GRAYED,     NULL    },
    {  NULL,    GUI_MDI_TILE_HORZ,      GUI_STYLE_MENU_GRAYED,     NULL    },
    {  NULL,    GUI_MDI_TILE_VERT,      GUI_STYLE_MENU_GRAYED,     NULL    },
    {  NULL,    GUI_MDI_ARRANGE_ICONS,  GUI_STYLE_MENU_GRAYED,     NULL    },
};

static gui_menu_struct MDIFirstSepMenu[] = {
    {  NULL,    GUI_MDI_FIRST_SEPARATOR,    GUI_STYLE_MENU_SEPARATOR,    NULL }
};

static bool MDIMenuStructInitialized = false;

gui_window *GUIGetRoot( void )
{
    return( Root );
}

static int MDIGetWndIndex( gui_window *wnd )
{
    int     item;

    for( item = 0; item < MAX_NUM_MDI_WINDOWS; item++ ) {
        if( MDIWindows[item] == wnd ) {
            return( item );
        }
    }
    return( -1 );
}

static void EnableMDIMenus( gui_window *root, bool enable )
{
    GUIEnableMDIActions( enable );
    if( enable ) {
        if( GUIMDIMenuID != 0 && GUIGetMenuPopupCount( root, GUIMDIMenuID ) != 0 ){
            GUIAppendMenuToPopup( root, GUIMDIMenuID, MDISecondSepMenu, false );
        }
    } else {
        GUIDeleteMenuItem( root, GUI_MDI_SECOND_SEPARATOR, false );
    }
}

static bool AddMenu( gui_window *wnd, gui_window *parent, int num_items, gui_menu_struct *menu )
{
    int         item;
    bool        has_items;
    bool        found_flag;
    gui_window  *root;

    if( GUIMDI && ( parent == NULL ) ) {
        found_flag = false;
        has_items = false;
        for( item = 0; item < num_items; item++ ) {
            if( menu[item].style & GUI_STYLE_MENU_MDIWINDOW ) {
                GUIMDIMenuID = menu[item].id;
                found_flag = true;
                has_items = ( menu[item].num_child_menus > 0 );
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
    menu.label = label;
    menu.id = MDIWIN2ID( index );
    menu.style = GUI_STYLE_MENU_ENABLED;
    if( index == CurrMDIWindow ) {
        menu.style |= GUI_STYLE_MENU_CHECKED;
    }
    menu.num_child_menus = 0;
    menu.child = NULL;
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

void MDIResetMenus( gui_window *wnd, gui_window *parent, int num_items, gui_menu_struct *menu )
{
    gui_window  *root;
    int         item;
    int         num_mdi_items;

    if( !AddMenu( wnd, parent, num_items, menu ) ) {
        return;
    }
    root = GUIGetRootWindow();
    num_mdi_items = NumMDIWindows;
    if( num_mdi_items > MAX_NUM_MDI_WINDOWS )
        num_mdi_items = MAX_NUM_MDI_WINDOWS;
    for( item = 0; item < num_mdi_items; item++ ) {
        InsertMenuForWindow( root, item, -1 );
    }
    if( NumMDIWindows > MAX_NUM_MDI_WINDOWS ) {
        MDIMoreMenu[0].label = LIT( XMore_Windows );
        MDIMoreMenu[0].hinttext = LIT( More_Windows_Hint );
        GUIAppendMenuToPopup( root, GUIMDIMenuID, MDIMoreMenu, false );
    }
}

bool GUIEnableMDIMenus( bool enable )
{
    int         item;
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
        for( item = 0; item < num_mdi_items; item++ ) {
            GUIEnableMenuItem( root, MDIWIN2ID( item ), enable, false );
        }
        return( true );
    }
    return( false );
}

void InitMDI( gui_window *wnd, gui_create_info *dlg_info )
{
    gui_window  *root;

    root = GUIGetRootWindow();
    AddMenu( wnd, dlg_info->parent, dlg_info->num_items, dlg_info->menu );
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
                    MDIMoreMenu[0].label = LIT( XMore_Windows );
                    MDIMoreMenu[0].hinttext = LIT( More_Windows_Hint );
                    GUIAppendMenuToPopup( root, GUIMDIMenuID, MDIMoreMenu, false );
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
    gui_window  *start, *next, *parent;
    bool        done;

    done = false;
    start = next = wnd;
    parent = GUIGetParentWindow( wnd );
    while( !done ) {
        next = GUIGetNextWindow( next );
        if( next == NULL ) {
            next = GUIGetFirstSibling( start );
        }
        if( parent != GUIGetParentWindow( next ) ) {
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
    int         item;
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
            for( item = deleted_item; item < num_mdi_items; item++ ) {
                GUIDeleteMenuItem( root, MDIWIN2ID( item ), false );
                MDIWindows[item] = MDIWindows[item + 1];
            }
            GUIDeleteMenuItem( root, MDIWIN2ID( num_mdi_items ), false );
            MDIWindows[num_mdi_items] = NULL;

            // re-add all menu items from index on
            position = GUIGetMenuPopupCount( root, GUIMDIMenuID );
            if( NumMDIWindows > MAX_NUM_MDI_WINDOWS ) {
                position--;
            }
            for( item = deleted_item; item < num_mdi_items; item++ ) {
                InsertMenuForWindow( root, item, position + ( item - deleted_item ) );
                if( CurrMDIWindow == item ) {
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
        GUIAppendMenuToPopup( wnd, GUIMDIMenuID, MDIFirstSepMenu, false );
    }

    for( i = 0; i < ARRAY_SIZE( MDIMenu ); i++ ) {
        GUIAppendMenuToPopup( wnd, GUIMDIMenuID, &MDIMenu[i], false );
    }
}
