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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/



#include "auipvt.h"

gui_colour_set WndColours[] = {

    { GUI_WHITE,        GUI_BLACK },            /* GUI_MENU_PLAIN    */
    { GUI_BLACK,        GUI_BRIGHT_WHITE },     /* GUI_MENU_STANDOUT */
    { GUI_GREY,         GUI_BLACK },            /* GUI_MENU_GRAYED */
    { GUI_BRIGHT_YELLOW,GUI_BLACK },            /* GUI_MENU_ACTIVE */
    { GUI_BRIGHT_YELLOW,GUI_BRIGHT_WHITE },     /* GUI_MENU_ACTIVE_STANDOUT */
    { GUI_BRIGHT_WHITE, GUI_BLACK },            /* GUI_BACKGROUND */
    { GUI_BLUE,         GUI_BLACK },            /* GUI_TITLE_ACTIVE    */
    { GUI_GREY,         GUI_BLACK },            /* GUI_TITLE_INACTIVE    */
    { GUI_BRIGHT_BLUE,  GUI_BLACK },            /* GUI_FRAME_ACTIVE    */
    { GUI_GREY,         GUI_BLACK },            /* GUI_FRAME_INACTIVE    */
    { GUI_BRIGHT_WHITE, GUI_RED },              /* GUI_ICON    */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_MENU_GREYED_ACTIVE    */
    { GUI_BLACK,        GUI_CYAN },             /* GUI_HOTSPOT */
    { GUI_BRIGHT_WHITE, GUI_CYAN },             /* GUI_HOTSPOT_STANDOUT */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_FRAME_RESIZE    */
};

int WndNumColours = { ArraySize( WndColours ) };

wnd_attr        WndPlainAttr = GUI_MENU_PLAIN;
wnd_attr        WndTabStopAttr = GUI_MENU_STANDOUT;
wnd_attr        WndSelectedAttr = GUI_MENU_ACTIVE;
wnd_attr        WndCursorAttr = GUI_MENU_ACTIVE_STANDOUT;
