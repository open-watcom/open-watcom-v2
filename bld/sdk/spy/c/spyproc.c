/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Spy window procedure implementation.
*
****************************************************************************/


#include "spy.h"
#include "spydll.h"
#include "mark.h"
#include "aboutdlg.h"
#include "wwinhelp.h"
#include "log.h"
#ifdef __NT__
    #include <commctrl.h>
#endif


static bool     spyAll;
static WORD     statusHite = 25;

static const MenuItemHint menuHints[] = {
    SPY_SAVE,                       STR_HINT_SAVE,
    SPY_SAVE_AS,                    STR_HINT_SAVE_AS,
    SPY_LOG,                        STR_HINT_LOG,
    SPY_PAUSE_LOG,                  STR_HINT_PAUSE_LOG,
    SPY_CONFIG_LOG,                 STR_HINT_CONFIG_LOG,
    SPY_SET_FONT,                   STR_HINT_SET_FONT,
    SPY_TOP,                        STR_HINT_TOP,
    SPY_EXIT,                       STR_HINT_EXIT,
    SPY_CLEAR_MESSAGES,             STR_HINT_CLEAR_MESSAGES,
    SPY_AUTO_SCROLL,                STR_HINT_AUTO_SCROLL,
    SPY_MARK,                       STR_HINT_MARK,
    SPY_WINDOW,                     STR_HINT_WINDOW,
    SPY_ADD_WINDOW,                 STR_HINT_ADD_WINDOW,
    SPY_ANOTHER_WINDOW,             STR_HINT_ADD_WINDOW,
    SPY_ALL_WINDOWS,                STR_HINT_ALL_WINDOWS,
    SPY_OFFON,                      STR_HINT_OFFON,
    SPY_STOP,                       STR_HINT_STOP,
    SPY_PEEK_WINDOW,                STR_HINT_PEEK_WINDOW,
    SPY_SHOW_SELECTED_WINDOWS,      STR_HINT_SHOW_SELECTED_WINDOWS,
    SPY_MESSAGES_WATCH,             STR_HINT_MESSAGES_WATCH,
    SPY_MESSAGES_STOP,              STR_HINT_MESSAGES_STOP,
    SPY_MESSAGES_ASCFG,             STR_HINT_MESSAGES_ASCFG,
    SPY_MESSAGES_SAVE,              STR_HINT_MESSAGES_SAVE,
    SPY_MESSAGES_LOAD,              STR_HINT_MESSAGES_LOAD,
    SPY_ABOUT,                      STR_HINT_ABOUT,
    SPY_SHOW_TOOLBAR,               STR_HINT_SHOW_TOOLBAR,
    SPY_SHOW_HELP,                  STR_HINT_SHOW_HELP,
    SPY_HELP_CONTENTS,              STR_HINT_HELP_CONTENTS,
    SPY_HELP_SRCH,                  STR_HINT_HELP_SRCH,
    SPY_HELP_ON_HELP,               STR_HINT_HELP_ON_HELP
};


/*
 * enableSpy - turn on spying
 */
static void enableSpy( void )
{
    SetFilter();
    EnableMenuItem( SpyMenu, SPY_ADD_WINDOW, MF_ENABLED );
    EnableMenuItem( SpyMenu, SPY_STOP, MF_ENABLED );
    ClearMessageCount();
    CheckMenuItem( SpyMenu, SPY_ALL_WINDOWS, MF_UNCHECKED );
    spyAll = false;
    SetSpyState( ON );

} /* enableSpy */

/*
 * disableSpy - set spy to stopped state
 */
static void disableSpy( void )
{
    ClearFilter();
    CheckMenuItem( SpyMenu, SPY_ALL_WINDOWS, MF_UNCHECKED );
    EnableMenuItem( SpyMenu, SPY_ADD_WINDOW, MF_GRAYED );
    EnableMenuItem( SpyMenu, SPY_STOP, MF_GRAYED );
    SetSpyState( NEITHER );
    spyAll = false;

} /* disableSpy */

/*
 * setMultipleWindows
 */
static void setMultipleWindows( HWND hwnd )
{
    char        str[128];
    const char  *rcstr;

    rcstr = GetRCString( STR_MULTIPLE_WIN_TITLE );
    sprintf( str, rcstr, SpyName );
    SetWindowText( hwnd, str );

} /* setMultipleWindows */

