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
* Description:  Heap information dialogs.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include "heapwalk.h"
#include "jdlg.h"


/* Local Window callback functions prototypes */
BOOL __export FAR PASCAL SummaryInfoProc( HWND hwnd, WORD msg, WORD wparam, DWORD lparam );

static  FARPROC         dialProc;
static  unsigned        dialCount = 0;

#define GBL_INFO_DIALOG         1
#define LCL_INFO_DIALOG         2
#define MEM_INFO_DIALOG         3

/*
 * fillMemManDialog
 */
static void fillMemManDialog( HWND hwnd ) {

    MEMMANINFO          info;
    char                buf[15];
    char                *msgtitle;

    memset( &info, 0, sizeof( MEMMANINFO ) );
    info.dwSize = sizeof( MEMMANINFO );
    if( MemManInfo( &info ) == 0 ) {
        msgtitle = HWAllocRCString( STR_MEM_MAN_INFO_TITLE );
        RCMessageBox( HeapWalkMainWindow, STR_CANT_GET_MEM_MAN_INFO,
                      msgtitle, MB_OK | MB_ICONINFORMATION );
        HWFreeRCString( msgtitle );
        return;
    }
    ultoa( info.dwLargestFreeBlock, buf, 10 );
    SetStaticText( hwnd, MEMMAN_LARGEST_FREE_BLOCK, buf );
    ultoa( info.dwMaxPagesAvailable, buf, 10 );
    SetStaticText( hwnd, MEMMAN_MAX_PAGES_AVAIL, buf );
    ultoa( info.dwTotalUnlockedPages, buf, 10 );
    SetStaticText( hwnd, MEMMAN_UNLOCKED_PAGES, buf );
    ultoa( info.dwTotalLinearSpace, buf, 10 );
    SetStaticText( hwnd, MEMMAN_TOT_LIN_SPACE, buf );
    ultoa( info.dwFreePages, buf, 10 );
    SetStaticText( hwnd, MEMMAN_FREE_PAGES, buf );
    ultoa( info.dwSwapFilePages, buf, 10 );
    SetStaticText( hwnd, MEMMAN_SWAP_PAGES, buf );
    ultoa( info.dwFreeLinearSpace, buf, 10 );
    SetStaticText( hwnd, MEMMAN_FREE_LIN_SPACE, buf );
    ultoa( info.dwTotalPages, buf, 10 );
    SetStaticText( hwnd, MEMMAN_TOT_PAGES, buf );
    ultoa( info.wPageSize, buf, 10 );
    SetStaticText( hwnd, MEMMAN_PAGE_SIZE, buf );
    ultoa( info.dwMaxPagesLockable, buf, 10 );
    SetStaticText( hwnd, MEMMAN_LOCKABLE_PAGES, buf );
}

/*
 * fillGblInfoDialog
 */
static void fillGblInfoDialog( HWND hwnd ) {

    GLOBALINFO  meminfo;
    char        buf[15];
    char        *msgtitle;

    memset( &meminfo, 0, sizeof( GLOBALINFO ) );
    meminfo.dwSize = sizeof( GLOBALINFO );
    if( GlobalInfo( &meminfo ) == 0 ) {
        msgtitle = HWAllocRCString( STR_GLOB_HEAP_INFO );
        RCMessageBox( HeapWalkMainWindow, STR_CANT_GET_GBL_HEAP_INFO,
                      msgtitle, MB_OK | MB_ICONINFORMATION );
        HWFreeRCString( msgtitle );
        return;
    }
    utoa( meminfo.wcItems, buf, 10 );
    SetStaticText( hwnd, GBL_INFO_TOT_ITEMS, buf );
    utoa( meminfo.wcItemsFree, buf, 10 );
    SetStaticText( hwnd, GBL_INFO_FREE_ITEMS, buf );
    utoa( meminfo.wcItemsLRU, buf, 10 );
    SetStaticText( hwnd, GBL_INFO_LRU_ITEMS, buf );
}

/*
 * fillLclInfoDialog
 */
