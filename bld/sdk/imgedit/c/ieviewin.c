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


#include "precomp.h"
#include "imgedit.h"
#include <string.h>
#include "ieprofil.h"

static int              showState = SW_SHOWNORMAL;
static COLORREF         bkgroundColor = BK_WHITE;
static HWND             hViewWindow;
static BOOL             fOneViewWindow = TRUE;

/*
 * drawBorder - draw the border for the view window
 */
static void drawBorder( img_node *node )
{
    WPI_PRES    presborder;
    HPEN        hgraypen;
    HPEN        hwhitepen;
    HPEN        hblackpen;
    HPEN        holdpen;
    WPI_RECT    rcclient;
    HBRUSH      hnewbrush;
    HBRUSH      holdbrush;
    HBRUSH      nullbrush;
    int         width;
    int         height;
#ifndef __NT__
    WPI_POINT   pt;
#endif
    int         top;
    int         bottom;

    presborder = _wpi_getpres( node->viewhwnd );
#if defined( __NT__ )
    hwhitepen = _wpi_createpen( PS_SOLID, 0, GetSysColor( COLOR_BTNHIGHLIGHT ) );
    hblackpen = _wpi_createpen( PS_SOLID, 0, GetSysColor( COLOR_BTNTEXT ) );
#else
    hwhitepen = _wpi_createpen( PS_SOLID, 0, CLR_WHITE );
    hblackpen = _wpi_createpen( PS_SOLID, 0, CLR_BLACK );
#endif

    GetClientRect( node->viewhwnd, &rcclient );
    width = _wpi_getwidthrect( rcclient );
    height = _wpi_getheightrect( rcclient );

    if( node->imgtype != BITMAP_IMG ) {
#if defined( __NT__ )
        hgraypen = _wpi_createpen( PS_SOLID, 0, GetSysColor( COLOR_BTNSHADOW ) );
#else
        hgraypen = _wpi_createpen( PS_SOLID, 0, CLR_DARKGRAY );
#endif
        holdpen = _wpi_selectobject( presborder, hgraypen );

#if defined( __NT__ )
        hnewbrush = _wpi_createsolidbrush( GetSysColor( COLOR_BTNFACE ) );
#else
        hnewbrush = _wpi_createsolidbrush( CLR_PALEGRAY );
#endif
        holdbrush = _wpi_selectobject( presborder, hnewbrush );

        top = 0;
        bottom = height;
        top = _wpi_cvth_y( top, height );
        bottom = _wpi_cvth_y( bottom, height );
        _wpi_rectangle( presborder, 0, top, width, bottom );

        /*
         * Draw black border and selected background color in the view window.
         */
        _wpi_selectobject( presborder, hblackpen );
        _wpi_selectobject( presborder, holdbrush );
        _wpi_deleteobject( hnewbrush );
        hnewbrush = _wpi_createsolidbrush( bkgroundColor );
        _wpi_selectobject( presborder, hnewbrush );

        top = BORDER_WIDTH - 1;
        bottom = height - BORDER_WIDTH + 1;
        top = _wpi_cvth_y( top, height );
        bottom = _wpi_cvth_y( bottom, height );
#ifndef __NT__
        /*
         * Draw the border relative to the size of the object being displayed,
         * not the window containing it.
         */
        _wpi_rectangle( presborder, BORDER_WIDTH - 1, top,
                        node->width + BORDER_WIDTH + 1, top + node->height + 2 );
#endif
        _wpi_selectobject( presborder, holdbrush );
        _wpi_selectobject( presborder, holdpen );
        _wpi_deleteobject( hnewbrush );
    } else {
#ifdef __OS2_PM__
        // I can't seem to get the thick pen to work so I'm using this
        // method.
        hgraypen = _wpi_createpen( PS_SOLID, 0, CLR_PALEGRAY );
        holdpen = _wpi_selectobject( presborder, hgraypen );
        hnewbrush = _wpi_createsolidbrush( CLR_PALEGRAY );
        holdbrush = _wpi_selectobject( presborder, hnewbrush );

        _wpi_rectangle( presborder, 0, 0, width + 1, BORDER_WIDTH + 1 );
        _wpi_rectangle( presborder, 0, 0, BORDER_WIDTH + 1, height + 1 );
        _wpi_rectangle( presborder, 0, height - BORDER_WIDTH, width + 1, height + 1 );
        _wpi_rectangle( presborder, width - BORDER_WIDTH, 0, width + 1, height + 1 );

        _wpi_selectobject( presborder, holdbrush );
        _wpi_deleteobject( hnewbrush );
        _wpi_selectobject( presborder, holdpen );
        _wpi_deleteobject( hgraypen );
#else
#if defined( __NT__ )
        hgraypen = _wpi_createpen( PS_INSIDEFRAME, BORDER_WIDTH,
                                   GetSysColor( COLOR_BTNFACE ) );
#else
        hgraypen = _wpi_createpen( PS_INSIDEFRAME, BORDER_WIDTH, CLR_PALEGRAY );
#endif
        holdpen = _wpi_selectobject( presborder, hgraypen );
        nullbrush = _wpi_createnullbrush();
        holdbrush = _wpi_selectbrush( presborder, nullbrush );

        _wpi_rectangle( presborder, 0, 0, rcclient.right, rcclient.bottom );
        _wpi_getoldbrush( presborder, holdbrush );
        _wpi_selectobject( presborder, holdpen );
        _wpi_deleteobject( hgraypen );
        _wpi_deletenullbrush( nullbrush );
#endif

        nullbrush = _wpi_createnullbrush();
#if defined( __NT__ )
        hgraypen = _wpi_createpen( PS_SOLID, 0, GetSysColor( COLOR_BTNSHADOW ) );
#else
        hgraypen = _wpi_createpen( PS_SOLID, 0, CLR_DARKGRAY );
#endif
        holdbrush = _wpi_selectbrush( presborder, nullbrush );
        holdpen = _wpi_selectobject( presborder, hgraypen );
        top = 0;
        bottom = height;
        top = _wpi_cvth_y( top, height );
        bottom = _wpi_cvth_y( bottom, height );
        _wpi_rectangle( presborder, 0, top, width, bottom );

        _wpi_selectobject( presborder, hblackpen );
        top = BORDER_WIDTH - 1;
        bottom = height - BORDER_WIDTH + 1;
        top = _wpi_cvth_y( top, height );
        bottom = _wpi_cvth_y( bottom, height );
#ifndef __NT__
        /*
         * Draw the border relative to the size of the object being displayed,
         * not the window containing it.
         */
        _wpi_rectangle( presborder, BORDER_WIDTH - 1, top,
                        node->width + BORDER_WIDTH + 1, top + node->height + 2 );
#endif

        _wpi_selectobject( presborder, holdpen );
        _wpi_selectbrush( presborder, holdbrush );
        _wpi_deletenullbrush( nullbrush );
    }

    /*
     * Give the view window the 3D effect.
     */
#ifndef __NT__
    holdpen = _wpi_selectobject( presborder, hwhitepen );

    _wpi_setpoint( &pt, 0, height - 1 );
    _wpi_cvth_pt( &pt, height );
    _wpi_movetoex( presborder, &pt, NULL );

    _wpi_setpoint( &pt, 0, 0 );
    _wpi_cvth_pt( &pt, height );
    _wpi_lineto( presborder, &pt );
    pt.x = width;
    _wpi_lineto( presborder, &pt );

    _wpi_setpoint( &pt, width - BORDER_WIDTH + 1, BORDER_WIDTH - 2 );
    _wpi_cvth_pt( &pt, height );
    _wpi_movetoex( presborder, &pt, NULL );

    pt.y = height - BORDER_WIDTH + 1;
    _wpi_cvth_pt( &pt, height );
    _wpi_lineto( presborder, &pt );
    pt.x = BORDER_WIDTH - 2;
    _wpi_lineto( presborder, &pt );

    _wpi_selectobject( presborder, hgraypen );

    _wpi_setpoint( &pt, BORDER_WIDTH - 2, BORDER_WIDTH - 2 );
    _wpi_cvth_pt( &pt, height );
    _wpi_lineto( presborder, &pt );
    pt.x = width - BORDER_WIDTH + 1;
    _wpi_lineto( presborder, &pt );

    _wpi_selectobject( presborder, holdpen );
#endif
    _wpi_deleteobject( hgraypen );
    _wpi_deleteobject( hwhitepen );
    _wpi_deleteobject( hblackpen );
    _wpi_releasepres( node->viewhwnd, presborder );

} /* drawBorder */

