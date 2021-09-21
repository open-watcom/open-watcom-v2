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


#include "guiwind.h"
#include <string.h>
#include <stdlib.h>
#include "guistyle.h"
#include "guixutil.h"
#include "guicontr.h"
#include "guixwind.h"
#include "guicombo.h"
#include "guimapky.h"
#include "guixdlg.h"
//#include "guixhook.h"
#include "ctl3dcvr.h"
#include "guirdlg.h"
#include "wclbproc.h"
#ifdef __NT__
    #undef _WIN32_IE
    #define _WIN32_IE   0x0400
    #include <commctrl.h>
#endif
#include "oswincls.h"


typedef struct dialog_wnd_node {
    gui_window              *wnd;
    struct dialog_wnd_node  *next;
} dialog_wnd_node;

extern  bool            EditControlHasFocus;

controls_struct GUIControls[GUI_NUM_CONTROL_CLASSES] = {
#if defined( __NT__ ) && !defined( _WIN64 )
    #define pick(enumcls,uitype,classn,classn_os2,style,xstyle_nt) {classn,classn,style,xstyle_nt},
#elif defined( __OS2__ )
    #define pick(enumcls,uitype,classn,classn_os2,style,xstyle_nt) {classn,classn_os2,style},
#else
    #define pick(enumcls,uitype,classn,classn_os2,style,xstyle_nt) {classn,classn,style},
#endif
    #include "_guicont.h"
    #undef pick
};

static dialog_wnd_node  *DialogHead = NULL;

WPI_MRESULT CALLBACK GUIEditFunc( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );
WPI_MRESULT CALLBACK GUIGroupBoxFunc( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );

bool GUIInsertCtrlWnd( gui_window *wnd )
{
    dialog_wnd_node     *dlg_wnd_node;

    dlg_wnd_node = (dialog_wnd_node *)GUIMemAlloc( sizeof( dialog_wnd_node ) );
    if( dlg_wnd_node != NULL ) {
        dlg_wnd_node->wnd = wnd;
        dlg_wnd_node->next = DialogHead;
        DialogHead = dlg_wnd_node;
        return( true );
    }
    return( false );
}

gui_window *GUIGetCtrlWnd( HWND hwnd )
{
    dialog_wnd_node    **owner;
    dialog_wnd_node    *curr;

    for( owner = &DialogHead; (curr = *owner) != NULL; owner = &curr->next ) {
        if( curr->wnd->hwnd == hwnd ) {
            return( curr->wnd );
        }
    }
    return( NULL );
}

static void GUIDeleteCtrlWnd( gui_window *wnd )
{
    dialog_wnd_node    **owner;
    dialog_wnd_node    **prev_owner;
    dialog_wnd_node    *curr;

    prev_owner = NULL;
    for( owner = &DialogHead; (curr = *owner) != NULL; owner = &curr->next ) {
        if( curr->wnd == wnd ) {
            if( prev_owner == NULL ) {
                DialogHead = curr->next;
            } else {
                (*prev_owner)->next = curr->next;
            }
            GUIMemFree( curr );
            break;
        }
        prev_owner = owner;
    }
}

/*
 * GUIGetControlByID - get the first control_item* in the given HWND with the
 *                     given id
 */

control_item *GUIGetControlByID( gui_window *parent_wnd, gui_ctl_id id )
{
    control_item * curr;

    for( curr = parent_wnd->controls; curr != NULL; curr = curr->next ) {
        if( curr->id == id ) {
            return( curr );
        }
    }
    return( NULL );
}

/*
 * GUIGetControlByHWND - get the first control_item * in the given HWND with the
 *                       given HWND
 */

control_item * GUIGetControlByHwnd( gui_window *parent_wnd, HWND control )
{
    control_item        *curr;

    for( curr = parent_wnd->controls; curr != NULL; curr = curr->next ) {
        if( curr->hwnd == control ) {
            return( curr );
        }
    }
    return( NULL );
}

/*
 * GUIControlInsert - insert a control_item associated with the given
 *                    gui_window for the given class
 */

control_item *GUIControlInsert( gui_window *parent_wnd, gui_control_class control_class,
                                HWND hwnd, gui_control_info *ctl_info,
                                WPI_WNDPROC win_call_back )
{
    control_item        *item;

    item = (control_item *)GUIMemAlloc( sizeof( control_item ) );
    if( item != NULL ) {
        item->control_class = control_class;
        item->text = ctl_info->text;
        item->style = ctl_info->style;
        item->checked = ctl_info->style & GUI_STYLE_CONTROL_CHECKED;
        item->id = ctl_info->id;
        item->next = NULL;
        item->hwnd = hwnd;
        item->win_call_back = win_call_back;
        item->next = parent_wnd->controls;
        parent_wnd->controls = item;
    }
    return( item );
}

