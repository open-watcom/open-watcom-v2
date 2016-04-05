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
* Description:  Display and stress memory allocation.
*
****************************************************************************/


#include <ctype.h>
#include "heapwalk.h"
#include <stress.h>
#include "jdlg.h"


/* Local Window callback functions prototypes */
BOOL __export FAR PASCAL FreeNDlgProc( HWND hwnd, WORD msg, WORD wparam, DWORD lparam );

static DWORD    FreeAmt;
static DWORD    AllocAmt;
static WORD     DialMode;

static char AmtTable[] = { 1, 2, 5, 10, 25, 50, 0 };

static void UpdateAllocMenu( void ) {

    HMENU       mh;
    WORD        i;
    WORD        cmd;

    /* the first time we come in here is before the main window has
     * been created so in that case don't do anything */

    if( !IsWindow( HeapWalkMainWindow ) ) return;
    mh = GetMenu( HeapWalkMainWindow );
    if( AllocAmt == 0 ) {
        cmd = MF_BYCOMMAND | MF_GRAYED;
        i = HEAPMENU_ALLOC_FIRST;
        while( i <= HEAPMENU_ALLOC_LAST ) {
            EnableMenuItem( mh, i, cmd );
            i++;
        }
    } else {
        i = 0;
        while( AmtTable[i] != 0 ) {
            if( FreeAmt >= AmtTable[i] * 1024L ) {
                EnableMenuItem( mh, HEAPMENU_ALLOC_FIRST + i,
                                MF_BYCOMMAND | MF_ENABLED );
            } else {
                EnableMenuItem( mh, HEAPMENU_ALLOC_FIRST + i,
                                MF_BYCOMMAND | MF_GRAYED );
            }
            i++;
        }
        if( FreeAmt > 0 ) {
            EnableMenuItem( mh, HEAPMENU_ALLOC_NK, MF_BYCOMMAND | MF_ENABLED );
        } else {
            EnableMenuItem( mh, HEAPMENU_ALLOC_NK, MF_BYCOMMAND | MF_GRAYED );
        }
        cmd = MF_BYCOMMAND | MF_ENABLED;
    }
    i = HEAPMENU_FREE_FIRST;
    while( i <= HEAPMENU_FREE_LAST ) {
        EnableMenuItem( mh, i, cmd );
        i++;
    }
} /* UpdateAllocMenu */


void UpdateAllocInfo( HWND hwnd ) {

    char        buf[20];

    sprintf( buf, "%lu", FreeAmt / 1024L );
    SetStaticText( hwnd, ALLOC_FREE_K, buf );
    sprintf( buf, "%lu", FreeAmt );
    SetStaticText( hwnd, ALLOC_FREE_BYTE, buf );
    sprintf( buf, "%lu", AllocAmt / 1024L );
    SetStaticText( hwnd, ALLOC_ALLOCATED, buf );
    UpdateAllocMenu();
} /* UpdateAllocInfo */

static BOOL MyFreeMem( DWORD amt ) {
    FreeAmt += amt;
    if( !AllocMem( FreeAmt ) ) {
        FreeAmt -= amt;
        return( FALSE );
    }
    AllocAmt -= amt;
    return( TRUE );
} /* MyFreeMem */

static BOOL MyAllocMem( DWORD amt ) {
    if( amt > FreeAmt ) return( FALSE );
    AllocAmt += amt;
    FreeAmt -= amt;
    AllocMem( FreeAmt );
    return( TRUE );
} /* MyallocMem */

static BOOL MyAllocAllBut( DWORD amt ) {

    FreeAllMem();
    AllocAmt = GetFreeSpace( 0 );
    if( !AllocMem( amt ) ) {
        FreeAmt = AllocAmt;
        AllocAmt = 0;
        return( FALSE );
    }
    FreeAmt = amt;
    AllocAmt -= FreeAmt;
    return( TRUE );
} /* MyAllocAllBut */