/*
 * redrawViewWnd - process the WM_PAINT message for the view windows
 */
static void redrawViewWnd( HWND hwnd )
{
    WPI_PRES    pres;
    WPI_PRES    hps;
    WPI_PRES    mempres;
    HDC         memdc;
    HBITMAP     bitmap;
    HBITMAP     oldbitmap;
    img_node    *node;
    PAINTSTRUCT ps;

    node = SelectFromViewHwnd( hwnd );
    if( node == NULL ) {
        return;
    }

    hps = _wpi_beginpaint( hwnd, NULL, &ps );

    drawBorder( node );
    pres = _wpi_getpres( hwnd );

    bitmap = CreateViewBitmap( node );
    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    oldbitmap = _wpi_selectbitmap( mempres, bitmap );

    _wpi_bitblt( pres, BORDER_WIDTH, BORDER_WIDTH, node->width, node->height,
                 mempres, 0, 0, SRCCOPY );
    _wpi_getoldbitmap( mempres, oldbitmap );
    _wpi_deletebitmap( bitmap );
    _wpi_deletecompatiblepres( mempres, memdc );

    _wpi_releasepres( hwnd, pres );
    _wpi_endpaint( hwnd, hps, &ps );

} /* redrawViewWnd */

/*
 * ViewEnumProc - enumerate the child windows and show the view window
 *                for each window
 */
