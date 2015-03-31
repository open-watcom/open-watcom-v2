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
#include "guicolor.h"
#include "guimenus.h"
#include "guiscale.h"
#include "guixutil.h"
#include "guiscrol.h"
#include "guixwind.h"
#include "guiwnclr.h"
#include "guimapky.h"
#include "guicontr.h"
#include "guihook.h"
#include "guixhook.h"
#include "guipaint.h"
#include "guizlist.h"

#define ERROR_STYLE MB_OK | MB_ICONEXCLAMATION

extern  gui_window      *GUICurrWnd;
extern  WPI_TEXTMETRIC  GUItm;
extern  WPI_INST        GUIMainHInst;
extern  WPI_INST        GUIResHInst;

static void MaxChild( gui_window *wnd, void *param )
{
    param = param;
    if( _wpi_iszoomed( wnd->hwnd_frame ) ) {
        GUISetRedraw( wnd, false );
        GUIRestoreWindow( wnd );
        GUIMaximizeWindow( wnd );
        GUISetRedraw( wnd, true );
        GUIShowWindowNA( wnd );
    }
}

void GUIMaximizeZoomedChildren( gui_window *wnd )
{
    GUIEnumChildWindows( wnd, &MaxChild, NULL );
}

/*
 * GUIIsOpen -- Is the given window open
 */

bool GUIIsOpen( gui_window *wnd )
{
    if( _wpi_iswindow( GUIMainHInst, wnd->hwnd ) ) {
        return( true );
    }
    return( false );
}

bool GUIIsParentADialog( gui_window *wnd )
{
    wnd = GUIGetParentWindow( wnd );
    while( wnd ) {
        if( wnd->flags & IS_DIALOG ) {
            return( true );
        }
        wnd = GUIGetParentWindow( wnd );
    }
    return( false );
}

bool GUIIsRectInUpdateRect( gui_window *wnd, WPI_RECT *rect )
{
    WPI_RECT    update_rect;
    WPI_RECT    intersect;

    if( !wnd || !wnd->ps || !rect ) {
        return( true );
    }

#ifdef __OS2_PM__
    update_rect = *(wnd->ps);
#else
    _wpi_getpaintrect( wnd->ps, &update_rect );
#endif
    _wpi_intersectrect( GUIMainHInst, &intersect, &update_rect, rect );

    if( _wpi_isrectempty( GUIMainHInst, &intersect ) ) {
        return( false );
    }

    return( true );
}

void GUICalcLocation( gui_rect *rect, gui_coord *pos, gui_coord *size,
                      HWND parent )
{
    WPI_RECT    r;
    GUI_RECTDIM left, top, right, bottom;

    if( parent == NULLHANDLE ) {
        parent = HWND_DESKTOP;
    }
    pos->x = rect->x;
    pos->y = rect->y;
    size->x = rect->width;
    size->y = rect->height;
    if( parent == HWND_DESKTOP ) {
        GUIScaleToScreen( pos );
    } else {
        GUIScaleToScreenR( pos );
        _wpi_getclientrect( parent, &r );
        _wpi_getrectvalues( r, &left, &top, &right, &bottom );
        pos->x += left;
        pos->y += top;
    }
    GUIScaleToScreenR( size );

    pos->y = _wpi_cvtc_y_size_plus1( parent, pos->y, size->y );
}

/*
 * GUISetupStruct -- setup the gui_window structure according to the given
 *                   create_info information.
 */

bool GUISetupStruct( gui_window *wnd, gui_create_info *dlg_info,
                      gui_coord *pos, gui_coord *size, HWND parent,
                      HMENU *menu )
{
    GUICalcLocation( &dlg_info->rect, pos, size, parent );
    if( wnd != NULL ) {
        if( !(wnd->flags & IS_DIALOG) ) {
            wnd->style = dlg_info->style;
            wnd->scroll = dlg_info->scroll;
        }
        if( !GUISetColours( wnd, dlg_info->num_attrs, dlg_info->colours ) ) {
            return( false );
        }
    }

    if( ( ( parent == HWND_DESKTOP ) || ( dlg_info->style & GUI_POPUP ) ) &&
        ( menu != NULL ) ) {
        if( dlg_info->resource_menu != NULL ) {
            *menu =  _wpi_loadmenu( GUIResHInst, dlg_info->resource_menu );
        } else {
            return( GUICreateMenus( wnd, dlg_info->num_menus, dlg_info->menu, menu ) );
        }
    }

    return( true );
}

