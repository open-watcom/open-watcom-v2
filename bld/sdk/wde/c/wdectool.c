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



#include "wdeglbl.h"
#include <limits.h>
#include "wdeobjid.h"
#include "wdemain.h"
#include "wdeopts.h"
#include "wderes.h"
#include "wdestat.h"
#include "wdetest.h"
#include "wdehints.h"
#include "wdedebug.h"
#include "wdetoolb.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wdecust.h"
#include "wdefordr.h"
#include "wdecctl.h"
#include "wdectool.h"
#include "wde_rc.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define BUTTONX         22
#define BUTTONY         22
#define BUTTON_PAD      4
#define TOOL_BORDERX    1
#define TOOL_BORDERY    1
#define CONTROLS_DEFX   86
#define CONTROLS_DEFY   180
#define CONTROLS_INSET  100
#define CONTROLS_DEFXCC 96
#define CONTROLS_DEFYCC 280

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
bool WdeControlsHook( HWND, UINT, WPARAM, LPARAM );
void WdeCToolHelpHook( HWND hwnd, int id, bool pressed );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    char                *up;
    char                *down;
    WORD                id;
    OBJ_ID              obj_id;
    unsigned char       flags;
} WdeControlBit;

#define WCB_FLAG_COMMON_CONTROL 0x01

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void    WdeDestroyControls( void );
static WORD    WdeGetMenuFromOBJID( OBJ_ID );
static OBJ_ID  WdeGetOBJIDFromMenu( WORD );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WdeToolBarInfo *WdeControlsInfo      = NULL;
static WdeToolBar     *WdeControls          = NULL;
static bool            WdeStickyMode        = FALSE;

#define WCBFCC  WCB_FLAG_COMMON_CONTROL

static WdeControlBit WdeControlBits[] = {
    { "SelTool",  "SelToolD",  IDM_SELECT_MODE,    EDIT_SELECT,   0 },
    { "StikTool", "StikToolD", IDM_STICKY_TOOLS,   -1,            0 },
    { "DiagTool", "DiagToolD", IDM_DIALOG_TOOL,    DIALOG_OBJ,    0 },
    { "PushTool", "PushToolD", IDM_PBUTTON_TOOL,   PBUTTON_OBJ,   0 },
    { "RadTool",  "RadToolD",  IDM_RBUTTON_TOOL,   RBUTTON_OBJ,   0 },
    { "ChekTool", "ChekToolD", IDM_CBUTTON_TOOL,   CBUTTON_OBJ,   0 },
    { "TextTool", "TextToolD", IDM_TEXT_TOOL,      TEXT_OBJ,      0 },
    { "GrpTool",  "GrpToolD",  IDM_GBUTTON_TOOL,   GBUTTON_OBJ,   0 },
    { "FramTool", "FramToolD", IDM_FRAME_TOOL,     FRAME_OBJ,     0 },
    { "IconTool", "IconToolD", IDM_ICON_TOOL,      ICON_OBJ,      0 },
    { "EditTool", "EditToolD", IDM_EDIT_TOOL,      EDIT_OBJ,      0 },
    { "ListTool", "ListToolD", IDM_LISTBOX_TOOL,   LISTBOX_OBJ,   0 },
    { "CombTool", "CombToolD", IDM_COMBOBOX_TOOL,  COMBOBOX_OBJ,  0 },
    { "HScrTool", "HScrToolD", IDM_HSCROLL_TOOL,   HSCROLL_OBJ,   0 },
    { "VScrTool", "VScrToolD", IDM_VSCROLL_TOOL,   VSCROLL_OBJ,   0 },
    { "SBoxTool", "SBoxToolD", IDM_SIZEBOX_TOOL,   SIZEBOX_OBJ,   0 },
    { "SBTool",   "SBToolD",   IDM_STATUSBAR_TOOL, SBAR_OBJ,      WCBFCC },
    { "LVTool",   "LVToolD",   IDM_LISTVIEW_TOOL,  LVIEW_OBJ,     WCBFCC },
    { "TVTool",   "TVToolD",   IDM_TREEVIEW_TOOL,  TVIEW_OBJ,     WCBFCC },
    { "TCTool",   "TCToolD",   IDM_TABCNTL_TOOL,   TABCNTL_OBJ,   WCBFCC },
    { "AniTool",  "AniToolD",  IDM_ANIMATE_TOOL,   ANIMATE_OBJ,   WCBFCC },
    { "UDTool",   "UDToolD",   IDM_UPDOWN_TOOL,    UPDOWN_OBJ,    WCBFCC },
    { "TBTool",   "TBToolD",   IDM_TRACKBAR_TOOL,  TRACKBAR_OBJ,  WCBFCC },
    { "PGTool",   "PGToolD",   IDM_PROGRESS_TOOL,  PROGRESS_OBJ,  WCBFCC },
    { "HKTool",   "HKToolD",   IDM_HOTKEY_TOOL,    HOTKEY_OBJ,    WCBFCC },
    { "HdrTool",  "HdrToolD",  IDM_HEADER_TOOL,    HEADER_OBJ,    WCBFCC },
#ifndef __NT__
    { "Cst1Tool", "Cst1ToolD", IDM_CUSTOM1_TOOL,   CUSTCNTL1_OBJ, 0 },
    { "Cst2Tool", "Cst2ToolD", IDM_CUSTOM2_TOOL,   CUSTCNTL2_OBJ, 0 },
#endif
    { NULL,       NULL,        -1,                 -1,            0 }
};

