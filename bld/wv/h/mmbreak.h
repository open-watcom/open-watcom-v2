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


MENU_ITEM( MENU_MAIN_BREAK_TOGGLE,              XToggle )
MENU_ITEM( MENU_MAIN_BREAK_AT_CURSOR,           XAt_Cursor )
MENU_ITEM( MENU_MAIN_BREAK_CREATE_NEW,          XBrkNew_  )
MENU_BAR
MENU_ITEM( MENU_MAIN_BREAK_ON_DLL,              XOn_DLL_Load )
MENU_ITEM( MENU_MAIN_BREAK_ON_DEBUG_MESSAGE,    On_Debug_XMessage )
MENU_BAR
MENU_ITEM( MENU_MAIN_BREAK_VIEW_ALL,            XView_All  )
MENU_ITEM( MENU_MAIN_BREAK_CLEAR_ALL,           XClear_All  )
MENU_ITEM( MENU_MAIN_BREAK_DISABLE_ALL,         XDisable_All  )
MENU_ITEM( MENU_MAIN_BREAK_ENABLE_ALL,          XEnable_All  )
MENU_BAR
MENU_ITEM( MENU_MAIN_BREAK_SAVE_ALL,            XSave_  )
MENU_ITEM( MENU_MAIN_BREAK_RESTORE_ALL,         XRestore_ )
