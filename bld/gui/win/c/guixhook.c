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
#include "guixutil.h"
#include "guixhook.h"

static  bool    (*ProcessMsg)(gui_window *, HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2, WPI_MRESULT *) = NULL;
static  bool    (*IsMaximized)( gui_window * )          = NULL;
static  bool    (*UpdatedMenu)( void )                  = NULL;
static  void    (*ResizeStatus)( gui_window * )         = NULL;
static  void    (*FreeStatus)( void )                   = NULL;
static  bool    (*NewWindow)( HWND )                    = NULL;
static  void    (*MDIMaximize)(bool, gui_window *)      = NULL;
static  bool    (*IsMDIChildWindow)(gui_window *)       = NULL;
static  void    (*SetMDIRestoredSize)(HWND, WPI_RECT *) = NULL;

void GUISetMDIProcessMessage( bool (*func)(gui_window *, HWND, WPI_MSG, WPI_PARAM1,
                              WPI_PARAM2, WPI_MRESULT *) )
{
    ProcessMsg = func;
}

bool GUIMDIProcessMessage( gui_window *wnd, HWND hwnd, WPI_MSG msg,
                           WPI_PARAM1 wparam, WPI_PARAM2 lparam, WPI_MRESULT *ret )
{
    if( ProcessMsg != NULL ) {
        return( (*ProcessMsg)( wnd, hwnd, msg, wparam, lparam, ret ) );
    }
    return( false );
}

bool GUIMDIMaximized( gui_window *wnd )
{
    if( IsMaximized != NULL ) {
        return( (*IsMaximized)( wnd ) );
    }
    return( _wpi_iszoomed( GUIGetParentFrameHWND( wnd ) ) != 0 );
}

void GUISetMDIMaximized( bool (*func)( gui_window * ) )
{
    IsMaximized = func;
}

bool GUIMDIUpdatedMenu( void )
{
    if( UpdatedMenu != NULL ) {
        return( (*UpdatedMenu)() );
    }
    return( false );
}

void GUISetMDIUpdatedMenu( bool (*func)( void ) )
{
    UpdatedMenu = func;
}

void GUISetIsMDIChildWindow( bool (*func)( gui_window *) )
{
    IsMDIChildWindow = func;
}

bool GUIIsMDIChildWindow( gui_window *wnd )
{
    if( IsMDIChildWindow != NULL ) {
        return( (*IsMDIChildWindow)(wnd) );
    }
    return( false );
}

void GUIResizeStatus( gui_window *wnd )
{
    if( ResizeStatus != NULL ) {
        (*ResizeStatus)( wnd );
    }
}

void GUISetResizeStatus( void (*func)( gui_window * ) )
{
    ResizeStatus = func;
}

void GUIFreeStatus( void )
{
    if( FreeStatus != NULL ) {
        (*FreeStatus)();
    }
}

void GUISetFreeStatus( void (*func)( void ) )
{
    FreeStatus = func;
}

void GUISetMDINewWindow( bool (*func)(HWND) )
{
    NewWindow = func;
}

bool GUIMDINewWindow( HWND hwnd )
{
    if( NewWindow != NULL ) {
        return( (*NewWindow)( hwnd ) );
    } else {
        return( false );
    }
}

void GUISetMDIMaximize( void (*func)(bool, gui_window *) )
{
    MDIMaximize = func;
}

void GUIMDIMaximize( bool max, gui_window *wnd )
{
    if( MDIMaximize != NULL ) {
        (*MDIMaximize)( max, wnd );
    }
}

void GUISetSetMDIRestoredSize( void (*func)(HWND, WPI_RECT *) )
{
    SetMDIRestoredSize = func;
}

void GUISetMDIRestoredSize( HWND hwnd, WPI_RECT *rect )
{
    if( SetMDIRestoredSize != NULL ) {
        (*SetMDIRestoredSize)( hwnd, rect );
    }
}
