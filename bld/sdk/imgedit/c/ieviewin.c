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


#include <string.h>
#include "imgedit.h"
#include "ieprofil.h"

static int              showState = SW_SHOWNORMAL;
static COLORREF         bkgroundColour = BK_WHITE;
static HWND             hViewWindow;
static BOOL             fOneViewWindow = TRUE;

/*
 * drawBorder - Draws the border for the view window.
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
    WPI_POINT   pt;
    int         top;
    int         bottom;

    presborder = _wpi_getpres( node->viewhwnd );
    hwhitepen = _wpi_createpen( PS_SOLID, 0, CLR_WHITE );
    hblackpen = _wpi_createpen( PS_SOLID, 0, CLR_BLACK );

    GetClientRect(node->viewhwnd, &rcclient);
    width = _wpi_getwidthrect( rcclient );
    height = _wpi_getheightrect( rcclient );

    if (node->imgtype != BITMAP_IMG) {
        hgraypen = _wpi_createpen( PS_SOLID, 0, CLR_DARKGRAY );
        holdpen = _wpi_selectobject(presborder, hgraypen );

        hnewbrush = _wpi_createsolidbrush( CLR_PALEGRAY );
        holdbrush = _wpi_selectobject( presborder, hnewbrush );

        top = 0;
        bottom = height;
        top = _wpi_cvth_y( top, height );
        bottom = _wpi_cvth_y( bottom, height );
        _wpi_rectangle( presborder, 0, top, width, bottom );

        /*
         * Draws black border and selected background colour in the view window
         */
        _wpi_selectobject( presborder, hblackpen );
        _wpi_selectobject( presborder, holdbrush );
        _wpi_deleteobject( hnewbrush );
        hnewbrush = _wpi_createsolidbrush( bkgroundColour );
        _wpi_selectobject( presborder, hnewbrush );

        top = BORDER_WIDTH - 1;
        bottom = height - BORDER_WIDTH + 1;
        top = _wpi_cvth_y( top, height );
        bottom = _wpi_cvth_y( bottom, height );
        _wpi_rectangle( presborder, BORDER_WIDTH - 1, top,
                                    width - BORDER_WIDTH + 1, bottom );

        _wpi_selectobject(presborder, holdbrush );
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

        _wpi_rectangle(presborder, 0, 0, width+1, BORDER_WIDTH+1);
        _wpi_rectangle(presborder, 0, 0, BORDER_WIDTH+1, height+1);
        _wpi_rectangle(presborder, 0, height-BORDER_WIDTH, width+1, height+1);
        _wpi_rectangle(presborder, width-BORDER_WIDTH, 0, width+1, height+1);

        _wpi_selectobject( presborder, holdbrush );
        _wpi_deleteobject( hnewbrush );
        _wpi_selectobject( presborder, holdpen );
        _wpi_deleteobject( hgraypen );
#else
        hgraypen = _wpi_createpen( PS_INSIDEFRAME, BORDER_WIDTH, CLR_PALEGRAY );
        holdpen = _wpi_selectobject( presborder, hgraypen );
        nullbrush = _wpi_createnullbrush();
        holdbrush = _wpi_selectbrush( presborder, nullbrush );

        _wpi_rectangle(presborder, 0, 0, rcclient.right, rcclient.bottom);
        _wpi_getoldbrush( presborder, holdbrush );
        _wpi_selectobject( presborder, holdpen );
        _wpi_deleteobject( hgraypen );
        _wpi_deletenullbrush( nullbrush );
#endif

        nullbrush = _wpi_createnullbrush();
        hgraypen = _wpi_createpen( PS_SOLID, 0, CLR_DARKGRAY );
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
        _wpi_rectangle(presborder, BORDER_WIDTH-1, top,
                                        width - BORDER_WIDTH + 1, bottom);

        _wpi_selectobject( presborder, holdpen );
        _wpi_selectbrush( presborder, holdbrush );
        _wpi_deletenullbrush( nullbrush );
    }

    /*
     * Give the view window the 3D effect.
     */
    holdpen = _wpi_selectobject( presborder, hwhitepen );

    _wpi_setpoint( &pt, 0, height-1 );
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
    _wpi_deleteobject( hgraypen );
    _wpi_deleteobject( hwhitepen );
    _wpi_deleteobject( hblackpen );
    _wpi_releasepres( node->viewhwnd, presborder );
} /* drawBorder */

/*
 * redrawViewWnd - processes the WM_PAINT message for the view windows.
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
    if (!node) return;

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
 * ViewEnumProc - enumerates the child windows and shows the view window
 *                for each window.
 */
