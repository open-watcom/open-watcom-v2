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
* Description:  Set or reset mouse pointer type.
*
****************************************************************************/


#include "guiwind.h"
#include "watcom.h"


gui_mcursor_handle GUIAPI GUISetMouseCursor( gui_mcursor_type type )
{
    HCURSOR     cursor;

    switch( type ) {
    case GUI_ARROW_CURSOR:
        cursor = _wpi_getsyscursor( IDC_ARROW );
        break;
    case GUI_HOURGLASS_CURSOR:
        cursor = _wpi_getsyscursor( IDC_WAIT );
        break;
    case GUI_CROSS_CURSOR:
        cursor = _wpi_getsyscursor( IDC_CROSS );
        break;
    default:
        return( NULL );
    }
    if( cursor != NULLHANDLE )
        return( (void *)_wpi_setcursor( cursor ) );
    return( NULL );
}

void GUIAPI GUIResetMouseCursor( gui_mcursor_handle old )
{
    if( old != NULL ) {
        _wpi_setcursor( (HCURSOR)(pointer_uint)old );
    }
}
