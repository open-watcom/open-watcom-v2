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


#ifndef _GUIMENU_H_
#define _GUIMENU_H_
extern bool GUICreateMenus( gui_window *wnd,  gui_create_info *dlg_info );
extern void GUIFreeMenus( gui_window *wnd );
extern void GUIFreeMenuItems( MENUITEM *menus );
extern bool GUICreateMenuItems( int num_menus, gui_menu_struct *info, MENUITEM ** );
extern void GUIChangeMenu( MENUITEM *menu, gui_menu_styles style );
extern int GUIGetNumIgnore( gui_menu_struct *info, int num_menus );
extern void GUIFreeVBarMenu( VBARMENU *menu );
extern bool GUIAllocVBarMenu( VBARMENU **pmenu );
extern MENUITEM *GUIAllocMenuItems( int num_menus );
extern bool GUIDeleteToolbarMenuItem( gui_window *wnd, gui_ctl_id id );
extern bool GUIAppendToolbarMenu( gui_window *wnd, gui_menu_struct *menu, bool redraw );
#endif // _GUIMENU_H_
