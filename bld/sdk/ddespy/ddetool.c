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


#include "wddespy.h"

typedef struct ddetoolinfo {
    struct toolbar      *hdl;
    TOOLDISPLAYINFO     info;
    BOOL                fixed;
    RECT                floatrect;
    HBITMAP             *bitmaps;
}DDEToolBarInfo;

typedef struct {
    char        *name;
    WORD        id;
    WORD        flags;
}DDEButtonInfo;

#define BUTTON_CNT              12

static DDEButtonInfo ButInfo[ BUTTON_CNT ] = {
            "STRING",   DDEMENU_TRK_STR,        ITEM_STICKY,
            "CONV",     DDEMENU_TRK_CONV,       ITEM_STICKY,
            "LINK",     DDEMENU_TRK_LINK,       ITEM_STICKY,
            "SERVER",   DDEMENU_TRK_SERVER,     ITEM_STICKY,
            NULL,       0,                      ITEM_BLANK,
            "LOG",      DDEMENU_LOG_FILE,       0,
            "MARK",     DDEMENU_MARK,           0,
            "ERASE",    DDEMENU_CLEAR,          0,
            NULL,       0,                      ITEM_BLANK,
            "HWNDAKA",  DDEMENU_HWND_ALIAS,     0,
            "TASKAKA",  DDEMENU_TASK_ALIAS,     0,
            "CONVAKA",  DDEMENU_CONV_ALIAS,     0
};

static DDEToolBarInfo   ToolBar;

#define TOOL_BUTTON_WIDTH       ( 23 + 4 )
#define TOOL_BUTTON_HITE        ( 19 + 4 )
#define TOOL_SPACE              ( TOOL_BUTTON_WIDTH / 2 )
#define TOOL_OUTLINE_WIDTH      ( TOOL_BUTTON_WIDTH / 10 )
#define TOOL_OUTLINE_HITE       ( TOOL_BUTTON_HITE / 10 )
#define TOOLBAR_HITE            ( TOOL_BUTTON_HITE + \
                                  2 * TOOL_OUTLINE_HITE + 3 )
#define TOOLBAR_MIN_WIDTH       ( TOOL_BUTTON_WIDTH + \
                                  2 * TOOL_OUTLINE_WIDTH + 3 )


/*
 * ResizeForTB - Make room in the main window for the tool bar or
 *               remove the space when the toolbar is floating
 *      For fixed toolbar rect is the bounding rectangle of the bar
 *      otherwise rect should be NULL
 */

static void ResizeForTB( RECT *area, HWND hwnd ) {

    DDEWndInfo          *info;
    RECT                winsize;

    info = (DDEWndInfo *)GetWindowLong( hwnd, 0 );
    GetClientRect( hwnd, &winsize );
    if( area == NULL ) {
        info->list.ypos = 0;
    } else {
        info->list.ypos = area->bottom - area->top + 1;
    }
    ResizeListBox( winsize.right - winsize.left,
                   winsize.bottom - winsize.top, &(info->list ) );
}

/*
 * MyToolBarProc - hook message handler for the tool bar.
 */

BOOL MyToolBarProc( HWND hwnd, unsigned msg, UINT wparam, LONG lparam )
{
    MINMAXINFO  *minmax;
//    HWND      toolhwnd;
    HMENU       mh;

    wparam=wparam;
    switch( msg ) {
    case WM_CREATE:
        mh = GetMenu( DDEMainWnd);
        CheckMenuItem( mh, DDEMENU_TOOLBAR, MF_BYCOMMAND | MF_CHECKED );
        ConfigInfo.show_tb = TRUE;
        break;
#if(0)
    case WM_LBUTTONDBLCLK:
        /* flip the current state of the toolbar -
         * if we are fixed then start to float or vice versa
         */
        ToolBar.fixed = !ToolBar.fixed;
        if( ToolBar.fixed ) {
            GetFixedTBRect( DDEMainWnd, &ToolBar.info.area );
            ToolBar.info.style = TOOLBAR_FIXED_STYLE;
            ToolBarDisplay( ToolBar.hdl, &ToolBar.info );
            ResizeForTB( &ToolBar.info.area, DDEMainWnd );
        } else {
            ToolBar.info.area = ToolBar.floatrect;
            ToolBar.info.style = TOOLBAR_FLOAT_STYLE;
            ToolBarDisplay( ToolBar.hdl, &ToolBar.info );
            MoveWindow( hwnd, ToolBar.info.area.left, ToolBar.info.area.top,
                        ToolBar.info.area.right - ToolBar.info.area.left,
                        ToolBar.info.area.bottom - ToolBar.info.area.top,
                        TRUE );
            ResizeForTB( NULL, DDEMainWnd );
        }
        toolhwnd = ToolBarWindow( ToolBar.hdl );
        ShowWindow( toolhwnd, SW_NORMAL );
        UpdateWindow( toolhwnd );
        return( TRUE );
#endif
    case WM_MOVE:
    case WM_SIZE:
        // Whenever we are moved or sized as a floating toolbar, we remember our position
        // so that we can restore it when dbl. clicked
        if( !ToolBar.fixed ) {
            GetWindowRect( hwnd, &( ToolBar.floatrect ) );
        }
        break;
    case WM_GETMINMAXINFO:
        minmax = (MINMAXINFO *)lparam;
        minmax->ptMinTrackSize.x = TOOLBAR_MIN_WIDTH +
                                    3 * GetSystemMetrics( SM_CXFRAME );
        break;
    case WM_DESTROY:
        if( IsWindowVisible( DDEMainWnd ) ) {
            ResizeForTB( NULL, DDEMainWnd );
        }
        mh = GetMenu( DDEMainWnd);
        CheckMenuItem( mh, DDEMENU_TOOLBAR, MF_BYCOMMAND | MF_UNCHECKED );
        ConfigInfo.show_tb = FALSE;
        break;
    }
    return( FALSE );
}

