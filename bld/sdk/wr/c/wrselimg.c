/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#include "wrglbl.h"
#include "wrmsg.h"
#include "wrmaini.h"
#include "wrdmsgi.h"
#include "selimage.rh"
#include "jdlg.h"
#include "winexprt.h"
#include "wresdefn.h"
#include "wclbproc.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT extern INT_PTR CALLBACK WRSelectImageDlgProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void WRSetEntries( HWND, WRSelectImageInfo * );
static bool WRSetWinInfo( HWND, WRSelectImageInfo * );
static bool WRGetWinInfo( HWND, WRSelectImageInfo * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

void WRAPI WRFreeSelectImageInfo( WRSelectImageInfo *siinfo )
{
    if( siinfo != NULL ) {
        MemFree( siinfo );
    }
}

WRSelectImageInfo * WRAPI WRSelectImage( HWND parent, WRInfo *info, HELPFUNC help_callback )
{
    DLGPROC             dlgproc;
    HINSTANCE           inst;
    INT_PTR             modified;
    WRSelectImageInfo   *siinfo;

    if( info == NULL ) {
        return( NULL );
    }

    siinfo = (WRSelectImageInfo *)MemAlloc( sizeof( WRSelectImageInfo ) );
    if( siinfo == NULL ) {
        return( NULL );
    }
    memset( siinfo, 0, sizeof( WRSelectImageInfo ) );

    siinfo->help_callback = help_callback;
    siinfo->info = info;

    inst = WRGetInstance();

    dlgproc = MakeProcInstance_DLG( WRSelectImageDlgProc, inst );

    modified = JDialogBoxParam( inst, "WRSelectImage", parent, dlgproc, (LPARAM)(LPVOID)siinfo );

    FreeProcInstance_DLG( dlgproc );

    if( modified == -1 || modified == IDCANCEL ) {
        MemFree( siinfo );
        siinfo = NULL;
    }

    return( siinfo );
}

void WRSetEntries( HWND hdlg, WRSelectImageInfo *siinfo )
{
    HWND                lbox;
    WResTypeNode        *tnode;
    char                *empty_str;

    if( siinfo == NULL || siinfo->info == NULL || hdlg == (HWND)NULL ) {
        return;
    }

    lbox = GetDlgItem( hdlg, IDM_SELIMGLBOX );
    if( lbox == (HWND)NULL ) {
        return;
    }
    SendMessage( lbox, LB_RESETCONTENT, 0, 0 );

    tnode = WRFindTypeNode( siinfo->info->dir, siinfo->type, NULL );
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

static bool WRSetWinInfo( HWND hdlg, WRSelectImageInfo *siinfo )
{
    WResTypeNode        *tnode;
    bool                lbox_set;

    if( siinfo == NULL || hdlg == (HWND)NULL ) {
        return( false );
    }

    lbox_set = false;

    tnode = WRFindTypeNode( siinfo->info->dir, RESOURCE2INT( RT_BITMAP ), NULL );
    if( tnode != NULL ) {
        CheckDlgButton( hdlg, IDM_SELIMGBMP, BST_CHECKED );
        siinfo->type = RESOURCE2INT( RT_BITMAP );
        WRSetEntries( hdlg, siinfo );
        lbox_set = true;
    } else {
        EnableWindow( GetDlgItem( hdlg, IDM_SELIMGBMP ), FALSE );
    }

    tnode = WRFindTypeNode( siinfo->info->dir, RESOURCE2INT( RT_GROUP_CURSOR ), NULL );
    if( tnode != NULL ) {
        if( !lbox_set ) {
            CheckDlgButton( hdlg, IDM_SELIMGCUR, BST_CHECKED );
            siinfo->type = RESOURCE2INT( RT_GROUP_CURSOR );
            WRSetEntries( hdlg, siinfo );
            lbox_set = true;
        }
    } else {
        EnableWindow( GetDlgItem( hdlg, IDM_SELIMGCUR ), FALSE );
    }

    tnode = WRFindTypeNode( siinfo->info->dir, RESOURCE2INT( RT_GROUP_ICON ), NULL );
    if( tnode != NULL ) {
        if( !lbox_set ) {
            CheckDlgButton( hdlg, IDM_SELIMGICO, BST_CHECKED );
            siinfo->type = RESOURCE2INT( RT_GROUP_ICON );
            WRSetEntries( hdlg, siinfo );
            lbox_set = true;
        }
    } else {
        EnableWindow( GetDlgItem( hdlg, IDM_SELIMGICO ), FALSE );
    }

    if( !lbox_set ) {
        WRDisplayErrorMsg( WR_PRJNOIMAGES );
    }

    return( lbox_set );
}

static bool WRGetWinInfo( HWND hdlg, WRSelectImageInfo *siinfo )
{
    HWND        lbox;
    int         index;

    if( siinfo == NULL || siinfo->info == NULL || hdlg == (HWND)NULL ) {
        return( false );
    }

    lbox = GetDlgItem( hdlg, IDM_SELIMGLBOX );
    if( lbox == (HWND)NULL ) {
        return( false );
    }

    index = (int)SendMessage( lbox, LB_GETCURSEL, 0, 0 );
    if( index == LB_ERR ) {
        return( false );
    }

    siinfo->lnode = (WResLangNode *)SendMessage( lbox, LB_GETITEMDATA, (WPARAM)index, 0 );
    if( siinfo->lnode == NULL ) {
        return( false );
    }

    return( true );
}

WINEXPORT INT_PTR CALLBACK WRSelectImageDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WRSelectImageInfo   *siinfo;
    bool                ret;

    ret = false;

    switch( message ) {
    case WM_DESTROY:
        WRUnregisterDialog( hDlg );
        break;

    case WM_INITDIALOG:
        siinfo = (WRSelectImageInfo *)lParam;
        if( siinfo == NULL ) {
            EndDialog( hDlg, FALSE );
        }
        SET_DLGDATA( hDlg, siinfo );
        WRRegisterDialog( hDlg );
        if( !WRSetWinInfo( hDlg, siinfo ) ) {
            EndDialog( hDlg, FALSE );
        }
        ret = true;
        break;

    case WM_SYSCOLORCHANGE:
        WRCtl3dColorChange();
        break;

    case WM_COMMAND:
        siinfo = (WRSelectImageInfo *)GET_DLGDATA( hDlg );
        switch( LOWORD( wParam ) ) {
        case IDM_SELIMGHELP:
            if( siinfo != NULL && siinfo->help_callback != NULL ) {
                siinfo->help_callback();
            }
            break;

        case IDOK:
            if( siinfo == NULL ) {
                EndDialog( hDlg, FALSE );
                ret = true;
            } else if( WRGetWinInfo( hDlg, siinfo ) ) {
                EndDialog( hDlg, TRUE );
                ret = true;
            }
            break;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            ret = true;
            break;

        case IDM_SELIMGBMP:
            if( GET_WM_COMMAND_CMD( wParam, lParam ) != BN_CLICKED ) {
                break;
            }
            if( siinfo->type != RESOURCE2INT( RT_BITMAP ) ) {
                siinfo->type = RESOURCE2INT( RT_BITMAP );
                WRSetEntries( hDlg, siinfo );
            }
            break;

        case IDM_SELIMGCUR:
            if( GET_WM_COMMAND_CMD( wParam, lParam ) != BN_CLICKED ) {
                break;
            }
            if( siinfo->type != RESOURCE2INT( RT_GROUP_CURSOR ) ) {
                siinfo->type = RESOURCE2INT( RT_GROUP_CURSOR );
                WRSetEntries( hDlg, siinfo );
            }
            break;

        case IDM_SELIMGICO:
            if( GET_WM_COMMAND_CMD( wParam, lParam ) != BN_CLICKED ) {
                break;
            }
            if( siinfo->type != RESOURCE2INT( RT_GROUP_ICON ) ) {
                siinfo->type = RESOURCE2INT( RT_GROUP_ICON );
                WRSetEntries( hDlg, siinfo );
            }
            break;
        }
        break;
    }

    return( ret );
}
