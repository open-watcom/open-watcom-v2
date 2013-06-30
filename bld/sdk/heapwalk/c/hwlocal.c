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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "heapwalk.h"

typedef struct {
    LOCALENTRY          top;
    LOCALENTRY          sel;
    BOOL                valid_sel;
} LocalStateStruct;

static HGLOBAL          LocalHeapHandle;
static char             Owner[MAX_MODULE_NAME +1];
static HWND             LocalHeapHwnd;

LOCALENTRY      **LocalHeapList;
unsigned        LocalHeapCount;

extern msglist Sort_types[];
extern msglist LocalNormMsg[];
extern msglist LocalUSERMsg[];
extern msglist LocalGDIMsg[];
extern msglist FlagMsg[];


/*
 * PutOutLocalHeader
 */
static void PutOutLocalHeader( FILE *fptr ) {

    time_t      tm;
    LclInfo     info;
    char        *sorttype;

    tm = time( NULL );
    LclHeapInfo( &info );

    RCfprintf( fptr, STR_SNAP_LCL_CREATED, asctime( localtime( &tm ) ) );
    RCfprintf( fptr, STR_LCL_HEAP_OF, Owner );
    sorttype = SrchMsg( LSortType, Sort_types, "" );
    RCfprintf( fptr, STR_LCL_SORTED_BY, sorttype );
    RCfprintf( fptr, STR_LCL_SUMMARY_HEADING );
    RCfprintf( fptr, STR_LCL_FREE_OBJECTS, info.free_count, info.free_size );
    RCfprintf( fptr, STR_LCL_MOVEABLE_OBJECTS,
               info.movable_count, info.movable_size );
    RCfprintf( fptr, STR_LCL_FIXED_OBJECTS, info.fixed_count, info.fixed_size );
    RCfprintf( fptr, STR_LCL_TOTAL_LINE );
    RCfprintf( fptr, STR_LCL_TOTAL_OBJECTS, info.tot_count, info.tot_size );
    fprintf( fptr, "\n%s\n\n", HeapLocalTitles );
} /* PutOutLocalHeader */

/*
 * SortByLocalType
 * NB this routine assumes that p1 and p2 have the same wHeapType
 */

int SortByLocalType(  LOCALENTRY **p1, LOCALENTRY **p2 )
{

    char        *type1;
    char        *type2;

    switch( (*p1)->wHeapType ) {
    case NORMAL_HEAP:
        type1 = SrchMsg( (*p1)->wType, LocalNormMsg, "" );
        type2 = SrchMsg( (*p2)->wType, LocalNormMsg, "" );
        break;
    case USER_HEAP:
        type1 = SrchMsg( (*p1)->wType, LocalUSERMsg, "" );
        type2 = SrchMsg( (*p2)->wType, LocalUSERMsg, "" );
        break;
    case GDI_HEAP:
        type1 = SrchMsg( (*p1)->wType, LocalGDIMsg, "" );
        type2 = SrchMsg( (*p2)->wType, LocalGDIMsg, "" );
        break;
    }
    return( strcmp( type1, type2 ) );
}


/*
 * FreeLocalList - frees the local heap list
 */

static void FreeLocalList( void ) {

    unsigned    i;

    if( LocalHeapList == NULL ) return;
    for( i=0; i < LocalHeapCount; i++ ) {
        MemFree( LocalHeapList[i] );
    }
    MemFree( LocalHeapList );
    LocalHeapList = NULL;
}

/*
 * LclHeapInfo - return statistics about the local heap
 */

void LclHeapInfo( LclInfo *info ) {

    unsigned    index;

    memset( info, 0, sizeof( LclInfo ) );
    for( index=0; index < LocalHeapCount; index++ ) {
        switch( LocalHeapList[index]->wFlags ) {
        case LF_FREE:
            info->free_count ++;
            info->free_size += LocalHeapList[index]->wSize;
            break;
        case LF_MOVEABLE:
            info->movable_count ++;
            info->movable_size += LocalHeapList[index]->wSize;
            break;
        case LF_FIXED:
            info->fixed_count ++;
            info->fixed_size += LocalHeapList[index]->wSize;
            break;
        }
    }
    info->tot_count = info->free_count + info->movable_count + info->fixed_count;
    info->tot_size = info->free_size + info->movable_size + info->fixed_size;
}