/*
 * GUIError -- display an error message
 */

void GUIError( char *str )
{
    HWND focus;

    focus = _wpi_getfocus();
    if( focus != NULLHANDLE ) {
        _wpi_messagebox( focus, (LPSTR) str, NULL, ERROR_STYLE );
    }
}


gui_window *GUIFindWindowFromHWND( HWND hwnd )
{
    gui_window *curr;

    for( curr = GUIGetFront(); curr != NULL; curr = GUIGetNextWindow( curr ) ) {
        if( ( curr->hwnd == hwnd ) || ( curr->hwnd_frame == hwnd ) ||
            ( curr->root == hwnd ) || ( curr->root_frame == hwnd ) ) {
            return( curr );
        }
    }

    return( NULL );
}

bool GUIIsGUIChild( HWND hwnd )
{
    gui_window  *root;

    root = GUIGetRootWindow();
    if( root ) {
        hwnd = GUIGetTopParentHWND( hwnd );
        if( root->root_frame == hwnd ) {
            return( true );
        }
    }

    return( false );
}

/*
 * GUISetRedraw -- set the redraw flag for a given window
 */
bool GUISetRedraw( gui_window *wnd, bool redraw )
{
    _wpi_setredraw( wnd->hwnd, ( redraw ) ? TRUE : FALSE );
    return( true );
}

/*
 * GUIBringNewToFront - bring the next window in the z-order to the front.
 *                      Do not bring forward the given window or a decendent
 *                      of that window.
 */

bool GUIBringNewToFront( gui_window *prev )
{
    gui_window *curr;

    for( curr = GUIGetFront(); curr != NULL; curr = GUIGetNextWindow( curr ) ) {
        if( ( curr != prev ) && !_wpi_ischild( prev->hwnd, curr->hwnd ) &&
            !(curr->flags & DOING_DESTROY) ) {
            GUIBringToFront( curr );
            return( true );
        }
    }
    return( false );
}

gui_window *GUIXGetRootWindow( void )
{
    gui_window *curr;

    for( curr = GUIGetFront(); curr != NULL; curr = GUIGetNextWindow( curr ) ) {
        if( curr->flags & IS_ROOT ) {
            return( curr );
        }
    }

    return( NULL );
}

gui_window *GUIFindFirstChild( gui_window *parent )
{
    gui_window *wnd;

    for( wnd = GUIGetFront(); wnd != NULL; wnd = GUIGetNextWindow( wnd ) ) {
        if( wnd->parent == parent && !( wnd->flags & UTILITY_BIT ) ){
            return( wnd );
        }
    }

    return( NULL );
}

gui_window *GUIFindFirstPopupWithNoParent( void )
{
    gui_window *wnd;

    for( wnd = GUIGetFront(); wnd != NULL; wnd = GUIGetNextWindow( wnd ) ) {
        if( ( wnd->style & GUI_POPUP ) && ( wnd->parent == NULL ) ) {
            return( wnd );
        }
    }

    return( NULL );
}

static void GUIMarkChildrenWithFlag( gui_window *parent, gui_flags flag )
{
    gui_window *wnd;

    for( wnd = GUIGetFront(); wnd != NULL; wnd = GUIGetNextWindow( wnd ) ) {
        if( wnd->parent == parent ){
            wnd->flags |= flag;
        }
    }
}

void GUIDestroyAllChildren( gui_window *parent )
{
    gui_window  *wnd;

    GUIMarkChildrenWithFlag( parent, DOING_DESTROY );
    wnd = GUIFindFirstChild( parent );
    while( wnd ) {
        wnd->flags |= UTILITY_BIT;
        GUIDestroyWnd( wnd );
        wnd = GUIFindFirstChild( parent );
    }
}

void GUIDestroyAllPopupsWithNoParent( void )
{
    gui_window  *wnd;

    wnd = GUIFindFirstPopupWithNoParent();
    while( wnd ) {
        GUIDestroyWnd( wnd );
        wnd = GUIFindFirstPopupWithNoParent();
    }
}


/*
 * GUIFreeWindowMemory -- free the memory of the given window
 */

