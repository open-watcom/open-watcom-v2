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


#ifndef _GUIHOOK_H_
#define _GUIHOOK_H_
extern void GUISetHotSpotCleanup( void (*func)(void) );
extern void GUICleanupHotSpots( void );
extern void GUIXChangeTitle( gui_window *wnd );
extern void GUISetXChangeTitle( void (*func)(gui_window*) );
extern bool GUIXInitMDI( gui_window *wnd );
extern void GUISetXInitMDI( bool (*func)(gui_window*) );
extern void GUISetInitMDI( void (*func)(gui_window*, gui_create_info *) );
extern void GUIInitMDI( gui_window *wnd, gui_create_info * );
extern void GUISetMDIChangeTitle( void (*func)(gui_window *) );
extern void GUIChangeMDITitle( gui_window *wnd );
extern void GUISetMDIBroughtToFront( void (*func)(gui_window *) );
extern void GUIMDIBroughtToFront( gui_window *wnd );
extern void GUISetMDIDelete( void (*func)(gui_window *) );
extern void GUIMDIDelete( gui_window *wnd );
extern void GUISetMDIResetMenus( void (*func)( gui_window *, gui_window *, gui_ctl_idx, gui_menu_struct * ) );
extern void GUIMDIResetMenus( gui_window *wnd, gui_window *, gui_ctl_idx num_menus, gui_menu_struct *menu );
extern void GUISetMDIDeleteMenuItem( void (*func)(gui_ctl_id id) );
extern void GUIMDIDeleteMenuItem( gui_ctl_id id );
extern void GUISetEnableMDIActions( void (*func)(bool enable) );
extern void GUIEnableMDIActions( bool enable );
extern void GUISetAddMDIActions( void (*func)(bool has_items, gui_window *wnd) );
extern void GUIAddMDIActions( bool has_items, gui_window *wnd );
#endif // _GUIHOOK_H_