/*
 * FormatLocalHeapListItem - format a single line of the local heap list
 */

BOOL FormatLocalHeapListItem( char *line, unsigned index ) {

    LOCALENTRY  *item;
    char        *flags;
    char        *type;

    if( index == LocalHeapCount ) return( FALSE );
    item = LocalHeapList[index];
    flags = SrchMsg( item->wFlags, FlagMsg, "" );
    switch( item->wHeapType ) {
    case NORMAL_HEAP:
        type = SrchMsg( item->wType, LocalNormMsg, "" );
        break;
    case USER_HEAP:
        type = SrchMsg( item->wType, LocalUSERMsg, "" );
        break;
    case GDI_HEAP:
        type = SrchMsg( item->wType, LocalGDIMsg, "" );
        break;
    }
    sprintf( line, "%04X    %04X  %6d %-9s %4d  %-20s",
             item->wAddress,
             (UINT)item->hHandle,
             item->wSize,
             flags,
             item->wcLock,
             type
          );
    return( TRUE );
}

/*
 * EnableLocalMenu - grey or enable the local menu in the main window
 */

static void EnableLocalMenu( BOOL enable )
{
    WORD                action;
    HMENU               mh;

    if( enable ) {
        action = MF_ENABLED;
    } else {
        action = MF_GRAYED;
    }
    mh = GetMenu( HeapWalkMainWindow );
    if( HeapType != HEAPMENU_DISPLAY_DPMI ) {
        EnableMenuItem( mh, HEAPMENU_LOCAL_LOCALWALK,  action);
        EnableMenuItem( mh, HEAPMENU_COMPACT_AND_LOCALWALK, action);
    }
    EnableMenuItem( mh, HEAPMENU_GDI_LOCALWALK,  action);
    EnableMenuItem( mh, HEAPMENU_USER_LOCALWALK, action);
} /* EnableLocalMenu */

/*
 * AddToLocalHeapList - add an entry the the local heap list
 */

static BOOL AddToLocalHeapList( LOCALENTRY *item, unsigned i ) {

    LocalHeapList[i] = MemAlloc( sizeof( LOCALENTRY ) );
    if( LocalHeapList[i] == NULL ) return( FALSE );
    *LocalHeapList[i] = *item;
    return( TRUE );
} /* AddToLocalHeapList */

/*
 * SaveLocalListState - save the top item and selected item in the
 *              list box so we can restore them later
 */
BOOL SaveLocalListState( HWND boxhwnd, LocalStateStruct *info ) {

    LRESULT     top, sel;

    top = SendMessage( boxhwnd, LB_GETTOPINDEX, 0, 0L );
    sel = SendMessage( boxhwnd, LB_GETCURSEL, 0, 0L );
    if( sel != LB_ERR && sel < LocalHeapCount ) {
        info->sel = *LocalHeapList[sel];
        info->valid_sel = TRUE;
    } else {
        info->valid_sel = FALSE;
    }
    if( top >= LocalHeapCount ) top = 0;
    info->top = *LocalHeapList[top];
    return( TRUE );
}

/*
 * ReDisplayLocalHeapList - dump heap list into list box
 */
static void ReDisplayLocalHeapList( HWND boxhwnd, LocalStateStruct *info ) {

    unsigned    i;
    WORD        addr;

    if( info == NULL ) {
        SendMessage( boxhwnd, LB_SETTOPINDEX, 0, 0 );
        SendMessage( boxhwnd, LB_SETCURSEL, -1, 0 );
    }
    SetBoxCnt( boxhwnd, LocalHeapCount );
    if( info != NULL ) {
        addr = info->top.wAddress;
        for( i=0; i < LocalHeapCount; i++ ) {
            if( LocalHeapList[i]->wAddress == addr ) {
                SendMessage( boxhwnd, LB_SETTOPINDEX, i, 0 );
            }
        }
    }
}

