/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Toolbar class for Windows and OS/2.
*
****************************************************************************/


#include "commonui.h"
#include <string.h>
#include <assert.h>
#include "bool.h"
#include "watcom.h"
#include "cguimem.h"
#include "toolbr.h"
#include "loadcc.h"
#include "wclbproc.h"
#ifdef __NT__
    #include <commctrl.h>
#endif


/* Window callback functions prototypes */
WINEXPORT WPI_MRESULT CALLBACK  ToolBarWndProc( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );
#ifdef __NT__
WINEXPORT LRESULT CALLBACK      WinToolWndProc( HWND, UINT, WPARAM, LPARAM );
WINEXPORT LRESULT CALLBACK      ToolContainerWndProc( HWND, UINT, WPARAM, LPARAM );
#endif
#ifdef __OS2__
WINEXPORT WPI_MRESULT CALLBACK  FrameProc( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );
#endif

#define HNULL                   0

#define BORDER_WIDTH( bar )     1

#define BLANK_SIZE( w ) ((w) / 3)

#define WPI_GET_WNDINFO( w )    ((toolbar *)_wpi_getwindowlongptr( w, 0 ))
#define WPI_SET_WNDINFO( w, d ) (_wpi_setwindowlongptr( w, 0, d ))

typedef int     tb_idx;

#ifdef __NT__
HBITMAP TB_CreateTransparentBitmap( HBITMAP, int, int );
#endif

static char     *className = "WTool";
#ifdef __NT__
static char     *containerClassName = "WToolContainer";
#endif
#ifdef __OS2_PM__
static bool     toolBarClassRegistered = false;
#endif

static bool         gdiObjectsCreated = false;
static HPEN         blackPen;
static HPEN         btnShadowPen;
static HPEN         btnHighlightPen;
static HPEN         btnFacePen;
static HBRUSH       blackBrush;
static HBRUSH       btnFaceBrush;
static COLORREF     btnColor;
static WPI_INST     appInst;
#ifdef __OS2_PM__
static WPI_WNDPROC  oldFrameProc;
#endif

static tool     *currTool;
static bool     currIsDown;
static ctl_id   lastID = NO_ID;
static bool     mouse_captured = false;
static bool     ignore_mousemove = false;   /* ReleaseCapture() generates
                                           a WM_MOUSEMOVE message */
#ifndef __OS2_PM__
static bool round_corners = true;       /* Platform has rounded buttons? */
#else
static bool round_corners = false;      /* Platform has rounded buttons? */
#endif

/*
 * findTool - find tool item based on id
 */
static tool *findTool( tool *list, ctl_id id )
{
    for( ; list != NULL; list = list->next ) {
        if( list->id == id ) {
            break;
        }
    }
    return( list );

} /* findTool */

/*
 * addTool - add an item to the toolbar list
 */
static void addTool( tool **list, tool *t )
{
    tool    *curr;
    tool    **last;

    last = list;
    for( curr = *list; curr != NULL; curr = curr->next ) {
        last = &curr->next;
    }
    *last = t;
    t->next = NULL;

} /* addTool */

/*
 * deleteTool - delete an item from the toolbar list
 */
static void deleteTool( tool **list, tool *t )
{
    tool    *curr, **last;

    last = list;
    for( curr = *list; curr != NULL; curr = curr->next ) {
        if( curr == t ) {
            *last = t->next;
            t->next = NULL;
            break;
        }
        last = &curr->next;
    }

} /* deleteTool */

/*
 * buttonPosition - get position of a button on the toolbar
 */
static bool buttonPosition( HWND hwnd, toolbar *bar, tool *top, WPI_POINT *p )
{
    WPI_RECT    rect;
    int         width;
    int         height;
    tool        *curr;
    WPI_POINT   pos;
#ifdef __OS2_PM__
    int         wndheight;
#endif

    _wpi_getclientrect( hwnd, &rect );
    width = _wpi_getwidthrect( rect ) - 2 * bar->border.x;
    height = _wpi_getheightrect( rect ) - 2 * bar->border.y;
#ifdef __OS2_PM__
    wndheight = _wpi_getheightrect( rect );
#endif
    curr = bar->tool_list;
    pos.y = 0;
    while( pos.y + bar->button_size.y <= height ) {
        pos.x = 0;
        while( pos.x + bar->button_size.x <= width ) {
            /*
             * We assert curr because top MUST be in the list - the only
             * way curr can be NULL is if top is NULL (bad) or not in the
             * list (also bad).
             */
            assert( curr != NULL );
            if( curr == top ) {
                p->x = pos.x + bar->border.x;
#ifndef __OS2_PM__
                p->y = pos.y + bar->border.y;
#else
                p->y = _wpi_cvth_y( pos.y, wndheight ) - bar->button_size.y -
                    bar->border.y + 1;
#endif
                return( true );
            }
            if( curr->flags & ITEM_BLANK ) {
                pos.x += curr->u.blank_space;
            } else {
                pos.x += bar->button_size.x - 1;
            }
            curr = curr->next;
        }
        pos.y += bar->button_size.y - 1;
    }
    return( false );

} /* buttonPosition */

/*
 * createButtonList - create all buttons on a tool bar
 */
static void createButtonList( HWND hwnd, toolbar *bar, tool *top )
{
    WPI_POINT   pos;

    /*
     * top must be an element in the tool_list hanging off of bar
     * we are going to create buttons for all the tools from top
     * to the end of the list.
     */
    for( ; top != NULL; top = top->next ) {
        if( !buttonPosition( hwnd, bar, top, &pos ) ) {
            /* No more buttons will fit. */
            break;
        }
        _wpi_setrectvalues( &top->area, pos.x, pos.y, pos.x + bar->button_size.x, pos.y + bar->button_size.y );
    }

} /* createButtonList */


#ifdef __NT__

/*
 * reinsertButtons - insert buttons once again when a toolbar that was destroyed is
 *                   recreated
 */
static void reinsertButtons( toolbar *bar )
{
    tool        *t;
    TBBUTTON    tbb;
    TBADDBITMAP tbab;
    BITMAP      bm;
    tb_idx      n;
    TOOLINFO    ti;
    if( IsCommCtrlLoaded() ) {
        for( t = bar->tool_list; t != NULL; t = t->next ) {
            if( !(t->flags & ITEM_BLANK) ) {
                GetObject( t->u.hbitmap, sizeof( BITMAP ), &bm );
                SendMessage( bar->hwnd, TB_SETBITMAPSIZE, 0, MAKELONG( bm.bmWidth, bm.bmHeight ) );
                tbab.hInst = NULL;
                tbab.nID = (UINT_PTR)TB_CreateTransparentBitmap( t->u.hbitmap, bm.bmWidth, bm.bmHeight );
                tbb.iBitmap = (int)SendMessage( bar->hwnd, TB_ADDBITMAP, 1, (LPARAM)&tbab );
                tbb.idCommand = t->id;
                tbb.fsState = TBSTATE_ENABLED;
                tbb.fsStyle = TBSTYLE_BUTTON;
                if( t->flags & ITEM_STICKY ) {
                    tbb.fsStyle |= TBSTYLE_CHECK;
                }
            } else {
                tbb.iBitmap = -1;
                tbb.idCommand = -1;
                tbb.fsState = 0;
                tbb.fsStyle = TBSTYLE_SEP;
            }
            tbb.iString = 0;
            SendMessage( bar->hwnd, TB_ADDBUTTONS, 1, (LPARAM)&tbb );
            if( bar->tooltips != NULL && !(t->flags & ITEM_BLANK) ) {
                n = (tb_idx)SendMessage( bar->hwnd, TB_BUTTONCOUNT, 0, 0L );
                SendMessage( bar->hwnd, TB_GETITEMRECT, n - 1, (LPARAM)&ti.rect );
                ti.cbSize = sizeof( TOOLINFO );
                ti.uFlags = 0;
                ti.hwnd = bar->hwnd;
                ti.uId = t->id;
                ti.hinst = GET_HINSTANCE( bar->owner );
                ti.lpszText = t->tip;
                SendMessage( bar->tooltips, TTM_ADDTOOL, 0, (LPARAM)&ti );
            }
        }
    }

} /* reinsertButtons */