BOOL CALLBACK ViewEnumProc( HWND hwnd, LONG lparam )
{
    lparam = lparam;

    if( _wpi_getowner( hwnd ) ) {
        return( TRUE );
    }

    if( _wpi_isiconic( hwnd ) ) {
        return( TRUE );
    } else {
        SendMessage( hwnd, UM_SHOWVIEWWINDOW, 0, 0L );
    }
    return( TRUE );

} /* ViewEnumProc */

/*
 * CreateViewWin - create the view window
 */
HWND CreateViewWin( int width, int height )
{
    HWND        hwnd;
#ifdef __OS2_PM__
    hwnd = PMCreateViewWin( hViewWindow, fOneViewWindow, &showState, width, height );
#else
    hwnd = WinCreateViewWin( hViewWindow, fOneViewWindow, &showState, width, height );
#endif
    return( hwnd );

} /* CreateViewWin */

/*
 * ViewWindowProc - window procedure for the view window
 */
MRESULT CALLBACK ViewWindowProc( HWND hwnd, WPI_MSG msg,
                                 WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    static HMENU                sysmenu;
    static HWND                 hframe;
    HMENU                       hmenu;
    WPI_RECT                    rcview;
    IMGED_DIM                   left;
    IMGED_DIM                   top;
    IMGED_DIM                   right;
    IMGED_DIM                   bottom;

    switch ( msg ) {
    case WM_CREATE:
        hframe = _wpi_getframe( hwnd );
        sysmenu = _wpi_getcurrentsysmenu( hframe );
        _wpi_deletemenu( sysmenu, SC_RESTORE, FALSE );
        _wpi_deletemenu( sysmenu, SC_SIZE, FALSE );
        _wpi_deletemenu( sysmenu, SC_MINIMIZE, FALSE );
        _wpi_deletemenu( sysmenu, SC_MAXIMIZE, FALSE );
        _wpi_deletemenu( sysmenu, SC_TASKLIST, FALSE );
#ifdef __OS2_PM__
        _wpi_deletemenu( sysmenu, SC_HIDE, FALSE );
#endif
        _wpi_deletesysmenupos( sysmenu, 1 );
        _wpi_deletesysmenupos( sysmenu, 2 );
        break;

    case WM_PAINT:
        redrawViewWnd( hwnd );
        return( 0 );

    case WM_MOVE:
        _wpi_getwindowrect( _wpi_getframe( hwnd ), &rcview );
        _wpi_getrectvalues( rcview, &left, &top, &right, &bottom );
        ImgedConfigInfo.view_xpos = (short)left;
        ImgedConfigInfo.view_ypos = (short)top;
        break;

    case WM_CLOSE:
        hmenu = GetMenu( _wpi_getframe( HMainWindow ) );
        if( fOneViewWindow ) {
            CheckViewItem( hmenu );
        } else {
            PrintHintTextByID( WIE_USEOPTIONSTOHIDEALL, NULL );
        }
        break;

    case WM_DESTROY:
        hViewWindow = NULL;
        break;

    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0 );

} /* ViewWindowProc */

