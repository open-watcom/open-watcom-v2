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


#include "heapwalk.h"
#include <string.h>

#define   MONITOR_TIMER         500
#define   REFRESH_TIME          500

#define   BAR_HITE              10
#define   BAR_LENGTH            1000
#define   BAR_XPOS              ( ( MONITOR_WIDTH - BAR_LENGTH ) / 8 )

#define   MONITOR_WIDTH         1600
#define   MONITOR_HITE          130

#define   TICK_LENGTH           10
#define   TEXT_HITE             ( ( MONITOR_HITE - TEXT_SPACE - \
                                 2 * BAR_HITE ) / ( SIZE_CNT + 1 ) )
#define   TEXT_SPACE            10

#define   SECTION_WIDTH(x, y)   ( ( (DWORD)BAR_LENGTH * (DWORD)(x) ) / ( y ) )

/*
 * indices for the sizes array in the LocalMonInfo struct
 */
enum {
    STATIC_SIZE = 0,
    STACK_SIZE,
    FIXED_SIZE,
    MOVE_SIZE,
    FREE_SIZE,
    OTHER_SIZE,
    SIZE_CNT            /* this entry must always be last */
};

typedef struct {
    HANDLE      instance;
    HTASK       task_hdl;
    HGLOBAL     handle;
    WORD        sizes[SIZE_CNT];
    WORD        total_size;
    WORD        stack_used;
} LocalMonInfo;

typedef struct {
    WORD        use_cnt;
    HBRUSH      brush[ SIZE_CNT ];
} MonitorTools;

static COLORREF         BarColors[ SIZE_CNT ] = {
                                RGB( 0, 0, 0 ),
                                RGB( 0, 255, 0 ),
                                RGB( 255, 0, 0 ),
                                RGB( 170, 170, 170 ),
                                RGB( 0, 0, 255 ),
                                RGB( 255, 255, 255 )
                        };

static BOOL             LabelsInitialized = FALSE;
static char             *MonitorLabels[ SIZE_CNT ];
static MonitorTools     Brushes;


/*
 * GetMonitorInfo - refresh information about the local heap being monitored
 */
static BOOL GetMonitorInfo( LocalMonInfo *info ) {

    LocalMonInfo        old_info;
    TASKENTRY           taskinfo;
    LOCALENTRY          localinfo;
    BOOL                ret;
    WORD                i;

    old_info = *info;

    memset( &localinfo, 0, sizeof( LOCALENTRY ) );
    localinfo.dwSize = sizeof( LOCALENTRY );
    memset( &taskinfo, 0, sizeof( TASKENTRY ) );
    taskinfo.dwSize = sizeof( TASKENTRY );

    TaskFindHandle( &taskinfo, info->task_hdl );
    info->sizes[ STATIC_SIZE ] = taskinfo.wStackTop;
    info->sizes[ STACK_SIZE ] = taskinfo.wStackBottom - taskinfo.wStackTop;
    info->sizes[ FIXED_SIZE ] = 0;
    info->sizes[ MOVE_SIZE ] = 0;
    info->sizes[ FREE_SIZE ] = 0;

    ret = LocalFirst( &localinfo, info->handle );
    while( ret ) {
        switch( localinfo.wFlags ) {
        case LF_FIXED:
            info->sizes[ FIXED_SIZE ] += localinfo.wSize;
            break;
        case LF_MOVEABLE:
            info->sizes[ MOVE_SIZE ] += localinfo.wSize;
            break;
        case LF_FREE:
            info->sizes[ FREE_SIZE ] += localinfo.wSize;
            break;
        }
        ret = LocalNext( &localinfo );
    }
    info->stack_used = taskinfo.wStackBottom - taskinfo.wStackMinimum;
    if( !memcmp( info, &old_info, sizeof( LocalMonInfo ) ) ) {
        return( FALSE );
    }
    info->sizes[ OTHER_SIZE ] = info->total_size;
    for( i=0; i < SIZE_CNT; i++ ) {
        if( i != OTHER_SIZE ) info->sizes[ OTHER_SIZE ] -= info->sizes[i];
    }
    return( TRUE );
}

