/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  GUI library main window procedure and other assorted guts
*
****************************************************************************/


#include "guiwind.h"
#include <stdlib.h>
#include <stdio.h>
#include "guiscale.h"
#include "guiutil.h"
#include "guixutil.h"
#include "guimenus.h"
#include "guipaint.h"
#include "guixwind.h"
#include "guimapky.h"
#include "initmode.h"
#include "guisysin.h"
#include "guisysfi.h"
#include "guitool.h"
#include "guiscrol.h"
#include "guiwinlp.h"
#include "guistr.h"
#include "guixhook.h"
#include "ctl3dcvr.h"
#include "guihook.h"
#include "guicolor.h"
#include "guicontr.h"
#include "guixdlg.h"
#include "guistyle.h"
#include "guifront.h"
#include "guisystr.h"
#include "guixmain.h"
#include "guirdlg.h"
#include "guimdi.h"
#include "guistat.h"


#if !defined(__NT__)
#define WM_PAINTICON            0x0026
#endif
#define WM_GUI_USEREVENT        (WM_USER+0x654)

#define GUI_CLASSNAME_MAX       64

#define BACKGROUND_STYLES       WS_CHILD | WS_VISIBLE
#define GUI_DIALOG_STYLE        DS_MODALFRAME

#define REGISTER_STYLE          CS_DBLCLKS
#define REGISTER_DIALOG_STYLE   CS_PARENTDC

#define SENDPOINTGUIEVENT(w, gm, wp, lp)    SendPointEvent( wp, lp, w, gm, true )

typedef struct wmcreate_info {
    int                 size;
    gui_window          *wnd;
    gui_create_info     *dlg_info;
} wmcreate_info;

bool            EditControlHasFocus = false;

gui_window      *GUICurrWnd = NULL;
WPI_INST        GUIMainHInst;

char            GUIClass[GUI_CLASSNAME_MAX + 1];
char            GUIDialogClass[GUI_CLASSNAME_MAX + 1] = "GUIDialogClass";
char            GUIDefaultClassName[] = "GUIClass";

static int                  NumWindows = 0;
static bool                 Posted = false;
static bool                 FirstInstance = true;
static gui_window_styles    Style;
#ifdef __OS2_PM__
static WPI_WNDPROC          oldFrameProc;
#endif

/* forward reference */