/*
 * InitLocalHeapList - create a local heap list
 */

static BOOL InitLocalHeapList( HWND hwnd, HWND listhdl, BOOL keeppos )
{
    LOCALINFO           info;
    LOCALENTRY          item;
    LocalStateStruct    state;
    unsigned            size;
    unsigned            lim;
    BOOL                ret;

    /* get number of elements */
    info.dwSize = sizeof( LOCALINFO );
    info.wcItems = 0;
    if( keeppos ) {
        SaveLocalListState( listhdl, &state );
    }
    FreeLocalList();
    for( ;; ) {
        if( LocalInfo( &info, LocalHeapHandle ) == 0 ) break;
        lim = info.wcItems;
        size = lim * sizeof( LOCALENTRY * );
        LocalHeapList = MemAlloc( size );
        if( LocalHeapList == NULL ) break;
        memset( &item, 0, sizeof( LOCALENTRY ) );
        item.dwSize = sizeof( LOCALENTRY );
        LocalHeapCount = 0;
        if( LocalFirst( &item, LocalHeapHandle ) == 0 ) break;
        for( ;; ) {
            ret = AddToLocalHeapList( &item, LocalHeapCount );
            if( ret == FALSE ) break;
            LocalHeapCount ++;
            if( LocalHeapCount == lim ) break;
            if( LocalNext( &item ) == 0 ) break;
        }
        if( ret == FALSE ) break;
        SortLocalHeapList( hwnd, LSortType );
        ReDisplayLocalHeapList( listhdl, keeppos ? &state:NULL );
        return( TRUE );
    }
    ErrorBox( NULL, STR_UNABLE_DISP_LCL_HEAP, MB_OK | MB_ICONINFORMATION );
    return( FALSE );
} /* InitLocalHeapList */

/*
 * CreateLocalPushWin - create the push windows for the local heap window
 */
static HWND *CreateLocalPushWin( HWND hwnd ) {
    HWND        *ret;

    ret = MakePushWin( hwnd, HeapLocalTitles, 6,
                     HEAPMENU_SORT_ADDR, HEAPMENU_SORT_HANDLE,
                     HEAPMENU_SORT_SIZE, 0, 0, HEAPMENU_SORT_TYPE );
    return( ret );
}

/*
 * LocalHeapProc - process messages from the local heap window
 */

