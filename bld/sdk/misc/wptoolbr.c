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
#include <assert.h>
#include <stdlib.h>

#ifdef __OS2_PM__
#define INCL_PM
#define INCL_WINFRAMEMGR
#define INCL_NLS
#define INCL_GPILCIDS
#define INCL_GPIPRIMITIVES
#include <os2.h>

#define __FAR
#else
#include <windows.h>
#endif

#include "wpi.h"
#include "mem.h"
#include "wptoolbr.h"
#define TOOLBR_DIM  WPI_RECTDIM

typedef struct tool {
    struct tool *next;
    union {
        HBITMAP bitmap;
        WORD    blank_space;
    } u;                        // For compatability with compilers that
                                // don't support anonymous unions
    HBITMAP     depressed;
    WORD        id;
    UINT        flags;
    WORD        state;
    WPI_RECT    area;
} tool;

typedef struct toolbar {
    HWND        hwnd;
    HWND        owner;          // this is actually the parent
    toolhook    hook;
    helphook    helphook;
    WPI_POINT   button_size;
    WPI_POINT   border;
    HBITMAP     background;
    HBRUSH      foreground;
    HBRUSH      bgbrush;
    int         border_width;
    tool        *tool_list;
    char        is_fixed:1;
    char        spare:7;
} toolbar;

#define HNULL                   0

#define BORDER_WIDTH( bar )     1

#ifndef __OS2_PM__
#ifdef __WINDOWS_386__
    #define MAKEPTR( a ) ((void far *)MK_FP32( (void *) a ))
#else
    #define MAKEPTR( a ) ((LPVOID) a)
#endif
#endif

#define BLANK_SIZE( w ) ( (w) / 3 )
#define GET_INFO( w )   ((toolbar *)_wpi_getwindowlong( w, 0 ))
#define SET_INFO( w,i ) (_wpi_setwindowlong( w, 0, (LONG)(LPSTR)i))

static char     toolBarClassRegistered;
static char     *className = "WTool";

static char     gdiObjectsCreated;
static HPEN     blackPen;
static HPEN     btnShadowPen;
static HPEN     btnHighlightPen;
static HPEN     btnFacePen;
static HBRUSH   blackBrush;
static HBRUSH   btnFaceBrush;
static COLORREF btnColour;
static WPI_INST appInst;

static tool     *currTool;
static char     currIsDown;
static WORD     lastID = -1;
static BOOL     mouse_captured = FALSE;
static BOOL     ignore_mousemove = FALSE; // release_capture generates a
                                          // WM_MOUSEMOVE msg

MRESULT CALLBACK ToolBarWndProc( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );

static void toolbardestroywindow( HWND hwnd )
{
    hwnd = _wpi_getframe( hwnd );
    _wpi_destroywindow( hwnd );
}

/*
 * findTool - find tool item based on id
 */
static tool *findTool( tool *list, WORD id )
{
    while( list != NULL ) {
        if( list->id == id ) break;
        list = list->next;
    }
    return( list );

} /* findTool */

/*
 * addTool - add an item to the tool bar list
 */
static void addTool( tool **list, tool *t )
{
    tool        *curr, **last;

    last = list;
    for( curr = *list; curr != NULL; curr = curr->next ) {
        last = &curr->next;
    }
    *last = t;
    t->next = NULL;

} /* addTool */

/*
 * deleteTool - delete an item from the tool bar list
 */