/*
 * CheckViewItem - handle when the view window option is selected from the menu
 */
void CheckViewItem( HMENU hmenu )
{
    WPI_ENUMPROC        fp_enum;

    if( _wpi_isitemchecked( hmenu, IMGED_VIEW ) ) {
        _wpi_checkmenuitem( hmenu, IMGED_VIEW, MF_UNCHECKED, FALSE );
        showState = SW_HIDE;
        ImgedConfigInfo.show_state &= ~SET_SHOW_VIEW;
    } else {
        _wpi_checkmenuitem( hmenu, IMGED_VIEW, MF_CHECKED, FALSE );
        showState = SW_SHOWNORMAL;
        ImgedConfigInfo.show_state |= SET_SHOW_VIEW;
    }

    if( hViewWindow == NULL ) {
        return;
    }

    ShowWindow( _wpi_getframe( hViewWindow ), showState );
    if( _wpi_iswindow( Instance, ClientWindow ) ) {
        fp_enum = _wpi_makeenumprocinstance( ViewEnumProc, Instance );
        _wpi_enumchildwindows( ClientWindow, fp_enum, 0L );
        _wpi_freeprocinstance( fp_enum );
    }

} /* CheckViewItem */

/*
 * GetViewBkColor - return the selected background color
 */
COLORREF GetViewBkColor( void )
{
    return( bkgroundColor );

} /* GetViewBkColor */

/*
 * BkColorEnumProc - used to change the background color of all MDI children
 */
BOOL CALLBACK BkColorEnumProc( HWND hwnd, LONG lparam )
{
    img_node    *node;

    lparam = lparam;

    if( _wpi_getowner( hwnd ) ) {
        return( TRUE );
    }

    if( _wpi_isiconic( hwnd ) ) {
        return( TRUE );
    }
    node = SelectImage( _wpi_getclient( hwnd ) );
    if( node == NULL ) {
        return( TRUE );
    }
    if( node->imgtype == BITMAP_IMG ) {
        return( TRUE );
    }
    InvalidateRect( _wpi_getclient( hwnd ), NULL, FALSE );
    InvalidateRect( node->viewhwnd, NULL, FALSE );
    return( TRUE );

} /* BkColorEnumProc */

/*
 * SetViewBkColor - set the background color (and appropriate inverse)
 */
void SetViewBkColor( COLORREF color )
{
    WPI_ENUMPROC        fp_enum;

    if( color == bkgroundColor ) {
        return;
    }

    bkgroundColor = color;
    if( !_wpi_iswindow( Instance, hViewWindow ) ) {
        return;
    }
    InvalidateRect( hViewWindow, NULL, TRUE );

    fp_enum = _wpi_makeenumprocinstance( BkColorEnumProc, Instance );
    _wpi_enumchildwindows( ClientWindow, fp_enum, 0L );
    _wpi_freeprocinstance( fp_enum );

} /* SetViewBkColor */

/*
 * ResetViewWindow - when a new MDI child is activated, reset the position
 *                   of the view window
 *                 - first check if only 1 view window is being shown, or all of them
 */
void ResetViewWindow( HWND hwnd )
{
    WPI_RECT    currentloc;
    WPI_RECT    newloc;
    IMGED_DIM   left;
    IMGED_DIM   top;
    IMGED_DIM   right;
    IMGED_DIM   bottom;
    HWND        hframe;
    HWND        currentframe;

    hframe = _wpi_getframe( hwnd );
    if( hViewWindow != NULL ) {
        currentframe = _wpi_getframe( hViewWindow );
    } else {
        currentframe = NULL;
    }

    if( fOneViewWindow ) {
        _wpi_getwindowrect( hframe, &newloc );
        if( hViewWindow != NULL ) {
            _wpi_getwindowrect( currentframe, &currentloc );
            ShowWindow( currentframe, SW_HIDE );
        } else {
            currentloc = newloc;
        }
        _wpi_getrectvalues( currentloc, &left, &top, &right, &bottom );

        SetWindowPos( hframe, NULL, left, top,
                     _wpi_getwidthrect( newloc ), _wpi_getheightrect( newloc ),
                     SWP_MOVE | SWP_SIZE | SWP_NOZORDER | SWP_HIDEWINDOW );
        hViewWindow = hwnd;
        ShowWindow( hframe, showState );
    } else {
        hViewWindow = hwnd;
        ShowWindow( hframe, showState );
        _wpi_bringwindowtotop( hframe );
    }
#ifndef __OS2_PM__
    RedrawWindow( hwnd, NULL, NULL, RDW_UPDATENOW );
#endif

} /* ResetViewWindow */

