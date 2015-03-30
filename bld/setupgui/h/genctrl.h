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
* Description:  forward declarations for creating controls ( guictrl.c file )
*
****************************************************************************/


#include "vhandle.h"

extern void set_dlg_textwindow( gui_control_info *gui_controls,
                          int num_controls, const char *s,
                          gui_ctl_id id,
                          gui_ord x1,                   //top-left x
                          gui_ord y1,                   //top-left y
                          gui_ord x2,                   //bottom-right x
                          int lines,                    //number of lines
                          gui_scroll_styles scroll );   // scroll style )

extern void set_dlg_dynamstring( gui_control_info *gui_controls,
                                 int num_controls, const char *s,
                                 gui_ctl_id id, gui_ord x1, gui_ord y1, gui_ord x2 );


extern void set_dlg_radio( gui_control_info *gui_controls,
                           int num_controls, int num_radio_buttons, const char *s,
                           gui_ctl_id id, gui_ord x1, gui_ord y1, gui_ord x2 );

extern void set_dlg_check( gui_control_info *gui_controls,
                           int num_controls, const char *s,
                           gui_ctl_id id, gui_ord x1, gui_ord y1, gui_ord x2 );

extern void set_dlg_edit( gui_control_info *gui_controls,
                          int num_controls, const char *s,
                          gui_ctl_id id, gui_ord x1, gui_ord y1, gui_ord x2 );

extern gui_ctl_id set_dlg_push_button( vhandle var_handle, const char *text,
                                gui_control_info *gui_controls,
                                int num_controls, int row, int num_buttons,
                                int of, int cols, int bwidth );

extern int find_width( int );
