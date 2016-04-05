/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DDE Spy toolbar.
*
****************************************************************************/


#include "commonui.h"
#include "wddespy.h"

typedef struct ddetoolinfo {
    toolbar             *hdl;
    TOOLDISPLAYINFO     info;
    BOOL                fixed;
    RECT                floatrect;
    HBITMAP             *bitmaps;
} DDEToolBarInfo;

typedef struct {
    char        *name;
    WORD        id;
    WORD        flags;
    int         tip_id;
} DDEButtonInfo;

#define BUTTON_CNT              12

static DDEButtonInfo ButInfo[BUTTON_CNT] = {
    "STRING",   DDEMENU_TRK_STR,        ITEM_STICKY,    STR_TIP_TRK_STR,
    "CONV",     DDEMENU_TRK_CONV,       ITEM_STICKY,    STR_TIP_TRK_CONV,
    "LINK",     DDEMENU_TRK_LINK,       ITEM_STICKY,    STR_TIP_TRK_LINK,
    "SERVER",   DDEMENU_TRK_SERVER,     ITEM_STICKY,    STR_TIP_TRK_SERVER,
    NULL,       0,                      ITEM_BLANK,     -1,
    "LOG",      DDEMENU_LOG_FILE,       0,              STR_TIP_LOG_FILE,
    "MARK",     DDEMENU_MARK,           0,              STR_TIP_MARK,
    "ERASE",    DDEMENU_CLEAR,          0,              STR_TIP_CLEAR,
    NULL,       0,                      ITEM_BLANK,     -1,
    "HWNDAKA",  DDEMENU_HWND_ALIAS,     0,              STR_TIP_HWND_ALIAS,
    "TASKAKA",  DDEMENU_TASK_ALIAS,     0,              STR_TIP_TASK_ALIAS,
    "CONVAKA",  DDEMENU_CONV_ALIAS,     0,              STR_TIP_CONV_ALIAS
};

static DDEToolBarInfo   ToolBar;

#define TOOL_BUTTON_WIDTH       (23 + 4)
#define TOOL_BUTTON_HITE        (19 + 4)
#define TOOL_SPACE              (TOOL_BUTTON_WIDTH / 2)
#define TOOL_OUTLINE_WIDTH      (TOOL_BUTTON_WIDTH / 10)
#define TOOL_OUTLINE_HITE       (TOOL_BUTTON_HITE / 10)
#define TOOLBAR_HITE            (TOOL_BUTTON_HITE + 2 * TOOL_OUTLINE_HITE + 3)
#define TOOLBAR_MIN_WIDTH       (TOOL_BUTTON_WIDTH + 2 * TOOL_OUTLINE_WIDTH + 3)


/*
 * resizeForTB - make room in the main window for the toolbar or
 *               remove the space when the toolbar is floating
 *             - for fixed toolbar rect is the bounding rectangle of the bar
 *             - otherwise rect should be NULL
 */
static void resizeForTB( RECT *area, HWND hwnd )
{
    DDEWndInfo          *info;
    RECT                winsize;

    info = (DDEWndInfo *)GET_WNDINFO( hwnd );
    GetClientRect( hwnd, &winsize );
    if( area == NULL ) {
        info->list.ypos = 0;
    } else {
        info->list.ypos = area->bottom - area->top;
    }
    ResizeListBox( winsize.right - winsize.left,
                   winsize.bottom - winsize.top, &info->list );

} /* resizeForTB */

/*
 * myToolBarProc - hook message handler for the toolbar
 */