control_item *GUIControlInsertByHWND( HWND hwnd, gui_window *parent_wnd )
{
    control_item        *item;
    HWND                phwnd;
    gui_control_class   control_class;

    phwnd = _wpi_getparent( hwnd );
    if( ( parent_wnd == NULL ) || ( phwnd != parent_wnd->hwnd ) ) {
        return( NULL );
    }
    item = (control_item *)GUIMemAlloc( sizeof( control_item ) );
    if( item != NULL ) {
        memset( item, 0, sizeof( control_item ) );
        control_class = GUIGetControlClassFromHWND( hwnd );
        if( control_class == GUI_BAD_CLASS ) {
            GUIMemFree( item );
            return( NULL );
        }
        item->control_class = control_class;
        item->style = GUIGetControlStylesFromHWND( hwnd, control_class );
        item->id = _wpi_getdlgctrlid( hwnd );
        item->next = NULL;
        item->hwnd = hwnd;
        item->next = parent_wnd->controls;
        parent_wnd->controls = item;
    }
    return( item );
}

/*
 * GUIControlDelete - delete the control_item with the given it which is
 *                 associated with the given window handle
 */

void GUIControlDelete( gui_window *wnd, gui_ctl_id id )
{
    control_item        *curr;
    control_item        *prev;
    control_item        *next;

    prev = NULL;
    for( curr = wnd->controls; curr != NULL; curr = next ) {
        next = curr->next;
        if( curr->id == id ) {
            if( prev == NULL ) {
                wnd->controls = next;
            } else {
                prev->next = next;
            }
            GUIMemFree( curr );
            break;
        }
        prev = curr;
    }
    if( wnd->controls == NULL ) {
        GUIDeleteCtrlWnd( wnd );
    }
}

/*
 * GUIControlDeleteAll -- delete all control_item structs that are associated
 *                        with the given window
 */

void GUIControlDeleteAll( gui_window *wnd )
{
    control_item        *curr;
    control_item        *next;

    for( curr = wnd->controls; curr != NULL; curr = next ) {
        next = curr->next;
        GUIMemFree( curr );
    }
    wnd->controls = NULL;
    GUIDeleteCtrlWnd( wnd );
}

/*
 * GUIEditFunc - callback function for all edit windows
 */

WPI_MRESULT CALLBACK GUIEditFunc( HWND hwnd, WPI_MSG message, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    control_item        *info;
    WPI_WNDPROC         win_call_back;
    HWND                parent;
    HWND                grand_parent;
    gui_window          *wnd;
    gui_key_control     key_control;

    parent = _wpi_getparent( hwnd );
    wnd = GUIGetCtrlWnd( parent );
    info = NULL;
    if( wnd == NULL ) { /* needed for GUI_EDIT_COMBOBOX */
        grand_parent = _wpi_getframe( _wpi_getparent( parent ) );
        wnd = GUIGetCtrlWnd( grand_parent );
        if( wnd != NULL ) {
            info = GUIGetControlByHwnd( wnd, parent );
        }
        parent = grand_parent;
    } else {
        info = GUIGetControlByHwnd( wnd, hwnd );
    }
    if( info == NULL ) {
        return( 0L );
    }
    win_call_back = info->win_call_back;
    switch( message ) {
#ifndef __OS2_PM__
    case WM_SETFOCUS:
        EditControlHasFocus = true;
        break;
    case WM_KILLFOCUS:
        EditControlHasFocus = false;
        break;
    case WM_MOUSEACTIVATE:
        return( true );
        break;
    case WM_KEYDOWN:
        if( EditControlHasFocus ) {
            if( GUIWindowsMapKey( wparam, lparam, &key_control.key_state.key ) ) {
                switch( key_control.key_state.key ) {
                case GUI_KEY_UP:
                case GUI_KEY_DOWN:
                    key_control.id = info->id;
                    GUIGetKeyState( &key_control.key_state.state );
                    GUIEVENT( wnd, GUI_KEY_CONTROL, &key_control );
                }
            }
        //} else {
            //if( GUICurrWnd != NULL ) {
                //GUISendMessage( GUICurrWnd->hwnd, message, wparam, lparam );
            //}
        }
        break;
#endif
    case WM_CHAR:
        if( EditControlHasFocus ) {
#ifdef __OS2_PM__
            if( !IS_KEY_UP( wparam ) && GUIWindowsMapKey( wparam, lparam, &key_control.key_state.key ) ) {
                switch( key_control.key_state.key ) {
                //case GUI_KEY_ENTER:
                case GUI_KEY_UP:
                case GUI_KEY_DOWN:
                    key_control.id = info->id;
                    GUIGetKeyState( &key_control.key_state.state );
                    if( GUIEVENT( wnd, GUI_KEY_CONTROL, &key_control ) ) {
                        return( 0L );
                    }
                }
            }
#else
            if( GUIWindowsMapKey( wparam, lparam, &key_control.key_state.key ) ) {
                GUIGetKeyState( &key_control.key_state.state );
                if( key_control.key_state.key == GUI_KEY_ENTER ) {
                    key_control.id = info->id;
                    if( GUIEVENT( wnd, GUI_KEY_CONTROL, &key_control ) ) {
                        return( 0L );
                    }
                }
            }
#endif
        //} else {
            //if( GUICurrWnd != NULL ) {
                //GUISendMessage( GUICurrWnd->hwnd, message, wparam, lparam );
            //}
        }
        break;
    case WM_RBUTTONUP:
        break;
    case WM_NCDESTROY:
        GUIControlDelete( wnd, info->id );
        _wpi_subclasswindow( hwnd, win_call_back );
        break;
    }
    return( (WPI_MRESULT)_wpi_callwindowproc( win_call_back, hwnd, message, wparam, lparam ) );
}

