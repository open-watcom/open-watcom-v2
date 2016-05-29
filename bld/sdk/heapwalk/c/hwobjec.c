/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Global object dialogs.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "heapwalk.h"
#include "jdlg.h"


/* Local Window callback functions prototypes */
BOOL __export FAR PASCAL AddDlgProc( HWND hwnd, WORD msg, WORD wparam, DWORD lparam );

/* static variables used by the add function */
static FARPROC  DialProc;
static WORD     AddCount;
static DWORD    AddTotal;

void ShowSelector( HWND list ) {

    int         index;
    char        buf[100];
    WORD        sel;
    char        *msgtitle;

    index = (int)SendMessage( list, LB_GETCURSEL, 0, 0L );
    if( index == LB_ERR ) {
        msgtitle = HWAllocRCString( STR_GET_SELECTOR );
        RCMessageBox( HeapWalkMainWindow, STR_NO_ITEM_SELECTED,
                      msgtitle, MB_OK | MB_ICONEXCLAMATION );
        HWFreeRCString( msgtitle );
        return;
    }
    sel = GlobalHandleToSel( HeapList[index]->info.ge.hBlock );
    if( sel == 0 || HeapList[index]->info.ge.hBlock == 0 ) {
        RCsprintf( buf, STR_CANT_GET_SELECTOR,
                   HeapList[index]->info.ge.hBlock );
    } else {
        RCsprintf( buf, STR_SELECTOR_FOR_HANDLE, sel,
                   HeapList[index]->info.ge.hBlock );
    }
    msgtitle = HWAllocRCString( STR_GET_SELECTOR );
    MessageBox( HeapWalkMainWindow, buf, msgtitle, MB_OK );
    HWFreeRCString( msgtitle );
}

BOOL GlobDiscardObj( HWND list ) {

    int         index;
    char        buf[100];
    char        *msgtitle;

    index = (int)SendMessage( list, LB_GETCURSEL, 0, 0L );
    if( index == LB_ERR ) {
        msgtitle = HWAllocRCString( STR_DISCARD );
        RCMessageBox( HeapWalkMainWindow, STR_NO_ITEM_SELECTED,
                      msgtitle, MB_OK | MB_ICONEXCLAMATION );
        HWFreeRCString( msgtitle );
        return( FALSE );
    }
    if( GlobalDiscard( HeapList[index]->info.ge.hBlock ) == 0 ) {
        msgtitle = HWAllocRCString( STR_DISCARD );
        RCMessageBox( HeapWalkMainWindow, STR_CANT_DISCARD_ITEM,
                    msgtitle, MB_OK | MB_ICONINFORMATION );
        HWFreeRCString( msgtitle );
        return( FALSE );
    } else {
        RCsprintf( buf, STR_HDL_DISCARDED, HeapList[index]->info.ge.hBlock,
                 HeapList[index]->info.ge.dwBlockSize );
        msgtitle = HWAllocRCString( STR_DISCARD );
        MessageBox( HeapWalkMainWindow, buf, msgtitle, MB_OK );
        HWFreeRCString( msgtitle );
        return( TRUE );
    }
}

