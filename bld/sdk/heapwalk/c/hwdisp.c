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
* Description:  GUI support routines for heap walker.
*
****************************************************************************/


#include <string.h>
#include "heapwalk.h"
#include "jdlg.h"


static FARPROC  DialProc;
static WORD     DialCount = 0;

static unsigned ColorCount( WORD bitcount, WORD clrused ) {

    unsigned    color_count;

    if( bitcount == 1 ) {
        color_count = 2;
    } else if( clrused == 0 ) {
        color_count = 1 << bitcount;
        if( bitcount == 24 ) color_count = 0;
    } else {
        color_count = clrused;
    }
    return( color_count );
} /* ColorCount */

static HWND MkDisplayWin( UINT captionid, HWND parent )
{

    HWND        hdl;
    char        *caption;

    caption = HWGetRCString( captionid );
    hdl = CreateWindow(
        ITEM_DISPLAY_CLASS,     /* Window class name */
        caption,                /* Window caption */
        WS_OVERLAPPED|WS_CAPTION
        |WS_SYSMENU|WS_THICKFRAME
        |WS_MAXIMIZEBOX,        /* Window style */
        0,                      /* Initial X position */
        0,                      /* Initial Y position */
        200,                    /* Initial X size */
        100,                    /* Initial Y size */
        parent,                 /* Parent window handle */
        NULL,                   /* Window menu handle */
        Instance,               /* Program instance handle */
        NULL);                  /* Create parameters */
        SetWindowLong( hdl, 0, 0L );
        return( hdl );
}

static HWND ShowBitmapHeapItem( heap_list *hl, HWND parent ) {

    HWND                hdl;
    ResInfo             *info;
    WORD                width;
    WORD                height;
    BITMAPINFOHEADER    *bmheader;

    info = MemAlloc( sizeof( ResInfo ) );
    if( info == NULL ) return( NULL );
    info->type = GD_BITMAP;
    info->hdl = hl->info.ge.hBlock;
    info->res = LockResource( hl->info.ge.hBlock );
    bmheader = (BITMAPINFOHEADER *)info->res;
    if( bmheader->biSize != sizeof( BITMAPINFOHEADER ) ) {
        UnlockResource( info->hdl );
        return( NULL );
    }
    hdl = MkDisplayWin( STR_BITMAP, parent );
    SetWindowLong( hdl, 0, (DWORD)info );
    ShowWindow( hdl, SW_SHOWNORMAL );
    width = bmheader->biWidth;
    width += 2 * GetSystemMetrics( SM_CXFRAME );
    height = bmheader->biHeight;
    height += GetSystemMetrics( SM_CYCAPTION );
    height += GetSystemMetrics( SM_CYFRAME );
    MoveWindow( hdl, 0, 0, width, height, TRUE );
    return( hdl );
}

static HWND ShowIconHeapItem( heap_list *hl, HWND parent ) {

    HWND                hdl;
    ResInfo             *info;
    BITMAPINFOHEADER    *btinfo;
    DWORD               req_size;
    WORD                width;
    WORD                height;

    info = MemAlloc( sizeof( ResInfo ) );
    if( info == NULL ) return( NULL );
    btinfo = (BITMAPINFOHEADER *)LockResource( hl->info.ge.hBlock );
    if( btinfo->biSize == sizeof( BITMAPINFOHEADER ) ) {
        req_size = sizeof( BITMAPINFOHEADER );
        req_size += ColorCount( btinfo->biBitCount, btinfo->biClrUsed ) * sizeof( RGBQUAD );
        req_size += ( btinfo->biWidth * btinfo->biHeight ) / ( 8 / btinfo->biBitCount );
        if( req_size > hl->info.ge.dwBlockSize ) return( NULL );
    }
    info->type = GD_ICON;
    info->hdl = hl->info.ge.hBlock;
    info->res = (char *)btinfo;
    hdl = MkDisplayWin( STR_ICON, parent );
    SetWindowLong( hdl, 0, (DWORD)info );
    width = GetSystemMetrics( SM_CXICON );
    height = GetSystemMetrics( SM_CYICON ) +
             2 * GetSystemMetrics( SM_CYFRAME ) +
             GetSystemMetrics( SM_CYCAPTION );
    SetWindowPos( hdl, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER );
    ShowWindow( hdl, SW_SHOWNORMAL );
    return( hdl );
}

