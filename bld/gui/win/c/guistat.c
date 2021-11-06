/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Status Window handling
*
****************************************************************************/


#include "guiwind.h"
#include <string.h>
#include "guiscale.h"
#include "guixwind.h"
#include "statwnd.h"
#include "guiutil.h"
#include "guixhook.h"
#include "guistr.h"
#include "guixutil.h"
#include "guistat.h"


statwnd         *GUIStatusWnd;

static  void    (*ResizeStatus)( gui_window * ) = NULL;
static  void    (*FreeStatus)( void )           = NULL;

static void DoFreeStatus( void )
{
    StatusWndDestroy( GUIStatusWnd );
    StatusWndFini();
}

static void SetStatusRect( HWND parent, WPI_RECT *status_wpi_rect, guix_ord x, guix_ord height )
{
    WPI_RECT    wpi_rect;
    WPI_RECTDIM left, right, top, bottom;
    guix_ord    y;
    guix_ord    h;

    _wpi_getclientrect( parent, &wpi_rect );
    _wpi_getrectvalues( wpi_rect, &left, &top, &right, &bottom );
    y = _wpi_cvth_y_plus1( bottom - height, bottom - top );
    h = _wpi_cvth_y_plus1( bottom - top, bottom - top );
    _wpi_setwrectvalues( status_wpi_rect, x, y, right - left, h );
}

static void DoResizeStatus( gui_window *wnd )
{
    WPI_RECT    wpi_rect;
    WPI_RECTDIM left, top, right, bottom;

    if( GUIHasStatus( wnd ) ) {
        _wpi_getwindowrect( wnd->status, &wpi_rect );
        _wpi_mapwindowpoints( HWND_DESKTOP, wnd->root, (WPI_LPPOINT)&wpi_rect, 2 );
        _wpi_getrectvalues( wpi_rect, &left, &top, &right, &bottom );
        /* maintain height and left position of status window -- tie the
           rest to the client are of the parent */
        SetStatusRect( wnd->root, &wpi_rect, left, bottom - top );
        _wpi_getrectvalues( wpi_rect, &left, &top, &right, &bottom );
        _wpi_movewindow( wnd->status, left, top, right - left, bottom - top, TRUE );
    }
}

static void CalcStatusRect( gui_window *wnd, gui_ord x, gui_ord height, WPI_RECT *wpi_rect )
{
    gui_text_metrics    metrics;
    guix_ord            size_y;

    if( height == 0 ) {
        GUIGetTextMetrics( wnd, &metrics );
        /* windows is 2 pixels higher than client */
        size_y = GUIScaleToScreenV( metrics.max.y ) + TOTAL_VERT + 2;
    } else {
        size_y = GUIScaleToScreenV( height );
    }
    SetStatusRect( wnd->root, wpi_rect, GUIScaleToScreenH( x ), size_y );
}

/*
 * GUICreateStatusWindow -- create a status window.  For now, only look
 *                          at rect.left and rect.height for inforamation.
 *                          Tie the rest to the parent window.
 */

bool GUIAPI GUICreateStatusWindow( gui_window *wnd, gui_ord x, gui_ord height,
                            gui_colour_set *colour )
{
    WPI_RECT    wpi_rect;

    colour = colour;
    if( wnd->root == NULLHANDLE ) {
        return( false );
    }
    ResizeStatus = &DoResizeStatus;
    FreeStatus = &DoFreeStatus;
    if( !StatusWndInit( GUIMainHInst, NULL, 0, NULLHANDLE ) ) {
        return( false );
    }
    GUIStatusWnd = StatusWndStart();
    CalcStatusRect( wnd, x, height, &wpi_rect );
    wnd->status = StatusWndCreate( GUIStatusWnd, wnd->root, &wpi_rect, GUIMainHInst, NULL );
    if( wnd->status == NULLHANDLE ) {
        return( false );
    }
    DoResizeStatus( wnd );
    GUIResizeBackground( wnd, true );
    return( true );
}

bool GUIAPI GUIDrawStatusText( gui_window *wnd, const char *text )
{
    WPI_PRES    pres;
    const char  *out_text;

    if( !GUIHasStatus( wnd) ) {
        return( false );
    }
    pres = _wpi_getpres( wnd->status );
    if( ( text == NULL ) || ( *text == '\0' ) ) {
        out_text = LIT( Blank );
    } else {
        out_text = text;
    }
    StatusWndDrawLine( GUIStatusWnd, pres, wnd->font, out_text, DT_SINGLELINE | DT_VCENTER | DT_LEFT );
    _wpi_releasepres( wnd->status, pres );
    if( ( text == NULL ) || ( *text == '\0' ) ) {
        GUIEVENT( wnd, GUI_STATUS_CLEARED, NULL );
    }
    return( true );
}

bool GUIAPI GUIHasStatus( gui_window *wnd )
{
    return( wnd->status != NULLHANDLE );
}


bool GUIAPI GUICloseStatusWindow( gui_window *wnd )
{
    HWND        status;
    if( !GUIHasStatus( wnd ) ) {
        return( false );
    }
    status = wnd->status;
    wnd->status = NULLHANDLE;
    DestroyWindow( status );
    GUIResizeBackground( wnd, true );
    return( true );
}

bool GUIAPI GUIResizeStatusWindow( gui_window *wnd, gui_ord x, gui_ord height )
{
    WPI_RECT    wpi_rect;
    WPI_RECTDIM left, top, right, bottom;

    if( !GUIHasStatus( wnd ) ) {
        return( false );
    }
    CalcStatusRect( wnd, x, height, &wpi_rect );
    _wpi_getrectvalues( wpi_rect, &left, &top, &right, &bottom );
    _wpi_movewindow( wnd->status, left, top, right - left, bottom - top, TRUE );
    GUIResizeBackground( wnd, true );
    return( true );
}

void GUIResizeStatus( gui_window *wnd )
{
    if( ResizeStatus != NULL ) {
        (*ResizeStatus)( wnd );
    }
}

void GUIFreeStatus( void )
{
    if( FreeStatus != NULL ) {
        (*FreeStatus)();
    }
}
