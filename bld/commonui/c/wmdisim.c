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
* Description:  MDI implementation for Windows and OS/2.
*
****************************************************************************/


#include "precomp.h"
#ifdef __WINDOWS_386__
    #include <malloc.h>
#endif
#include <string.h>
#include <stdio.h>

#include "bool.h"
#include "wmdisim.h"
#include "mem.h"


#define MAX_STR         256
#define STATE_NORMAL    0x00
#define STATE_MAX       0x01
#define CLOSE_BITMAP_X  18
#define CLOSE_BITMAP_Y  18

typedef struct mdi_data {
    struct mdi_data     *next;
    HWND                hwnd;
    WPI_RECT            orig_size;
    char                orig_state;
    char                curr_state;
} mdi_data;

#if defined( __WINDOWS__ )
typedef int (WINAPI *TILECHILDPROC)( HWND parent, WORD action );
typedef int (WINAPI *CASCADECHILDPROC)( HWND parent, WORD action );
#endif

#if defined( __NT__ )
extern int WINAPI TileChildWindows( HWND parent, WORD action );
extern int WINAPI CascadeChildWindows( HWND parent, WORD action );
#endif

static mdi_info mdiInfo;
static char     childrenMaximized;
static char     updatedMenu;
static char     insertedItems;
static HBITMAP  closeBitmap;
static HBITMAP  restoreBitmap;
static HBITMAP  restoredBitmap;
static mdi_data *mdiHead;
static mdi_data *mdiTail;
static HWND     currentWindow;
//static WPI_RECT       minChildRect;
//static char   haveMinChildRect;

#define GET_WND_MDI_DATA( hwnd ) ((mdi_data *)_wpi_getwindowlongptr( hwnd, mdiInfo.data_off ))
#define SET_WND_MDI_DATA( hwnd, data ) ((mdi_data *)_wpi_setwindowlongptr( hwnd, mdiInfo.data_off, data ))

static void deleteMaximizedMenuConfig( void );
static void setMaximizedMenuConfig( HWND hwnd );

void SetSystemMenu( HWND hwnd );

/*
 * MDIInit - initialize MDI
 */
void MDIInit( mdi_info *mi )
{
    mdiInfo = *mi;

} /* MDIInit */

/*
 * MDIInitMenu - initialize menu for MDI
 */
void MDIInitMenu( void )
{
    if( childrenMaximized ) {
        MDIClearMaximizedMenuConfig();
        deleteMaximizedMenuConfig();
        setMaximizedMenuConfig( currentWindow );
        if( currentWindow != NULLHANDLE ) {
            mdiInfo.set_window_title( currentWindow );
        }
    } else {
        _wpi_drawmenubar( mdiInfo.root );
    }

} /* MDIInitMenu */

/*
 * MDISetOrigSize
 */
void MDISetOrigSize( HWND hwnd, WPI_RECT *rect )
{
    mdi_data    *md;

    md = GET_WND_MDI_DATA( hwnd );

    CopyRect( &md->orig_size, rect );

} /* MDISetOrigSize */

/*
 * doMaximize - handle maximizing an edit window
 */