static bool myToolBarProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    MINMAXINFO  *minmax;
//  HWND        toolhwnd;
    HMENU       mh;

    wparam = wparam;
    switch( msg ) {
    case WM_CREATE:
        mh = GetMenu( DDEMainWnd );
        CheckMenuItem( mh, DDEMENU_TOOLBAR, MF_BYCOMMAND | MF_CHECKED );
        ConfigInfo.show_tb = true;
        break;
#if 0
    case WM_LBUTTONDBLCLK:
        /*
         * Flip the current state of the toolbar.
         * If we are fixed then start to float or vice versa.
         */
        ToolBar.fixed = !ToolBar.fixed;
        if( ToolBar.fixed ) {
            GetFixedTBRect( DDEMainWnd, &ToolBar.info.area );
            ToolBar.info.style = TOOLBAR_FIXED_STYLE;
            ToolBarDisplay( ToolBar.hdl, &ToolBar.info );
            resizeForTB( &ToolBar.info.area, DDEMainWnd );
        } else {
            ToolBar.info.area = ToolBar.floatrect;
            ToolBar.info.style = TOOLBAR_FLOAT_STYLE;
            ToolBarDisplay( ToolBar.hdl, &ToolBar.info );
            MoveWindow( hwnd, ToolBar.info.area.left, ToolBar.info.area.top,
                        ToolBar.info.area.right - ToolBar.info.area.left,
                        ToolBar.info.area.bottom - ToolBar.info.area.top,
                        TRUE );
            resizeForTB( NULL, DDEMainWnd );
        }
        toolhwnd = ToolBarWindow( ToolBar.hdl );
        ShowWindow( toolhwnd, SW_NORMAL );
        UpdateWindow( toolhwnd );
        return( true );
#endif
    case WM_MOVE:
    case WM_SIZE:
        /*
         * Whenever we are moved or sized as a floating toolbar, we remember our position
         * so that we can restore it when double clicked.
         */
        if( !ToolBar.fixed ) {
            GetWindowRect( hwnd, &ToolBar.floatrect );
        }
        break;
    case WM_GETMINMAXINFO:
        minmax = (MINMAXINFO *)lparam;
        minmax->ptMinTrackSize.x = TOOLBAR_MIN_WIDTH + 3 * GetSystemMetrics( SM_CXFRAME );
        break;
    case WM_DESTROY:
        if( IsWindowVisible( DDEMainWnd ) ) {
            resizeForTB( NULL, DDEMainWnd );
        }
        mh = GetMenu( DDEMainWnd );
        CheckMenuItem( mh, DDEMENU_TOOLBAR, MF_BYCOMMAND | MF_UNCHECKED );
        ConfigInfo.show_tb = false;
        break;
    }
    return( false );

} /* myToolBarProc */

/*
 * GetFixedTBRect
 */
void GetFixedTBRect( HWND hwnd, RECT *rect )
{
    HDC         dc;
    POINT       org;

    dc = GetDC( hwnd );
    GetWindowOrgEx( dc, &org );
    ReleaseDC( hwnd, dc );
    GetClientRect( hwnd, rect );
    rect->top += org.y;
    rect->bottom = rect->top + TOOLBAR_HITE;

} /* GetFixedTBRect */


/*
 * showTBHint
 */
static void showTBHint( HWND hwnd, ctl_id id, bool select )
{
    DDEWndInfo          *info;

    hwnd = hwnd;
    info = (DDEWndInfo *)GET_WNDINFO( DDEMainWnd );
    HintToolBar( info->hintbar, id, select );

} /* showTBHint */

/*
 * MakeDDEToolBar - create the toolbar
 */
