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


#ifndef _GUISMOVE_H_
#define _GUISMOVE_H_
typedef enum {
    RESIZE_NONE         = 0x00,
    RESIZE_UP           = 0x01,
    RESIZE_RIGHT        = 0x02,
    RESIZE_DOWN         = 0x04,
    RESIZE_LEFT         = 0x08
} resize_dir;

extern bool GUIDoMoveResize( gui_window *wnd, int row, int col, EVENT ev, gui_point *adjust );
extern bool GUIStartMoveResize( gui_window *wnd, ORD row, ORD col, resize_dir dir );
extern void GUIStartKeyboardMoveResize( gui_window *wnd, bool move );
extern bool GUIDoKeyboardMoveResize( EVENT ev );
extern bool GUIDoMoveResizeCheck( gui_window * wnd, EVENT ev, ORD row, ORD col );

#endif /* _GUISMOVE_H_ */