void GUIFreeWindowMemory( gui_window *wnd, bool from_parent, bool dialog )
{
    gui_window  *root;
    HWND        capture;

    from_parent = from_parent;
    if( ( wnd->hwnd != NULLHANDLE ) && ( GUICurrWnd == wnd ) ) {
        capture = _wpi_getcapture();
        if( capture == wnd->hwnd ) {
            _wpi_releasecapture();
        }
    }
    if( wnd->font != NULL ) {
        _wpi_deletefont( wnd->font );
        wnd->font = NULL;
    }
    if( wnd->icon != (WPI_HICON)NULL ) {
        _wpi_destroyicon( wnd->icon );
    }
    GUIFreeColours( wnd );
    GUIFreeBKBrush( wnd );
    GUIControlDeleteAll( wnd );
    GUICloseToolBar( wnd );
    GUIFreeHint( wnd );
    _wpi_setwindowlongptr( wnd->hwnd, GUI_EXTRA_WORD * EXTRA_SIZE, 0 );
    if( wnd->root != NULLHANDLE ) {
        _wpi_setwindowlongptr( wnd->root, GUI_EXTRA_WORD * EXTRA_SIZE, 0 );
    }
    if( !dialog ) {
        GUIMDIDelete( wnd );
        if( GUICurrWnd == wnd ) {
            GUICurrWnd = NULL;
        }
        GUIFreePopupList( wnd );
    }
    GUIDeleteFromList( wnd );
    /* If the window being deleted was the current window, choose a new
     * window to bring to front.  Don't do this if the window that's being
     * destroyed is being destroyed because it's parent is being destroyed
     * (ie never got WM_CLOSE so DOING_CLOSE isn't set).
     */
    if( !dialog && ( wnd->flags & DOING_CLOSE ) && ( GUICurrWnd == NULL ) &&
        !GUIIsParentADialog( wnd ) ) {
        // if the root window has received a WM_DESTROY then just run away
        root = GUIGetRootWindow();
        if( root && !( root->flags & DOING_DESTROY ) ) {
            GUIBringNewToFront( wnd );
        }
    }
    if( wnd->hdc != (WPI_PRES)NULL ) {
        _wpi_releasepres( wnd->hwnd, wnd->hdc );
        wnd->hdc = NULLHANDLE;
    }
#ifdef __OS2_PM__
    GUIFreeWndPaintHandles( wnd, true );
    if( wnd->root_pinfo.normal_pres != (WPI_PRES)NULL ) {
        _wpi_deleteos2normpres( wnd->root_pinfo.normal_pres );
        wnd->root_pinfo.normal_pres = (WPI_PRES)NULL;
    }
    if( wnd->hwnd_pinfo.normal_pres != (WPI_PRES)NULL ) {
        _wpi_deleteos2normpres( wnd->hwnd_pinfo.normal_pres );
        wnd->hwnd_pinfo.normal_pres = (WPI_PRES)NULL;
    }
#endif
    GUIMemFree( wnd );
}

bool GUIScrollOn( gui_window *wnd, int bar )
{
    if( ( bar == SB_VERT ) && GUI_VSCROLL_ON( wnd ) ) {
        return( true );
    }
    if( ( bar == SB_HORZ ) && GUI_HSCROLL_ON( wnd ) ) {
        return( true );
    }
    return( false );
}

void GUISetRowCol( gui_window *wnd, gui_coord *size )
{
    gui_coord   my_size;

    if( size == NULL ) {
        my_size.y = _wpi_getheightrect( wnd->hwnd_client );
        my_size.x = _wpi_getwidthrect( wnd->hwnd_client );
    } else {
        my_size = *size;
    }

    GUIToText( &my_size, wnd );
    wnd->num_rows = my_size.y;
    wnd->num_cols = my_size.x;
}

/*
 * GUIInvalidateResize -- invalidate the bottom portion of the window that
 *                        may now contain a line of text
 */