static void doMaximize( HWND hwnd )
{
    DWORD               style;
    mdi_data            *md;
    WPI_RECT            r;
    bool                iconic;
    WPI_RECTDIM         left;
    WPI_RECTDIM         top;
    WPI_RECTDIM         right;
    WPI_RECTDIM         bottom;

    setMaximizedMenuConfig( hwnd );

    md = GET_WND_MDI_DATA( hwnd );

    if( mdiInfo.start_max_restore != NULL ) {
        mdiInfo.start_max_restore( hwnd );
    }

    iconic = _wpi_isiconic( hwnd );
    if( iconic ) {
        _wpi_getrestoredrect( hwnd, &md->orig_size );
    } else {
        _wpi_getwindowrect( hwnd, &md->orig_size );
    }
    md->orig_state = md->curr_state;
    md->curr_state = STATE_MAX;

    if( mdiInfo.set_style != NULL ) {
        (mdiInfo.set_style)( hwnd, TRUE );
    } else {
        style = _wpi_getwindowlong( hwnd, GWL_STYLE );
        style &= ~mdiInfo.reg_style;
        style |= mdiInfo.max_style;
        _wpi_setwindowlong( hwnd, GWL_STYLE, style );
    }
    _wpi_setscrollrange( hwnd, SB_VERT, 1, 1, TRUE );
    _wpi_setscrollrange( hwnd, SB_HORZ, 1, 1, TRUE );
    _wpi_getwindowrect( mdiInfo.container, &r );

    _wpi_getrectvalues( r, &left, &top, &right, &bottom );

    if( !iconic ) {
        _wpi_offsetrect( mdiInfo.hinstance, &md->orig_size, -left, -top );
    }

    _wpi_setrectvalues( &r, 0, 0, right-left+1, bottom-top+1 );

    if( iconic ) {
        _wpi_setrestoredrect( hwnd, &r );
    } else {
        _wpi_getrectvalues( r, &left, &top, &right, &bottom );
        _wpi_movewindow( hwnd, left, top, right, bottom, TRUE );
    }

    if( mdiInfo.end_max_restore != NULL ) {
        mdiInfo.end_max_restore( hwnd );
    }

    _wpi_invalidaterect( hwnd, NULL, FALSE );

} /* doMaximize */

/*
 * doRestore - handle restoring an edit window
 */
static void doRestore( HWND hwnd )
{
    DWORD       style;
    mdi_data    *md;
    WPI_RECTDIM left;
    WPI_RECTDIM top;
    WPI_RECTDIM right;
    WPI_RECTDIM bottom;

    md = GET_WND_MDI_DATA( hwnd );

    if( md->curr_state == STATE_NORMAL ) {
        return;
    }

    if( mdiInfo.start_max_restore != NULL ) {
        mdiInfo.start_max_restore( hwnd );
    }

    md->curr_state = md->orig_state = STATE_NORMAL;

    if( mdiInfo.set_style != NULL ) {
        (mdiInfo.set_style)( hwnd, FALSE );
    } else {
        style = _wpi_getwindowlong( hwnd, GWL_STYLE );
        style &= ~mdiInfo.max_style;
        style |= mdiInfo.reg_style;
        _wpi_setwindowlong( hwnd, GWL_STYLE, style );
    }

    _wpi_setscrollrange( hwnd, SB_VERT, 1, 1, TRUE );
    _wpi_setscrollrange( hwnd, SB_HORZ, 1, 1, TRUE );
    _wpi_updatewindow( hwnd );
    _wpi_getrectvalues( md->orig_size, &left, &top, &right, &bottom );
    _wpi_movewindow( hwnd, left, top, right - left, bottom - top, TRUE );
    if( mdiInfo.end_max_restore != NULL ) {
        mdiInfo.end_max_restore( hwnd );
    }

} /* doRestore */

/*
 * doRestoreAll - set all children as needing restoration
 */
static void doRestoreAll( void )
{
    mdi_data    *md;

    if( !childrenMaximized ) {
        return;
    }
    _wpi_setwindowtext( mdiInfo.root, mdiInfo.main_name );
    childrenMaximized = FALSE;
    md = mdiHead;
    while( md != NULL ) {
        doRestore( md->hwnd );
        md = md->next;
    }
    MDIClearMaximizedMenuConfig();
    deleteMaximizedMenuConfig();

} /* doRestoreAll */

#ifndef __OS2_PM__

/*
 * doMaximizeAll - maximize all children
 */
static void doMaximizeAll( HWND first )
{
    mdi_data    *md;
    bool        was_max;

    was_max = childrenMaximized;

    childrenMaximized = TRUE;

    doMaximize( first );

    if( !was_max ) {
        md = mdiHead;
        while( md != NULL ) {
            if( md->hwnd != first ) {
                if( _wpi_isiconic( md->hwnd ) ) {
                    doMaximize( md->hwnd );
                }
            }
            md = md->next;
        }
    }

    SetSystemMenu( first );

} /* doMaximizeAll */

#endif

/*
 * getMenuBitmaps - load restore/restored bitmaps, and
 *                  get a bitmap for the close gadget (ack pft)
 */
