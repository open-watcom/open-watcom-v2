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
* Description:  Tools menu (owner drawn).
*
****************************************************************************/


#include "precomp.h"

#include "wdeglbl.h"
#include "wde_rc.h"
#include "wdetmenu.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    char    *bmp;
    HBITMAP hbmp;
    WORD    id;
    char    string[64];
} WdeToolBitMapType;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WdeToolBitMapType WdeMenuBitmaps[] = {
    { "SelMode",        NULL, IDM_SELECT_MODE,   "" },
    { "StikMode",       NULL, IDM_STICKY_TOOLS,  "" },
    { "DiagMode",       NULL, IDM_DIALOG_TOOL,   "" },
    { "PushMode",       NULL, IDM_PBUTTON_TOOL,  "" },
    { "RadMode",        NULL, IDM_RBUTTON_TOOL,  "" },
    { "ChekMode",       NULL, IDM_CBUTTON_TOOL,  "" },
    { "GrpMode",        NULL, IDM_GBUTTON_TOOL,  "" },
    { "TextMode",       NULL, IDM_TEXT_TOOL,     "" },
    { "FramMode",       NULL, IDM_FRAME_TOOL,    "" },
    { "IconMode",       NULL, IDM_ICON_TOOL,     "" },
    { "EditMode",       NULL, IDM_EDIT_TOOL,     "" },
    { "ListMode",       NULL, IDM_LISTBOX_TOOL,  "" },
    { "CombMode",       NULL, IDM_COMBOBOX_TOOL, "" },
    { "HScrMode",       NULL, IDM_HSCROLL_TOOL,  "" },
    { "VScrMode",       NULL, IDM_VSCROLL_TOOL,  "" },
    { "SBoxMode",       NULL, IDM_SIZEBOX_TOOL,  "" },
    { "Custom1",        NULL, IDM_CUSTOM1_TOOL,  "" },
    { "Custom2",        NULL, IDM_CUSTOM2_TOOL,  "" },
    { "AlignLeft",      NULL, IDM_FMLEFT,        "" },
    { "AlignRight",     NULL, IDM_FMRIGHT,       "" },
    { "AlignTop",       NULL, IDM_FMTOP,         "" },
    { "AlignBottom",    NULL, IDM_FMBOTTOM,      "" },
    { "AlignHCentre",   NULL, IDM_FMHCENTRE,     "" },
    { "AlignVCentre",   NULL, IDM_FMVCENTRE,     "" },
    { "SameWidth",      NULL, IDM_SAME_WIDTH,    "" },
    { "SameHeight",     NULL, IDM_SAME_HEIGHT,   "" },
    { "SameSize",       NULL, IDM_SAME_SIZE,     "" },
    { NULL,             NULL, -1,                "" }
};

void WdeInitToolMenu( HINSTANCE inst, HMENU menu )
{
    int i;

    for( i = 0; WdeMenuBitmaps[i].bmp != NULL; i++ ) {
        WdeMenuBitmaps[i].hbmp = LoadBitmap( inst, WdeMenuBitmaps[i].bmp );
        GetMenuString( menu, WdeMenuBitmaps[i].id, WdeMenuBitmaps[i].string, 255,
                       MF_BYCOMMAND );
        if( WdeMenuBitmaps[i].hbmp != NULL ) {
            ModifyMenu( menu, WdeMenuBitmaps[i].id, MF_BYCOMMAND | MF_OWNERDRAW,
                        WdeMenuBitmaps[i].id, (LPCSTR)&WdeMenuBitmaps[i] );
        }
    }
}

void WdeFiniToolMenu( void )
{
    int i;

    for( i = 0; WdeMenuBitmaps[i].bmp != NULL; i++ ) {
        if( WdeMenuBitmaps[i].hbmp != NULL ) {
            DeleteObject( WdeMenuBitmaps[i].hbmp );
        }
    }
}

void WdeHandleMeasureItem( MEASUREITEMSTRUCT *mis )
{
    BITMAP              bm;
    HDC                 hdc;
    RECT                rc;
    HBITMAP             check_bitmap;
    BITMAP              check_bm;
    WdeToolBitMapType   *od_data;
    int                 border_width;
    int                 spacing;

    if( mis->CtlType == ODT_MENU ) {
        border_width = GetSystemMetrics( SM_CXBORDER );
        spacing = border_width * 2 + 4;
        check_bitmap = LoadBitmap( NULL, MAKEINTRESOURCE( OBM_CHECK ) );
        GetObject( check_bitmap, sizeof( BITMAP ), &check_bm );
        od_data = (WdeToolBitMapType *)mis->itemData;
        GetObject( od_data->hbmp, sizeof( BITMAP ), &bm );
        hdc = GetDC( NULL );
        rc.left = rc.top = 0;
        DrawText( hdc, od_data->string, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_CALCRECT );
        ReleaseDC( NULL, hdc );
        mis->itemWidth = bm.bmWidth + check_bm.bmWidth + spacing + rc.right;
        mis->itemHeight = max( bm.bmHeight, rc.bottom );
    }
}

