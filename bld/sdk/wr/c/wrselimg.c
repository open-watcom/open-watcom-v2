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


#include <wwindows.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wrmsg.h"
#include "wrmaini.h"
#include "wrdmsgi.h"
#include "selimage.h"
#include "jdlg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT extern BOOL CALLBACK WRSelectImageProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void WRSetEntries( HWND, WRSelectImageInfo * );
static BOOL WRSetWinInfo( HWND, WRSelectImageInfo * );
static BOOL WRGetWinInfo( HWND, WRSelectImageInfo * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

void WRAPI WRFreeSelectImageInfo( WRSelectImageInfo *info )
{
    if( info != NULL ) {
        WRMemFree( info );
    }
}

WRSelectImageInfo * WRAPI WRSelectImage( HWND parent, WRInfo *rinfo, FARPROC hcb )
{
    DLGPROC             proc;
    HINSTANCE           inst;
    BOOL                modified;
    WRSelectImageInfo   *info;

    if( rinfo == NULL ) {
        return( NULL );
    }

    info = (WRSelectImageInfo *)WRMemAlloc( sizeof( WRSelectImageInfo ) );
    if( info == NULL ) {
        return( NULL );
    }
    memset( info, 0, sizeof( WRSelectImageInfo ) );

    info->hcb = hcb;
    info->info = rinfo;

    inst = WRGetInstance();

    proc = (DLGPROC)MakeProcInstance( (FARPROC)WRSelectImageProc, inst );

    modified = JDialogBoxParam( inst, "WRSelectImage", parent, proc, (LPARAM)info );

    FreeProcInstance( (FARPROC)proc );

    if( modified == -1 || modified == IDCANCEL ) {
        WRMemFree( info );
        info = NULL;
    }

    return( info );
}

void WRSetEntries( HWND hdlg, WRSelectImageInfo *info )
{
    HWND                lbox;
    WResTypeNode        *tnode;
    char                *empty_str;

    if( info == NULL || info->info == NULL || hdlg == (HWND)NULL ) {
        return;
    }

    lbox = GetDlgItem( hdlg, IDM_SELIMGLBOX );
    if( lbox == (HWND)NULL ) {
        return;
    }
    SendMessage( lbox, LB_RESETCONTENT, 0, 0 );

    tnode = WRFindTypeNode( info->info->dir, info->type, NULL );
    if( tnode == NULL ) {
        empty_str = WRAllocRCString( WR_EMPTY );
        if( empty_str != NULL ) {
            WRSetLBoxWithStr( lbox, empty_str, NULL );
            WRFreeRCString( empty_str );
        }
        return;
    }

    WRSetResNamesFromTypeNode( lbox, tnode );
    SendMessage( lbox, LB_SETCURSEL, 0, 0 );
}

BOOL WRSetWinInfo( HWND hdlg, WRSelectImageInfo *info )
{
    WResTypeNode        *tnode;
    BOOL                lbox_set;

    if( info == NULL || hdlg == (HWND)NULL ) {
        return( FALSE );
    }

    lbox_set = FALSE;

    tnode = WRFindTypeNode( info->info->dir, (uint_16)RT_BITMAP, NULL );
    if( tnode != NULL ) {
        CheckDlgButton( hdlg, IDM_SELIMGBMP, 1 );
        info->type = (uint_16)RT_BITMAP;
        WRSetEntries( hdlg, info );
        lbox_set = TRUE;
    } else {
        EnableWindow( GetDlgItem( hdlg, IDM_SELIMGBMP ), FALSE );
    }

    tnode = WRFindTypeNode( info->info->dir, (uint_16)RT_GROUP_CURSOR, NULL );
    if( tnode != NULL ) {
        if( !lbox_set ) {
            CheckDlgButton( hdlg, IDM_SELIMGCUR, 1 );
            info->type = (uint_16)RT_GROUP_CURSOR;
            WRSetEntries( hdlg, info );
            lbox_set = TRUE;
        }
    } else {
        EnableWindow( GetDlgItem( hdlg, IDM_SELIMGCUR ), FALSE );
    }

    tnode = WRFindTypeNode( info->info->dir, (uint_16)RT_GROUP_ICON, NULL );
    if( tnode != NULL ) {
        if( !lbox_set ) {
            CheckDlgButton( hdlg, IDM_SELIMGICO, 1 );
            info->type = (uint_16)RT_GROUP_ICON;
            WRSetEntries( hdlg, info );
            lbox_set = TRUE;
        }
    } else {
        EnableWindow( GetDlgItem( hdlg, IDM_SELIMGICO ), FALSE );
    }

    if( !lbox_set ) {
        WRDisplayErrorMsg( WR_PRJNOIMAGES );
    }

    return( lbox_set );
}

BOOL WRGetWinInfo( HWND hdlg, WRSelectImageInfo *info )
{
    HWND        lbox;
    LRESULT     index;

    if( info == NULL || info->info == NULL || hdlg == (HWND)NULL ) {
        return( FALSE );
    }

    lbox = GetDlgItem( hdlg, IDM_SELIMGLBOX );
    if( lbox == (HWND)NULL ) {
        return( FALSE );
    }

    index = SendMessage( lbox, LB_GETCURSEL, 0, 0 );
    if( index == LB_ERR ) {
        return( FALSE );
    }

    info->lnode = (WResLangNode *)SendMessage( lbox, LB_GETITEMDATA, (WPARAM)index, 0 );
    if( info->lnode == NULL ) {
        return( FALSE );
    }

    return( TRUE );
}

WINEXPORT BOOL CALLBACK WRSelectImageProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WRSelectImageInfo   *info;
    BOOL                ret;

    ret = FALSE;

    switch( message ) {
    case WM_DESTROY:
        WRUnregisterDialog( hDlg );
        break;

    case WM_INITDIALOG:
        info = (WRSelectImageInfo *)lParam;
        if( info == NULL ) {
            EndDialog( hDlg, FALSE );
        }
        SET_DLGDATA( hDlg, (LONG_PTR)info );
        WRRegisterDialog( hDlg );
        if( !WRSetWinInfo( hDlg, info ) ) {
            EndDialog( hDlg, FALSE );
        }
        ret = TRUE;
        break;

    case WM_SYSCOLORCHANGE:
        WRCtl3dColorChange();
        break;

    case WM_COMMAND:
        info = (WRSelectImageInfo *)GET_DLGDATA( hDlg );
        switch( LOWORD( wParam ) ) {
        case IDM_SELIMGHELP:
            if( info != NULL && info->hcb != NULL ) {
                (*info->hcb)();
            }
            break;

        case IDOK:
            if( info == NULL ) {
                EndDialog( hDlg, FALSE );
                ret = TRUE;
            } else if( WRGetWinInfo( hDlg, info ) ) {
                EndDialog( hDlg, TRUE );
                ret = TRUE;
            }
            break;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            ret = TRUE;
            break;

        case IDM_SELIMGBMP:
            if( GET_WM_COMMAND_CMD( wParam, lParam ) != BN_CLICKED ) {
                break;
            }
            if( info->type != (uint_16)RT_BITMAP ) {
                info->type = (uint_16)RT_BITMAP;
                WRSetEntries( hDlg, info );
            }
            break;

        case IDM_SELIMGCUR:
            if( GET_WM_COMMAND_CMD( wParam, lParam ) != BN_CLICKED ) {
                break;
            }
            if( info->type != (uint_16)RT_GROUP_CURSOR ) {
                info->type = (uint_16)RT_GROUP_CURSOR;
                WRSetEntries( hDlg, info );
            }
            break;

        case IDM_SELIMGICO:
            if( GET_WM_COMMAND_CMD( wParam, lParam ) != BN_CLICKED ) {
                break;
            }
            if( info->type != (uint_16)RT_GROUP_ICON ) {
                info->type = (uint_16)RT_GROUP_ICON;
                WRSetEntries( hDlg, info );
            }
            break;
        }
        break;
    }

    return( ret );
}