static void fillLclInfoDialog( HWND hwnd ) {

    char                buf[15];
    LclInfo             info;

    LclHeapInfo( &info );
    utoa( info.free_count ,buf, 10 );
    SetStaticText( hwnd, LCL_INFO_FREE_CNT, buf );
    utoa( info.movable_count ,buf, 10 );
    SetStaticText( hwnd, LCL_INFO_MOVE_CNT, buf );
    utoa( info.fixed_count ,buf, 10 );
    SetStaticText( hwnd, LCL_INFO_FIXED_CNT, buf );
    utoa( info.free_size ,buf, 10 );
    SetStaticText( hwnd, LCL_INFO_FREE_SIZE, buf );
    utoa( info.fixed_size ,buf, 10 );
    SetStaticText( hwnd, LCL_INFO_FIXED_SIZE, buf );
    utoa( info.movable_size ,buf, 10 );
    SetStaticText( hwnd, LCL_INFO_MOVE_SIZE, buf );
    utoa( info.tot_size ,buf, 10 );
    SetStaticText( hwnd, LCL_INFO_TOT_SIZE, buf );
    utoa( info.tot_count ,buf, 10 );
    SetStaticText( hwnd, LCL_INFO_TOT_CNT, buf );
}

/*
 * SummaryInfoProc - process messages from summary information dialogs
 */
BOOL FAR PASCAL SummaryInfoProc( HWND hwnd, WORD msg, WORD wparam, DWORD lparam )
{
    wparam = wparam;
    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        SetWindowLong( hwnd, DWL_USER, lparam );
        switch( lparam ) {
        case MEM_INFO_DIALOG:
            fillMemManDialog( hwnd );
            break;
        case LCL_INFO_DIALOG:
            fillLclInfoDialog( hwnd );
            break;
        case GBL_INFO_DIALOG:
            fillGblInfoDialog( hwnd );
            break;
        }
        break;
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDOK:
            SendMessage( hwnd, WM_CLOSE, 0, 0 );
            break;
        case HW_INFO_REFRESH:
            switch( GetWindowLong( hwnd, DWL_USER ) ) {
            case MEM_INFO_DIALOG:
                fillMemManDialog( hwnd );
                break;
            case LCL_INFO_DIALOG:
                fillLclInfoDialog( hwnd );
                break;
            case GBL_INFO_DIALOG:
                fillGblInfoDialog( hwnd );
                break;
            }
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow( hwnd );
        break;
    case WM_NCDESTROY:
        dialCount --;
        if( dialCount == 0 ) {
            FreeProcInstance( dialProc );
        }
        return( FALSE ); /* we need to let WINDOWS see this message or
                            fonts are left undeleted */
    default:
        return( FALSE );
    }
    return( TRUE );
} /* SummaryInfoProc */

/*
 * initProcInst - make sure dialProc points to a valid procedure and
 *                increment the dialCount usage counter
 */

static void initProcInst( void ) {

    if( dialCount == 0 ) {
        dialProc = MakeProcInstance( (FARPROC)SummaryInfoProc, Instance );
    }
    dialCount ++;
} /* initProcInst */

/*
 * DisplayGlobHeapInfo - display information about the global heap
 */

void DisplayGlobHeapInfo( HWND parent ) {

    initProcInst();
    JCreateDialogParam( Instance, "GBL_HEAP_INFO", parent ,
                          (DLGPROC)dialProc, GBL_INFO_DIALOG );
} /* DisplayGlobMemInfo */

/*
 * DisplayMemManInfo - display information about the memory manager
 */

void DisplayMemManInfo( HWND parent ) {

    initProcInst();
    JCreateDialogParam( Instance, "MEMMAN_INFO", parent, (DLGPROC)dialProc,
                       MEM_INFO_DIALOG );
} /* DisplayMemManInfo */

/*
 * DisplayLocalHeapInfo - display information about the global heap
 */

HWND DisplayLocalHeapInfo( HWND parent ) {

    HWND                dialog;

    initProcInst();
    dialog = JCreateDialogParam( Instance, "LCL_HEAP_INFO", parent,
                             (DLGPROC)dialProc, LCL_INFO_DIALOG);
    return( dialog );
} /* DisplayLocalHeapInfo */
