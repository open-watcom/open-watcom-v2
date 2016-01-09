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
* Description:  Spy window procedure implementation.
*
****************************************************************************/


#include "bool.h"
#include "spy.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mark.h"
#include "aboutdlg.h"
#include "wwinhelp.h"

static BOOL     spyAll;
static WORD     statusHite = 25;

static MenuItemHint menuHints[] = {
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
    SetFilter( HandleMessageInst );
    EnableMenuItem( SpyMenu, SPY_ADD_WINDOW, MF_ENABLED );
    EnableMenuItem( SpyMenu, SPY_STOP, MF_ENABLED );
    ClearMessageCount();
    CheckMenuItem( SpyMenu, SPY_ALL_WINDOWS, MF_UNCHECKED );
    spyAll = FALSE;
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
    spyAll = FALSE;

} /* disableSpy */

/*
 * setMultipleWindows
 */
static void setMultipleWindows( HWND hwnd )
{
    char        str[128];
    char        *rcstr;

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
    char        *fmtstr;
    int         len;

    len = GetWindowText( selwin, tmp, sizeof( tmp ) );
    tmp[len] = 0;
    if( len == 0 ) {
        fmtstr = GetRCString( STR_1_WIN_TITLE );
        sprintf( str, fmtstr, SpyName, UINT_STR_LEN, (UINT)(pointer_int)selwin );
    } else {
        fmtstr = GetRCString( STR_1_NAMED_WIN_TITLE );
        sprintf( str, fmtstr, SpyName, UINT_STR_LEN, (UINT)(pointer_int)selwin, tmp );
    }
    SetWindowText( hwnd, str );

} /* setSingleWindow */

/*
 * SaveExtra - save extra to file
 */
void SaveExtra( FILE *f )
{
    SpyLogTitle( f );

} /* SaveExtra */


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
void setUpForPick( HWND hwnd, WORD cmdid ) {

#ifdef __WINDOWS__
    SendMessage( hwnd, WM_TIMER, cmdid, 0 );
#else
    ShowWindow( hwnd, SW_MINIMIZE );
    SetTimer( hwnd, cmdid, 300, NULL );
#endif
}