#ifdef __OS2_PM__
WPI_MRESULT CALLBACK GUIFrameProc( HWND, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
#endif
WPI_MRESULT CALLBACK GUIWindowProc( HWND, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );

/*
 * GUIXSetupWnd -- set the default values in the gui_window structure
 *                 struct will have been memset to 0
 */

void GUIXSetupWnd( gui_window *wnd )
{
    wnd->flags = NONE_SET;
    wnd->vrange = -1;
    wnd->hrange = -1;
}

static void GUISetWindowClassName( void )
{
    char        *class_name;

    class_name = GUIGetWindowClassName();           /* user replaceable stub function */
    if( class_name == NULL || *class_name == '\0' ) {
        class_name = GUIDefaultClassName;
    }
    strncpy( GUIClass, class_name, GUI_CLASSNAME_MAX );
    GUIClass[GUI_CLASSNAME_MAX] = '\0';
}

void GUIWantPartialRows( gui_window *wnd, bool want )
{
    if( wnd != NULL ) {
        if( want ) {
            wnd->flags |= PARTIAL_ROWS;
        } else {
            wnd->flags &= ~PARTIAL_ROWS;
        }
    }
}

void GUISetCheckResizeAreaForChildren( gui_window *wnd, bool check )
{
    /* unused parameters */ (void)wnd; (void)check;
}

/*
 * GUIDestroyWnd -- Destroy given window or all windows if NULL.
 *                  Window will not get WM_CLOSE message.
 */

void GUIDestroyWnd( gui_window *wnd )
{
    HWND        hwnd;
    gui_window  *curr;

    if( wnd == NULL ) {
        curr = GUIGetFront();
        while( curr != NULL ) {
            if( curr->flags & DOING_CLOSE ) {
                curr = GUIGetNextWindow( curr );
            } else if( _wpi_getparent( GUIGetParentFrameHWND( curr ) ) == HWND_DESKTOP ) {
                GUIDestroyWnd( curr );
                curr = GUIGetFront();
            } else {
                curr = GUIGetNextWindow( curr );
            }
        }
    } else {
        if ( GUIIsOpen( wnd ) ) {
            /* this will make a new window be chosen as current if this
             * window was current */
            hwnd = GUIGetParentFrameHWND( wnd );
            wnd->flags |= DOING_CLOSE;
            if( NumWindows == 1 || ( _wpi_getparent( hwnd ) == HWND_DESKTOP ) && (wnd->style & GUI_POPUP) == 0 ) {
                _wpi_sendmessage( hwnd, WM_CLOSE, 0, 0 );
            } else {
                DestroyWindow( hwnd );
            }
        }
    }
}

static bool DoRegisterClass( WPI_INST hinst, char *class_name, WPI_CLASSPROC win_call_back, UINT style, int extra )
{
#ifdef __OS2_PM__
    return( WinRegisterClass( hinst.hab, class_name, win_call_back, CS_CLIPCHILDREN | CS_SIZEREDRAW | CS_MOVENOTIFY | style, extra ) );
#else
    WNDCLASS    wc;

    wc.style = style;
    wc.lpfnWndProc = win_call_back;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = extra;
    wc.hInstance = hinst;
    wc.hIcon = _wpi_loadicon( NULLHANDLE, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULLHANDLE, IDC_ARROW );
    wc.hbrBackground = NULLHANDLE;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = class_name;

    return( RegisterClass( &wc ) != 0 );
#endif
}
/*
 * SetupClass - Register the GUIClass class
 */

static bool SetupClass( void )

{
    if( DoRegisterClass( GUIMainHInst, GUIClass, (WPI_CLASSPROC)GUIWindowProc,
                         REGISTER_STYLE, EXTRA_SIZE * NUM_EXTRA_WORDS ) ) {

        return( DoRegisterClass( GUIMainHInst, GUIDialogClass, (WPI_CLASSPROC)GUIWindowProc,
                                 REGISTER_DIALOG_STYLE, EXTRA_SIZE * NUM_EXTRA_WORDS ) );
    }
    return( false );
}

void GUICleanup( void )
{
    GUIDeath();                 /* user replaceable stub function */
    GUICleanupHotSpots();
    GUIFreeStatus();
    GUI3DDialogFini();
    GUIFiniDialog();
    GUISysFini();
    GUIFiniInternalStringTable();
    GUILoadStrFini();
}

#ifdef __OS2_PM__

HMQ             GUIPMmq;        /* debugger needs access to this */

int GUIXMain( int argc, char *argv[] )
{
    bool        ok;
    int         ret;
    bool        register_done;
    HAB         inst;

    inst = WinInitialize( 0 );
    if( inst == 0 ) {
        return( 0 );
    }

    GUIPMmq = WinCreateMsgQueue( inst, 0 );
    if( GUIPMmq == 0 ) {
        return( 0 );
    }

    register_done = false;

#else

int GUIXMain( int argc, char *argv[],
              WPI_INST inst, WPI_INST hPrevInstance, LPSTR lpCmdLine,
              int nShowCmd )
{
    bool        ok;
    int         ret;
    bool        register_done;

    ret = 0;
    lpCmdLine = lpCmdLine;
    nShowCmd = nShowCmd;
    register_done = ( hPrevInstance != 0 );
    FirstInstance = ( hPrevInstance == 0 );

#endif

    GUIMainTouched = true;

    GUIMemOpen();

    GUIStoreArgs( argv, argc );

    _wpi_setwpiinst( inst, 0, &GUIMainHInst );
    memcpy( &GUIResHInst, &GUIMainHInst, sizeof( WPI_INST ) ) ;

    ok = true;

    GUISetWindowClassName();

    ok = GUIFirstCrack();       /* user replaceable stub function */

    if( ok ) {
        if( !register_done ) {
            ok = SetupClass();
        }
    }

    if( ok ) {
        ok = GUILoadStrInit( argv[0] );
    }

    if( ok ) {
        ok = GUIInitInternalStringTable();
    }

    if( ok ) {
        GUIInitGUIMenuHint();
        GUImain();
    }

    if( GUIGetFront() == NULL && !Posted ) {  /* no windows created */
        _wpi_postquitmessage( 0 );
    }

    if( NumWindows ) {
        ret = GUIWinMessageLoop();
    }

#ifdef __OS2_PM__
    WinDestroyMsgQueue( GUIPMmq );
    WinTerminate( inst );
#endif
    GUICleanup();
    GUIDead();                  /* user replaceable stub function */
    GUIMemClose();
    return( ret );
}

/*
 * ShowWnd
 */

static void ShowWnd( HWND hwnd )
{
    if( hwnd == NULLHANDLE ) {
        GUIError( LIT( Open_Failed ) );
    } else {
        _wpi_showwindow( hwnd, SW_SHOWNORMAL );
        _wpi_updatewindow( hwnd );
    }
}

void GUIShowWindow( gui_window *wnd )
{
    GUIInvalidatePaintHandles( wnd );
    if( wnd->root != NULLHANDLE ) {
        ShowWnd( wnd->root_frame );
    }
    if( wnd->hwnd != NULLHANDLE ) {
        ShowWnd( wnd->hwnd_frame );
    }
}

void GUIShowWindowNA( gui_window *wnd )
{
    int flags;

    flags = SW_SHOWNA;
#ifdef __OS2_PM__
    flags |= SWP_ACTIVATE;
#endif
    GUIInvalidatePaintHandles( wnd );
    if( wnd->root_frame != NULLHANDLE ) {
        _wpi_showwindow( wnd->root_frame, flags );
    }
    if( ( wnd->hwnd_frame != NULLHANDLE ) && ( wnd->hwnd_frame != wnd->root_frame ) ) {
        _wpi_showwindow( wnd->hwnd_frame, flags );
    }
}

/*
 * GUIWndInit -- initialize display windows
 */

bool GUIWndInit( unsigned DClickInterval, gui_window_styles style )
{
    Style = style;
    GUISysInit( INIT_MOUSE_INITIALIZED );
    _wpi_setdoubleclicktime( DClickInterval );
    GUISetScreen( 0, 0, _wpi_getsystemmetrics( SM_CXSCREEN ), _wpi_getsystemmetrics( SM_CYSCREEN ) );
    GUIInitDialog();
    return( true );
}

static bool CreateBackgroundWnd( gui_window *wnd, gui_create_info *dlg_info )
{
    DWORD               style;
    wmcreate_info       wmcreateinfo;
    HWND                frame_hwnd;
    GUI_RECTDIM         left, top, right, bottom;

    _wpi_getclientrect( wnd->root, &wnd->hwnd_client_rect );
    _wpi_getrectvalues( wnd->hwnd_client_rect, &left, &top, &right, &bottom );

    style = BACKGROUND_STYLES;

    wmcreateinfo.size = sizeof(wmcreate_info);
    wmcreateinfo.wnd = wnd;
    wmcreateinfo.dlg_info = dlg_info;

    wnd->hwnd = NULLHANDLE;
    frame_hwnd = NULLHANDLE;

    _wpi_createanywindow( GUIClass, NULL, style,
                          0, 0, right-left, bottom-top,
                          wnd->root, NULLHANDLE, GUIMainHInst,
                          &wmcreateinfo, &wnd->hwnd, 0, &frame_hwnd );

    wnd->hwnd_frame = wnd->root_frame;

    return( wnd->hwnd != NULLHANDLE );
}

static void DoSetWindowLongPtr( HWND hwnd, gui_window *wnd )
{
    _wpi_setwindowlongptr( hwnd, GUI_CONTAINER_WORD1 * EXTRA_SIZE, 0L );
    _wpi_setwindowlongptr( hwnd, GUI_CONTAINER_WORD2 * EXTRA_SIZE, 0L );
    _wpi_setwindowlongptr( hwnd, GUI_EXTRA_WORD * EXTRA_SIZE, (LONG_PTR)wnd );
}

/*
 * GUIXCreateWindow
 */

bool GUIXCreateWindow( gui_window *wnd, gui_create_info *dlg_info, gui_window *parent )
{
    DWORD               style;
    HMENU               hmenu;
    gui_coord           pos;
    gui_coord           size;
    HWND                parent_hwnd;
    LPSTR               class_name;
    HWND                hwnd;
    wmcreate_info       wmcreateinfo;
    HWND                frame_hwnd;
    HWND                client_hwnd;
#ifdef __OS2_PM__
    ULONG               frame_flags;
    ULONG               flags;
    ULONG               show_flags;
    WPI_RECT            rect;
    WPI_RECT            parent_client_rect;
#else
    DWORD               exstyle;
#endif

#ifdef __OS2_PM__
// Get rid of the changable font style for PM GUI app's
    dlg_info->style &= ~GUI_CHANGEABLE_FONT;
#endif

    wnd->root_pinfo.force_count = NUMBER_OF_FORCED_REPAINTS;
    wnd->hwnd_pinfo.force_count = NUMBER_OF_FORCED_REPAINTS;

    wnd->parent = parent;
    style = COMMON_STYLES;
    if( parent == NULL ) {
        parent_hwnd = HWND_DESKTOP;
        if( (dlg_info->style & GUI_POPUP) == 0 ) {
            wnd->flags |= IS_ROOT;
        }
    } else {
        parent_hwnd = parent->hwnd;
        if( (dlg_info->style & GUI_POPUP) == 0 ) {
            style |= CHILD_STYLE;
        }
    }
    hmenu = NULLHANDLE;
    if( !GUISetupStruct( wnd, dlg_info, &pos, &size, parent_hwnd, &hmenu ) ) {
        return( false );
    }
    if( (wnd->style & GUI_NOFRAME) == 0 ) {
        if( dlg_info->title != NULL ) {
            style |= WS_CAPTION;
            wnd->flags |= HAS_CAPTION;
        } else {
            style |= WS_BORDER;
        }
    }
    if( dlg_info->style & GUI_RESIZEABLE ) {
        style |= WS_THICKFRAME;
    }
    if( dlg_info->scroll & GUI_HSCROLL ) {
        style |= WS_HSCROLL;
    }
    if( dlg_info->scroll & GUI_VSCROLL ) {
        style |= WS_VSCROLL;
    }
    if( wnd->flags & HAS_CAPTION ) {
        if( dlg_info->style & GUI_MAXIMIZE ) {
            style |= WS_MAXIMIZEBOX;
        }
        if( dlg_info->style & GUI_MINIMIZE ) {
            style |= WS_MINIMIZEBOX;
        }
        if( dlg_info->style & GUI_SYSTEM_MENU ) {
            style |= WS_SYSMENU;
        }
    } else {
        wnd->style &= ~(GUI_MAXIMIZE|GUI_MINIMIZE|GUI_SYSTEM_MENU);
        dlg_info->style &= ~(GUI_MAXIMIZE|GUI_MINIMIZE|GUI_SYSTEM_MENU);
    }
    if( dlg_info->style & GUI_POPUP ) {
        style |= WS_POPUP;
    }
    if( dlg_info->style & GUI_DIALOG_LOOK ) {
        style |= GUI_DIALOG_STYLE;
        class_name = GUIDialogClass;
    } else {
        class_name = GUIClass;
    }

    wnd->font = GUIGetSystemFont();
    GUIInitHint( wnd, dlg_info->menu.num_items, dlg_info->menu.menu, MENU_HINT );
    GUISetGUIHint( wnd );
    wmcreateinfo.size = sizeof(wmcreate_info);
    wmcreateinfo.wnd  = wnd;
    wmcreateinfo.dlg_info = dlg_info;
    hwnd        = NULLHANDLE;
    frame_hwnd  = NULLHANDLE;
    client_hwnd = NULLHANDLE;
#ifdef __OS2_PM__
    /* frame */
    flags = FCF_TASKLIST | FCF_NOBYTEALIGN;
    frame_flags = 0;
    if( wnd->flags & HAS_CAPTION ) {
        flags |= FCF_TITLEBAR;
        if( dlg_info->style & GUI_SYSTEM_MENU ) {
            flags |= FCF_SYSMENU;
        }
        if( dlg_info->style & GUI_MAXIMIZE ) {
            flags |= FCF_MAXBUTTON;
        }
        if( dlg_info->style & GUI_MINIMIZE ) {
            flags |= FCF_MINBUTTON;
        }
        flags |= FCF_BORDER;
    } else {
        if( (wnd->style & GUI_NOFRAME) == 0 ) {
            flags |= FCF_BORDER;
            frame_flags |= FS_BORDER;
        }
        wnd->style &= ~(GUI_MAXIMIZE|GUI_MINIMIZE|GUI_SYSTEM_MENU);
        dlg_info->style &= ~(GUI_MAXIMIZE|GUI_MINIMIZE|GUI_SYSTEM_MENU);
    }
    if( dlg_info->style & GUI_RESIZEABLE ) {
        flags |= FCF_SIZEBORDER;
    }
    if( parent_hwnd == HWND_DESKTOP ) {
        if( dlg_info->menu.num_items > 0 ) {
            //flags |= FCF_MENU;
        }
    }
    if( dlg_info->scroll & GUI_HSCROLL ) {
        flags |= FCF_HORZSCROLL;
    }
    if( dlg_info->scroll & GUI_VSCROLL ) {
        flags |= FCF_VERTSCROLL;
    }
    if( dlg_info->style & GUI_DIALOG_LOOK ) {
        flags |= FCF_DLGBORDER;
    }
    if( dlg_info->style & GUI_POPUP ) {
        flags |= FCF_NOMOVEWITHOWNER;
    }
#if defined( _M_I86 )
    frame_hwnd = WinCreateStdWindow( parent_hwnd, frame_flags | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, &flags,
                                     NULL, (char *)dlg_info->title,
                                     0, (HMODULE)0, 0, NULL );
#else
    frame_hwnd = WinCreateStdWindow( parent_hwnd, frame_flags | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, &flags,
                                     NULL, dlg_info->title,
                                     0, (HMODULE)0, 0, NULL );
#endif
    if( frame_hwnd != NULLHANDLE ) {
        oldFrameProc = _wpi_subclasswindow( frame_hwnd, GUIFrameProc );
        _wpi_setmenu( frame_hwnd, hmenu );
        _wpi_getclientrect( parent_hwnd, &parent_client_rect );
        show_flags = SWP_SIZE | SWP_MOVE;
        WinSetWindowPos( frame_hwnd, HWND_TOP, pos.x, pos.y, size.x, size.y, show_flags );
        hwnd = frame_hwnd;
        if( parent_hwnd == HWND_DESKTOP ) {
            wnd->root_frame = frame_hwnd;
        } else {
            wnd->hwnd_frame = frame_hwnd;
        }
        _wpi_getclientrect( frame_hwnd, &rect );
        client_hwnd = WinCreateWindow( frame_hwnd, class_name, NULL,
                                       WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                                       rect.xLeft, rect.yBottom,
                                       rect.xRight-rect.xLeft,
                                       rect.yTop-rect.yBottom,
                                       frame_hwnd, HWND_TOP, FID_CLIENT,
                                       &wmcreateinfo, NULL );
        if( client_hwnd == NULLHANDLE ) {
            WinDestroyWindow( frame_hwnd );
            frame_hwnd = NULLHANDLE;
            hwnd = NULLHANDLE;
        }
    }
#else
    exstyle = WS_EX_NOPARENTNOTIFY;
#ifdef __NT__
    if( dlg_info->style & GUI_3D_BORDER ) {
        exstyle |= WS_EX_CLIENTEDGE;
        style &= ~WS_BORDER;
    }
#endif
    hwnd = _wpi_createwindow_ex( exstyle, class_name, dlg_info->title, style, 0, 0, pos.x,
                                 pos.y, size.x, size.y, parent_hwnd, hmenu, GUIMainHInst,
                                 &wmcreateinfo, &frame_hwnd );
#endif
    if( hwnd == NULLHANDLE ) {
        return( false );
    }

    GUISetIcon( wnd, dlg_info->icon );

    if( dlg_info->style & (GUI_INIT_MAXIMIZED | GUI_INIT_MINIMIZED) ) {
        GUISetRedraw( wnd, false );
        if( dlg_info->style & GUI_INIT_MAXIMIZED ) {
            GUIMaximizeWindow( wnd );
        } else if( dlg_info->style & GUI_INIT_MINIMIZED ) {
            GUIMinimizeWindow( wnd );
        }
        GUISetRedraw( wnd, true );
        if( wnd->style & GUI_INIT_INVISIBLE ) {
            _wpi_showwindow( hwnd, SW_HIDE );
        }
    }

    if( (wnd->style & GUI_INIT_INVISIBLE) == 0 ) {
        GUIShowWindowNA( wnd );
    }

    return( wnd != NULL );
}

bool SendPointEvent( WPI_PARAM1 wparam, WPI_PARAM2 lparam,
                     gui_window *wnd, gui_event gui_ev, bool force_current )
{
    WPI_POINT   currentpoint;
    gui_point   point;

    if( force_current || !EditControlHasFocus ) {
        wparam = wparam;
        lparam = lparam;
        currentpoint.x = GET_WM_MOUSEMOVE_POSX( wparam, lparam );
        currentpoint.y = GET_WM_MOUSEMOVE_POSY( wparam, lparam );
        if( force_current && ( GUICurrWnd != wnd ) && ( GUICurrWnd != NULL ) ) {
            //wnd = GUICurrWnd;
        }
        currentpoint.y = _wpi_cvtc_y( wnd->hwnd, currentpoint.y );
        _wpi_clienttoscreen( wnd->hwnd, &currentpoint );
        GUIMakeRelative( wnd, &currentpoint, &point );
        GUIEVENT( wnd, gui_ev, &point );
        return( true );
    }
    return( false );
}


void GUIResizeBackground( gui_window *wnd, bool force_msg )
{
    WPI_RECT    status;
    int         t_height, s_height;
    GUI_RECTDIM left, top, right, bottom;
    gui_coord   size;

    if( wnd->root == NULLHANDLE ) {
        if( wnd->hwnd != NULLHANDLE ) {
            _wpi_getclientrect( wnd->hwnd, &wnd->hwnd_client_rect );
        }
        return;
    }

    t_height = 0;
    s_height = 0;

    if( ( wnd->tbinfo != NULL ) && ( wnd->tbinfo->info.is_fixed ) ) {
        t_height = _wpi_getheightrect( wnd->tbinfo->fixedrect );
    }

    if( wnd->status != NULLHANDLE ) {
        _wpi_getwindowrect( wnd->status, &status );
        s_height = _wpi_getheightrect( status );
    }

    _wpi_getclientrect( wnd->root_frame, &wnd->root_client_rect );
    _wpi_getrectvalues( wnd->root_client_rect, &left, &top, &right, &bottom );
    bottom -= top;
    right  -= left;
    top = left = 0;

#ifdef __OS2_PM__
    top    += s_height;
    bottom -= t_height;
#else
    top    += t_height;
    bottom -= s_height;
#endif

    /* if the root client is a separate window resize it too */
    _wpi_movewindow( wnd->hwnd, left, top, right - left, bottom - top, TRUE );

    _wpi_getclientrect( wnd->hwnd, &wnd->hwnd_client_rect );

    if( force_msg && (wnd->flags & SENT_INIT) ) {
        size.x = right - left;
        size.y = bottom - top;
        GUIScreenToScaleR( &size );
        GUIEVENT( wnd, GUI_RESIZE, &size );
    }
}

#ifndef __OS2_PM__
static bool SetFocusToParent( void )
{
    HWND        curr_hwnd;

    // for a reason I do not yet comprehend, the MDI stuff should no longer
    // be a deterent to setting the focus to the top parent
    if( ( GUICurrWnd != NULL ) && !EditControlHasFocus /* && !GUIMDI */
      && (GUICurrWnd->style & GUI_POPUP) == 0 ) {
        curr_hwnd = GUIGetTopParentHWND( GUICurrWnd->hwnd );
        if( GUIGetCtrlWnd( curr_hwnd ) == NULL ) {
            if( _wpi_iswindow( GUIMainHInst, curr_hwnd ) ) {
                if( _wpi_getfocus() != curr_hwnd ) {
                    _wpi_setfocus( curr_hwnd );
                    _wpi_setactivewindow( curr_hwnd );
                    return( true );
                }
            }
        }
    }
    return( false );
}
#endif

void GUIDoResize( gui_window *wnd, HWND hwnd, gui_coord *size )
{
    hwnd = hwnd;
    if( wnd->style & GUI_CHANGEABLE_FONT ) {
        GUIEnableSysMenuItem( wnd, GUI_CHANGE_FONT, true );
    }
    if( wnd->hwnd != NULLHANDLE ) {
        GUIResizeStatus( wnd );
        GUIResizeBackground( wnd, false );
    }
    GUIResizeToolBar( wnd );
    if( (wnd->flags & NEEDS_RESIZE_REDRAW) == 0 ) {
        wnd->old_rows = wnd->num_rows;
    }
    GUISetRowCol( wnd, size );
    wnd->flags |= NEEDS_RESIZE_REDRAW;
    GUISetScroll( wnd );
    if( wnd->flags & SENT_INIT ) {
        GUIScreenToScaleR( size );
        GUIEVENT( wnd, GUI_RESIZE, size );
    }
    GUIInvalidatePaintHandles( wnd );
#ifdef __OS2_PM__
    //_wpi_invalidaterect( hwnd, NULL, TRUE );
#endif
    GUIInvalidateResize( wnd );
    //_wpi_updatewindow( hwnd );
}

static void ProcessMenu( gui_window *wnd, gui_ctl_id id )
{
    if( id < GUI_LAST_MENU_ID ) {
        GUIEVENT( wnd, GUI_CLICKED, &id );
    } else {
        switch( id ) {
        case GUI_CHANGE_FONT :
            GUIChangeFont( wnd );
            break;
        }
    }
}

/*
 * CheckDoFront -- if not implementing MDI, always bring to front.  If MDI,
 *                 only bring the window to the front if it's not the MDI
 *                 parent.
 */

static void CheckDoFront( gui_window *wnd )
{
    if( !GUIMDI || wnd->parent  ) {
        GUIBringToFront( wnd );
    }
}

static void NextWndToFront( HWND hwnd )
{
    HWND        next;
    gui_window  *wnd;

    next = _wpi_getnextwindow( hwnd );
    if( next != NULLHANDLE ) {
        wnd = GUIGetWindow( next );
        if( wnd != NULL ) {
            GUIBringToFront( wnd );
        }
    }
}

static bool IsToolBarCommand( gui_window *wnd, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
#ifdef __NT__
    wparam=wparam; //lparam=lparam;
    return( wnd != NULL && wnd->tbinfo != NULL && wnd->tbinfo->hdl != NULL &&
            GET_WM_COMMAND_HWND( wparam, lparam ) == ToolBarWindow( wnd->tbinfo->hdl ) );
#else
    wnd=wnd; wparam=wparam; lparam=lparam;
    return( false );
#endif
}

static WPI_POINT prevpoint = { -1, -1 };

/*  Procedure to control windows */

WPI_MRESULT CALLBACK GUIWindowProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    gui_window          *wnd;
    gui_window          *root;
    gui_ctl_id          id;
    WPI_POINT           currentpoint;
    gui_coord           point;
    gui_coord           size;
    WPI_MRESULT         ret;
    WPI_RECT            rect;
    HWND                parent;
#ifdef __WINDOWS_386__
    wmcreate_info __far *wmcreateinfo;
#else
    wmcreate_info       *wmcreateinfo;
#endif
    gui_create_info     *dlg_info;
    bool                use_defproc;
    HWND                win;
#ifndef __OS2_PM__
    gui_key_state       key_state;
    RECT                rc;
#endif

    root = NULL;
    dlg_info = NULL;
    if( msg == WM_CREATE ) {
#ifdef __WINDOWS_386__
        CREATESTRUCT __far *lpcs = (CREATESTRUCT __far *)MK_FP32( (void *)lparam );
        wmcreateinfo = (wmcreate_info __far *)MK_FP32( _wpi_getcreateparms( lpcs ) );
#else
        CREATESTRUCT FAR *lpcs = (CREATESTRUCT FAR *)lparam;
        wmcreateinfo = (wmcreate_info *)_wpi_getcreateparms( lpcs );
#endif
        if( wmcreateinfo != NULL ) {
            wnd = wmcreateinfo->wnd;
            dlg_info = wmcreateinfo->dlg_info;
            if( wnd->hwnd == NULLHANDLE ) {
#ifdef __OS2_PM__
                if( wnd->root_frame != NULLHANDLE && wnd->root == NULLHANDLE ) {
                    wnd->root = hwnd;
                } else {
                    wnd->hwnd = hwnd;
                }
#else
                if( _wpi_getparent( hwnd ) == HWND_DESKTOP ) {
                    wnd->root       = hwnd;
                    wnd->root_frame = hwnd;
                } else {
                    wnd->hwnd       = hwnd;
                    wnd->hwnd_frame = hwnd;
                }
#endif
            }
            DoSetWindowLongPtr( hwnd, wnd );
        }
    }
    wnd = GUIGetWindow( hwnd );
    if( wnd == NULL ) {
        return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );
    }

    ret = 0L;
    if( GUIMDIProcessMessage( wnd, hwnd, msg, wparam, lparam, &ret ) ) {
        return( ret );
    }

    if( wnd->root == hwnd ) {
        /* message for root window */
        switch( msg ) {
        case WM_CREATE :
            ret = 0L;
#ifdef __OS2_PM__
            wnd->root_pinfo.normal_pres = _wpi_createos2normpres( GUIMainHInst, hwnd );
#endif
            _wpi_getclientrect( wnd->root_frame, &wnd->root_client_rect );
            if( !CreateBackgroundWnd( wnd, dlg_info ) ) {
                ret = (WPI_MRESULT)WPI_ERROR_ON_CREATE;
            }
            return( ret );
        }
    } else if( ( wnd->root != NULLHANDLE ) && ( hwnd == wnd->hwnd ) ) {
        /* message for container window */
        switch( msg ) {
        case WM_SIZE :
            if( !_wpi_isiconic( _wpi_getframe( hwnd ) ) ) {
                size.x = _wpi_getwmsizex( wparam, lparam );
                size.y = _wpi_getwmsizey( wparam, lparam );
                GUIDoResize( wnd, hwnd, &size );
            }
            /* fall through */
        case WM_MOVE :
        case WM_VSCROLL :
        case WM_HSCROLL :
        case WM_CLOSE :
            return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );
        }
    }

    use_defproc = false;
    ret = 0L;
    switch( msg ) {
    case WM_CREATE :
#ifdef __OS2_PM__
        wnd->hwnd_pinfo.normal_pres = _wpi_createos2normpres( GUIMainHInst, hwnd );
#endif
        NumWindows++; // even if -1 is returned, window will get WM_DESTROY
        win = GUIGetParentFrameHWND( wnd );
        if( ( wnd->root_frame != NULLHANDLE ) || (dlg_info->style & GUI_POPUP) ) {
            if( !GUIAddToSystemMenu( wnd, win, 0, NULL, dlg_info->style ) ) {
                return( (WPI_MRESULT)WPI_ERROR_ON_CREATE );
            }
        } else {
            if( !GUIAddToSystemMenu( wnd, win, dlg_info->menu.num_items, dlg_info->menu.menu, dlg_info->style ) ) {
                return( (WPI_MRESULT)WPI_ERROR_ON_CREATE );
            }
        }
        _wpi_getclientrect( hwnd, &wnd->hwnd_client_rect );
        GUISetRowCol( wnd, NULL );
        if( ( hwnd == wnd->hwnd ) && ( wnd->root == NULLHANDLE ) ) {
            GUIMDINewWindow( hwnd );
        }
        if( !GUIEVENT( wnd, GUI_INIT_WINDOW, NULL ) ) {
            /* app decided not to create window */
            return( (WPI_MRESULT)WPI_ERROR_ON_CREATE );
        }
        wnd->flags |= SENT_INIT;
        GUISetScroll( wnd ); /* initalize scroll ranges */
        GUIBringToFront( wnd );
        break;
#if defined(__NT__)
    case WM_CTLCOLORBTN :
    case WM_CTLCOLORDLG :
    //case WM_CTLCOLORLISTBOX :
    case WM_CTLCOLORSTATIC :
    //case WM_CTLCOLOREDIT :
        ret = (WPI_MRESULT)CvrCtl3dCtlColorEx( msg, wparam, lparam );
        if( ret == (WPI_MRESULT)NULL ) {
            SetBkColor( (HDC)wparam, GetNearestColor( (HDC)wparam, GUIGetBack( wnd, GUI_BACKGROUND ) ) );
            ret = (WPI_MRESULT)wnd->bk_brush;
        }
        break;
#elif !defined( __OS2_PM__ )
    case WM_CTLCOLOR :
        switch( HIWORD( lparam ) ) {
        case CTLCOLOR_BTN :
        case CTLCOLOR_DLG :
        case CTLCOLOR_EDIT :
        case CTLCOLOR_LISTBOX :
        case CTLCOLOR_MSGBOX :
        case CTLCOLOR_STATIC :
            ret = (WPI_MRESULT)CvrCtl3dCtlColorEx( msg, wparam, lparam );
            if( ret == (WPI_MRESULT)NULL ) {
                SetBkColor( (HDC)wparam, GetNearestColor( (HDC)wparam, GUIGetBack( wnd, GUI_BACKGROUND ) ) );
                ret = (WPI_MRESULT)wnd->bk_brush;
            }
            break;
        }
        break;
#endif
#ifndef __OS2_PM__
    case WM_INITMENUPOPUP :
        ret = GUIProcessInitMenuPopup( wnd, hwnd, msg, wparam, lparam );
        break;
    case WM_MENUSELECT :
        ret = GUIProcessMenuSelect( wnd, hwnd, msg, wparam, lparam );
        break;
#endif
    case WM_GETMINMAXINFO :
        {
#ifdef __WINDOWS_386__
            WPI_MINMAXINFO __far *minmax= (WPI_MINMAXINFO __far *)MK_FP32( (void *)lparam );
#else
            WPI_MINMAXINFO *minmax = (WPI_MINMAXINFO *)lparam;
#endif
            ret = _wpi_defwindowproc( hwnd, msg, wparam, lparam );
            if( wnd->root == NULLHANDLE ) {
                parent = _wpi_getparent( hwnd );
                _wpi_getclientrect( parent, &rect );
                _wpi_setmaxposition( minmax, 0, 0 );
                _wpi_setmaxtracksize( minmax, _wpi_getwidthrect( rect ), _wpi_getheightrect( rect ) );
            }
            break;
        }
    case WM_ERASEBKGND:
#ifdef __OS2_PM__
        //GUIInvalidatePaintHandles( wnd );
        ret = (WPI_MRESULT)true;
#else
        if( !_wpi_isiconic( hwnd ) ) {
            GetClientRect( hwnd, &rc );
            FillRect( (HDC)wparam, &rc, wnd->bk_brush );
        }
        use_defproc = true;
#endif
        break;
#if !defined(__OS2_PM__)
    case WM_PAINTICON :
        {
            HICON   old;

            old = SET_HICON( wnd->hwnd, wnd->icon );
            ret = _wpi_defwindowproc( hwnd, msg, wparam, lparam );
            SET_HICON( wnd->hwnd, old );
            break;
        }
#endif
    case WM_PAINT:
        if( _wpi_isiconic( hwnd ) ) {
            use_defproc = true;
        } else {
            GUIPaint( wnd, hwnd, false );
        }
        break;
#ifndef __OS2_PM__
    case WM_ACTIVATEAPP:
        {
            bool activate;

            activate = ( wparam != 0 );
            root = GUIGetRootWindow();
            ActivateNC( root, activate );
            if( GUICurrWnd != NULL ) {
                ActivateNC( GUICurrWnd, activate );
            }
            if( root != NULL ) {
                GUIEVENT( root, GUI_ACTIVATEAPP, &activate );
            }
            use_defproc = true;
        }
        break;
#if 0
    // this repaints the nc client area when the window loses focus to
    // a window that is not a descendant of a GUI window
    case WM_KILLFOCUS :
        if( !GUIIsGUIChild( (HWND)wparam ) ) {
            ActivateNC( wnd, false );
        }
        break;
#endif
    case WM_SETFOCUS :
        if( !_wpi_ismsgsetfocus( msg, lparam ) ) {
            use_defproc = true;
        } else if( !EditControlHasFocus ) {
            SetFocusToParent();
        }
        break;
#endif
    case WM_VSCROLL :
    case WM_HSCROLL :
        GUIProcessScrollMsg( wnd, msg, wparam, lparam );
        break;
#ifdef __NT__
    case WM_MOUSEWHEEL :
        {
            // Try to handle mousewheel messages...
            // Fake them into GUIProcessScrollMsg()
            // as "normal" vertical scroll messages.
            short gcWheelDelta; //wheel delta from roll
            WORD  wKey;

            // The wnd I get is not the same as WM_VSCROLL : above gets...
            // Note to self: Fix it...
            // Seems like the main app window gets the message, rather than
            // the MDI clients...

            gcWheelDelta = HIWORD(wparam);
            wKey = LOWORD(wparam);
            // Scroll wheel upwards  gives  120
            //    "     "   downward   "   -120
            if( wnd != GUICurrWnd ) // Send to child window with focus
                wnd = GUICurrWnd;
            if( gcWheelDelta > 0 ) {
                // positive - scroll up
                if( wKey == MK_CONTROL || wKey == MK_SHIFT ) {
                    GUIProcessScrollMsg( wnd, WM_VSCROLL, SB_PAGEUP, 0L );
                } else {
                    GUIProcessScrollMsg( wnd, WM_VSCROLL, SB_LINEUP, 0L );
                }
            } else {
                // negative - scroll down
                if( wKey == MK_CONTROL || wKey == MK_SHIFT ) {
                    GUIProcessScrollMsg( wnd, WM_VSCROLL, SB_PAGEDOWN, 0L );
                } else {
                    GUIProcessScrollMsg( wnd, WM_VSCROLL, SB_LINEDOWN, 0L );
                }
            }
            // Inform GUI system we are done with scrolling for now.
            GUIProcessScrollMsg( wnd, WM_VSCROLL, SB_ENDSCROLL, 0 );
        }
        break;
#endif
    case WM_MOVE :
        use_defproc = true;
        if( (wnd->flags & DOING_CLOSE) == 0 ) {
            if( !GUIParentHasFlags( wnd, IS_MINIMIZED ) ) {
                GUIEVENT( wnd, GUI_MOVE, NULL );
            }
        }
        break;
    case WM_SIZE:
        use_defproc = true;
        if( (wnd->flags & DOING_CLOSE) == 0 ) {
            if( _wpi_isiconic( _wpi_getframe( hwnd ) ) ) {
                wnd->flags |= IS_MINIMIZED;
                if( wnd->style & GUI_CHANGEABLE_FONT ) {
                    GUIEnableSysMenuItem( wnd, GUI_CHANGE_FONT, false );
                }
                GUIEVENT( wnd, GUI_ICONIFIED, NULL );
                if( GUIMDI ) {
                    GUIBringNewToFront( wnd );
                }
            } else {
                wnd->flags &= ~IS_MINIMIZED;
                size.x = _wpi_getwmsizex( wparam, lparam );
                size.y = _wpi_getwmsizey( wparam, lparam );
                GUIDoResize( wnd, hwnd, &size );
                if( wnd->flags & IS_ROOT ) {
                    win = GUIGetParentFrameHWND( wnd );
                    if( !_wpi_isiconic( win ) ) {
                        GUIMaximizeZoomedChildren( wnd );
                    }
                }
            }
#ifdef __NT__
            //Call back to tell about resizing so system tray can be used
            WndSizeChange( hwnd, wparam, lparam );
#endif
        }
        break;
    case WM_MOUSEMOVE:
        currentpoint.x = GET_WM_MOUSEMOVE_POSX( wparam, lparam );
        currentpoint.y = GET_WM_MOUSEMOVE_POSY( wparam, lparam );
        point.x = currentpoint.x;
        point.y = currentpoint.y;
        GUIScreenToScaleR( &point );
        if( ( currentpoint.x != prevpoint.x ) || ( currentpoint.y != prevpoint.y ) ) {
            prevpoint.x = currentpoint.x;
            prevpoint.y = currentpoint.y;
            SENDPOINTGUIEVENT( wnd, GUI_MOUSEMOVE, wparam, lparam );
        }
        break;
#ifndef __OS2_PM__
    case WM_NCLBUTTONDOWN :
    case WM_NCMBUTTONDOWN :
    case WM_NCRBUTTONDOWN :
        CheckDoFront( wnd );
        use_defproc = true;
        break;
    case WM_RBUTTONDOWN:
        _wpi_setcapture( hwnd );
        CheckDoFront( wnd );
        SENDPOINTGUIEVENT( wnd, GUI_RBUTTONDOWN, wparam, lparam );
        break;
#else
    case WM_RBUTTONDOWN :
        WPI_MAKEPOINT( wparam, lparam, currentpoint );
        win = PM1632WinWindowFromPoint( hwnd, &currentpoint, false );
        if( ( win != NULLHANDLE ) && ( win != hwnd ) ) {
            id = _wpi_getdlgctrlid( win );
            if( id != 0 ) {
                GUIEVENT( wnd, GUI_CONTROL_RCLICKED, &id );
            }
        } else {
            _wpi_setcapture( hwnd );
            CheckDoFront( wnd );
            SENDPOINTGUIEVENT( wnd, GUI_RBUTTONDOWN, wparam, lparam );
        }
        break;
#endif
    case WM_LBUTTONDOWN:
        _wpi_setcapture( hwnd );
        CheckDoFront( wnd );
        SENDPOINTGUIEVENT( wnd, GUI_LBUTTONDOWN, wparam, lparam );
        use_defproc = true;
        break;
    case WM_LBUTTONUP:
        _wpi_releasecapture();
        SENDPOINTGUIEVENT( wnd, GUI_LBUTTONUP, wparam, lparam );
        use_defproc = true;
        break;
    case WM_RBUTTONUP:
        _wpi_releasecapture();
        SENDPOINTGUIEVENT( wnd, GUI_RBUTTONUP, wparam, lparam );
        break;
    case WM_LBUTTONDBLCLK:
        CheckDoFront( wnd );
        SENDPOINTGUIEVENT( wnd, GUI_LBUTTONDBLCLK, wparam, lparam );
        break;
    case WM_RBUTTONDBLCLK:
        CheckDoFront( wnd );
        SENDPOINTGUIEVENT( wnd, GUI_RBUTTONDBLCLK, wparam, lparam );
        break;
    case WM_SYSCOMMAND:
        id = _wpi_getid( wparam );
        switch( id ) {
        case SC_NEXTWINDOW :
            if( GUIMDI ) {
                NextWndToFront( hwnd );
                break;
            }
            /* fall through */
        default :
            if( (id & 0xf000) == (SC_NEXTWINDOW & 0xf000) ) {
                /* top value same for all SC_* values */
                use_defproc = true;
            } else {
                ProcessMenu( wnd, id );
            }
            break;
        }
        break;
#ifdef __OS2_PM__
    case WM_CONTROL :
        GUIProcessControlNotification( SHORT1FROMMP( wparam ), SHORT2FROMMP( wparam ), wnd );
        break;
#else
    case WM_PARENTNOTIFY:
        if( ( LOWORD( wparam ) == WM_RBUTTONDOWN ) ||
            ( LOWORD( wparam ) == WM_LBUTTONDOWN ) ||
            ( LOWORD( wparam ) == WM_MBUTTONDOWN ) ) {
            if( wnd->root == NULLHANDLE ) {
                CheckDoFront( wnd );
            }
        }
        if( LOWORD(wparam) == WM_RBUTTONDOWN ) {
            WPI_MAKEPOINT( wparam, lparam, currentpoint );
            MapWindowPoints( hwnd, NULLHANDLE, &currentpoint, 1 );
            win = _wpi_windowfrompoint( currentpoint );
            id = _wpi_getdlgctrlid( win );
            if( id != 0 ) {
                if( _wpi_getparent( win ) == hwnd ) {
                    GUIEVENT( wnd, GUI_CONTROL_RCLICKED, &id );
                }
            }
        }
        break;
    case WM_ENDSESSION :
        {
            gui_end_session     es;

            es.endsession = ( wparam != 0 );
            es.logoff = ( lparam == 0x80000000L );
            GUIEVENT( wnd, GUI_ENDSESSION, &es );
        }
        break;
    case WM_QUERYENDSESSION :
        {
            gui_end_session     es;

            es.endsession = ( wparam != 0 );
            es.logoff = ( lparam == 0x80000000L );  // ENDSESSION_LOGOFF
            if( !GUIEVENT( wnd, GUI_QUERYENDSESSION, &es ) ) {
                ret = (WPI_MRESULT)true;
            }
        }
        break;
#endif
    case WM_COMMAND:
        if( _wpi_ismenucommand( wparam, lparam ) ||
            IsToolBarCommand( wnd, wparam, lparam ) ) { /* from menu or toolbar */
            ProcessMenu( wnd, _wpi_getid( wparam ) );
            //SetFocusToParent();
        } else {
            GUIProcessControlMsg( wparam, lparam, wnd, NULL );
        }
        use_defproc = true;
        break;
#ifndef __OS2_PM__
    case WM_VKEYTOITEM :
        ret = (WPI_MRESULT)-1;
        GUIGetKeyState( &key_state.state );
        if( GUIWindowsMapKey( wparam, lparam, &key_state.key ) ) {
            ret = (WPI_MRESULT)GUIEVENT( wnd, GUI_KEYTOITEM, &key_state );
        }
        break;
#endif
#ifdef __OS2_PM__
    case WM_CHAR :
    case WM_TRANSLATEACCEL :
#else
    case WM_MENUCHAR :
    case WM_SYSKEYDOWN :
    case WM_SYSKEYUP :
    case WM_KEYUP :
    case WM_KEYDOWN :
#endif
        ret = (WPI_MRESULT)GUIProcesskey( hwnd, msg, wparam, lparam );
        break;
    case WM_CLOSE :
        if( wnd->flags & DOING_CLOSE ) {
            use_defproc = true;
        } else if( wnd->style & GUI_CLOSEABLE ) {
            if( GUIEVENT( wnd, GUI_CLOSE, NULL ) ) {
                wnd->flags |= DOING_CLOSE;
                if( wnd->flags & IS_ROOT ) {
                    use_defproc = true;
                } else {
                    _wpi_destroywindow( wnd->hwnd_frame );
                }
            }
        }
        break;
#ifdef __NT__
    // Message to deal with tray icons (Win 95 and NT 4.0 ).
    case WM_TRAYCALLBACK :
        TrayCallBack( hwnd, wparam, lparam );
        break;
#endif
    case WM_DESTROY :
        wnd->flags |= DOING_DESTROY;
        NumWindows--;
        GUIEVENT( wnd, GUI_DESTROY, NULL );
        GUIDestroyAllChildren( wnd );
        if( wnd->flags & IS_ROOT ) {
            GUIDestroyAllPopupsWithNoParent();
        }
        GUIFreeWindowMemory( wnd, false, false );
        if( NumWindows == 0 ) {
            _wpi_postquitmessage( 0 );
            Posted = true;
        }
        break;
    default:
        use_defproc = true;
        break;
    }
    if( use_defproc )
        return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );

    return( ret );
}

