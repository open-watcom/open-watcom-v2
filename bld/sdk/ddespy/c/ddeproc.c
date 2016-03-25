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
* Description:  Main window procedure for the DDE Spy.
*
****************************************************************************/


#include "commonui.h"
#include <string.h>
#include <math.h>
#include "wddespy.h"
#include "aboutdlg.h"
#include "wwinhelp.h"
#include "jdlg.h"

static FARPROC          DDEMsgFp;
static const MenuItemHint menuHints[] = {
    DDEMENU_SAVE,                   STR_HINT_SAVE,
    DDEMENU_SAVE_AS,                STR_HINT_SAVE_AS,
    DDEMENU_LOG_FILE,               STR_HINT_LOG_FILE,
    DDEMENU_LOG_PAUSE,              STR_HINT_LOG_PAUSE,
    DDEMENU_LOG_CONFIG,             STR_HINT_LOG_CONFIG,
    DDEMENU_FONT,                   STR_HINT_FONT,
    DDEMENU_TOOLBAR,                STR_HINT_TOOLBAR,
    DDEMENU_HINTBAR,                STR_HINT_HINTBAR,
    DDEMENU_TOP,                    STR_HINT_TOP,
    DDEMENU_EXIT,                   STR_HINT_EXIT,
    DDEMENU_CLEAR,                  STR_HINT_CLEAR,
    DDEMENU_MARK,                   STR_HINT_MARK,
    DDEMENU_SCREEN_OUT,             STR_HINT_SCREEN_OUT,
    DDEMENU_SCROLL,                 STR_HINT_SCROLL,
    DDEMENU_MON_POST,               STR_HINT_MON_POST,
    DDEMENU_MON_SENT,               STR_HINT_MON_SENT,
    DDEMENU_MON_STR,                STR_HINT_MON_STR,
    DDEMENU_MON_CB,                 STR_HINT_MON_CB,
    DDEMENU_MON_ERR,                STR_HINT_MON_ERR,
    DDEMENU_MON_CONV,               STR_HINT_MON_CONV,
    DDEMENU_MON_LNK,                STR_HINT_MON_LNK,
    DDEMENU_MSG_FILTER,             STR_HINT_MSG_FILTER,
    DDEMENU_CB_FILTER,              STR_HINT_CB_FILTER,
    DDEMENU_TRK_STR,                STR_HINT_TRK_STR,
    DDEMENU_TRK_CONV,               STR_HINT_TRK_CONV,
    DDEMENU_TRK_LINK,               STR_HINT_TRK_LINK,
    DDEMENU_TRK_SERVER,             STR_HINT_TRK_SERVER,
    DDEMENU_NO_ALIAS,               STR_HINT_NO_ALIAS,
    DDEMENU_ALIAS_PURGE,            STR_HINT_ALIAS_PURGE,
    DDEMENU_HWND_ALIAS,             STR_HINT_HWND_ALIAS,
    DDEMENU_TASK_ALIAS,             STR_HINT_TASK_ALIAS,
    DDEMENU_CONV_ALIAS,             STR_HINT_CONV_ALIAS,
    DDEMENU_HELP_CONTENTS,          STR_HINT_HELP_CONTENTS,
    DDEMENU_HELP_SRCH,              STR_HINT_HELP_SRCH,
    DDEMENU_HELP_ON_HELP,           STR_HINT_HELP_ON_HELP,
    DDEMENU_ABOUT,                  STR_HINT_ABOUT
};

/*
 * SetMainWndDefault - set the MainWndConfig structure to the default
 *                     values for size/position of the main window
 */
void SetMainWndDefault( void )
{
    MainWndConfig.visible = true;
    MainWndConfig.xpos = 0;
    MainWndConfig.ypos = 0;
    MainWndConfig.xsize = GetSystemMetrics( SM_CXSCREEN );
    MainWndConfig.ysize = 150;
    MainWndConfig.state = 0;

} /* SetMainWndDefault */

/*
 * initMonitoring - check the appropriate menu items to reflect the
 *                  current monitoring state
 */