static HWND ShowCursorHeapItem( heap_list *hl, HWND parent ) {

    HWND        hdl;
    ResInfo     *info;
    WORD        width;
    WORD        height;

    info = MemAlloc( sizeof( ResInfo ) );
    if( info == NULL ) return( NULL );
    hdl = MkDisplayWin( STR_CURSOR, parent );
    info->type = GD_CURSOR;
    info->hdl = hl->info.ge.hBlock;
    info->res = LockResource( info->hdl );
    SetWindowLong( hdl, 0, (DWORD)info );
    width = 2 * GetSystemMetrics( SM_CXCURSOR );
    height = GetSystemMetrics( SM_CYCURSOR ) +
             2 * GetSystemMetrics( SM_CYFRAME ) +
             GetSystemMetrics( SM_CYCAPTION );
    SetWindowPos( hdl, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER );
    ShowWindow( hdl, SW_SHOWNORMAL );
    return( hdl );
}

static HWND ShowMenuHeapItem( heap_list *hl, HWND parent ) {

    HWND        hdl;
    HMENU       menu;
    void __far  *ptr;
    ResInfo     *info;

    info = MemAlloc( sizeof( ResInfo ) );
    if( info == NULL ) return( NULL );
    info->type = GD_MENU;
    ptr = LockResource( hl->info.ge.hBlock );
    info->hdl = hl->info.ge.hBlock;
    info->res = ptr;
    info->menu_const = NULL;
    menu = LoadMenuIndirect( ptr );
    hdl = MkDisplayWin( STR_MENU, parent );
    SetWindowLong( hdl, 0, (DWORD)info );
    SetMenu( hdl, menu );
    ShowWindow( hdl, SW_SHOWNORMAL );
    return( hdl );
}

BOOL __export FAR PASCAL DialogDispProc( HWND hwnd, WORD msg, WORD wparam,
                                    DWORD lparam )
{
    wparam = wparam;
    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        DialCount ++;
        break;
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
    case WM_CLOSE:
        DestroyWindow( hwnd );
        break;
    case WM_NCDESTROY:
        DialCount --;
        if( DialCount == 0 ) {
            FreeProcInstance( DialProc );
        }
        return( FALSE ); /* we need to let WINDOWS see this message or
                            fonts are left undeleted */
    default:
        return( FALSE );
    }
    return( TRUE );
}

static HWND ShowDialogHeapItem( heap_list *hl, HWND hwnd ) {

    LPSTR       ptr;
    HWND        dial;
    HWND        hdl;
    heap_list   tmp;
    ResInfo     *info;
    char        *rcstr;

    info = MemAlloc( sizeof( ResInfo ) );
    if( info == NULL ) return( NULL );
    info->type = GD_DIALOG;
    ptr = LockResource( hl->info.ge.hBlock );
    if( ptr == NULL ) {
        rcstr = HWAllocRCString( STR_SHOW );
        RCMessageBox( HeapWalkMainWindow, STR_CANT_LOCK_MEM,
                    rcstr, MB_OK | MB_ICONINFORMATION );
        HWFreeRCString( rcstr );
        return( NULL );
    }
    info->hdl = hl->info.ge.hBlock;
    info->res = ptr;
    /* this window remains invisible.  it is created only to ensure
       that system resource get freed */
    hdl = MkDisplayWin( STR_NADA, hwnd );
    SetWindowLong( hdl, 0, (DWORD)info );
    ShowWindow( hdl, SW_HIDE );
    if( DialCount == 0 ) {
        DialProc = MakeProcInstance( (FARPROC)DialogDispProc, Instance );
    }

    /*
     * A task's Instance is the same as the selector of its DGROUP
     * so use this fact to get an instance handle to display the
     * dialog
     */

    GetDGroupItem( hl->szModule, &tmp );
    dial = CreateDialogIndirect( (HANDLE) tmp.info.ge.hBlock,
                                 ptr, hdl, (DLGPROC)DialProc );
    if( dial == NULL ) {
        rcstr = HWAllocRCString( STR_SHOW );
        RCMessageBox( HeapWalkMainWindow, STR_CANT_CREATE_DLG,
                        rcstr, MB_OK | MB_ICONINFORMATION );
        HWFreeRCString( rcstr );
        return( NULL );
    }
    ShowWindow( dial, SW_SHOW );
    return( dial );
}


