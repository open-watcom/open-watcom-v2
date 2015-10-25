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


#include "imgedit.h"
#include "ietoolbr.h"
#include "ieprofil.h"
#include "loadcc.h"

int TBWidth = TB_WIDTH;
int TBHeight = TB_HEIGHT;

static void             *toolBar;
static WORD             currentTool;
static BOOL             hotspotPresent = FALSE;
static button_bitmaps   bitmaps[NUMBER_OF_TOOLS] = {
    { CLPRECT,  CLPRECTD,   NULL, NULL },
    { PENCIL,   PENCILD,    NULL, NULL },
    { LINE,     LINED,      NULL, NULL },
    { BRUSH,    BRUSHD,     NULL, NULL },
    { CIRCLEO,  CIRCLEOD,   NULL, NULL },
    { CIRCLEF,  CIRCLEFD,   NULL, NULL },
    { RECTO,    RECTOD,     NULL, NULL },
    { RECTF,    RECTFD,     NULL, NULL },
    { FILL,     FILLD,      NULL, NULL },
    { HOTSPOT,  HOTSPOTD,   NULL, NULL }
};

/*
 * ToolBarHelpProc
 */
static void ToolBarHelpProc( HWND hwnd, WPI_PARAM1 wparam, bool pressed )
{
    hwnd = hwnd;
    if( pressed ) {
        ShowHintText( wparam );
    } else {
        SetHintText( " " );
    }

} /* ToolBarHelpProc */

/*
 * ToolBarProc - hook function that intercepts messages to the toolbar
 */
bool ToolBarProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    HMENU               hmenu;
    static HMENU        hsysmenu;
    short               i;
    WORD                id;
    WPI_RECT            rctool;
    HWND                hframe;
    IMGED_DIM           left;
    IMGED_DIM           top;

    switch( msg ) {
    case WM_CREATE:
        currentTool = IMGED_FREEHAND;
        hframe = _wpi_getframe( hwnd );
        hsysmenu = _wpi_getcurrentsysmenu( hframe );
        _wpi_deletemenu( hsysmenu, SC_RESTORE, FALSE );
        _wpi_deletemenu( hsysmenu, SC_SIZE, FALSE );
        _wpi_deletemenu( hsysmenu, SC_MINIMIZE, FALSE );
        _wpi_deletemenu( hsysmenu, SC_MAXIMIZE, FALSE );
        _wpi_deletemenu( hsysmenu, SC_TASKLIST, FALSE );
#ifdef __OS2_PM__
        _wpi_deletemenu( hsysmenu, SC_HIDE, FALSE );
#endif
        _wpi_deletesysmenupos( hsysmenu, 1 );
        _wpi_deletesysmenupos( hsysmenu, 2 );
        break;

    case WM_USER:
        if( !lparam ) {
            ShowHintText( wparam );
        }

        id = LOWORD( wparam );
        ToolBarSetState( toolBar, id, lparam ? BUTTON_UP : BUTTON_DOWN );
        if( id != currentTool ) {
            if( lparam ) {
                ToolBarSetState( toolBar, currentTool, BUTTON_DOWN );
            } else {
                ToolBarSetState( toolBar, currentTool, BUTTON_UP );
            }
        } else {
            ToolBarSetState( toolBar, currentTool, BUTTON_DOWN );
            if( !lparam ) {
                currentTool = id;
            }
        }
        break;

    case WM_CLOSE:
        hmenu = _wpi_getmenu( _wpi_getframe( HMainWindow ) );
        CheckToolbarItem( hmenu );
        return( true );

    case WM_MOVE:
        _wpi_getwindowrect( _wpi_getframe( hwnd ), &rctool );
        _wpi_getrectvalues( rctool, &left, &top, NULL, NULL );
        ImgedConfigInfo.tool_xpos = (short)left;
        ImgedConfigInfo.tool_ypos = (short)top;
        break;

    case WM_DESTROY:
        for( i = 0; i < NUMBER_OF_TOOLS; i++ ) {
            _wpi_deletebitmap( bitmaps[i].hToolBmp );
            _wpi_deletebitmap( bitmaps[i].hToolDep );
        }
        break;

    default:
        return( false );
    }
    return( false );

} /* ToolBarProc */

/*
 * InitTools - initializes the image editor toolbar
 */