#endif

/*
 * ToolBarRedrawButtons - redraw the toolbar buttons
 */
void ToolBarRedrawButtons( toolbar *bar )
{
    if( bar ) {
        createButtonList( bar->hwnd, bar, bar->tool_list );
    }

} /* ToolBarRedrawButtons */

/*
 * ToolBarInit - initialize the tool bar
 */
toolbar *ToolBarInit( HWND parent )
{
    COLORREF    clr_btnface = 0;
    COLORREF    clr_btnshadow = 0;
    COLORREF    clr_btnhighlight = 0;
    COLORREF    clr_black = 0;
    toolbar     *bar;
#ifndef __OS2_PM__
    /* Win16 and Win32 version of the initialization */
    WNDCLASS    wc;
    HANDLE      instance;

    instance = GET_HINSTANCE( parent );
    appInst = instance;

  #ifdef __NT__
    if( LoadCommCtrl() ) {
        if( !GetClassInfo( instance, containerClassName, &wc ) ) {
            wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
            wc.lpfnWndProc = GetWndProc( ToolContainerWndProc );
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hInstance = instance;
            wc.hIcon = NULL;
            wc.hCursor = LoadCursor( NULL, IDC_ARROW );
            wc.hbrBackground = (HBRUSH)(pointer_uint)(COLOR_3DFACE + 1);
            wc.lpszMenuName = NULL;
            wc.lpszClassName = containerClassName;
            RegisterClass( &wc );
        }
    } else {
  #endif
        if( !GetClassInfo( instance, className, &wc ) ) {
            wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
            wc.lpfnWndProc = GetWndProc( ToolBarWndProc );
            wc.lpszMenuName = NULL;
            wc.cbClsExtra = 0;
            wc.cbWndExtra = sizeof( LPVOID );
            wc.hInstance = instance;
            wc.hIcon = HNULL;
            wc.hCursor = LoadCursor( (HANDLE) HNULL, IDC_ARROW );
            wc.hbrBackground = WPI_NULL;
            wc.lpszMenuName = NULL;
            wc.lpszClassName = className;
            RegisterClass( &wc );
        }
        clr_btnshadow = GetSysColor( COLOR_BTNSHADOW );
        clr_btnhighlight = GetSysColor( COLOR_BTNHIGHLIGHT );
        clr_btnface = GetSysColor( COLOR_BTNFACE );
  #ifdef __NT__
        clr_black = GetSysColor( COLOR_BTNTEXT );
  #else
        clr_black = RGB( 0, 0, 0 );
  #endif
        btnColor = clr_btnface;
  #ifdef __NT__
    }
  #endif
#else
    /* OS/2 PM version of the initialization */
    int         rc;
    HAB         hab;

    hab = WinQueryAnchorBlock( parent );
    appInst.hab = hab;
    appInst.mod_handle = (HMODULE)0;

    if( !toolBarClassRegistered ) {
        rc = WinRegisterClass( hab, className, GetWndProc( ToolBarWndProc ),
                               CS_MOVENOTIFY | CS_SIZEREDRAW | CS_CLIPSIBLINGS,
                               sizeof( PVOID ) );
        toolBarClassRegistered = true;
    }
    clr_btnshadow = CLR_DARKGRAY;
    clr_btnhighlight = CLR_WHITE;
    clr_btnface = CLR_PALEGRAY;
    clr_black = CLR_BLACK;
    btnColor = CLR_PALEGRAY;
#endif

    bar = (toolbar *)MemAlloc( sizeof( toolbar ) );
    if ( bar ) {
        memset( bar, 0, sizeof( toolbar ) );
        bar->border_width = 1;
        bar->owner = parent;
    }

#ifdef __NT__
    if( !IsCommCtrlLoaded() ) {
#endif
        if( !gdiObjectsCreated ) {
            blackPen = _wpi_createpen( PS_SOLID, BORDER_WIDTH( bar ), clr_black );
            btnShadowPen = _wpi_createpen( PS_SOLID, BORDER_WIDTH( bar ), clr_btnshadow );
            btnHighlightPen = _wpi_createpen( PS_SOLID, BORDER_WIDTH( bar ), clr_btnhighlight );
            btnFacePen = _wpi_createpen( PS_SOLID, BORDER_WIDTH( bar ), clr_btnface );
            blackBrush = _wpi_createsolidbrush( clr_black );
            btnFaceBrush = _wpi_createsolidbrush( clr_btnface );
            gdiObjectsCreated = true;
        }
#ifdef __NT__
    }
#endif

    return( bar );

} /* ToolBarInit */

#ifndef __OS2_PM__

/*
 * ToolBarChangeSysColors - update the variables in which system colors are stored
 */
void ToolBarChangeSysColors( COLORREF tbFace, COLORREF tbHighlight, COLORREF tbShadow )
{
    tbFace = tbFace; tbHighlight = tbHighlight; tbShadow = tbShadow;
  #ifdef __NT__
    if( !IsCommCtrlLoaded() ) {
  #endif
        if( gdiObjectsCreated ) {
            DeleteObject( blackPen );
            DeleteObject( btnShadowPen );
            DeleteObject( btnHighlightPen );
            DeleteObject( btnFacePen );
            DeleteObject( btnFaceBrush );
        }

        blackPen = CreatePen( PS_SOLID, BORDER_WIDTH( bar ), GetSysColor( COLOR_BTNTEXT ) );
        btnShadowPen = CreatePen( PS_SOLID, BORDER_WIDTH( bar ), GetSysColor( COLOR_BTNSHADOW ) );
        btnHighlightPen = CreatePen( PS_SOLID, BORDER_WIDTH( bar ), GetSysColor( COLOR_BTNHIGHLIGHT ) );
        btnFacePen = CreatePen( PS_SOLID, BORDER_WIDTH( bar ), GetSysColor( COLOR_BTNFACE ) );
        btnColor = GetSysColor( COLOR_BTNFACE );
        btnFaceBrush = CreateSolidBrush( btnColor );
        gdiObjectsCreated = true;
  #ifdef __NT__
    }
  #endif

} /* ToolBarChangeSysColors */

#endif

/*
 * ToolBarDestroy - done with the toolbar
 */
void ToolBarDestroy ( toolbar *bar )
{
    tool    *curr;
    tool    *next;

    if( bar ) {
        if( bar->hwnd != HNULL ) {
            _wpi_destroywindow( _wpi_getframe( bar->hwnd ) );
        }
#ifdef __NT__
        if( bar->container != NULL ) {
            DestroyWindow( bar->container );
        }
#endif
        for( curr = bar->tool_list; curr != NULL; curr = next ) {
            next = curr->next;
            MemFree( curr );
        }
        if( bar->bgbrush != WPI_NULL ) {
            _wpi_deletebrush( bar->bgbrush );
        }
        MemFree( bar );
    }

} /* ToolBarDestroy */

/*
 * ToolBarFini - done with all tool bars
 */
void ToolBarFini( toolbar *bar )
{
    ToolBarDestroy( bar );

    if( gdiObjectsCreated ) {
        _wpi_deletepen( blackPen );
        _wpi_deletepen( btnShadowPen );
        _wpi_deletepen( btnHighlightPen );
        _wpi_deletepen( btnFacePen );
        _wpi_deletebrush( blackBrush );
        _wpi_deletebrush( btnFaceBrush );
        gdiObjectsCreated = false;
    }

} /* ToolBarFini */

/*
 * ToolBarAddItem - add a specific bitmap to the tool bar
 */
