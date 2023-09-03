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
#include <stdlib.h>
#include "servio.h"
#include "trptypes.h"
#include "packet.h"
#include "options.h"


extern int              NumPrinters( void );
extern unsigned         PrnAddress( int );

WINEXPORT INT_PTR CALLBACK OptionsDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    char    parms[PARMS_MAXLEN];
    HWND    edit;
    int     num = NumPrinters();
    char    buff[20];

    /* unused parameters */ (void)lparam;

    edit = GetDlgItem( hwnd, IDDI_PORT_EDIT );
    switch( msg ) {
    case WM_INITDIALOG:
        RemoteLinkGet( parms, sizeof( parms ) );
        EnableWindow( edit, FALSE );
        if( parms[0] >= '1' && parms[0] <= '3' ) {
            if( parms[0] > num + '0' ) {
                parms[0] = num + '0';
            }
        }
        switch( parms[0] ) {
        case '1':
        default:
            if( num >= 1 ) {
                SendDlgItemMessage( hwnd, IDDI_LPT1, BM_SETCHECK, 1, 0 );
                SetDlgItemText( hwnd, IDDI_PORT_EDIT, utoa( PrnAddress( 0 ), buff, 16 ) );
            }
            break;
        case '2':
            if( num >= 2 ) {
                SendDlgItemMessage( hwnd, IDDI_LPT2, BM_SETCHECK, 1, 0 );
                SetDlgItemText( hwnd, IDDI_PORT_EDIT, utoa( PrnAddress( 1 ), buff, 16 ) );
            }
            break;
        case '3':
            if( num >= 3 ) {
                SendDlgItemMessage( hwnd, IDDI_LPT3, BM_SETCHECK, 1, 0 );
                SetDlgItemText( hwnd, IDDI_PORT_EDIT, utoa( PrnAddress( 2 ), buff, 16 ) );
            }
            break;
        case 'p':
        case 'P':
            SendDlgItemMessage( hwnd, IDDI_PORT_ADDR, BM_SETCHECK, 1, 0 );
            SetDlgItemText( hwnd, IDDI_PORT_EDIT, parms + 1 );
            EnableWindow( edit, TRUE );
        }
        if( num < 3 )
            EnableWindow( GetDlgItem( hwnd, IDDI_LPT3 ), FALSE );
        if( num < 2 )
            EnableWindow( GetDlgItem( hwnd, IDDI_LPT2 ), FALSE );
        if( num < 1 )
            EnableWindow( GetDlgItem( hwnd, IDDI_LPT1 ), FALSE );
        return( TRUE );

    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDOK:
            parms[1] = '\0';
            if( SendDlgItemMessage( hwnd, IDDI_LPT1, BM_GETCHECK, 0, 0 ) == BST_CHECKED ) {
                parms[0] = '1';
            } else if( SendDlgItemMessage( hwnd, IDDI_LPT2, BM_GETCHECK, 0, 0 ) == BST_CHECKED ) {
                parms[0] = '2';
            } else if( SendDlgItemMessage( hwnd, IDDI_LPT3, BM_GETCHECK, 0, 0 ) == BST_CHECKED ) {
                parms[0] = '3';
            } else if( SendDlgItemMessage( hwnd, IDDI_PORT_ADDR, BM_GETCHECK, 0, 0 ) == BST_CHECKED ) {
                parms[0] = 'p';
                GetDlgItemText( hwnd, IDDI_PORT_EDIT, parms + 1, sizeof( parms ) - 1 );
            } else {
                parms[0] = '1';
            }
            RemoteLinkSet( parms );
        case IDCANCEL:
            EndDialog( hwnd, TRUE );
            return( TRUE );
        case IDDI_LPT1:
            SetDlgItemText( hwnd, IDDI_PORT_EDIT, utoa( PrnAddress( 0 ), buff, 16 ) );
            EnableWindow( edit, FALSE );
            break;
        case IDDI_LPT2:
            SetDlgItemText( hwnd, IDDI_PORT_EDIT, utoa( PrnAddress( 1 ), buff, 16 ) );
            EnableWindow( edit, FALSE );
            break;
        case IDDI_LPT3:
            SetDlgItemText( hwnd, IDDI_PORT_EDIT, utoa( PrnAddress( 2 ), buff, 16 ) );
            EnableWindow( edit, FALSE );
            break;
        case IDDI_PORT_ADDR:
            SetDlgItemText( hwnd, IDDI_PORT_EDIT, parms[0] == '\0' ? "" : parms + 1 );
            EnableWindow( edit, TRUE );
            break;
        }
        break;
    }
    return( FALSE );

}
