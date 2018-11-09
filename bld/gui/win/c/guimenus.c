/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  GUI utility functions: menus, popup windows
*
*               Example: \watcom\binnt\wdw.exe (wv built for Windows NT)
*
****************************************************************************/


#include "guiwind.h"
#include <string.h>
#include "guimenus.h"
#include "guixwind.h"
#include "guixutil.h"
#include "guifloat.h"
#include "guifont.h"
#include "guistr.h"
#include "guixhook.h"
#include "guihook.h"
#include "guimdi.h"


#define MAX_STR 256

gui_menu_struct GUIHint[] = {
 { NULL, GUI_CHANGE_FONT, GUI_STYLE_MENU_ENABLED, NULL },
 { NULL, GUI_FIX_TOOLBAR, GUI_STYLE_MENU_ENABLED, NULL }
};

void GUIInitGUIMenuHint( void )
{
    GUIHint[GUI_MENU_IDX( GUI_CHANGE_FONT )].label = LIT( Change_XFont_ );
    GUIHint[GUI_MENU_IDX( GUI_CHANGE_FONT )].hinttext = LIT( Change_Font_for_this_window  );
    GUIHint[GUI_MENU_IDX( GUI_FIX_TOOLBAR )].label = LIT( XFix_Tool_Bar );
    GUIHint[GUI_MENU_IDX( GUI_FIX_TOOLBAR )].hinttext = LIT( Make_Tool_Bar_Fixed );
}

static bool GetMenuFlags( HMENU hmenu, gui_ctl_id id_position, bool by_position,
                          unsigned *menu_flags, unsigned *attr_flags )
{
    WPI_MENUSTATE       mstate;

    if( hmenu != NULLHANDLE ) {
        _wpi_getmenustate( hmenu, id_position, &mstate, ( by_position ) ? TRUE : FALSE );
        _wpi_getmenuflagsfromstate( &mstate, menu_flags, attr_flags );
        return( true );
    }

    return( false );
}

static bool GetParentMenuPos( HMENU hmenu, HMENU popup, HMENU *parent, int *position )
{
    int         num_items;
    int         item;
    HMENU       submenu;

    num_items = _wpi_getmenuitemcount( hmenu );
    for( item = 0; item < num_items; item++ ) {
        submenu = _wpi_getsubmenu( hmenu, item );
        if( submenu != NULLHANDLE ) {
            if( submenu == popup ) {
                if( parent != NULL ) {
                    *parent = hmenu;
                }
                if( position != NULL ) {
                    *position = item;
                }
                return( true );
            } else {
                if( GetParentMenuPos( submenu, popup, parent, position ) ) {
                    return( true );
                }
            }
        }
    }
    return( false );
}

static HMENU GetPopupHMENU( gui_window *wnd, HMENU hmenu, gui_ctl_id id,
                            HMENU *parent, int *position, hint_type type )
{
    popup_info  *info;

    if( parent != NULL ) {
        *parent = NULLHANDLE;
    }
    if( position != NULL ) {
        *position = 0;
    }
    for( info = wnd->popup; info != NULL; info = info->next ) {
        if( ( info->id == id ) && ( info->type == type ) ) {
            if( ( parent != NULL ) || ( position != NULL ) ) {
                GetParentMenuPos( hmenu, info->popup, parent, position );
                if( parent != NULL && *parent == NULLHANDLE ) {
                    return( NULLHANDLE );
                }
            }
            return( info->popup );
        }
    }
    return( NULLHANDLE );
}

#if 0
void GetStateForMenu( gui_window *wnd, gui_ctl_id id, WPI_MENUSTATE *mstate )
{
    HMENU           hmenu, popup, parent;
    int             position;

    hmenu = GUIGetHMENU( wnd );
    if( hmenu == NULLHANDLE ) {
        return;
    }

    popup = GetPopupHMENU( wnd, hmenu, id, &parent, &position, MENU_HINT );

    if( popup != NULLHANDLE ) {
        _wpi_getmenustate( parent, position, mstate, TRUE );
    } else {
        _wpi_getmenustate( hmenu, id, mstate, FALSE );
    }
}