/*
 * PaintMonitor - redraw the monitor window
 */
static void PaintMonitor( HWND hwnd, HDC dc, LocalMonInfo *info ) {

    RECT        area;
    WORD        i;
    char        buf[80];
    WORD        xpos, ypos;
    HBRUSH      white;

    SaveDC( dc );
    GetClientRect( hwnd, &area );
    white = GetStockObject( WHITE_BRUSH );
    FillRect( dc, &area, white );
    SetMapMode( dc, MM_ANISOTROPIC );
    SetWindowOrg( dc, 0, 0 );
    SetWindowExt( dc, MONITOR_WIDTH, MONITOR_HITE );
    SetViewportOrg( dc, 0, 0 );
    SetViewportExt( dc, area.right, area.bottom );

    area.top = BAR_HITE;
    area.bottom = 2 * BAR_HITE;
    area.left = BAR_XPOS;
    for( i=0; i < SIZE_CNT; i++ ) {
        if( info->sizes[i] == 0 ) continue;
        area.right = area.left + SECTION_WIDTH( info->sizes[i],
                                                info->total_size );
        FillRect( dc, &area, Brushes.brush[i] );

        xpos = ( area.left + area.right ) / 2;
        MoveTo( dc, xpos, area.bottom );
        ypos = area.bottom + TEXT_SPACE + ( SIZE_CNT - i ) * TEXT_HITE;
        LineTo( dc, xpos, ypos );
        xpos += TICK_LENGTH;
        LineTo( dc, xpos, ypos );
        if( i == STACK_SIZE ) {
            sprintf( buf, MonitorLabels[i], info->sizes[i],
                     info->stack_used );
        } else {
            sprintf( buf, MonitorLabels[i], info->sizes[i] );
        }
        TextOut( dc, xpos, ypos, buf, strlen( buf ) );
        area.left = area.right;
    }

    area.left = BAR_XPOS;
    area.right = area.left + BAR_LENGTH;
    MoveTo( dc, area.left, area.top );
    LineTo( dc, area.right, area.top );
    LineTo( dc, area.right, area.bottom );
    LineTo( dc, area.left, area.bottom );
    LineTo( dc, area.left, area.top );

    TextOut( dc, 0, BAR_HITE, "0", 1 );
    sprintf( buf, "%u", info->total_size );
    TextOut( dc, area.right + area.left, BAR_HITE, buf, strlen( buf ) );
    RestoreDC( dc, -1 );
}

/*
 * LocalMonitorProc - handle messages while monitoring a local heap
 */