BOOL FAR PASCAL AllocDlgProc( HWND hwnd, WORD msg, WORD wparam, DWORD lparam )
{
    HWND        parent;
    RECT        area;

    lparam = lparam;
    wparam = wparam;
    switch( msg ) {
    case WM_INITDIALOG:
        parent = (HWND)GetWindowWord( hwnd, GWW_HWNDPARENT );
        GetClientRect( parent, &area );
        SetWindowPos( hwnd, NULL, -area.left, -area.top, 0, 0,
                      SWP_NOSIZE | SWP_NOZORDER );
        FreeAmt = GetFreeSpace( 0 );
        AllocAmt = 0;
        UpdateAllocInfo( hwnd );
        break;
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
    case WM_CLOSE:
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
} /* AllocDlgProc */

static DWORD ParseAmount( char *buf ) {

    char        *ptr;
    BOOL        in_k;
    char        *end;
    DWORD       ret;

    ptr = buf;
    if( *ptr == '\0' ) return( -1 );
    while( *ptr ) ptr++;
    ptr --;
    while( isspace( *ptr ) ) ptr--;
    if( *ptr == 'k' || *ptr == 'K' ){
        in_k = TRUE;
        *ptr = '\0';
        ptr --;
    } else {
        in_k = FALSE;
    }
    while( isspace( *ptr ) ) ptr--;
    ptr++;
    ret = strtoul( buf, &end, 0 );
    if( end != ptr ) return( -1 );
    if( in_k ) ret *= 1024;
    return( ret );
} /* ParseAmount */


BOOL FAR PASCAL FreeNDlgProc( HWND hwnd, WORD msg, WORD wparam, DWORD lparam )
{
    char        buf[256];
    DWORD       amt;
    char        *str;

    switch( msg ) {
    case WM_INITDIALOG:
        CenterDlg( hwnd );
        sprintf( buf, "%lu", FreeAmt / 1024L );
        SetStaticText( hwnd, FREE_FREE_K, buf );
        sprintf( buf, "%lu", FreeAmt );
        SetStaticText( hwnd, FREE_FREE_BYTE, buf );
        sprintf( buf, "%lu", AllocAmt );
        SetStaticText( hwnd, FREE_ALLOCATED, buf );
        switch( DialMode ) {
        case HEAPMENU_FREE_NK:
            str = HWGetRCString( STR_FREE_N_BYTES );
            SetWindowText( hwnd, str );
            str = HWGetRCString( STR_BYTES_TO_FREE );
            SetDlgItemText( hwnd, FREE_AMT_TITLE, str );
            break;
        case HEAPMENU_ALLOC_NK:
            str = HWGetRCString( STR_ALLOC_N_BYTES );
            SetWindowText( hwnd, str );
            str = HWGetRCString( STR_BYTES_TO_ALLOC );
            SetDlgItemText( hwnd, FREE_AMT_TITLE, str );
            break;
        case HEAPMENU_ALLOC_BUT_NK:
            str = HWGetRCString( STR_ALLOC_ALL_BUT_N );
            SetWindowText( hwnd, str );
            str = HWGetRCString( STR_BYTES_TO_LEAVE_FREE );
            SetDlgItemText( hwnd, FREE_AMT_TITLE, str );
            break;
        }
        break;
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
    case WM_COMMAND:
        if( HIWORD( lparam ) == BN_CLICKED ) {
            switch( wparam ) {
            case FREE_OK:
                GetDlgItemText( hwnd, FREE_AMOUNT, buf, 40 );
                amt = ParseAmount( buf );
                if( amt == -1 ) {
                    GetWindowText( hwnd, buf, sizeof( buf ) );
                    RCMessageBox( hwnd, STR_AMT_ENTERED_INVALID,
                                buf, MB_OK | MB_ICONEXCLAMATION );
                    break;
                }
                switch( DialMode ) {
                case HEAPMENU_FREE_NK:
                    if( !MyFreeMem( amt ) ) {
                        GetWindowText( hwnd, buf, sizeof( buf ) );
                        RCMessageBox( hwnd, STR_CANT_FREE_REQ_AMT,
                                       buf, MB_OK | MB_ICONEXCLAMATION );
                    }
                    break;
                case HEAPMENU_ALLOC_NK:
                    if( !MyAllocMem( amt ) ) {
                        GetWindowText( hwnd, buf, sizeof( buf ) );
                        RCMessageBox( hwnd, STR_CANT_ALLOC_REQ_AMT,
                                       buf, MB_OK | MB_ICONEXCLAMATION );
                    }
                    break;
                case HEAPMENU_ALLOC_BUT_NK:
                    if( !MyAllocAllBut( amt ) ) {
                        GetWindowText( hwnd, buf, sizeof( buf ) );
                        RCMessageBox( hwnd, STR_CANT_ALLOC_REQ_AMT,
                                       buf, MB_OK | MB_ICONEXCLAMATION );
                    }
                    break;
                }
                EndDialog( hwnd, TRUE );
                break;
            case FREE_CANCEL:
                EndDialog( hwnd, FALSE );
                break;
            }
        }
        break;
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}


/***************** interface routines ****************************/

void DoNBytes( HWND parent, WORD type ) {

    FARPROC             fp;
    INT_PTR             ret;

    DialMode = type;
    fp = MakeProcInstance( (FARPROC)FreeNDlgProc, Instance );
    if( fp != NULL ) {
        ret = JDialogBox( Instance, "FREE_N_DLG", parent, (DLGPROC)fp );
        if( ret != -1 ) {
            FreeProcInstance( fp );
            return;
        }
    }
    /* there's not enough memory to do the dialog
       so free some memory so we can do it */
    FreeAllMem();
    fp = MakeProcInstance( (FARPROC)FreeNDlgProc, Instance );
    ret = JDialogBox( Instance, "FREE_N_DLG", parent, (DLGPROC)fp );
    FreeProcInstance( fp );
    AllocMem( FreeAmt );
}


void AllocAllBut( WORD type ) {

    DWORD       amt;

    if( type == HEAPMENU_ALLOC_ALL ) {
        amt = 0;
    } else {
        amt = AmtTable[type - HEAPMENU_ALLOC_BUT_FIRST] * 1024L;
    }
    MyAllocAllBut( amt );
}

void AllocMore( WORD type ) {

    DWORD       amt;

    amt = AmtTable[type - HEAPMENU_ALLOC_FIRST] * 1024L;
    MyAllocMem( amt );
}

void FreeSomeMem( WORD type ) {

    DWORD       amt;

    amt = AmtTable[type - HEAPMENU_FREE_FIRST] * 1024L;
    MyFreeMem( amt );
}

void MyFreeAllMem( void ) {
    FreeAllMem();
    FreeAmt = GetFreeSpace( 0 );
    AllocAmt = 0;
} /* FreeAllMem */