static HPALETTE CreateDIBPalette( BITMAPINFO *info )
{
    unsigned            num_colours, i;
    LOGPALETTE          *palette;
    HPALETTE            palette_handle;
    RGBQUAD             *quads;

    num_colours = info->bmiHeader.biClrUsed;
    if( num_colours == 0 && info->bmiHeader.biBitCount != 24 ) {
        num_colours = 1 << info->bmiHeader.biBitCount;
    }

    palette_handle = (HPALETTE)0;

    if( num_colours ) {
        palette = malloc( sizeof( LOGPALETTE ) +
                num_colours * sizeof( PALETTEENTRY ) );
        if( palette == NULL ) return( (HPALETTE)0 );
        palette->palNumEntries = num_colours;
        palette->palVersion = 0x300;

        quads = &info->bmiColors[0];
        for( i = 0; i < num_colours; i++ ) {
            palette->palPalEntry[i].peRed = quads[i].rgbRed;
            palette->palPalEntry[i].peGreen = quads[i].rgbGreen;
            palette->palPalEntry[i].peBlue = quads[i].rgbBlue;
            palette->palPalEntry[i].peFlags = 0;
        }
        palette_handle = CreatePalette( palette );
        free( palette );
    }
    return( palette_handle );
}


static void PaintBitMap( BITMAPINFOHEADER *hdr, HDC dc ) {

    HDC                 mdc;
    HANDLE              oldhdl;
    HBITMAP             map;
    unsigned            color_count;
    char                *bytes;
    HPALETTE            palette;
    HPALETTE            oldpalette;

    color_count = ColorCount( hdr->biBitCount, hdr->biClrUsed );
    bytes = (char *)hdr + sizeof( BITMAPINFOHEADER );
    bytes += color_count * sizeof( RGBQUAD );
    palette = CreateDIBPalette( (BITMAPINFO *)hdr );
    oldpalette = SelectPalette( dc, palette, FALSE );
    RealizePalette( dc );
    map = CreateDIBitmap( dc, (LPBITMAPINFOHEADER)hdr,
      CBM_INIT, bytes, (LPBITMAPINFO)hdr, DIB_RGB_COLORS );
    mdc = CreateCompatibleDC( dc );
    oldhdl = SelectObject( mdc, map );
    BitBlt( dc, 0, 0, hdr->biWidth, hdr->biHeight, mdc, 0, 0, SRCCOPY );
    SelectPalette( dc, oldpalette, FALSE );
    DeleteObject( palette );
    SelectObject( mdc, oldhdl );
    DeleteObject( map );
    DeleteDC( mdc );
} /* PaintBitMap */

/*
 * this is an undocumented windows structure that was determined
 * by experimentation.  It is subject to change by Microsoft
 */
typedef struct cursorheader {
    WORD        xhotspot;
    WORD        yhotspot;
    WORD        width;          /* the width and height fields */
    WORD        height;         /* may be reversed */
    DWORD       I_dont_know_what_the_hell_this_is;
} CursorHeader;

/*
 * PaintCursor - takes a cursor converts it to an icon and
 *               displays it
 */

