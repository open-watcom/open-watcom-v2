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


#include "guiwind.h"
#include <stdlib.h>
#include <string.h>
#include "wmdisim.h"
#include "guixutil.h"
#include "guixhook.h"
#include "guixmdi.h"
#include "guimdi.h"
#include "guiscrol.h"
#include "guixwind.h"
#include "guistyle.h"

#define MAX_LENGTH 256

typedef struct {
    int         height;
    int         width;
    int         num;
    int         mod;
    gui_rect    rect;
    gui_coord   bump;
} cascade_info;

static  mdi_info        MDIInfo;
static  char            Buffer[MAX_LENGTH];
static  bool            DoneMDIInit             = FALSE;
static  bool            DoingMaxRestore         = FALSE;
static  bool            ArrangeIcons            = FALSE;
extern  WPI_INST        GUIMainHInst;

static void StartMaxRestore( HWND hwnd )
{
    hwnd = hwnd;
    DoingMaxRestore = TRUE;
}

static void SetStyle( HWND hwnd, int max )
{
#ifndef __OS2_PM__
    DWORD       style;
    gui_window  *wnd;

    wnd = GUIGetWindow( hwnd );
    style = _wpi_getwindowlong( hwnd, GWL_STYLE );
    if( max ) {
        style |= WS_VSCROLL+WS_HSCROLL;
        style &= ~( COMMON_STYLES | WS_THICKFRAME | WS_CAPTION );
    } else {
        if( wnd->scroll & GUI_HSCROLL ) {
            style |= WS_HSCROLL;
        }
        if( wnd->scroll & GUI_VSCROLL ) {
            style |= WS_VSCROLL;
        }
        if( wnd->style & GUI_RESIZEABLE ) {
            style |= WS_THICKFRAME;
        }
        if( !(wnd->style & GUI_NOFRAME) ) {
            style |= WS_CAPTION;
        }
        style |= COMMON_STYLES;
    }
    _wpi_setwindowlong( hwnd, GWL_STYLE, style );
#else
    hwnd = hwnd;
    max = max;
#endif
}

static void EndMaxRestore( HWND hwnd )
{
    gui_window  *wnd;
    WPI_RECT    rect;
    gui_coord   size;

    wnd = GUIGetWindow( hwnd );
    if( GUI_HSCROLL_ON( wnd ) ) {
        GUISetRangePos( wnd, SB_HORZ );
    }
    if( GUI_VSCROLL_ON( wnd ) ) {
        GUISetRangePos( wnd, SB_VERT );
    }
    DoingMaxRestore = FALSE;
    _wpi_getwindowrect( hwnd, &rect );
    size.x = _wpi_getwidthrect( rect );
    size.y = _wpi_getheightrect( rect );
    GUIDoResize( wnd, hwnd, &size );
}

static void SetWindowTitle( HWND hwnd )
{
    char        buffer[MAX_LENGTH];

    _wpi_getwindowtext( hwnd, &buffer, MAX_LENGTH - 1 );
    MDISetMainWindowTitle( &buffer );
}

void XChangeTitle( gui_window *wnd )
{
    if( wnd->root != NULLHANDLE ) {
        _wpi_getwindowtext( wnd->root, &Buffer, MAX_LENGTH - 1 );
        MDIInitMenu();
    } else {
        SetWindowTitle( wnd->hwnd );
    }
}

static bool IsMaximized( gui_window *wnd )
{
    HWND        hwnd;

    if( MDIIsMaximized() ) {
        hwnd = GUIGetParentFrameHWND( wnd );
        if( GUIIsMDIChildWindow( wnd ) ) {
            return( MDIIsWndMaximized( hwnd ) );
        } else {
            return( _wpi_iszoomed( hwnd ) );
        }
    } else {
        return( FALSE );
    }
}

static bool UpdatedMenu( void )
{
    return( MDIUpdatedMenu() );
}

static void MDIMaximize( bool max, gui_window *wnd )
{
#ifndef __OS2_PM__
    if( max ) {
        GUISendMessage( wnd->hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0L );
    } else {
        GUISendMessage( wnd->hwnd, WM_SYSCOMMAND, SC_RESTORE, 0L );
    }
#else
    max = max;
    wnd = wnd;
#endif
}