static void initMonitoring( HWND hwnd )
{
    HMENU       mh;
    unsigned    i;

    mh = GetMenu( hwnd );
    for( i = 0; i < MAX_DDE_MON; i++ ) {
        if( Monitoring[i] ) {
            CheckMenuItem( mh, DDE_MON_FIRST + i, MF_BYCOMMAND | MF_CHECKED );
        }
    }

} /* initMonitoring */

/*
 * monitorChange - change the check state a menu item to reflect a
 *                 change in the monitoring state
 */
static void monitorChange( HWND hwnd, unsigned i )
{
    UINT        action;
    HMENU       mh;

    action = MF_BYCOMMAND;
    mh = GetMenu( hwnd );
    Monitoring[i] = !Monitoring[i];
    action |= ( Monitoring[i] ) ? MF_CHECKED : MF_UNCHECKED;
    CheckMenuItem( mh, DDE_MON_FIRST + i, action );

} /* monitorChange */

/*
 * resetFonts - repaint things and recalculate the size of things after
 *              a font change
 */
static void resetFonts( DDEWndInfo *info )
{
    HFONT       font;
    HDC         dc;
    LONG        new_char_extent;
    SIZE        sz;

    font = GetMonoFont();
    dc = GetDC( info->list.box );
    font = SelectObject( dc, font );
    GetTextExtentPoint( dc, "0000000000", 10, &sz );
    new_char_extent = sz.cx / 10;
    SelectObject( dc, font );
    ReleaseDC( info->list.box, dc );

    /* estimate the new width of the text in the list box */
    info->horz_extent = info->horz_extent * (new_char_extent / info->char_extent + 1);
    info->char_extent = new_char_extent;
    SendMessage( info->list.box, LB_SETHORIZONTALEXTENT, info->horz_extent, 0L );
    SendMessage( info->list.box, WM_SETFONT, (WPARAM)GetMonoFont(), MAKELONG( TRUE, 0 ) );
    SetTrackFont();

} /* resetFonts */

/*
 * hideHintBar - hide or show the hint status bar
 */
static void hideHintBar( HWND hwnd, DDEWndInfo *info, BOOL hide )
{
    HMENU               mh;
    RECT                area;
    HWND                hinthwnd;
    WORD                height;

    mh = GetMenu( hwnd );
    hinthwnd = GetHintHwnd( info->hintbar );
    GetClientRect( hwnd, &area );
    height = area.bottom - area.top;
    if( hide ) {
        info->list.hinthite = 0;
        ShowWindow( hinthwnd, SW_HIDE );
        CheckMenuItem( mh, DDEMENU_HINTBAR, MF_BYCOMMAND | MF_UNCHECKED );
    } else {
        info->list.hinthite = SizeHintBar( info->hintbar );
        if( height < info->list.ypos + info->list.hinthite ) {
            ShowWindow( hinthwnd, SW_HIDE );
        } else {
            ShowWindow( hinthwnd, SW_SHOW );
        }
        CheckMenuItem( mh, DDEMENU_HINTBAR, MF_BYCOMMAND | MF_CHECKED );
    }
    ResizeListBox( (WORD)( area.right - area.left ), height, &info->list );

} /* hideHintBar */

/*
 * DDEMainWndProc - process messages for the main window
 */