#define NUM_TOOLS (sizeof( WdeControlBits ) / sizeof( WdeControlBit ) - 1)

WORD WdeGetCToolID( void )
{
    HMENU       menu;
    UINT        state;
    int         i;

    if( WdeControls == NULL ) {
        return( 0 );
    }

    menu = WdeGetResMenuHandle();

    for( i = 0; i < NUM_TOOLS; i++ ) {
        if( WdeControlBits[i].obj_id != -1 ) {
            state = GetMenuState( menu, WdeControlBits[i].id, MF_BYCOMMAND );
            if( state != -1 && (state & MF_CHECKED) ) {
                return( WdeControlBits[i].id );
            }
        }
    }

    return( 0xffff );
}

bool WdeInitControls( HINSTANCE inst )
{
    bool        usingCommonControls;
    int         i;

    WdeControlsInfo = WdeAllocToolBarInfo( NUM_TOOLS );

    if( WdeControlsInfo == NULL ) {
        return( false );
    }

    usingCommonControls = IsCommCtrlLoaded();

    for( i = 0; i < NUM_TOOLS; i++ ) {
        if( WdeControlBits[i].flags & WCB_FLAG_COMMON_CONTROL ) {
            if( !usingCommonControls ) {
                continue;
            }
        }
        WdeControlsInfo->items[i].u.bmp = LoadBitmap( inst, WdeControlBits[i].up );
        WdeControlsInfo->items[i].id = WdeControlBits[i].id;
        WdeControlsInfo->items[i].flags = ITEM_DOWNBMP | ITEM_STICKY;
        WdeControlsInfo->items[i].depressed = LoadBitmap( inst, WdeControlBits[i].down );
    }

    WdeControlsInfo->dinfo.button_size.x = BUTTONX + BUTTON_PAD;
    WdeControlsInfo->dinfo.button_size.y = BUTTONY + BUTTON_PAD;
    WdeControlsInfo->dinfo.border_size.x = TOOL_BORDERX;
    WdeControlsInfo->dinfo.border_size.y = TOOL_BORDERY;
    WdeControlsInfo->dinfo.style = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;
    WdeControlsInfo->dinfo.hook = WdeControlsHook;
    WdeControlsInfo->dinfo.helphook = WdeCToolHelpHook;
    WdeControlsInfo->dinfo.foreground = NULL;
    WdeControlsInfo->dinfo.background = LoadBitmap( inst, "WdeToolBk" );
    WdeControlsInfo->dinfo.is_fixed = FALSE;

    return( true );
}

