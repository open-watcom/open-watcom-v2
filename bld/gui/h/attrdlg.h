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
* Description:  Colours definition for dialogs
*
****************************************************************************/


/*    enum                            foreground        background */
pick( GUI_DLG_NORMAL,                 GUI_WHITE,        GUI_BLUE )
pick( GUI_DLG_FRAME,                  GUI_BLUE,         GUI_CYAN )
pick( GUI_DLG_SHADOW,                 GUI_BLACK,        GUI_BLUE )
pick( GUI_DLG_SCROLL_ICON,            GUI_WHITE,        GUI_BLUE )
pick( GUI_DLG_SCROLL_BAR,             GUI_WHITE,        GUI_BLUE )
    /* hot spot is inverted on purpose */
pick( GUI_DLG_BUTTON_PLAIN,           GUI_BLUE,         GUI_WHITE )
pick( GUI_DLG_BUTTON_STANDOUT,        GUI_BRIGHT_WHITE, GUI_WHITE )
pick( GUI_DLG_BUTTON_ACTIVE,          GUI_BLUE,         GUI_CYAN )
pick( GUI_DLG_BUTTON_ACTIVE_STANDOUT, GUI_BRIGHT_WHITE, GUI_CYAN )