static void PaintCursor( HWND hwnd, HDC dc, CursorHeader *cursor ) {

    char                *and_mask;
    char                *xor_mask;
    HICON               icon;
    BITMAPINFOHEADER    *tmp;
    RECT                area;
    WORD                width;
    WORD                cursor_width;
    HBRUSH              brush;
    WORD                save_width;

    tmp = (BITMAPINFOHEADER *) ( (char *)cursor + 4 );
    if( tmp->biSize == sizeof( BITMAPINFOHEADER ) ) {
        and_mask = (char *) cursor + 4 + sizeof( BITMAPINFOHEADER ) +
                   2 * sizeof( RGBQUAD );
        xor_mask =  and_mask + ( tmp->biWidth * tmp->biHeight ) / 16;
        icon = CreateIcon( Instance, tmp->biWidth, tmp->biHeight/2,
                        1, 1, and_mask, xor_mask );
    } else {
        and_mask = (char *) cursor + sizeof( CursorHeader );
        xor_mask = and_mask + ( cursor->width * cursor->height ) / 8;
        icon = CreateIcon( Instance, cursor->width, cursor->height,
                            1, 1, and_mask, xor_mask );
    }
    GetClientRect( hwnd, &area );
    cursor_width = GetSystemMetrics( SM_CXCURSOR );
    if( cursor_width * 2 > area.right ) {
        width = cursor_width;
    } else {
        width = area.right / 2;
    }
    save_width = area.left;
    area.left = width;
    brush = GetStockObject( BLACK_BRUSH );
    FillRect( dc, &area, brush );
    area.left = save_width;
    area.right = width;
    brush = GetStockObject( WHITE_BRUSH );
    FillRect( dc, &area, brush );
    SetMapMode( dc, MM_TEXT );
    DrawIcon( dc, 0, 0, icon );
    DrawIcon( dc, width, 0, icon );
    DestroyIcon( icon );
}

static void PaintIcon( HDC dc, ResInfo *info ) {

    BITMAPINFO          *btinfo;
    HICON               icon;
    unsigned            clr_cnt;
    char                *and_mask;
    char                *xor_mask;
    RECT                area;
    WORD                i;
    HBRUSH              brush;

    btinfo = (BITMAPINFO *) (info->res);
    if( btinfo->bmiHeader.biSize == sizeof( BITMAPINFOHEADER ) ) {
        clr_cnt = ColorCount( btinfo->bmiHeader.biBitCount,
                  btinfo->bmiHeader.biClrUsed );
        and_mask = (char *)btinfo + sizeof( BITMAPINFOHEADER ) +
                   clr_cnt * sizeof( RGBQUAD );
        xor_mask = and_mask +
                   ( btinfo->bmiHeader.biWidth * btinfo->bmiHeader.biHeight )
                   / ( 2 * ( 8 / btinfo->bmiHeader.biBitCount ) );
        icon = CreateIcon( Instance, btinfo->bmiHeader.biWidth,
               btinfo->bmiHeader.biHeight/2, 1, 1, and_mask, xor_mask );
        SetMapMode( dc, MM_TEXT );
        brush = GetStockObject( WHITE_BRUSH );
        area.top = 0;
        area.bottom = btinfo->bmiHeader.biHeight;
        area.right = btinfo->bmiHeader.biWidth;
        area.left = area.right + btinfo->bmiHeader.biWidth;
        DrawIcon( dc, btinfo->bmiHeader.biWidth, 0, icon );
        DestroyIcon( icon );
        /* take the mirror image of the icon */
        for( i = 0; i < btinfo->bmiHeader.biHeight / 2; i++ ) {
            BitBlt( dc, 0, btinfo->bmiHeader.biHeight / 2 - i,
                    btinfo->bmiHeader.biWidth, 1, dc,
                    btinfo->bmiHeader.biWidth, i, SRCCOPY );
        }
        FillRect( dc, &area, brush );
    } else {
        icon = info->hdl;
        SetMapMode( dc, MM_TEXT );
        DrawIcon( dc, 0, 0, icon );
    }
}

static void AddRes( HWND hwnd ) {

    WORD        i;

    for( i = 0; i < MAX_RES; i++ ){
        if( ResHwnd[i] == NULL ) {
            ResHwnd[i] = hwnd;
        }
    }
} /* AddRes */

static void DeleteRes( HWND hwnd ) {

    WORD        i;

    for( i=0; i < MAX_RES; i++ ) {
        if( ResHwnd[i] == hwnd ) {
            ResHwnd[i] = NULL;
            break;
        }
    }
} /* DeleteRes */

