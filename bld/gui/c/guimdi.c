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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "guiwind.h"
#include "guistr.h"
#include "guihook.h"
#include "guixmdi.h"
#include "clibext.h"

#define MAX_LENGTH      80

extern void GUIAddMDIActions( bool has_items, gui_window *wnd );

static gui_menu_struct MDISecondSepMenu[] = {
{ NULL,         GUI_MDI_SECOND_SEPARATOR, GUI_SEPARATOR,        NULL }
};

static  char MenuHint[MAX_NUM_MDI_WINDOWS][MAX_LENGTH];

static gui_menu_struct MDIMoreMenu[] = {
{    NULL, GUI_MDI_MORE_WINDOWS, GUI_ENABLED, NULL    }
};

extern  bool            GUIMDI;
extern  gui_window      *GUICurrWnd;
        int             GUIMDIMenuID = -1;
        gui_window      *Root   = NULL;
static  int             NumMDIWindows   = 0;
static  int             CurrMDIWindow   = -1;
static  gui_window      *MDIWindows[MAX_NUM_MDI_WINDOWS];

static  int             TotalWindows    = 0;
static  gui_window      **ChildWindows  = NULL;

typedef struct {
    gui_window  *dlg_wnd;
    unsigned    list_ctrl;
} dlg_init;

gui_window *GUIGetRoot( void )
{
    return( Root );
}

static int GetIndex( gui_window *wnd )
{
    int i;

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
        if( GUIMDIMenuID!=-1 && GUIGetMenuPopupCount( root, GUIMDIMenuID )!=0 ){
            GUIAppendMenuToPopup( root, GUIMDIMenuID, MDISecondSepMenu, false );
        }
    } else {
        GUIDeleteMenuItem( root, GUI_MDI_SECOND_SEPARATOR, false );
    }
}

