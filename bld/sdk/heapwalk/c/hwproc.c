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
* Description:  Heap walker window procedre.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include "bool.h"
#include "heapwalk.h"
#include "aboutdlg.h"
#include "wwinhelp.h"
#include "jdlg.h"


static  BOOL    NoRefresh = FALSE;

/*
 * HideResources - hide all resource display windows
 */
static void HideResources( BOOL hide ) {

    int         cmd;
    WORD        i;

    cmd = hide ? SW_HIDE : SW_SHOWNOACTIVATE;
    for( i=0; i < MAX_RES; i++ ) {
        if( ResHwnd[i] != NULL ) {
            ShowWindow( ResHwnd[i], cmd );
        }
    }
} /* HideResources */

static HWND *SetUpPushWindows( HWND hwnd, WORD type ) {

    HWND        *ret;

    if( type == HEAPMENU_DISPLAY_DPMI ) {
        ret = MakePushWin( hwnd, HeapDPMITitles, 8, HEAPMENU_SORT_HANDLE,
                                HEAPMENU_SORT_ADDR, HEAPMENU_SORT_SIZE,
                                HEAPMENU_SORT_GRAN, HEAPMENU_SORT_TYPE,
                                HEAPMENU_SORT_DPL, 0 ,0 );
    } else {
        ret = MakePushWin( hwnd, HeapTitles, 8, HEAPMENU_SORT_HANDLE,
                        HEAPMENU_SORT_ADDR, HEAPMENU_SORT_SIZE,
                        HEAPMENU_SORT_MODULE, HEAPMENU_SORT_FLAG,
                        0, 0, HEAPMENU_SORT_TYPE );
    }
    return( ret );
}

/*
 * SetDisplayType - changes the heap display type to 'type' unless type is
 *                  'HEAPMENU_DISPLAY_INIT' in which case this is assumed to
 *                  be the first call to SetDisplayType and the heap display
 *                  type is set to 'HeapType'
 */