static void SetMDIRestoredSize( HWND hwnd, WPI_RECT *rect )
{
    MDISetOrigSize( hwnd, rect );
}

static bool IsMDIChildWindow( gui_window *wnd )
{
    return( GUIGetParentWindow( wnd ) == GUIGetRootWindow() );
}

bool XInitMDI( gui_window *wnd )
{
    if( wnd->root != NULLHANDLE ) {
        /* root window */
        memset( &MDIInfo, 0, sizeof( mdi_info ) );
        MDIInfo.root = wnd->root;
        MDIInfo.container = wnd->hwnd;
        MDIInfo.data_off = GUI_MDI_WORD * EXTRA_SIZE;
        _wpi_getwindowtext( wnd->root, &Buffer, MAX_LENGTH - 1 );
        MDIInfo.main_name = &Buffer;
        MDIInfo.start_max_restore = &StartMaxRestore;
        MDIInfo.end_max_restore = &EndMaxRestore;
        MDIInfo.set_window_title = SetWindowTitle;
        MDIInfo.set_style = &SetStyle;
        MDIInfo.hinstance = GUIMainHInst;
        MDIInit( &MDIInfo );
        DoneMDIInit = TRUE;
        return( FALSE );
    } else {
        /* child window */
        return( TRUE );
    }
}

void GUIMDIBringToFront( gui_window *wnd )
{
    if( wnd != NULL ) {
        if( MDIIsMaximized() && GUIIsMinimized( wnd ) ) {
            GUIMDIMaximize( TRUE, wnd );
        }
        if( GUIIsMinimized( wnd ) ) {
            GUIRestoreWindow( wnd );
        }
        GUIBringToFront( wnd );
    }
}

static bool MDIProcessMessage( gui_window *wnd, HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                               WPI_PARAM2 lparam, WPI_MRESULT *ret )
{
    WORD        param;
#ifndef __OS2_PM__
    WINDOWPOS   *pos;
#endif

    if( wnd->root == hwnd ) {
        /* message for root window */
        switch( msg ) {
        case WM_DESTROY :
            MDIClearMaximizedMenuConfig();
            break;
#ifndef __OS2_PM__
        case WM_NCLBUTTONDBLCLK:
            if( MDIHitClose( hwnd, msg, wparam, lparam ) ) {
                *ret = 0;
                return( TRUE );
            }
            break;
#endif
        case WM_COMMAND :
            if( DoneMDIInit && MDIIsSysCommand( hwnd, msg, wparam, lparam ) ) {
                *ret = 0;
                return( TRUE );
            }
            param = _wpi_getid( wparam );
            if( _wpi_ismenucommand( wparam, lparam ) &&
                ( param >= GUI_MDI_MENU_FIRST ) &&
                ( param <= GUI_MDI_MENU_LAST ) ) {
                switch( param ) {
                case GUI_MDI_CASCADE :
                    MDICascade();
                    break;
                case GUI_MDI_TILE_HORZ :
                    MDITile( TRUE );
                    break;
                case GUI_MDI_TILE_VERT :
                    MDITile( FALSE );
                    break;
                case GUI_MDI_ARRANGE_ICONS :
                    ArrangeIcons = TRUE;
                    MDICascade();
                    ArrangeIcons = FALSE;
                    break;
                case GUI_MDI_MORE_WINDOWS :
                    GUIMDIMoreWindows();
                    break;
                default :
                    if( ( param >= GUI_MDI_FIRST_WINDOW ) &&
                        ( param <= GUI_MDI_FIRST_WINDOW + MAX_NUM_MDI_WINDOWS ) ) {
                        /* window selected */
                        GUIMDIBringToFront( GUIMDIGetWindow( param ) );
                    }
                }
                return( TRUE );
            }
        }
    } else if( wnd->root == NULLHANDLE ) {
        /* child window */
        switch( msg ) {
#ifndef __OS2_PM__
        case WM_WINDOWPOSCHANGING :
            if( ArrangeIcons ) {
                /* arrange icons is really a cascade but don't allow
                   no-iconic windows to move */
                if( !_wpi_isiconic( hwnd ) ) {
                    pos = (WINDOWPOS *)lparam;
                    pos->flags |= SWP_NOMOVE | SWP_NOSIZE;
                    *ret = 0;
                    return( TRUE );
                }
            }
            break;
        case WM_SYSCHAR:
        case WM_SYSKEYDOWN :
        case WM_SYSKEYUP :
            *ret = GUISendMessage( GUIGetTopParentHWND( hwnd ), msg, wparam, lparam );
            return( TRUE );
            break;
#endif
        case WM_MOVE :
        case WM_SIZE :
            if( DoingMaxRestore ) {
                *ret = 0;
                return( TRUE );
            }
            break;
#ifndef __OS2_PM__
        case WM_MOUSEACTIVATE :
            SetFocus( hwnd );
            *ret = MA_ACTIVATE;
            return( TRUE );
#else
        case WM_ACTIVATE :
            if( wparam ) {
                SetFocus( hwnd );
                return( TRUE );
            }
#endif
        case WM_GETMINMAXINFO :
            /* don't want app to fool with this */
            *ret = _wpi_defwindowproc( hwnd, msg, wparam, lparam );
            return( TRUE );
        }
        if( DoneMDIInit ) {
            return( MDIChildHandleMessage( hwnd, msg, wparam, lparam, ret ) );
        }
    } else if( wnd->hwnd == hwnd ) {
        /* container window */
        switch( msg ) {
        case WM_SIZE :
            MDIContainerResized();
            break;
        }
    }
    return( FALSE );
}