void GUIInvalidateResize( gui_window *wnd )
{
    WPI_RECT    rect;
    GUI_RECTDIM left, top, right, bottom;

    if( ( wnd->flags & NEEDS_RESIZE_REDRAW ) &&
        ( wnd->old_rows != wnd->num_rows ) ) {
        _wpi_getrectvalues( wnd->hwnd_client, &left, &top, &right, &bottom );
        GUIGetMetrics( wnd );
        if( wnd->old_rows < wnd->num_rows ) {
            /* window grew */
            top = wnd->old_rows * AVGYCHAR( GUItm );
        } else {
            /* window shrunk */
            top = wnd->num_rows * AVGYCHAR( GUItm );
        }
        _wpi_setrectvalues( &rect, left, top, right, bottom );
        _wpi_invalidaterect( wnd->hwnd, &rect, TRUE );
        wnd->flags &= ~NEEDS_RESIZE_REDRAW;
    }
    wnd->old_rows = wnd->num_rows;
}

WPI_MRESULT GUISendMessage( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                            WPI_PARAM2 lparam )
{
    if( hwnd != NULLHANDLE ) {
        return( _wpi_sendmessage( hwnd, msg, wparam, lparam ) );
    } else {
        return( 0L );
    }
}

WPI_MRESULT GUISendDlgItemMessage( HWND parent, gui_ctl_id id, WPI_MSG msg,
                                   WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    HWND hwnd;

    hwnd = _wpi_getdlgitem( parent, id );
    if( hwnd != NULLHANDLE ) {
        return( _wpi_sendmessage( hwnd, msg, wparam, lparam ) );
    } else {
        return( 0L );
    }
}

void GUIMakeRelative( gui_window *wnd, WPI_POINT *pt, gui_point *point )
{
    WPI_RECT    rect;
    GUI_RECTDIM left, top, right, bottom;

    rect = wnd->hwnd_client;
    _wpi_mapwindowpoints( wnd->hwnd, HWND_DESKTOP, (WPI_LPPOINT)&rect, 2 );
    _wpi_getrectvalues( rect, &left, &top, &right, &bottom );
    point->x = pt->x - left;
    point->y = pt->y - top;

    if( GUI_DO_HSCROLL( wnd ) || GUI_DO_VSCROLL( wnd ) ) {
        if( GUI_DO_HSCROLL( wnd ) ) {
            point->x += GUIGetScrollPos( wnd, SB_HORZ );
        }
        if( GUI_DO_VSCROLL( wnd ) ) {
            point->y += GUIGetScrollPos( wnd, SB_VERT );
        }
    }
    GUIScreenToScaleRPt( point );
}

HWND GUIGetScrollHWND( gui_window *wnd )
{
    if( wnd == NULL ) {
        return( NULLHANDLE );
    }
    if( wnd->root != NULLHANDLE ) {
        return( wnd->root );
    }
    return( wnd->hwnd );
}

void GUISetScrollPos( gui_window *wnd, int bar, int new, bool redraw )
{
    int *pos;

    redraw = redraw;
    if( bar == SB_HORZ ) {
        pos = &wnd->hpos;
    } else {
        pos = &wnd->vpos;
    }
    if( *pos != new ) {
        *pos = new;
        _wpi_setscrollpos( GUIGetParentFrameHWND( wnd ), bar, new, ( redraw ) ? TRUE : FALSE );
    }
}

int GUIGetScrollPos( gui_window *wnd, int bar )
{
    if( bar == SB_HORZ ) {
        return( wnd->hpos );
    } else {
        return( wnd->vpos );
    }
}

void GUISetScrollRange( gui_window *wnd, int bar, int min, int max, bool redraw )
{
    int *old_range;

    if( bar == SB_HORZ ) {
        old_range = &wnd->hrange;
    } else {
        old_range = &wnd->vrange;
    }
    if( ( max - min ) != *old_range ) {
        *old_range = max - min;
        _wpi_setscrollrange( GUIGetParentFrameHWND( wnd ), bar, min, max, ( redraw ) ? TRUE : FALSE );
        if( bar == SB_HORZ ) {
            GUIRedrawScroll( wnd, SB_HORZ, redraw );
        } else {
            GUIRedrawScroll( wnd, SB_VERT, redraw );
        }
    }
}

int GUIGetScrollRange( gui_window *wnd, int bar )
{
    if( bar == SB_HORZ ) {
        return( wnd->hrange );
    } else {
        return( wnd->vrange );
    }
}

