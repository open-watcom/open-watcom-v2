/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
#include "dbgwtool.h"
#include "dbginit.h"
#include "menudef.h"
#include "dbgicon.h"
#include "wv.rh"


#define TOOL(a,b,c) { "", BITMAPID( BITMAP_ ##a ), MENU_TOOL_ ##b, MENU_LIT( HELP_ ##c ), MENU_LIT( TIP_ ##c ) }

static gui_toolbar_struct ToolBar[] = {
    TOOL( GO,     GO,               XGo ),
    TOOL( OVER,   TRACE_OVER,       XOver ),
    TOOL( INTO,   STEP_INTO,        XInto ),
    TOOL( RETURN, RETURN_TO_CALLER, XReturn ),
    TOOL( BACK,   UNDO,             XUndo ),
    TOOL( FOR,    REDO,             XRedo ),
    TOOL( UP,     UP_STACK,         UXnwind_Stack ),
    TOOL( DOWN,   DOWN_STACK,       RXewind_Stack ),
    TOOL( HOME,   HOME,             XHome ),
};

#undef TOOL

static gui_toolbar_items tb_ToolBar = GUI_TOOLBAR_ARRAY( ToolBar );

void    InitToolBar( void )
{
    int         i;

    for( i = 0; i < ArraySize( ToolBar ); ++i ) {
        ToolBar[i].hinttext = WndLoadString( (gui_res_id)(pointer_uint)ToolBar[i].hinttext );
        ToolBar[i].tip = WndLoadString( (gui_res_id)(pointer_uint)ToolBar[i].tip );
    }
}

void    FiniToolBar( void )
{
    int         i;

    for( i = 0; i < ArraySize( ToolBar ); ++i ) {
        WndFree( (void *)ToolBar[i].hinttext );
        WndFree( (void *)ToolBar[i].tip );
    }
}

void    WndToolOpen( gui_ord height, bool fixed )
{
    if( GUIIsGUI() ) {
        WndCreateToolBarWithTips( height, fixed, &tb_ToolBar );
    }
}

void    WndToolClose( void )
{
    if( GUIIsGUI() ) {
        WndCloseToolBar();
    }
}
