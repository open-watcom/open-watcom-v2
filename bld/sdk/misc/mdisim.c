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


#define OEMRESOURCE
#include <windows.h>
#include <string.h>
#include "mdisim.h"
#ifdef __WINDOWS_386__
#include <stdio.h>
#include <malloc.h>
#endif

typedef char    bool;

#define MAX_STR         256
#define STATE_NORMAL    0x00
#define STATE_MAX       0x01
#define CLOSE_BITMAP_X  18
#define CLOSE_BITMAP_Y  18

typedef struct mdi_data {
    struct mdi_data     *next;
    HWND                hwnd;
    RECT                orig_size;
    char                orig_state;
    char                curr_state;
} mdi_data;

extern LPVOID   MemAlloc( UINT );
extern void     MemFree( LPVOID );

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
//static RECT   minChildRect;
//static char   haveMinChildRect;

#define MDI_DATA_FROM_HWND( hwnd ) ((mdi_data *) GetWindowLong( hwnd, mdiInfo.data_off ))

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
        if( currentWindow != NULL ) {
            mdiInfo.set_window_title( currentWindow );
        }
    } else {
        DrawMenuBar( mdiInfo.root );
    }

} /* MDIInitMenu */

void MDISetOrigSize( HWND hwnd, RECT *rect )
{
    mdi_data    *md;

    md = MDI_DATA_FROM_HWND( hwnd );

    CopyRect( &md->orig_size, rect );
}

/*
 * doMaximize - handle maximizing an edit window
 */
static void doMaximize( HWND hwnd )
{
    DWORD               style;
    mdi_data            *md;
    RECT                r;
    WINDOWPLACEMENT     place;
    bool                iconic;

    setMaximizedMenuConfig( hwnd );

    md = MDI_DATA_FROM_HWND( hwnd );

    if( mdiInfo.start_max_restore != NULL ) {
        mdiInfo.start_max_restore( hwnd );
    }

    iconic = IsIconic( hwnd );
    if( iconic ) {
        place.length = sizeof( WINDOWPLACEMENT );
        GetWindowPlacement( hwnd, &place );
        CopyRect( &md->orig_size, &place.rcNormalPosition );
    } else {
        GetWindowRect( hwnd, &md->orig_size );
    }
    md->orig_state = md->curr_state;
    md->curr_state = STATE_MAX;

    if( mdiInfo.set_style != NULL ) {
        (mdiInfo.set_style)( hwnd, TRUE );
    } else {
        style = GetWindowLong( hwnd, GWL_STYLE );
        style &= ~mdiInfo.reg_style;
        style |= mdiInfo.max_style;
        SetWindowLong( hwnd, GWL_STYLE, style );
    }
    SetScrollRange( hwnd, SB_VERT, 1, 1, TRUE );
    SetScrollRange( hwnd, SB_HORZ, 1, 1, TRUE );
    GetWindowRect( mdiInfo.container, &r );

    if( !iconic ) {
        OffsetRect( &md->orig_size, -r.left, -r.top );
    }

    OffsetRect( &r, -r.left, -r.top );
    r.right++;
    r.bottom++;

    if( iconic ) {
        CopyRect( &place.rcNormalPosition, &r );
        place.showCmd = SW_SHOWNORMAL;
        SetWindowPlacement( hwnd, &place );
    } else {
        MoveWindow( hwnd, r.left, r.top, r.right, r.bottom, TRUE );
    }

    if( mdiInfo.end_max_restore != NULL ) {
        mdiInfo.end_max_restore( hwnd );
    }

    InvalidateRect( hwnd, NULL, NULL );

} /* doMaximize */

/*
 * doRestore - handle restoring an edit window
 */
