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


#include "dbgdefn.h"
#include "dbgwind.h"
#include "rcdef.h"

extern bool SysGUI();

#include "menudef.h"
static gui_toolbar_struct ToolBar[] = {
{ "",  BITMAP_GO,    MENU_TOOL_GO,              MENU_LIT( HELP_XGo ) },
{ "",  BITMAP_OVER,  MENU_TOOL_TRACE_OVER,      MENU_LIT( HELP_XOver ) },
{ "",  BITMAP_INTO,  MENU_TOOL_STEP_INTO,       MENU_LIT( HELP_XInto ) },
{ "",  BITMAP_RETURN,MENU_TOOL_RETURN_TO_CALLER,MENU_LIT( HELP_XReturn ) },
{ "",  BITMAP_BACK,  MENU_TOOL_UNDO,         MENU_LIT( HELP_XUndo ) },
{ "",  BITMAP_FOR,   MENU_TOOL_REDO,         MENU_LIT( HELP_XRedo ) },
{ "",  BITMAP_UP,    MENU_TOOL_UP_STACK,        MENU_LIT( HELP_UXnwind_Stack ) },
{ "",  BITMAP_DOWN,  MENU_TOOL_DOWN_STACK,      MENU_LIT( HELP_RXewind_Stack ) },
{ "",  BITMAP_HOME,  MENU_TOOL_HOME,            MENU_LIT( HELP_XHome ) },
};

void    InitToolBar()
{
    int         i;

    for( i = 0; i < ArraySize( ToolBar ); ++i ) {
        ToolBar[i].hinttext = WndLoadString( (int)ToolBar[i].hinttext );
    }
}

void    FiniToolBar()
{
    int         i;

    for( i = 0; i < ArraySize( ToolBar ); ++i ) {
        WndFree( ToolBar[i].hinttext );
    }
}

void    WndToolOpen( gui_ord height, bool fixed )
{
    if( SysGUI() ) {
        WndCreateToolBar( height, fixed, ArraySize(ToolBar), &ToolBar );
    }
}

void    WndToolClose()
{
    if( SysGUI() ) {
        WndCloseToolBar();
    }
}
