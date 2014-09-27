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
#include <stdlib.h>
#include <string.h>
#include "guistr.h"
#include "guihook.h"
#include "guixmdi.h"

extern  bool            GUIMDI;

void InitMDI( gui_window *wnd, gui_create_info *dlg_info );
void ChangeTitle( gui_window *wnd );
void XChangeTitle( gui_window *wnd );
void MDIResetMenus( gui_window *wnd, gui_window *parent, int num_menus, gui_menu_struct *menu );
void MDIDelete( gui_window *wnd );
void BroughtToFront( gui_window *wnd );
void MDIDeleteMenu( unsigned id );
void AddMDIActions( bool has_items, gui_window *wnd );
void EnableMDIActions( bool enable );

void GUIMDIInit( void )
{
    GUIMDI = true;
    GUISetXInitMDI( &XInitMDI );
    GUISetInitMDI( &InitMDI );
    GUISetMDIChangeTitle( &ChangeTitle );
    GUISetXChangeTitle( &XChangeTitle );
    GUISetMDIBroughtToFront( &BroughtToFront );
    GUISetMDIDelete( &MDIDelete );
    GUISetMDIResetMenus( &MDIResetMenus );
    GUISetMDIDeleteMenuItem( &MDIDeleteMenu );
    GUISetAddMDIActions( &AddMDIActions );
    GUISetEnableMDIActions( &EnableMDIActions );
    GUIXMDIInit();
}