void GetFixedTBRect( HWND hwnd, RECT *rect ) {

    HDC         dc;
    POINT       org;

    dc = GetDC( hwnd );
    GetWindowOrgEx( dc, &org );
    ReleaseDC( hwnd, dc );
    GetClientRect( hwnd, rect );
    rect->top += org.y;
    rect->bottom = rect->top + TOOLBAR_HITE;
}


/*
 * showTBHint
 */
static void showTBHint( HWND hwnd, UINT menuid, BOOL select ) {

    DDEWndInfo          *info;

    hwnd = hwnd;
    info = (DDEWndInfo *)GetWindowLong( DDEMainWnd, 0 );
    HintToolBar( info->hintbar, menuid, select );
}

/*
 * MakeDDEToolBar - create the tool bar
 */
void MakeDDEToolBar( HWND hwnd ) {

    HWND                toolhwnd;
    TOOLITEMINFO        item;
    WORD                i;
    BOOL                show;

    show = ConfigInfo.show_tb;
    ToolBar.info.style = TOOLBAR_FIXED_STYLE;
    ToolBar.info.button_size.x = TOOL_BUTTON_WIDTH;
    ToolBar.info.button_size.y = TOOL_BUTTON_HITE;
    ToolBar.info.border_size.x = TOOL_OUTLINE_WIDTH;
    ToolBar.info.border_size.y = TOOL_OUTLINE_HITE;
    GetFixedTBRect( hwnd, &ToolBar.info.area );
    ToolBar.info.hook = MyToolBarProc;
    ToolBar.info.helphook = showTBHint;
    ToolBar.info.background = 0;
    ToolBar.info.foreground = 0;
    ResizeForTB( &ToolBar.info.area, hwnd );
    ToolBar.fixed = TRUE;
    ToolBar.floatrect = ToolBar.info.area;
    ToolBar.floatrect.bottom += GetSystemMetrics( SM_CYCAPTION ) +
                                3 * GetSystemMetrics( SM_CYFRAME );

    ToolBar.hdl = ToolBarInit( hwnd );
    ToolBarDisplay( ToolBar.hdl, &ToolBar.info );

    ToolBar.bitmaps = MemAlloc( BUTTON_CNT * sizeof( HBITMAP ) );
    for( i=0; i < BUTTON_CNT; i++ ) {
        if( ButInfo[i].flags & ITEM_BLANK ) {
            ToolBar.bitmaps[i] = NULL;
            item.blank_space = TOOL_SPACE;
            item.id = 0;
            item.depressed = NULL;
        } else {
            ToolBar.bitmaps[i] = LoadBitmap( Instance, ButInfo[i].name );
            item.bmp = ToolBar.bitmaps[i];
            item.id = ButInfo[i].id;
            item.depressed = ToolBar.bitmaps[i];
        }
        item.flags = ButInfo[i].flags;
        ToolBarAddItem( ToolBar.hdl, &item );
    }
//    ToolBarDisplay( ToolBar.hdl, &ToolBar.info );
    toolhwnd = ToolBarWindow( ToolBar.hdl );
    if( show ) {
        ShowWindow( toolhwnd, SW_NORMAL );
        UpdateWindow( toolhwnd );
    } else {
        DestroyWindow( toolhwnd );
        ResizeForTB( NULL, DDEMainWnd );
    }
}

/*
 * DDEToolBarFini - destroy the tool bar and free memory associated with it
 */
void DDEToolBarFini() {

    unsigned    i;

    ToolBarFini( ToolBar.hdl );
    for( i=0; i < BUTTON_CNT; i++ ) {
        if( ToolBar.bitmaps[i] != NULL ) {
            DeleteObject( ToolBar.bitmaps[i] );
        }
    }
    MemFree( ToolBar.bitmaps );
}

/*
 * ResizeTB - if the tool bar is fixed resize it when its 'owner'
 *              window is resized
 */

void ResizeTB( HWND owner ) {

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
}

/*
 * ToggleTB - toggle the tool bar between shown and destroyed states
 */

BOOL ToggleTB( HWND parent ) {

    HWND        hwnd;

    hwnd = ToolBarWindow( ToolBar.hdl );
    if( IsWindow( hwnd ) ) {
        DestroyWindow( hwnd );
        return( TRUE );
    } else {
        if( ToolBar.fixed ) {
            ToolBar.info.style = TOOLBAR_FIXED_STYLE;
            GetFixedTBRect( parent, &ToolBar.info.area );
            ResizeForTB( &ToolBar.info.area, parent );
        } else {
            ToolBar.info.style = TOOLBAR_FLOAT_STYLE;
            ToolBar.info.area = ToolBar.floatrect;
        }
        ToolBarDisplay( ToolBar.hdl, &ToolBar.info );
        hwnd = ToolBarWindow( ToolBar.hdl );
        MoveWindow( hwnd, ToolBar.info.area.left, ToolBar.info.area.top,
                    ToolBar.info.area.right - ToolBar.info.area.left,
                    ToolBar.info.area.bottom - ToolBar.info.area.top, TRUE );
        ShowWindow( hwnd, SW_NORMAL );
        UpdateWindow( hwnd );
        return( FALSE );
    }
}

/*
 * DDESetStickyState - set the state of a sticky tool bar button
 */

void DDESetStickyState( WORD id, BOOL isdown ) {

    if( isdown ) {
        ToolBarSetState( ToolBar.hdl, id, BUTTON_DOWN );
    } else {
        ToolBarSetState( ToolBar.hdl, id, BUTTON_UP );
    }
}
