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


#include "precomp.h"
#include "wreglbl.h"
#include "wremain.h"
#include "wreresin.h"
#include "wreseted.h"
#include "wreprop.h"
#include "wrdll.h"
#include "prop.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WREPropertyInfo {
    WResID      *name;
    WResID      *new_name;
    char        *symbol;
    char        *new_symbol;
    WRHashTable *symbol_table;
    uint_16     mflags;
    uint_16     new_mflags;
} WREPropertyInfo;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL WREPropertyProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

static Bool WREGetPropName( HWND hDlg, WREProperyInfo *info )
{
    char        *name;
    HWND        combo;
    WRHashEntry *entry;

    if( info == NULL ) {
        return( FALSE );
    }

    combo = GetDlgItem( hDlg, IDM_PROP_NAME );

    name = WREGetStrFromCombo( combo );

    if( name == NULL ) {
        return( FALSE );
    }

    WRStripSymbol( name );

    if( WRIsValidSymbol( name ) ) {
        entry = WRFindHashEntryFromName( info->symbol_table, name );
    }

    if( entry != NULL ) {
        info->new_symbol = WREStrDup( entry->name );
    }

    if( name != NULL ) {
        WREMemFree( name );
    }

    return( TRUE );
}

static void WRESetPropName( HWND hDlg, WREProperyInfo *info )
{
    if( info == NULL ) {
        return;
    }
}

static void WRESetPropMemoryFlags( HWND hDlg, uint_16 mflags )
{
    if( mflags & MEMFLAG_MOVEABLE ) {
        CheckDlgButton( hDlg, IDM_PROP_MV, 1 );
    }

    if( mflags & MEMFLAG_DISCARDABLE ) {
        CheckDlgButton( hDlg, IDM_PROP_DSC, 1 );
    }

    if( mflags & MEMFLAG_PURE ) {
        CheckDlgButton( hDlg, IDM_PROP_PUR, 1 );
    }

    if( mflags & MEMFLAG_PRELOAD ) {
        CheckDlgButton( hDlg, IDM_PROP_PRE, 1 );
    } else {
        CheckDlgButton( hDlg, IDM_PROP_LOC, 1 );
    }
}

static void WREGetPropMemoryFlags( HWND hDlg, uint_16 *mflags )
{
    if( mflags == NULL ) {
        return;
    }

    *mflags &= ~(MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE | MEMFLAG_PURE | MEMFLAG_PRELOAD);

    if( IsDlgButtonChecked( hDlg, IDM_PROP_MV ) ) {
        *mflags |= MEMFLAG_MOVEABLE;
    }

    if( IsDlgButtonChecked( hDlg, IDM_PROP_DSC ) ) {
        *mflags |= MEMFLAG_DISCARDABLE;
    }

    if( IsDlgButtonChecked( hDlg, IDM_PROP_PUR ) ) {
        *mflags |= MEMFLAG_PURE;
    }

    if( IsDlgButtonChecked( hDlg, IDM_PROP_PRE ) ) {
        *mflags |= MEMFLAG_PRELOAD;
    }
}

BOOL WREPropertyProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WREProperyInfo      *info;
    BOOL                ret;

    ret = FALSE;

    switch( message ) {
    case WM_INITDIALOG:
        info = (WREProperyInfo *)lParam;
        SET_DLGDATA( hDlg, (LONG_PTR)info );
        WRESetPropMemoryFlags( hDlg, info->mflags )
        ret = TRUE;
        break;

    case WM_SYSCOLORCHANGE:
        WRECtl3dColorChange();
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDM_PROP_HELP:
            WREHelpRoutine();
            break;

        case IDOK:
            info = (WREProperyInfo *)GET_DLGDATA( hDlg );
            WREGetPropMemoryFlags( hDlg, info->new_mflags );
            EndDialog( hDlg, TRUE );
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