LRESULT CALLBACK DDEMainWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    DDEWndInfo          *info;
    char                *alias_title;
    FARPROC             fp;
    RECT                area;
    HMENU               mh;
    UINT                flag;
    HDC                 dc;
    HFONT               font;
    about_info          ai;
    unsigned            cmd;
    SIZE                sz;
    HWND                hinthwnd;
    BOOL                alias_state;

    info = (DDEWndInfo *)GET_WNDINFO( hwnd );
    switch( msg ) {
    case WM_CREATE:
        DDEMainWnd = hwnd;
        MainWndConfig.hwnd = hwnd;
        DDEMsgFp = MakeProcInstance( (FARPROC)DDEMsgProc, Instance );
        initMonitoring( hwnd );
        DdeInitialize( &DDEInstId, (PFNCALLBACK)DDEMsgFp,
                       APPCLASS_MONITOR | MF_CALLBACKS | MF_CONV |
                       MF_ERRORS | MF_HSZ_INFO | MF_LINKS |
                       MF_POSTMSGS | MF_SENDMSGS, 0L );
        info = MemAlloc( sizeof( DDEWndInfo ) );
        memset( info, 0, sizeof( DDEWndInfo ) );
        memset( &area, 0, sizeof( RECT ) );
        info->hintbar = HintWndCreate( hwnd, &area, Instance, NULL );
        SetHintsText( info->hintbar, menuHints, sizeof( menuHints ) / sizeof( MenuItemHint ) );
        hinthwnd = GetHintHwnd( info->hintbar );
        info->list.ypos = 0;
        info->horz_extent = 0;
        CreateListBox( hwnd, &info->list );
        font = GetMonoFont();
        dc = GetDC( info->list.box );
        font = SelectObject( dc, font );
        GetTextExtentPoint( dc, "0000000000", 10, &sz );
        info->char_extent = sz.cx / 10;
        SelectObject( dc, font );
        ReleaseDC( info->list.box, dc );
        SET_WNDINFO( hwnd, (LONG_PTR)info );
        InitAliases();
        LogInit( hwnd, Instance, LogHeader );
        MakeDDEToolBar( hwnd );
        mh = GetMenu( hwnd );
        if( ConfigInfo.scroll ) {
            CheckMenuItem( mh, DDEMENU_SCROLL, MF_BYCOMMAND | MF_CHECKED );
        }
        if( !ConfigInfo.alias ) {
            CheckMenuItem( mh, DDEMENU_NO_ALIAS, MF_BYCOMMAND | MF_CHECKED );
        }
        if( ConfigInfo.screen_out ) {
            CheckMenuItem( mh, DDEMENU_SCREEN_OUT, MF_BYCOMMAND | MF_CHECKED );
        }
        hideHintBar( hwnd, info, !ConfigInfo.show_hints );
        if( ConfigInfo.on_top ) {
            CheckMenuItem( mh, DDEMENU_TOP, MF_CHECKED | MF_BYCOMMAND );
            SetWindowPos( hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        }
        break;
    case WM_MENUSELECT:
        HintMenuSelect( info->hintbar, hwnd, wparam, lparam );
        break;
    case WM_MOVE:
        GetWindowRect( hwnd, &area );
        if( MainWndConfig.state != SIZE_MINIMIZED ) {
            MainWndConfig.last_xpos = MainWndConfig.xpos;
            MainWndConfig.last_ypos = MainWndConfig.ypos;
            MainWndConfig.xpos = area.left;
            MainWndConfig.ypos = area.top;
        }
        break;
    case WM_SIZE:
        ResizeListBox( LOWORD( lparam ), HIWORD( lparam ), &info->list );
        ResizeTB( hwnd );
        GetWindowRect( hwnd, &area );
        if( wparam != SIZE_MAXIMIZED && wparam != SIZE_MINIMIZED ) {
            MainWndConfig.xsize = area.right - area.left;
            MainWndConfig.ysize = area.bottom - area.top;
        } else {
            MainWndConfig.xpos = MainWndConfig.last_xpos;
            MainWndConfig.ypos = MainWndConfig.last_ypos;
        }
        MainWndConfig.state = wparam;
        GetClientRect( hwnd, &area );
        area.top = area.bottom - info->list.hinthite;
        hinthwnd = GetHintHwnd( info->hintbar );
        hideHintBar( hwnd, info, !ConfigInfo.show_hints );
        MoveWindow( hinthwnd, area.left, area.top,
                    area.right - area.left, info->list.hinthite, TRUE );
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case DDEMENU_MON_POST:
        case DDEMENU_MON_SENT:
        case DDEMENU_MON_STR:
        case DDEMENU_MON_CB:
        case DDEMENU_MON_ERR:
        case DDEMENU_MON_CONV:
        case DDEMENU_MON_LNK:
            monitorChange( hwnd, cmd - DDE_MON_FIRST );
            break;
        case DDEMENU_TRK_STR:
        case DDEMENU_TRK_CONV:
        case DDEMENU_TRK_LINK:
        case DDEMENU_TRK_SERVER:
            DisplayTracking( cmd - DDE_TRK_FIRST );
            break;
        case DDEMENU_SCREEN_OUT:
            ConfigInfo.screen_out = !ConfigInfo.screen_out;
            mh = GetMenu( hwnd );
            if( ConfigInfo.screen_out ) {
                CheckMenuItem( mh, DDEMENU_SCREEN_OUT, MF_BYCOMMAND | MF_CHECKED );
            } else {
                CheckMenuItem( mh, DDEMENU_SCREEN_OUT, MF_BYCOMMAND | MF_UNCHECKED );
            }
            break;
        case DDEMENU_EXIT:
            DestroyWindow( hwnd );
            break;
        case DDEMENU_CLEAR:
            info->horz_extent = 0;
            SendMessage( info->list.box, LB_SETHORIZONTALEXTENT, 0, 0L );
            SendMessage( info->list.box, LB_RESETCONTENT, 0, 0L );
            break;
        case DDEMENU_MARK:
            ProcessMark( hwnd, Instance, RecordMsg );
            break;
        case DDEMENU_SAVE:
            SaveListBox( SLB_SAVE_TMP, DumpHeader, ".\\wdde.txt", AppName,
                         hwnd, info->list.box );
            break;
        case DDEMENU_SAVE_AS:
            SaveListBox( SLB_SAVE_AS, DumpHeader, NULL, AppName,
                         hwnd, info->list.box );
            break;
        case DDEMENU_TOOLBAR:
            ToggleTB( hwnd );
            hideHintBar( hwnd, info, !ConfigInfo.show_hints );
            break;
        case DDEMENU_HINTBAR:
            ConfigInfo.show_hints = !ConfigInfo.show_hints;
            hideHintBar( hwnd, info, !ConfigInfo.show_hints );
            break;
        case DDEMENU_TOP:
            ConfigInfo.on_top = !ConfigInfo.on_top;
            mh = GetMenu( hwnd );
            if( ConfigInfo.on_top ) {
                CheckMenuItem( mh, DDEMENU_TOP, MF_CHECKED | MF_BYCOMMAND );
                SetWindowPos( hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
            } else {
                CheckMenuItem( mh, DDEMENU_TOP, MF_UNCHECKED | MF_BYCOMMAND );
                SetWindowPos( hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
            }
            break;
        case DDEMENU_LOG_FILE:
            mh = GetMenu( hwnd );
            flag = MF_BYCOMMAND;
            if( LogToggle() ) {
                flag |= MF_CHECKED;
            } else {
                flag |= MF_UNCHECKED;
                CheckMenuItem( mh, DDEMENU_LOG_PAUSE, flag );
            }
            CheckMenuItem( mh, DDEMENU_LOG_FILE, flag );
            break;
        case DDEMENU_LOG_PAUSE:
            mh = GetMenu( hwnd );
            flag = MF_BYCOMMAND;
            if( SpyLogPauseToggle() ) {
                flag |= MF_CHECKED;
            } else {
                flag |= MF_UNCHECKED;
            }
            CheckMenuItem( mh, DDEMENU_LOG_PAUSE, flag );
            break;
        case DDEMENU_FONT:
            if( ChooseMonoFont( hwnd ) ) {
                if( ConfigInfo.show_hints ) {
                    info->list.hinthite = SizeHintBar( info->hintbar );
                    hideHintBar( hwnd, info, !ConfigInfo.show_hints );
                }
                GetClientRect( hwnd, &area );
                ResizeListBox( (WORD)( area.right - area.left ), (WORD)( area.bottom - area.top ),
                               &info->list );
                resetFonts( info );
            }
            break;
        case DDEMENU_LOG_CONFIG:
            LogConfigure();
            break;
        case DDEMENU_MSG_FILTER:
            fp = MakeProcInstance( (FARPROC)FilterDlgProc, Instance );
            JDialogBoxParam( Instance, "MSG_FILTER_DLG", DDEMainWnd, (DLGPROC)fp, 1 );
            FreeProcInstance( fp );
            break;
        case DDEMENU_CB_FILTER:
            fp = MakeProcInstance( (FARPROC)FilterDlgProc, Instance );
            JDialogBoxParam( Instance, "CB_FILTER_DLG", DDEMainWnd, (DLGPROC)fp, 0 );
            FreeProcInstance( fp );
            break;
        case DDEMENU_ABOUT:
            ai.owner = hwnd;
            ai.inst = Instance;
            ai.name = AllocRCString( STR_ABOUT_NAME );
            ai.version = AllocRCString( STR_ABOUT_VERSION );
            ai.title = AllocRCString( STR_ABOUT_TITLE );
            DoAbout( &ai );
            FreeRCString( ai.name );
            FreeRCString( ai.version );
            FreeRCString( ai.title );
            break;
        case DDEMENU_HELP_CONTENTS:
            if( !WHtmlHelp( hwnd, DDE_CHM_FILE, HELP_CONTENTS, 0 ) ) {
                WWinHelp( hwnd, DDE_HELP_FILE, HELP_CONTENTS, 0 );
            }
            break;
        case DDEMENU_HELP_SRCH:
            if( !WHtmlHelp( hwnd, DDE_CHM_FILE, HELP_PARTIALKEY, (HELP_DATA)"" ) ) {
                WWinHelp( hwnd, DDE_HELP_FILE, HELP_PARTIALKEY, (HELP_DATA)"" );
            }
            break;
        case DDEMENU_HELP_ON_HELP:
            WWinHelp( hwnd, HELP_HELP_FILE, HELP_HELPONHELP, 0 );
            break;
        case DDEMENU_SCROLL:
            ConfigInfo.scroll = !ConfigInfo.scroll;
            mh = GetMenu( hwnd );
            flag = MF_BYCOMMAND;
            if( ConfigInfo.scroll ) {
                flag |= MF_CHECKED;
            } else {
                flag |= MF_UNCHECKED;
            }
            CheckMenuItem( mh, DDEMENU_SCROLL, flag );
            break;
        case DDEMENU_HWND_ALIAS:
            alias_title = AllocRCString( STR_ADD_HWND_ALIAS );
            Query4Aliases( HwndAlias, Instance, hwnd, alias_title );
            FreeRCString( alias_title );
            break;
        case DDEMENU_TASK_ALIAS:
            alias_title = AllocRCString( STR_ADD_TASK_ALIAS );
            Query4Aliases( TaskAlias, Instance, hwnd, alias_title );
            FreeRCString( alias_title );
            break;
        case DDEMENU_CONV_ALIAS:
            alias_title = AllocRCString( STR_ADD_CONV_ALIAS );
            Query4Aliases( ConvAlias, Instance, hwnd, alias_title );
            FreeRCString( alias_title );
            break;
        case DDEMENU_ALIAS_PURGE:
            alias_state = ConfigInfo.alias;
            ConfigInfo.alias = FALSE;
            RefreshAliases();
            ConfigInfo.alias = alias_state;
            FreeAlias( ConvAlias );
            FreeAlias( TaskAlias );
            FreeAlias( HwndAlias );
            InitAliases();
            break;
        case DDEMENU_NO_ALIAS:
            ConfigInfo.alias = !ConfigInfo.alias;
            mh = GetMenu( hwnd );
            if( !ConfigInfo.alias ) {
                CheckMenuItem( mh, DDEMENU_NO_ALIAS, MF_BYCOMMAND | MF_CHECKED );
            } else {
                CheckMenuItem( mh, DDEMENU_NO_ALIAS, MF_BYCOMMAND | MF_UNCHECKED );
            }
            RefreshAliases();
            break;
        default:
            return( DefWindowProc( hwnd, msg, wparam, lparam ) );
            break;
        }
        break;
    case WM_ENDSESSION:
        if( wparam ) {
            SaveConfigFile();
            SpyLogClose();
        }
        break;
    case WM_DESTROY:
        HintWndDestroy( info->hintbar );
        HintFini();
        SpyLogClose();
        FreeProcInstance( DDEMsgFp );
        SaveConfigFile();
        FiniTrackWnd();
        DDEToolBarFini();
        DestroyMonoFonts();
        FiniRCStrings();
        WWinHelp( hwnd, DDE_HELP_FILE, HELP_QUIT, 0 );
        PostQuitMessage( 0 );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( TRUE );

} /* DDEMainWndProc */