void ToolBarAddItem( toolbar *bar, TOOLITEMINFO *info )
{
    tool        *t;
#ifdef __NT__
    TBBUTTON    tbb;
    TBADDBITMAP tbab;
    BITMAP      bm;
    tb_idx      n;
    TOOLINFO    ti;
#endif

    t = (tool *)MemAlloc( sizeof( tool ) );
    if( info->flags & ITEM_BLANK ) {
        t->u.blank_space = info->u.blank_space;
    } else {
        t->u.hbitmap = info->u.hbitmap;
    }
    t->id = info->id;
    t->next = NULL;
    t->flags = info->flags;
    t->depressed = info->depressed;
    t->state = BUTTON_UP;
#ifdef __NT__
    strcpy( t->tip, info->tip );
#endif
    addTool( &bar->tool_list, t );
    if( !(info->flags & ITEM_BLANK) && info->u.hbitmap != HNULL ) {
        createButtonList( bar->hwnd, bar, t );
    }

#ifdef __NT__
    if( IsCommCtrlLoaded() ) {
        if( !(info->flags & ITEM_BLANK) ) {
            GetObject( info->u.hbitmap, sizeof( BITMAP ), &bm );
            SendMessage( bar->hwnd, TB_SETBITMAPSIZE, 0, MAKELONG( bm.bmWidth, bm.bmHeight ) );
            tbab.hInst = NULL;
            tbab.nID = (UINT_PTR)TB_CreateTransparentBitmap( info->u.hbitmap, bm.bmWidth, bm.bmHeight );
            tbb.iBitmap = (int)SendMessage( bar->hwnd, TB_ADDBITMAP, 1, (LPARAM)&tbab );
            tbb.idCommand = info->id;
            tbb.fsState = TBSTATE_ENABLED;
            tbb.fsStyle = TBSTYLE_BUTTON;
            if( info->flags & ITEM_STICKY ) {
                tbb.fsStyle |= TBSTYLE_CHECK;
            }
        } else {
            tbb.iBitmap = -1;
            tbb.idCommand = -1;
            tbb.fsState = 0;
            tbb.fsStyle = TBSTYLE_SEP;
        }
        tbb.iString = 0;
        SendMessage( bar->hwnd, TB_ADDBUTTONS, 1, (LPARAM)&tbb );
        if( bar->tooltips != NULL && !(info->flags & ITEM_BLANK) ) {
            n = (tb_idx)SendMessage( bar->hwnd, TB_BUTTONCOUNT, 0, 0L );
            SendMessage( bar->hwnd, TB_GETITEMRECT, n - 1, (LPARAM)&ti.rect );
            ti.cbSize = sizeof( TOOLINFO );
            ti.uFlags = 0;
            ti.hwnd = bar->hwnd;
            ti.uId = info->id;
            ti.hinst = GET_HINSTANCE( bar->owner );
            ti.lpszText = info->tip;
            SendMessage( bar->tooltips, TTM_ADDTOOL, 0, (LPARAM)&ti );
        }
    }
#endif

} /* ToolBarAddItem */

/*
 * ToolBarSetState - set the state of a toolbar button
 */
void ToolBarSetState( toolbar *bar, ctl_id id, WORD state )
{
    tool        *t;

#ifdef __NT__
    if( !IsCommCtrlLoaded() ) {
#endif
        t = findTool( bar->tool_list, id );
        t->state = state;

        /* Force the button to be redrawn. */
        _wpi_invalidaterect( bar->hwnd, &t->area, FALSE );
#ifdef __NT__
    } else {
        SendMessage( bar->hwnd, TB_CHECKBUTTON, (WPARAM)id, MAKELONG( state & BUTTON_DOWN, 0 ) );
    }
#endif

} /* ToolBarSetState */

/*
 * ToolBarGetState - get the state of a toolbar button
 */
WORD ToolBarGetState( toolbar *bar, ctl_id id )
{
    tool        *t;
#ifdef __NT__
    tb_idx      n;
    TBBUTTON    tbb;

    if( !IsCommCtrlLoaded() ) {
#endif
        t = findTool( bar->tool_list, id );
        return( t->state );
#ifdef __NT__
    } else {
        n = (tb_idx)SendMessage( bar->hwnd, TB_COMMANDTOINDEX, id, 0L );
        SendMessage( bar->hwnd, TB_GETBUTTON, n, (LPARAM)&tbb );
        return( (tbb.fsState & TBSTATE_CHECKED) ? BUTTON_DOWN : BUTTON_UP );
    }
#endif

} /* ToolBarGetState */

/*
 * ToolBarDeleteItem - delete an item from the tool bar
 */
bool ToolBarDeleteItem( toolbar *bar, ctl_id id )
{
    tool    *t;
    tool    *next;
#ifdef __NT__
    tb_idx  n;

    if( !IsCommCtrlLoaded() ) {
#endif
        t = findTool( bar->tool_list, id );
        if( t != NULL ) {
            /*
             * We need to destroy the window on the toolbar and recreate
             * all the other tools after this one.
             */
            next = t->next;
            deleteTool( &bar->tool_list, t );
            createButtonList( bar->hwnd, bar, next );
            return( true );
        }
#ifdef __NT__
    } else {
        n = (tb_idx)SendMessage( bar->hwnd, TB_COMMANDTOINDEX, id, 0L );
        SendMessage( bar->hwnd, TB_DELETEBUTTON, n, 0L );
        return( true );
    }
#endif

    return( false );

} /* ToolBarDeleteItem */

#ifdef __OS2_PM__

/*
 * FrameProc - frame procedure for use on OS/2 PM
 */
WPI_MRESULT CALLBACK FrameProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    HWND        client;

    switch( msg ) {
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDBLCLK:
        client = WinWindowFromID( hwnd, FID_CLIENT );
        if( client != NULLHANDLE ) {
            _wpi_sendmessage( client, msg, (WPI_PARAM1)0x0FFFFFFF, 0 );
        }
        break;
    case WM_SYSCOMMAND:
        if( SHORT1FROMMP( wparam ) != SC_CLOSE ) {
            client = WinWindowFromID( hwnd, FID_CLIENT );
            if( client != NULLHANDLE ) {
                _wpi_sendmessage( client, msg, wparam, lparam );
            }
            break;
        }
        /* fall through */
    case WM_CLOSE:
        client = WinWindowFromID( hwnd, FID_CLIENT );
        if( client != NULLHANDLE ) {
            _wpi_sendmessage( client, WM_CLOSE, 0, 0 );
        }
        _wpi_destroywindow( hwnd );
        return( NULL );
    }
    return( _wpi_callwindowproc( oldFrameProc, hwnd, msg, wparam, lparam ) );

} /* FrameProc */

#endif

/*
 * ToolBarDisplay - create and show the tool bar
 */