BOOL __export FAR PASCAL LocalMonitorProc( HWND hwnd, WORD msg, WORD wparam,
                                    DWORD lparam )
{
    LocalMonInfo        *info;
    HDC                 dc;
    WORD                i;
    PAINTSTRUCT         paint;
    char                *msgtitle;

    info = (LocalMonInfo *)GetWindowLong( hwnd, 0 );
    switch( msg ) {
    case WM_CREATE:
        info = (LocalMonInfo *)( ( (CREATESTRUCT *)lparam )->lpCreateParams );
        SetWindowLong( hwnd, 0, (DWORD)info );
        if( !SetTimer( hwnd, MONITOR_TIMER, REFRESH_TIME, NULL ) ) {
            msgtitle = AllocRCString( STR_MONITOR_LCL_HEAP );
            RCMessageBox( NULL, STR_CANT_OPEN_LCL_MONITOR, msgtitle,
                        MB_OK | MB_TASKMODAL | MB_ICONINFORMATION );
            FreeRCString( msgtitle );
            DestroyWindow( hwnd );
        }
        GetMonitorInfo( info );
        break;
    case WM_PAINT:
        dc = BeginPaint( hwnd, &paint );
        PaintMonitor( hwnd, dc, info );
        EndPaint( hwnd, &paint );
        break;
    case WM_TIMER:
        if( GetMonitorInfo( info ) ) {
            dc = GetDC( hwnd );
            PaintMonitor( hwnd, dc, info );
            ReleaseDC( hwnd, dc );
        }
        break;
    case WM_SIZE:
        dc = GetDC( hwnd );
        PaintMonitor( hwnd, dc, info );
        ReleaseDC( hwnd, dc );
        break;
    case WM_DESTROY:
        KillTimer( hwnd, MONITOR_TIMER );
        Brushes.use_cnt --;
        if( Brushes.use_cnt == 0 ) {
            for( i=0; i < SIZE_CNT; i++ ) {
                DeleteObject( Brushes.brush[i] );
            }
        }
        MemFree( info );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( NULL );
}

/*
 * GetTaskHdl - return the task handle associated with the given instance
 *              handle
 */

static HTASK GetTaskHdl( HANDLE instance ) {

    TASKENTRY           taskinfo;
    BOOL                ret;

    memset( &taskinfo, 0, sizeof( TASKENTRY ) );
    taskinfo.dwSize = sizeof( TASKENTRY );
    ret = TaskFirst( &taskinfo );
    while( ret ) {
        if( taskinfo.hInst == instance ) {
            return( taskinfo.hTask );
        }
        ret = TaskNext( &taskinfo );
    }
    return( NULL );
}

/*
 * BeginMonitor - begin to monitor a local heap
 */

void BeginMonitor( heap_list *item ) {

    HWND                hwnd;
    char                title[50];
    int                 width;
    int                 hight;
    LocalMonInfo        *info;
    WORD                i;
    char                *msgtitle;

    if( !LabelsInitialized ) {
        MonitorLabels[ STATIC_SIZE ] = GetRCString( STR_STATIC_DATA );
        MonitorLabels[ STACK_SIZE ] = GetRCString( STR_STACK_ALLOCATED );
        MonitorLabels[ FIXED_SIZE ] = GetRCString( STR_FIXED_HEAP );
        MonitorLabels[ MOVE_SIZE ] = GetRCString( STR_MOVEABLE_HEAP );
        MonitorLabels[ FREE_SIZE ] = GetRCString( STR_FREE_HEAP );
        MonitorLabels[ OTHER_SIZE ] = GetRCString( STR_MONITOR_UNKNOWN );
        LabelsInitialized = TRUE;
    }
    if( Brushes.use_cnt == 0 ) {
        for( i=0; i < SIZE_CNT; i++ ) {
            Brushes.brush[i] = CreateSolidBrush( BarColors[i] );
        }
    }
    Brushes.use_cnt++;
    width = GetSystemMetrics( SM_CXSCREEN );
    hight = GetSystemMetrics( SM_CYSCREEN );
    RCsprintf( title, STR_LCL_MONITOR_TITLE, item->szModule );

    info = MemAlloc( sizeof( LocalMonInfo ) );
    memset( info, 0, sizeof( LocalMonInfo ) );
    info->instance = item->info.ge.hBlock;
    info->task_hdl = GetTaskHdl( info->instance );
    info->total_size = item->info.ge.dwBlockSize;
    info->handle = item->info.ge.hBlock;

    hwnd = CreateWindow(
        LOCAL_MONITOR_CLASS,            /* Window class name */
        title,                          /* Window caption */
        WS_OVERLAPPED|WS_CAPTION
        |WS_SYSMENU|WS_THICKFRAME
        |WS_MAXIMIZEBOX,                /* Window style */
        width/12,                       /* Initial X position */
        hight/4,                        /* Initial Y position */
        5*width/6,                      /* Initial X size */
        hight/2,                        /* Initial Y size */
        HeapWalkMainWindow,             /* Parent window handle */
        NULL,                           /* Window menu handle */
        Instance,                       /* Program instance handle */
        info );                         /* Create parameters */
    if( hwnd == NULL ) {
        msgtitle = AllocRCString( STR_MONITOR_LCL_HEAP );
        RCMessageBox( NULL, STR_CANT_OPEN_LCL_MONITOR, HeapWalkName,
                    MB_OK | MB_TASKMODAL | MB_ICONINFORMATION );
        FreeRCString( msgtitle );
    }
    ShowWindow( hwnd, SW_SHOWNORMAL );
    UpdateWindow( hwnd );
}