/*
 * HideViewWindow - hide the view window when a draw pad is minimized
 */
void HideViewWindow( HWND hwnd )
{
    img_node    *node;

    node = SelectImage( hwnd );
    if( node == NULL ) {
        return;
    }
    ShowWindow( _wpi_getframe( node->viewhwnd ), SW_HIDE );

} /* HideViewWindow */

/*
 * RePositionViewWnd - reposition the size of the window (for when a
 *                     different icon is selected)
 */
void RePositionViewWnd( img_node *node )
{
    WPI_RECT    location;
    int         h_adj;
    int         v_adj;
    IMGED_DIM   left;
    IMGED_DIM   top;
    IMGED_DIM   right;
    IMGED_DIM   bottom;
    HWND        frame;
    
    frame = _wpi_getframe( node->viewhwnd );
    _wpi_getwindowrect( frame, &location );

    h_adj = 2 * _wpi_getsystemmetrics( SM_CXDLGFRAME ) + 2 * BORDER_WIDTH;
    v_adj = 2 * _wpi_getsystemmetrics( SM_CYDLGFRAME ) +
#ifndef __NT__
                _wpi_getsystemmetrics( SM_CYCAPTION ) + 2 * BORDER_WIDTH - 1;
#else
                _wpi_getsystemmetrics( SM_CYSMCAPTION ) + 2 * BORDER_WIDTH - 1;
#endif
    _wpi_getrectvalues( location, &left, &top, &right, &bottom );
#ifdef __OS2_PM__
    SetWindowPos( frame, NULL, left, bottom, h_adj + node->width, v_adj + node->height,
                 SWP_SIZE | SWP_MOVE | SWP_NOZORDER | SWP_HIDEWINDOW );
#else
    SetWindowPos( node->viewhwnd, NULL, left, top, h_adj + node->width, v_adj + node->height,
                 SWP_SIZE | SWP_MOVE | SWP_NOZORDER | SWP_HIDEWINDOW );
#endif
    hViewWindow = node->viewhwnd;
    ShowWindow( frame, showState );
    SetFocus( HMainWindow );
    InvalidateRect( hViewWindow, NULL, TRUE );

} /* RePositionViewWnd */

/*
 * SetViewWindow - set whether there is a single view window or multiple ones
 */
void SetViewWindow( BOOL justone )
{
    WPI_ENUMPROC        fp_enum;
    int                 prev_show_state;

    fOneViewWindow = justone;

    if( !justone ) {
        if( _wpi_iswindow( Instance, ClientWindow ) ) {
            fp_enum = _wpi_makeenumprocinstance( ViewEnumProc, Instance );
            _wpi_enumchildwindows( ClientWindow, fp_enum, 0L );
            _wpi_freeprocinstance( fp_enum );
        }
    } else {
        prev_show_state = showState;
        showState = SW_HIDE;
        if( _wpi_iswindow( Instance, ClientWindow ) ) {
            fp_enum = _wpi_makeenumprocinstance( ViewEnumProc, Instance );
            _wpi_enumchildwindows( ClientWindow, fp_enum, 0L );
            _wpi_freeprocinstance( fp_enum );
        }
        showState = prev_show_state;
        if( _wpi_iswindow( Instance, hViewWindow ) ) {
            ShowWindow( hViewWindow, showState );
        }
    }

} /* SetViewWindow */

/*
 * ShowViewWindows - toggle the showing of multiple view windows
 */
void ShowViewWindows( HWND hwnd )
{
    img_node    *node;

    node = GetImageNode( hwnd );
    if( node == NULL ) {
        return;
    }

    if( fOneViewWindow ) {
        if( hViewWindow != node->viewhwnd ) {
            ShowWindow( node->viewhwnd, SW_HIDE );
        } else {
            ShowWindow( node->viewhwnd, showState );
        }
    } else {
        ShowWindow( node->viewhwnd, showState );
    }

} /* ShowViewWindows */

/*
 * IsOneViewWindow - return TRUE if there is only 1 view window or FALSE otherwise
 */
BOOL IsOneViewWindow( void )
{
    return( fOneViewWindow );

} /* IsOneViewWindow */