void GUISetRangePos( gui_window *wnd, int bar )
{
    int range;
    int pos;

    range = GUIGetScrollRange( wnd, bar );
    pos = GUIGetScrollPos( wnd, bar );
    _wpi_setscrollrange( GUIGetParentFrameHWND( wnd ), bar, 0, range, FALSE );
    _wpi_setscrollpos( GUIGetParentFrameHWND( wnd ), bar, pos, TRUE );
}

void GUIRedrawScroll( gui_window *wnd, int bar, bool redraw_now )
{
    WPI_RECT    rect;
    WPI_RECT    client;
    HWND        hwnd;
    GUI_RECTDIM left, top, right, bottom;
    GUI_RECTDIM clleft, cltop, clright, clbottom;

    hwnd = GUIGetParentFrameHWND( wnd );
    _wpi_getwindowrect( hwnd, &rect );
    _wpi_getclientrect( hwnd, &client );
    _wpi_mapwindowpoints( HWND_DESKTOP, hwnd, (WPI_LPPOINT)&rect, 2 );
    _wpi_getrectvalues( rect, &left, &top, &right, &bottom );
    _wpi_getrectvalues( client, &clleft, &cltop, &clright, &clbottom );
    if( bar == SB_HORZ ) {
        top = clbottom;
    } else {
        left = clright;
    }
    _wpi_setrectvalues( &rect, left, top, right, bottom );
    GUIInvalidatePaintHandles( wnd );
    _wpi_invalidaterect( hwnd, &rect, TRUE );
    if( redraw_now && !( wnd->flags & NEEDS_RESIZE_REDRAW ) ) {
        _wpi_updatewindow( hwnd );
    }
}

HWND GUIGetParentHWND( gui_window *wnd )
{
    if( wnd == NULL ) {
        return( NULLHANDLE );
    }
    if( wnd->root != NULLHANDLE ) {
        return( wnd->root );
    }
    return( wnd->hwnd );
}

HWND GUIGetParentFrameHWND( gui_window *wnd )
{
    if( wnd == NULL ) {
        return( NULLHANDLE );
    }
    if( wnd->root_frame != NULLHANDLE ) {
        return( wnd->root_frame );
    }
    return( wnd->hwnd_frame );
}

HWND GUIGetTopParentHWND( HWND hwnd )
{
    HWND curr_hwnd;
    HWND parent;

    curr_hwnd = hwnd;
    for( ;; ) {
        parent = _wpi_getparent( curr_hwnd );
        if( parent == HWND_DESKTOP ) break;
        curr_hwnd = parent;
    }
    return( curr_hwnd );
}

gui_window *GUIGetTopGUIWindow( HWND hwnd )
{
    hwnd = hwnd;
    return( GUIGetRootWindow() );
}

/*
 * GUIGetWindow - get the gui_window associated with the hwnd
 */

gui_window *GUIGetWindow( HWND hwnd )
{
    gui_window  *wnd;

    if( ( hwnd != NULLHANDLE ) && ( hwnd != HWND_DESKTOP ) ) {
        wnd = (gui_window *)_wpi_getwindowlongptr( hwnd, GUI_EXTRA_WORD * EXTRA_SIZE );
        return( wnd );
    }

    return( NULL );
}

gui_window *GUIGetParentWindow( gui_window *wnd )
{
    if( wnd != NULL ) {
        return( wnd->parent );
    } else {
        return( NULL );
    }
}

bool GUIParentHasFlags( gui_window *wnd, gui_flags flags )
{
    while( wnd ) {
        if( wnd->flags & flags ) {
            return( true );
        }
        wnd = GUIGetParentWindow( wnd );
    }

    return( false );
}

gui_window *GUIGetFirstSibling( gui_window *wnd )
{
    gui_window  *parent;

    if( wnd == NULL ) {
        return( NULL );
    }

    parent = wnd->parent;
    if( ( parent == NULL ) || ( parent->flags & IS_DIALOG ) ) {
        return( NULL );
    }

    return( GUIFindFirstChild( parent ) );
}

WPI_FONT GUIGetSystemFont( void )
{
    WPI_FONT    font;

#ifdef __OS2_PM__
    WPI_FONT    ret;

    font = NULL;
    ret = _wpi_getsystemfont();
    if( ret ) {
        font = (WPI_FONT)GUIMemAlloc( sizeof( *font ) );
        if( font ) {
            *font = *ret;
        }
    }
#else
    font = _wpi_getsystemfont();
#endif

    return( font );
}