BOOL CALLBACK ViewEnumProc( HWND hwnd, LONG lparam )
{
    lparam = lparam;

    if ( _wpi_getowner(hwnd) ) {
        return 1;
    }

    if ( _wpi_isiconic(hwnd) ) {
        return 1;
    } else {
        SendMessage( hwnd, UM_SHOWVIEWWINDOW, 0, 0L );
    }
    return 1;
} /* ViewEnumProc */

/*
 * CreateViewWin - Creates the view window
 */
HWND CreateViewWin( int width, int height )
{
    HWND        hwnd;
#ifdef __OS2_PM__
    hwnd = PMCreateViewWin(hViewWindow, fOneViewWindow, &showState, width,
                                                                height );
#else
    hwnd = WinCreateViewWin(hViewWindow, fOneViewWindow, &showState, width,
                                                                height);
#endif
    return( hwnd );

} /* CreateViewWin */

/*
 * ViewWindowProc - Window procedure for the view window.
 */
MRESULT CALLBACK ViewWindowProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                                                        WPI_PARAM2 lparam )
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
        return 0;

    case WM_MOVE:
        _wpi_getwindowrect( _wpi_getframe(hwnd), &rcview );
        _wpi_getrectvalues( rcview, &left, &top, &right, &bottom );
        ImgedConfigInfo.view_xpos = (short)left;
        ImgedConfigInfo.view_ypos = (short)top;
        break;

    case WM_CLOSE:
        hmenu = GetMenu(_wpi_getframe(HMainWindow));
        if (fOneViewWindow) {
            CheckViewItem(hmenu);
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
    return(0);

} /* ViewWindowProc */

/*
 * CheckViewItem - This procedure handles when the view window option is
 *                 selected from the menu.
 */
void CheckViewItem( HMENU hmenu )
{
    WPI_ENUMPROC        fp_enum;

    if ( _wpi_isitemchecked(hmenu, IMGED_VIEW) ) {
        _wpi_checkmenuitem(hmenu, IMGED_VIEW, MF_UNCHECKED, FALSE);
        showState = SW_HIDE;
        ImgedConfigInfo.show_state &= ~SET_SHOW_VIEW;
    } else {
        _wpi_checkmenuitem( hmenu, IMGED_VIEW, MF_CHECKED, FALSE );
        showState = SW_SHOWNORMAL;
        ImgedConfigInfo.show_state |= SET_SHOW_VIEW;
    }

    if ( !hViewWindow ) return;

    ShowWindow( _wpi_getframe(hViewWindow), showState );
    if ( _wpi_iswindow(Instance, ClientWindow) ) {
        fp_enum = _wpi_makeenumprocinstance( ViewEnumProc, Instance );
        _wpi_enumchildwindows( ClientWindow, fp_enum, 0L );
        _wpi_freeprocinstance( fp_enum );
    }
} /* CheckViewItem */

/*
 * GetBkColour - returns the selected background colour.
 */
COLORREF GetBkColour( void )
{
    return( bkgroundColour );
} /* GetBkColour */

/*
 * BkColourEnumProc - used to change the background colour of all mdi
 *                    children
 */
BOOL CALLBACK BkColourEnumProc( HWND hwnd, LONG lparam )
{
    img_node    *node;

    lparam = lparam;

    if (_wpi_getowner(hwnd)) {
        return 1;
    }

    if ( _wpi_isiconic(hwnd) ) {
        return 1;
    }
    node = SelectImage( _wpi_getclient(hwnd) );
    if (!node) return 1;
    if (node->imgtype == BITMAP_IMG) {
        return 1;
    }
    InvalidateRect( _wpi_getclient(hwnd), NULL, FALSE );
    InvalidateRect( node->viewhwnd, NULL, FALSE );
    return 1;
} /* BkColourEnumProc */

/*
 * SetBkColour - Sets the back ground colour (and appropriate inverse)
 */
void SetBkColour( COLORREF colour )
{
    WPI_ENUMPROC        fp_enum;

    if (colour == bkgroundColour) {
        return;
    }

    bkgroundColour = colour;
    if ( !(_wpi_iswindow(Instance, hViewWindow)) ) {
        return;
    }
    InvalidateRect(hViewWindow, NULL, TRUE);

    fp_enum = _wpi_makeenumprocinstance( BkColourEnumProc, Instance );
    _wpi_enumchildwindows( ClientWindow, fp_enum, 0L );
    _wpi_freeprocinstance( fp_enum );
} /* SetBkColour */

