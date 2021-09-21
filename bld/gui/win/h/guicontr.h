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


#ifndef _GUICONTR_H_
#define _GUICONTR_H_

typedef struct {
    const char          *classname;
    const char          *osclassname;
    DWORD               style;
#if defined( __NT__ ) && !defined( _WIN64 )
    DWORD               xstyle;
#endif
} controls_struct;

extern control_item *GUIGetControlByID( gui_window *parent_wnd, gui_ctl_id id );
extern control_item *GUIGetControlByHwnd( gui_window *parent_wnd, HWND control );
extern control_item *GUIControlInsert( gui_window *parent_wnd, gui_control_class control_class,
                              HWND control, gui_control_info *ctl_info,
                              WPI_WNDPROC win_call_back );
extern control_item *GUIControlInsertByHWND( HWND hwnd, gui_window *parent_wnd );
extern void GUIControlDelete( gui_window *wnd, gui_ctl_id id );
extern void GUIControlDeleteAll( gui_window *wnd );
extern void GUIChangeHWnd( HWND, HWND );
extern gui_window *GUIGetParentWnd( HWND );
extern bool GUIAddParent( HWND, gui_window *wnd );
extern WPI_WNDPROC GUIDoSubClass( HWND, gui_control_class );
extern bool GUIInsertCtrlWnd( gui_window *wnd );
extern gui_window *GUIGetCtrlWnd( HWND hwnd );
extern LONG GUISetControlStyle( gui_control_info *ctl_info );
extern bool GUICheckRadioButton( gui_window *wnd, gui_ctl_id id );
#endif // _GUICONTR_H_
