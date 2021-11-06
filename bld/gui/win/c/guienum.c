/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include <string.h>
#include "guixutil.h"
#include "wclbproc.h"
#include "guixwind.h"


/* Local Window callback functions prototypes */
WINEXPORT BOOL CALLBACK GUIEnumChildWindowsEnumFunc( HWND hwnd, WPI_PARAM2 lparam );

typedef struct {
    gui_window          *parent;
    ENUMCALLBACK        *func;
    void                *param;
} enum_info;

BOOL CALLBACK GUIEnumChildWindowsEnumFunc( HWND hwnd, WPI_PARAM2 lparam )
{
    gui_window  *wnd;
    enum_info   *info;
    char        osclassname[GUI_CLASSNAME_MAX + 1];
    bool        is_gui;
    int         len;

    len = _wpi_getclassname( hwnd, osclassname, sizeof( osclassname ) );
    osclassname[len] = '\0';
    is_gui = ( strcmp( osclassname, GUIClass ) == 0 );
#ifdef __OS2_PM__
    if( !is_gui ) {
        hwnd = WinWindowFromID( hwnd, FID_CLIENT );
        if( hwnd != NULLHANDLE ) {
            len = _wpi_getclassname( hwnd, osclassname, sizeof( osclassname ) );
            osclassname[len] = '\0';
            is_gui = ( strcmp( osclassname, GUIClass ) == 0 );
        }
    }
#endif
    if( is_gui ) {
        info = (enum_info *)lparam;
        wnd = GUIGetWindow( hwnd );
        if( ( wnd != NULL ) && ( wnd->parent == info->parent ) ) {
            (*(info->func))( wnd, info->param );
        }
    }
    return( TRUE );
}

void GUIAPI GUIEnumChildWindows( gui_window *wnd, ENUMCALLBACK *func, void *param )
{
    enum_info       info;
#ifdef __OS2_PM__
    WPI_ENUMPROC    wndenumproc;
#else
    WNDENUMPROC         wndenumproc;
#endif

    info.parent = wnd;
    info.func = func;
    info.param = param;
#ifdef __OS2_PM__
    wndenumproc = _wpi_makeenumprocinstance( GUIEnumChildWindowsEnumFunc, GUIMainHInst );
    _wpi_enumchildwindows( wnd->hwnd, wndenumproc, (LPARAM)&info );
    _wpi_freeenumprocinstance( wndenumproc );
#else
    wndenumproc = MakeProcInstance_WNDENUM( GUIEnumChildWindowsEnumFunc, GUIMainHInst );
    EnumChildWindows( wnd->hwnd, wndenumproc, (LPARAM)&info );
    FreeProcInstance_WNDENUM( wndenumproc );
#endif
}
