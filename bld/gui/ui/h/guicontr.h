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


#ifndef _GUICONTR_H_
#define _GUICONTR_H_

struct gui_control {
    gui_control_class   class;
    unsigned            id;
    gui_control         *sibling;
    gui_window          *parent;
    int                 index;
};

extern EVENT GUIProcessControlEvent( gui_window *wnd, EVENT ev, gui_ord row, gui_ord col );
extern gui_control *GUIGetControl( gui_window *wnd, unsigned id );
extern gui_control *GUIGetControlByIndex( gui_window *wnd, int index );
extern void GUIFreeAllControls( gui_window *wnd );
extern gui_control *GUIInsertControl( gui_window *wnd, gui_control_info *info,
                                      int index );
extern void GUIResizeControls( gui_window *wnd, int row_diff, int col_diff );

#endif // _GUICONTR_H_
