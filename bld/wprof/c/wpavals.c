/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Default profiler color scheme.
*
****************************************************************************/


#include "common.h"
#include "aui.h"
#include "wpaui.h"


gui_colour_set WndColours[] = {

    { GUI_WHITE,        GUI_BLUE },         /* GUI_MENU_PLAIN           */
    { GUI_BR_WHITE,     GUI_BLUE },         /* GUI_MENU_STANDOUT        */
    { GUI_GREY,         GUI_BLUE },         /* GUI_MENU_GRAYED          */
    { GUI_BR_WHITE,     GUI_BLACK },        /* GUI_MENU_ACTIVE          */
    { GUI_BR_YELLOW,    GUI_BLACK },        /* GUI_MENU_ACTIVE_STANDOUT */
    { GUI_BLACK,        GUI_BR_WHITE },     /* GUI_BACKGROUND           */
    { GUI_BR_YELLOW,    GUI_BLUE },         /* GUI_MENU_FRAME           */
    { GUI_GREY,         GUI_BR_WHITE },     /* GUI_TITLE_INACTIVE       */
    { GUI_BLUE,         GUI_CYAN },         /* GUI_FRAME_ACTIVE         */
    { GUI_GREY,         GUI_BR_WHITE },     /* GUI_FRAME_INACTIVE       */
    { GUI_BR_WHITE,     GUI_RED },          /* GUI_ICON                 */
    { GUI_GREY,         GUI_BLACK },        /* GUI_MENU_GRAYED_ACTIVE   */
    { GUI_GREY,         GUI_CYAN },         /* GUI_FRAME_RESIZE         */
    { GUI_BLACK,        GUIEX_WND_BKGRND }, /* GUI_CONTROL_BACKGROUND   */
    { GUI_BLACK,        GUI_BR_WHITE },     /* WPA_PLAIN                */
    { GUI_BR_WHITE,     GUI_BLACK },        /* WPA_TABSTOP              */
    { GUI_BR_WHITE,     GUI_RED },          /* WPA_SELECTED             */
    { GUI_RED,          GUI_BR_WHITE },     /* WPA_STANDOUT             */
    { GUI_RED,          GUI_BR_WHITE },     /* WPA_HOTSPOT              */
    { GUI_BR_RED,       GUI_BLACK },        /* WPA_STANDOUT_TABSTOP     */
    { GUI_BR_WHITE,     GUI_BLUE },         /* WPA_STATUS_LINE          */
    { GUI_RED,          GUI_BR_WHITE },     /* WPA_ABS_BAR              */
    { GUI_GREEN,        GUI_BR_WHITE },     /* WPA_REL_BAR              */
    { GUI_GREY,         GUI_BR_WHITE },     /* WPA_PLAIN_INACTIVE       */
    { GUI_RED,          GUI_BR_WHITE },     /* WPA_OVERVIEW_NAME        */
    { GUI_BR_RED,       GUI_BR_WHITE },     /* WPA_CLEAR_EXTRA          */
};

int                 WndNumColours   = WPA_NUMBER_OF_COLOURS;

wnd_attr            WndPlainAttr    = WPA_PLAIN;
wnd_attr            WndTabStopAttr  = WPA_TABSTOP;
wnd_attr            WndSelectedAttr = WPA_SELECTED;
wnd_attr            WndCursorAttr   = WPA_SELECTED;
wnd_attr            WndStatusColour = WPA_STATUS_LINE;

gui_window_styles   WndStyle        = GUI_PLAIN+GUI_GMOUSE;