#ifdef __OS2_PM__
WPI_MRESULT CALLBACK GUIFrameProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    HWND        client;
    gui_window  *wnd;

    client = WinWindowFromID( hwnd, FID_CLIENT );
    wnd = GUIGetWindow( client );
    if( wnd != NULL ) {
        switch( msg ) {
        case WM_SAVEAPPLICATION:
            {
                gui_end_session     es;

                es.endsession = true;
                es.logoff = false;
                GUIEVENT( wnd, GUI_ENDSESSION, &es );
                return( 0L );
            }
        case WM_TRANSLATEACCEL:
            // Don't let OS/2 process F10 as an accelerator
            // Note: similar code exists in guimapky.c but we need to
            // take different default action
            if( IS_VKEY_F10( wparam ) )
                return( 0L );
            break;
        case WM_CHAR:
            return( GUIProcesskey( hwnd, msg, wparam, lparam ) );
        case WM_INITMENUPOPUP :
            return( GUIProcessInitMenuPopup( wnd, hwnd, msg, wparam, lparam ) );
        case WM_MENUSELECT:
            return( GUIProcessMenuSelect( wnd, hwnd, msg, wparam, lparam ) );
        case WM_SETFOCUS:
            if( lparam == 0 ) {
                ActivateNC( wnd, false );
            }
            break;
        case WM_ACTIVATE:
            if( wparam ) {
                CheckDoFront( wnd );
            }
            break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            CheckDoFront( wnd );
            break;
        case WM_VSCROLL :
        case WM_HSCROLL :
            GUIProcessScrollMsg( wnd, msg, wparam, lparam );
            return( 0L );
        }
    }
    return( _wpi_callwindowproc( oldFrameProc, hwnd, msg, wparam, lparam ) );
}
#endif

void GUISetF10Menus( bool setting )
{
    setting = setting;
}

bool GUIIsFirstInstance( void )
{
    return( FirstInstance );
}
