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


#include "common.h"
#include "aui.h"
#include "wpaui.h"

extern gui_colour_set WndColours[] = {

    { GUI_WHITE,        GUI_BLUE },             /* GUI_MENU_PLAIN    */
    { GUI_BRIGHT_WHITE, GUI_BLUE },             /* GUI_MENU_STANDOUT */
    { GUI_GREY,         GUI_BLUE },             /* GUI_MENU_GRAYED */
    { GUI_BRIGHT_WHITE, GUI_BLACK },            /* GUI_MENU_ACTIVE    */
    { GUI_BRIGHT_YELLOW,GUI_BLACK },            /* GUI_MENU_ACTIVE_STANDOUT */
    { GUI_BLACK,        GUI_BRIGHT_WHITE },     /* GUI_BACKGROUND */
    { GUI_BRIGHT_YELLOW,GUI_BLUE },             /* GUI_MENU_FRAME */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_TITLE_INACTIVE    */
    { GUI_BLUE,         GUI_CYAN },             /* GUI_FRAME_ACTIVE    */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_FRAME_INACTIVE    */
    { GUI_BRIGHT_WHITE, GUI_RED },              /* GUI_ICON    */
    { GUI_GREY,         GUI_BLACK },            /* GUI_MENU_GRAYED_ACTIVE */
    { GUI_GREY,         GUI_CYAN },             /* GUI_FRAME_RESIZE    */
    { GUI_BLACK,        GUI_BRIGHT_WHITE },     /* WPA_PLAIN    */
    { GUI_BRIGHT_WHITE, GUI_BLACK },            /* WPA_TABSTOP */
    { GUI_BRIGHT_WHITE, GUI_RED },              /* WPA_SELECTED */
    { GUI_RED,          GUI_BRIGHT_WHITE },     /* WPA_STANDOUT */
    { GUI_RED,          GUI_BRIGHT_WHITE },     /* WPA_HOTSPOT */
    { GUI_BRIGHT_RED,   GUI_BLACK },            /* WPA_STANDOUT_TABSTOP */
    { GUI_BRIGHT_WHITE, GUI_BLUE },             /* WPA_STATUS_LINE */
    { GUI_RED,          GUI_BRIGHT_WHITE },     /* WPA_ABS_BAR */
    { GUI_GREEN,        GUI_BRIGHT_WHITE },     /* WPA_REL_BAR */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* WPA_PLAIN_INACTIVE    */
    { GUI_RED,          GUI_BRIGHT_WHITE },     /* WPA_OVERVIEW_NAME  */
    { GUI_BRIGHT_RED,   GUI_BRIGHT_WHITE },     /* WPA_CLEAR_EXTRA    */
};

#if defined(__DOS__)
extern gui_colour_set NecColours[] = {

    { GUI_WHITE,        GUI_BLUE },             /* GUI_MENU_PLAIN    */
    { GUI_BRIGHT_WHITE, GUI_BLUE },             /* GUI_MENU_STANDOUT */
    { GUI_GREY,         GUI_BLUE },             /* GUI_MENU_GRAYED */
    { GUI_BRIGHT_WHITE, GUI_BLACK },            /* GUI_MENU_ACTIVE    */
    { GUI_BRIGHT_YELLOW,GUI_BLACK },            /* GUI_MENU_ACTIVE_STANDOUT */
    { GUI_BLACK,        GUI_BRIGHT_WHITE },     /* GUI_BACKGROUND */
    { GUI_BRIGHT_YELLOW,GUI_BLUE },             /* GUI_MENU_FRAME */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_TITLE_INACTIVE    */
    { GUI_BLUE,         GUI_CYAN },             /* GUI_FRAME_ACTIVE    */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_FRAME_INACTIVE    */
    { GUI_BRIGHT_WHITE, GUI_RED },              /* GUI_ICON    */
    { GUI_GREY,         GUI_BLACK },            /* GUI_MENU_GRAYED_ACTIVE */
    { GUI_GREY,         GUI_CYAN },             /* GUI_FRAME_RESIZE    */
    { GUI_WHITE,        GUI_NORMAL },     /* WPA_PLAIN    */
    { GUI_RED,          GUI_NORMAL },            /* WPA_TABSTOP */
    { GUI_WHITE,        GUI_REVERSE },              /* WPA_SELECTED */
    { GUI_GREEN,        GUI_NORMAL },     /* WPA_STANDOUT */
    { GUI_BLUE,         GUI_NORMAL },     /* WPA_HOTSPOT */
    { GUI_GREEN,        GUI_REVERSE },            /* WPA_STANDOUT_TABSTOP */
    { GUI_WHITE,        GUI_NORMAL },             /* WPA_STATUS_LINE */
    { GUI_RED,          GUI_REVERSE },       /* WPA_ABS_BAR */
    { GUI_GREEN,        GUI_REVERSE },     /* WPA_REL_BAR */
    { GUI_WHITE,        GUI_BLACK },     /* WPA_PLAIN_INACTIVE    */
    { GUI_RED,          GUI_NORMAL },     /* WPA_OVERVIEW_NAME  */
    { GUI_RED,          GUI_NORMAL },     /* WPA_CLEAR_EXTRA    */
};
#endif

extern int          WndNumColours = WPA_NUMBER_OF_COLOURS;

extern wnd_attr     WndPlainAttr = WPA_PLAIN;
extern wnd_attr     WndTabStopAttr = WPA_TABSTOP;
extern wnd_attr     WndSelectedAttr = WPA_SELECTED;
extern wnd_attr     WndCursorAttr = WPA_SELECTED;
extern wnd_attr     WndStatusColour = WPA_STATUS_LINE;

wnd_update_list     WndFlags = 0;
gui_window_styles   WndStyle = GUI_PLAIN+GUI_GMOUSE;