bool GUIIsMenuItemChecked( gui_window *wnd, gui_ctl_id id )
{
    WPI_MENUSTATE       mstate;

    GetStateForMenu( wnd, id, &mstate );

    return( _wpi_ismenucheckedfromstate( &mstate ) );
}

bool GUIIsMenuItemEnabled( gui_window *wnd, gui_ctl_id id )
{
    WPI_MENUSTATE       mstate;

    GetStateForMenu( wnd, id, &mstate );

    return( _wpi_ismenuenabledfromstate( &mstate ) );
}
#endif

void GUISetMenu( gui_window *wnd, HMENU hmenu )
{
    HWND        frame;
    WPI_RECT    rect;
    gui_coord   size;
    int         height;
#ifndef __OS2_PM__
    HMENU       omenu;
#endif

#ifndef __OS2_PM__
    omenu = GUIGetHMENU( wnd );
#endif
    frame = GUIGetParentFrameHWND( wnd );
    _wpi_getclientrect( frame, &rect );
    height = _wpi_getheightrect( rect );
    _wpi_setmenu( frame, hmenu );
    _wpi_getclientrect( frame, &rect );
    if( height != ( size.y = _wpi_getheightrect( rect ) ) ) {
        size.x = _wpi_getwidthrect( rect );
        GUIDoResize( wnd, GUIGetParentHWND( wnd ), &size );
    }
#ifndef __OS2_PM__
    if( omenu != NULLHANDLE ) {
        _wpi_destroymenu( omenu );
    }
#endif
}

static bool GetPopupId( gui_window *wnd, HMENU hmenu, gui_ctl_id *id )
{
    popup_info  *info;

    for( info = wnd->popup; info != NULL; info = info->next ) {
        if( info->popup == hmenu ) {
            *id = info->id;
            return( true );
        }
    }
    return( false );
}


static bool InsertPopup( gui_window *wnd, gui_ctl_id id, HMENU popup, hint_type type )
{
    popup_info  *info;
    HMENU       hmenu;

    hmenu = GetPopupHMENU( wnd, GUIGetHMENU( wnd ), id, NULL, NULL, type );
    if( hmenu != popup ) {
        info = (popup_info *)GUIMemAlloc( sizeof( popup_info ) );
        if( info != NULL ) {
            info->next = wnd->popup;
            wnd->popup = info;
            info->id = id;
            info->popup = popup;
            info->type = type;
            return( true );
        } else {
            return( false );
        }
    }
    return( true );
}

static void DeletePopup( gui_window *wnd, gui_ctl_id id )
{
    popup_info  *curr;
    popup_info  *prev;

    prev = NULL;
    for( curr = wnd->popup; curr != NULL; prev = curr, curr=curr->next ) {
        if( ( curr->id == id ) && ( curr->type == MENU_HINT ) ) {
            break;
        }
    }
    if( curr != NULL ) {
        if( prev != NULL ) {
            prev->next = curr->next;
        } else {
            wnd->popup = curr->next;
        }
        GUIMemFree( curr );
    }
}