static void deleteTool( tool **list, tool *t )
{
    tool        *curr, **last;

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
 * buttonPosition - get position of a button on the tool bar
 */
static BOOL buttonPosition( HWND hwnd, toolbar *bar, tool *top, WPI_POINT *p )
{
    WPI_RECT    rect;
    int         width, height;
    int         wndheight;
    tool        *curr;
    WPI_POINT   pos;

    _wpi_getclientrect( hwnd, &rect );
    width = _wpi_getwidthrect( rect ) - 2 * bar->border.x;
    height = _wpi_getheightrect( rect ) - 2 * bar->border.y;
    wndheight = _wpi_getheightrect( rect );
    curr = bar->tool_list;

    pos.y = 0;
    while( pos.y + bar->button_size.y <= height ) {
        pos.x = 0;
        while( pos.x + bar->button_size.x <= width ) {
            // we assert curr because top MUST be in the list - the only
            // way curr can be NULL is if top is NULL (bad) or not in the
            // list (also bad).
            assert( curr != NULL );
            if( curr == top ) {
                p->x = pos.x + bar->border.x;
#ifndef __OS2_PM__
                p->y = pos.y + bar->border.y;
#else
                p->y = _wpi_cvth_y(pos.y, wndheight) -
                                        bar->button_size.y - bar->border.y+1;
#endif
                return( TRUE );
            }
            if( curr->flags & ITEM_BLANK ) {
                pos.x += curr->u.blank_space;
            } else {
                pos.x += bar->button_size.x-1;
            }
            curr = curr->next;
        }
        pos.y += bar->button_size.y-1;
    }
    return( FALSE );

} /* buttonPosition */

/*
 * createButtonList - create all buttons on a tool bar
 */
static void createButtonList( HWND hwnd, toolbar *bar, tool *top )
{
    WPI_POINT   pos;

    // top must be an element in the tool_list hanging off of bar
    // we are going to create buttons for all the tools from top
    // to the end of the list
    while( top != NULL ) {
        if( !buttonPosition( hwnd, bar, top, &pos ) ) {
            // no more buttons will fit
            break;
        }
        _wpi_setrectvalues( &(top->area), pos.x, pos.y,
                                                pos.x + bar->button_size.x,
                                                pos.y + bar->button_size.y );
        top = top->next;
    }

} /* createButtonList */

void ToolBarRedrawButtons( struct toolbar *bar )
{
    if( bar ) {
        createButtonList( bar->hwnd, bar, bar->tool_list );
    }
}

/*
 * ToolBarInit - initialize the tool bar
 */
toolbar *ToolBarInit( HWND parent )
{
    COLORREF    clr_btnface;
    COLORREF    clr_btnshadow;
    COLORREF    clr_btnhighlight;
    COLORREF    clr_black;
    toolbar     *bar;
#ifndef __OS2_PM__
        /*
         ****************
         * Windows version of initialization
         ****************
         */
    WNDCLASS    wc;
    HANDLE      instance;

    instance = GET_HINSTANCE( parent );
    appInst = instance;

    if( !toolBarClassRegistered ) {
        wc.style = CS_DBLCLKS;
        wc.lpfnWndProc = (LPVOID) ToolBarWndProc;
        wc.lpszMenuName = NULL;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = sizeof( LPVOID );
        wc.hInstance = instance;
        wc.hIcon = HNULL;
        wc.hCursor = LoadCursor( (HANDLE) HNULL, IDC_ARROW );
        wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = className;
        RegisterClass( &wc );
        toolBarClassRegistered = TRUE;
    }
    clr_btnshadow = GetSysColor( COLOR_BTNSHADOW );
    clr_btnhighlight = GetSysColor( COLOR_BTNHIGHLIGHT );
    btnColour = GetSysColor( COLOR_BTNFACE );
    clr_btnface = btnColour;
    clr_black = RGB(0, 0, 0);
#else
        /*
         ******************
         * PM Version of the initialization
         ******************
         */
    int         rc;
    HAB         hab;

    hab = WinQueryAnchorBlock( parent );
    appInst.hab = hab;
    appInst.mod_handle = NULL;

    if( !toolBarClassRegistered ) {
        rc = WinRegisterClass( hab, className,
                                (PFNWP)ToolBarWndProc,
                                CS_MOVENOTIFY | CS_SIZEREDRAW | CS_CLIPSIBLINGS,
                                sizeof(PVOID)  );
        toolBarClassRegistered = TRUE;
    }
    clr_btnshadow = CLR_DARKGRAY;
    clr_btnhighlight = CLR_WHITE;
    clr_btnface = CLR_PALEGRAY;
    clr_black = CLR_BLACK;
    btnColour = CLR_PALEGRAY;
#endif

    bar = (toolbar *)MemAlloc( sizeof( toolbar ) );
    if ( bar ) {
        memset ( bar, 0, sizeof( toolbar ) );
        bar->border_width = 1;
        bar->owner = parent;
    }

    if( !gdiObjectsCreated ) {
        blackPen = _wpi_createpen( PS_SOLID, BORDER_WIDTH(bar), clr_black );
        btnShadowPen = _wpi_createpen( PS_SOLID, BORDER_WIDTH(bar), clr_btnshadow );
        btnHighlightPen = _wpi_createpen( PS_SOLID, BORDER_WIDTH(bar), clr_btnhighlight );
        btnFacePen = _wpi_createpen( PS_SOLID, BORDER_WIDTH(bar), clr_btnface );
        blackBrush = _wpi_createsolidbrush( clr_black );
        btnFaceBrush = _wpi_createsolidbrush( clr_btnface );
        gdiObjectsCreated = TRUE;
    }

    return( bar );

} /* ToolBarInit */

/*
 * ToolBarFini - done with the tool bar
 */
void ToolBarDestroy ( toolbar *bar )
{
    tool        *curr, *tmp;

    if ( bar ) {
        if( bar->hwnd != HNULL ) {
            toolbardestroywindow( bar->hwnd );
        }
        curr = bar->tool_list;
        while( curr != NULL ) {
            tmp = curr;
            curr = curr->next;
            MemFree( tmp );
        }
        if( bar->bgbrush != NULL ) {
            _wpi_deleteobject( bar->bgbrush );
        }
    }

    MemFree( bar );
}

/*
 * ToolBarFini - done with all tool bars
 */
void ToolBarFini( toolbar *bar )
{
    ToolBarDestroy ( bar );

    if( gdiObjectsCreated ) {
        _wpi_deleteobject( blackPen );
        _wpi_deleteobject( btnShadowPen );
        _wpi_deleteobject( btnHighlightPen );
        _wpi_deleteobject( btnFacePen );
        _wpi_deleteobject( blackBrush );
        _wpi_deleteobject( btnFaceBrush );
        gdiObjectsCreated = FALSE;
    }

} /* ToolBarFini */

/*
 * ToolBarAddItem - add a specific bitmap to the tool bar
 */
void ToolBarAddItem( toolbar *bar, TOOLITEMINFO *info )
{
    tool        *t;

    t = (tool *)MemAlloc( sizeof( tool ) );
    if( info->flags & ITEM_BLANK ) {
        t->u.blank_space = info->u.blank_space;
    } else {
        t->u.bitmap = info->u.bmp;
    }
    t->id = info->id;
    t->next = NULL;
    t->flags = info->flags;
    t->depressed = info->depressed;
    t->state = BUTTON_UP;
    addTool( &bar->tool_list, t );
    if( !(info->flags & ITEM_BLANK) && info->u.bmp != HNULL ) {
        createButtonList( bar->hwnd, bar, t );
    }

} /* ToolBarAddItem */

void ToolBarSetState( toolbar *bar, WORD id, WORD state )
{
    tool        *t;

    t = findTool( bar->tool_list, id );
    t->state = state;
    // force the button to be redrawn
    _wpi_invalidaterect( bar->hwnd, &t->area, FALSE );
}

WORD ToolBarGetState( toolbar *bar, WORD id )
{
    tool        *t;

    t = findTool( bar->tool_list, id );
    return( t->state );
}

/*
 * ToolBarDeleteItem - delete an item from the tool bar
 */
BOOL ToolBarDeleteItem( toolbar *bar, WORD id )
{
    tool        *t, *next;

    t = findTool( bar->tool_list, id );
    if( t != NULL ) {
        // need to destroy the window on the tool bar and recreate
        // all the other tools after this one
        next = t->next;
        deleteTool( &bar->tool_list, t );
        createButtonList( bar->hwnd, bar, next );
        return( TRUE );
    }
    return( FALSE );

} /* ToolBarDeleteItem */

#ifdef __OS2_PM__
static  WPI_PROC                oldFrameProc;
WPI_MRESULT CALLBACK FrameProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                                WPI_PARAM2 lparam )
{
    HWND        client;

    switch( msg ) {
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDBLCLK:
        client = WinWindowFromID( hwnd, FID_CLIENT );
        if( client != NULLHANDLE ) {
#ifndef __OS2_PM__
            _wpi_sendmessage( client, msg, 0, (WPI_PARAM2)0xfffffff );
#else
            _wpi_sendmessage( client, msg, (WPI_PARAM1)0xfffffff, 0 );
#endif
        }
        break;
    case WM_SYSCOMMAND:
        if( SHORT1FROMMP(wparam) != SC_CLOSE ) {
            client = WinWindowFromID( hwnd, FID_CLIENT );
            if( client != NULLHANDLE ) {
                _wpi_sendmessage( client, msg, wparam, lparam );
            }
            break;
        }
        // fall through!!
    case WM_CLOSE:
        client = WinWindowFromID( hwnd, FID_CLIENT );
        if( client != NULLHANDLE ) {
            _wpi_sendmessage( client, WM_CLOSE, 0, 0 );
        }
        _wpi_destroywindow( hwnd );
        return( NULL );
    }
    return( _wpi_callwindowproc( oldFrameProc, hwnd, msg, wparam, lparam ) );
}
#endif
/*
 * ToolBarDisplay - create and show the tool bar
 */