void ToolBarDisplay( toolbar *bar, TOOLDISPLAYINFO *disp )
{
    int     height;
    int     width;
#ifdef __OS2_PM__
    HWND    frame;
    HWND    parent;
#endif

    currTool = NULL;
    currIsDown = false;
    lastID = NO_ID;
    mouse_captured = false;

    if( bar->bgbrush != WPI_NULL ) {
        _wpi_deletebrush( bar->bgbrush );
        bar->bgbrush = WPI_NULL;
    }

    if( disp->background != WPI_NULL ) {
        bar->bgbrush = _wpi_createpatternbrush( disp->background );
    }

    if( bar->hwnd != HNULL ) {
        if( _wpi_getcapture() == bar->hwnd ) {
            _wpi_releasecapture();
        }
        _wpi_destroywindow( _wpi_getframe( bar->hwnd ) );
    }
    bar->button_size = disp->button_size;
    bar->hook = disp->hook;
    bar->helphook = disp->helphook;
    bar->border = disp->border_size;
    bar->background = disp->background;
    bar->foreground = disp->foreground;
    bar->is_fixed = disp->is_fixed;
    width = _wpi_getwidthrect( disp->area );
    height = _wpi_getheightrect( disp->area );

#ifndef __OS2_PM__
#ifdef __NT__
    if( IsCommCtrlLoaded() ) {
        if( !bar->is_fixed ) {
            bar->container = CreateWindowEx( WS_EX_TOOLWINDOW, containerClassName, NULL,
                                             WS_VISIBLE | WS_CAPTION | WS_SYSMENU |
                                             WS_THICKFRAME, disp->area.left,
                                             disp->area.top, width, height, bar->owner,
                                             NULL, GET_HINSTANCE( bar->owner ), bar );
            bar->hwnd = CreateWindow( TOOLBARCLASSNAME, NULL,
                                      WS_CHILD | WS_VISIBLE | TBSTYLE_WRAPABLE |
                                      TBSTYLE_FLAT, 0, 0, 0, 0, bar->container, NULL,
                                      GET_HINSTANCE( bar->owner ), NULL );
        } else {
            CREATESTRUCT cs;
            cs.dwExStyle = 0L;
            cs.lpszClass = TOOLBARCLASSNAME;
            cs.lpszName = NULL;
            cs.style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TBSTYLE_FLAT;
            cs.x = 0;
            cs.y = 0;
            cs.cx = 0;
            cs.cy = 0;
            cs.hwndParent = bar->owner;
            cs.hMenu = NULL;
            cs.hInstance = GET_HINSTANCE( bar->owner );
            cs.lpCreateParams = NULL;
            bar->container = NULL;
            bar->hwnd = CreateWindow( cs.lpszClass, cs.lpszName, cs.style, cs.x, cs.y,
                                      cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance,
                                      cs.lpCreateParams );
            GetWindowRect( bar->hwnd, &disp->area );
            ScreenToClient( bar->owner, (LPPOINT)&disp->area.left );
            ScreenToClient( bar->owner, (LPPOINT)&disp->area.right );
            if( bar->hook != NULL ) {
                /*
                 * The toolbar isn't subclassed until after the WM_CREATE message is sent,
                 * but the hook may be looking for this message, so fake it by calling the
                 * hook from here.
                 */
                bar->hook( bar->hwnd, WM_CREATE, 0, (LPARAM)&cs );
            }
        }
        bar->old_wndproc = (WNDPROC)GET_WNDPROC( bar->hwnd );
        SetProp( bar->hwnd, "bar", (LPVOID)bar );
        SET_WNDPROC( bar->hwnd, (LONG_PTR)GetWndProc( WinToolWndProc ) );
        SendMessage( bar->hwnd, TB_BUTTONSTRUCTSIZE, sizeof( TBBUTTON ), 0L );
        if( disp->use_tips ) {
            bar->tooltips = CreateWindow( TOOLTIPS_CLASS, NULL,
                                          WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                                          0, 0, 0, 0, bar->hwnd, NULL,
                                          GET_HINSTANCE( bar->owner ), NULL );
            SetWindowPos( bar->tooltips, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
            SendMessage( bar->hwnd, TB_SETTOOLTIPS, (WPARAM)bar->tooltips, 0L );
        } else {
            bar->tooltips = NULL;
        }
        reinsertButtons( bar );
  #ifndef _WIN64
    } else if( LOBYTE( LOWORD( GetVersion() ) ) >= 4 && !bar->is_fixed ) {
  #else
    } else if( !bar->is_fixed ) {
  #endif
        CreateWindowEx( WS_EX_TOOLWINDOW, className, NULL, disp->style,
                        disp->area.left, disp->area.top, width, height,
                        bar->owner, (HMENU) HNULL, GET_HINSTANCE( bar->owner ), bar );
    } else {
  #ifndef _WIN64
        if( LOBYTE( LOWORD( GetVersion() ) ) >= 4 ) {
  #endif
            CreateWindow( className, NULL, WS_CHILD | WS_CLIPSIBLINGS,
                          disp->area.left, disp->area.top, width, height,
                          bar->owner, (HMENU) HNULL, GET_HINSTANCE( bar->owner ), bar );
  #ifndef _WIN64
        } else {
            CreateWindow( className, NULL, disp->style, disp->area.left, disp->area.top,
                          width, height, bar->owner, (HMENU) HNULL,
                          GET_HINSTANCE( bar->owner ), bar );
        }
  #endif
    }
#else
    CreateWindow( className, NULL, disp->style, disp->area.left, disp->area.top, width,
                  height, bar->owner, (HMENU) HNULL, GET_HINSTANCE( bar->owner ), bar );
#endif
    /*
     * Windows ignores the GETMINMAXINFO before the WM_CREATE or
     * something so we kluge it.
     */
#ifdef __NT__
    if( !IsCommCtrlLoaded() ) {
#endif
        MoveWindow( bar->hwnd, disp->area.left, disp->area.top, width, height, TRUE );
#ifdef __NT__
    }
#endif
#else
    if( disp->is_fixed ) {
        parent = bar->owner;
    } else {
        parent = HWND_DESKTOP;
    }
    frame = WinCreateStdWindow( parent, 0L, &disp->style, NULL, "", 0L, (HMODULE)0, 0, NULL );

    WinSetOwner( frame, bar->owner );
    oldFrameProc = _wpi_subclasswindow( frame, FrameProc );

    WinSetPresParam( frame, PP_BACKGROUNDCOLORINDEX, (ULONG)sizeof( LONG ) + 1, (PVOID)&btnColor );
    WinCreateWindow( frame, className, "", WS_VISIBLE, 0, 0, 0, 0, frame, HWND_TOP, FID_CLIENT, (PVOID)bar, NULL );

    WinSetWindowPos( frame, HWND_TOP, disp->area.xLeft, disp->area.yBottom, width, height, SWP_MOVE | SWP_SIZE | SWP_HIDE );
#endif

} /* ToolBarDisplay */

/*
 * ToolBarWindow - return a handle to the toolbar window
 */
HWND ToolBarWindow( toolbar *bar )
{
    if( bar == NULL ) {
        return( HNULL );
    }
#ifdef __NT__
    if( bar->container != NULL ) {
        return( bar->container );
    }
#endif
    return( _wpi_getframe( bar->hwnd ) );

} /* ToolBarWindow */

/*
 * UpdateToolBar - update our tool bar
 */
void UpdateToolBar( toolbar *bar )
{
    _wpi_invalidaterect( bar->hwnd, NULL, TRUE );
    _wpi_updatewindow( bar->hwnd );

} /* UpdateToolBar */

/*
 * drawTopLeftCorner - draws the top left corner of a button
 */
static void drawTopLeftCorner( WPI_PRES pres, WPI_POINT size, int border, HPEN pen )
{
    HPEN        old_pen;
    WPI_POINT   pt;

    old_pen = _wpi_selectpen( pres, pen );
    _wpi_setpoint( &pt, border, size.y - 2 * border );
    _wpi_cvth_pt( &pt, size.y );
    _wpi_movetoex( pres, &pt, NULL );

    _wpi_setpoint( &pt, border, border );
    _wpi_cvth_pt( &pt, size.y );
    _wpi_lineto( pres, &pt );

    _wpi_setpoint( &pt, size.x - border, border );
    _wpi_cvth_pt( &pt, size.y );
    _wpi_lineto( pres, &pt );
    _wpi_getoldpen( pres, old_pen );

} /* drawTopLeftCorner */

/*
 * drawTopLeftInsideCorner - draws the top left corner of a button
 */
static void drawTopLeftInsideCorner( WPI_PRES pres, WPI_POINT size, int border, HPEN pen )
{
    HPEN        old_pen;
    WPI_POINT   pt;

    old_pen = _wpi_selectpen( pres, pen );
    _wpi_setpoint( &pt, border * 2, size.y - 2 * border );
    _wpi_cvth_pt( &pt, size.y );
    _wpi_movetoex( pres, &pt, NULL );

    _wpi_setpoint( &pt, border * 2, border * 2 );
    _wpi_cvth_pt( &pt, size.y );
    _wpi_lineto( pres, &pt );

    pt.x = size.x - border;
    _wpi_lineto( pres, &pt );
    _wpi_getoldpen( pres, old_pen );

} /* drawTopLeftCorner */

/*
 * drawBottomRightCorner - draws the bottom right corner of a button
 */
static void drawBottomRightCorner( WPI_PRES pres, WPI_POINT size, int border, HPEN pen )
{
    HPEN        old_pen;
    WPI_POINT   pt;
    int         height;

    height = size.y;
    size.x -= 2 * border;
    size.y -= 2 * border;
    old_pen = _wpi_selectpen( pres, pen );
    _wpi_setpoint( &pt, size.x, border );
    _wpi_cvth_pt( &pt, height );
    _wpi_movetoex( pres, &pt, NULL );

    _wpi_setpoint( &pt, size.x, size.y );
    _wpi_cvth_pt( &pt, height );
    _wpi_lineto( pres, &pt );

    _wpi_setpoint( &pt, border - 1, size.y );
    _wpi_cvth_pt( &pt, height );
    _wpi_lineto( pres, &pt );
    _wpi_getoldpen( pres, old_pen );

} /* drawBottomRightCorner */

/*
 * drawBottomRightInsideCorner - draw the inside corner of a button
 */
static void drawBottomRightInsideCorner( WPI_PRES pres, WPI_POINT size, int border, HPEN pen )
{
    HPEN        old_pen;
    WPI_POINT   pt;
    int         height;

    height = size.y;
    size.x -= 3 * border;
    size.y -= 3 * border;
    old_pen = _wpi_selectpen( pres, pen );
    _wpi_setpoint( &pt, 2 * border, size.y );
    _wpi_cvth_pt( &pt, height );
    _wpi_movetoex( pres, &pt, NULL );

    _wpi_setpoint( &pt, size.x, size.y );
    _wpi_cvth_pt( &pt, height );
    _wpi_lineto( pres, &pt );

    _wpi_setpoint( &pt, size.x, border );
    _wpi_cvth_pt( &pt, height );
    _wpi_lineto( pres, &pt );
    _wpi_getoldpen( pres, old_pen );

} /* drawBottomRightInsideCorner */

/*
 * drawBorder - draw to border of a button
 */
static void drawBorder( WPI_PRES pres, WPI_POINT size, int border )
{
    HPEN        old_pen;
    int         x;
    int         y;
    WPI_POINT   pt;

    y = _wpi_cvth_y( size.y - 1, size.y );
    x = size.x - 1;

    old_pen = _wpi_selectpen( pres, blackPen );
    _wpi_setpoint( &pt, 0, _wpi_cvth_y( 0, size.y ) );
    _wpi_movetoex( pres, &pt, NULL );

    pt.x = x;
    _wpi_lineto( pres, &pt );
    pt.y = y;
    _wpi_lineto( pres, &pt );
    pt.x = 0;
    _wpi_lineto( pres, &pt );
    pt.y = _wpi_cvth_y( 0, size.y );
    _wpi_lineto( pres, &pt );
    _wpi_getoldpen( pres, old_pen );

    old_pen = _wpi_selectpen( pres, btnFacePen );
    _wpi_setpoint( &pt, 0, _wpi_cvth_y( 0, size.y ) );
    _wpi_movetoex( pres, &pt, NULL );
    pt.x = border;
    if( !round_corners ) {
        pt.x -= 1;
    }
    _wpi_lineto( pres, &pt );
    pt.x = x;
    _wpi_movetoex( pres, &pt, NULL );
    pt.y = _wpi_cvth_y( border, size.y );
    if( !round_corners ) {
        pt.y += 1;
    }
    _wpi_lineto( pres, &pt );

    pt.y = y;
    _wpi_movetoex( pres, &pt, NULL );
    pt.x = x - border;
    if( !round_corners ) {
        pt.x += 1;
    }
    _wpi_lineto( pres, &pt );
    pt.x = 0;
    _wpi_movetoex( pres, &pt, NULL );
    pt.y = _wpi_cvth_y( _wpi_cvth_y( y, size.y ) - border, size.y );
    if( !round_corners ) {
        pt.y -= 1;
    }
    _wpi_lineto( pres, &pt );
    _wpi_getoldpen( pres, old_pen );

} /* drawBorder */

/*
 * toolBarDrawBitmap - draw the bitmap on a button
 */
static void toolBarDrawBitmap( WPI_PRES pres, WPI_POINT dst_size, WPI_POINT dst_org, WPI_HBITMAP hbitmap )
{
    WPI_BITMAP  bm;
    WPI_PRES    mempres;
    HDC         memdc;
    WPI_POINT   src_org;
    WPI_POINT   src_size;
    HBRUSH      old_brush;
    WPI_HBITMAP old_hbitmap;

    DPtoLP( pres, &dst_size, 1 );
    DPtoLP( pres, &dst_org, 1 );

    mempres = _wpi_createcompatiblepres( pres, appInst, &memdc );
    old_hbitmap = _wpi_selectbitmap( mempres, hbitmap );

    _wpi_getbitmapstruct( hbitmap, &bm );

    src_size.x = _wpi_bitmapwidth( &bm );
    src_size.y = _wpi_bitmapheight( &bm );
    DPtoLP( pres, &src_size, 1 );

    src_org.x = 0;
    src_org.y = 0;
    DPtoLP( pres, &src_org, 1 );

#if defined( __NT__ ) || defined( __WINDOWS__ )
    SetStretchBltMode( pres, COLORONCOLOR );
#else
    SetStretchBltMode( pres, STRETCH_DELETESCANS );
#endif

    /*
     * If it's a monochrome bitmap try and do the right thing - I can see
     * this pissing off some users, but oh well.
     */
    if( _wpi_bitmapbitcount( &bm ) == 1 && _wpi_bitmapplanes( &bm ) == 1 ) {
        old_brush = _wpi_selectbrush( pres, blackBrush );

        _wpi_stretchblt( pres, dst_org.x, dst_org.y, dst_size.x, dst_size.y, mempres,
                         src_org.x, src_org.y, src_size.x, src_size.y, 0xB8074A );

        _wpi_getoldbitmap( mempres, old_hbitmap );
        _wpi_getoldbrush( pres, old_brush );
    } else {
        _wpi_stretchblt( pres, dst_org.x, dst_org.y, dst_size.x, dst_size.y, mempres,
                         src_org.x, src_org.y, src_size.x, src_size.y, SRCCOPY );

        _wpi_getoldbitmap( mempres, old_hbitmap );
    }
    _wpi_deletecompatiblepres( mempres, memdc );

} /* toolBarDrawBitmap */

/*
 * drawButton - draw a button on the toolbar
 */
static void drawButton( HWND hwnd, tool *tool, bool down, WPI_PRES pres,
                        WPI_PRES mempres, HDC mem )
{
    toolbar     *bar;
    HBRUSH      brush;
    WPI_HBITMAP hbitmap;
    WPI_HBITMAP old_hbitmap;
    int         shift;
    bool        selected;
    WPI_POINT   dst_size;
    WPI_POINT   dst_org;
    WPI_HBITMAP used_hbitmap;
    WPI_RECTDIM left;
    WPI_RECTDIM right;
    WPI_RECTDIM top;
    WPI_RECTDIM bottom;
    bool        delete_pres;
    bool        delete_mempres;
#if defined( __NT__ ) || defined( __WINDOWS__ )
    HBITMAP     hbitmap2;
    HBITMAP     old_hbitmap2;
    HBITMAP     tmp_hbitmap;
    HDC         mem2;
    RECT        fill;
    COLORREF    cr;
#endif

    if( tool->flags & ITEM_BLANK ) {
        return;
    }

    /* draw the button */
    bar = WPI_GET_WNDINFO( hwnd );

    if( tool->flags & ITEM_STICKY ) {
        selected = ( tool->state == BUTTON_DOWN );
    } else {
        selected = down;
    }
    shift = 0;
    if( selected ) {
        shift = 2 * BORDER_WIDTH( bar );
    }

    delete_pres = false;
    delete_mempres = false;
    if( pres == NULLHANDLE ) {
        pres = _wpi_getpres( hwnd );
        mempres = NULLHANDLE;
        delete_pres = true;
    }

    if( mempres == NULLHANDLE || mem == NULLHANDLE ) {
        mempres = _wpi_createcompatiblepres( pres, appInst, &mem );
        delete_mempres = true;
    }
    hbitmap = _wpi_createcompatiblebitmap( pres, bar->button_size.x, bar->button_size.y );
    old_hbitmap = _wpi_selectbitmap( mempres, hbitmap );
#if defined( __NT__ ) || defined( __WINDOWS__ )
    mem2 = CreateCompatibleDC( pres );
    hbitmap2 = CreateCompatibleBitmap( pres, bar->button_size.x, bar->button_size.y );
    old_hbitmap2 = SelectObject( mem2, hbitmap2 );
#endif

    brush = btnFaceBrush;
    if( selected && bar->bgbrush != WPI_NULL ) {
        brush = bar->bgbrush;
    }
    _wpi_fillrect( mempres, &tool->area, btnColor, brush );

    dst_size = bar->button_size;
    dst_size.x -= 4 * BORDER_WIDTH( bar );
    dst_size.y -= 4 * BORDER_WIDTH( bar );
    dst_org.x = (1 * BORDER_WIDTH( bar )) + shift;
    dst_org.y = (1 * BORDER_WIDTH( bar )) + shift;
#ifdef __OS2_PM__
    dst_org.y = _wpi_cvth_y( dst_org.y, bar->button_size.y );
    dst_org.y = dst_org.y - dst_size.y + 1;
#endif
    used_hbitmap = tool->u.hbitmap;
    if( selected ) {
        /*
         * If the button is selected and it has the ITEM_DOWNBMP flag
         * then we draw the alternate bitmap instead.
         */
        if( tool->flags & ITEM_DOWNBMP ) {
            used_hbitmap = tool->depressed;
        }
    }
    toolBarDrawBitmap( mempres, dst_size, dst_org, used_hbitmap );

#if defined( __NT__ ) || defined( __WINDOWS__ )
    /* New, on Win32 platforms, use TB_TransparentBlt(). */
    /* Get background color of button bitmap. */
    tmp_hbitmap = SelectObject( mem2, used_hbitmap );
    /* The pixel (0, 0) must be in the background color. */
    cr = GetPixel( mem2, 0, 0 );
    tmp_hbitmap = SelectObject( mem2, tmp_hbitmap );
    /* IMPORTANT: must set required new background color for destination bitmap. */
    SetBkColor( mem2, GetSysColor( COLOR_BTNFACE ) );
    fill.top = 0;
    fill.left = 0;
    fill.right = bar->button_size.x;
    fill.bottom = bar->button_size.y;
    FillRect( mem2, &fill, brush );
    TB_TransparentBlt( mem2, dst_org.x, dst_org.y, dst_size.x, dst_size.y, mempres, cr );
    if( old_hbitmap != HNULL ) {
        SelectObject( mempres, old_hbitmap );
        DeleteObject( hbitmap );
    }
    if( delete_mempres ) {
        DeleteDC( mempres );
    }
    /* Switch new bitmap into variables expected by code below. */
    mempres = mem2;
    delete_mempres = true;
    hbitmap = hbitmap2;
    old_hbitmap = old_hbitmap2;
#endif

    drawBorder( mempres, bar->button_size, BORDER_WIDTH( bar ) );
    if( selected ) {
        drawTopLeftCorner( mempres, bar->button_size, BORDER_WIDTH( bar ), btnShadowPen );
        drawTopLeftInsideCorner( mempres, bar->button_size, BORDER_WIDTH( bar ), btnFacePen );
    } else {
        drawTopLeftCorner( mempres, bar->button_size, BORDER_WIDTH( bar ), btnHighlightPen );
        drawBottomRightCorner( mempres, bar->button_size, BORDER_WIDTH( bar ), btnShadowPen );
        drawBottomRightInsideCorner( mempres, bar->button_size, BORDER_WIDTH( bar ), btnShadowPen );
    }
    _wpi_getrectvalues( tool->area, &left, &top, &right, &bottom );
    _wpi_bitblt( pres, left, top, bar->button_size.x, bar->button_size.y, mempres, 0, 0,
                 SRCCOPY ); /* Copy it to the screen. */
    _wpi_getoldbitmap( mempres, old_hbitmap );
    if( delete_pres ) {
        _wpi_releasepres( hwnd, pres );
    }
    _wpi_deletebitmap( hbitmap );
    if( delete_mempres ) {
        _wpi_deletecompatiblepres( mempres, mem );
    }

} /* drawButton */

/*
 * The following function is never used.  Remove the #if 0 if it is needed in the future.
 */
#if 0

/*
 * isPointInToolbar - check if the point in the WM_MOUSEMOVE message is in the toolbar
 */
static bool isPointInToolbar( HWND hwnd, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    WPI_POINT   p;
    WPI_RECT    r;

    lparam = lparam;
    wparam = wparam;

    WPI_MAKEPOINT( wparam, lparam, p );
    _wpi_getclientrect( hwnd, &r );
    return( _wpi_ptinrect( &r, p ) != 0 );

} /* isPointInToolbar */

#endif

/*
 * findToolAtPoint - find a tool at a given point
 */
static tool *findToolAtPoint( toolbar *bar, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    WPI_POINT   p;
    tool        *tool;

    lparam = lparam;
    wparam = wparam;

    WPI_MAKEPOINT( wparam, lparam, p );
    for( tool = bar->tool_list; tool != NULL; tool = tool->next ) {
        if( _wpi_ptinrect( &tool->area, p ) ) {
            if( tool->flags & ITEM_BLANK ) {
                return( NULL );
            } else {
                return( tool );
            }
        }
    }
    return( NULL );

} /* findToolAtPoint */

#ifdef __NT__

/*
 * customHitTest - find a tool at a given point for a native toolbar
 */
static ctl_id customHitTest( toolbar *bar, POINT *pt )
{
    tb_idx      i;
    tb_idx      count;
    TBBUTTON    tbb;
    RECT        rc;

    count = (tb_idx)SendMessage( bar->hwnd, TB_BUTTONCOUNT, 0, 0L );
    for( i = 0; i < count; i++ ) {
        SendMessage( bar->hwnd, TB_GETITEMRECT, i, (LPARAM)&rc );
        if( PtInRect( &rc, *pt ) ) {
            SendMessage( bar->hwnd, TB_GETBUTTON, i, (LPARAM)&tbb );
            return( tbb.idCommand );
        }
    }
    return( NO_ID );

} /* customHitTest */

#endif

/*
 * HasToolAtPoint - return TRUE if a tool exists at a given point
 */
bool HasToolAtPoint( toolbar *bar, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
#ifdef __NT__
    POINT   pt;

    if( !IsCommCtrlLoaded() ) {
#endif
        return( findToolAtPoint( bar, wparam, lparam ) != NULL );
#ifdef __NT__
    } else {
        pt.x = LOWORD( lparam );
        pt.y = HIWORD( lparam );
        return( customHitTest( bar, &pt ) != NO_ID );
    }
#endif

} /* HasToolAtPoint */

/*
 * FindToolIDAtPoint - Find the tool ID at a given point, if any.  Returns
 *                     TRUE if tool exists at a given point.
 */
bool FindToolIDAtPoint( toolbar *bar, WPI_PARAM1 wparam, WPI_PARAM2 lparam, ctl_id *id )
{
    tool    *ctool;
#ifdef __NT__
    POINT   pt;
    ctl_id  ret;

    if( !IsCommCtrlLoaded() ) {
#endif
        ctool = findToolAtPoint( bar, wparam, lparam );
        if( ctool != NULL ) {
            *id = ctool->id;
            return( true );
        } else {
            return( false );
        }
#ifdef __NT__
    } else {
        pt.x = LOWORD( lparam );
        pt.y = HIWORD( lparam );
        ret = customHitTest( bar, &pt );
        if( ret != NO_ID ) {
            *id = ret;
            return( true );
        } else {
            return( false );
        }
    }
#endif

} /* FindToolIDAtPoint */

/*
 * ToolBarWndProc - callback routine for the tool bar
 */
WPI_MRESULT CALLBACK ToolBarWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    toolbar         *bar;
    tool            *tool;
    WPI_RECT        inter;
    WPI_PRES        pres;
    WPI_PRES        mempres;
    HDC             memdc;
    PAINTSTRUCT     ps;
    bool            posted;

    bar = WPI_GET_WNDINFO( hwnd );
    if( msg == WM_CREATE ) {
#if defined( __OS2_PM__ )
        bar = (toolbar *)PVOIDFROMMP( wparam );
        WinSetPresParam( hwnd, PP_BACKGROUNDCOLORINDEX, (ULONG)sizeof( LONG ) + 1, (PVOID)&btnColor );
#elif defined( __WINDOWS_386__ )
        bar = MapAliasToFlat( (DWORD)((CREATESTRUCT __far *)MK_FP32( (void *)lparam ))->lpCreateParams );
#else
        bar = (toolbar *)((CREATESTRUCT *)lparam)->lpCreateParams;
#endif
        bar->hwnd = hwnd;
        WPI_SET_WNDINFO( hwnd, (LONG_PTR)bar );
    }
    if( bar != NULL && bar->hook != NULL ) {
        if( bar->hook( hwnd, msg, wparam, lparam ) && msg != WM_DESTROY ) {
            return( 0L );
        }
    }
    switch( msg ) {
    case WM_SIZE:
        if( bar && bar->tool_list && !bar->is_fixed ) {
            createButtonList( hwnd, bar, bar->tool_list );
            _wpi_invalidaterect( hwnd, NULL, TRUE );
            _wpi_updatewindow( hwnd );
        }
        break;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
        if( bar && bar->tool_list ) {
            currTool = NULL;
            tool = findToolAtPoint( bar, wparam, lparam );
            if( tool ) {
                if( bar->hook != NULL ) {
                    bar->hook( hwnd, WM_USER, MPFROMSHORT( tool->id ), (WPI_PARAM2)0 );
                }
                currTool = tool;
                drawButton( hwnd, tool, true, NULLHANDLE, NULLHANDLE, NULLHANDLE );
                mouse_captured = true;
                _wpi_setcapture( hwnd );
                currIsDown = true;
                if( bar->helphook != NULL ) {
                    bar->helphook( hwnd, currTool->id, true );
                }
            }
        }
        break;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        if( bar && bar->tool_list ) {
            tool = findToolAtPoint( bar, wparam, lparam );
            posted = false;
            if( tool != NULL ) {
                if( tool == currTool ) {
                    _wpi_postmessage( bar->owner, WM_COMMAND, tool->id, CMDSRC_MENU );
                    posted = true;
                    drawButton( hwnd, tool, false, NULLHANDLE, NULLHANDLE, NULLHANDLE );
                }
            }
            if( !posted && bar->hook != NULL ) {
                if( currTool != NULL ) {
                    bar->hook( hwnd, WM_USER, MPFROMSHORT( currTool->id ), (WPI_PARAM2)1 );
                }
            }
            if( currTool != NULL ) {
                mouse_captured = false;
                ignore_mousemove = true; /* ReleaseCapture() generates a
                                            WM_MOUSEMOVE message */
                _wpi_releasecapture();
                if( bar->helphook != NULL ) {
                    bar->helphook( hwnd, currTool->id, false );
                }
                currTool = NULL;
            }
        }
        break;
    case WM_MOUSEMOVE:
        if( ignore_mousemove ) {
            ignore_mousemove = false;
            break;
        }
        tool = findToolAtPoint( bar, wparam, lparam );
        if( currTool ) {
            if( tool == currTool ) {
                if( !currIsDown ) {
                    currIsDown = true;
                    drawButton( hwnd, currTool, true, NULLHANDLE, NULLHANDLE, NULLHANDLE );
                    if( bar->helphook != NULL ) {
                        bar->helphook( hwnd, currTool->id, true );
                    }
                }
            } else {
                if( currIsDown ) {
                    drawButton( hwnd, currTool, false, NULLHANDLE, NULLHANDLE, NULLHANDLE );
                    currIsDown = false;
                    if( bar->helphook != NULL ) {
                        bar->helphook( hwnd, currTool->id, false );
                    }
                }
            }
        } else {
            if( bar->helphook != NULL ) {
                if( tool ) {
                    bar->helphook( hwnd, tool->id, true );
                    lastID = tool->id;
                } else if( lastID != NO_ID ) {
                    bar->helphook( hwnd, lastID, false );
                    lastID = NO_ID;
                }
            }
        }
        break;
#if defined( __NT__ ) || defined( __WINDOWS__ )
    case WM_SYSCOLORCHANGE:
        ToolBarChangeSysColors( (COLORREF)0L, (COLORREF)0L, (COLORREF)0L );
        break;
#endif
    case WM_PAINT:
#if defined( __NT__ ) || defined( __WINDOWS__ )
        if( btnColor != GetSysColor( COLOR_BTNFACE ) ) {
            /*
             * WM_SYSCOLORCHANGED is sometimes not received by the window,
             * so we have to fake it.
             */
            ToolBarChangeSysColors( (COLORREF)0L, (COLORREF)0L, (COLORREF)0L );
        }
#endif
        pres = _wpi_beginpaint( hwnd, NULLHANDLE, &ps );
        mempres = _wpi_createcompatiblepres( pres, appInst, &memdc );
#ifdef __OS2_PM__
        WinFillRect( pres, &ps, CLR_PALEGRAY );
        for( tool = bar->tool_list; tool != NULL; tool = tool->next ) {
            if( _wpi_intersectrect( appInst, &inter, &ps, &tool->area ) ) {
#else
        _wpi_fillrect( pres, &ps.rcPaint, clr_btnface, btnFaceBrush );
        for( tool = bar->tool_list; tool != NULL; tool = tool->next ) {
            if( _wpi_intersectrect( appInst, &inter, &ps.rcPaint, &tool->area ) ) {
#endif
                drawButton( hwnd, tool, false, pres, mempres, memdc );
            }
        }
        _wpi_deletecompatiblepres( mempres, memdc );
        _wpi_endpaint( hwnd, NULLHANDLE, &ps );
        break;
    case WM_DESTROY:
        bar->hwnd = HNULL;
        break;
    }
    return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );

} /* ToolBarWndProc */

#ifdef __NT__

/*
 * WinToolWndProc - callback for native toolbars
 */
LRESULT CALLBACK WinToolWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    toolbar         *bar;
    ctl_id          id;
    tb_idx          n;
    TBBUTTON        tbb;
    DWORD           pos;
    POINT           pt;
    RECT            rc;

    bar = (toolbar *)GetProp( hwnd, "bar" );
    switch( msg ) {
    case WM_MOUSEMOVE:
        if( FindToolIDAtPoint( bar, wparam, lparam, &id ) ) {
            n = (tb_idx)SendMessage( hwnd, TB_COMMANDTOINDEX, id, 0L );
            SendMessage( hwnd, TB_GETBUTTON, n, (LPARAM)&tbb );
            if( (tbb.fsState & TBSTATE_PRESSED) || (wparam & MK_LBUTTON) == 0 ) {
                bar->helphook( hwnd, id, true );
                lastID = id;
                SetTimer( hwnd, 1, 50, NULL );
            } else if( lastID != NO_ID ) {
                bar->helphook( hwnd, lastID, false );
                lastID = NO_ID;
                KillTimer( hwnd, 1 );
            }
        } else if( lastID != NO_ID ) {
            bar->helphook( hwnd, lastID, false );
            lastID = NO_ID;
            KillTimer( hwnd, 1 );
        }
        break;
    case WM_TIMER:
        if( lastID != NO_ID ) {
            pos = GetMessagePos();
            pt.x = LOWORD( pos );
            pt.y = HIWORD( pos );
            ScreenToClient( hwnd, &pt );
            GetClientRect( hwnd, &rc );
            if( !PtInRect( &rc, pt ) ) {
                bar->helphook( hwnd, lastID, false );
                lastID = NO_ID;
                KillTimer( hwnd, 1 );
            }
        };
        break;
    case WM_LBUTTONUP:
        if( lastID != NO_ID ) {
            bar->helphook( hwnd, lastID, false );
            lastID = NO_ID;
            KillTimer( hwnd, 1 );
        }
        break;
    case WM_DESTROY:
        if( bar->hook != NULL ) {
            bar->hook( hwnd, msg, wparam, lparam );
        }
        break;
    }
    return( CallWindowProc( bar->old_wndproc, hwnd, msg, wparam, lparam ) );

} /* WinToolWndProc */

