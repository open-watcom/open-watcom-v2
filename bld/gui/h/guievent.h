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


/* to be included by the /gui directory */

typedef enum {
    GUI_UPDATE,
    GUI_OPEN,
    GUI_RESIZE,
    GUI_CLOSE,
    GUI_NOT_ACTIVE,
    GUI_NOW_ACTIVE,
    GUI_CURSOR_RIGHT,  /* cursor functions */
    GUI_CURSOR_DOWN,
    GUI_CURSOR_LEFT,
    GUI_CURSOR_UP,
    GUI_SCROLL_UP,     /* scrolling fucntions */
    GUI_SCROLL_DOWN,
    GUI_PAGE_UP,
    GUI_PAGE_DOWN,
    GUI_NO_EVENT,
    GUI_SCROLL_LEFT,
    GUI_SCROLL_RIGHT,
    GUI_PAGE_LEFT,
    GUI_PAGE_RIGHT,
    GUI_BACKWARD_COMPLETE,
    GUI_MOVE_DOWN,
    GUI_CLOSE_WIND,
    GUI_FORWARD_COMPLETE,
    GUI_GROW_WIND,
    GUI_GRAB_TEXT,
    GUI_MOVE_LEFT,
    GUI_NARROW_WIND,
    GUI_PAINT_WIND,
    GUI_QUERY_CONTENTS,
    GUI_MOVE_RIGHT,
    GUI_SHRINK_WIND,
    GUI_MOVE_UP,
    GUI_WIDEN_WIND,
    GUI_ZOOM_WIND,
    GUI_TAB_BACKWARD,
    GUI_TAB_FORWARD,
    GUI_RUB_OUT,
    GUI_ESCAPE,
    GUI_ENTER,
    GUI_HOME,
    GUI_END,
    GUI_CTRL_UP,
    GUI_CTRL_DOWN,
    GUI_CTRL_LEFT,
    GUI_CTRL_RIGHT,
    GUI_INSERT,
    GUI_DELETE,
    GUI_SETFOCUS,
    GUI_COMMAND,          /* got menu command               */
    GUI_PAINT,            /* paint message                  */
    GUI_CLICKED,          /* control or menu clicked        */
    GUI_DCLICKED,         /* control or menu double clicked */
    GUI_LBUTTONDOWN,      /* mouse messages                 */
    GUI_LBUTTONUP,
    GUI_LBUTTONDBLCLK,
    GUI_RBUTTONDOWN,
    GUI_RBUTTONUP,
    GUI_RBUTTONDBLCLK,
    GUI_MOUSEMOVE,
    GUI_KEYDOWN,         /* keystroke messages */
    GUI_KEYUP,
    GUI_DESTROY,         /* destroy */
    GUI_INIT_DIALOG
} GUI_EVENT;