/*
 * setSingleWindow - set window title for one window spyed on
 */
static void setSingleWindow( HWND hwnd, HWND selwin )
{
    char        str[128];
    char        tmp[32];
    const char  *fmtstr;
    int         len;
    char        hexstr[20];

    len = GetWindowText( selwin, tmp, sizeof( tmp ) );
    tmp[len] = '\0';
    GetHexStr( hexstr, (UINT_PTR)selwin, HWND_HEX_LEN );
    hexstr[HWND_HEX_LEN] = '\0';
    if( len == 0 ) {
        fmtstr = GetRCString( STR_1_WIN_TITLE );
        sprintf( str, fmtstr, SpyName, hexstr );
    } else {
        fmtstr = GetRCString( STR_1_NAMED_WIN_TITLE );
        sprintf( str, fmtstr, SpyName, hexstr, tmp );
    }
    SetWindowText( hwnd, str );

} /* setSingleWindow */

/*
 * setUpForPick - for windows Send a WM_TIMER message to the callback
 *              - for NT this function minimizes the spy window and sets
 *                a timer.  When the timer goes off the pick dialog is
 *                started.  This is a kludge to ensure that the screen is
 *                properly refreshed after the spy window is minimized before
 *                we grab the screen bitmap and put up the big 'invisible
 *                window'.  The time that we wait is not always enough
 *                but it is good enough for most cases and I don't want
 *                to make it so long that the pause is too obvious to the
 *                user
 */
static void setUpForPick( HWND hwnd, UINT_PTR timerid )
{
#ifdef __WINDOWS__
    SendMessage( hwnd, WM_TIMER, timerid, 0 );
#else
    ShowWindow( hwnd, SW_MINIMIZE );
    SetTimer( hwnd, timerid, 300, NULL );
#endif
}

static void doSpyAll( HWND hwnd, bool state )
{
    const char  *rcstr;
    char        tmp[32];

    if( !state ) {
        SendMessage( hwnd, WM_COMMAND, GET_WM_COMMAND_MPS( SPY_STOP, 0, 0 ) );
    }  else {
        spyAll = state;
        EnableMenuItem( SpyMenu, SPY_STOP, MF_ENABLED );
        CheckMenuItem( SpyMenu, SPY_ALL_WINDOWS, MF_CHECKED );
        SetFilter();
        ClearSelectedWindows();
        if( SpyState == NEITHER ) {
            ClearMessageCount();
        }
        rcstr = GetRCString( STR_ALL_WIN_TITLE );
        sprintf( tmp, rcstr, SpyName );
        SetWindowText( hwnd, tmp );
        EnableMenuItem( SpyMenu, SPY_ADD_WINDOW, MF_GRAYED );
    }
}

static void showHintBar( HWND hwnd )
{
    RECT        area;
    HWND        statushwnd;

    GetClientRect( hwnd, &area );
    if( SpyMainWndInfo.show_hints ) {
        statushwnd = GetHintHwnd( StatusHdl );
        if( area.bottom - area.top < TOOLBAR_HEIGHT + statusHite ) {
            ShowWindow( statushwnd, SW_HIDE );
        } else {
            ShowWindow( statushwnd, SW_SHOW );
        }
    }
}

static void markCallback( const char *res )
{
    SpyOut( res, NULL, "" );
}

/*
 * SpyWindowProc - handle messages for the spy appl.
 */
