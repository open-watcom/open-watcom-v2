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


#ifndef _GUIXDLG_H_
#define _GUIXDLG_H_

typedef struct dialog_node {
    gui_window          *wnd;
    a_dialog            *dialog;
    int                 num_controls;
    char                *name;
    bool                colours_set;
    struct dialog_node  *next;
} dialog_node;

extern VFIELD *GUIGetField( gui_window * wnd, unsigned id );
extern a_dialog *GUIGetDialog( gui_window * wnd );
extern void GUIFocusChangeNotify( a_dialog *ui_dialog );
extern bool GUIDoAddControl( gui_control_info *control, gui_window *wnd,
                     VFIELD *field );
extern void GUIDoFreeField( VFIELD *field, a_radio_group **group );
extern void GUIRefreshControl( gui_window *wnd, unsigned id );
extern bool GUIInsertDialog( gui_window *wnd );
extern dialog_node *GUIGetDlgByWnd( gui_window *wnd );
extern gui_control *GUIAddAControl( gui_control_info *info, gui_window *wnd );
extern bool GUIResizeDialog( gui_window *wnd, SAREA *area );
extern void GUIDeleteDialog( a_dialog *dialog );
extern bool GUIDeleteField( gui_window *wnd, unsigned id );
extern a_list *GUIGetList( VFIELD *field );
extern unsigned GUIGetControlId( gui_window *wnd, VFIELD *field );
extern EVENT GUIProcessControlNotify( EVENT ev, a_dialog *ui_dialog, gui_window *wnd );
extern void GUIPushControlEvents();
extern void GUIPopControlEvents();
extern void GUIFreeDialog( a_dialog *dialog, VFIELD *fields, char *title,
                           bool colours_set, bool is_dialog );

extern bool GUIGetDlgRect( gui_window *wnd, SAREA *area );
#endif
