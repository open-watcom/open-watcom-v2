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
#include "guihook.h"
#include "guimdi.h"

static  void    (*fCleanupHotSpots)(void)               = NULL;
static  void    (*fXChangeTitle)( gui_window *)         = NULL;
static  bool    (*fXInitMDI)( gui_window *)             = NULL;
static  void    (*fInitMDI)( gui_window *, gui_create_info *) = NULL;
static  void    (*fChangeTitle)(gui_window * )          = NULL;
static  void    (*fBroughtToFront)(gui_window * )       = NULL;
static  void    (*fDelete)(gui_window * )               = NULL;
static  void    (*fReset)( gui_window *, gui_window *, gui_ctl_idx, gui_menu_struct *) = NULL;
static  void    (*fDeleteMenuItem)(gui_ctl_id id)       = NULL;
static  void    (*fEnableMDIActions)( bool enable )     = NULL;
static  void    (*fAddMDIActions)( bool has_items, gui_window *wnd )    = NULL;

void GUISetHotSpotCleanup( void (*func)(void) )
{
    fCleanupHotSpots = func;
}

void GUICleanupHotSpots( void )
{
    if( fCleanupHotSpots != NULL ) {
        (*fCleanupHotSpots)();
    }
}

extern void GUISetXChangeTitle( void (*func)(gui_window*) )
{
    fXChangeTitle = func;
}

void GUIXChangeTitle( gui_window *wnd )
{
    if( fXChangeTitle != NULL ) {
        (*fXChangeTitle)(wnd);
    }
}

extern void GUISetXInitMDI( bool (*func)(gui_window*) )
{
    fXInitMDI = func;
}

bool GUIXInitMDI( gui_window *wnd )
{
    if( fXInitMDI != NULL ) {
        return( (*fXInitMDI)(wnd) );
    }
    return( false );
}

void GUISetInitMDI( void (*func)(gui_window*, gui_create_info *) )
{
    fInitMDI = func;
}

void GUIInitMDI( gui_window *wnd, gui_create_info *dlg_info )
{
    if( fInitMDI != NULL ) {
        (*fInitMDI)(wnd, dlg_info);
    }
}

void GUIChangeMDITitle( gui_window *wnd )
{
    if( fChangeTitle != NULL ) {
        (*fChangeTitle)(wnd);
    }
}

void GUISetMDIChangeTitle( void (*func)(gui_window * ) )
{
    fChangeTitle = func;
}

void GUISetMDIBroughtToFront( void (*func)(gui_window *) )
{
    fBroughtToFront = func;
}

void GUIMDIBroughtToFront( gui_window *wnd )
{
    if( fBroughtToFront != NULL ) {
        (*fBroughtToFront)(wnd );
    }
}

void GUISetMDIDelete( void (*func)(gui_window *) )
{
    fDelete = func;
}

void GUIMDIDelete( gui_window *wnd )
{
    if( fDelete != NULL ) {
        (*fDelete)(wnd );
    }
}

void GUISetMDIResetMenus( void (*func)( gui_window *, gui_window *,
                          gui_ctl_idx, gui_menu_struct * ) )
{
    fReset = func;
}

void GUIMDIResetMenus( gui_window *wnd, gui_window *parent, gui_ctl_idx num_menus, gui_menu_struct *menu )
{
    if( fReset != NULL ) {
        (*fReset )( wnd, parent, num_menus, menu );
    }
}

void GUISetMDIDeleteMenuItem( void (*func)(gui_ctl_id id) )
{
    fDeleteMenuItem = func;
}

void GUIMDIDeleteMenuItem( gui_ctl_id id )
{
    if( fDeleteMenuItem != NULL ) {
        (*fDeleteMenuItem )( id );
    }
}

void    GUISetEnableMDIActions( void (*func)(bool enable) )
{
    fEnableMDIActions = func;
}

void GUIEnableMDIActions( bool enable )
{
    if( fEnableMDIActions != NULL ) {
        (*fEnableMDIActions)( enable );
    }
}

void    GUISetAddMDIActions( void (*func)( bool has_items, gui_window *wnd ) )
{
    fAddMDIActions = func;
}

void GUIAddMDIActions( bool has_items, gui_window *wnd )
{
    if( fAddMDIActions != NULL ) {
        (*fAddMDIActions)( has_items, wnd );
    }
}