/*
 * ToolContainerWndProc - window procedure for the frame window containing a native toolbar
 */
LRESULT CALLBACK ToolContainerWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    HWND        otherwnd;
    toolbar     *bar;

    if( msg == WM_NCCREATE ) {
        bar = ((CREATESTRUCT *)lparam)->lpCreateParams;
        SetProp( hwnd, "bar", bar );
    } else {
        bar = (toolbar *)GetProp( hwnd, "bar" );
    }
    if( bar != NULL ) {
        bar->hook( hwnd, msg, wparam, lparam );
    }

    switch( msg ) {
    case WM_SIZE:
        otherwnd = GetWindow( hwnd, GW_CHILD );
        SendMessage( otherwnd, WM_SIZE, 0, 0L );
        break;
    case WM_COMMAND:
        otherwnd = GetWindow( hwnd, GW_OWNER );
        SendMessage( otherwnd, WM_COMMAND, wparam, lparam );
        break;
    }
    return( DefWindowProc( hwnd, msg, wparam, lparam ) );

} /* ToolContainerWndProc */

#endif

/*
 * ChangeToolButtonBitmap - change a bitmap for a toolbar item
 */
void ChangeToolButtonBitmap( toolbar *bar, ctl_id id, WPI_HBITMAP hbitmap )
{
    tool        *t;
#ifdef __NT__
    tb_idx      n;
    TBBUTTON    tbb;
    TBADDBITMAP tbab;

    if( !IsCommCtrlLoaded() ) {
#endif
        t = findTool( bar->tool_list, id );
        if( t != NULL ) {
            t->u.hbitmap = hbitmap;
            _wpi_invalidaterect( bar->hwnd, &t->area, TRUE );
            _wpi_updatewindow( bar->hwnd );
        }
#ifdef __NT__
    } else {
        n = (tb_idx)SendMessage( bar->hwnd, TB_COMMANDTOINDEX, id, 0L );
        if( n >= 0 ) {
            SendMessage( bar->hwnd, TB_GETBUTTON, n, (LPARAM)&tbb );
            tbab.hInst = NULL;
            tbab.nID = (UINT_PTR)TB_CreateTransparentBitmap( hbitmap,
                bar->button_size.x - bar->border.x,
                bar->button_size.y - bar->border.y - 2 );
            tbb.iBitmap = (int)SendMessage( bar->hwnd, TB_ADDBITMAP, 1, (LPARAM)&tbab );
            SendMessage( bar->hwnd, TB_DELETEBUTTON, n, 0L );
            SendMessage( bar->hwnd, TB_INSERTBUTTON, n, (LPARAM)&tbb );
        }
    }
#endif

} /* ChangeToolButtonBitmap */