static void getMenuBitmaps( void )
{
    if( restoreBitmap == NULLHANDLE ) {
#ifdef __OS2_PM__
        restoreBitmap = WinGetSysBitmap( HWND_DESKTOP, SBMP_RESTOREBUTTON );
#else
        restoreBitmap = LoadBitmap( (HANDLE)NULL, MAKEINTRESOURCE( OBM_RESTORE ) );
#endif
    }
    if( restoredBitmap == NULLHANDLE ) {
#ifdef __OS2_PM__
        restoredBitmap = WinGetSysBitmap( HWND_DESKTOP, SBMP_RESTOREBUTTONDEP );
#else
        restoredBitmap = LoadBitmap( (HANDLE)NULL, MAKEINTRESOURCE( OBM_RESTORED ) );
#endif
    }

    if( closeBitmap == NULLHANDLE ) {
#ifdef __OS2_PM__
        closeBitmap = WinGetSysBitmap( HWND_DESKTOP, SBMP_SYSMENU );
#else
        closeBitmap = LoadBitmap( mdiInfo.hinstance, "CLOSEBMP" );
#endif
    }
    updatedMenu = TRUE;

} /* getMenuBitmaps */

/*
 * duplicateMenu - create a duplicate copy of a menu
 */
static HMENU duplicateMenu( HMENU orig )
{
    WPI_MENUSTATE       mstate;
    int                 num;
    unsigned            menu_flags;
    unsigned            attr_flags;
    UINT                id;
    char                name[MAX_STR];
    int                 i;
    HMENU               copy;
    HMENU               sub;

    copy = NULLHANDLE;
    if( orig != NULLHANDLE ) {
        copy = _wpi_createpopupmenu();
        if( copy == NULLHANDLE ) {
            return( NULLHANDLE );
        }
        num = (int)_wpi_getmenuitemcount( orig );
        for( i = 0; i < num; i++ ) {
            if( _wpi_getmenustate( orig, i, &mstate, TRUE ) ) {
                _wpi_getmenuflagsfromstate( &mstate, &menu_flags, &attr_flags );
                if( _wpi_ismenuseparatorfromstate( &mstate ) ) {
                    _wpi_appendmenu( copy, menu_flags, attr_flags, 0, NULLHANDLE, NULL );
                } else if( _wpi_ismenupopupfromstate( &mstate ) ) {
                    sub = duplicateMenu( _wpi_getsubmenu( orig, i ) );
                    name[0] = 0;
                    _wpi_getmenutext( orig, i, name, MAX_STR - 1, TRUE );
                    _wpi_appendmenu( copy, menu_flags, attr_flags, 0, sub, name );
                } else {
                    id = _wpi_getmenuitemid( orig, i );
                    _wpi_getmenutext( orig, i, name, MAX_STR - 1, TRUE );
                    _wpi_appendmenu( copy, menu_flags, attr_flags, id, NULLHANDLE, name );
                }
            }
        }
    }
    return( copy );

} /* duplicateMenu */

/*
 * generateSystemMenu - generate a copy of the system menu for given window
 */
static HMENU generateSystemMenu( HWND hwnd )
{
    HMENU       sys_menu;

    sys_menu = _wpi_getsystemmenu( hwnd );
    if( sys_menu != NULLHANDLE ) {
        return( duplicateMenu( sys_menu ) );
    } else {
        return( NULLHANDLE );
    }

} /* generateSystemMenu */

/*
 * modifyChildSystemMenu - adjust system menu to make it a child system menu
 */
static HMENU modifyChildSystemMenu( HMENU sys_menu )
{
    if( sys_menu == NULLHANDLE ) {
        return( NULLHANDLE );
    }

    /* fix hotkey designation for close */
    _wpi_setmenutext( sys_menu, SC_CLOSE, "&Close\tCtrl+F4", FALSE );

    /* remove task switch option */
    _wpi_deletemenu( sys_menu, SC_TASKLIST, FALSE );

    /* add next window option */
    _wpi_appendmenu( sys_menu, MF_STRING, 0, SC_NEXTWINDOW, NULLHANDLE, "Nex&t\tCtrl+F6" );

    return( sys_menu );

} /* modifyChildSystemMenu */

/*
 * SetSystemMenu -- make the system menu showing belong to the current window
 */