void WdeShutdownControls( void )
{
    int i;

    WdeDestroyControls();

    if( WdeControlsInfo == NULL ) {
        return;
    }

    for( i = 0; i < NUM_TOOLS; i++ ) {
        if( WdeControlsInfo->items[i].u.bmp == WdeControlsInfo->items[i].depressed ) {
            WdeControlsInfo->items[i].depressed = (HBITMAP)NULL;
        }
        if( WdeControlsInfo->items[i].u.bmp != NULL ) {
            DeleteObject( WdeControlsInfo->items[i].u.bmp );
        }
        if( WdeControlsInfo->items[i].depressed != NULL ) {
            DeleteObject( WdeControlsInfo->items[i].depressed );
        }
    }

    WdeFreeToolBarInfo( WdeControlsInfo );
}

void WdeDestroyControls( void )
{
    if( WdeControls ) {
        WdeDestroyToolBar( WdeControls );
    }
}

void WdeToggleStickyTools( void )
{
    if( WdeGetNumRes() ) {
        WdeSetStickyMode( !WdeStickyMode );
    }
}

bool WdeGetStickyMode( void )
{
    return( WdeStickyMode );
}

bool WdeControlsToolBarExists( void )
{
    return( WdeControls != NULL );
}

bool WdeSetStickyMode( bool mode )
{
    HMENU   menu;
    bool    old_mode;

    if( !WdeGetNumRes() ) {
        return( false );
    }

    menu = WdeGetResMenuHandle();

    old_mode = WdeStickyMode;

    WdeStickyMode = mode;

    if( WdeStickyMode ) {
        WdeSetToolBarItemState( WdeControls, IDM_STICKY_TOOLS, BUTTON_DOWN );
        CheckMenuItem( menu, IDM_STICKY_TOOLS, MF_BYCOMMAND | MF_CHECKED );
    } else {
        WdeSetToolBarItemState( WdeControls, IDM_STICKY_TOOLS, BUTTON_UP );
        CheckMenuItem( menu, IDM_STICKY_TOOLS, MF_BYCOMMAND | MF_UNCHECKED );
    }

    WdeSetStatusText( NULL, "", TRUE );

    return( old_mode );
}

void WdeSetBaseObject( WORD menu_selection )
{
    HMENU               menu;
    OBJ_ID              obj_id;
    WORD                id;
    WdeToolBar          *tbar;

    if( !WdeGetNumRes() ) {
        return;
    }

    tbar = WdeControls;
    menu = WdeGetResMenuHandle();
    id = WdeGetCToolID();
    obj_id = -1;

    if( id != (WORD)-1 && id != menu_selection ) {
        CheckMenuItem( menu, id, MF_BYCOMMAND | MF_UNCHECKED );
        WdeSetToolBarItemState( tbar, id, BUTTON_UP );
    }

    if( menu_selection != (WORD)-1 ) {
        obj_id = WdeGetOBJIDFromMenu( menu_selection );
        if( obj_id != -1 ) {
            SetBaseObjType( obj_id );
            CheckMenuItem( menu, menu_selection, MF_BYCOMMAND | MF_CHECKED );
            WdeSetToolBarItemState( tbar, menu_selection, BUTTON_DOWN );
            WdeSetStatusText( NULL, "", TRUE );
        }
    }
}

WORD WdeGetMenuFromOBJID( OBJ_ID id )
{
    int i;

    for( i = 0; WdeControlBits[i].up != NULL; i++ ) {
        if( WdeControlBits[i].obj_id == id ) {
            return( WdeControlBits[i].id );
        }
    }

    return( -1 );
}

