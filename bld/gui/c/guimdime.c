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


#include "guiwind.h"
#include "guistr.h"

#define MAX_LENGTH      80

extern  gui_window      *Root;
extern  int             GUIMDIMenuID;

static gui_menu_struct MDIMenu[] = {
    {  NULL,    GUI_MDI_CASCADE,        GUI_GRAYED,     NULL    },
    {  NULL,    GUI_MDI_TILE_HORZ,      GUI_GRAYED,     NULL    },
    {  NULL,    GUI_MDI_TILE_VERT,      GUI_GRAYED,     NULL    },
    {  NULL,    GUI_MDI_ARRANGE_ICONS,  GUI_GRAYED,     NULL    },
};
#define NUM_MDI_MENUS ( sizeof( MDIMenu ) / sizeof( gui_menu_struct ) )

static gui_menu_struct MDIFirstSepMenu[] = {
    {    NULL,    GUI_MDI_FIRST_SEPARATOR,    GUI_SEPARATOR,    NULL    }
};

static bool MDIMenuStructInitialized = FALSE;

static void InitMDIMenuStruct( void )
{
    MDIMenu[ 0 ].label = LIT( XCascade );
    MDIMenu[ 0 ].hinttext = LIT( Cascade_Hint );
    MDIMenu[ 1 ].label = LIT( Tile_XHorz );
    MDIMenu[ 1 ].hinttext = LIT( Tile_Horz_Hint );
    MDIMenu[ 2 ].label = LIT( Tile_XVert );
    MDIMenu[ 2 ].hinttext = LIT( Tile_Vert_Hint );
    MDIMenu[ 3 ].label = LIT( XArrange_Icons );
    MDIMenu[ 3 ].hinttext = LIT( Arrange_Icons_Hint );
}

void EnableMDIActions( bool enable )
{
    GUIEnableMenuItem( Root, GUI_MDI_CASCADE, enable, FALSE );
    GUIEnableMenuItem( Root, GUI_MDI_TILE_HORZ, enable, FALSE );
    GUIEnableMenuItem( Root, GUI_MDI_TILE_VERT, enable, FALSE );
    GUIEnableMenuItem( Root, GUI_MDI_ARRANGE_ICONS, enable, FALSE );
}

void AddMDIActions( bool has_items, gui_window *wnd )
{
    int         i;

    if( !MDIMenuStructInitialized ) {
        InitMDIMenuStruct();
        MDIMenuStructInitialized = TRUE;
    }

    if( has_items ) {
        GUIAppendMenuToPopup( wnd, GUIMDIMenuID, MDIFirstSepMenu, FALSE );
    }

    for( i = 0; i < NUM_MDI_MENUS; i++ ) {
        GUIAppendMenuToPopup( wnd, GUIMDIMenuID, &MDIMenu[i], FALSE );
    }
}
