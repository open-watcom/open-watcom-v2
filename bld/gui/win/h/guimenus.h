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


#ifndef _GUIMENUS_H_
#define _GUIMENUS_H_

extern void         GUIInitGUIMenuHint( void );
extern void         GUISetMenu( gui_window *wnd, HMENU hmenu );
extern bool         GUICreateMenus( gui_window *wnd, int, gui_menu_struct *menu, HMENU *hmenu );
extern bool         GUIAddToSystemMenu( gui_window *wnd, HWND hwnd, int num_menus, gui_menu_struct *menu, gui_create_styles style );
extern char         *GUIAddCharacter( char *old, int index );
extern void         GUICreateMenuFlags( gui_menu_styles style, unsigned *, unsigned * );
extern WPI_MRESULT  GUIProcessMenuSelect( gui_window *wnd, HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
extern WPI_MRESULT  GUIProcessInitMenuPopup( gui_window *wnd, HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
extern void         GUISetGUIHint( gui_window *wnd );
extern bool         GUIEnableSysMenuItem( gui_window *wnd, gui_ctl_id id, bool enable );
extern HMENU        GUIGetHMENU( gui_window *wnd );
extern void         GUIFreePopupList( gui_window *wnd );
extern HMENU        GUICreateSubMenu( gui_window *wnd, int num, gui_menu_struct *menu, hint_type type );
extern void         GUIDeleteFloatingPopups( gui_window *wnd );

#endif // _GUIMENUS_H_