void SetSystemMenu( HWND hwnd )
{
    HMENU       sys_menu;
    HMENU       menu;

    sys_menu = NULLHANDLE;
    if( hwnd != NULLHANDLE ) {
        sys_menu = generateSystemMenu( hwnd );
    }
    menu = _wpi_getmenu( mdiInfo.root );
    getMenuBitmaps();
#ifndef __OS2_PM__
    if( sys_menu != NULL ) {
        ModifyMenu( menu, 0, MF_POPUP | MF_BYPOSITION | MF_BITMAP,
                    (UINT_PTR)sys_menu, (LPVOID)closeBitmap );
    } else {
        ModifyMenu( menu, 0, MF_BYPOSITION | MF_BITMAP, -1, (LPVOID)closeBitmap );
    }
#else
    if( sys_menu != NULLHANDLE ) {
        _wpi_modifymenu( menu, 0, MF_POPUP | MF_STRING, 0, 0, sys_menu, "SYSMENU", TRUE );
    } else {
        _wpi_modifymenu( menu, 0, MF_STRING, 0, 0, NULLHANDLE, "SYSMENU", TRUE );
    }
#endif
    _wpi_drawmenubar( mdiInfo.root );

} /* SetSystemMenu */

#ifndef __OS2_PM__

/*
 * hitSysMenu - check if a specified point hit the system menu
 */
static bool hitSysMenu( HWND hwnd, WPI_PARAM2 lparam )
{
    WPI_RECT    r;
    WPI_POINT   pt;
    int         left;
    int         top;
    int         right;
    int         bottom;

    _wpi_getwindowrect( hwnd, &r );
    _wpi_getrectvalues( r, &left, &top, &right, &bottom );
    top  += _wpi_getsystemmetrics( SM_CYCAPTION ) +
            _wpi_getsystemmetrics( SM_CYFRAME );
    left += _wpi_getsystemmetrics( SM_CXFRAME );
    bottom = top + CLOSE_BITMAP_Y;
    right  = left + CLOSE_BITMAP_X;
    _wpi_setrectvalues( &r, left, top, right, bottom );
    WPI_MAKEPOINT(lparam, lparam, pt);
    return( _wpi_ptinrect( &r, pt ) );

} /* hitSysMenu */

#endif

#if 0

/*
 * HitRestoreButton - check if a specified point hit the restore button
 */
bool HitRestoreButton( HWND hwnd, WPI_PARAM2 lparam )
{
    WPI_RECT    r;
    WPI_POINT   pt;
    int         left;
    int         top;
    int         right;
    int         bottom;

    _wpi_getwindowrect( hwnd, &r );
    _wpi_getrectvalues( r, &left, &top, &right, &bottom );
    top   += _wpi_getsystemmetrics( SM_CYCAPTION ) +
             _wpi_getsystemmetrics( SM_CYFRAME );
    bottom = top + CLOSE_BITMAP_Y;
    right -= _wpi_getsystemmetrics( SM_CXFRAME );
    left   = right - CLOSE_BITMAP_X;
    _wpi_setrectvalues( &r, left, top, right, bottom );
    WPI_MAKEPOINT(lparam, lparam, pt);
    return( _wpi_ptinrect( &r, pt ) );

} /* HitRestoreButton */

/*
 * SetRestoreBitmap - set the bitmap on our restore menu item
 */
void SetRestoreBitmap( bool pressed )
{
    HMENU       menu;

    menu = _wpi_getmenu( mdiInfo.root );
    if( pressed ) {
        ModifyMenu( menu, 7, MF_BYPOSITION | MF_BITMAP | MF_HELP,
                    SC_RESTORE, (LPVOID) restoredBitmap );
    } else {
        ModifyMenu( menu, 7, MF_BYPOSITION | MF_BITMAP | MF_HELP,
                    SC_RESTORE, (LPVOID) restoreBitmap );
    }
    _wpi_drawmenubar( mdiInfo.root );

} /* SetRestoreBitmap */

#endif

/*
 * setMaximizedMenuConfig - set up main menu in the maximized configuration
 */