OBJ_ID WdeGetOBJIDFromMenu( WORD id )
{
    int i;

    for( i = 0; WdeControlBits[i].up != NULL; i++ ) {
        if( WdeControlBits[i].id == id ) {
            return( WdeControlBits[i].obj_id );
        }
    }

    return( -1 );
}

bool WdeCreateControlsToolBar( void )
{
    RECT        t, r, screen;
    HWND        parent;
    WORD        id;
    char        *text;

    if( WdeControls != NULL ) {
        return ( true );
    }

    WdeSetStatusText( NULL, "", FALSE );
    WdeSetStatusByID( WDE_CREATINGTOOLBOX, -1 );

    parent = WdeGetMainWindowHandle();

    WdeGetCntlTBarPosOption( &t );

    if( t.left == t.right && t.top == t.bottom ) {
        GetWindowRect( parent, &r );
#ifdef __NT__
        if( GetModuleHandle( "COMCTL32.DLL" ) == NULL ) {
#endif
            t.left = r.right - CONTROLS_DEFX;
            t.top = r.top + CONTROLS_INSET;
            t.right = r.right;
            t.bottom = t.top + CONTROLS_DEFY;
#ifdef __NT__
        } else {
            t.left = r.right - CONTROLS_DEFXCC;
            t.top = r.top + CONTROLS_INSET;
            t.right = r.right;
            t.bottom = t.top + CONTROLS_DEFYCC;
        }
#endif
    }

    GetWindowRect( GetDesktopWindow(), &screen );
    if( !IntersectRect( &r, &screen, &t ) ) {
        r.right = t.right - t.left;
        r.bottom = t.bottom - t.top;
        r.left = 0;
        if( screen.right > r.right )
            r.left = ( screen.right - r.right ) / 2;
        r.top = 0;
        if( screen.bottom - screen.top - r.bottom  > 0 )
            r.top = ( screen.bottom - screen.top - r.bottom ) / 2;
        r.right += r.left;
        r.bottom += r.top;
        t = r;
    }

    WdeControlsInfo->dinfo.area = t;

    WdeControls = WdeCreateToolBar( WdeControlsInfo, parent );

    if( WdeControls == NULL ) {
        return( false );
    }

    text = WdeAllocRCString( WDE_TOOLBOXCAPTION );
    SendMessage( WdeControls->win, WM_SETTEXT, 0, (LPARAM)(LPSTR)text );
    if( text != NULL ) {
        WdeFreeRCString( text );
    }

    text = WdeAllocRCString( WDE_HIDETOOLBOX );
    ModifyMenu ( WdeGetInitialMenuHandle(), IDM_SHOW_TOOLS,
                 MF_BYCOMMAND | MF_STRING, IDM_SHOW_TOOLS, text );
    ModifyMenu ( WdeGetResMenuHandle(), IDM_SHOW_TOOLS,
                 MF_BYCOMMAND | MF_STRING, IDM_SHOW_TOOLS, text );
    if( text != NULL ) {
        WdeFreeRCString( text );
    }

    if( WdeGetNumRes() != 0 ) {
        id = WdeGetMenuFromOBJID( GetBaseObjType() );
        if( id != (WORD)-1 ) {
            WdeSetToolBarItemState( WdeControls, id, BUTTON_DOWN );
        }
        WdeSetStickyMode( WdeStickyMode );
    }

    WdeSetStatusReadyText();

    return( true );
}

void WdeHandleShowToolsMenu( void )
{
    if( WdeControls != NULL ) {
        WdeDestroyControls();
        WdeSetOption( WdeOptIsCntlsTBarVisible, FALSE );
    } else {
        WdeCreateControlsToolBar();
        WdeSetOption( WdeOptIsCntlsTBarVisible, TRUE );
    }
}

void WdeCToolHelpHook( HWND hwnd, int id, bool pressed )
{
    _wde_touch( hwnd );
    WdeHandleToolHint( id, pressed );
}

