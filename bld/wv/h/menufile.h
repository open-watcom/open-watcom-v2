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
* Description:  Definition of pop-up menu for source file view.
*
****************************************************************************/


MENU_ITEM( MENU_FILE_INSPECT,           XInspect )
MENU_ITEM( MENU_FILE_BREAK,             XBreak )
MENU_ITEM( MENU_FILE_RUN,               Run_to_XCursor )
MENU_ITEM( MENU_FILE_SKIP_TO_CURSOR,    SXkip_to_Cursor )
MENU_ITEM( MENU_FILE_STEP_INTO,         XEnter_Function )
MENU_ITEM( MENU_FILE_WATCH,             XWatch )
MENU_ITEM( MENU_FILE_SEARCH,            XFind  )
MENU_ITEM( MENU_FILE_HOME,              XHome  )
MENU_CASCADE( MENU_FILE_SHOW,           XShow, FileShowMenu )