static void doSpyAll( HWND hwnd, BOOL state ) {

    char        *rcstr;
    char        tmp[32];

    if( !state ) {
        SendMessage( hwnd, WM_COMMAND, GET_WM_COMMAND_MPS( SPY_STOP, 0, 0 ) );
    }  else {
        spyAll = state;
        EnableMenuItem( SpyMenu, SPY_STOP, MF_ENABLED );
        CheckMenuItem( SpyMenu, SPY_ALL_WINDOWS, MF_CHECKED );
        SetFilter( HandleMessageInst );
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

static void showHintBar( HWND hwnd ) {

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

void markCallback( char *res )
{
    SpyOut( res, NULL );
}

/*
 * SpyWindowProc - handle messages for the spy appl.
 */
LRESULT CALLBACK SpyWindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    int         check;
    HWND        selwin;
    HWND        hinthwnd;
    WORD        cmdid = 0;
    RECT        area;
    BOOL        pausestate;
    BOOL        spyallstate;
    about_info  ai;
    HMENU       mh;

    switch ( msg ) {
    case WM_CREATE:
        GetClientRect( hwnd, &area );
        mh = GetMenu( hwnd );
        area.top = area.bottom - statusHite;
        StatusHdl = HintWndCreate( hwnd, &area, Instance, NULL );
        statusHite = SizeHintBar( StatusHdl );
        SetHintsText( StatusHdl, menuHints, sizeof( menuHints ) / sizeof( MenuItemHint ) );
        if( SpyMainWndInfo.show_hints ) {
            CheckMenuItem( mh, SPY_SHOW_HELP, MF_CHECKED | MF_BYCOMMAND );
        } else {
            hinthwnd = GetHintHwnd( StatusHdl );
            ShowWindow( hinthwnd, SW_HIDE );
        }
        CreateSpyBox( hwnd );
        SET_WNDINFO( hwnd, (LONG_PTR)SpyListBox );
        CreateSpyTool( hwnd );
        ShowSpyTool( SpyMainWndInfo.show_toolbar );
        if( SpyMainWndInfo.show_toolbar ) {
            CheckMenuItem( mh, SPY_SHOW_TOOLBAR, MF_CHECKED | MF_BYCOMMAND );
        }
        LogInit( hwnd, Instance, SpyLogTitle );
        CheckMenuItem( SpyMenu, SPY_AUTO_SCROLL, MF_CHECKED );
        EnableMenuItem( SpyMenu, SPY_ADD_WINDOW, MF_GRAYED );
        EnableMenuItem( SpyMenu, SPY_STOP, MF_GRAYED );
        EnableMenuItem( SpyMenu, SPY_OFFON, MF_GRAYED );
        if( SpyMainWndInfo.on_top ) {
            CheckMenuItem( mh, SPY_TOP, MF_CHECKED | MF_BYCOMMAND );
            SetWindowPos( hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                          SWP_NOMOVE | SWP_NOSIZE );
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
            mh = GetMenu( hwnd );
            hinthwnd = GetHintHwnd( StatusHdl );
            if( SpyMainWndInfo.show_hints ) {
                CheckMenuItem( mh, SPY_SHOW_HELP, MF_CHECKED | MF_BYCOMMAND );
                showHintBar( hwnd );
            } else {
                CheckMenuItem( mh, SPY_SHOW_HELP, MF_UNCHECKED | MF_BYCOMMAND );
                ShowWindow( hinthwnd, SW_HIDE );
            }
            GetClientRect( hwnd, &area );
            ResizeSpyBox( area.right - area.left, area.bottom - area.top );
            break;
        case SPY_SHOW_TOOLBAR:
            SpyMainWndInfo.show_toolbar = !SpyMainWndInfo.show_toolbar;
            mh = GetMenu( hwnd );
            if( SpyMainWndInfo.show_toolbar ) {
                CheckMenuItem( mh, SPY_SHOW_TOOLBAR, MF_CHECKED | MF_BYCOMMAND );
            } else {
                CheckMenuItem( mh, SPY_SHOW_TOOLBAR, MF_UNCHECKED | MF_BYCOMMAND );
            }
            ShowSpyTool( SpyMainWndInfo.show_toolbar );
            GetClientRect( hwnd, &area );
            ResizeSpyBox( area.right - area.left, area.bottom - area.top );
            break;
        case SPY_TOP:
            SpyMainWndInfo.on_top = !SpyMainWndInfo.on_top;
            mh = GetMenu( hwnd );
            if( SpyMainWndInfo.on_top ) {
                CheckMenuItem( mh, SPY_TOP, MF_CHECKED | MF_BYCOMMAND );
                SetWindowPos( hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                              SWP_NOMOVE | SWP_NOSIZE );
            } else {
                CheckMenuItem( mh, SPY_TOP, MF_UNCHECKED | MF_BYCOMMAND );
                SetWindowPos( hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                              SWP_NOMOVE | SWP_NOSIZE );
            }
            break;
        case SPY_MARK:
            pausestate = SpyMessagesPaused;
            SpyMessagesPaused = FALSE;              /* make sure marks are
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
            SaveListBox( SLB_SAVE_AS, SaveExtra, "", SpyName, hwnd, SpyListBox );
            break;
        case SPY_SAVE:
            SaveListBox( SLB_SAVE_TMP, SaveExtra, ".\\wspy.txt", SpyName, hwnd,
                         SpyListBox );
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
            if( !WHtmlHelp( hwnd, "spy.chm", HELP_PARTIALKEY, (HELP_DATA)"" ) ) {
                WWinHelp( hwnd, "spy.hlp", HELP_PARTIALKEY, (HELP_DATA)"" );
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
            ai.first_cr_year = "1993";
            ai.title = AllocRCString( STR_ABOUT_TITLE );
            DoAbout( &ai );
            FreeRCString( ai.name );
            FreeRCString( ai.version );
            FreeRCString( ai.title );
            break;
        case SPY_AUTO_SCROLL:
            if( SpyMessagesAutoScroll ) {
                SpyMessagesAutoScroll = FALSE;
                CheckMenuItem( SpyMenu, SPY_AUTO_SCROLL, MF_UNCHECKED );
            } else {
                SpyMessagesAutoScroll = TRUE;
                CheckMenuItem( SpyMenu, SPY_AUTO_SCROLL, MF_CHECKED );
            }
            break;
        case SPY_PAUSE_LOG:
            if( SpyLogPauseToggle() ) {
                CheckMenuItem( SpyMenu, SPY_PAUSE_LOG,
                               MF_BYCOMMAND | MF_CHECKED );
            } else {
                CheckMenuItem( SpyMenu, SPY_PAUSE_LOG,
                               MF_BYCOMMAND | MF_UNCHECKED );
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
            if( AutoSaveConfig ) {
                check = MF_UNCHECKED;
                AutoSaveConfig = FALSE;
            } else {
                AutoSaveConfig = TRUE;
                check = MF_CHECKED;
            }
            CheckMenuItem( SpyMenu, SPY_MESSAGES_ASCFG, check );
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
                SendMessage( hwnd, WM_COMMAND,
                             GET_WM_COMMAND_MPS( SPY_WINDOW, 0, 0 ) );
            } else {
                SendMessage( hwnd, WM_COMMAND,
                             GET_WM_COMMAND_MPS( SPY_ADD_WINDOW, 0, 0 ) );
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
        MoveWindow( hinthwnd, area.left, area.top,
                    area.right - area.left, statusHite, TRUE );
        ResizeSpyBox( LOWORD( lparam ), HIWORD( lparam ) );
        ResizeSpyTool( LOWORD( lparam ), HIWORD( lparam ) );
        showHintBar( hwnd );
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
        break;
#if defined( __NT__ )
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

