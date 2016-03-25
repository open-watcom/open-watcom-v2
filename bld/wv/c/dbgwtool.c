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
* Description:  Debugger toolbar.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgwind.h"
#include "rcdef.h"
#include "dbgwtool.h"
#include "dbginit.h"


#include "menudef.h"
static gui_toolbar_struct ToolBar[] = {
    { "", BITMAP_GO, MENU_TOOL_GO, MENU_LIT( HELP_XGo ), MENU_LIT( TIP_XGo ) },
    { "", BITMAP_OVER, MENU_TOOL_TRACE_OVER, MENU_LIT( HELP_XOver ),
      MENU_LIT( TIP_XOver ) },
    { "", BITMAP_INTO, MENU_TOOL_STEP_INTO, MENU_LIT( HELP_XInto ),
      MENU_LIT( TIP_XInto ) },
    { "", BITMAP_RETURN, MENU_TOOL_RETURN_TO_CALLER, MENU_LIT( HELP_XReturn ),
      MENU_LIT( TIP_XReturn ) },
    { "", BITMAP_BACK, MENU_TOOL_UNDO, MENU_LIT( HELP_XUndo ), MENU_LIT( TIP_XUndo ) },
    { "", BITMAP_FOR, MENU_TOOL_REDO, MENU_LIT( HELP_XRedo ), MENU_LIT( TIP_XRedo ) },
    { "", BITMAP_UP, MENU_TOOL_UP_STACK, MENU_LIT( HELP_UXnwind_Stack ),
      MENU_LIT( TIP_UXnwind_Stack ) },
    { "", BITMAP_DOWN, MENU_TOOL_DOWN_STACK, MENU_LIT( HELP_RXewind_Stack ),
      MENU_LIT( TIP_RXewind_Stack ) },
    { "",  BITMAP_HOME, MENU_TOOL_HOME, MENU_LIT( HELP_XHome ), MENU_LIT( TIP_XHome ) }
};

void    InitToolBar( void )
{
    int         i;

    for( i = 0; i < ArraySize( ToolBar ); ++i ) {
        ToolBar[i].hinttext = WndLoadString( (gui_res_id)(pointer_int)ToolBar[i].hinttext );
        ToolBar[i].tip = WndLoadString( (gui_res_id)(pointer_int)ToolBar[i].tip );
    }
}

void    FiniToolBar( void )
{
    int         i;

    for( i = 0; i < ArraySize( ToolBar ); ++i ) {
        WndFree( (void *)ToolBar[i].hinttext );
    }
}

void    WndToolOpen( gui_ord height, bool fixed )
{
    if( SysGUI() ) {
        WndCreateToolBarWithTips( height, fixed, ArraySize(ToolBar), ToolBar );
    }
}

void    WndToolClose( void )
{
    if( SysGUI() ) {
        WndCloseToolBar();
    }
}