static void setMaximizedMenuConfig( HWND hwnd )
{
#ifndef __OS2_PM__
    HMENU       menu;
    HMENU       sys_menu;

    if( insertedItems ) {
        SetSystemMenu( hwnd );
    } else {
        getMenuBitmaps();
        menu = _wpi_getmenu( mdiInfo.root );
        insertedItems = TRUE;
        sys_menu = generateSystemMenu( hwnd );
        if( sys_menu != NULL ) {
            InsertMenu( menu, 0, MF_POPUP | MF_BYPOSITION | MF_BITMAP,
                        (UINT_PTR)sys_menu, (LPVOID)closeBitmap );
        } else {
            InsertMenu( menu, 0, MF_BYPOSITION | MF_BITMAP, -1, (LPVOID)closeBitmap );
        }
        InsertMenu( menu, -1, MF_HELP | MF_BYPOSITION | MF_BITMAP, SC_RESTORE,
                    (LPVOID)restoreBitmap );
        _wpi_drawmenubar( mdiInfo.root );
    }
#else
    hwnd = hwnd;
#endif

} /* setMaximizedMenuConfig */

/*
 * MDIClearMaximizedMenuConfig - done with maximized menu configuration
 */
void MDIClearMaximizedMenuConfig( void )
{
    updatedMenu = FALSE;
    if( closeBitmap != NULLHANDLE ) {
        _wpi_deletebitmap( closeBitmap );
        closeBitmap = NULLHANDLE;
    }
    if( restoreBitmap != NULLHANDLE ) {
        _wpi_deletebitmap( restoreBitmap );
        restoreBitmap = NULLHANDLE;
    }
    if( restoredBitmap != NULLHANDLE ) {
        _wpi_deletebitmap( restoredBitmap );
        restoredBitmap = NULLHANDLE;
    }

} /* MDIClearMaximizedMenuConfig */

/*
 * deleteMaximizedMenuConfig - delete the maximized menu configuration
 */
static void deleteMaximizedMenuConfig( void )
{
    HMENU       root_menu;
    int         count;

    if( !insertedItems ) {
        return;
    }
    insertedItems = FALSE;
    root_menu = _wpi_getmenu( mdiInfo.root );
    _wpi_deletemenu( root_menu, 0, TRUE );
    count = (int)_wpi_getmenuitemcount( root_menu );
    _wpi_deletemenu( root_menu, count-1, TRUE );
    _wpi_drawmenubar( mdiInfo.root );

} /* deleteMaximizedMenuConfig */

/*
 * MDISetMainWindowTitle - set the title of the main window
 */
void MDISetMainWindowTitle( char *fname )
{
    char        buff[MAX_STR];

    if( childrenMaximized ) {
        sprintf( buff, "%s - %s", mdiInfo.main_name, fname );
        _wpi_setwindowtext( mdiInfo.root, buff );
    }

} /* MDISetMainWindowTitle */

/*
 * MDIIsMaximized - test if we are currently maximized
 */
int MDIIsMaximized( void )
{
    return( childrenMaximized );

} /* MDIIsMaximized */

/*
 * MDIIsWndMaximized - test is given window is currently maximized
 */
int MDIIsWndMaximized( HWND hwnd )
{
    mdi_data    *md;

    md = GET_WND_MDI_DATA( hwnd );
    return( md->curr_state == STATE_MAX );

} /* MDIIsWndMaximized */

/*
 * MDIUpdatedMenu - test if we have updated (added to) the menus
 */
int MDIUpdatedMenu( void )
{
    return( updatedMenu );

} /* MDIUpdatedMenu */

/*
 * MDISetMaximized - set the current maximized state
 */
void MDISetMaximized( int setting )
{
    childrenMaximized = setting;

} /* MDISetMaximized */

/*
 * MDITile - do a tile
 */
