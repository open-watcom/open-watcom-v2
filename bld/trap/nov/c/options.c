/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include <wwindows.h>
#include "servio.h"
#include "trptypes.h"
#include "packet.h"
#include "options.h"


WINEXPORT INT_PTR CALLBACK OptionsDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    char    parms[PARMS_MAXLEN];

    /* unused parameters */ (void)lparam;

    switch( msg ) {
    case WM_INITDIALOG:
        RemoteLinkGet( parms, sizeof( parms ) );
        SetDlgItemText( hwnd, IDDI_SERVER_NAME, parms );
        SendDlgItemMessage( hwnd, IDDI_SERVER_NAME, EM_SETSEL, 0, -1 );
        return( true );

    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDOK:
            GetDlgItemText( hwnd, IDDI_SERVER_NAME, parms, sizeof( parms ) );
            RemoteLinkSet( parms );
        case IDCANCEL:
            EndDialog( hwnd, true );
            return( true );
        }
        break;
    }
    return( false );

}