static bool AddMenu( gui_window *wnd, gui_window *parent, int num_menus,
                     gui_menu_struct *menu )
{
    int         i;
    int         has_items;
    bool        found_flag;
    gui_window  *root;

    if( GUIMDI && ( parent == NULL ) ) {
        found_flag = false;
        for( i = 0; i < num_menus; i++ ) {
            if( menu[i].style & GUI_MDIWINDOW ) {
                GUIMDIMenuID = menu[i].id;
                found_flag = true;
                has_items = ( menu[i].num_child_menus > 0 );
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
    int length;

    length = strlen( LIT( Window_Name_Hint ) );
    strncpy( MenuHint[index], LIT( Window_Name_Hint ), length );
    strcpy( MenuHint[index]+length, name );
}

static void InsertMenuForWindow( gui_window *root, int index, int offset )
{
    char                name[MAX_LENGTH];
    char                label[MAX_LENGTH];
    gui_menu_struct     menu;

    MakeLabel( index, name, label );
    menu.label = label;
    menu.id = GUI_MDI_FIRST_WINDOW + index;
    menu.style = GUI_ENABLED;
    if( index == CurrMDIWindow ) {
        menu.style |= GUI_CHECKED;
    }
    menu.num_child_menus = 0;
    menu.child = NULL;
    MakeHintText( index, name );
    menu.hinttext = MenuHint[index];
    if( GUIMDIMenuID != -1 ) {
        GUIInsertMenuToPopup( root, GUIMDIMenuID, offset, &menu, false );
    }
}

void MDIDeleteMenu( unsigned id )
{
    if( id == GUIMDIMenuID ) {
        GUIMDIMenuID = -1;
    }
}

void MDIResetMenus( gui_window *wnd, gui_window *parent, int num_menus, gui_menu_struct *menu )
{
    gui_window  *root;
    int         i;
    int         max_num;

    if( !AddMenu( wnd, parent, num_menus, menu ) ) {
        return;
    }
    root = GUIGetRootWindow();
    max_num = NumMDIWindows;
    if( NumMDIWindows > MAX_NUM_MDI_WINDOWS ) {
        max_num = MAX_NUM_MDI_WINDOWS;
    }
    for( i = 0; i < max_num; i++ ) {
        InsertMenuForWindow( root, i, -1 );
    }
    if( NumMDIWindows > MAX_NUM_MDI_WINDOWS ) {
        MDIMoreMenu[0].label = LIT( XMore_Windows );
        MDIMoreMenu[0].hinttext = LIT( More_Windows_Hint );
        GUIAppendMenuToPopup( root, GUIMDIMenuID, MDIMoreMenu, false );
    }
}

bool GUIEnableMDIMenus( bool enable )
{
    int         i;
    int         num_menus;
    gui_window  *root;

    root = GUIGetRootWindow();
    if( root != NULL ) {
        GUIEnableMDIActions( enable );
        num_menus = NumMDIWindows;
        if( NumMDIWindows > MAX_NUM_MDI_WINDOWS ) {
            num_menus = MAX_NUM_MDI_WINDOWS;
            GUIEnableMenuItem( root, GUI_MDI_MORE_WINDOWS, enable, false );
        }
        for( i = 0; i < num_menus; i++ ) {
            GUIEnableMenuItem( root, GUI_MDI_FIRST_WINDOW + i, enable, false );
        }
        return( true );
    }
    return( false );
}

void InitMDI( gui_window *wnd, gui_create_info *dlg_info )
{
    gui_window  *root;

    root = GUIGetRootWindow();
    AddMenu( wnd, dlg_info->parent, dlg_info->num_menus, dlg_info->menu );
    if( GUIXInitMDI( wnd ) ) {
        if( dlg_info->parent && ( GUIGetParentWindow( dlg_info->parent ) != NULL ) ) {
            return;
        }
        if( CurrMDIWindow != -1 ) {
            GUICheckMenuItem( root, CurrMDIWindow + GUI_MDI_FIRST_WINDOW, false, false );
        }
        NumMDIWindows++;
        if( NumMDIWindows == 1 ) {
            EnableMDIMenus( root, true );
        }
        CurrMDIWindow = NumMDIWindows - 1;
        if( NumMDIWindows > MAX_NUM_MDI_WINDOWS ) {
            if( NumMDIWindows == MAX_NUM_MDI_WINDOWS + 1 ) {
                if( GUIMDIMenuID != -1 ) {
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
    GUISetMenuText( root, GUI_MDI_FIRST_WINDOW + index, label, false );
    MakeHintText( index, name );
}

void ChangeTitle( gui_window *wnd )
{
    int         index;

    index = GetIndex( wnd );
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
    index = GetIndex( wnd );
    if( ( CurrMDIWindow != -1 ) && ( root != NULL ) ) {
        GUICheckMenuItem( root, CurrMDIWindow + GUI_MDI_FIRST_WINDOW, false, false );
    }
    CurrMDIWindow = index;
    if( ( CurrMDIWindow != -1 ) && ( root != NULL ) ) {
        GUICheckMenuItem( root, CurrMDIWindow + GUI_MDI_FIRST_WINDOW, true, false );
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
        if( ( next != avoid ) && GetIndex( next ) == -1 ) {
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
    int         index, offset;
    int         i, num_menu_windows;

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

    index = GetIndex( wnd );
    if( NumMDIWindows == ( MAX_NUM_MDI_WINDOWS + 1 ) ) {
        GUIDeleteMenuItem( root, GUI_MDI_MORE_WINDOWS, false );
    }
    NumMDIWindows--;
    if( index != -1 ) {
        if( index == NumMDIWindows ) {
            GUIDeleteMenuItem( root, index + GUI_MDI_FIRST_WINDOW, false );
            MDIWindows[index] = NULL;
        } else {
            // delete all MDI menu items from this index on
            num_menu_windows = NumMDIWindows;
            if( num_menu_windows > MAX_NUM_MDI_WINDOWS - 1 )
                num_menu_windows = MAX_NUM_MDI_WINDOWS - 1;
            for( i = index; i < num_menu_windows; i++ ) {
                GUIDeleteMenuItem( root, i + GUI_MDI_FIRST_WINDOW, false );
                MDIWindows[i] = MDIWindows[i+1];
            }
            GUIDeleteMenuItem( root, num_menu_windows + GUI_MDI_FIRST_WINDOW, false );
            MDIWindows[num_menu_windows] = NULL;

            // re-add all menu items from index on
            offset = GUIGetMenuPopupCount ( root, GUIMDIMenuID );
            if( NumMDIWindows > MAX_NUM_MDI_WINDOWS ) {
                offset--;
            }
            for( i = index; i < num_menu_windows; i++ ) {
                InsertMenuForWindow( root, i, offset + i - index );
                if( CurrMDIWindow == i ) {
                    CurrMDIWindow--;
                    if( CurrMDIWindow < 0 ) {
                        CurrMDIWindow = -1;
                    } else {
                        GUICheckMenuItem( root, CurrMDIWindow + GUI_MDI_FIRST_WINDOW, true, false );
                    }
                }
            }

            // Fill in the last spot in the MDIWindows array and insert it
            // after the 8th element
            if( NumMDIWindows >= MAX_NUM_MDI_WINDOWS ) {
                MDIWindows[MAX_NUM_MDI_WINDOWS-1] =
                    FindNextMDIMenuWindowNotInArray( MDIWindows[MAX_NUM_MDI_WINDOWS-2], wnd );
                if( MDIWindows[MAX_NUM_MDI_WINDOWS-1] != NULL ) {
                    offset = GUIGetMenuPopupCount( root, GUIMDIMenuID ) - 1;
                    if( NumMDIWindows > MAX_NUM_MDI_WINDOWS ) {
                        offset--;
                    }
                    InsertMenuForWindow( root, MAX_NUM_MDI_WINDOWS-1, offset );
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
    ChildWindows[TotalWindows-1] = wnd;
    if( GUIGetWindowText( wnd, buffer, sizeof( buffer ) ) != 0 ) {
        GUIAddText( info->dlg_wnd, info->list_ctrl, buffer );
    } else {
        GUIAddText( info->dlg_wnd, info->list_ctrl, "" );
    }
    if( wnd == GUICurrWnd ) {
        GUISetCurrSelect( info->dlg_wnd, info->list_ctrl, TotalWindows - 1 );
    }
}

/*
 * DlgCount -- callback function for GUIEnumChildWindows to count the number
 *             of MDI child windows
 */

static void DlgCount( gui_window *wnd, void *param )
{
    param = param;
    wnd = wnd;
    TotalWindows++;
}

static void IconCount( gui_window *wnd, void *param )
{
    param = param;
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

static void PickInit( gui_window *wnd, unsigned list_ctrl )
{
    gui_window  *root;
    int         num_windows;
    dlg_init    info;

    root = GUIGetRootWindow();
    num_windows = GUIGetNumChildWindows();
    ChildWindows = (gui_window **)GUIMemAlloc( sizeof( gui_window *) * num_windows );
    info.dlg_wnd = wnd;
    info.list_ctrl = list_ctrl;
    TotalWindows = 0;
    GUIEnumChildWindows( root, &DlgInit, &info );
}

void GUIMDIMoreWindows( void )
{
    int         chosen;
    gui_window  *wnd;

    chosen = GUIDlgPick( LIT( Select_Window ), &PickInit );
    if( ( chosen >= 0 ) && ( chosen < TotalWindows ) ) {
        wnd = ChildWindows[chosen];
        if( GUIIsMinimized( wnd ) ) {
            GUIRestoreWindow( wnd );
        }
        GUIBringToFront( wnd );
    }
    GUIMemFree( ChildWindows );
    ChildWindows = NULL;
    TotalWindows = 0;
}

gui_window *GUIMDIGetWindow( int id )
{
    int         index;

    index = id - GUI_MDI_FIRST_WINDOW;
    if( index < MAX_NUM_MDI_WINDOWS ) {
        return( MDIWindows[index] );
    } else {
        return( NULL );
    }
}
