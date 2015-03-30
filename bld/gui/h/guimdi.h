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


#ifndef _GUIMDI_H_
#define _GUIMDI_H_

extern gui_window   *GUIMDIGetWindow( gui_ctl_id id );
extern void         GUIMDIMoreWindows( void );
extern int          GUIGetNumChildWindows( void );
extern int          GUIGetNumIconicWindows( void );
extern gui_window   *GUIGetRoot( void );
extern void         EnableMDIActions( bool enable );
extern void         AddMDIActions( bool has_items, gui_window *wnd );
extern void         MDIDeleteMenu( gui_ctl_id id );
extern void         MDIResetMenus( gui_window *wnd, gui_window *parent, int num_menus, gui_menu_struct *menu );
extern void         InitMDI( gui_window *wnd, gui_create_info *dlg_info );
extern void         ChangeTitle( gui_window *wnd );
extern void         BroughtToFront( gui_window *wnd );
extern gui_window   *FindNextMDIMenuWindowNotInArray( gui_window *wnd, gui_window *avoid );
extern void         MDIDelete( gui_window *wnd );

#endif // _GUIMDI_H_
