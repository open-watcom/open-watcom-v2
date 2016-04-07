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
#include <string.h>
#include "guixutil.h"
#include "wprocmap.h"
#include "guixwind.h"


/* Local Window callback functions prototypes */
WINEXPORT BOOL CALLBACK GUIEnumChildWindowsEnumFunc( HWND hwnd, WPI_PARAM2 lparam );

#define CLASS_LENGTH    8

extern  char            GUIClass[];

typedef struct {
    gui_window          *parent;
    ENUMCALLBACK        *func;
    void                *param;
} enum_info;

BOOL CALLBACK GUIEnumChildWindowsEnumFunc( HWND hwnd, WPI_PARAM2 lparam )
{
    gui_window  *wnd;
    enum_info   *info;
    char        buff[CLASS_LENGTH + 1];
    bool        is_gui;

    GetClassName( hwnd, buff, CLASS_LENGTH + 1 );
    is_gui = strncmp( buff, GUIClass, CLASS_LENGTH ) == 0;
#ifdef __OS2_PM__
    if( !is_gui ) {
        hwnd = WinWindowFromID( hwnd, FID_CLIENT );
        if( hwnd != NULLHANDLE ) {
            GetClassName( hwnd, buff, CLASS_LENGTH + 1 );
            is_gui = strncmp( buff, GUIClass, CLASS_LENGTH ) == 0;
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

void GUIEnumChildWindows( gui_window *wnd, ENUMCALLBACK *func, void *param )
{
    WPI_ENUMPROC    fp;
    enum_info       info;

    fp = _wpi_makeenumprocinstance( GUIEnumChildWindowsEnumFunc, GUIMainHInst );
    info.parent = wnd;
    info.func = func;
    info.param = param;
    _wpi_enumchildwindows( wnd->hwnd, fp, (LPARAM)&info );
    _wpi_freeprocinstance( (WPI_PROC)fp );
}