BOOL GlobSetObjPos( HWND list, BOOL oldest ) {

    int                 index;
    GLOBALENTRY         ge;
    GLOBALENTRY         *item;
    BOOL                is_newest;
    char                buf[160];
    msg_id              strid;

    index = (int)SendMessage( list, LB_GETCURSEL, 0, 0L );
    if( index == LB_ERR ) {
        RCMessageBox( HeapWalkMainWindow, STR_NO_ITEM_SELECTED,
                    HeapWalkName, MB_OK | MB_ICONEXCLAMATION );
        return( FALSE );
    }
    item = &( HeapList[index]->info.ge );
    /* see if the object can be moved */
    memset( &ge, 0, sizeof( GLOBALENTRY ) );
    ge.dwSize = sizeof( GLOBALENTRY );
    GlobalFirst( &ge, GLOBAL_LRU );
    if( ge.hBlock == item->hBlock ) {
        is_newest = TRUE;
        GlobalLRUOldest( item->hBlock );
    } else {
        is_newest = FALSE;
        GlobalLRUNewest( item->hBlock );
    }
    memset( &ge, 0, sizeof( GLOBALENTRY ) );
    ge.dwSize = sizeof( GLOBALENTRY );
    GlobalFirst( &ge, GLOBAL_LRU );
    if( ( is_newest && ge.hBlock == item->hBlock )
                || !is_newest && ge.hBlock != item->hBlock ) {
        RCsprintf( buf, STR_CANT_REPOSITION, item->hBlock );
        MessageBox( HeapWalkMainWindow, buf, HeapWalkName,
                    MB_OK | MB_ICONINFORMATION );
        return( FALSE );
    } else if( is_newest && !oldest) {
        GlobalLRUNewest( HeapList[index]->info.ge.hBlock );
    } else if( !is_newest && oldest ) {
        GlobalLRUOldest( HeapList[index]->info.ge.hBlock );
    }
    if( oldest ) {
        strid = STR_HDL_IS_NOW_OLDEST;
    } else {
        strid = STR_HDL_IS_NOW_NEWEST;
    }
    RCsprintf( buf, strid, item->hBlock );
    MessageBox( HeapWalkMainWindow, buf, HeapWalkName, MB_OK );
    return( TRUE );
}


BOOL FAR PASCAL AddDlgProc( HWND hwnd, WORD msg, WORD wparam, DWORD lparam )
{
    HWND        parent;
    RECT        area;

    wparam = wparam;
    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        SetStaticText( hwnd, ADD_CNT, "0" );
        SetStaticText( hwnd, ADD_TOTAL, "0" );
        parent = (HWND)GetWindowWord( hwnd, GWW_HWNDPARENT );
        GetClientRect( parent, &area );
        SetWindowPos( hwnd, NULL, -area.left, -area.top, 0, 0,
                      SWP_NOSIZE | SWP_NOZORDER );
        break;
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
    case WM_COMMAND:
        if( wparam == ADD_OK && HIWORD( lparam ) == BN_CLICKED ) {
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
        } else {
            return( FALSE );
        }
        break;
    case WM_CLOSE:
        DestroyWindow( hwnd );
        break;
    case WM_NCDESTROY:
        EndAdd();
        FreeProcInstance( DialProc );
        return( FALSE ); /* we need to let WINDOWS see this message or
                            fonts are left undeleted */
    default:
        return( FALSE );
    }
    return( TRUE );
} /* AddDlgProc */

void SetMenusForAdd( HWND hwnd, BOOL start ) {

    HMENU       mh;
    WORD        flags;

    if( start ) {
        flags = MF_BYCOMMAND | MF_GRAYED;
    } else {
        flags = MF_BYCOMMAND | MF_ENABLED;
    }
    mh = GetMenu( hwnd );
    EnableMenuItem( mh, HEAPMENU_DISPLAY_ENTIRE, flags );
    EnableMenuItem( mh, HEAPMENU_DISPLAY_LRU, flags );
    EnableMenuItem( mh, HEAPMENU_DISPLAY_FREE, flags );
    EnableMenuItem( mh, HEAPMENU_DISPLAY_DPMI, flags );

    EnableMenuItem( mh, HEAPMENU_SORT_ADDR, flags );
    EnableMenuItem( mh, HEAPMENU_SORT_HANDLE, flags );
    EnableMenuItem( mh, HEAPMENU_SORT_MODULE, flags );
    EnableMenuItem( mh, HEAPMENU_SORT_SIZE, flags );
    EnableMenuItem( mh, HEAPMENU_SORT_TYPE, flags );
    EnableMenuItem( mh, HEAPMENU_SORT_EXTEND, flags );

    EnableMenuItem( mh, HEAPMENU_FILE_SAVE, flags );
    EnableMenuItem( mh, HEAPMENU_FILE_SAVE_TO, flags );

    EnableMenuItem( mh, HEAPMENU_OBJECT_DISCARD, flags );
    EnableMenuItem( mh, HEAPMENU_OBJECT_OLDEST, flags );
    EnableMenuItem( mh, HEAPMENU_OBJECT_NEWEST, flags );
    EnableMenuItem( mh, HEAPMENU_ADD, flags );

    EnableMenuItem( mh, HEAPMENU_GLOBAL_COMPACT, flags );
    EnableMenuItem( mh, HEAPMENU_GLOBAL_COMP_DISC, flags );
    EnableMenuItem( mh, HEAPMENU_GLOBAL_REFRESH, flags );
} /* SetMenusForAdd */