void InitTools( HWND hparent )
{
    WPI_POINT           buttonsize = { TB_BUTTONSIZE, TB_BUTTONSIZE };
    WPI_POINT           border = { TB_BORDER, TB_BORDER };
    WPI_RECT            toolbar_loc;
    TOOLDISPLAYINFO     tdi;
    TOOLITEMINFO        tii;
    short               i;
    HMENU               hmenu;
    HWND                htoolbar;

    //TBWidth = TB_WIDTH - 2 + 2 * _wpi_getsystemmetrics( SM_CXBORDER );
    //TBHeight = TB_HEIGHT - 2 + 2 * _wpi_getsystemmetrics( SM_CYBORDER );

#ifdef __NT__
    if( IsCommCtrlLoaded() ) {
        TBWidth = TB_WIDTHCC;
        TBHeight = TB_HEIGHTCC;
    }
#endif

    _wpi_setrectvalues( &toolbar_loc, (IMGED_DIM)ImgedConfigInfo.tool_xpos,
                                      (IMGED_DIM)ImgedConfigInfo.tool_ypos,
                                      (IMGED_DIM)(ImgedConfigInfo.tool_xpos + TBWidth),
                                      (IMGED_DIM)(ImgedConfigInfo.tool_ypos + TBHeight) );

    for( i = 0; i < NUMBER_OF_TOOLS; i++ ) {
        bitmaps[i].hToolBmp = _wpi_loadbitmap( Instance, bitmaps[i].pszToolBmp );
        bitmaps[i].hToolDep = _wpi_loadbitmap( Instance, bitmaps[i].pszToolDep );
    }

    toolBar = ToolBarInit( hparent );
    tdi.button_size = buttonsize;
    tdi.border_size = border;
    tdi.area = toolbar_loc;
    tdi.style = TOOLBAR_FLOATNOSIZE_STYLE;
//    tdi.style = TOOLBAR_FLOAT_STYLE;
    tdi.hook = ToolBarProc;
    tdi.helphook = ToolBarHelpProc;
    tdi.background = (HBITMAP)0;
    tdi.foreground = (HBRUSH)0;
    tdi.is_fixed = FALSE;

    ToolBarDisplay( toolBar, &tdi );

    htoolbar = ToolBarWindow( toolBar );
    if( htoolbar != (HWND)NULL ) {
        RECT    rect;
        int     w, h;
        _wpi_getclientrect( htoolbar, &rect );
        w = _wpi_getwidthrect( rect );
        h = _wpi_getheightrect( rect );
        if( w < TBWidth || h < TBHeight ) {
            GetWindowRect( htoolbar, &rect );
            w = _wpi_getwidthrect( rect ) + (TBWidth - w);
            h = _wpi_getheightrect( rect ) + (TBHeight - h);
            SetWindowPos( htoolbar, HWND_TOP, 0, 0, w, h,
                          SWP_SIZE | SWP_NOZORDER | SWP_NOMOVE );
        }
    }

    tii.flags = ITEM_DOWNBMP | ITEM_STICKY;

    for( i = 0; i < NUMBER_OF_TOOLS - 1; i++ ) {
        tii.u.bmp = bitmaps[i].hToolBmp;
        tii.id = i + IMGED_CLIP;
        tii.depressed = bitmaps[i].hToolDep;
        ToolBarAddItem( toolBar, &tii );
    }

    hmenu = _wpi_getmenu( _wpi_getframe( HMainWindow ) );
    if( ImgedConfigInfo.show_state & SET_SHOW_TOOL ) {
        CheckToolbarItem( hmenu );
    }
    ToolBarSetState( toolBar, IMGED_FREEHAND, BUTTON_DOWN );
    SetToolType( IMGED_FREEHAND );
    _wpi_enablemenuitem( hmenu, IMGED_HOTSPOT, FALSE, FALSE );
    SendMessage( htoolbar, WM_SIZE, 0, 0 );

} /* InitTools */

/*
 * CheckToolbarItem - handle when the view toolbar option is selected from the menu
 */
void CheckToolbarItem( HMENU hmenu )
{
    HWND        htoolbar;

    htoolbar = ToolBarWindow( toolBar );

    if( _wpi_isitemchecked( hmenu, IMGED_TOOLBAR ) ) {
        _wpi_checkmenuitem( hmenu, IMGED_TOOLBAR, MF_UNCHECKED, FALSE );
        _wpi_showwindow( htoolbar, SW_HIDE );
        ImgedConfigInfo.show_state &= ~SET_SHOW_TOOL;
    } else {
        _wpi_checkmenuitem( hmenu, IMGED_TOOLBAR, MF_CHECKED, FALSE );
        _wpi_showwindow( htoolbar, SW_SHOWNA );
        _wpi_setfocus( HMainWindow );
        ImgedConfigInfo.show_state |= SET_SHOW_TOOL;
    }

} /* CheckToolbarItem */

/*
 * CloseToolBar - call the clean up routine
 */
void CloseToolBar( void )
{
    ToolBarFini( toolBar );

} /* CloseToolBar */

/*
 * AddHotSpotTool - add the hot spot button to the toolbar if necessary
 */
void AddHotSpotTool( BOOL faddhotspot )
{
    TOOLITEMINFO        tii;
    HMENU               hmenu;

    if( faddhotspot ) {
        if( hotspotPresent ) {
            return;
        }
        tii.u.bmp = bitmaps[9].hToolBmp;
        tii.id = IMGED_HOTSPOT;
        tii.flags = ITEM_DOWNBMP | ITEM_STICKY;
        tii.depressed = bitmaps[9].hToolDep;
        ToolBarAddItem( toolBar, &tii );
        hotspotPresent = TRUE;
        if( HMainWindow != NULL ) {
            hmenu = _wpi_getmenu( _wpi_getframe( HMainWindow ) );
            _wpi_enablemenuitem( hmenu, IMGED_HOTSPOT, TRUE, FALSE );
        }
    } else {
        if( !hotspotPresent ) {
            return;
        }
        ToolBarDeleteItem( toolBar, IMGED_HOTSPOT );
        if( currentTool == IMGED_HOTSPOT ) {
            currentTool = IMGED_FREEHAND;
            SetToolType( (int)currentTool );
            ToolBarSetState( toolBar, currentTool, BUTTON_DOWN );
        }
        hotspotPresent = FALSE;
        if( HMainWindow != NULL ) {
            hmenu = _wpi_getmenu( _wpi_getframe( HMainWindow ) );
            _wpi_enablemenuitem( hmenu, IMGED_HOTSPOT, FALSE, FALSE );
        }
    }
    UpdateToolBar( toolBar );

} /* AddHotSpotTool */

/*
 * PushToolButton - push a button on the toolbar
 */
void PushToolButton( WORD cmdid )
{
    if( cmdid != currentTool ) {
        ToolBarSetState( toolBar, currentTool, BUTTON_UP );
    }
    ToolBarSetState( toolBar, cmdid, BUTTON_DOWN );
    currentTool = cmdid;

} /* PushToolButton */
