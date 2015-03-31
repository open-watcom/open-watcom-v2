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
* Description:  GUI library dialog routines.
*
****************************************************************************/


#ifndef _GUIXDLG_H_
#define _GUIXDLG_H_

typedef struct dialog_node {
    gui_window          *wnd;
    a_dialog            *ui_dlg_info;
    int                 num_controls;
    char                *name;
    bool                colours_set;
    struct dialog_node  *next;
} dialog_node;

extern VFIELD *GUIGetField( gui_window * wnd, gui_ctl_id id );
extern a_dialog *GUIGetDialog( gui_window * wnd );
extern void GUIFocusChangeNotify( a_dialog *ui_dlg_info );
extern bool GUIDoAddControl( gui_control_info *ctl_info, gui_window *wnd, VFIELD *field );
extern void GUIDoFreeField( VFIELD *field, a_radio_group **group );
extern void GUIRefreshControl( gui_window *wnd, gui_ctl_id id );
extern bool GUIInsertDialog( gui_window *wnd );
extern dialog_node *GUIGetDlgByWnd( gui_window *wnd );
extern gui_control *GUIAddAControl( gui_control_info *ctl_info, gui_window *wnd );
extern bool GUIResizeDialog( gui_window *wnd, SAREA *area );
extern void GUIDeleteDialog( a_dialog *ui_dlg_info );
extern bool GUIDeleteField( gui_window *wnd, gui_ctl_id id );
extern a_list *GUIGetList( VFIELD *field );
extern gui_ctl_id GUIGetControlId( gui_window *wnd, VFIELD *field );
extern EVENT GUIProcessControlNotify( EVENT ev, a_dialog *ui_dlg_info, gui_window *wnd );
extern void GUIPushControlEvents( void );
extern void GUIPopControlEvents( void );
extern void GUIFreeDialog( a_dialog *ui_dlg_info, VFIELD *fields, char *title,
                           bool colours_set, bool is_dialog );

extern bool GUIGetDlgRect( gui_window *wnd, SAREA *area );
#endif