void ToolBarDisplay( toolbar *bar, TOOLDISPLAYINFO *disp )
{
    int         height, width;

    currTool = NULL;
    currIsDown = FALSE;
    lastID = -1;
    mouse_captured = FALSE;

    if( bar->bgbrush != NULL ) {
        _wpi_deleteobject( bar->bgbrush );
        bar->bgbrush = NULL;
    }

    if( disp->background != NULL ) {
        bar->bgbrush = _wpi_createpatternbrush( disp->background );
    }

    if( bar->hwnd != HNULL ) {
        if( _wpi_getcapture() == bar->hwnd ) {
            _wpi_releasecapture();
        }
        toolbardestroywindow( bar->hwnd );
    }

    bar->button_size = disp->button_size;
    bar->hook = disp->hook;
    bar->helphook = disp->helphook;
    bar->border = disp->border_size;
    bar->background = disp->background;
    bar->foreground = disp->foreground;
    bar->is_fixed = disp->is_fixed;
    width = _wpi_getwidthrect( (disp->area) );
    height = _wpi_getheightrect( (disp->area) );
#ifndef __OS2_PM__
    CreateWindow( className, NULL, disp->style,
        disp->area.left, disp->area.top, width, height,
        bar->owner, (HMENU) HNULL, GET_HINSTANCE( bar->owner ), bar );
    /*
     * Windows ignores the GETMINMAXINFO before the WM_CREATE or
     * something so we kluge it.
     */
    MoveWindow( bar->hwnd, disp->area.left, disp->area.top,
                                                width, height, TRUE );
#else
    {
        HWND    frame;
        HWND    parent;

        if ( disp->is_fixed ) {
            parent = bar->owner;
        } else {
            parent = HWND_DESKTOP;
        }
        frame = WinCreateStdWindow( parent, 0L, &(disp->style), NULL, "",
                                                0L, (HMODULE)0, 0, NULL);

        WinSetOwner( frame, bar->owner );
        oldFrameProc = _wpi_subclasswindow( frame, (WPI_PROC)FrameProc );

        WinSetPresParam( frame, PP_BACKGROUNDCOLORINDEX,
                                (ULONG)sizeof(LONG)+1, (PVOID)&btnColour );
        WinCreateWindow( frame, className, "", WS_VISIBLE, 0, 0, 0, 0, frame,
                                    HWND_TOP, FID_CLIENT, (PVOID)bar, NULL);

        WinSetWindowPos(frame, HWND_TOP, disp->area.xLeft,
                        disp->area.yBottom, width, height,
                        SWP_MOVE | SWP_SIZE | SWP_HIDE );
    }
#endif
} /* ToolBarDisplay */