static void doRestore( HWND hwnd )
{
    DWORD       style;
    mdi_data    *md;

    md = MDI_DATA_FROM_HWND( hwnd );

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
        style = GetWindowLong( hwnd, GWL_STYLE );
        style &= ~mdiInfo.max_style;
        style |= mdiInfo.reg_style;
        SetWindowLong( hwnd, GWL_STYLE, style );
    }

    SetScrollRange( hwnd, SB_VERT, 1, 1, TRUE );
    SetScrollRange( hwnd, SB_HORZ, 1, 1, TRUE );
    // unnecessarily messy as the MoveWindow repaints
    //UpdateWindow( hwnd );
    MoveWindow( hwnd, md->orig_size.left, md->orig_size.top,
                md->orig_size.right - md->orig_size.left,
                md->orig_size.bottom - md->orig_size.top, TRUE );
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
    SetWindowText( mdiInfo.root, mdiInfo.main_name );
    childrenMaximized = FALSE;
    md = mdiHead;
    while( md != NULL ) {
        doRestore( md->hwnd );
        md = md->next;
    }
    MDIClearMaximizedMenuConfig();
    deleteMaximizedMenuConfig();

} /* doRestoreAll */

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
                if( !IsIconic( md->hwnd ) ) {
                    doMaximize( md->hwnd );
                }
            }
            md = md->next;
        }
    }

    SetSystemMenu( first );

} /* doMaximizeAll */

/*
 * getMenuBitmaps - load restore/restored bitmaps, and
 *                  get a bitmap for the close gadget (ack pft)
 */
static void getMenuBitmaps( void )
{
    #if 0
        HBITMAP hbmp;
        BITMAP  bmp;
        WORD    new_real_width;
        WORD    old_real_width;
        WORD    new_width;
        LPSTR   bits;
        LPSTR   new_bits;
        LPSTR   old_bits;
        LPSTR   bmp_bits;
        WORD    size;
        int             x,y;
    #endif

    if( restoreBitmap == NULL ) {
        restoreBitmap = LoadBitmap( (HANDLE) NULL, MAKEINTRESOURCE( OBM_RESTORE ) );
    }
    if( restoredBitmap == NULL ) {
        restoredBitmap = LoadBitmap( (HANDLE) NULL, MAKEINTRESOURCE( OBM_RESTORED ) );
    }

    if( closeBitmap == NULL ) {
        closeBitmap = LoadBitmap( mdiInfo.hinstance, "CLOSEBMP" );
    }
    updatedMenu = TRUE;

} /* getMenuBitmaps */

static HMENU DuplicateMenu( HMENU orig )
{
    int         num;
    UINT        id;
    UINT        flags;
    char        name[MAX_STR];
    int         i;
    HMENU       copy;
    HMENU       sub;

    if( orig != NULL ) {
        copy = CreatePopupMenu();
        if( copy == NULL ) {
            return( NULL );
        }
        num = GetMenuItemCount( orig );
        for( i = 0; i < num; i++ ) {
            flags = GetMenuState( orig, i, MF_BYPOSITION );
            if( flags & MF_SEPARATOR ) {
                AppendMenu( copy, flags, 0, 0 );
            } else if( flags & MF_POPUP ) {
                sub = DuplicateMenu( GetSubMenu( orig, i ) );
                GetMenuString( orig, i, name, MAX_STR - 1, MF_BYPOSITION );
                AppendMenu( copy, flags, (UINT)sub, name );
            } else {
                id = GetMenuItemID( orig, i );
                GetMenuString( orig, i, name, MAX_STR - 1, MF_BYPOSITION );
                AppendMenu( copy, flags, id, name );
            }
        }
    }
    return( copy );
}

/*
 * generateSystemMenu - generate a copy of the system menu for given window
 */
static HMENU generateSystemMenu( HWND hwnd )
{
    HMENU       sys_menu;

    sys_menu = GetSystemMenu( hwnd, FALSE );
    if( sys_menu != NULL ) {
        return( DuplicateMenu( sys_menu ) );
    } else {
        return( NULL );
    }

} /* generateSystemMenu */

/*
 * modifyChildSystemMenu - adjust system menu to make it a child system menu
 */