void MDITile( int is_horz )
{
#if defined( __UNIX__ )
    return;
#else
#ifndef __OS2_PM__
    WORD        tile_how;
#if !defined( __NT__ )
    HANDLE      h;
#if defined( __WINDOWS_386__ )
    LPVOID      TileChildWindows;
    HINDIR      hindir;
#else
    TILECHILDPROC TileChildWindows;
#endif
#endif

    if( childrenMaximized ) {
        return;
    }

    if( is_horz ) {
        tile_how = MDITILE_HORIZONTAL;
    } else {
        tile_how = MDITILE_VERTICAL;
    }

#if !defined( __NT__ )
    h = LoadLibrary( "USER.EXE" );
    if( h == NULL ) {
        return;
    }
    TileChildWindows = (LPVOID)GetProcAddress( h, "TileChildWindows" );
    if( TileChildWindows == NULL ) {
        return;
    }
#if defined( __WINDOWS_386__ )
    hindir = GetIndirectFunctionHandle( TileChildWindows, INDIR_WORD, INDIR_WORD, INDIR_ENDLIST );
    InvokeIndirectFunction( hindir, mdiInfo.container, tile_how );
    free( hindir );
#else
    TileChildWindows( mdiInfo.container, tile_how );
#endif
    FreeLibrary( h );
#else
    TileChildWindows( mdiInfo.container, tile_how );
#endif
#else
    is_horz = is_horz;
#endif
#endif

} /* MDITile */

/*
 * MDICascade - do a cascade
 */
void MDICascade( void )
{
#if defined( __UNIX__ )
    return;
#else
#ifndef __OS2_PM__
#if !defined( __NT__ )
    HANDLE      h;
#if defined( __WINDOWS_386__ )
    LPVOID      CascadeChildWindows;
    HINDIR      hindir;
#else
    CASCADECHILDPROC CascadeChildWindows;
#endif
#endif

    if( childrenMaximized ) {
        return;
    }

#if !defined( __NT__ )
    h = LoadLibrary( "USER.EXE" );
    if( h == NULL ) {
        return;
    }
    CascadeChildWindows = (LPVOID)GetProcAddress( h, "CascadeChildWindows" );
    if( CascadeChildWindows == NULL ) {
        return;
    }
#if defined( __WINDOWS_386__ )
    hindir = GetIndirectFunctionHandle( CascadeChildWindows, INDIR_WORD, INDIR_WORD, INDIR_ENDLIST );
    InvokeIndirectFunction( hindir, mdiInfo.container, 0 );
    free( hindir );
#else
    CascadeChildWindows( mdiInfo.container, 0 );
#endif
    FreeLibrary( h );
#else
    CascadeChildWindows( mdiInfo.container, 0 );
#endif
#endif
#endif

} /* MDICascade */

/*
 * MDINewWindow - a new MDI window has been created
 */
int MDINewWindow( HWND hwnd )
{
    mdi_data    *md;

    md = (mdi_data *)MemAlloc( sizeof( mdi_data ) );
    if( md == NULL ) {
        return( FALSE );
    }
    md->hwnd = hwnd;
    SET_WND_MDI_DATA( hwnd, (LONG_PTR)md );
    if( mdiHead == NULL ) {
        mdiHead = mdiTail = md;
    } else {
        mdiTail->next = md;
        mdiTail = md;
    }

    if( childrenMaximized ) {
        doMaximize( hwnd );
        mdiInfo.set_window_title( hwnd );
    }
    return( TRUE );

} /* MDINewWindow */

/*
 * finiWindow - an MDI window is done
 */
