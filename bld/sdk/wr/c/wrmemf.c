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


#include <windows.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wrmaini.h"
#include "wrdmsgi.h"
#include "memflags.h"
#include "jdlg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WRMFInfo {
    FARPROC     hcb;
    uint_16     mflags;
    char        *name;
} WRMFInfo;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT extern BOOL CALLBACK WRMemFlagsProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void         WRSetWinInfo( HWND, WRMFInfo * );
static void         WRGetWinInfo( HWND, WRMFInfo * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

int WRAPI WRChangeMemFlags( HWND parent, char *name, uint_16 *mflags, FARPROC hcb )
{
    WRMFInfo    info;
    DLGPROC     proc;
    HINSTANCE   inst;
    BOOL        modified;

    if( mflags == NULL ) {
        return( FALSE );
    }

    info.hcb = hcb;
    info.name = name;
    info.mflags = *mflags;
    inst = WRGetInstance();

    proc = (DLGPROC)MakeProcInstance( (FARPROC)WRMemFlagsProc, inst );

    modified = JDialogBoxParam( inst, "WRMemFlags", parent, proc, (LPARAM)&info );

    FreeProcInstance( (FARPROC)proc );

    if( modified == IDOK ) {
        *mflags = info.mflags;
    }

    return( modified != -1 && modified == IDOK );
}

void WRSetWinInfo( HWND hDlg, WRMFInfo *info )
{
    if( info != NULL ) {
        SendDlgItemMessage( hDlg, IDM_MFNAME, WM_SETTEXT, 0, (LPARAM)(LPSTR)info->name );

        if( info->mflags & MEMFLAG_MOVEABLE ) {
            CheckDlgButton( hDlg, IDM_MFMV, 1 );
        }

        if( info->mflags & MEMFLAG_DISCARDABLE ) {
            CheckDlgButton( hDlg, IDM_MFDSC, 1 );
        }

        if( info->mflags & MEMFLAG_PURE ) {
            CheckDlgButton( hDlg, IDM_MFPUR, 1 );
        }

        if( info->mflags & MEMFLAG_PRELOAD ) {
            CheckDlgButton( hDlg, IDM_MFPRE, 1 );
        } else {
            CheckDlgButton( hDlg, IDM_MFLOC, 1 );
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

WINEXPORT BOOL CALLBACK WRMemFlagsProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WRMFInfo    *info;
    BOOL        ret;

    ret = FALSE;

    switch( message ) {
    case WM_DESTROY:
        WRUnregisterDialog( hDlg );
        break;

    case WM_INITDIALOG:
        info = (WRMFInfo *)lParam;
        SET_DLGDATA( hDlg, (LONG_PTR)info );
        WRRegisterDialog( hDlg );
        WRSetWinInfo( hDlg, info );
        ret = TRUE;
        break;

    case WM_SYSCOLORCHANGE:
        WRCtl3dColorChange();
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDM_MFHELP:
            info = (WRMFInfo *)GET_DLGDATA( hDlg );
            if( info != NULL && info->hcb != NULL ) {
                (*info->hcb)();
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
            ret = TRUE;
            break;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            ret = TRUE;
            break;
        }
        break;
    }

    return( ret );
}
