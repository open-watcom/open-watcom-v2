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
* Description:  Installer color theme configuration.
*
****************************************************************************/


#include <stdlib.h>
#include "gui.h"
#include "setup.h"


#if defined( _UI )

gui_colour_set MainColours[] = {

    /* Fore              Back        */
    { GUI_WHITE,        GUI_BLACK },            /* GUI_MENU_PLAIN    */
    { GUI_BLUE,         GUI_BRIGHT_WHITE },     /* GUI_MENU_STANDOUT */
    { GUI_GREY,         GUI_BLACK },            /* GUI_MENU_GRAYED */
    { GUI_BRIGHT_YELLOW,GUI_BLACK },            /* GUI_MENU_ACTIVE */
    { GUI_BRIGHT_YELLOW,GUI_BRIGHT_WHITE },     /* GUI_MENU_ACTIVE_STANDOUT */
    { GUI_BRIGHT_WHITE, GUI_BLUE },             /* GUI_BACKGROUND */
    { GUI_BRIGHT_WHITE, GUI_BLUE },             /* GUI_MENU_FRAME    */
    { GUI_GREY,         GUI_BLUE },             /* GUI_TITLE_INACTIVE    */

    { GUI_BLUE,         GUI_CYAN },             /* GUI_FRAME_ACTIVE    */
    { GUI_GREY,         GUI_BLACK },            /* GUI_FRAME_INACTIVE    */
    { GUI_BRIGHT_WHITE, GUI_RED },              /* GUI_ICON    */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_MENU_GRAYED_ACTIVE    */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_FRAME_RESIZE    */
    { GUI_BLUE,         GUI_WHITE },            /* GUI_CONTROL_BACKGROUND */
    { GUI_GREEN,        GUI_BLACK },            /* WND_PLAIN    */
    { GUI_BLACK,        GUI_GREEN },            /* WND_TABSTOP   */
    { GUI_BLUE,         GUI_BRIGHT_WHITE },     /* WND_SELECTED */
    { GUI_BLACK,        GUI_RED },              /* WND_HOTSPOT */
    { GUI_GREY,         GUI_BLACK },            /* WND_CENSORED */
    { GUI_BLACK,        GUI_WHITE },            /* WND_STATUS_BAR */
    { GUI_WHITE,        GUI_BLUE },             /* WND_STATUS_TEXT */
    { GUI_WHITE,        GUI_BLUE },             /* WND_STATUS_FRAME */
};

gui_colour_set StatusColours[] = {

    /* Fore              Back        */
    { GUI_WHITE,        GUI_BLACK },            /* GUI_MENU_PLAIN    */
    { GUI_BLUE,         GUI_BRIGHT_WHITE },     /* GUI_MENU_STANDOUT */
    { GUI_GREY,         GUI_BLACK },            /* GUI_MENU_GRAYED */
    { GUI_BRIGHT_YELLOW,GUI_BLACK },            /* GUI_MENU_ACTIVE */
    { GUI_BRIGHT_YELLOW,GUI_BRIGHT_WHITE },     /* GUI_MENU_ACTIVE_STANDOUT */
    { GUI_BRIGHT_WHITE, GUI_BLUE },             /* GUI_BACKGROUND */
    { GUI_BRIGHT_WHITE, GUI_BLUE },             /* GUI_MENU_FRAME    */
    { GUI_GREY,         GUI_BLUE },             /* GUI_TITLE_INACTIVE    */

    { GUI_BLUE,         GUI_CYAN },             /* GUI_FRAME_ACTIVE    */
    { GUI_GREY,         GUI_BLACK },            /* GUI_FRAME_INACTIVE    */
    { GUI_BRIGHT_WHITE, GUI_RED },              /* GUI_ICON    */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_MENU_GRAYED_ACTIVE    */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_FRAME_RESIZE    */
    { GUI_BLUE,         GUI_WHITE },            /* GUI_CONTROL_BACKGROUND */
    { GUI_GREEN,        GUI_BLACK },            /* WND_PLAIN    */
    { GUI_BLACK,        GUI_GREEN },            /* WND_TABSTOP   */
    { GUI_BLUE,         GUI_BRIGHT_WHITE },     /* WND_SELECTED */
    { GUI_BLACK,        GUI_RED },              /* WND_HOTSPOT */
    { GUI_GREY,         GUI_BLACK },            /* WND_CENSORED */
    { GUI_BLACK,        GUI_WHITE },            /* WND_STATUS_BAR */
    { GUI_WHITE,        GUI_BLUE },             /* WND_STATUS_TEXT */
    { GUI_WHITE,        GUI_BLUE },             /* WND_STATUS_FRAME */
};