static void finiWindow( HWND hwnd )
{
    mdi_data    *curr;
    mdi_data    *prev;

    curr = mdiHead;
    prev = NULL;
    while( curr != NULL ) {
        if( curr->hwnd == hwnd ) {
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    if( curr == NULL ) {
        return;
    }
    if( prev != NULL ) {
        prev->next = curr->next;
    }
    if( curr == mdiHead ) {
        mdiHead = curr->next;
    }
    if( curr == mdiTail ) {
        mdiTail = prev;
    }
    MemFree( curr );

} /* finiWindow */

#ifndef __OS2_PM__

/*
 * processSysCommand - process a WM_SYSCOMMAND message for an MDI child
 */
static int processSysCommand( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                              WPI_PARAM2 lparam, WPI_MRESULT *lrc )
{
    mdi_data    *md;

    md = GET_WND_MDI_DATA( hwnd );
    switch( LOWORD( wparam ) & 0xfff0 ) {
    case SC_RESTORE:
        *lrc = _wpi_defwindowproc( hwnd, msg, wparam, lparam );
        if( md->orig_state == STATE_MAX ) {
            md->orig_state = STATE_NORMAL;
            doMaximizeAll( hwnd );
        } else {
            doRestoreAll();
        }
        return( TRUE );
    case SC_MAXIMIZE:
        doMaximizeAll( hwnd );
        mdiInfo.set_window_title( hwnd );
        *lrc = 0;
        return( TRUE );
    case SC_CLOSE:
        *lrc = _wpi_defwindowproc( hwnd, msg, wparam, lparam );
        return( TRUE );
    case SC_MINIMIZE:
        if( md->curr_state == STATE_MAX ) {
            doRestoreAll();
            md->orig_state = STATE_MAX;
        }
        *lrc = _wpi_defwindowproc( hwnd, msg, wparam, lparam );
        return( TRUE );
    case SC_NEXTWINDOW:
        if( md->next == NULL ) {
            md = mdiHead;
        } else {
            md = md->next;
        }
        /* NOTE:  we are sending WM_SETFOCUS, for lack of anything
         *        better (WM_CHILDACTIVATE maybe?)
         */
        _wpi_sendmessage( md->hwnd, WM_SETFOCUS, 0, 0L );
        return( TRUE );
    }
    return( FALSE );

} /* processSysCommand */

#if 0

/*
 * tryContainerScrollBars - try to add container scroll bars
 */
static void tryContainerScrollBars( void )
{
    WPI_RECT    r;

    if( !haveMinChildRect || childrenMaximized ) {
        return;
    }
    _wpi_getwindowrect( mdiInfo.container, &r );
    if( minChildRect.top < r.top || minChildRect.bottom > r.bottom ) {
        _wpi_setscrollrange( mdiInfo.container, SB_VERT, minChildRect.top,
                             minChildRect.bottom, FALSE );
    } else {
        _wpi_setscrollrange( mdiInfo.container, SB_VERT, 1, 1, FALSE );
    }
    if( minChildRect.left < r.left || minChildRect.right > r.right ) {
        _wpi_setscrollrange( mdiInfo.container, SB_HORZ, minChildRect.left,
                             minChildRect.right, FALSE );
    } else {
        _wpi_setscrollrange( mdiInfo.container, SB_HORZ, 1, 1, FALSE );
    }

} /* tryContainerScrollBars */

/*
 * newChildPositions - handle re-location of a child window
 */
static void newChildPositions( void )
{
    mdi_data    *curr;
    RECT        r;
    RECT        orig;

    if( childrenMaximized ) {
        return;
    }

    curr = mdiHead;
    memset( &minChildRect, 0, sizeof( RECT ) );
    while( curr != NULL ) {
        _wpi_getwindowrect( curr->hwnd, &r );
        orig = minChildRect;
        UnionRect( &minChildRect, &orig, &r );
        haveMinChildRect = TRUE;
        curr = curr->next;
    }
    tryContainerScrollBars();

} /* newChildPositions */

/*
 * MDIResizeContainer - handle the resizing of the container window
 */
void MDIResizeContainer( void )
{
    tryContainerScrollBars();

} /* MDIResizeContainer */

#endif

#endif

/*
 * MDIHitClose - check if close bitmap was hit on menu of main window
 */
int MDIHitClose( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
#ifndef __OS2_PM__
    msg = msg;
    if( childrenMaximized && wparam == HTMENU ) {
        if( hitSysMenu( hwnd, lparam ) ) {
            _wpi_postmessage( currentWindow, WM_SYSCOMMAND, SC_CLOSE, 0L );
            return( TRUE );
        }
    }
#else
    hwnd = hwnd;
    msg = msg;
    wparam = wparam;
    lparam = lparam;
#endif
    return( FALSE );

} /* MDIHitClose */

#ifndef __OS2_PM__

/*
 * CheckForMessage - check for a WM_COMMAND message that needs to be
 *                   sent to the maximized window
 */
static bool CheckForMessage( HMENU menu, HWND currentWindow,
                             WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    int         num;
    int         i;
    UINT        id;
    UINT        flags;

    if( menu != NULL ) {
        num = (int)_wpi_getmenuitemcount( menu );
        for( i = 0; i < num; i++ ) {
            flags = GetMenuState( menu, i, MF_BYPOSITION );
            if( flags & MF_POPUP ) {
                if( CheckForMessage( GetSubMenu( menu, i ), currentWindow,
                                     wparam, lparam ) ) {
                    return( TRUE );
                }
            } else {
                id = GetMenuItemID( menu, i );
                if( id == wparam ) {
                    _wpi_sendmessage( currentWindow, WM_COMMAND, wparam, lparam );
                    return( TRUE );
                }
            }
        }
    }
    return( FALSE );

} /* CheckForMessage */

#endif

/*
 * MDIIsSysCommand - see if WM_COMMAND is really a WM_SYSCOMMAND
 */
int MDIIsSysCommand( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
#ifndef __OS2_PM__
    HMENU       sys_menu;

    hwnd = hwnd;
    msg = msg;
    if( childrenMaximized ) {
        if( LOWORD( wparam ) >= 0xF000 ) {
            _wpi_sendmessage( currentWindow, WM_SYSCOMMAND, wparam, lparam );
            return( TRUE );
        } else {
            sys_menu = GetSystemMenu( currentWindow, FALSE );
            CheckForMessage( sys_menu, currentWindow, wparam, lparam );
        }
    }
#else
    hwnd = hwnd;
    msg = msg;
    wparam = wparam;
    lparam = lparam;
#endif
    return( FALSE );

} /* MDIIsSysCommand */

/*
 * fixSystemMenu
 */
static void fixSystemMenu( HWND hwnd )
{
    modifyChildSystemMenu( _wpi_getsystemmenu( hwnd ) );

} /* fixSystemMenu */

/*
 * MDIChildHandleMessage - handle messages for MDI child windows
 */
int MDIChildHandleMessage( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                           WPI_PARAM2 lparam, WPI_MRESULT *lrc )
{
#ifndef __OS2_PM__
    bool        iconic;
#endif

    wparam = wparam;
    lparam = lparam;
    lrc    = lrc;

    switch( msg ) {
    case WM_CREATE:
        fixSystemMenu( hwnd );
        break;
    case WM_SIZE:
    case WM_MOVE:
//      newChildPositions();
        break;
#ifndef __OS2_PM__
    case WM_SYSCOMMAND:
        return( processSysCommand( hwnd, msg, wparam, lparam, lrc ) );
#endif
    case WM_DESTROY:
        finiWindow( hwnd );
        if( childrenMaximized && mdiHead == NULL ) {
            doRestoreAll();
            childrenMaximized = TRUE;
        }
        if( currentWindow == hwnd ) {
            currentWindow = NULLHANDLE;
        }
        break;
    case WM_SETFOCUS:
        currentWindow = hwnd;
        if( childrenMaximized ) {
            mdiInfo.set_window_title( hwnd );
            setMaximizedMenuConfig( hwnd );
        }
        break;
#ifndef __OS2_PM__
    case WM_COMMAND:
        if( childrenMaximized && LOWORD( wparam ) >= 0xF000 ) {
            _wpi_sendmessage( currentWindow, WM_SYSCOMMAND, wparam, lparam );
            return( TRUE );
        }
        break;
    case WM_NCLBUTTONDBLCLK:
        iconic = _wpi_isiconic( currentWindow );
        if( !childrenMaximized && (wparam == HTCAPTION) && iconic ) {
            _wpi_sendmessage( currentWindow, WM_SYSCOMMAND, SC_MAXIMIZE, 0L );
            *lrc = 0;
            return( TRUE );
        }
        break;
#endif
    }
    return( FALSE );

} /* MDIChildHandleMessage */

/*
 * MDIContainerResized - resize MDI windows when container resized, if we're
 *                       maximized
 */
void MDIContainerResized( void )
{
    mdi_data    *md;
    WPI_RECT    r;
    WPI_RECTDIM left;
    WPI_RECTDIM top;
    WPI_RECTDIM right;
    WPI_RECTDIM bottom;

    if( MDIIsMaximized() ) {
        _wpi_getwindowrect( mdiInfo.container, &r );
        _wpi_getrectvalues( r, &left, &top, &right, &bottom );
        md = mdiHead;
        while( md != NULL ) {
            if( _wpi_isiconic( md->hwnd ) ) {
                _wpi_movewindow( md->hwnd, 0, 0, right - left + 1,
                                 bottom - top + 1, TRUE );
            }
            md = md->next;
        }
    }

} /* MDIContainerResized */
