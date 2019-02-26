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


#ifndef _GUIHINT_H_
#define _GUIHINT_H_

typedef enum hint_type {
    MENU_HINT,
    FLOAT_HINT,
    TOOL_HINT,
    GUI_HINT,
} hint_type;

#define FIRST_HINT  MENU_HINT
#define LAST_HINT   GUI_HINT

typedef struct gui_hint_struct {
    gui_ctl_id  id;
    const char  *hinttext;
} gui_hint_struct;

typedef struct gui_hint_items {
    int                 num_items;
    gui_hint_struct     *hint;
} gui_hint_items;

typedef struct hints_info {
    gui_hint_items      menu;
    gui_hint_items      floating;
    gui_hint_items      tool;
    gui_hint_items      gui;
} hints_info;

extern bool GUIDisplayHintText( gui_window *wnd_with_status, gui_window *wnd,
                                gui_ctl_id id, hint_type type, gui_menu_styles style );
extern bool GUISetHintText( gui_window *wnd, gui_ctl_id id, hint_type type, const char *text );
extern bool GUIHasHintText( gui_window *wnd, gui_ctl_id id, hint_type type );
extern bool GUIHasHintType( gui_window *wnd, hint_type type );
extern void GUIInitHint( gui_window *wnd, const gui_menu_items *menus, hint_type type );
extern void GUIFiniHint( gui_window *wnd, hint_type type );
extern void GUIInitToolbarHint( gui_window *wnd, const gui_toolbar_items *toolinfo );
extern bool GUIAppendHintText( gui_window *wnd, const gui_menu_struct *menu, hint_type type );
extern bool GUIDeleteHintText( gui_window *wnd, gui_ctl_id id, hint_type type );
extern void GUIFreeHint( gui_window *wnd );

#endif