LRESULT CALLBACK SpyWindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    HWND        selwin;
    HWND        hinthwnd;
    ctl_id      cmdid = 0;
    RECT        area;
    bool        pausestate;
    bool        spyallstate;
    about_info  ai;
    HMENU       hmenu;

    switch( msg ) {
    case WM_CREATE:
        GetClientRect( hwnd, &area );
        hmenu = GetMenu( hwnd );
        area.top = area.bottom - statusHite;
        StatusHdl = HintWndCreate( hwnd, &area, Instance, NULL );
        statusHite = SizeHintBar( StatusHdl );
        SetHintsText( StatusHdl, menuHints, sizeof( menuHints ) / sizeof( MenuItemHint ) );
        if( SpyMainWndInfo.show_hints ) {
            CheckMenuItem( hmenu, SPY_SHOW_HELP, MF_CHECKED | MF_BYCOMMAND );
        } else {
            hinthwnd = GetHintHwnd( StatusHdl );
            ShowWindow( hinthwnd, SW_HIDE );
        }
        CreateSpyBox( hwnd );
        SET_WNDINFO( hwnd, (LONG_PTR)SpyListBox );
        CreateSpyTool( hwnd );
        ShowSpyTool( SpyMainWndInfo.show_toolbar );
        CheckMenuItem( hmenu, SPY_SHOW_TOOLBAR, MF_BYCOMMAND | (( SpyMainWndInfo.show_toolbar ) ? MF_CHECKED : MF_UNCHECKED) );
        LogInit( hwnd, Instance, LogSpyBoxHeader );
        CheckMenuItem( SpyMenu, SPY_AUTO_SCROLL, MF_CHECKED );
        EnableMenuItem( SpyMenu, SPY_ADD_WINDOW, MF_GRAYED );
        EnableMenuItem( SpyMenu, SPY_STOP, MF_GRAYED );
        EnableMenuItem( SpyMenu, SPY_OFFON, MF_GRAYED );
        if( SpyMainWndInfo.on_top ) {
            CheckMenuItem( hmenu, SPY_TOP, MF_CHECKED | MF_BYCOMMAND );
            SetWindowPos( hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        }
        break;
    case WM_TIMER:
        // See comment on setUpForPick
        KillTimer( hwnd, wparam );
        switch( wparam ) {
        case SPY_ADD_WINDOW:
            selwin = DoPickDialog( wparam );
            if( selwin != NULL ) {
                setMultipleWindows( hwnd );
                AddSelectedWindow( selwin );
            }
            break;
        case SPY_PEEK_WINDOW:
            DoPickDialog( wparam );
            break;
        case SPY_WINDOW:
            selwin = DoPickDialog( cmdid );
            if( selwin != NULL ) {
                ClearSelectedWindows();
                setSingleWindow( hwnd, selwin );
                enableSpy();
                AddSelectedWindow( selwin );
            }
            break;
        }
        break;
#ifdef __NT__
    case WM_COPYDATA:
        HandleMessage( (LPMSG)((COPYDATASTRUCT *)lparam)->lpData );
        break;
#endif
    case WM_MENUSELECT:
        hinthwnd = GetHintHwnd( StatusHdl );
        HintMenuSelect( StatusHdl, hwnd, wparam, lparam );
        break;
    case WM_COMMAND:
        cmdid = LOWORD( wparam );
        switch( cmdid ) {
        case SPY_SHOW_HELP:
            SpyMainWndInfo.show_hints = !SpyMainWndInfo.show_hints;
            hmenu = GetMenu( hwnd );
            hinthwnd = GetHintHwnd( StatusHdl );
            if( SpyMainWndInfo.show_hints ) {
                CheckMenuItem( hmenu, SPY_SHOW_HELP, MF_CHECKED | MF_BYCOMMAND );
                showHintBar( hwnd );
            } else {
                CheckMenuItem( hmenu, SPY_SHOW_HELP, MF_UNCHECKED | MF_BYCOMMAND );
                ShowWindow( hinthwnd, SW_HIDE );
            }
            GetClientRect( hwnd, &area );
            ResizeSpyBox( area.right - area.left, area.bottom - area.top );
            break;
        case SPY_SHOW_TOOLBAR:
            SpyMainWndInfo.show_toolbar = !SpyMainWndInfo.show_toolbar;
            hmenu = GetMenu( hwnd );
            ShowSpyTool( SpyMainWndInfo.show_toolbar );
            CheckMenuItem( hmenu, SPY_SHOW_TOOLBAR, MF_BYCOMMAND | (( SpyMainWndInfo.show_toolbar ) ? MF_CHECKED : MF_UNCHECKED) );
            GetClientRect( hwnd, &area );
            ResizeSpyBox( area.right - area.left, area.bottom - area.top );
            break;
        case SPY_TOP:
            SpyMainWndInfo.on_top = !SpyMainWndInfo.on_top;
            hmenu = GetMenu( hwnd );
            if( SpyMainWndInfo.on_top ) {
                CheckMenuItem( hmenu, SPY_TOP, MF_CHECKED | MF_BYCOMMAND );
                SetWindowPos( hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                              SWP_NOMOVE | SWP_NOSIZE );
            } else {
                CheckMenuItem( hmenu, SPY_TOP, MF_UNCHECKED | MF_BYCOMMAND );
                SetWindowPos( hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                              SWP_NOMOVE | SWP_NOSIZE );
            }
            break;
        case SPY_MARK:
            pausestate = SpyMessagesPaused;
            SpyMessagesPaused = false;              /* make sure marks are
                                                     * always added */
            ProcessMark( hwnd, Instance, markCallback );
            SpyMessagesPaused = pausestate;
            break;
        case SPY_SET_FONT:
            if( ChooseMonoFont( hwnd ) )  {
                statusHite = SizeHintBar( StatusHdl );
                ResetSpyListBox();
                showHintBar( hwnd );
            }
            break;
        case SPY_SAVE_AS:
            SaveListBox( SLB_SAVE_AS, LogSpyBoxHeader, LogSpyBoxLine, "", SpyName, hwnd, SpyListBox );
            break;
        case SPY_SAVE:
            SaveListBox( SLB_SAVE_TMP, LogSpyBoxHeader, LogSpyBoxLine, ".\\wspy.txt", SpyName, hwnd, SpyListBox );
            break;
        case SPY_LOG:
            if( LogToggle() ) {
                CheckMenuItem( SpyMenu, SPY_LOG, MF_BYCOMMAND | MF_CHECKED );
            } else {
                CheckMenuItem( SpyMenu, SPY_LOG, MF_BYCOMMAND | MF_UNCHECKED );
                CheckMenuItem( SpyMenu, SPY_PAUSE_LOG, MF_BYCOMMAND | MF_UNCHECKED );
            }
            break;
        case SPY_CONFIG_LOG:
            LogConfigure();
            break;
        case SPY_EXIT:
            ClearFilter();
            DestroyWindow( hwnd );
            break;
        case SPY_LIST_BOX:
            switch( GET_WM_COMMAND_CMD( wparam, lparam ) ) {
            case LBN_ERRSPACE:
                ClearSpyBox();
                break;
            case LBN_DBLCLK:
                DoMessageSelDialog( hwnd );
                break;
            }
            break;
        case SPY_SHOW_SELECTED_WINDOWS:
            spyallstate = spyAll;
            DoShowSelectedDialog( hwnd, &spyallstate );
            if( spyallstate ) {
                doSpyAll( hwnd, spyallstate );
                if( spyAll ) {
                    SetSpyState( ON );
                }
                break;
            }
            if( WindowCount == 0 ) {
                SetWindowText( hwnd, SpyName );
                disableSpy();
                break;
            }

            if( WindowCount == 1 ) {
                setSingleWindow( hwnd, WindowList[0] );
            } else {
                setMultipleWindows( hwnd );
            }
            if( SpyState == NEITHER ) {
                enableSpy();
            }
            break;
        case SPY_HELP_CONTENTS:
            if( !WHtmlHelp( hwnd, "spy.chm", HELP_CONTENTS, 0 ) ) {
                WWinHelp( hwnd, "spy.hlp", HELP_CONTENTS, 0 );
            }
            break;
        case SPY_HELP_SRCH:
            if( !WHtmlHelp( hwnd, "spy.chm", HELP_PARTIALKEY, (HELP_DATA)(LPCSTR)"" ) ) {
                WWinHelp( hwnd, "spy.hlp", HELP_PARTIALKEY, (HELP_DATA)(LPCSTR)"" );
            }
            break;
        case SPY_HELP_ON_HELP:
            WWinHelp( hwnd, "winhelp.hlp", HELP_HELPONHELP, 0 );
            break;
        case SPY_ABOUT:
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
        case SPY_AUTO_SCROLL:
            SpyMessagesAutoScroll = !SpyMessagesAutoScroll;
            CheckMenuItem( SpyMenu, SPY_AUTO_SCROLL, ( SpyMessagesAutoScroll ) ? MF_CHECKED : MF_UNCHECKED );
            break;
        case SPY_PAUSE_LOG:
            if( LogPauseToggle() ) {
                CheckMenuItem( SpyMenu, SPY_PAUSE_LOG, MF_BYCOMMAND | MF_CHECKED );
            } else {
                CheckMenuItem( SpyMenu, SPY_PAUSE_LOG, MF_BYCOMMAND | MF_UNCHECKED );
            }
            break;
        case SPY_PAUSE_MESSAGES:
            SpyMessagePauseToggle();
            break;
        case SPY_CLEAR_MESSAGES:
            ClearSpyBox();
            ClearMessageCount();
            break;
        case SPY_MESSAGES_ASCFG:
            AutoSaveConfig = !AutoSaveConfig;
            CheckMenuItem( SpyMenu, SPY_MESSAGES_ASCFG, ( AutoSaveConfig ) ? MF_CHECKED : MF_UNCHECKED );
            break;
        case SPY_MESSAGES_SAVE:
            DoSaveSpyConfig();
            break;
        case SPY_MESSAGES_LOAD:
            DoLoadSpyConfig();
            break;
        case SPY_MESSAGES_WATCH:
        case SPY_MESSAGES_STOP:
            DoMessageDialog( hwnd, cmdid );
            break;
        case SPY_OFFON:
            if( SpyState != NEITHER ) {
                SetSpyState( !SpyState );
            }
            break;
        case SPY_STOP:
            disableSpy();
            ClearSelectedWindows();
            SetWindowText( hwnd, SpyName );
            break;
        case SPY_ANOTHER_WINDOW:
            if( SpyState == NEITHER || spyAll ) {
                SendMessage( hwnd, WM_COMMAND, GET_WM_COMMAND_MPS( SPY_WINDOW, 0, 0 ) );
            } else {
                SendMessage( hwnd, WM_COMMAND, GET_WM_COMMAND_MPS( SPY_ADD_WINDOW, 0, 0 ) );
            }
            break;
        case SPY_PEEK_WINDOW:
        case SPY_ADD_WINDOW:
        case SPY_WINDOW:
            setUpForPick( hwnd, cmdid );
            break;
        case SPY_ALL_WINDOWS:
            doSpyAll( hwnd, !spyAll );
            if( spyAll ) {
                SetSpyState( ON );
            }
            break;
        }
        break;
#ifdef __NT__
    case WM_NOTIFY:
        if( ((NMHDR *)lparam)->code == NM_DBLCLK &&
            ((NMHDR *)lparam)->idFrom == SPY_LIST_BOX ) {
            DoMessageSelDialog( hwnd );
        }
        break;
#endif
    case WM_CLOSE:
        PostMessage( hwnd, WM_COMMAND, GET_WM_COMMAND_MPS( SPY_EXIT, 0, 0 ) );
        break;
    case WM_ENDSESSION:
        if( wparam ) {
            SpyFini();
        }
        break;
    case WM_DESTROY:
        HintWndDestroy( StatusHdl );
        HintFini();
        StatusWndFini();
        DestroyMonoFonts();
        DestroySpyTool();
        WWinHelp( hwnd, "spy.hlp", HELP_QUIT, 0 );
        PostQuitMessage( 0 );
        break;
    case WM_MOVE:
        GetWindowRect( hwnd, &area );
        if( !SpyMainWndInfo.minimized ) {
            SpyMainWndInfo.last_xpos = SpyMainWndInfo.xpos;
            SpyMainWndInfo.last_ypos = SpyMainWndInfo.ypos;
            SpyMainWndInfo.xpos = area.left;
            SpyMainWndInfo.ypos = area.top;
        }
        break;
    case WM_SIZE:
        if( wparam != SIZE_MAXIMIZED && wparam != SIZE_MINIMIZED ) {
            GetWindowRect( hwnd, &area );
            SpyMainWndInfo.xsize = area.right - area.left;
            SpyMainWndInfo.ysize = area.bottom - area.top;
        } else {
            SpyMainWndInfo.xpos = SpyMainWndInfo.last_xpos;
            SpyMainWndInfo.ypos = SpyMainWndInfo.last_ypos;
        }
        SpyMainWndInfo.minimized = ( wparam == SIZE_MINIMIZED );
        GetClientRect( hwnd, &area );
        area.top = area.bottom - statusHite;
        hinthwnd = GetHintHwnd( StatusHdl );
        MoveWindow( hinthwnd, area.left, area.top, area.right - area.left, statusHite, TRUE );
        ResizeSpyBox( LOWORD( lparam ), HIWORD( lparam ) );
        ResizeSpyTool( LOWORD( lparam ), HIWORD( lparam ) );
        showHintBar( hwnd );
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
        break;
#ifdef __NT__
    case WM_ERASEBKGND: {
        static RECT r;
        GetClientRect( hwnd, &r );
        FillRect( (HDC)wparam, &r, (HBRUSH)(COLOR_BTNFACE + 1) );
        return 1;
    }
#endif
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0 );

} /* SpyWindowProc */