void GUIDeleteFloatingPopups( gui_window *wnd )
{
    popup_info  *curr;
    popup_info  *prev;
    popup_info  *tmp;

    prev = NULL;
    for( curr = wnd->popup; curr != NULL; ) {
        if( curr->type == FLOAT_HINT ) {
            if( prev != NULL ) {
                prev->next = curr->next;
            } else {
                wnd->popup = curr->next;
            }
            tmp = curr->next;
            GUIMemFree( curr );
            curr = tmp;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void GUIFreePopupList( gui_window *wnd )
{
    popup_info  *curr;
    popup_info  *next;

    for( curr = wnd->popup; curr != NULL; curr = next ) {
        next = curr->next;
        GUIMemFree( curr );
    }
    wnd->popup = NULLHANDLE;
}

void GUISetGUIHint( gui_window *wnd )
{
    GUIInitHint( wnd, ARRAY_SIZE( GUIHint ), GUIHint, GUI_HINT );
}

HMENU   GUIHFloatingPopup       = NULLHANDLE;

/*
 * GUIGetHMENU -- return the HMENU that the user can change
 */

HMENU GUIGetHMENU( gui_window *wnd )
{
    HMENU hmenu;

    if( wnd->root_frame == NULLHANDLE ) { /* child window */
        if( wnd->hwnd_frame == NULLHANDLE )
            return( NULLHANDLE );
        hmenu = _wpi_getsystemmenu( wnd->hwnd_frame );
    } else {
        hmenu = _wpi_getmenu( wnd->root_frame );
    }
    return( hmenu );
}

static void GUIDrawMenuBar( gui_window *wnd )
{
    if( wnd->root_frame != NULLHANDLE ) {
        if( _wpi_getmenu( wnd->root_frame ) != NULLHANDLE ) {
            _wpi_drawmenubar( wnd->root_frame );
        }
    } else {
        if( wnd->style & GUI_POPUP ) {
            if( _wpi_getmenu( wnd->hwnd_frame ) != NULLHANDLE ) {
                _wpi_drawmenubar( wnd->hwnd_frame );
            }
        }
    }
}

int GUIGetMenuPopupCount( gui_window *wnd, gui_ctl_id id )
{
    HMENU       hmenu;
    HMENU       popup;
    int         num_items;

    hmenu = GUIGetHMENU( wnd );
    if( hmenu == NULLHANDLE ) {
        return( 0 );
    }

    popup = GetPopupHMENU( wnd, hmenu, id, NULL, NULL, MENU_HINT );

    if( popup != NULLHANDLE ) {
        num_items = _wpi_getmenuitemcount( popup );
    } else {
        num_items = -1;
    }

    return( num_items );
}

static HMENU GetOrMakeHMENU( gui_window *wnd, bool floating, bool *made_root )
{
    HMENU       hmenu;

    if( made_root ) {
        *made_root = false;
    }
    if( floating ) {
        if( GUIHFloatingPopup == NULLHANDLE ) {
            GUIHFloatingPopup = _wpi_createpopupmenu();
        }
        hmenu = GUIHFloatingPopup;
    } else {
        hmenu = GUIGetHMENU( wnd );
        if( hmenu == NULLHANDLE ) {
            if( wnd->root_frame != NULLHANDLE ) {
                hmenu = _wpi_createmenu();
                if( hmenu != NULLHANDLE ) {
                    //GUISetMenu( wnd, hmenu );
                    if( made_root ) {
                        *made_root = true;
                    }
                }
            }
        }
    }
    return( hmenu );
}

/*
 * GUIDeleteMenuItem -- delete the given menu item
 */

bool GUIDeleteMenuItem( gui_window *wnd, gui_ctl_id id, bool floating )
{
    HMENU       hmenu, popup, parent;
    int         position;
    hint_type   type;

    if( floating ) {
        type = FLOAT_HINT;
    } else {
        type = MENU_HINT;
    }
    hmenu = GetOrMakeHMENU( wnd, floating, NULL );
    if( hmenu == NULLHANDLE ) {
        return( false );
    }

    popup = GetPopupHMENU( wnd, hmenu, id, &parent, &position, type );

    if( popup != NULLHANDLE ) {
        _wpi_deletemenu( parent, position, TRUE );
    } else {
        _wpi_deletemenu( hmenu, id, FALSE );
    }

    GUIDeleteHintText( wnd, id );
    DeletePopup( wnd, id );
    GUIMDIDeleteMenuItem( id );

    if( hmenu == parent ) {
        GUIDrawMenuBar( wnd );
    }

    return( true );
}

static void CheckItem( HMENU hmenu, gui_ctl_id id, bool check )
{
    _wpi_checkmenuitem( hmenu, id, ( check ) ? TRUE : FALSE, FALSE );
}

static void CheckPopup( HMENU hmenu, int position, bool check )
{
    _wpi_checkmenuitem( hmenu, position, ( check ) ? TRUE : FALSE, TRUE );
}

/*
 * GUICheckMenuItem -- check or uncheck a menu item
 */

bool GUICheckMenuItem( gui_window *wnd, gui_ctl_id id, bool check, bool floating )
{
    HMENU       hmenu, popup, parent;
    int         position;
    hint_type   type;

    if( floating ) {
        type = FLOAT_HINT;
    } else {
        type = MENU_HINT;
    }
    hmenu = GetOrMakeHMENU( wnd, floating, NULL );
    if( hmenu == NULLHANDLE ) {
        return( false );
    }

    popup = GetPopupHMENU( wnd, hmenu, id, &parent, &position, MENU_HINT );

    if( popup != NULLHANDLE ) {
        CheckPopup( parent, position, check );
    } else {
        CheckItem( hmenu, id, check );
    }

    if( hmenu == parent ) {
        GUIDrawMenuBar( wnd );
    }

    return( true );
}

static void EnableItem( HMENU hmenu, gui_ctl_id id, bool enable )
{
    _wpi_enablemenuitem( hmenu, id, ( enable ) ? TRUE : FALSE, FALSE );
}

static void EnablePopup( HMENU hmenu, int position, bool enable )
{
    _wpi_enablemenuitem( hmenu, position, ( enable ) ? TRUE : FALSE, TRUE );
}

/*
 * GUIEnableSysMenuItem -- enable or disable a system menu item.  Used
 *                         by GUI library only
 */

bool GUIEnableSysMenuItem( gui_window *wnd, gui_ctl_id id, bool enable )
{
    HMENU       hmenu;

    hmenu = _wpi_getsystemmenu( GUIGetParentFrameHWND( wnd ) );
    if( hmenu != NULLHANDLE ) {
        EnableItem( hmenu, id, enable );
    }
    return( true );
}

/*
 * GUIEnableMenuItem -- enable or disable a menu item
 */

bool GUIEnableMenuItem( gui_window *wnd, gui_ctl_id id, bool enable, bool floating )
{
    HMENU       hmenu, popup, parent;
    int         position;
    hint_type   type;

    if( floating ) {
        type = FLOAT_HINT;
    } else {
        type = MENU_HINT;
    }
    hmenu = GetOrMakeHMENU( wnd, floating, NULL );
    if( hmenu == NULLHANDLE ) {
        return( false );
    }

    popup = GetPopupHMENU( wnd, hmenu, id, &parent, &position, MENU_HINT );

    if( popup != NULLHANDLE ) {
        EnablePopup( parent, position, enable );
    } else {
        EnableItem( hmenu, id, enable );
    }

    if( hmenu == parent ) {
        GUIDrawMenuBar( wnd );
    }

    return( true );
}

/*
 * GUISetMenuText -- change the text of a menu item
 */

bool GUISetMenuText( gui_window *wnd, gui_ctl_id id, const char *text, bool floating )
{
    HMENU       hmenu, popup, parent;
    int         position;
    hint_type   type;
    bool        ret;

    if( floating ) {
        type = FLOAT_HINT;
    } else {
        type = MENU_HINT;
    }
    hmenu = GetOrMakeHMENU( wnd, floating, NULL );
    if( hmenu == NULLHANDLE ) {
        return( false );
    }

    popup = GetPopupHMENU( wnd, hmenu, id, &parent, &position, MENU_HINT );

    if ( popup != NULLHANDLE ) {
        ret = ( _wpi_setmenutext( parent, position, text, TRUE ) != 0 );
    } else {
        ret = ( _wpi_setmenutext( hmenu, id, text, FALSE ) != 0 );
    }

    if( hmenu == parent ) {
        GUIDrawMenuBar( wnd );
    }

    return( ret );
}

/*
 * GUICreateMenuFlags -- take gui_menu_styles information and return Windows
 *                       menu style
 */

void GUICreateMenuFlags( gui_menu_styles style, unsigned *menu_flags, unsigned *attr_flags )
{
    *menu_flags = 0;
    *attr_flags = 0;
    if( style == GUI_STYLE_MENU_ENABLED ) {
        *attr_flags |= MF_ENABLED;
    }
    if( style & GUI_STYLE_MENU_CHECKED ) {
        *attr_flags |= MF_CHECKED;
    }
    if( style & GUI_STYLE_MENU_GRAYED ) {
        *attr_flags |= MF_GRAYED;
    }
    if( style & GUI_STYLE_MENU_SEPARATOR ) {
        *menu_flags |= MF_SEPARATOR;
    }
}

HMENU GUICreateSubMenu( gui_window *wnd, int num_items, gui_menu_struct *menu, hint_type type )
{
    HMENU       hmenu;
    int         item;
    HMENU       submenu;
    unsigned    menu_flags;
    unsigned    attr_flags;

    hmenu = _wpi_createpopupmenu();
    if( hmenu == NULLHANDLE ) {
        return( NULLHANDLE );
    }
    for( item = 0; item < num_items; item++ ) {
        GUICreateMenuFlags( menu[item].style, &menu_flags, &attr_flags );
        if( menu[item].child.num_items ) {
            submenu = GUICreateSubMenu( wnd, menu[item].child.num_items, menu[item].child.menu, type );
            _wpi_appendmenu( hmenu, MF_POPUP | menu_flags, attr_flags, menu[item].id, submenu, menu[item].label );
            InsertPopup( wnd, menu[item].id, submenu, type );
        } else {
            _wpi_appendmenu( hmenu, menu_flags, attr_flags, menu[item].id, NULLHANDLE, menu[item].label );
        }
    }
    return( hmenu );
}

/*
 * AppendMenus -- menu items to a HMENU
 */

static bool AppendMenus( gui_window *wnd, HMENU main, int num_items, gui_menu_struct *menu )
{
    int                 item;
    HMENU               hmenu;

    for( item = 0; item < num_items; item++ ) {
        hmenu = GUICreateSubMenu( wnd, menu[item].child.num_items, menu[item].child.menu, MENU_HINT );
        if( hmenu != NULLHANDLE ) {
            _wpi_appendmenu( main, MF_STRING | MF_POPUP, MF_ENABLED, menu[item].id, hmenu, menu[item].label );
            InsertPopup( wnd, menu[item].id, hmenu, MENU_HINT );
        }
    }
    return( true );
}

/*
 * GUICreateMenus -- create a menu resourse
 */

bool GUICreateMenus( gui_window *wnd, int num_items, gui_menu_struct *menu, HMENU *hmenu )
{
    if( hmenu == NULL ) {
        return( false );
    }
    *hmenu = NULLHANDLE;
    if( num_items == 0 ) {
        return( true );
    }
    *hmenu = _wpi_createmenu();
    if( *hmenu == NULLHANDLE ) {
        return( false );
    }
    return( AppendMenus( wnd, *hmenu, num_items, menu ) );
}

/*
 * GUIAddToSystemMenu -- add menus items to the system menus.  Used to give
 *                       menu items to child windows
 */

bool GUIAddToSystemMenu( gui_window *wnd, HWND hwnd, int num_to_add,
                         gui_menu_struct *menu, gui_create_styles style )
{
    HMENU           system;
    int             num_items;

    if( (style & GUI_SYSTEM_MENU) == 0 ) {
        return( true );
    }
    system = _wpi_getsystemmenu( hwnd );
    if( system == NULLHANDLE ) {
        return( false );
    }
    if( GUIMDI && ( _wpi_getparent( hwnd ) != NULLHANDLE ) ) {
        num_items = _wpi_getmenuitemcount( system );
#ifndef __OS2_PM__
        ModifyMenu( system, num_items - 1, MF_STRING | MF_BYPOSITION | MF_GRAYED,
                    SC_NEXTWINDOW, LIT( NexXt ) ); // add \tCtrl+F6" );
        ModifyMenu( system, num_items - 3, MF_STRING | MF_BYPOSITION | MF_ENABLED,
                    SC_CLOSE, LIT( XClose ) ); // add \tctrl+f4" );
#endif
    }
    if( (style & GUI_CLOSEABLE) == 0 ) {
        _wpi_enablemenuitem( system, SC_CLOSE, FALSE, FALSE );
    }
    if( style & GUI_CHANGEABLE_FONT ) {
        if( _wpi_appendmenu( system, MF_SEPARATOR, 0, 0, NULLHANDLE, NULL ) ) {
            _wpi_appendmenu( system, MF_STRING, MF_ENABLED,
                             GUIHint[GUI_MENU_IDX( GUI_CHANGE_FONT )].id, NULLHANDLE,
                             GUIHint[GUI_MENU_IDX( GUI_CHANGE_FONT )].label );
        }
    }
    if( num_to_add > 0 ) {
        if( _wpi_appendmenu( system, MF_SEPARATOR, 0, 0, NULLHANDLE, NULL ) ) {
            return( AppendMenus( wnd, system, num_to_add, menu ) );
        }
    }
    return( true );
}

/*
 * DisplayMenuHintText -- display hint text for a menu item
 */

static void DisplayMenuHintText( gui_window *wnd, WPI_PARAM1 wparam, WPI_PARAM2 lparam, hint_type type )
{
    gui_window          *top_wnd;
    gui_menu_styles     style;
    gui_ctl_id          id;
    HMENU               hpopup;
    int                 menu_closed;
    int                 is_separator;
    int                 is_popup;
    int                 is_hilite;
    int                 is_enabled;
    int                 is_sysmenu;
#ifndef __OS2_PM__
    WORD                flags;
#else
    WPI_MENUSTATE       mstate;
    HMENU               hmenu;
#endif

    lparam = lparam;
    id = GET_WM_MENUSELECT_ITEM( wparam, lparam );
    menu_closed = _wpi_is_close_menuselect( wparam, lparam );

#ifndef __OS2_PM__
    flags = GET_WM_MENUSELECT_FLAGS( wparam, lparam );
    is_sysmenu = ( flags & MF_SYSMENU );
    is_separator = ( flags & MF_SEPARATOR );
    is_hilite = ( flags & MF_HILITE );
    is_popup = ( flags & MF_POPUP );
    is_enabled = ( ( flags & MF_GRAYED ) || ( flags & MF_DISABLED ) );
    hpopup = NULLHANDLE;
    if( is_popup ) {
        hpopup = (HMENU)GET_WM_MENUSELECT_ITEM( wparam, lparam );
    }
#else
    hmenu = (HMENU)lparam;
    if( !menu_closed &&
        WinSendMsg( hmenu, MM_QUERYITEM, MPFROM2SHORT(id, true), MPFROMP(&mstate) ) ) {
        is_sysmenu = ( (mstate.afStyle & MF_SYSMENU) != 0 );
        is_separator = false;
        is_popup = ( (mstate.afStyle & MF_POPUP) != 0 );
        is_hilite = ( (mstate.afAttribute & MF_HILITE) != 0 );
        is_enabled = ( (mstate.afAttribute & MF_DISABLED) != 0 );
        hpopup = NULLHANDLE;
        if( is_popup ) {
            hpopup = mstate.hwndSubMenu;
        }
    } else {
        menu_closed = true;
    }
#endif

    if( !( menu_closed || is_separator || is_hilite ) ) {
        return;
    }

    top_wnd = GUIGetTopGUIWindow( wnd->hwnd );
    if( top_wnd == NULL ) {
        return;
    }


    if( menu_closed ) {
        style = GUI_STYLE_MENU_IGNORE; /* will display blank */
    } else {
        if( is_separator ) {
            style = GUI_STYLE_MENU_SEPARATOR; /* will display blank */
        } else {
            /* must be MF_HILITE */
            if( is_enabled ) {
                style = GUI_STYLE_MENU_GRAYED;
            } else {
                style = GUI_STYLE_MENU_ENABLED;
            }
            if( is_sysmenu ) {
                if( id > GUI_LAST_MENU_ID ) {
                    /* menu item added by GUI */
                    type = GUI_HINT;
                }
            }
            if( is_popup ) {
                if( !GetPopupId( wnd, hpopup, &id ) ) {
                    style = GUI_STYLE_MENU_IGNORE; /* will display blank */
                }
            }
        }
    }

    GUIDisplayHintText( top_wnd, wnd, id, type, style );
}

WPI_MRESULT GUIProcessMenuSelect( gui_window *wnd, HWND hwnd, WPI_MSG msg,
                                  WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    hint_type   type;

    if( GUIHFloatingPopup != NULLHANDLE ) {
        type = FLOAT_HINT;
#ifndef __OS2_PM__
        ShowOwnedPopups( hwnd, true );
#endif
        GUIPopupMenuSelect( wparam, lparam );
    } else {
        type = MENU_HINT;
    }
    DisplayMenuHintText( wnd, wparam, lparam, type );
    return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );
}


WPI_MRESULT GUIProcessInitMenuPopup ( gui_window *wnd, HWND hwnd, WPI_MSG msg,
                                      WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    gui_ctl_id  id;
    HMENU       hmenu;

    hmenu = GET_WM_INITMENU_MENU( wparam, lparam );
    if ( GetPopupId( wnd, hmenu, &id ) ) {
        GUIEVENT( wnd, GUI_INITMENUPOPUP, &id );
    }
    return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );
}


static bool AddMenu( HMENU hmenu, gui_window *wnd, gui_menu_struct *menu,
                     bool insert, gui_ctl_id id_position, bool by_position,
                     hint_type type )
{
    unsigned    menu_flags;
    unsigned    attr_flags;
    gui_ctl_id  newitem;
    HMENU       submenu;

    submenu = NULLHANDLE;
    newitem = menu->id;
    if( hmenu == NULLHANDLE ) {
        return( false );
    }
    if( menu->child.num_items == 0 ) {
        GUICreateMenuFlags( menu->style, &menu_flags, &attr_flags );
    } else {
        submenu = GUICreateSubMenu( wnd, menu->child.num_items, menu->child.menu, type );
        if( submenu == NULLHANDLE ) {
            return( false );
        } else {
            GUICreateMenuFlags( menu->style, &menu_flags, &attr_flags );
            menu_flags |= MF_POPUP;
            InsertPopup( wnd, newitem, submenu, type );
        }
    }
    if( insert ) {
        if( by_position ) {
            if( ( (int)id_position != -1 ) && GUIMDIUpdatedMenu() ) {
                id_position++;
            }
        }
        _wpi_insertmenu( hmenu, id_position, menu_flags, attr_flags,
                         newitem, submenu, menu->label, ( by_position ) ? TRUE : FALSE );
    } else {
        _wpi_appendmenu( hmenu, menu_flags, attr_flags,
                         newitem, submenu, menu->label );
    }
    GUIAppendHintText( wnd, menu, type );
    GUIMDIResetMenus( wnd, wnd->parent, 1, menu );
    if( ( type != FLOAT_HINT ) && ( hmenu == GUIGetHMENU( wnd ) ) ) {
        GUIDrawMenuBar( wnd );
    }
    return( true );
}

bool GUIInsertMenuByIdx( gui_window *wnd, int position, gui_menu_struct *menu, bool floating )
{
    HMENU       hmenu;
    hint_type   type;
    bool        made_root;
    bool        ret;

    if( floating ) {
        type = FLOAT_HINT;
    } else {
        type = MENU_HINT;
    }
    hmenu = GetOrMakeHMENU( wnd, floating, &made_root );
    ret = AddMenu( hmenu, wnd, menu, true, position, true, type );
    if( ret && made_root ) {
        GUISetMenu( wnd, hmenu );
    }
    return( ret );
}

bool GUIInsertMenuByID( gui_window *wnd, gui_ctl_id id, gui_menu_struct *menu )
{
    WPI_MENUSTATE   mstate;
    HMENU           hmenu;
    int             position;
    HMENU           parent;
    bool            made_root;
    bool            ret;

    ret = false;
    hmenu = GetOrMakeHMENU( wnd, false, &made_root );
    if( !_wpi_getmenustate( hmenu, id, &mstate, FALSE ) ) {
        if( GetPopupHMENU( wnd, hmenu, id, &parent, &position, MENU_HINT ) != NULLHANDLE ) {
            ret = AddMenu( parent, wnd, menu, true, position, true, MENU_HINT );
        }
    } else {
        ret = AddMenu( hmenu, wnd, menu, true, id, false, MENU_HINT );
    }
    if( ret && made_root ) {
        GUISetMenu( wnd, hmenu );
    }
    return( ret );
}

bool GUIAppendMenu( gui_window *wnd, gui_menu_struct *menu, bool floating )
{
    HMENU       hmenu;
    hint_type   type;
    bool        ret;
    bool        made_root;

    if( floating ) {
        type = FLOAT_HINT;
    } else {
        type = MENU_HINT;
    }
    hmenu = GetOrMakeHMENU( wnd, floating, &made_root );
    ret = AddMenu( hmenu, wnd, menu, false, 0, false, type );
    if( ret && made_root ) {
        GUISetMenu( wnd, hmenu );
    }
    return( ret );
}

bool GUIAppendMenuByIdx( gui_window *wnd, int position, gui_menu_struct *menu )
{
    HMENU       hmenu;
    HMENU       submenu;
    bool        made_root;
    bool        ret;

    hmenu = GetOrMakeHMENU( wnd, false, &made_root );
    if( hmenu == NULLHANDLE ) {
        return( false );
    }
    if( ( position != -1 ) && GUIMDIUpdatedMenu() ) {
        position++;
    }
    submenu = _wpi_getsubmenu( hmenu, position );
    ret = AddMenu( submenu, wnd, menu, false, 0, false, MENU_HINT );
    if( ret && made_root ) {
        GUISetMenu( wnd, hmenu );
    }
    return( ret );
}

static HMENU ChangeMenuToPopup ( gui_window *wnd, HMENU hmenu, gui_ctl_id id, hint_type type )
{
    HMENU               popup;
    char                name[MAX_STR];
    unsigned            menu_flags;
    unsigned            attr_flags;

    popup = _wpi_createpopupmenu();
    if( popup != NULLHANDLE ) {
        _wpi_getmenutext( hmenu, id, name, MAX_STR - 1, FALSE );
        GetMenuFlags( hmenu, id, false, &menu_flags, &attr_flags );
        _wpi_modifymenu( hmenu, id, menu_flags | MF_POPUP, attr_flags,
                         id, popup, name, FALSE );
        GUIDrawMenuBar( wnd );
        InsertPopup( wnd, id, popup, type );
    }

    return( popup );
}

static bool AddPopup( gui_window *wnd, gui_ctl_id id, gui_menu_struct *menu,
                      bool insert, int position, bool floating )
{
    HMENU               hmenu;
    HMENU               popup;
    hint_type           type;

    hmenu = GetOrMakeHMENU( wnd, floating, NULL );
    if( hmenu == NULLHANDLE ) {
        return( false );
    }

    if( floating ) {
        type = FLOAT_HINT;
    } else {
        type = MENU_HINT;
    }

    popup = GetPopupHMENU( wnd, hmenu, id, NULL, NULL, type );
    if( popup == NULLHANDLE ) {
        popup = ChangeMenuToPopup ( wnd, hmenu, id, type );
    }

    return( AddMenu( popup, wnd, menu, insert, position, true, type ) );
}

bool GUIInsertMenuToPopup( gui_window *wnd, gui_ctl_id id, int position,
                           gui_menu_struct *menu, bool floating )
{
    return( AddPopup( wnd, id, menu, true, position, floating ) );
}

bool GUIAppendMenuToPopup( gui_window *wnd, gui_ctl_id id, gui_menu_struct *menu,
                           bool floating )
{
    return( AddPopup( wnd, id, menu, false, 0, floating ) );
}
