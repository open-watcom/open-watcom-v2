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
#include "guixdlg.h"

unsigned GUIIsChecked( gui_window *wnd, unsigned id )
{
    unsigned    ret;

    ret = (unsigned)
        GUISendDlgItemMessage( wnd->hwnd, id, BM_GETCHECK,
                               (WPI_PARAM1)NULL, (WPI_PARAM2)NULL );

    return( ret & 0x3 );
}

bool GUISetChecked( gui_window *wnd, unsigned id, unsigned check )
{
    if( ( check == GUI_CHECKED ) && !( wnd->flags & IS_RES_DIALOG ) ) {
        if( GUIIsChecked( wnd, id ) != GUI_CHECKED ) {
            return( GUIProcessControlNotification( id, BN_CLICKED, wnd ) );
        }
    } else {
        GUISendDlgItemMessage( wnd->hwnd, id, BM_SETCHECK, (WPI_PARAM1)check, (WPI_PARAM2)NULL );
    }

    return( true );
}