#else                   // win or winnt or OS/2

gui_colour_set MainColours[] = {

    /* Fore              Back        */
    { GUI_BRIGHT_WHITE, GUI_BLACK },            /* GUI_MENU_PLAIN    */
    { GUI_BLACK,        GUI_BRIGHT_WHITE },     /* GUI_MENU_STANDOUT */
    { GUI_GREY,         GUI_BLACK },            /* GUI_MENU_GRAYED */
    { GUI_BRIGHT_YELLOW,GUI_BLACK },            /* GUI_MENU_ACTIVE */
    { GUI_BRIGHT_YELLOW,GUI_BRIGHT_WHITE },     /* GUI_MENU_ACTIVE_STANDOUT */
    { GUI_BRIGHT_BLUE,  GUI_BRIGHT_BLUE },      /* GUI_BACKGROUND */
    { GUI_BRIGHT_WHITE, GUI_BLUE },             /* GUI_MENU_FRAME    */
    { GUI_GREY,         GUI_BLACK },            /* GUI_TITLE_INACTIVE    */

    { GUI_BLUE,         GUI_CYAN },             /* GUI_FRAME_ACTIVE    */
    { GUI_GREY,         GUI_BLACK },            /* GUI_FRAME_INACTIVE    */
    { GUI_BRIGHT_WHITE, GUI_RED },              /* GUI_ICON    */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_MENU_GRAYED_ACTIVE    */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_FRAME_RESIZE    */
    { GUI_BLACK,        GUIEX_WND_BKGRND },     /* GUI_CONTROL_BACKGROUND */
    { GUI_GREEN,        GUI_BLACK },            /* WND_PLAIN    */
    { GUI_BLACK,        GUI_GREEN },            /* WND_TABSTOP   */
    { GUI_BLACK,        GUI_BRIGHT_WHITE },     /* WND_SELECTED */
    { GUI_BLACK,        GUI_RED },              /* WND_HOTSPOT */
    { GUI_GREY,         GUI_BLACK },            /* WND_CENSORED */
    { GUI_BRIGHT_WHITE, GUI_BLACK },            /* WND_STATUS_BAR */
    { GUI_BLACK,        GUI_BRIGHT_WHITE },     /* WND_STATUS_TEXT */
    { GUI_BLACK,        GUI_BRIGHT_WHITE },     /* WND_STATUS_FRAME */
};

gui_colour_set StatusColours[] = {

    /* Fore              Back        */
    { GUI_BRIGHT_WHITE, GUI_BLACK },            /* GUI_MENU_PLAIN    */
    { GUI_BLACK,        GUI_BRIGHT_WHITE },     /* GUI_MENU_STANDOUT */
    { GUI_GREY,         GUI_BLACK },            /* GUI_MENU_GRAYED */
    { GUI_BRIGHT_YELLOW,GUI_BLACK },            /* GUI_MENU_ACTIVE */
    { GUI_BRIGHT_YELLOW,GUI_BRIGHT_WHITE },     /* GUI_MENU_ACTIVE_STANDOUT */
    { GUI_BLACK,        GUIEX_DLG_BKGRND },     /* GUI_BACKGROUND */
    { GUI_BRIGHT_WHITE, GUI_BLUE },             /* GUI_MENU_FRAME    */
    { GUI_GREY,         GUI_BLACK },            /* GUI_TITLE_INACTIVE    */

    { GUI_BLUE,         GUI_CYAN },             /* GUI_FRAME_ACTIVE    */
    { GUI_GREY,         GUI_BLACK },            /* GUI_FRAME_INACTIVE    */
    { GUI_BRIGHT_WHITE, GUI_RED },              /* GUI_ICON    */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_MENU_GRAYED_ACTIVE    */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_FRAME_RESIZE    */
    { GUI_BLACK,        GUIEX_WND_BKGRND },     /* GUI_CONTROL_BACKGROUND */
    { GUI_GREEN,        GUI_BLACK },            /* WND_PLAIN    */
    { GUI_BLACK,        GUI_GREEN },            /* WND_TABSTOP   */
    { GUI_BLACK,        GUI_BRIGHT_WHITE },     /* WND_SELECTED */
    { GUI_BLACK,        GUI_RED },              /* WND_HOTSPOT */
    { GUI_GREY,         GUI_BLACK },            /* WND_CENSORED */
    { GUIEX_DLG_BKGRND, GUI_BLACK },            /* WND_STATUS_BAR */
    { GUI_BLACK,        GUIEX_DLG_BKGRND },     /* WND_STATUS_TEXT */
    { GUI_BLACK,        GUIEX_DLG_BKGRND }      /* WND_STATUS_FRAME */
};

#endif