void WdeHandleDrawItem( DRAWITEMSTRUCT *dis )
{
    HBRUSH              back_brush;
    COLORREF            back_color;
    COLORREF            fore_color;
    COLORREF            old_back_color;
    COLORREF            old_fore_color;
    HDC                 mem_dc;
    HBITMAP             old_bitmap;
    HBITMAP             check_bitmap;
    BITMAP              bm;
    BITMAP              check_bm;
    int                 x;
    int                 y;
    int                 border_width;
    RECT                rc;
    WdeToolBitMapType   *od_data;

    border_width = GetSystemMetrics( SM_CXBORDER );
    if( dis->CtlType == ODT_MENU ) {
        od_data = (WdeToolBitMapType *)dis->itemData;
        check_bitmap = LoadBitmap( NULL, MAKEINTRESOURCE( OBM_CHECK ) );
        GetObject( check_bitmap, sizeof( BITMAP ), &check_bm );
        if( dis->itemState & ODS_SELECTED ) {
            back_brush = CreateSolidBrush( GetSysColor( COLOR_HIGHLIGHT ) );
            back_color = GetSysColor( COLOR_HIGHLIGHT );
            fore_color = GetSysColor( COLOR_HIGHLIGHTTEXT );
        } else {
            back_brush = CreateSolidBrush( GetSysColor( COLOR_MENU ) );
            back_color = GetSysColor( COLOR_MENU );
            fore_color = GetSysColor( COLOR_MENUTEXT );
        }
        FillRect( dis->hDC, &dis->rcItem, back_brush );
        mem_dc = CreateCompatibleDC( dis->hDC );
        old_bitmap = SelectObject( mem_dc, od_data->hbmp );
        GetObject( od_data->hbmp, sizeof( BITMAP ), &bm );
        x = dis->rcItem.left + check_bm.bmWidth + border_width;
        y = dis->rcItem.top + (dis->rcItem.bottom - dis->rcItem.top) / 2 -
            bm.bmHeight / 2;
        if( dis->itemState & ODS_SELECTED ) {
            PatBlt( mem_dc, 0, 0, bm.bmWidth, bm.bmHeight, DSTINVERT );
            BitBlt( dis->hDC, x, y, bm.bmWidth, bm.bmHeight, mem_dc, 0, 0, SRCPAINT );
            PatBlt( mem_dc, 0, 0, bm.bmWidth, bm.bmHeight, DSTINVERT );
        } else {
            BitBlt( dis->hDC, x, y, bm.bmWidth, bm.bmHeight, mem_dc, 0, 0, SRCAND );
        }
        if( dis->itemState & ODS_CHECKED ) {
            SelectObject( mem_dc, check_bitmap );
            x = dis->rcItem.left + border_width;
            y = dis->rcItem.top + (dis->rcItem.bottom - dis->rcItem.top) / 2 -
                check_bm.bmHeight / 2;
            if( dis->itemState & ODS_SELECTED ) {
                PatBlt( mem_dc, 0, 0, check_bm.bmWidth, check_bm.bmHeight, DSTINVERT );
                BitBlt( dis->hDC, x, y, check_bm.bmWidth, check_bm.bmHeight, mem_dc,
                        0, 0, SRCPAINT );
                PatBlt( mem_dc, 0, 0, check_bm.bmWidth, check_bm.bmHeight, DSTINVERT );
            } else {
                BitBlt( dis->hDC, x, y, check_bm.bmWidth, check_bm.bmHeight, mem_dc,
                        0, 0, SRCAND );
            }
        }
        CopyRect( &rc, &dis->rcItem );
        rc.left += bm.bmWidth + check_bm.bmWidth + border_width * 2 + 4;
        old_fore_color = SetTextColor( dis->hDC, fore_color );
        old_back_color = SetBkColor( dis->hDC, back_color );
        DrawText( dis->hDC, od_data->string, -1, &rc,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE );
        SetTextColor( dis->hDC, old_fore_color );
        SetBkColor( dis->hDC, old_back_color );
        SelectObject( mem_dc, old_bitmap );
        DeleteObject( back_brush );
        DeleteDC( mem_dc );
    }
}