/*
 * GUIGroupBoxFunc - callback function for all GroupBox windows
 */

WPI_MRESULT CALLBACK GUIGroupBoxFunc( HWND hwnd, WPI_MSG message, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    control_item        *info;
    WPI_WNDPROC         win_call_back;
    WPI_PRES            hdc;
    WPI_RECT            wpi_rect;
    HWND                parent;
    gui_window          *wnd;

    parent = _wpi_getparent( hwnd );
    wnd = GUIGetCtrlWnd( parent );
    info = NULL;
    if( wnd != NULL ) {
        info = GUIGetControlByHwnd( wnd, hwnd );
    }
    if( info == NULL ) {
        return( 0L );
    }
    win_call_back = info->win_call_back;
    switch( message ) {
    case WM_ERASEBKGND:
        hdc = _wpi_getpres( hwnd );
        _wpi_getupdaterect( hwnd, &wpi_rect );
        _wpi_fillrect( hdc, &wpi_rect, (WPI_COLOUR)0, wnd->bk_brush );
        _wpi_releasepres( hwnd, hdc );
        break;
    }
    return( (WPI_MRESULT)_wpi_callwindowproc( win_call_back, hwnd, message, wparam, lparam ) );
}

WPI_WNDPROC GUIDoSubClass( HWND hwnd, gui_control_class control_class )
{
    WPI_WNDPROC old;
#ifdef __OS2_PM__
    WPI_WNDPROC new;
#endif

    //CvrCtl3dSubclassCtl( hwnd );

    switch( control_class ) {
    case GUI_EDIT_COMBOBOX:
        return( GUISubClassEditCombobox( hwnd ) );
    case GUI_EDIT:
    case GUI_EDIT_MLE:
#ifdef __OS2_PM__
        new = (WPI_WNDPROC)_wpi_makeprocinstance( (WPI_PROC)GUIEditFunc, GUIMainHInst );
        old = _wpi_subclasswindow( hwnd, new );
#else
        old = (WPI_WNDPROC)GET_WNDPROC( hwnd );
        SET_WNDPROC( hwnd, (LONG_PTR)MakeProcInstance_WND( GUIEditFunc, GUIMainHInst ) );
#endif
        return( old );
    case GUI_GROUPBOX:
#ifdef __OS2_PM__
        new = (WPI_WNDPROC)_wpi_makeprocinstance( (WPI_PROC)GUIGroupBoxFunc, GUIMainHInst );
        old = _wpi_subclasswindow( hwnd, new );
#else
        old = (WPI_WNDPROC)GET_WNDPROC( hwnd );
        SET_WNDPROC( hwnd, (LONG_PTR)MakeProcInstance_WND( GUIGroupBoxFunc, GUIMainHInst ) );
#endif
        return( old );
    default:
        return( NULL );
    }
}