bool WdeControlsHook( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    MINMAXINFO          *minmax;
    WdeToolBar          *tbar;
    OBJ_ID              obj_id;
    WORD                id;
    char                *text;
    OBJPTR              obj;
    WdeOrderMode        mode;
    CMDID               cid;
    bool                ignore_msg;
    bool                ret;

    ignore_msg = FALSE;

    if( WdeGetNumRes() != 0 && !WdeInCleanup() ) {
        obj = WdeGetCurrentDialog();
        if( obj && Forward( obj, GET_ORDER_MODE, &mode, NULL ) && mode != WdeSelect ) {
            ignore_msg = TRUE;
        }
    }

    if( !ignore_msg && WdeGetTestMode() ) {
        ignore_msg = TRUE;
    }

    if( ignore_msg ) {
        switch( msg ) {
        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONUP:
        case WM_LBUTTONUP:
            return( true );
        }
    }

    if( (tbar = WdeControls) == NULL ) {
        if( msg == WM_GETMINMAXINFO ) {
            minmax = (MINMAXINFO *)lParam;
            minmax->ptMinTrackSize.x = 8;
        }
        return( false );
    }

    ret = false;

    switch( msg ) {
    case WM_USER:
        if( WdeGetNumRes() ) {
            obj_id = GetBaseObjType();
            id = WdeGetMenuFromOBJID( obj_id );
            if( id == wParam || (wParam == IDM_STICKY_TOOLS && WdeStickyMode) ) {
                lParam = !lParam;
            }
            WdeHandleStickyToolPress( tbar, wParam, lParam );
        }
        break;

    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        if( FindToolIDAtPoint( tbar->tbar, wParam, lParam, &cid ) ) {
            if( cid == IDM_CUSTOM1_TOOL ) {
                ret = !WdeIsCurrentCustControlSet( 0 );
            } else if( cid == IDM_CUSTOM2_TOOL ) {
                ret = !WdeIsCurrentCustControlSet( 1 );
            }
        }
        break;

    case WM_SIZE:
        if ( wParam != SIZE_MAXIMIZED && wParam != SIZE_MINIMIZED ) {
            GetWindowRect( hwnd, &tbar->last_pos );
        }
        break;

    case WM_MOVE:
        if( !IsZoomed( hwnd ) ) {
            GetWindowRect( hwnd, &tbar->last_pos );
        }
        break;

    case WM_GETMINMAXINFO:
        if( GetModuleHandle( "COMCTL32.DLL" ) == NULL ) {
            minmax = (MINMAXINFO *)lParam;
            minmax->ptMinTrackSize.x =
                2 * GetSystemMetrics( SM_CXFRAME ) +
                tbar->info->dinfo.border_size.x +
                tbar->info->dinfo.button_size.x - 1;
            minmax->ptMinTrackSize.y =
                2 * GetSystemMetrics( SM_CYFRAME ) +
                GetSystemMetrics(SM_CYCAPTION) +
                tbar->info->dinfo.border_size.y +
                tbar->info->dinfo.button_size.y - 1;
            ret = true;
        }
        break;

    case WM_CLOSE:
        WdeDestroyControls();
        WdeSetOption ( WdeOptIsCntlsTBarVisible, FALSE );
        ret = true;
        break;

    case WM_DESTROY:
        WdeControls = NULL;
        WdeSetCntlTBarPosOption( &tbar->last_pos );
        text = WdeAllocRCString( WDE_SHOWTOOLBOX );
        ModifyMenu( WdeGetResMenuHandle(), IDM_SHOW_TOOLS,
                    MF_BYCOMMAND | MF_STRING, IDM_SHOW_TOOLS, text );
        ModifyMenu( WdeGetInitialMenuHandle(), IDM_SHOW_TOOLS,
                    MF_BYCOMMAND | MF_STRING, IDM_SHOW_TOOLS, text );
        if( text != NULL ) {
            WdeFreeRCString( text );
        }
        WdeCloseToolBar( tbar );
        break;

    }

    return( ret );
}