HWND StartAdd( HWND parent, ListBoxInfo *info ) {

    HWND        dialog;

    AddCount = 0;
    AddTotal = 0;
    DialProc = MakeProcInstance( (FARPROC)AddDlgProc, Instance );
    if( DialProc != NULL ) {
        dialog = JCreateDialog( Instance, "ADD_DLG", parent , (DLGPROC)DialProc );
        if( dialog != NULL ) {
            SetMenusForAdd( parent, TRUE );
            SetListBoxForAdd( info->box, TRUE );
        }
        return( dialog );
    }
    return( NULL );
} /* StartAdd */

void RefreshAdd( HWND dialog, HWND lbhwnd ) {
    int         *items;
    int         cnt;
    DWORD       total;
    int         i;
    char        buf[100];

    total = 0;
    cnt = (int)SendMessage( lbhwnd, LB_GETSELCOUNT, 0, 0L );
    items = MemAlloc( cnt * sizeof( int ) );
    if( cnt != 0 ) {
        if( items == NULL ) {
            ErrorBox( HeapWalkMainWindow, STR_CANT_COMPLETE_ADD, MB_OK | MB_ICONINFORMATION );
            return;
        }
        SendMessage( lbhwnd, LB_GETSELITEMS, cnt, (LPARAM)items );
        for( i = 0; i < cnt; i++ ) {
            total += HeapList[items[i]]->info.ge.dwBlockSize;
        }
    }
    sprintf( buf, "%d", cnt );
    SetStaticText( dialog, ADD_CNT, buf );
    sprintf( buf, "%lu", total );
    SetStaticText( dialog, ADD_TOTAL, buf );
}

BOOL FAR PASCAL SetCodeDlgProc( HWND hwnd, WORD msg, WORD wparam, DWORD lparam )
{
    DWORD       size;
    DWORD       info;
    char        buf[40];
    char        *end;
    char        *last;
    const char  *str;

    switch( msg ) {
    case WM_INITDIALOG:
        CenterDlg( hwnd );
        info = SetSwapAreaSize( 0 );
        str = HWGetRCString( STR_VALUE_K );
        sprintf( buf, str, LOWORD( info ) / 64 );
        SetStaticText( hwnd, CODE_CUR_SIZE, buf );
        sprintf( buf, str, HIWORD( info ) / 64 );
        SetStaticText( hwnd, CODE_MAX_SIZE, buf );
        break;
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
    case WM_COMMAND:
        if( HIWORD( lparam ) == BN_CLICKED ) {
            switch( wparam ) {
            case CODE_OK:
                GetDlgItemText( hwnd, CODE_VALUE, buf, 40 );
                end = buf;
                while( *end ) end++;
                if( end != buf ) {
                    end --;
                    while( isspace( *end ) ) end--;
                    end++;
                }
                size = strtoul( buf, &last, 0 );
                if( end != last || end == buf ) {
                    RCMessageBox( hwnd, STR_INVALID_SIZE, HeapWalkName,
                                MB_OK | MB_ICONEXCLAMATION );
                    break;
                }
                info = SetSwapAreaSize( 0 );
                size *= 64;
                if( size > HIWORD( info ) ) {
                    RCMessageBox( hwnd, STR_SIZE_TOO_LARGE, HeapWalkName,
                                MB_OK | MB_ICONEXCLAMATION );
                    break;
                }
                SetSwapAreaSize( size );
                EndDialog( hwnd, TRUE );
                break;
            case CODE_CANCEL:
                EndDialog( hwnd, FALSE );
                break;
            }
        }
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
} /* SetCodeDlgProc */