LONG GUISetControlStyle( gui_control_info *ctl_info )
{
    LONG        ret_style;

    ret_style = GUIControls[ctl_info->control_class].style;

    /* The GUI library has a group marked by GUI_STYLE_CONTROL_GROUP on the first and
     * last group item.  Windows has the WS_GROUP style mark the start
     * of each group.  So, I mark everything as WS_GROUP, excluding
     * the second, third, etc. member of a GUI_STYLE_CONTROL_GROUP.  That way,
     * everything is in a group of one, except GUI_STYLE_CONTROL_GROUPs, which are
     * grouped properly.
     */
    if( !( ctl_info->style & GUI_INIT_INVISIBLE ) ) {
        ret_style |= WS_VISIBLE;
    }
#ifndef __OS2_PM__
    if( ctl_info->scroll_style & GUI_VSCROLL ) {
        ret_style |= WS_VSCROLL;
    }
    if( ctl_info->scroll_style & GUI_HSCROLL ) {
        ret_style |= WS_HSCROLL;
    }
#endif
    if( ctl_info->style & GUI_STYLE_CONTROL_EDIT_INVISIBLE ) {
#ifndef __OS2_PM__
        ret_style |= ES_PASSWORD;
#else
        ret_style |= ES_UNREADABLE;
#endif
    }
    if( ctl_info->style & GUI_STYLE_CONTROL_LEFTNOWORDWRAP ) {
        ret_style |= SS_LEFTNOWORDWRAP;
    }
    if( ctl_info->style & GUI_STYLE_CONTROL_CENTRE ) {
        ret_style |= SS_CENTER;
    }
    if( ctl_info->style & GUI_STYLE_CONTROL_NOPREFIX ) {
#ifndef __OS2_PM__
        ret_style |= SS_NOPREFIX;
#else
        ret_style &= ~DT_MNEMONIC;
#endif
    }
    if( ctl_info->style & GUI_STYLE_CONTROL_MULTILINE ) {
        ret_style |= ES_MULTILINE;
    }
    if( ctl_info->style & GUI_STYLE_CONTROL_WANTRETURN ) {
        ret_style |= ES_WANTRETURN;
    }
    if( ctl_info->style & GUI_STYLE_CONTROL_3STATE ) {
        ret_style |= BS_3STATE;
    }
    if( ctl_info->style & GUI_STYLE_CONTROL_TAB_GROUP ) {
        ret_style |= WS_TABSTOP;
    }

    switch( ctl_info->control_class ) {
    case GUI_LISTBOX:
        if( ctl_info->style & GUI_STYLE_CONTROL_NOINTEGRALHEIGHT ) {
            ret_style |= LBS_NOINTEGRALHEIGHT;
        }
        if( ctl_info->style & GUI_STYLE_CONTROL_SORTED ) {
            ret_style |= LBS_SORT;
        }
#ifndef __OS2_PM__
        if( ctl_info->style & GUI_STYLE_CONTROL_WANTKEYINPUT ) {
            ret_style |= LBS_WANTKEYBOARDINPUT;
        }
#endif
#ifdef __OS2_PM__
        if( ctl_info->scroll_style & GUI_HSCROLL ) {
            ret_style |= LS_HORZSCROLL;
        }
#endif
        break;
    case GUI_COMBOBOX:
    case GUI_EDIT_COMBOBOX:
        if( ctl_info->style & GUI_STYLE_CONTROL_NOINTEGRALHEIGHT ) {
            ret_style |= CBS_NOINTEGRALHEIGHT;
        }
        if( ctl_info->style & GUI_STYLE_CONTROL_SORTED ) {
            ret_style |= CBS_SORT;
        }
        break;
#ifdef __OS2_PM__
    case GUI_EDIT:
        if( ctl_info->style & GUI_STYLE_CONTROL_READONLY ) {
            ret_style |= ES_READONLY;
        }
        break;
    case GUI_EDIT_MLE:
        if( ctl_info->style & GUI_STYLE_CONTROL_READONLY ) {
            ret_style |= MLS_READONLY;
        }
        break;
#else
    case GUI_EDIT:
    case GUI_EDIT_MLE:
        if( ctl_info->style & GUI_STYLE_CONTROL_READONLY ) {
            ret_style |= ES_READONLY;
        }
        if( ctl_info->style & GUI_STYLE_CONTROL_BORDER ) {
            ret_style |= WS_BORDER;
        }
        break;
#endif
    }

    return( ret_style );
}

