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
#include "guicombo.h"
#include "guicontr.h"
#include "guixhook.h"
#include "wprocmap.h"

extern  controls_struct GUIControls[];
extern  WPI_INST        GUIMainHInst;

typedef struct {
    bool        success;
    WPI_PROC    old;
} enum_info;

BOOL CALLBACK GUISubClassEditComboboxEnumFunc( HWND hwnd, WPI_PARAM2 lparam )
{
    char        buff[5];
    enum_info   *info;

    info = ( enum_info * )lparam;
    if( info == NULL ) {
        return( FALSE );
    }
    if( info->success == TRUE ) {
        return( TRUE );
    }
    GetClassName( hwnd, buff, 5 );
#ifndef __OS2_PM__
    //if( !strcmp( buff, "#6" ) ) {
    if( !strcmp( buff, GUIControls[GUI_EDIT].classname ) ) {
        info->success = TRUE;
        info->old = GUIDoSubClass( hwnd, GUI_EDIT );
        //GUICtl3dSubclassCtl( hwnd );
    }
#endif
    return( TRUE );
}

static WPI_PROC _GUISubClassEditCombobox( HWND hwnd, WPI_ENUMPROCx fn )
{
    enum_info       e_info;
    WPI_ENUMPROC    fp;

    e_info.success = FALSE;
    fp = _wpi_makeenumprocinstance( fn, GUIMainHInst );
    _wpi_enumchildwindows( hwnd, fp, (LPARAM)&e_info );
    if( e_info.success ) {
        return( e_info.old );
    }
    return( NULL );
}

WPI_PROC GUISubClassEditCombobox( HWND hwnd )
{
    return( _GUISubClassEditCombobox( hwnd, GUISubClassEditComboboxEnumFunc ) );
}