static void SetDisplayType( HWND hwnd, HWND **title, WORD type ) {

    HMENU       mh;
    char        buf[256];
    char        *typename;

    if( type == HeapType ) return;
    if( type == HEAPMENU_DISPLAY_DPMI && !WDebug386 ) {
        RCMessageBox( HeapWalkMainWindow, STR_INSTALL_WDEBUG,
                      HeapWalkName, MB_OK | MB_ICONEXCLAMATION );
        return;
    }
    mh = GetMenu( hwnd );
    if( type == HEAPMENU_DISPLAY_INIT ) {
        type = HeapType;
        HeapType = HEAPMENU_DISPLAY_INIT;
    } else {
        CheckMenuItem( mh, HeapType, MF_UNCHECKED | MF_BYCOMMAND );
    }

    /* undo anything done for the previous state */
    switch( HeapType ) {
    case HEAPMENU_DISPLAY_DPMI:
        EnableMenuItem( mh, HEAPMENU_ADD, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( mh, HEAPMENU_OBJECT_DISCARD,
                        MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( mh, HEAPMENU_OBJECT_OLDEST,
                        MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( mh, HEAPMENU_OBJECT_NEWEST,
                        MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( mh, HEAPMENU_LOCAL_LOCALWALK,
                        MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( mh, HEAPMENU_COMPACT_AND_LOCALWALK,
                        MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( mh, HEAPMENU_OBJECT_GET_SELECTOR,
                        MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( mh, HEAPMENU_LOCAL_MONITOR,
                        MF_BYCOMMAND | MF_ENABLED );
        ModifyMenu( mh, 2, MF_BYPOSITION | MF_POPUP,
                    (UINT)LoadMenu( Instance, "SORTMENU" ),
                    HWGetRCString( STR_SORT ) );
        KillPushWin( *title );
        *title = SetUpPushWindows( hwnd, HEAPMENU_DISPLAY_ENTIRE );
        if( GSortType == HEAPMENU_SORT_GRAN ||
            GSortType == HEAPMENU_SORT_DPL ) GSortType = HEAPMENU_SORT_HANDLE;
        break;
    case HEAPMENU_DISPLAY_LRU:
        DeleteMenu( mh, HEAPMENU_SORT_LRU, MF_BYCOMMAND );
        if( GSortType == HEAPMENU_SORT_LRU ) GSortType = HEAPMENU_SORT_HANDLE;
        break;
    case HEAPMENU_DISPLAY_INIT:
        if( type != HEAPMENU_DISPLAY_DPMI ) {
            *title = SetUpPushWindows( hwnd, type );
            ModifyMenu( mh, 2, MF_BYPOSITION | MF_POPUP,
                    (UINT)LoadMenu( Instance, "SORTMENU" ),
                    HWGetRCString( STR_SORT ) );
        } else {
            *title = NULL;
        }
        break;
    }
    /* do things for the new state */
    switch( type ) {
    case HEAPMENU_DISPLAY_DPMI:
        ModifyMenu( mh, 2, MF_BYPOSITION | MF_POPUP,
                    (UINT)LoadMenu( Instance, "SORTDPMIMENU" ),
                    HWGetRCString( STR_SORT ) );
        EnableMenuItem( mh, HEAPMENU_ADD, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( mh, HEAPMENU_OBJECT_DISCARD,
                        MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( mh, HEAPMENU_OBJECT_OLDEST,
                        MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( mh, HEAPMENU_OBJECT_NEWEST,
                        MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( mh, HEAPMENU_LOCAL_LOCALWALK,
                        MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( mh, HEAPMENU_COMPACT_AND_LOCALWALK,
                        MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( mh, HEAPMENU_OBJECT_GET_SELECTOR,
                        MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( mh, HEAPMENU_LOCAL_MONITOR,
                        MF_BYCOMMAND | MF_GRAYED );
        if( GSortType == HEAPMENU_SORT_MODULE ||
                GSortType == HEAPMENU_SORT_TYPE ) {
            CheckMenuItem( mh, GSortType, MF_UNCHECKED | MF_BYCOMMAND );
            GSortType = HEAPMENU_SORT_ADDR;
        }
//      SetTitle( HeapDPMITitles, title );
        KillPushWin( *title );
        *title = SetUpPushWindows( hwnd, type );
        typename = HWAllocRCString( STR_SELECTOR_LIST_ITEMS );
        break;
    case HEAPMENU_DISPLAY_LRU:
        InsertMenu( mh, HEAPMENU_SORT_FLAG, MF_BYCOMMAND, HEAPMENU_SORT_LRU,
                   GetRCString( STR_BY_AGE ) );
        typename = HWAllocRCString( STR_LRU_ITEMS );
        break;
    case HEAPMENU_DISPLAY_ENTIRE:
        typename = HWAllocRCString( STR_ENTIRE_HEAP );
        break;
    case HEAPMENU_DISPLAY_FREE:
        typename = HWAllocRCString( STR_FREE_ITEMS );
        break;
    default:
        typename = "";
        break;
    }
    CheckMenuItem( mh, type, MF_CHECKED | MF_BYCOMMAND );
    CheckMenuItem( mh, GSortType, MF_CHECKED | MF_BYCOMMAND );
    DrawMenuBar( hwnd );
    sprintf( buf, "%s - %s", HeapWalkName, typename );
    HWFreeRCString( typename );
    SetWindowText( hwnd, buf );
    HeapType = type;
} /* SetDisplayType */

/*
 * ResetFont - refresh all windows after a font change
 */

static void ResetFont( GblWndInfo *info ) {

    KillPushWin( info->list.title );
    info->list.title = SetUpPushWindows( HeapWalkMainWindow, HeapType );
    PositionListBox( &info->list, HeapWalkMainWindow );
    SetBoxFont( info->list.box, GetMonoFont() );
    ResetLocalFont();
    PaintAllWindows();
}

/*
 * CheckForLocalSelect - return the index of the current item if it
 *                       has a local heap otherwise print an appropriate
 *                       message and return LB_ERR
 */
static DWORD CheckForLocalSelect( GblWndInfo *info ) {

    int    index;

    index = (int)SendMessage( info->list.box, LB_GETCURSEL, 0, 0 );
    if( index == LB_ERR ) {
        RCMessageBox( NULL, STR_NO_ITEM_SELECTED, HeapWalkName,
                    MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL );
    } else if( !HeapList[index]->info.ge.wHeapPresent ) {
        RCMessageBox( NULL, STR_HAS_NO_LCL_HEAP, HeapWalkName,
                    MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL );
        index = LB_ERR;
    }
    return( index );
}

/*
 * HeapWalkProc - show task status
 */
BOOL FAR PASCAL HeapWalkProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    HMENU       mh;
    HMENU       mh2;
    HCURSOR     hourglass;
    HCURSOR     oldcursor;
    FARPROC     fp;
    DWORD       index;
    heap_list   hl;
    GblWndInfo  *info;
    RECT        area;
    about_info  ai;

    info = (GblWndInfo *)GetWindowLong( hwnd, 0 );
    switch( msg ) {
    case WM_CREATE:
        InitPaintProc();
        info = MemAlloc( sizeof( GblWndInfo ) );
        if( info == NULL ) {
            ErrorBox( hwnd, STR_UNABLE_2_STARTUP,
                     MB_OK | MB_ICONINFORMATION );
            PostQuitMessage( 0 );
        }
        memset( info, 0, sizeof( GblWndInfo ) );
        SetWindowLong( hwnd, 0, (DWORD)info );
//      hdc = GetDC( hwnd );
//      ReleaseDC(hwnd, hdc);
        SetDisplayType( hwnd, &( info->list.title ), HEAPMENU_DISPLAY_INIT );
        CreateListBox( hwnd, &info->list, GLOBAL_LB );
        info->alloc_proc = MakeProcInstance( (FARPROC)AllocDlgProc, Instance );
        info->alloc_dialog = JCreateDialog( Instance, "ALLOC_DLG",
                                           hwnd, (DLGPROC)info->alloc_proc );
        memset( &ResHwnd, 0, MAX_RES * sizeof( HWND ) );
        break;
    case WM_MEASUREITEM:
        break;
    case WM_MOVE:
        GetWindowRect( hwnd, &area );
        if( !info->minimized ) {
            Config.last_glob_xpos = Config.glob_xpos;
            Config.last_glob_ypos = Config.glob_ypos;
            Config.glob_xpos = area.left;
            Config.glob_ypos = area.top;
        }
        break;
    case WM_SIZE:
        ResizeListBox( LOWORD( lparam ), HIWORD( lparam ), &( info->list ) );
        if( wparam == SIZE_MINIMIZED || wparam == SIZE_MAXIMIZED ) {
            Config.glob_xpos = Config.last_glob_xpos;
            Config.glob_ypos = Config.last_glob_ypos;
        }
        if( wparam == SIZE_MINIMIZED ) {
            info->minimized = TRUE;
            HideResources( TRUE );
        } else if( info->minimized ) {
            HideResources( FALSE );
            info->minimized = FALSE;
        } else if( wparam != SIZE_MAXIMIZED )  {
            GetWindowRect( hwnd, &area );
            Config.glob_xsize = area.right - area.left;
            Config.glob_ysize = area.bottom - area.top;
        }
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
        break;
    case WM_CLOSE:
        PostMessage( hwnd, WM_COMMAND, HEAPMENU_EXIT, 0L );
        return( TRUE );
    case WM_QUERYENDSESSION:
        SaveConfigFile( FALSE );
        return( TRUE );
        break;
    case WM_DESTROY:
        FiniPaintProc();
        KillPushWin( info->list.title );
        SaveConfigFile( FALSE );
        if( info != NULL ) {
            FreeProcInstance( info->alloc_proc );
            MemFree( info );
        }
        DestroyMonoFonts();
        CvrCtl3dUnregister( Instance );
        CvrCtl3DFini( Instance );
        WWinHelp( hwnd, "heapwalk.hlp", HELP_QUIT, 0 );
        PostQuitMessage( 0 );
        break;
    case WM_ACTIVATEAPP:
        if( wparam && !NoRefresh ) {
            if( info->doing_add ) {
                info->need_refresh = TRUE;
            } else {
                InitHeapList( info->list.box, TRUE );
            }
        }
        break;
    case WM_MENUSELECT:
        if( LOWORD( lparam & MF_POPUP ) ) {
            mh = GetMenu( hwnd );
            mh2 = GetSubMenu( mh, 6 );
            if( (HMENU)wparam == mh2  ) {
                ShowWindow( info->alloc_dialog, SW_SHOWNOACTIVATE );
            } else if( (HMENU)wparam != GetSubMenu( mh2, 3 ) &&
                        (HMENU)wparam != GetSubMenu( mh2, 4 ) &&
                        (HMENU)wparam != GetSubMenu( mh2, 5 ) ) {
                ShowWindow( info->alloc_dialog, SW_HIDE );
            }
        }
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
        break;
    case WM_COMMAND:
        switch( wparam ) {
        case HEAPMENU_ABOUT:
            ai.owner = hwnd;
            ai.inst = Instance;
            ai.name = HWGetRCString( STR_ABOUT_NAME );
            ai.version = HWGetRCString( STR_ABOUT_VERSION );
            ai.title = HWGetRCString( STR_ABOUT_TITLE );
            DoAbout( &ai );
            break;
        case HEAPMENU_HELP_CONTENTS:
            WWinHelp( hwnd, "heapwalk.hlp", HELP_CONTENTS, 0 );
            break;
        case HEAPMENU_HELP_SRCH:
            WWinHelp( hwnd, "heapwalk.hlp", HELP_PARTIALKEY, (HELP_DATA)"" );
            break;
        case HEAPMENU_HELP_ON_HELP:
            WWinHelp( hwnd, "winhelp.hlp", HELP_HELPONHELP, 0 );
            break;
        case HEAPEX_LIST:
            if( !info->doing_add ) {
                if( HIWORD( lparam ) == LBN_DBLCLK ) {
                    ShowHeapObject( (HWND)LOWORD( lparam ) );
                }
            } else {
                if( HIWORD( lparam ) == LBN_SELCHANGE
                    || HIWORD( lparam ) == LBN_DBLCLK ) {
                    RefreshAdd( info->add_dialog, info->list.box );
                    RedrawBox( info->list.box, index );
                }
            }
            break;
        case HEAPMENU_GLOBAL_REFRESH:
            InitHeapList( info->list.box, TRUE );
            break;
        case HEAPMENU_FONT:
            if( ChooseMonoFont( hwnd ) ) {
                ResetFont( info );
            }
            break;
        case HEAPMENU_EXIT:
            DestroyWindow( hwnd );
            FreeHeapList();
            break;
        case HEAPMENU_DISPLAY_DPMI:
        case HEAPMENU_DISPLAY_ENTIRE:
        case HEAPMENU_DISPLAY_LRU:
        case HEAPMENU_DISPLAY_FREE:
            SetDisplayType( hwnd, &( info->list.title ), wparam );
            InitHeapList( info->list.box, FALSE );
            break;
        case HEAPMENU_SORT_ADDR:
        case HEAPMENU_SORT_HANDLE:
        case HEAPMENU_SORT_MODULE:
        case HEAPMENU_SORT_SIZE:
        case HEAPMENU_SORT_TYPE:
        case HEAPMENU_SORT_GRAN:
        case HEAPMENU_SORT_DPL:
        case HEAPMENU_SORT_FLAG:
        case HEAPMENU_SORT_LRU:
            mh = GetMenu( hwnd );
            CheckMenuItem( mh, GSortType, MF_UNCHECKED | MF_BYCOMMAND );
            CheckMenuItem( mh, wparam, MF_CHECKED | MF_BYCOMMAND );
            if( GSortType != wparam ) {
                GSortType = wparam;
                SortHeapList();
                ReDisplayHeapList( info->list.box, NULL );
            }
            break;
        case HEAPMENU_OBJECT_SHOW:
            ShowHeapObject( info->list.box );
            break;
        case HEAPMENU_OBJECT_DISCARD:
            if( GlobDiscardObj( info->list.box ) ) {
                InitHeapList( info->list.box, TRUE );
            }
            break;
        case HEAPMENU_OBJECT_NEWEST:
            if( GlobSetObjPos( info->list.box, FALSE ) ) {
                if( GSortType == HEAPMENU_SORT_LRU ) {
                    InitHeapList( info->list.box, TRUE );
                }
            }
            break;
        case HEAPMENU_OBJECT_OLDEST:
            if( GlobSetObjPos( info->list.box, TRUE ) ) {
                if( GSortType == HEAPMENU_SORT_LRU ) {
                    InitHeapList( info->list.box, TRUE );
                }
            }
            break;
        case HEAPMENU_OBJECT_GET_SELECTOR:
            ShowSelector( info->list.box );
            break;
        case HEAPMENU_GLOBAL_HEAPINFO:
            DisplayGlobHeapInfo( hwnd );
            break;
        case HEAPMENU_GLOBAL_MEMORYINFO:
            DisplayMemManInfo( hwnd );
            break;
        case HEAPMENU_GLOBAL_COMPACT:
            GlobalCompact( 0 );
            InitHeapList( info->list.box, TRUE );
            break;
        case HEAPMENU_GLOBAL_COMP_DISC:
            GlobalCompact( -1 );
            InitHeapList( info->list.box, TRUE );
            break;
        case HEAPMENU_GLOBAL_CODE_SIZE:
            fp = MakeProcInstance( (FARPROC)SetCodeDlgProc, Instance );
            JDialogBox( Instance, "CODE_AREA_DLG", hwnd, (DLGPROC)fp );
            FreeProcInstance( fp );
            break;
        case HEAPMENU_FILE_SAVE:
            InitHeapList( info->list.box, TRUE );
            SaveListBox( SLB_SAVE_TMP, PutOutGlobalHeader, Config.gfname,
                         HeapWalkName, hwnd, info->list.box );
            break;
        case HEAPMENU_FILE_SAVE_TO:
            InitHeapList( info->list.box, TRUE );
            SaveListBox( SLB_SAVE_AS, PutOutGlobalHeader, Config.gfname,
                         HeapWalkName, hwnd, info->list.box );
            break;
        case HEAPMENU_SAVE_CONFIG:
            SaveConfigFile( TRUE );
            break;
        case HEAPMENU_CONFIGURE:
            HWConfigure();
            break;
        case HEAPMENU_LOCAL_MONITOR:
            index = CheckForLocalSelect( info );
            if( index != LB_ERR ) {
                BeginMonitor( HeapList[index] );
            }
            break;
        case HEAPMENU_COMPACT_AND_LOCALWALK:
            LocalCompact( -1 );
            /* fall through */
        case HEAPMENU_LOCAL_LOCALWALK:
            index = CheckForLocalSelect( info );
            if( index != LB_ERR ) {
                LocalWalk( HeapList[index] );
            }
            break;
        case HEAPMENU_GDI_LOCALWALK:
            if( GetDGroupItem( "GDI", &hl ) ) {
                LocalWalk( &hl );
            }
            break;
        case HEAPMENU_USER_LOCALWALK:
            if( GetDGroupItem( "USER", &hl ) ) {
                LocalWalk( &hl );
            }
            break;
        case HEAPMENU_FREE_ALL:
            MyFreeAllMem();
            UpdateAllocInfo( info->alloc_dialog );
            PaintAllWindows();
            break;
        case HEAPMENU_FREE_1K:
        case HEAPMENU_FREE_2K:
        case HEAPMENU_FREE_5K:
        case HEAPMENU_FREE_10K:
        case HEAPMENU_FREE_25K:
        case HEAPMENU_FREE_50K:
            hourglass = LoadCursor( NULL, IDC_WAIT );
            SetCapture( hwnd );
            oldcursor= SetCursor( hourglass );
            FreeSomeMem( wparam );
            UpdateAllocInfo( info->alloc_dialog );
            SetCursor( oldcursor );
            ReleaseCapture();
            PaintAllWindows();
            break;
        case HEAPMENU_ALLOC_1K:
        case HEAPMENU_ALLOC_2K:
        case HEAPMENU_ALLOC_5K:
        case HEAPMENU_ALLOC_10K:
        case HEAPMENU_ALLOC_25K:
        case HEAPMENU_ALLOC_50K:
            hourglass = LoadCursor( NULL, IDC_WAIT );
            SetCapture( hwnd );
            oldcursor= SetCursor( hourglass );
            AllocMore( wparam );
            UpdateAllocInfo( info->alloc_dialog );
            SetCursor( oldcursor );
            ReleaseCapture();
            PaintAllWindows();
            break;
        case HEAPMENU_ALLOC_ALL:
        case HEAPMENU_ALLOC_BUT_1K:
        case HEAPMENU_ALLOC_BUT_2K:
        case HEAPMENU_ALLOC_BUT_5K:
        case HEAPMENU_ALLOC_BUT_10K:
        case HEAPMENU_ALLOC_BUT_25K:
        case HEAPMENU_ALLOC_BUT_50K:
            hourglass = LoadCursor( NULL, IDC_WAIT );
            SetCapture( hwnd );
            oldcursor= SetCursor( hourglass );
            AllocAllBut( wparam );
            UpdateAllocInfo( info->alloc_dialog );
            SetCursor( oldcursor );
            ReleaseCapture();
            PaintAllWindows();
            break;
        case HEAPMENU_ALLOC_NK:
        case HEAPMENU_ALLOC_BUT_NK:
        case HEAPMENU_FREE_NK:
            DoNBytes( hwnd, wparam );
            UpdateAllocInfo( info->alloc_dialog );
            PaintAllWindows();
            break;
        case HEAPMENU_ADD:
            info->add_dialog = StartAdd( hwnd, &info->list );
            if( info->add_dialog != NULL ) {
                info->doing_add = TRUE;
                info->need_refresh = FALSE;
            } else {
                ErrorBox( hwnd, STR_UNABLE_TO_ADD,
                          MB_OK| MB_ICONINFORMATION );
            }
            break;
        }
        break;
    case WM_USER:
        /* an owned window is being destroyed make sure
         * Windows doesn't take the focus away from us*/
        NoRefresh = TRUE;
        SetFocus( hwnd );
        NoRefresh = FALSE;
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( FALSE );

} /* HeapWalkProc */


void EndAdd( void ) {

    GblWndInfo  *info;
    char        search[2];

    search[0] = '*';
    search[1] = '\0';
    info = (GblWndInfo *)GetWindowLong( HeapWalkMainWindow, 0 );
    info->doing_add = FALSE;
    SetMenusForAdd( HeapWalkMainWindow, FALSE );
    if( info->need_refresh ) {
        InitHeapList( info->list.box, TRUE );
    }
    SetListBoxForAdd( info->list.box, FALSE );
} /* EndAdd */