static HWND CreateControl( gui_control_info *ctl_info, gui_window *parent_wnd, const guix_coord *scr_pos, const guix_coord *scr_size )
{
    DWORD       style;
    HWND        hwnd;
    char        *new_text;
#ifdef __OS2_PM__
    ENTRYFDATA  edata;
    void        *pctldata;
#elif defined( __WINDOWS__ )
#else
    DWORD       xstyle;
#endif

    new_text = _wpi_menutext2pm( ctl_info->text );

    style = GUISetControlStyle( ctl_info );
    if( ctl_info->text != NULL ) {
        switch( ctl_info->control_class ) {
        case GUI_LISTBOX:
            style |= WS_CAPTION;
            break;
        }
    }

#ifdef __OS2_PM__
    pctldata = NULL;
    if( ctl_info->control_class == GUI_EDIT ) {
        edata.cb = sizeof( edata );
        edata.cchEditLimit = 2048;
        edata.ichMinSel = 0;
        edata.ichMaxSel = 0;
        pctldata = &edata;
    }
#endif

    style &= ~WS_VISIBLE;  /* create invisible -- if GUIAlloc fails, window
                            * will never show
                            */
    if( parent_wnd != NULL ) {
        style |= WS_CHILD;
    }

#if defined( __OS2_PM__ )
    _wpi_createanywindow( (PSZ)GUIControls[ctl_info->control_class].classname,
                  new_text, style, scr_pos->x, scr_pos->y, scr_size->x, scr_size->y,
                  parent_wnd->hwnd, (HMENU)ctl_info->id, GUIMainHInst,
                  pctldata, &hwnd, ctl_info->id, &hwnd );
#elif defined( __WINDOWS__ )
    hwnd = CreateWindow( GUIControls[ctl_info->control_class].classname,
                  new_text, style, scr_pos->x, scr_pos->y, scr_size->x, scr_size->y,
                  parent_wnd->hwnd, (HMENU)ctl_info->id, GUIMainHInst, NULL );
#else
    // We do this crud to get 3d edges on edit controls, listboxes, and
    // comboboxes -rnk 07/07/95
    xstyle = 0L;
  #if !defined( _WIN64 )
    /* In W95 and later we don't want this crud any more... RR 2003.12.8 */
    if( LOBYTE(LOWORD(GetVersion())) < 4) {
        xstyle = GUIControls[ctl_info->control_class].xstyle;
    }
  #endif

    hwnd = CreateWindowEx( xstyle, GUIControls[ctl_info->control_class].classname,
        new_text, style, scr_pos->x, scr_pos->y, scr_size->x, scr_size->y, parent_wnd->hwnd,
        (HMENU)ctl_info->id, GUIMainHInst, NULL );

    /* From here to #else, new by RR 2003.12.05 */

    if( hwnd != NULL ) {
        /* Set the standard font for the new control                */
        /* Use system supplied font, so we do not need to worry     */
        /* about cleaning it up later (no DeleteObject() necessary) */
        HFONT setFont;

  #if !defined( _WIN64 )
        if( LOBYTE(LOWORD(GetVersion())) >= 4 ) {
  #endif
            /* New shell active, Win95 or later */
            setFont = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
  #if !defined( _WIN64 )
        } else {
            /* MSDN on net tells SYSTEM_FONT should be Tahoma on W2K    */
            /* and later. Does not appear to be correct (tested XP SP1) */
            setFont = (HFONT)GetStockObject( SYSTEM_FONT );
        }
  #endif

        SendMessage( hwnd, WM_SETFONT, (WPARAM)setFont, (LPARAM)0 );
    }
#endif

    if( new_text != NULL ) {
        _wpi_freemenutext( new_text );
    }

    return( hwnd );
}

/*
 * GUIAddControl - add the given control to the parent window
 */

