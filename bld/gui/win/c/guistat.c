/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2018 The Open Watcom Contributors. All Rights Reserved.
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

static void SetStatusRect( HWND parent, WPI_RECT *status, int x, int height )
{
    WPI_RECT    client;
    GUI_RECTDIM left, right, top, bottom;
    int         y, h;

    _wpi_getclientrect( parent, &client );
    _wpi_getrectvalues( client, &left, &top, &right, &bottom );
    y = _wpi_cvth_y_plus1( bottom - height, bottom - top );
    h = _wpi_cvth_y_plus1( bottom - top, bottom - top );
    _wpi_setwrectvalues( status, x, y, right - left, h );
}

static void DoResizeStatus( gui_window *wnd )
{
    WPI_RECT    status;
    GUI_RECTDIM left, top, right, bottom;

    if( GUIHasStatus( wnd ) ) {
        _wpi_getwindowrect( wnd->status, &status );
        _wpi_mapwindowpoints( HWND_DESKTOP, wnd->root, (WPI_LPPOINT)&status, 2 );
        _wpi_getrectvalues( status, &left, &top, &right, &bottom );
        /* maintain height and left position of status window -- tie the
           rest to the client are of the parent */
        SetStatusRect( wnd->root, &status, left, bottom - top );
        _wpi_getrectvalues( status, &left, &top, &right, &bottom );
        _wpi_movewindow( wnd->status, left, top, right - left, bottom - top, TRUE );
    }
}

static void CalcStatusRect( gui_window *wnd, gui_ord x, gui_ord height,
                            WPI_RECT *rect )
{
    gui_text_metrics    metrics;
    gui_coord           size;
    gui_coord           pos;

    pos.x = x;
    GUIScaleToScreenR( &pos );
    if( height == 0 ) {
        GUIGetTextMetrics( wnd, &metrics );
        size.y = metrics.max.y;
    } else {
        size.y = height;
    }
    GUIScaleToScreenR( &size );
    if( height == 0 ) {
        size.y += TOTAL_VERT + 2; /* windows is 2 pixels higher than client */
    }
    SetStatusRect( wnd->root, rect, pos.x, size.y );
}

/*
 * GUICreateStatusWindow -- create a status window.  For now, only look
 *                          at rect.left and rect.height for inforamation.
 *                          Tie the rest to the parent window.
 */

bool GUICreateStatusWindow( gui_window *wnd, gui_ord x, gui_ord height,
                            gui_colour_set *colour )
{
    WPI_RECT            status_rect;

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
    CalcStatusRect( wnd, x, height, &status_rect );
    wnd->status = StatusWndCreate( GUIStatusWnd, wnd->root, &status_rect,
                                   GUIMainHInst, NULL );
    if( wnd->status == NULLHANDLE ) {
        return( false );
    }
    DoResizeStatus( wnd );
    GUIResizeBackground( wnd, true );
    return( true );
}

bool GUIDrawStatusText( gui_window *wnd, const char *text )
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

bool GUIHasStatus( gui_window *wnd )
{
    return( wnd->status != NULLHANDLE );
}


bool GUICloseStatusWindow( gui_window *wnd )
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

bool GUIResizeStatusWindow( gui_window *wnd, gui_ord x, gui_ord height )
{
    WPI_RECT    status;
    GUI_RECTDIM left, top, right, bottom;

    if( !GUIHasStatus( wnd ) ) {
        return( false );
    }
    CalcStatusRect( wnd, x, height, &status );
    _wpi_getrectvalues( status, &left, &top, &right, &bottom );
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