static HMENU modifyChildSystemMenu( HMENU sys_menu )
{
    if( sys_menu == NULL ) {
        return( NULL );
    }

    /* fix hotkey designation for close
    */
    ModifyMenu( sys_menu, SC_CLOSE, MF_BYCOMMAND | MF_STRING,
                                    SC_CLOSE, "&Close\tCtrl+F4" );

    /* remove task switch option
    */
    DeleteMenu( sys_menu, SC_TASKLIST, MF_BYCOMMAND );

    /* add next window option
    */
    AppendMenu( sys_menu, MF_STRING, SC_NEXTWINDOW, "Nex&t\tCtrl+F6" );

    return( sys_menu );
}

/*
 * SetSystemMenu -- make the system menu showing belong to the current window
 */
void SetSystemMenu( HWND hwnd )
{
    HMENU       sys_menu;
    HMENU       menu;

    sys_menu = NULL;
    if( hwnd != NULL ) {
        sys_menu = generateSystemMenu( hwnd );
    }
    menu = GetMenu( mdiInfo.root );
    getMenuBitmaps();
    if( sys_menu != NULL ) {
        ModifyMenu( menu, 0, MF_POPUP | MF_BYPOSITION | MF_BITMAP,
                    (UINT) sys_menu, (LPVOID)closeBitmap );
    } else {
        ModifyMenu( menu, 0, MF_BYPOSITION | MF_BITMAP, -1,
                    (LPVOID)closeBitmap );
    }
    DrawMenuBar( mdiInfo.root );
}

/*
 * hitSysMenu - check if a specified point hit the system menu
 */
static bool hitSysMenu( HWND hwnd, LONG lparam )
{
    RECT        r;
    POINT       pt;

    GetWindowRect( hwnd, &r );
    r.top += GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CYFRAME );
    r.left += GetSystemMetrics( SM_CXFRAME );
    r.bottom = r.top + CLOSE_BITMAP_Y;
    r.right = r.left + CLOSE_BITMAP_X;
    pt.x = LOWORD( lparam );
    pt.y = HIWORD( lparam );
    return( PtInRect( &r, pt ) );

} /* hitSysMenu */

#if 0
/*
 * HitRestoreButton - check if a specified point hit the restore button
 */
bool HitRestoreButton( HWND hwnd, LONG lparam )
{
    RECT        r;

    GetWindowRect( hwnd, &r );
    r.top += GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CYFRAME );
    r.bottom = r.top + CLOSE_BITMAP_Y;
    r.right -= GetSystemMetrics( SM_CXFRAME );
    r.left = r.right - CLOSE_BITMAP_X;
    return( PtInRect( &r, MAKEPOINT( lparam ) ) );

} /* HitRestoreButton */

/*
 * SetRestoreBitmap - set the bitmap on our restore menu item
 */
void SetRestoreBitmap( bool pressed )
{
    HMENU       menu;

    menu = GetMenu( mdiInfo.root );
    if( pressed ) {
        ModifyMenu( menu, 7, MF_BYPOSITION | MF_BITMAP | MF_HELP,
                        SC_RESTORE, (LPVOID) restoredBitmap );
    } else {
        ModifyMenu( menu, 7, MF_BYPOSITION | MF_BITMAP | MF_HELP,
                        SC_RESTORE, (LPVOID) restoreBitmap );
    }
    DrawMenuBar( mdiInfo.root );

} /* SetRestoreBitmap */
#endif

/*
 * setMaximizedMenuConfig - set up main menu in the maximized configuration
 */
static void setMaximizedMenuConfig( HWND hwnd )
{
    HMENU       menu;
    HMENU       sys_menu;

    if( insertedItems ) {
        SetSystemMenu( hwnd );
    } else {
        getMenuBitmaps();
        menu = GetMenu( mdiInfo.root );
        insertedItems = TRUE;
        sys_menu = generateSystemMenu( hwnd );
        if( sys_menu != NULL ) {
            InsertMenu( menu, 0, MF_POPUP | MF_BYPOSITION | MF_BITMAP,
                        (UINT) sys_menu, (LPVOID) closeBitmap );
        } else {
            InsertMenu( menu, 0, MF_BYPOSITION | MF_BITMAP, -1,
                        (LPVOID) closeBitmap );
        }
        InsertMenu( menu, -1, MF_HELP | MF_BYPOSITION | MF_BITMAP, SC_RESTORE,
                    (LPVOID) restoreBitmap );
        DrawMenuBar( mdiInfo.root );
    }

} /* setMaximizedMenuConfig */