/*
 * ToolBarWindow - return a handle to the toolbar window ... for PM this
 *                 is a handle to the frame window
 */
HWND ToolBarWindow( toolbar *bar )
{
    if( bar == NULL ) {
        return( HNULL );
    }
    return( _wpi_getframe(bar->hwnd) );

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
static void drawTopLeftCorner( WPI_PRES pres, WPI_POINT size, int border,
                                                                HPEN pen )

{
    HPEN        old_pen;
    WPI_POINT   pt;

    old_pen = _wpi_selectobject( pres, pen );
    _wpi_setpoint( &pt, border, size.y - 2*border );
    _wpi_cvth_pt( &pt, size.y );
    _wpi_movetoex( pres, &pt, NULL );

    _wpi_setpoint( &pt, border, border );
    _wpi_cvth_pt( &pt, size.y );
    _wpi_lineto( pres, &pt );

    _wpi_setpoint( &pt, size.x - border, border );
    _wpi_cvth_pt( &pt, size.y );
    _wpi_lineto( pres, &pt );
    _wpi_selectobject( pres, old_pen );

} /* drawTopLeftCorner */

/*
 * drawTopLeftInsideCorner - draws the top left corner of a button
 */
static void drawTopLeftInsideCorner( WPI_PRES pres, WPI_POINT size,
                                                    int border, HPEN pen )

{
    HPEN        old_pen;
    WPI_POINT   pt;

    old_pen = _wpi_selectobject( pres, pen );
    _wpi_setpoint( &pt, border*2, size.y - 2*border );
    _wpi_cvth_pt( &pt, size.y );
    _wpi_movetoex( pres, &pt, NULL );

    _wpi_setpoint( &pt, border*2, border*2 );
    _wpi_cvth_pt( &pt, size.y );
    _wpi_lineto( pres, &pt );

    pt.x = size.x - border;
    _wpi_lineto( pres, &pt );
    _wpi_selectobject( pres, old_pen );
} /* drawTopLeftCorner */

/*
 * drawBottomRightCorner - draws the bottom right corner of a button
 */
static void drawBottomRightCorner( WPI_PRES pres, WPI_POINT size, int border,
                                                                HPEN pen )
{
    HPEN        old_pen;
    WPI_POINT   pt;
    int         height;

    height = size.y;
    size.x -= 2*border;
    size.y -= 2*border;
    old_pen = _wpi_selectobject( pres, pen );
    _wpi_setpoint( &pt, size.x, border );
    _wpi_cvth_pt( &pt, height );
    _wpi_movetoex( pres, &pt, NULL );

    _wpi_setpoint( &pt, size.x, size.y );
    _wpi_cvth_pt( &pt, height );
    _wpi_lineto( pres, &pt );

    _wpi_setpoint( &pt, border-1, size.y );
    _wpi_cvth_pt( &pt, height );
    _wpi_lineto( pres, &pt );
    _wpi_selectobject( pres, old_pen );

} /* drawBottomRightCorner */

/*
 * drawBottomRightInsideCorner - draw the inside corner of a button
 */
static void drawBottomRightInsideCorner( WPI_PRES pres, WPI_POINT size,
                                                    int border, HPEN pen )
{
    HPEN        old_pen;
    WPI_POINT   pt;
    int         height;

    height = size.y;
    size.x -= 3*border;
    size.y -= 3*border;
    old_pen = _wpi_selectobject( pres, pen );
    _wpi_setpoint( &pt, 2*border, size.y );
    _wpi_cvth_pt( &pt, height );
    _wpi_movetoex( pres, &pt, NULL );

    _wpi_setpoint( &pt, size.x, size.y );
    _wpi_cvth_pt( &pt, height );
    _wpi_lineto( pres, &pt );

    _wpi_setpoint( &pt, size.x, border );
    _wpi_cvth_pt( &pt, height );
    _wpi_lineto( pres, &pt );
    _wpi_selectobject( pres, old_pen );

} /* drawBottomRightInsideCorner */

/*
 * drawBorder - draw to border of a button
 */
static void drawBorder( WPI_PRES pres, WPI_POINT size, int border )
{
    HPEN        old_pen;
    int         x,y;
    WPI_POINT   pt;

    y = _wpi_cvth_y( size.y-1, size.y );
    x = size.x-1;

    old_pen = _wpi_selectobject( pres, blackPen );

    _wpi_setpoint( &pt, 0, _wpi_cvth_y(0, size.y) );
    _wpi_movetoex( pres, &pt, NULL );

    pt.x = x;
    _wpi_lineto( pres, &pt );
    pt.y = y;
    _wpi_lineto( pres, &pt );
    pt.x = 0;
    _wpi_lineto( pres, &pt );
    pt.y = _wpi_cvth_y(0, size.y);
    _wpi_lineto( pres, &pt );

    _wpi_selectobject( pres, btnFacePen );
    _wpi_setpoint( &pt, 0, _wpi_cvth_y(0, size.y) );
    _wpi_movetoex( pres, &pt, NULL );
    pt.x = border;
#ifdef __OS2_PM__
    pt.x -= 1;
#endif
    _wpi_lineto( pres, &pt );
    pt.x = x;
    _wpi_movetoex( pres, &pt, NULL );
    pt.y = _wpi_cvth_y( border, size.y );
#ifdef __OS2_PM__
    pt.y += 1;
#endif
    _wpi_lineto( pres, &pt );

    pt.y = y;
    _wpi_movetoex( pres, &pt, NULL );
    pt.x = x - border;
#ifdef __OS2_PM__
    pt.x += 1;
#endif
    _wpi_lineto( pres, &pt );
    pt.x = 0;
    _wpi_movetoex( pres, &pt, NULL );
    pt.y = _wpi_cvth_y( _wpi_cvth_y(y, size.y) - border, size.y );
#ifdef __OS2_PM__
    pt.y -= 1;
#endif
    _wpi_lineto( pres, &pt );

    _wpi_selectobject( pres, old_pen );

} /* drawBorder */

/*
 * toolBarDrawBitmap - draw the bitmap on a button
 */
static void toolBarDrawBitmap( WPI_PRES pres, WPI_POINT dst_size,
                                        WPI_POINT dst_org, HBITMAP bitmap )
{
    WPI_BITMAP          bm;
    WPI_PRES            mempres;
    HDC                 memdc;
    WPI_POINT           src_org, src_size;
    HBRUSH              old_brush;
    HBITMAP             old_bmp;

    DPtoLP( pres, &dst_size, 1 );
    DPtoLP( pres, &dst_org, 1 );

    mempres = _wpi_createcompatiblepres( pres, appInst, &memdc );
    old_bmp = _wpi_selectbitmap( mempres, bitmap );

    _wpi_getbitmapstruct( bitmap, &bm );

    src_size.x = _wpi_bitmapwidth( &bm );
    src_size.y = _wpi_bitmapheight( &bm );
    DPtoLP( pres, &src_size, 1 );

    src_org.x = 0;
    src_org.y = 0;
    DPtoLP( pres, &src_org, 1 );

    #ifdef __NT__
        SetStretchBltMode( pres, COLORONCOLOR );
    #else
        SetStretchBltMode( pres, STRETCH_DELETESCANS );
    #endif

    /*
     * If it's a monochrome bitmap try and do the right thing - I can see
     * this pissing off some users, but oh well.
     */
    if( _wpi_bitmapbitcount( &bm ) == 1 && _wpi_bitmapplanes( &bm ) == 1 ) {
        old_brush = _wpi_selectobject( pres, blackBrush );

        _wpi_stretchblt( pres, dst_org.x, dst_org.y, dst_size.x, dst_size.y,
            mempres, src_org.x, src_org.y, src_size.x, src_size.y, 0xb8074a );

        _wpi_getoldbitmap( mempres, old_bmp );
        _wpi_selectobject( pres, old_brush );
    } else {
        _wpi_stretchblt( pres, dst_org.x, dst_org.y, dst_size.x, dst_size.y,
            mempres, src_org.x, src_org.y, src_size.x, src_size.y, SRCCOPY );

        _wpi_getoldbitmap( mempres, old_bmp );
    }
    _wpi_deletecompatiblepres( mempres, memdc );

} /* toolBarDrawBitmap */

/*
 * drawButton - draw a button on the toolbar
 */
static void drawButton( HWND hwnd, tool *tool, BOOL down,
                        WPI_PRES pres, WPI_PRES mempres, HDC mem )
{
    toolbar     *bar;
    HBRUSH      brush;
    HBITMAP     bitmap, oldbmp;
    int         shift;
    BOOL        selected;
    WPI_POINT   dst_size;
    WPI_POINT   dst_org;
    HBITMAP     used_bmp;
    TOOLBR_DIM  left;
    TOOLBR_DIM  right;
    TOOLBR_DIM  top;
    TOOLBR_DIM  bottom;
    BOOL        delete_pres;
    BOOL        delete_mempres;

    if( tool->flags & ITEM_BLANK ) {
        return;
    }

    /* draw the button */
    bar = GET_INFO( hwnd );

    if( tool->flags & ITEM_STICKY ) {
        selected = (tool->state == BUTTON_DOWN);
    } else {
        selected = down;
    }
    shift = 0;
    if( selected ) {
        shift = 2*BORDER_WIDTH( bar );
    }

    delete_pres = FALSE;
    delete_mempres = FALSE;
    if( pres == NULLHANDLE ) {
        pres = _wpi_getpres( hwnd );
        mempres = NULLHANDLE;
        delete_pres = TRUE;
    }
    if( ( mempres == NULLHANDLE ) || ( mem == NULLHANDLE ) ) {
        mempres = _wpi_createcompatiblepres( pres, appInst, &mem );
        delete_mempres = TRUE;
    }
    bitmap = _wpi_createcompatiblebitmap( pres, bar->button_size.x,
                                            bar->button_size.y );
    oldbmp = _wpi_selectbitmap( mempres, bitmap );

    brush = btnFaceBrush;
    if( selected && bar->bgbrush != HNULL ) {
        brush = bar->bgbrush;
    }
    // Does this do anything ???
//    _wpi_fillrect( mempres, &tool->area, btnColour, brush );

    dst_size = bar->button_size;
    dst_size.x -= 4 * BORDER_WIDTH( bar );
    dst_size.y -= 4 * BORDER_WIDTH( bar );
    dst_org.x = (1 * BORDER_WIDTH( bar ))+shift;
    dst_org.y = (1 * BORDER_WIDTH( bar ))+shift;
#ifdef __OS2_PM__
    dst_org.y = _wpi_cvth_y( dst_org.y, (bar->button_size.y) );
    dst_org.y = dst_org.y - dst_size.y + 1;
#endif
    used_bmp = tool->u.bitmap;
    if( selected ) {
        // if the button is selected and it has the ITEM_DOWNBMP flag
        // then we draw the alternate bitmap instead.
        if( tool->flags & ITEM_DOWNBMP ) {
            used_bmp = tool->depressed;
        }
    }
    toolBarDrawBitmap( mempres, dst_size, dst_org, used_bmp );

    drawBorder( mempres, bar->button_size, BORDER_WIDTH( bar ) );
    if( selected ) {
        drawTopLeftCorner( mempres, bar->button_size, BORDER_WIDTH( bar ),
                btnShadowPen );
        drawTopLeftInsideCorner( mempres, bar->button_size,
                                        BORDER_WIDTH( bar ), btnFacePen );
    } else {
        drawTopLeftCorner( mempres, bar->button_size, BORDER_WIDTH( bar ),
                btnHighlightPen );
        drawBottomRightCorner( mempres, bar->button_size, BORDER_WIDTH( bar ),
                btnShadowPen );
        drawBottomRightInsideCorner( mempres, bar->button_size,
                                        BORDER_WIDTH( bar ), btnShadowPen );

    }
    _wpi_getrectvalues( (tool->area), &left, &top, &right, &bottom );
    _wpi_bitblt( pres, left, top, bar->button_size.x, bar->button_size.y,
            mempres, 0, 0, SRCCOPY );           /* copy it to screen */
    _wpi_getoldbitmap( mempres, oldbmp );
    if( delete_pres ) {
        _wpi_releasepres( hwnd, pres );
    }
    _wpi_deletebitmap( bitmap );
    if( delete_mempres ) {
        _wpi_deletecompatiblepres( mempres, mem );
    }

} /* drawButton */

/*
 * isPointInToolbar - is the point in the mousemove message in the toolbar
 */
BOOL isPointInToolbar( HWND hwnd, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    WPI_POINT   p;
    WPI_RECT    r;

    lparam = lparam;            // to suppress compiler warnings for PM
    wparam = wparam;            // to suppress compiler warnings for Win

    WPI_MAKEPOINT( wparam, lparam, p );
    _wpi_getclientrect( hwnd, &r );
    if( _wpi_ptinrect( &r, p ) ) {
        return( TRUE );
    }
    return( FALSE );
} /* isPointInToolbar */

/*
 * findToolAtPoint - find a tool at a given point
 */
static tool *findToolAtPoint( toolbar *bar, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    WPI_POINT   p;
    tool        *tool;

    lparam = lparam;            // to suppress compiler warnings for PM
    wparam = wparam;            // to suppress compiler warnings for Win

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

/*
 * HasToolAtPoint - return TRUE if tool exists at a given point
 */
BOOL HasToolAtPoint( struct toolbar *bar, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    return( findToolAtPoint( bar, wparam, lparam ) != NULL );

} /* HasToolAtPoint */

/*
 * ToolBarWndProc - callback routine for the tool bar
 */
MRESULT CALLBACK ToolBarWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                                                        WPI_PARAM2 lparam )
{
    CREATESTRUCT        __FAR   *cs;
    toolbar             *bar;
    tool                *tool;
    WPI_RECT            inter;
    WPI_PRES            pres;
    WPI_PRES            mempres;
    HDC                 memdc;
    PAINTSTRUCT         ps;
    BOOL                posted;

    bar = GET_INFO( hwnd );
    if( msg == WM_CREATE ) {
#ifndef __OS2_PM__
        cs = MAKEPTR( lparam );
        bar = (toolbar *)cs->lpCreateParams;
        #ifdef __WINDOWS_386__
            bar = MapAliasToFlat( (DWORD) bar );
        #endif
#else
        cs = PVOIDFROMMP( wparam );
        bar = (toolbar *)cs;
        WinSetPresParam( hwnd, PP_BACKGROUNDCOLORINDEX,
                                (ULONG)sizeof(LONG)+1, (PVOID)&btnColour );
#endif
        bar->hwnd = hwnd;
        SET_INFO( hwnd, bar );
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
                    bar->hook( hwnd, WM_USER, MPFROMSHORT(tool->id),
                                                        (WPI_PARAM2)0 );
                }
                currTool = tool;
                drawButton( hwnd, tool, TRUE, NULL, NULL, NULL );
                mouse_captured = TRUE;
                _wpi_setcapture( hwnd );
                currIsDown = TRUE;
                if( bar->helphook != NULL ) {
                    bar->helphook( hwnd, MPFROMSHORT(currTool->id), TRUE );
                }
            }
        }
        break;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        if( bar && bar->tool_list ) {
            tool = findToolAtPoint( bar, wparam, lparam );
            posted = FALSE;
            if( tool != NULL ) {
                if( tool == currTool ) {
                    _wpi_postmessage( bar->owner, WM_COMMAND, tool->id, CMDSRC_MENU );
                    posted = TRUE;
                    drawButton( hwnd, tool, FALSE, NULL, NULL, NULL );
                }
            }
            if( !posted && bar->hook != NULL ) {
                if( currTool != NULL ) {
                    bar->hook( hwnd, WM_USER, MPFROMSHORT(currTool->id),
                                                            (WPI_PARAM2)1 );
                }
            }
            if( currTool != NULL ) {
                mouse_captured = FALSE;
                ignore_mousemove = TRUE; // release_capture generates a
                                         // WM_MOUSEMOVE
                _wpi_releasecapture();
                if( bar->helphook != NULL ) {
                    bar->helphook( hwnd, MPFROMSHORT(currTool->id), FALSE );
                }
                currTool = NULL;
            }
        }
        break;
    case WM_MOUSEMOVE:
        if( ignore_mousemove ) {
            ignore_mousemove = FALSE;
            break;
        }
        tool = findToolAtPoint( bar, wparam, lparam );
        if( currTool ) {
            if( tool == currTool ) {
                if( !currIsDown ) {
                    currIsDown = TRUE;
                    drawButton( hwnd, currTool, TRUE, NULL, NULL, NULL );
                    if( bar->helphook != NULL ) {
                        bar->helphook( hwnd, MPFROMSHORT(currTool->id), TRUE );
                    }
                }
            } else {
                if( currIsDown ) {
                    drawButton( hwnd, currTool, FALSE, NULL, NULL, NULL );
                    currIsDown = FALSE;
                    if( bar->helphook != NULL ) {
                        bar->helphook( hwnd, MPFROMSHORT(currTool->id), FALSE );
                    }
                }
            }
        } else {
            if( bar->helphook != NULL ) {
                if( tool ) {
                    bar->helphook( hwnd, MPFROMSHORT(tool->id), TRUE );
                    lastID = tool->id;
                } else if( lastID != (WORD)-1 ) {
                    bar->helphook( hwnd, MPFROMSHORT(lastID), FALSE );
                    lastID = -1;
                }
            }
        }
        break;
    case WM_PAINT:
        pres = _wpi_beginpaint( hwnd, NULL, &ps );
        mempres = _wpi_createcompatiblepres( pres, appInst, &memdc );