void MakeDDEToolBar( HWND hwnd )
{
    HWND                toolhwnd;
    TOOLITEMINFO        item;
    WORD                i;
    bool                show;

    show = ConfigInfo.show_tb;
    ToolBar.info.style = TOOLBAR_FIXED_STYLE;
    ToolBar.info.button_size.x = TOOL_BUTTON_WIDTH;
    ToolBar.info.button_size.y = TOOL_BUTTON_HITE;
    ToolBar.info.border_size.x = TOOL_OUTLINE_WIDTH;
    ToolBar.info.border_size.y = TOOL_OUTLINE_HITE;
    GetFixedTBRect( hwnd, &ToolBar.info.area );
    ToolBar.info.hook = myToolBarProc;
    ToolBar.info.helphook = showTBHint;
    ToolBar.info.background = 0;
    ToolBar.info.foreground = 0;
    ToolBar.info.is_fixed = true;
    ToolBar.info.use_tips = true;
    ToolBar.fixed = true;
    ToolBar.floatrect = ToolBar.info.area;
    ToolBar.floatrect.bottom += GetSystemMetrics( SM_CYCAPTION ) +
                                3 * GetSystemMetrics( SM_CYFRAME );

    ToolBar.hdl = ToolBarInit( hwnd );
    ToolBarDisplay( ToolBar.hdl, &ToolBar.info );
    resizeForTB( &ToolBar.info.area, hwnd );

    ToolBar.bitmaps = MemAlloc( BUTTON_CNT * sizeof( HBITMAP ) );
    for( i=0; i < BUTTON_CNT; i++ ) {
        if( ButInfo[i].flags & ITEM_BLANK ) {
            ToolBar.bitmaps[i] = NULL;
            item.u.blank_space = TOOL_SPACE;
            item.id = 0;
            item.depressed = NULL;
        } else {
            ToolBar.bitmaps[i] = LoadBitmap( Instance, ButInfo[i].name );
            item.u.bmp = ToolBar.bitmaps[i];
            item.id = ButInfo[i].id;
            item.depressed = ToolBar.bitmaps[i];
        }
        if( ButInfo[i].tip_id >= 0 ) {
            LoadString( Instance, ButInfo[i].tip_id, item.tip, MAX_TIP );
        } else {
            item.tip[0] = '\0';
        }
        item.flags = ButInfo[i].flags;
        ToolBarAddItem( ToolBar.hdl, &item );
    }
//  ToolBarDisplay( ToolBar.hdl, &ToolBar.info );
    toolhwnd = ToolBarWindow( ToolBar.hdl );
    if( show ) {
        ShowWindow( toolhwnd, SW_NORMAL );
        UpdateWindow( toolhwnd );
    } else {
        DestroyWindow( toolhwnd );
        resizeForTB( NULL, DDEMainWnd );
    }

} /* MakeDDEToolBar */

/*
 * DDEToolBarFini - destroy the toolbar and free memory associated with it
 */
void DDEToolBarFini()
{
    unsigned    i;

    ToolBarFini( ToolBar.hdl );
    for( i = 0; i < BUTTON_CNT; i++ ) {
        if( ToolBar.bitmaps[i] != NULL ) {
            DeleteObject( ToolBar.bitmaps[i] );
        }
    }
    MemFree( ToolBar.bitmaps );

} /* DDEToolBarFini */

/*
 * ResizeTB - if the toolbar is fixed resize it when its 'owner'
 *            window is resized
 */
void ResizeTB( HWND owner )
{
    RECT        area;
    HWND        toolhwnd;

    if( ToolBar.fixed ) {
        GetFixedTBRect( owner, &area );
        toolhwnd = ToolBarWindow( ToolBar.hdl );
        if( IsWindow( toolhwnd ) ) {
            MoveWindow( toolhwnd, area.left, area.top, area.right - area.left,
                        area.bottom - area.top, TRUE );
        }
    }

} /* ResizeTB */

/*
 * ToggleTB - toggle the tool bar between shown and destroyed states
 */
bool ToggleTB( HWND parent )
{
    HWND        hwnd;

    hwnd = ToolBarWindow( ToolBar.hdl );
    if( IsWindow( hwnd ) ) {
        DestroyWindow( hwnd );
        return( true );
    } else {
        if( ToolBar.fixed ) {
            ToolBar.info.style = TOOLBAR_FIXED_STYLE;
            GetFixedTBRect( parent, &ToolBar.info.area );
        } else {
            ToolBar.info.style = TOOLBAR_FLOAT_STYLE;
            ToolBar.info.area = ToolBar.floatrect;
        }
        ToolBarDisplay( ToolBar.hdl, &ToolBar.info );
        if( ToolBar.fixed ) {
            resizeForTB( &ToolBar.info.area, parent );
        }
        hwnd = ToolBarWindow( ToolBar.hdl );
        MoveWindow( hwnd, ToolBar.info.area.left, ToolBar.info.area.top,
                    ToolBar.info.area.right - ToolBar.info.area.left,
                    ToolBar.info.area.bottom - ToolBar.info.area.top, TRUE );
        ShowWindow( hwnd, SW_NORMAL );
        UpdateWindow( hwnd );
        return( false );
    }

} /* ToggleTB */

/*
 * DDESetStickyState - set the state of a sticky toolbar button
 */
void DDESetStickyState( ctl_id itemid, bool isdown )
{
    if( isdown ) {
        ToolBarSetState( ToolBar.hdl, itemid, BUTTON_DOWN );
    } else {
        ToolBarSetState( ToolBar.hdl, itemid, BUTTON_UP );
    }

} /* DDESetStickyState */