/*
 * MDIClearMaximizedMenuConfig - done with maximized menu configuration
 */
void MDIClearMaximizedMenuConfig( void )
{
    updatedMenu = FALSE;
    if( closeBitmap != NULL ) {
        DeleteObject( closeBitmap );
        closeBitmap = NULL;
    }
    if( restoreBitmap != NULL ) {
        DeleteObject( restoreBitmap );
        restoreBitmap = NULL;
    }
    if( restoredBitmap != NULL ) {
        DeleteObject( restoredBitmap );
        restoredBitmap = NULL;
    }

} /* MDIClearMaximizedMenuConfig */

/*
 * deleteMaxinimizedMenuConfig - delete the maximized menu configuration
 */
static void deleteMaximizedMenuConfig( void )
{
    HMENU       root_menu;

    if( !insertedItems ) {
        return;
    }
    insertedItems = FALSE;
    root_menu = GetMenu( mdiInfo.root );
    DeleteMenu( root_menu, 0, MF_BYPOSITION );
    DeleteMenu( root_menu, GetMenuItemCount( root_menu )-1, MF_BYPOSITION );
    DrawMenuBar( mdiInfo.root );
}

/*
 * MDISetMainWindowTitle - set the title of the main window
 */
void MDISetMainWindowTitle( char *fname )
{
    char        buff[MAX_STR];

    if( childrenMaximized ) {
        wsprintf( buff, "%s - %s", mdiInfo.main_name, fname );
        SetWindowText( mdiInfo.root, buff );
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
 * MDIIsWndMaximized -- test is given window is currently maximized
 */
int MDIIsWndMaximized( HWND hwnd )
{
    mdi_data    *md;

    md = MDI_DATA_FROM_HWND( hwnd );
    return( md->curr_state == STATE_MAX );

} /* MDIIsWndMaximized */

/*
 * MDIUpdatedMenu - test if we have updated ( added to ) the menus
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
    WORD        tile_how;
    #ifndef __NT__
        HANDLE  h;
        #ifdef __WINDOWS_386__
            LPVOID      TileChildWindows;
            HINDIR      hindir;
        #else
            int (FAR PASCAL* TileChildWindows)( HWND parent, WORD action );
        #endif
    #else
        extern int FAR PASCAL TileChildWindows( HWND parent, WORD action );
    #endif

    if( childrenMaximized ) {
        return;
    }

    if( is_horz ) {
        tile_how = MDITILE_HORIZONTAL;
    } else {
        tile_how = MDITILE_VERTICAL;
    }

    #ifndef __NT__
        h = LoadLibrary( "USER.EXE" );
        if( h == NULL ) {
            return;
        }
        TileChildWindows = (LPVOID) GetProcAddress( h, "TileChildWindows" );
        if( TileChildWindows == NULL ) {
            return;
        }
        #ifdef __WINDOWS_386__
            hindir = GetIndirectFunctionHandle( TileChildWindows, INDIR_WORD,
                                    INDIR_WORD, INDIR_ENDLIST );
            InvokeIndirectFunction( hindir, mdiInfo.container, tile_how );
            free( hindir );
        #else
            TileChildWindows( mdiInfo.container, tile_how );
        #endif
        FreeLibrary( h );
    #else
        TileChildWindows( mdiInfo.container, tile_how );
    #endif

} /* MDITile */

/*
 * MDICascade - do a cascade
 */
void MDICascade( void )
{
    #ifndef __NT__
        HANDLE  h;
        #ifdef __WINDOWS_386__
            LPVOID      CascadeChildWindows;
            HINDIR      hindir;
        #else
            int (FAR PASCAL* CascadeChildWindows)( HWND parent, WORD action );
        #endif
    #else
        extern int FAR PASCAL CascadeChildWindows( HWND parent, WORD action );
    #endif


    if( childrenMaximized ) {
        return;
    }

    #ifndef __NT__
        h = LoadLibrary( "USER.EXE" );
        if( h == NULL ) {
            return;
        }
        CascadeChildWindows = (LPVOID) GetProcAddress( h, "CascadeChildWindows" );
        if( CascadeChildWindows == NULL ) {
            return;
        }
        #ifdef __WINDOWS_386__
            hindir = GetIndirectFunctionHandle( CascadeChildWindows, INDIR_WORD,
                                    INDIR_WORD, INDIR_ENDLIST );
            InvokeIndirectFunction( hindir, mdiInfo.container, 0 );
            free( hindir );
        #else
            CascadeChildWindows( mdiInfo.container, 0 );
        #endif
        FreeLibrary( h );
    #else
        CascadeChildWindows( mdiInfo.container, 0 );
    #endif

} /* MDICascade */

/*
 * MDINewWindow - a new MDI window has been created
 */
int MDINewWindow( HWND hwnd )
{
    mdi_data    *md;

    md = (mdi_data *) MemAlloc( sizeof( mdi_data ) );
    if( md == NULL ) {
        return( FALSE );
    }
    md->hwnd = hwnd;
    SetWindowLong( hwnd, mdiInfo.data_off, (LONG) md );
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
 * finiWindow - an mdi window is done
 */
static void finiWindow( HWND hwnd )
{
    mdi_data    *curr,*prev;

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

/*
 * processSysCommand - process a WM_SYSCOMMAND message for an MDI child
 */
static int processSysCommand( HWND hwnd, UINT msg, UINT wparam, LONG lparam,
                                LONG *lrc )
{
    mdi_data    *md;

    md = MDI_DATA_FROM_HWND( hwnd );
    switch( LOWORD( wparam ) & 0xfff0 ) {
    case SC_RESTORE:
        *lrc = DefWindowProc( hwnd, msg, wparam, lparam );
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
        *lrc = DefWindowProc( hwnd, msg, wparam, lparam );
        return( TRUE );
    case SC_MINIMIZE:
        if( md->curr_state == STATE_MAX ) {
            doRestoreAll();
            md->orig_state = STATE_MAX;
        }
        *lrc = DefWindowProc( hwnd, msg, wparam, lparam );
        return( TRUE );
    case SC_NEXTWINDOW:
        if( md->next == NULL ) {
            md = mdiHead;
        } else {
            md = md->next;
        }
        /* note:  we are sending WM_SETFOCUS, for lack of anything
         *        better (WM_CHILDACTIVATE maybe?)
         */
        SendMessage( md->hwnd, WM_SETFOCUS, 0, 0L );
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
    RECT        r;

    if( !haveMinChildRect || childrenMaximized ) {
        return;
    }
    GetWindowRect( mdiInfo.container, &r );
    if( minChildRect.top < r.top || minChildRect.bottom > r.bottom ) {
        SetScrollRange( mdiInfo.container, SB_VERT, minChildRect.top,
                        minChildRect.bottom, FALSE );
    } else {
        SetScrollRange( mdiInfo.container, SB_VERT, 1, 1, FALSE );
    }
    if( minChildRect.left < r.left || minChildRect.right > r.right ) {
        SetScrollRange( mdiInfo.container, SB_HORZ, minChildRect.left,
                        minChildRect.right, FALSE );
    } else {
        SetScrollRange( mdiInfo.container, SB_HORZ, 1, 1, FALSE );
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
        GetWindowRect( curr->hwnd, &r );
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

/*
 * MDIHitClose - check if close bitmap was hit on menu of main window
 */
int MDIHitClose( HWND hwnd, UINT msg, UINT wparam, LONG lparam )
{
    msg = msg;
    if( childrenMaximized && wparam == HTMENU ) {
        if( hitSysMenu( hwnd, lparam ) ) {
            PostMessage( currentWindow, WM_SYSCOMMAND, SC_CLOSE, 0L );
            return( TRUE );
        }
    }
    return( FALSE );

} /* MDIHitClose */

/*
 * CheckForMessage -- check for a WM_COMMAND message that needs to be
 *                    sent to the maximized window
 */

static bool CheckForMessage( HMENU menu, HWND currentWindow,
                             UINT wparam, LONG lparam )
{
    int         num;
    int         i;
    UINT        id;
    UINT        flags;

    if( menu != NULL ) {
        num = GetMenuItemCount( menu );
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
                    SendMessage( currentWindow, WM_COMMAND, wparam, lparam );
                    return( TRUE );
                }
            }
        }
    }
    return( FALSE );

} /* CheckForMessage */

/*
 * MDIIsSysCommand - see if WM_COMMAND is really a WM_SYSCOMMAND
 */
int MDIIsSysCommand( HWND hwnd, UINT msg, UINT wparam, LONG lparam )
{
    HMENU       sys_menu;

    hwnd = hwnd;
    msg = msg;
    if( childrenMaximized ) {
        if( LOWORD( wparam ) >= 0xF000 ) {
            SendMessage( currentWindow, WM_SYSCOMMAND, wparam, lparam );
            return( TRUE );
        } else {
            sys_menu = GetSystemMenu( currentWindow, FALSE );
            CheckForMessage( sys_menu, currentWindow, wparam, lparam );
        }
    }
    return( FALSE );

} /* MDIIsSysCommand */

static void fixSystemMenu( HWND hwnd )
{
    modifyChildSystemMenu( GetSystemMenu( hwnd, FALSE ) );
}

/*
 * MDIChildHandleMessage - handle messages for MDI child windows
 */
int MDIChildHandleMessage( HWND hwnd, UINT msg, UINT wparam, LONG lparam,
                                        LONG *lrc )
{
    switch( msg ) {
    case WM_CREATE:
        fixSystemMenu( hwnd );
        break;
    case WM_SIZE:
    case WM_MOVE:
//      newChildPositions();
        break;
    case WM_SYSCOMMAND:
        return( processSysCommand( hwnd, msg, wparam, lparam, lrc ) );
    case WM_DESTROY:
        finiWindow( hwnd );
        if( childrenMaximized && mdiHead == NULL ) {
            doRestoreAll();
            childrenMaximized = TRUE;
        }
        if( currentWindow == hwnd ) {
            currentWindow = NULL;
        }
        break;
    case WM_SETFOCUS:
        currentWindow = hwnd;
        if( childrenMaximized ) {
            mdiInfo.set_window_title( hwnd );
            setMaximizedMenuConfig( hwnd );
        }
        break;
    case WM_COMMAND:
        if( childrenMaximized && LOWORD( wparam ) >= 0xF000 ) {
            SendMessage( currentWindow, WM_SYSCOMMAND, wparam, lparam );
            return( TRUE );
        }
        break;
    case WM_NCLBUTTONDBLCLK:
        if( !childrenMaximized && wparam == HTCAPTION &&
            !IsIconic( currentWindow ) ) {
            SendMessage( currentWindow, WM_SYSCOMMAND, SC_MAXIMIZE, 0L );
            *lrc = 0;
            return( TRUE );
        }
        break;
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
    RECT        r;

    if( MDIIsMaximized() ) {
        GetWindowRect( mdiInfo.container, &r );
        md = mdiHead;
        while( md != NULL ) {
            if( !IsIconic( md->hwnd ) ) {
                MoveWindow( md->hwnd, 0, 0, r.right - r.left + 1,
                            r.bottom - r.top + 1, TRUE );
            }
            md = md->next;
        }
    }
} /* MDIContainerResized */
