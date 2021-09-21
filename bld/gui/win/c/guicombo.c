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
#include "guicombo.h"
#include "guicontr.h"
//#include "guixhook.h"
#include "ctl3dcvr.h"
#include "wclbproc.h"
#include "guixwind.h"
#include "oswincls.h"


typedef struct {
    bool        success;
    WPI_WNDPROC old;
} enum_info;

extern  controls_struct GUIControls[];

/* Local Window callback functions prototypes */
WINEXPORT BOOL CALLBACK GUISubClassEditComboboxEnumFunc( HWND hwnd, WPI_PARAM2 lparam );

BOOL CALLBACK GUISubClassEditComboboxEnumFunc( HWND hwnd, WPI_PARAM2 lparam )
{
    char        osclassname[GUI_CLASSNAME_MAX + 1];
    enum_info   *info;

    info = ( enum_info * )lparam;
    if( info == NULL ) {
        return( FALSE );
    }
    if( info->success == true ) {
        return( TRUE );
    }
    if( _wpi_getclassname( hwnd, osclassname, sizeof( osclassname ) ) ) {
        if( stricmp( osclassname, GUIControls[GUI_EDIT].osclassname ) == 0 ) {
            info->success = true;
            info->old = GUIDoSubClass( hwnd, GUI_EDIT );
            //CvrCtl3dSubclassCtl( hwnd );
        }
    }
    return( TRUE );
}

WPI_WNDPROC GUISubClassEditCombobox( HWND hwnd )
{
    enum_info           e_info;
#ifdef __OS2_PM__
    WPI_ENUMPROC        wndenumproc;
#else
    WNDENUMPROC         wndenumproc;
#endif

    e_info.success = false;
#ifdef __OS2_PM__
    wndenumproc = _wpi_makeenumprocinstance( GUISubClassEditComboboxEnumFunc, GUIMainHInst );
    _wpi_enumchildwindows( hwnd, wndenumproc, (LPARAM)&e_info );
    _wpi_freeenumprocinstance( wndenumproc );
#else
    wndenumproc = MakeProcInstance_WNDENUM( GUISubClassEditComboboxEnumFunc, GUIMainHInst );
    EnumChildWindows( hwnd, wndenumproc, (LPARAM)&e_info );
    FreeProcInstance_WNDENUM( wndenumproc );
#endif
    if( e_info.success ) {
        return( e_info.old );
    }
    return( NULL );
}