#if defined( __NT__ ) || defined( __WINDOWS__ )

/*
 * TB_TransparentBlt
 *
 * Purpose: Given two DC's and a color to assume as transparent in
 * the source, BitBlts the bitmap to the dest DC letting the existing
 * background show in place of the transparent color.
 * Adapted from an old MS SDK sample.
 *
 * NOTE: make sure BkColor is set in dest hDC.
 *
 * Parameters: hDC      HDC      destination, on which to draw.
 *             x, y     UINT     location at which to draw the bitmap
 *             width    UINT     width to draw
 *             height   UINT     height to draw
 *             hDCIn    HDC      source, to draw from
 *             cr       COLORREF to consider as transparent in source.
 *
 * Return Value: None
 */

#define ROP_DSPDxax  0x00E20746

void TB_TransparentBlt( HDC hDC, UINT x, UINT y, UINT width, UINT height,
                        HDC hDCIn, COLORREF cr )
{
   HDC      hDCMid, hMemDC;
   HBITMAP  mono_hbitmap;
   HBITMAP  mid_hbitmap;
   HBRUSH   hBr, hBrT;
   COLORREF crBack, crText;

   if( NULL == hDCIn ) {
      return;
   }

   /* Make two intermediate DC's */
   hDCMid = CreateCompatibleDC( hDC );
   hMemDC = CreateCompatibleDC( hDC );

   /* Create a monochrome bitmap for masking */
   mono_hbitmap = CreateCompatibleBitmap( hDCMid, x + width, y + height );
   SelectObject( hDCMid, mono_hbitmap );

   /* Create a mid-stage bitmap */
   mid_hbitmap = CreateCompatibleBitmap( hDC, x + width, y + height );
   SelectObject( hMemDC, mid_hbitmap );

   /* Create a monochrome mask where we have 0's in the image, 1's elsewhere. */
   crBack = SetBkColor( hDCIn, cr );
   BitBlt( hDCMid, x, y, width, height, hDCIn, x, y, SRCCOPY );
   SetBkColor( hDCIn, crBack );

   /* Put the unmodified image in the temporary bitmap */
   BitBlt( hMemDC, x, y, width, height, hDCIn, x, y, SRCCOPY );

   /* Create an select a brush of the background color */
   hBr = CreateSolidBrush( GetBkColor( hDC ) );
   hBrT = SelectObject( hMemDC, hBr );

   /* Force conversion of the monochrome to stay black and white. */
   crText = SetTextColor( hMemDC, 0L );
   crBack = SetBkColor( hMemDC, RGB( 255, 255, 255 ) );

   /*
    * Where the monochrome mask is 1, Blt the brush; where the mono
    * mask is 0, leave the destination untouched.  This results in
    * painting around the image with the background brush.  We do this
    * first in the temporary bitmap, then put the whole thing to the
    * screen (avoids flicker).
    */
   BitBlt( hMemDC, x, y, width, height, hDCMid, x, y, ROP_DSPDxax );
   BitBlt( hDC, x, y, width, height, hMemDC, x, y, SRCCOPY );

   SetTextColor( hMemDC, crText );
   SetBkColor( hMemDC, crBack );

   SelectObject( hMemDC, hBrT );
   DeleteObject( hBr );

   DeleteDC( hMemDC );
   DeleteDC( hDCMid );
   DeleteObject( mid_hbitmap );
   DeleteObject( mono_hbitmap );

}  /* TransparentBlt */