/*
 * ResetViewWindow - When a new mdi child is activated, we reset the position
 *                   of the view window.  First we check if only 1 view
 *                   window is being shown, or all of them.
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
    if (hViewWindow) {
        currentframe = _wpi_getframe( hViewWindow );
    } else {
        currentframe = NULL;
    }

    if (fOneViewWindow) {
        _wpi_getwindowrect( hframe, &newloc );
        if (hViewWindow) {
            _wpi_getwindowrect( currentframe, &currentloc );
            ShowWindow( currentframe, SW_HIDE );
        } else {
            currentloc = newloc;
        }
        _wpi_getrectvalues( currentloc, &left, &top, &right, &bottom );

        SetWindowPos(hframe,
                     NULL,
                     left,
                     top,
                     _wpi_getwidthrect(newloc),
                     _wpi_getheightrect(newloc),
                     SWP_MOVE | SWP_SIZE | SWP_NOZORDER | SWP_HIDEWINDOW);
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
 * HideViewWindow - Hides the view window when a draw pad is minimized.
 */
void HideViewWindow( HWND hwnd )
{
    img_node    *node;

    node = SelectImage(hwnd);
    if (!node) return;
    ShowWindow( _wpi_getframe(node->viewhwnd), SW_HIDE );
} /* HideViewWindow */

/*
 * RePositionViewWnd - Repositions the size of the window (for when a
 *                     different icon is selected.
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

    h_adj = 2 * _wpi_getsystemmetrics(SM_CXBORDER) + 2 * BORDER_WIDTH;
    v_adj = 2 * _wpi_getsystemmetrics(SM_CYBORDER) +
                _wpi_getsystemmetrics(SM_CYCAPTION) + 2 * BORDER_WIDTH - 1;
    _wpi_getrectvalues(location, &left, &top, &right, &bottom);
#ifdef __OS2_PM__
    SetWindowPos(frame,
                 NULL,
                 left,
                 bottom,
                 h_adj + node->width,
                 v_adj + node->height,
                 SWP_SIZE | SWP_MOVE | SWP_NOZORDER | SWP_HIDEWINDOW);
#else
    SetWindowPos(node->viewhwnd,
                 NULL,
                 left,
                 top,
                 h_adj + node->width,
                 v_adj + node->height,
                 SWP_SIZE | SWP_MOVE | SWP_NOZORDER | SWP_HIDEWINDOW);
#endif

    hViewWindow = node->viewhwnd;
    ShowWindow( frame, showState );
    SetFocus( HMainWindow );
    InvalidateRect( hViewWindow, NULL, TRUE );
} /* RePositionViewWnd */

/*
 * SetViewWindow - sets whether there is a single view window or multiple
 *                  ones.
 */
void SetViewWindow( BOOL justone )
{
    WPI_ENUMPROC        fp_enum;
    int                 prev_show_state;

    fOneViewWindow = justone;

    if (!justone) {
        if ( _wpi_iswindow(Instance, ClientWindow) ) {
            fp_enum = _wpi_makeenumprocinstance( ViewEnumProc, Instance );
            _wpi_enumchildwindows( ClientWindow, fp_enum, 0L );
            _wpi_freeprocinstance( fp_enum );
        }
    } else {
        prev_show_state = showState;
        showState = SW_HIDE;
        if ( _wpi_iswindow(Instance, ClientWindow) ) {
            fp_enum = _wpi_makeenumprocinstance( ViewEnumProc, Instance );
            _wpi_enumchildwindows( ClientWindow, fp_enum, 0L );
            _wpi_freeprocinstance( fp_enum );
        }
        showState = prev_show_state;
        if ( _wpi_iswindow(Instance, hViewWindow) ) {
            ShowWindow(hViewWindow, showState);
        }
    }
} /* SetViewWindow */

/*
 * ShowViewWindows - toggles the showing of multiple view windows.
 */
void ShowViewWindows( HWND hwnd )
{
    img_node    *node;

    node = GetImageNode( hwnd );
    if (!node) return;

    if (fOneViewWindow) {
        if (hViewWindow != node->viewhwnd) {
            ShowWindow( node->viewhwnd, SW_HIDE );
        } else {
            ShowWindow( node->viewhwnd, showState );
        }
    } else {
        ShowWindow( node->viewhwnd, showState );
    }
} /* ShowViewWindows */

/*
 * IsOneViewWindow - returns TRUE if there is only 1 view window or FALSE
 *                   otherwise
 */
BOOL IsOneViewWindow( void )
{
    return( fOneViewWindow );
} /* IsOneViewWindow */