void GUIXMDIInit( void )
{
    GUISetMDIProcessMessage( &MDIProcessMessage );
    GUISetMDIMaximized( &IsMaximized );
    GUISetMDIUpdatedMenu( &UpdatedMenu );
    GUISetMDINewWindow( &MDINewWindow );
    GUISetMDIMaximize( &MDIMaximize );
    GUISetSetMDIRestoredSize( &SetMDIRestoredSize );
    GUISetIsMDIChildWindow( &IsMDIChildWindow );
}

static void GUIInternalCascadeWindows( gui_window *wnd, void *param )
{
    cascade_info        *info;
    gui_rect            rect;

    if( GUIIsMinimized( wnd ) ) {
        return;
    }
    info = (cascade_info *)param;
    rect.width = info->width;
    rect.height = info->height;
    rect.x = info->num * info->bump.x;
    rect.y = info->num * info->bump.y;
    GUISetRedraw( wnd, FALSE );
    GUIResizeWindow( wnd, &rect );
    GUISetRedraw( wnd, TRUE );
    GUIBringToFront( wnd );

    info->num++;
    info->num = info->num % info->mod;
}

static void Cascade( gui_window *root, int num_windows, gui_rect *rect,
                     gui_coord *min_size )
{
    cascade_info        info;
    int                 mod;
    gui_text_metrics    metrics;
    gui_system_metrics  sys_metrics;

    GUIGetSystemMetrics( &sys_metrics );
    GUIGetTextMetrics( root, &metrics );
    info.bump.y = sys_metrics.caption_size + sys_metrics.resize_border.y;
    info.height = rect->height - info.bump.y * ( num_windows - 1 );
    info.mod = num_windows;
    if( info.height < min_size->y ) {
        info.height = min_size->y;
        info.mod = ( rect->height - min_size->y ) / info.bump.y + 1;
    }
    info.bump.x = info.bump.y;
    info.width = rect->width - info.bump.x * ( info.mod - 1 );
    if( info.width < min_size->x ) {
        info.width = min_size->x;
        mod = ( rect->width - min_size->x ) / info.bump.x + 1;
        if( mod < info.mod ) {
            info.mod = mod;
        }
    }
    info.rect = *rect;
    info.num = 0;
    GUIEnumChildWindows( root, &GUIInternalCascadeWindows, &info );
}

bool GUICascadeWindows( void )
{
    gui_rect            rect;
    int                 num_windows;
    gui_window          *root;
    gui_coord           min_size;
    int                 total_icons;

    root = GUIGetRootWindow();
    total_icons = GUIGetNumIconicWindows();
    num_windows = GUIGetNumChildWindows() - total_icons;
    if( ( root == NULL ) || ( num_windows <= 1 ) ) {
        return( FALSE );
    }
    GUIGetClientRect( root, &rect );
    GUIGetMinSize( &min_size );
    Cascade( root, num_windows, &rect, &min_size );
    GUIWndDirty( root );
    return( TRUE );
}