#ifdef __OS2_PM__
        WinFillRect( pres, &ps, CLR_PALEGRAY );
        for( tool = bar->tool_list; tool != NULL; tool = tool->next ) {
            if( _wpi_intersectrect( appInst, &inter, &ps, &tool->area ) ) {
#else
        for( tool = bar->tool_list; tool != NULL; tool = tool->next ) {
            if( _wpi_intersectrect( appInst, &inter, &ps.rcPaint, &tool->area ) ) {
#endif
                drawButton( hwnd, tool, FALSE, pres, mempres, memdc );
            }
        }
        _wpi_deletecompatiblepres( mempres, memdc );
        _wpi_endpaint( hwnd, NULL, &ps );
        break;

    case WM_DESTROY:
        bar->hwnd = HNULL;
        break;
    }
    return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );

} /* ToolBarWndPproc */

/*
 * ChangeToolButtonBitmap - change a bitmap for a toolbar item
 */
void ChangeToolButtonBitmap( toolbar *bar, WORD id, HBITMAP new )
{
    tool        *t;
    t = findTool( bar->tool_list, id );
    if( t != NULL ) {
        t->u.bitmap = new;
        _wpi_invalidaterect( bar->hwnd, &t->area, TRUE );
        _wpi_updatewindow( bar->hwnd );
    }

} /* ChangeToolButtonBitmap */