BOOL __export FAR PASCAL ItemDisplayProc( HWND hwnd, WORD msg, WORD wparam,
                                    DWORD lparam )
{
    ResInfo             *info;
    HDC                 dc;
    PAINTSTRUCT         paint;
    HMENU               menu;
    char                buf[40];
    RECT                area;

    info = (ResInfo *)GetWindowLong( hwnd, 0 );
    switch( msg ) {
    case WM_PAINT:
        switch( info->type ) {
        case GD_BITMAP:
            dc = BeginPaint( hwnd, &paint );
            PaintBitMap( (BITMAPINFOHEADER *) info->res, dc );
            EndPaint( hwnd, &paint );
            break;
        case GD_ICON:
            dc = BeginPaint( hwnd, &paint );
            PaintIcon( dc, info );
            EndPaint( hwnd, &paint );
            break;
        case GD_CURSOR:
            dc = BeginPaint( hwnd, &paint );
            PaintCursor( hwnd, dc, (CursorHeader *)(info->res) );
            EndPaint( hwnd, &paint );
        default:
            return( DefWindowProc( hwnd, msg, wparam, lparam ) );
            break;
        }
        break;
    case WM_SIZE:
        if( info->type == GD_CURSOR ) {
            GetClientRect( hwnd, &area );
            InvalidateRect( hwnd, &area, TRUE );
        }
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    case WM_COMMAND:
        if( info->type == GD_MENU ) {
            if( IsWindow( info->menu_const ) ) {
                DestroyWindow( info->menu_const );
            }
            if( DialCount == 0 ) {
                DialProc = MakeProcInstance( (FARPROC)DialogDispProc, Instance );
            }
            info->menu_const = JCreateDialog( Instance, "MENU_CONST", hwnd, (DLGPROC)DialProc );
            menu = GetMenu( hwnd );
            GetMenuString( menu, wparam, buf, 40, MF_BYCOMMAND );
            SetStaticText( info->menu_const, MENU_ITEM, buf );
            sprintf( buf, "0x%04X", wparam );
            SetStaticText( info->menu_const, MENU_CONSTANT, buf );
        }
        break;
    case WM_CLOSE:
        if( info->hdl != NULL ) {
            UnlockResource( info->hdl );
        }
        MemFree( info );
        DeleteRes( hwnd );
        DestroyWindow( hwnd );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( FALSE );
}

void ShowHeapObject( HWND lbhandle )
{
    heap_list   *hl;
    LRESULT     index;
    HWND        dispwnd;
    HWND        memhdl;
    BOOL        is_res;
    char        *rcstr;

    dispwnd = NULL;
    index = SendMessage( lbhandle, LB_GETCURSEL, 0 , 0L );
    if( index == LB_ERR ) {
        rcstr = HWAllocRCString( STR_SHOW );
        RCMessageBox( HeapWalkMainWindow, STR_NO_ITEM_SELECTED,
                        rcstr, MB_OK | MB_ICONEXCLAMATION );
        HWFreeRCString( rcstr );
        return;
    }
    hl = HeapList[ index ];
    if( hl->is_dpmi ) {
        memhdl = DispMem( Instance, HeapWalkMainWindow, hl->info.mem.sel, TRUE );
    } else if( hl->info.ge.hBlock != NULL ) {
        memhdl = DispMem( Instance, HeapWalkMainWindow, (WORD)hl->info.ge.hBlock, FALSE );
    }
    if( memhdl == NULL ) return;
    is_res = TRUE;
    if( !hl->is_dpmi && hl->info.ge.wType == GT_RESOURCE &&
                Config.disp_res ) {
        switch( hl->info.ge.wData ) {
        case GD_CURSORCOMPONENT:
            dispwnd = ShowCursorHeapItem( hl, memhdl );
            break;
        case GD_BITMAP:
            dispwnd = ShowBitmapHeapItem( hl, memhdl );
            break;
        case GD_ICONCOMPONENT:
            dispwnd = ShowIconHeapItem( hl, memhdl );
            break;
        case GD_MENU:
            dispwnd = ShowMenuHeapItem( hl, memhdl );
            break;
        case GD_DIALOG:
            dispwnd = ShowDialogHeapItem( hl, memhdl );
            break;
        default:
            dispwnd = NULL;
            is_res = FALSE;
        }
        if( is_res ) {
            if( dispwnd == NULL ) {
                ErrorBox( HeapWalkMainWindow, STR_UNABLE_TO_DISP_GRAPHIC,
                          MB_OK | MB_ICONINFORMATION );
            } else {
                AddRes( dispwnd );
            }
        }
    }
}
