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


MENU_ITEM( MENU_MAIN_RUN_GO,                    XGo  )
MENU_ITEM( MENU_MAIN_RUN_TO_CURSOR,             Run_to_XCursor  )
MENU_ITEM( MENU_MAIN_RUN_EXECUTE_TO,            EXxecute_to_  )
MENU_ITEM( MENU_MAIN_RUN_TRACE_OVER,            XStep_Over  )
MENU_ITEM( MENU_MAIN_RUN_STEP_INTO,             RunXTrace_Into  )
MENU_ITEM( MENU_MAIN_RUN_TRACE_NEXT,            XNext_Sequential )
MENU_ITEM( MENU_MAIN_RUN_RETURN_TO_CALLER,      XUntil_Return )
MENU_BAR
MENU_ITEM( MENU_MAIN_RUN_SKIP_TO_CURSOR,        SXkip_to_Cursor )
MENU_ITEM( MENU_MAIN_RUN_RESTART,               XRestart  )
MENU_ITEM( MENU_MAIN_DEBUG_STARTUP,             XDebug_Startup  )
MENU_BAR
MENU_ITEM( MENU_MAIN_SAVE_REPLAY,               SaXve_ )
MENU_ITEM( MENU_MAIN_RESTORE_REPLAY,            RestXore_ )