#endif

#ifdef __NT__

/*
 * TB_CreateTransparentBitmap()
 *
 * Purpose: Creates a transparent bitmap for use with Win32 native toolbars.
 *
 * Parameters: hBitmap  HBITMAP original bitmap
 *             width    int     width of the original bitmap
 *             height   int     height of the original bitmap
 *
 * Return Value: Handle to the new transparent bitmap.
 */

HBITMAP TB_CreateTransparentBitmap( HBITMAP hbitmap, int width, int height )
{
    HDC     hDC1;
    HDC     hDC2;
    HBITMAP old_hbitmap1;
    HBITMAP old_hbitmap2;
    HBITMAP new_hbitmap;

    hDC1 = CreateCompatibleDC( NULL );
    old_hbitmap1 = SelectObject( hDC1, hbitmap );
    hDC2 = CreateCompatibleDC( hDC1 );
    new_hbitmap = CreateCompatibleBitmap( hDC1, width, height );
    old_hbitmap2 = SelectObject( hDC2, new_hbitmap );
    SetBkColor( hDC2, GetSysColor( COLOR_3DFACE ) );
    TB_TransparentBlt( hDC2, 0, 0, width, height, hDC1, GetPixel( hDC1, 0, 0 ) );
    SelectObject( hDC1, old_hbitmap1 );
    SelectObject( hDC2, old_hbitmap2 );
    DeleteDC( hDC1 );
    DeleteDC( hDC2 );
    return( new_hbitmap );

} /* TB_CreateTransparentBitmap */

#endif