BOOL __export FAR PASCAL LocalHeapProc( HWND hwnd, WORD msg, WORD wparam,
                                    DWORD lparam ) {
    LclWndInfo          *info;
    LocalStateStruct    state;

    info = (LclWndInfo *)GetWindowLong( hwnd, 0 );
    switch( msg ) {
    case WM_CREATE:
        info = MemAlloc( sizeof( LclWndInfo ) );
        if( info == NULL ) {
            ErrorBox( NULL, STR_LCL_HEAP_NO_MEM, MB_OK | MB_ICONINFORMATION );
        }
        memset( info, 0, sizeof( LclWndInfo ) );
        SetWindowLong( hwnd, 0, (DWORD)info );
        EnableLocalMenu( FALSE );
        info->list.title = CreateLocalPushWin( hwnd );
        CreateListBox( hwnd, &(info->list), LOCAL_LB );
//      SetTitle( HeapLocalTitles, info->list.title );
        if( !InitLocalHeapList( hwnd, info->list.box, FALSE ) ) {
            DestroyWindow( hwnd );
        }
        break;
    case WM_ACTIVATEAPP:
        if( wparam ) {
            if( !InitLocalHeapList( hwnd, info->list.box, TRUE ) ) {
                DestroyWindow( hwnd );
            } else {
                SaveLocalListState( info->list.box, &state );
                ReDisplayLocalHeapList( info->list.box, &state );
            }
        }
        break;
    case WM_SHOWWINDOW:
        if( IsWindow( info->dialog ) ) {
            if( !wparam && LOWORD( lparam ) == SW_PARENTCLOSING ) {
                ShowWindow( info->dialog, SW_HIDE );
            } else if( wparam && LOWORD( lparam ) == SW_PARENTOPENING ) {
                ShowWindow( info->dialog, SW_SHOWNOACTIVATE );
            }
        }
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    case WM_SIZE:
        ResizeListBox( LOWORD( lparam ), HIWORD( lparam ), &( info->list ) );
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
        break;
    case WM_COMMAND:
        switch( wparam ) {
        case HEAPMENU_GLOBAL_REFRESH:
            if( !InitLocalHeapList( hwnd, info->list.box, TRUE ) ) {
                DestroyWindow( hwnd );
            } else {
                SaveLocalListState( info->list.box, &state );
                ReDisplayLocalHeapList( info->list.box, &state );
            }
            break;
        case HEAPMENU_SORT_ADDR:
        case HEAPMENU_SORT_SIZE:
        case HEAPMENU_SORT_TYPE:
        case HEAPMENU_SORT_HANDLE:
            SortLocalHeapList( hwnd, wparam );
            ReDisplayLocalHeapList( info->list.box, NULL );
            break;
        case HEAPMENU_FILE_SAVE:
            SaveListBox( SLB_SAVE_TMP, PutOutLocalHeader, Config.lfname,
                         HeapWalkName, hwnd, info->list.box );
            break;
        case HEAPMENU_FILE_SAVE_TO:
            SaveListBox( SLB_SAVE_AS, PutOutLocalHeader, Config.lfname,
                         HeapWalkName, hwnd, info->list.box );
            break;
        case HEAPMENU_GLOBAL_MEMORYINFO:
            info->dialog = DisplayLocalHeapInfo( hwnd );
            break;
        case HEAPMENU_EXIT:
            DestroyWindow( hwnd );
            break;
        }
        break;
    case WM_DESTROY:
        KillPushWin( info->list.title );
        EnableLocalMenu( TRUE );
        MemFree( info );
        FreeLocalList();
        break;
    case WM_NCDESTROY:
        SendMessage( HeapWalkMainWindow, WM_USER, 0, 0 );
        LocalHeapHwnd = NULL;
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( TRUE );
}

void ResetLocalFont() {

    LclWndInfo          *info;

    if( LocalHeapHwnd != NULL ) {
        info = (LclWndInfo *)GetWindowLong( LocalHeapHwnd, 0 );
        KillPushWin( info->list.title );
        info->list.title = CreateLocalPushWin( LocalHeapHwnd );
        PositionListBox( &info->list, LocalHeapHwnd );
        SetBoxFont( info->list.box, GetMonoFont() );
    }
}

/*
 * LocalWalk - begin a walk of the local heap
 */

void LocalWalk( heap_list *item ) {

    char        title[30];
    int         width;
    int         hight;

    LocalHeapHandle = item->info.ge.hBlock;
    width = GetSystemMetrics( SM_CXSCREEN );
    hight = GetSystemMetrics( SM_CYSCREEN );
    strcpy( Owner, item->szModule );
    RCsprintf( title, STR_LCL_WIN_TITLE, Owner );
    LocalHeapHwnd = CreateWindow(
        LOCAL_DISPLAY_CLASS,    /* Window class name */
        title,          /* Window caption */
        WS_OVERLAPPED|WS_CAPTION
        |WS_SYSMENU|WS_THICKFRAME
        |WS_MAXIMIZEBOX,        /* Window style */
        width/8,                /* Initial X position */
        hight/3,                /* Initial Y position */
        3*width/4,              /* Initial X size */
        hight/3,                /* Initial Y size */
        HeapWalkMainWindow,     /* Parent window handle */
        NULL,                   /* Window menu handle */
        Instance,               /* Program instance handle */
        NULL);                  /* Create parameters */
    if( LocalHeapHwnd == NULL ) {
        RCMessageBox( NULL, STR_CANT_OPEN_LCL_WIN, HeapWalkName,
                    MB_OK | MB_TASKMODAL | MB_ICONINFORMATION );
    }
    ShowWindow( LocalHeapHwnd, SW_SHOWNORMAL );
    UpdateWindow( LocalHeapHwnd );
} /* LocalWalk */