bool GUIAPI GUIAddControl( gui_control_info *ctl_info, gui_colour_set *plain, gui_colour_set *standout )
{
    guix_coord          scr_pos;
    guix_coord          scr_size;
    HWND                hwnd;
    control_item        *item;
    gui_window          *parent_wnd;

    plain = plain;
    standout = standout;
    parent_wnd = ctl_info->parent;
    GUICalcLocation( &ctl_info->rect, &scr_pos, &scr_size, parent_wnd->hwnd );
    hwnd = CreateControl( ctl_info, parent_wnd, &scr_pos, &scr_size );
    if( hwnd == NULLHANDLE ) {
        return( false );
    }
    item = GUIControlInsert( parent_wnd, ctl_info->control_class, hwnd, ctl_info, NULL );
    if( item == NULL ) {
        return( false );
    }
    if( GUIGetCtrlWnd( parent_wnd->hwnd ) == NULL ) {
        if( !GUIInsertCtrlWnd( parent_wnd ) ) {
            GUIControlDelete( parent_wnd, ctl_info->id );
            return( false );
        }
    }
    GUIInitControl( item, parent_wnd, NULL );
#ifndef __OS2_PM__
    if( ctl_info->control_class == GUI_DEFPUSH_BUTTON ) {
        GUISendMessage( parent_wnd->hwnd, DM_SETDEFID, ctl_info->id, 0 );
    }
#endif
    if( (ctl_info->style & GUI_STYLE_CONTROL_INIT_INVISIBLE) == 0 ) {
        _wpi_showwindow( hwnd, SW_SHOW );
    }
    return( true );
}

void GUIAPI GUIEnumControls( gui_window *wnd, CONTRENUMCALLBACK *func, void *param )
{
    control_item        *curr;

    for( curr = wnd->controls; curr != NULL; curr = curr->next ) {
        (*func)( wnd, curr->id, param );
    }
}

bool GUICheckRadioButton( gui_window *wnd, gui_ctl_id id )
{
    control_item        *curr;
    bool                in_group;
    bool                found_id;
    gui_ctl_id          first;
    gui_ctl_id          last;
    bool                done;

    first = id;
    last = id;
    in_group = false;
    found_id = false;
    done = false;
    for( curr = wnd->controls; curr != NULL; curr = curr->next ) {
        if( curr->id == id ) {
            found_id = true;
        }
        if( curr->style & GUI_STYLE_CONTROL_GROUP ) {
            in_group = !in_group;
            if( in_group ) {
                first = curr->id;
            } else {
                last = curr->id;
                if( found_id ) {
                    in_group = false;
                    for( curr = wnd->controls; curr != NULL; curr = curr->next ) {
                        if( curr->id == first ) {
                            in_group = true;
                        }
                        if( in_group ) {
                            if( curr->id == id ) {
                                GUISendMessage( curr->hwnd, BM_SETCHECK, (WPI_PARAM1)true, (WPI_PARAM2)0 );
                            } else {
                                GUISendMessage( curr->hwnd, BM_SETCHECK, (WPI_PARAM1)false, (WPI_PARAM2)0 );
                            }
                        }
                        if( curr->id == last ) {
                            return( true );
                        }
                    }
                    break;
                }
            }
        }
    }
    return( false );
}

bool GUIAPI GUIDeleteControl( gui_window *wnd, gui_ctl_id id )
{
    control_item        *control;

    control = GUIGetControlByID( wnd, id );
    if( control != NULL ) {
        _wpi_destroywindow( control->hwnd );
        GUIControlDelete( wnd, id );
        return( true );
    }
    return( false );
}

bool GUIAPI GUILimitEditText( gui_window *wnd, gui_ctl_id id, int len )
{
    control_item        *control;
    HWND                hwnd;

    control = GUIGetControlByID( wnd, id );
    if( control != NULL ) {
        hwnd = _wpi_getdlgitem( wnd->hwnd, id );
        if( control->control_class == GUI_EDIT ) {
            _wpi_sendmessage( hwnd, EM_LIMITTEXT, len, 0 );
#ifdef __OS2_PM__
        } else if( len > 0 && control->control_class == GUI_EDIT_MLE ) {
            _wpi_sendmessage( hwnd, MLM_SETTEXTLIMIT, len, 0 );
#endif
        }
        return( true );
    }

    return( false );
}

static void ShowControl( gui_window *wnd, gui_ctl_id id, int show_flag )
{
    control_item        *control;

    control = GUIGetControlByID( wnd, id );
    if( control != NULL ) {
        _wpi_showwindow( control->hwnd, show_flag );
    }
}

void GUIAPI GUIHideControl( gui_window *wnd, gui_ctl_id id )
{
    ShowControl( wnd, id, SW_HIDE );
}

void GUIAPI GUIShowControl( gui_window *wnd, gui_ctl_id id )
{
    ShowControl( wnd, id, SW_SHOW );
}

bool GUIAPI GUIIsControlVisible( gui_window *wnd, gui_ctl_id id )
{
    control_item        *control;

    control = GUIGetControlByID( wnd, id );
    if( control != NULL ) {
        if( _wpi_iswindowvisible( control->hwnd ) ) {
            return( true );
        }
    }
    return( false );
}
