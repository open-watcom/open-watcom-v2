/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#define INCLUDE_COMMDLG_H
#include "wrglbl.h"
#include "wrmaini.h"
#include "wrdmsgi.h"
#include "memflags.rh"
#include "jdlg.h"
#include "winexprt.h"
#include "wclbproc.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WRMFInfo {
    HELPFUNC        help_callback;
    uint_16         mflags;
    char            *name;
} WRMFInfo;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT extern INT_PTR CALLBACK WRMemFlagsDlgProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void         WRSetWinInfo( HWND, WRMFInfo * );
static void         WRGetWinInfo( HWND, WRMFInfo * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

bool WRAPI WRChangeMemFlags( HWND parent, char *name, uint_16 *mflags, HELPFUNC help_callback )
{
    WRMFInfo    info;
    DLGPROC     dlgproc;
    HINSTANCE   inst;
    INT_PTR     modified;

    if( mflags == NULL ) {
        return( false );
    }

    info.help_callback = help_callback;
    info.name = name;
    info.mflags = *mflags;
    inst = WRGetInstance();

    dlgproc = MakeProcInstance_DLG( WRMemFlagsDlgProc, inst );

    modified = JDialogBoxParam( inst, "WRMemFlags", parent, dlgproc, (LPARAM)(LPVOID)&info );

    FreeProcInstance_DLG( dlgproc );

    if( modified == IDOK ) {
        *mflags = info.mflags;
    }

    return( modified != -1 && modified == IDOK );
}

void WRSetWinInfo( HWND hDlg, WRMFInfo *info )
{
    if( info != NULL ) {
        SendDlgItemMessage( hDlg, IDM_MFNAME, WM_SETTEXT, 0, (LPARAM)(LPCSTR)info->name );

        if( info->mflags & MEMFLAG_MOVEABLE ) {
            CheckDlgButton( hDlg, IDM_MFMV, BST_CHECKED );
        }

        if( info->mflags & MEMFLAG_DISCARDABLE ) {
            CheckDlgButton( hDlg, IDM_MFDSC, BST_CHECKED );
        }

        if( info->mflags & MEMFLAG_PURE ) {
            CheckDlgButton( hDlg, IDM_MFPUR, BST_CHECKED );
        }

        if( info->mflags & MEMFLAG_PRELOAD ) {
            CheckDlgButton( hDlg, IDM_MFPRE, BST_CHECKED );
        } else {
            CheckDlgButton( hDlg, IDM_MFLOC, BST_CHECKED );
        }
    }
}

void WRGetWinInfo( HWND hDlg, WRMFInfo *info )
{
    if( info != NULL ) {
        info->mflags = info->mflags &
            ~(MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE | MEMFLAG_PURE | MEMFLAG_PRELOAD);

        if( IsDlgButtonChecked( hDlg, IDM_MFMV ) ) {
            info->mflags |= MEMFLAG_MOVEABLE;
        }

        if( IsDlgButtonChecked( hDlg, IDM_MFDSC ) ) {
            info->mflags |= MEMFLAG_DISCARDABLE;
        }

        if( IsDlgButtonChecked( hDlg, IDM_MFPUR ) ) {
            info->mflags |= MEMFLAG_PURE;
        }

        if( IsDlgButtonChecked( hDlg, IDM_MFPRE ) ) {
            info->mflags |= MEMFLAG_PRELOAD;
        }
    }
}

WINEXPORT INT_PTR CALLBACK WRMemFlagsDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WRMFInfo    *info;
    bool        ret;

    ret = false;

    switch( message ) {
    case WM_DESTROY:
        WRUnregisterDialog( hDlg );
        break;

    case WM_INITDIALOG:
        info = (WRMFInfo *)lParam;
        SET_DLGDATA( hDlg, info );
        WRRegisterDialog( hDlg );
        WRSetWinInfo( hDlg, info );
        ret = true;
        break;

    case WM_SYSCOLORCHANGE:
        WRCtl3dColorChange();
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDM_MFHELP:
            info = (WRMFInfo *)GET_DLGDATA( hDlg );
            if( info != NULL && info->help_callback != NULL ) {
                info->help_callback();
            }
            break;

        case IDOK:
            info = (WRMFInfo *)GET_DLGDATA( hDlg );
            if( info != NULL ) {
                WRGetWinInfo( hDlg, info );
                EndDialog( hDlg, TRUE );
            } else {
                EndDialog( hDlg, FALSE );
            }
            ret = true;
            break;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            ret = true;
            break;
        }
        break;
    }

    return( ret );
}
