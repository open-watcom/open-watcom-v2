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
#include <math.h>
#include "imgedit.h"
#include "iedraw.h"
#include "iemem.h"

#define MIN_DRAW_WIN_WIDTH      150

static WPI_POINT        pointSize = {0, 0};
static short            toolType;
static HWND             hDrawArea = NULL;
static short            currentMouseButton;

/*
 * GridEnumProc - used to grid all the children
 */
BOOL CALLBACK GridEnumProc( HWND hwnd, LONG lparam )
{
    HWND        frame;

    lparam = lparam;
    if ( _wpi_getowner(hwnd) ) {
        return 1;
    }

    if ( _wpi_isiconic(hwnd) ) {
        return 1;
    } else {
        InvalidateRect( _wpi_getclient(hwnd), NULL, FALSE );
    }
    frame = frame;              // suppress compiler warnings
    return 1;
} /* GridEnumProc */

/*
 * CalculateDims - calculates the size of the "pixel" in the drawing
 *                      area and establishes the device draw area size.
 */
void CalculateDims( short img_width, short img_height, short *area_width,
                                short *area_height )
{
    int         point_size1;
    int         point_size2;
    int         max_width;
    int         max_height;
    WPI_RECT    rcclient;
    int         x_adj;
    int         y_adj;

    GetClientRect( ClientWindow, &rcclient );
    x_adj = (short)(2*_wpi_getsystemmetrics(SM_CXFRAME));
    y_adj = (short)(2*_wpi_getsystemmetrics(SM_CYFRAME) +
                        _wpi_getsystemmetrics(SM_CYCAPTION));

#if 0
    max_width = _wpi_getwidthrect(rcclient) - x_adj - origin->x;
    max_height = _wpi_getheightrect(rcclient) - y_adj - origin->y;
#else
    max_width = _wpi_getwidthrect(rcclient) - x_adj;
    max_height = _wpi_getheightrect(rcclient) - y_adj;
#endif

    point_size1 = max (1, max_width / img_width);
    point_size2 = max (1, max_height / img_height);

    if( point_size1 < 1 ) {
        point_size1 = 1;
    }
    if( point_size2 < 1 ) {
        point_size2 = 1;
    }

    pointSize.x = min( point_size1, point_size2 );
    while( (pointSize.x * img_width) < MIN_DRAW_WIN_WIDTH ) {
        pointSize.x++;
    }

    pointSize.y = pointSize.x;

    *area_width = (short)(pointSize.x * img_width);
    *area_height = (short)(pointSize.y * img_height);
} /* CalculateDims */

/*
 * BlowupImage - Stretches the view window into the window given by hwnd.
 */
void BlowupImage( HWND hmdiwnd, WPI_PRES pres )
{
    HDC         memdc;
    WPI_PRES    mempres;
    WPI_RECT    client;
    HBITMAP     oldbitmap;
    HBITMAP     newbitmap;
    HWND        hwnd;
    img_node    *node;
    BOOL        new_pres;

    if( hmdiwnd ) {
        hwnd = hmdiwnd;
    } else {
        node = GetCurrentNode();
        if( !node ) {
            return;
        }
        hwnd = node->hwnd;
    }

    newbitmap = EnlargeImage( hwnd );
    if (!newbitmap) return;

    new_pres = FALSE;
    if( pres == (WPI_PRES)NULL ) {
        pres = _wpi_getpres( hwnd );
        new_pres = TRUE;
    }
    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    oldbitmap = _wpi_selectobject( mempres, newbitmap );

    if (ImgedConfigInfo.grid_on) {
        showGrid( hwnd, mempres );
    } else {
        GetClientRect(hwnd, &client);
        _wpi_bitblt( pres, 0, 0, _wpi_getwidthrect(client),
                        _wpi_getheightrect(client), mempres, 0, 0, SRCCOPY );
        RedrawPrevClip(hwnd);   // Redraws if there was a clip region specified.
    }

    _wpi_selectobject( mempres, oldbitmap );
    _wpi_deletebitmap( newbitmap );
    _wpi_deletecompatiblepres( mempres, memdc );

    if( new_pres ) {
        _wpi_releasepres( hwnd, pres );
    }
} /* BlowupImage */

void IEInvalidateNode( img_node *node )
{
    HWND        hwnd;

    if( node == NULL ) {
        node = GetCurrentNode();
        if( node == NULL ) {
            return;
        }
    }

    hwnd = node->hwnd;

    InvalidateRect( hwnd, NULL, FALSE );
}

/*
 * CheckBounds - checks the bounds vs the client rectangle.
 */
void CheckBounds( HWND hwnd, WPI_POINT *pt )
{
    WPI_RECT    client;
    IMGED_DIM   left;
    IMGED_DIM   right;
    IMGED_DIM   top;
    IMGED_DIM   bottom;

    GetClientRect(hwnd, &client);
    _wpi_getrectvalues( client, &left, &top, &right, &bottom );

    if (pt->x >= right) {
        pt->x = right-1;
    } else if (pt->x < left) {
        pt->x = left;
    }
    if (pt->y >= bottom) {
        pt->y = bottom-1;
    } else if (pt->y < top) {
        pt->y = top;
    }
} /* CheckBounds */

/*
 * showGrid - Display the grid on the draw area.
 */
static void showGrid( HWND hwnd, WPI_PRES mempres )
{
    short       i;
    short       psx;
    short       psy;
    WPI_RECT    rcclient;
    HPEN        hblackpen;
    HPEN        holdpen;
    img_node    *node;
    short       width;
    short       height;
    IMGED_DIM   left;
    IMGED_DIM   right;
    IMGED_DIM   top;
    IMGED_DIM   bottom;
    WPI_PRES    pres;
    WPI_POINT   startpt;
    WPI_POINT   endpt;

    node = SelectImage( hwnd );

    _wpi_torgbmode( mempres );
    GetClientRect( hwnd, &rcclient );
    width = (short)( _wpi_getwidthrect(rcclient) );
    height = (short)( _wpi_getheightrect(rcclient) );

    if (((width / node->width) < POINTSIZE_MIN) ||
                                ((height / node->height) < POINTSIZE_MIN)) {
        psx = 0;
        psy = 0;
    } else {
        psx = width / node->width;
        psy = height / node->height;

        hblackpen = _wpi_createpen( PS_SOLID, 0, BLACK );
        holdpen = _wpi_selectobject( mempres, hblackpen );

        _wpi_getrectvalues( rcclient, &left, &top, &right, &bottom );

        for (i=0; i < width; i = i + psx) {
            _wpi_setpoint( &startpt, i, top );
            _wpi_setpoint( &endpt, i, bottom );
            _wpi_movetoex( mempres, &startpt, NULL );
            _wpi_lineto( mempres, &endpt );
        }
        for (i=0; i <= height; i = i + psy) {
            _wpi_setpoint( &startpt, left, i );
            _wpi_setpoint( &endpt, right, i );
            _wpi_movetoex( mempres, &startpt, NULL );
            _wpi_lineto( mempres, &endpt );
        }

        _wpi_selectobject( mempres, holdpen );
        _wpi_deleteobject( hblackpen );
    }

    pres = _wpi_getpres( hwnd );
    _wpi_bitblt( pres, 0, 0, width, height, mempres, 0, 0, SRCCOPY );
    _wpi_releasepres( hwnd, pres );
    RedrawPrevClip(hwnd);  // Redraws if there was a clip region specified.
} /* showGrid */

/*
 * DrawSinglePoint - needed for when the mouse button is initially pressed.
 */
void DrawSinglePoint( HWND hwnd, WPI_POINT *pt, short mousebutton )
{
    HBRUSH      colourbrush;
    HBRUSH      oldbrush;
    HPEN        colourpen;
    HPEN        oldpen;
    COLORREF    selected_colour;
    COLORREF    dithered;
    short       truncated_x;
    short       truncated_y;
    short       i;
    short       j;
    WPI_POINT   logical_pt;
    WPI_RECT    rcclient;
    short       width;
    short       height;
    short       wndwidth;
    short       wndheight;
    wie_clrtype type;
    WPI_PRES    pres;
    int         brushsize;
    BOOL        gridvisible;

    GetClientRect( hwnd, &rcclient );
    wndwidth = _wpi_getwidthrect( rcclient );
    wndheight = _wpi_getheightrect( rcclient );
    brushsize = ImgedConfigInfo.brush_size;

    CheckBounds( hwnd, pt );

    gridvisible = ImgedConfigInfo.grid_on && (pointSize.x >= POINTSIZE_MIN &&
                                                pointSize.y >= POINTSIZE_MIN);
    if (gridvisible) {
        if (toolType == IMGED_BRUSH) {
            truncated_x = max(0, (pt->x/pointSize.x - brushsize/2)) * pointSize.x+1;
            truncated_y = max(0, (pt->y/pointSize.y - brushsize/2)) * pointSize.y+1;
            width = (short)(pointSize.x - 1);
            height = (short)(pointSize.y - 1);
            /*
             * We just have to check that we don't spill over the image dimensions
             */
            truncated_x = min(truncated_x, wndwidth-pointSize.x*brushsize+1);
            truncated_y = min(truncated_y, wndheight-pointSize.y*brushsize+1);
        } else {
            truncated_x = ( pt->x / pointSize.x) * pointSize.x + 1;
            truncated_y = ( pt->y / pointSize.y) * pointSize.y + 1;
            width = (short)(pointSize.x - 1);
            height = (short)(pointSize.y - 1);
        }
    } else {
        if (toolType == IMGED_BRUSH) {
            truncated_x = max(0, (pt->x / pointSize.x - brushsize/2)) * pointSize.x;
            truncated_y = max(0, (pt->y / pointSize.y - brushsize/2)) * pointSize.y;
            width = (short)(pointSize.x * brushsize);
            height = (short)(pointSize.y * brushsize);
            /*
             * We just have to check that we don't spill over the image dimensions
             */
            truncated_x = min( truncated_x, wndwidth-width );
            truncated_y = min( truncated_y, wndheight-width );
        } else {
            truncated_x = ( pt->x / pointSize.x) * pointSize.x;
            truncated_y = ( pt->y / pointSize.y) * pointSize.y;
            width = (short)pointSize.x;
            height = (short)pointSize.y;
        }
    }
    logical_pt.x = truncated_x / pointSize.x;
    logical_pt.y = truncated_y / pointSize.y;

    pres = _wpi_getpres( hwnd );
    _wpi_torgbmode( pres );
    dithered = GetSelectedColour(mousebutton, &selected_colour, &type);
    colourbrush = _wpi_createsolidbrush( selected_colour );
    oldbrush = _wpi_selectobject( pres, colourbrush );
    colourpen = _wpi_createpen( PS_SOLID, 0, selected_colour );
    oldpen = _wpi_selectobject( pres, colourpen );

    if (gridvisible && (toolType == IMGED_BRUSH)) {
        for (i=0; i < brushsize; ++i) {
            for (j=0; j < brushsize; ++j) {
                _wpi_patblt(pres, truncated_x+i*pointSize.x,
                                  truncated_y+j*pointSize.y,
                                  width, height, PATCOPY);
            }
        }
    } else {
        _wpi_patblt(pres, truncated_x, truncated_y, width, height, PATCOPY);
    }

    _wpi_selectobject( pres, oldbrush );
    _wpi_selectobject( pres, oldpen );
    _wpi_releasepres( hwnd, pres );

    _wpi_deleteobject( colourbrush );
    _wpi_deleteobject( colourpen );

    /*
     * draws the points in the view window
     */
    if (toolType == IMGED_BRUSH) {
        if (type == SCREEN_CLR) {
            BrushThePoints(selected_colour, BLACK, WHITE, &logical_pt,
                                                                brushsize);
        } else if (type == INVERSE_CLR) {
            BrushThePoints(selected_colour, WHITE, WHITE, &logical_pt,
                                                                brushsize);
        } else {
            BrushThePoints(selected_colour, selected_colour, BLACK,
                                                    &logical_pt, brushsize);
        }
    } else {
        if (type == SCREEN_CLR) {
            DrawThePoints(selected_colour, BLACK, WHITE, &logical_pt);
        } else if (type == INVERSE_CLR) {
            DrawThePoints(selected_colour, WHITE, WHITE, &logical_pt);
        } else {
            DrawThePoints(selected_colour, selected_colour, BLACK, &logical_pt);
        }
    }
} /* DrawSinglePoint */

/*
 * drawPt - Actually draws the point on the drawing region. (uses LineDDA)
 */
void CALLBACK drawPt( int xpos, int ypos, WPI_PARAM2 lparam )
{
    HBRUSH      colourbrush;
    HBRUSH      oldbrush;
    HPEN        colourpen;
    HPEN        oldpen;
    COLORREF    selected_colour;
    COLORREF    dithered;
    short       mousebutton;
    WPI_PRES    pres;
    HWND        hwnd;
    short       area_x;
    short       area_y;
    short       width;
    short       height;
    short       i;
    short       j;
    WPI_POINT   pt;
    WPI_RECT    rcclient;
    wie_clrtype type;
    int         brushsize;
    BOOL        gridvisible;

    hwnd = (HWND)GET_HWND_PARAM2( lparam );
    mousebutton = currentMouseButton;
    _wpi_getclientrect( hwnd, &rcclient );
    brushsize = ImgedConfigInfo.brush_size;

    gridvisible = ImgedConfigInfo.grid_on && (pointSize.x >= POINTSIZE_MIN &&
                                                pointSize.y >= POINTSIZE_MIN);
    if ((!gridvisible) && (toolType == IMGED_FREEHAND)) {
        area_x = xpos * pointSize.x;
        area_y = ypos * pointSize.y;
        width = (short)pointSize.x;
        height = (short)pointSize.y;
    } else if ((!gridvisible) && (toolType == IMGED_BRUSH)) {
        area_x = max(0, xpos-brushsize/2) * pointSize.x;
        area_y = max(0, ypos-brushsize/2) * pointSize.y;
        width = (short)(brushsize * pointSize.x);
        height = (short)(brushsize * pointSize.y);
        /*
         * We just have to check that we don't spill over the image dimensions
         */
        area_x = min( area_x, _wpi_getwidthrect(rcclient)-width );
        area_y = min( area_y, _wpi_getheightrect(rcclient)-width );
    } else if ((gridvisible) && (toolType == IMGED_FREEHAND)) {
        area_x = xpos * pointSize.x+1;
        area_y = ypos * pointSize.y+1;
        width = (short)(pointSize.x-1);
        height = (short)(pointSize.y-1);
    } else {
        area_x = max(0, xpos-brushsize/2) * pointSize.x+1;
        area_y = max(0, ypos-brushsize/2) * pointSize.y+1;
        width = (short)(pointSize.x - 1);
        height = (short)(pointSize.y - 1);
        /*
         * We just have to check that we don't spill over the image dimensions
         */
        area_x = min( area_x, _wpi_getwidthrect(rcclient) - pointSize.x *
                                                                brushsize+1 );
        area_y = min( area_y, _wpi_getheightrect(rcclient) - pointSize.y *
                                                                brushsize+1 );
    }

    pres = _wpi_getpres( hwnd );
    _wpi_torgbmode( pres );
    dithered = GetSelectedColour( mousebutton, &selected_colour, &type );

    colourbrush = _wpi_createsolidbrush( selected_colour );
    oldbrush = _wpi_selectobject(pres, colourbrush);
    colourpen = _wpi_createpen(PS_SOLID, 0, selected_colour);
    oldpen = _wpi_selectobject(pres, colourpen);

    if (gridvisible && (toolType == IMGED_BRUSH)) {
        for (i=0; i < brushsize; ++i) {
            for (j=0; j < brushsize; ++j) {
                _wpi_patblt(pres, area_x+i*pointSize.x, area_y+j*pointSize.y,
                                                width, height, PATCOPY);
            }
        }
    } else {
        _wpi_patblt(pres, area_x, area_y, width, height, PATCOPY);
    }

    _wpi_selectobject(pres, oldbrush);
    _wpi_selectobject(pres, oldpen);
    _wpi_releasepres( hwnd, pres );

    _wpi_deleteobject( colourbrush );
    _wpi_deleteobject( colourpen );

    pt.x = area_x / pointSize.x;
    pt.y = area_y / pointSize.y;
    if (toolType == IMGED_BRUSH) {
        if (type == SCREEN_CLR) {
            BrushThePoints(selected_colour, BLACK, WHITE, &pt, brushsize);
        } else if (type == INVERSE_CLR) {
            BrushThePoints(selected_colour, WHITE, WHITE, &pt, brushsize);
        } else {
            BrushThePoints(selected_colour, selected_colour, BLACK, &pt,
                                                                brushsize);
        }
    } else {
        if (type == SCREEN_CLR) {
            DrawThePoints(selected_colour, BLACK, WHITE, &pt);
        } else if (type == INVERSE_CLR) {
            DrawThePoints(selected_colour, WHITE, WHITE, &pt);
        } else {
            DrawThePoints(selected_colour, selected_colour, BLACK, &pt);
        }
    }
} /* drawPt */

/*
 * Paint - When the mouse button is down, we want to paint on the
 *                   drawing area.
 */
void Paint( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, int mousebutton )
{
    WPI_POINT           s_pt;
    WPI_POINT           e_pt;
    WPI_PARAM2          lparam;
    WPI_LINEDDAPROC     fp;

    s_pt.x = MAKELOGPTX(start_pt->x);
    s_pt.y = MAKELOGPTY(start_pt->y);
    e_pt.x =  MAKELOGPTX(end_pt->x);
    e_pt.y = MAKELOGPTY(end_pt->y);

    CheckBounds( hwnd, &s_pt );
    CheckBounds( hwnd, &e_pt );

    s_pt.x = s_pt.x / pointSize.x;
    s_pt.y = s_pt.y / pointSize.y;
    e_pt.x = e_pt.x / pointSize.x;
    e_pt.y = e_pt.y / pointSize.y;

    currentMouseButton = mousebutton;
    SET_HWND_PARAM2( lparam, hwnd );
    fp = _wpi_makelineddaprocinstance( (WPI_LINEDDAPROC)drawPt, Instance );
    _wpi_linedda( s_pt.x, s_pt.y, e_pt.x, e_pt.y, (WPI_LINEDDAPROC)fp, lparam);
    _wpi_freeprocinstance( fp );

    DrawSinglePoint( hwnd, end_pt, mousebutton );
    memcpy(start_pt, end_pt, sizeof(WPI_POINT));
} /* Paint */

/*
 * DrawLine - Draws a line on the drawing area.
 */
void DrawLine( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, int mousebutton )
{
    COLORREF    colour;
    COLORREF    dithered;
    WPI_POINT   imgstart_pt;
    WPI_POINT   imgend_pt;
    wie_clrtype type;
    img_node    *node;

    CheckBounds( hwnd, start_pt );
    CheckBounds( hwnd, end_pt );
    imgstart_pt.x = start_pt->x / pointSize.x;
    imgstart_pt.y = start_pt->y / pointSize.y;
    imgend_pt.x = end_pt->x / pointSize.x;
    imgend_pt.y = end_pt->y / pointSize.y;

    dithered = GetSelectedColour(mousebutton, &colour, &type);

    if (type == SCREEN_CLR) {
        LineXorAnd(BLACK, WHITE, &imgstart_pt, &imgend_pt);
    } else if (type == INVERSE_CLR) {
        LineXorAnd(WHITE, WHITE, &imgstart_pt, &imgend_pt);
    } else {
        LineXorAnd(colour, BLACK, &imgstart_pt, &imgend_pt);
    }
    node = SelectImage( hwnd );
    InvalidateRect(node->viewhwnd, NULL, FALSE);
    BlowupImage( hwnd, NULL );
} /* DrawLine */

/*
 * OutlineLine - outlines the line before it is drawn.
 */
void OutlineLine( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt,
                                        WPI_POINT *prev_pt, BOOL firsttime )
{
    WPI_POINT   startpt;
    WPI_POINT   endpt;
    WPI_POINT   prevpt;
    int         prevROP2;
    HPEN        holdpen;
    HPEN        hwhitepen;
    WPI_PRES    pres;

    CheckBounds( hwnd, start_pt );
    CheckBounds( hwnd, end_pt );
    startpt.x = MAKELOGPTX( start_pt->x ) + pointSize.x/2;
    startpt.y = MAKELOGPTY( start_pt->y ) + pointSize.y/2;
    endpt.x = MAKELOGPTX( end_pt->x ) + pointSize.x/2;
    endpt.y = MAKELOGPTY( end_pt->y ) + pointSize.y/2;

    pres = _wpi_getpres( hwnd );
    _wpi_torgbmode( pres );
    hwhitepen = _wpi_createpen( PS_SOLID, 0, WHITE );
    holdpen = _wpi_selectobject( pres, hwhitepen );

    prevROP2 = _wpi_setrop2( pres, R2_XORPEN );

    if (!firsttime) {
        CheckBounds( hwnd, prev_pt );
        prevpt.x = MAKELOGPTX( prev_pt->x ) + pointSize.x/2;
        prevpt.y = MAKELOGPTY( prev_pt->y ) + pointSize.y/2;
        _wpi_movetoex( pres, &startpt, NULL );
        _wpi_lineto( pres, &prevpt );
    }

    _wpi_movetoex( pres, &startpt, NULL );
    _wpi_lineto( pres, &endpt );

    _wpi_setrop2( pres, prevROP2 );
    _wpi_selectobject( pres, holdpen );
    _wpi_deleteobject( hwhitepen );

    _wpi_releasepres( hwnd, pres );
}  /* OutlineLine */

/*
 * DisplayRegion - Draws the region (rectangle or ellipse) first in the view
 *              window, then in the draw area.
 */
void DisplayRegion( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, int mousebutton )
{
    COLORREF    colour;
    COLORREF    dithered;
    COLORREF    solid;
    WPI_POINT   imgstart_pt;
    WPI_POINT   imgend_pt;
    WPI_RECT    rect;
    BOOL        dofillrgn;
    BOOL        is_rect;
    wie_clrtype type;
    img_node    *node;

    CheckBounds( hwnd, start_pt );
    CheckBounds( hwnd, end_pt );
    imgstart_pt.x = min(start_pt->x / pointSize.x, end_pt->x / pointSize.x);
    imgend_pt.x = max(start_pt->x / pointSize.x, end_pt->x / pointSize.x);
#ifdef __OS2_PM__
    imgstart_pt.y = max(start_pt->y / pointSize.y, end_pt->y / pointSize.y);
    imgend_pt.y = min(start_pt->y / pointSize.y, end_pt->y / pointSize.y) - 1;
#else
    imgstart_pt.y = min(start_pt->y / pointSize.y, end_pt->y / pointSize.y);
    imgend_pt.y = max(start_pt->y / pointSize.y, end_pt->y / pointSize.y) + 1;
#endif

    imgend_pt.x += 1;

    dithered = GetSelectedColour(mousebutton, &solid, &type);
    switch (toolType) {
    case IMGED_RECTO:
        dofillrgn = FALSE;
        is_rect = TRUE;
        colour = solid;
        break;

    case IMGED_RECTF:
        dofillrgn = TRUE;
        is_rect = TRUE;
        colour = dithered;
        break;

    case IMGED_CIRCLEO:
        dofillrgn = FALSE;
        is_rect = FALSE;
        colour = solid;
        break;

    case IMGED_CIRCLEF:
        dofillrgn = TRUE;
        is_rect = FALSE;
        colour = dithered;
        break;

    default:
        return;
    }

    _wpi_setrectvalues(&rect, imgstart_pt.x, imgstart_pt.y, imgend_pt.x,
                                                        imgend_pt.y);
    if (type == SCREEN_CLR) {
        RegionXorAnd(BLACK, WHITE, dofillrgn, &rect, is_rect);
    } else if (type == INVERSE_CLR) {
        RegionXorAnd(WHITE, WHITE, dofillrgn, &rect, is_rect);
    } else {
        RegionXorAnd(colour, BLACK, dofillrgn, &rect, is_rect);
    }

    node = GetCurrentNode();
    InvalidateRect( node->viewhwnd, NULL, FALSE );
    BlowupImage( hwnd, NULL );
} /* DisplayRegion */

/*
 * OutlineClip - displays the potential region to be clipped to the clip board
 */
void OutlineClip( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt,
                                        WPI_POINT *prev_pt, BOOL firsttime )
{
    WPI_RECT    newpos;
    WPI_RECT    oldpos;
    short       temp;
    WPI_PRES    pres;
    IMGED_DIM   left;
    IMGED_DIM   top;
    IMGED_DIM   right;
    IMGED_DIM   bottom;

    CheckBounds( hwnd, start_pt );
    CheckBounds( hwnd, end_pt );
    CheckBounds( hwnd, prev_pt );
    left = (IMGED_DIM)MAKELOGPTX(start_pt->x);
    top = (IMGED_DIM)MAKELOGPTY(start_pt->y);
    right = (IMGED_DIM)MAKELOGPTX(end_pt->x);
    bottom = (IMGED_DIM)MAKELOGPTY(end_pt->y);

    if (left > right) {
        temp = right;
        right = left + pointSize.x;
        left = temp;
    } else {
        right += pointSize.x;
    }
    if (top > bottom) {
        temp = bottom;
        bottom = top + pointSize.y;
        top = temp;
    } else {
        bottom += pointSize.y;
    }
    _wpi_setrectvalues( &newpos, left, top, right, bottom );

    left = (IMGED_DIM)MAKELOGPTX( start_pt->x );
    top = (IMGED_DIM)MAKELOGPTY( start_pt->y );
    right = (IMGED_DIM)MAKELOGPTX( prev_pt->x );
    bottom = (IMGED_DIM)MAKELOGPTY( prev_pt->y );

    if (left > right) {
        temp = right;
        right = left + pointSize.x;
        left = temp;
    } else {
        right += pointSize.x;
    }
    if (top > bottom) {
        temp = bottom;
        bottom = top + pointSize.y;
        top = temp;
    } else {
        bottom += pointSize.y;
    }
    _wpi_setrectvalues( &oldpos, left, top, right, bottom );

    pres = _wpi_getpres( hwnd );
    OutlineRectangle( firsttime, pres, &oldpos, &newpos );
    _wpi_releasepres( hwnd, pres );
} /* OutlineClip */

/*
 * OutlineRegion - displays the potential region (rectangle or ellipse)
 *                 on the draw area.
 */
void OutlineRegion( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt,
                                        WPI_POINT *prev_pt, BOOL firsttime )
{
    WPI_POINT   topleft;
    WPI_POINT   bottomright;
    WPI_POINT   prevtl;                 // previous top left point
    WPI_POINT   prevbr;                 // previous bottom right point
    int         prevROP2;
    HBRUSH      hbrush;
    HBRUSH      holdbrush;
    HPEN        hwhitepen;
    HPEN        holdpen;
    int         temp;
    WPI_PRES    pres;

    CheckBounds( hwnd, start_pt );
    CheckBounds( hwnd, end_pt );
    CheckBounds( hwnd, prev_pt );
    _wpi_setpoint(&topleft, MAKELOGPTX(start_pt->x), MAKELOGPTY(start_pt->y));
    _wpi_setpoint(&bottomright, MAKELOGPTX(end_pt->x), MAKELOGPTY(end_pt->y));

    if (topleft.x > bottomright.x) {
        temp = (short)bottomright.x;
        bottomright.x = topleft.x + pointSize.x;
        topleft.x = temp;
    } else {
        bottomright.x += pointSize.x;
    }
    if (topleft.y > bottomright.y) {
        temp = (int)bottomright.y;
        bottomright.y = topleft.y + pointSize.y;
        topleft.y = temp;
    } else {
        bottomright.y += pointSize.y;
    }

    prevtl.x = MAKELOGPTX( start_pt->x );
    prevtl.y = MAKELOGPTY( start_pt->y );
    prevbr.x = MAKELOGPTX( prev_pt->x );
    prevbr.y = MAKELOGPTY( prev_pt->y );

    if (prevtl.x > prevbr.x) {
        temp = (int)prevbr.x;
        prevbr.x = prevtl.x + pointSize.x;
        prevtl.x = temp;
    } else {
        prevbr.x += pointSize.x;
    }

    if (prevtl.y > prevbr.y) {
        temp = (int)prevbr.y;
        prevbr.y = prevtl.y + pointSize.y;
        prevtl.y = temp;
    } else {
        prevbr.y += pointSize.y;
    }

    ++prevtl.x;
    ++prevtl.y;
    ++topleft.x;
    ++topleft.y;

    pres = _wpi_getpres( hwnd );
    _wpi_torgbmode( pres );
    prevROP2 = _wpi_setrop2( pres, R2_XORPEN );
    hbrush = _wpi_createsolidbrush( BLACK );
    hwhitepen = _wpi_createpen( PS_SOLID, 0, WHITE );

    holdbrush = _wpi_selectobject( pres, hbrush );
    holdpen = _wpi_selectobject( pres, hwhitepen );

    if (!firsttime) {
        if ((toolType == IMGED_CIRCLEO) || (toolType == IMGED_CIRCLEF)) {
            _wpi_ellipse( pres, prevtl.x, prevtl.y, prevbr.x, prevbr.y );
        } else {
            _wpi_rectangle( pres, prevtl.x, prevtl.y, prevbr.x, prevbr.y );
        }
    }

    if ((toolType == IMGED_CIRCLEO) || (toolType == IMGED_CIRCLEF)) {
        _wpi_ellipse( pres, topleft.x, topleft.y, bottomright.x, bottomright.y );
    } else {
        _wpi_rectangle( pres, topleft.x, topleft.y, bottomright.x, bottomright.y );
    }
    _wpi_selectobject( pres, holdpen );
    _wpi_selectobject( pres, holdbrush );

    _wpi_deleteobject( hwhitepen );
    _wpi_deleteobject( hbrush );
    _wpi_setrop2( pres, prevROP2 );
    _wpi_releasepres( hwnd, pres );
} /* OutlineRegion */

/*
 * FillArea - Fills the area.
 */
void FillArea( WPI_POINT *pt, int mousebutton )
{
    WPI_POINT   devicept;
    COLORREF    colour;
    wie_clrtype type;
    img_node    *node;

    devicept.x = pt->x / pointSize.x;
    devicept.y = pt->y / pointSize.y;
    colour = GetSelectedColour( mousebutton, NULL, &type );

    FillXorAnd(colour, &devicept, type);
    node = GetCurrentNode();
    InvalidateRect( node->viewhwnd, NULL, FALSE );
    BlowupImage( node->hwnd, NULL );
#ifdef __OS2_PM__
    currentMouseButton = currentMouseButton;    // just to suppres complaints
#endif
} /* FillArea */

/*
 * CreateNewDrawPad - creates a new draw pad which is a child of the client
 *                    window.  These are the mdi children.
 */
void CreateNewDrawPad( img_node *node )
{
#ifdef __OS2_PM__
    hDrawArea = PMNewDrawPad( node );
    WinSendMsg( hDrawArea, WM_ACTIVATE, MPFROMSHORT(TRUE), MPFROMHWND(hDrawArea) );
#else
    hDrawArea = WinNewDrawPad( node );
#endif
} /* CreateNewDrawPad */

/*
 * CreateDrawnImage - Creates the enlarged image in the drawing area.  (used
 *                    when activating a new image or selecting a new image).
 */
void CreateDrawnImage( img_node *node )
{
    WPI_RECT    rcclient;
    int         cx;
    int         cy;

    GetClientRect( node->hwnd, &rcclient );
    hDrawArea = node->hwnd;

    cx = _wpi_getwidthrect(rcclient);
    cy = _wpi_getheightrect(rcclient);

    cx = cx / node->width;
    cy = cy / node->height;

    pointSize.x = max (1, cx);
    pointSize.y = max (1, cy);

} /* CreateDrawnImage */

/*
 * CheckGridItem - Check the grid item and display the grid if necessary.
 *                 First we check if an item has been created or not, if not
 *                 we just check some flags so that when it is created, a
 *                 grid will (not) be displayed.
 */
void CheckGridItem( HMENU hmenu )
{
    WPI_ENUMPROC        fp_enum;
    HCURSOR             prevcursor;

    prevcursor = _wpi_setcursor( _wpi_getsyscursor(IDC_WAIT) );
    if ( _wpi_isitemchecked(hmenu, IMGED_GRID) ) {
        _wpi_checkmenuitem( hmenu, IMGED_GRID, MF_UNCHECKED, FALSE );
        ImgedConfigInfo.grid_on = FALSE;
        BlowupImage( NULL, NULL );
        PrintHintTextByID( WIE_GRIDTURNEDOFF, NULL );
    } else {
        _wpi_checkmenuitem( hmenu, IMGED_GRID, MF_CHECKED, FALSE );
        ImgedConfigInfo.grid_on = TRUE;
        BlowupImage( NULL, NULL );
        PrintHintTextByID( WIE_GRIDTURNEDON, NULL );
    }
    PressGridButton();

    if( DoImagesExist() ) {
        fp_enum = _wpi_makeenumprocinstance( GridEnumProc, Instance );
        _wpi_enumchildwindows( ClientWindow, fp_enum, 0L );
        _wpi_freeprocinstance( fp_enum );
    }
    _wpi_setcursor( prevcursor );
} /* CheckGridItem */

/*
 * ResetDrawArea - Resets the drawing area when a new image is selected from
 *                 the select icon menu option. (only for icon files).
 */
void ResetDrawArea( img_node *node )
{
    WPI_RECT    rcclient;
    WPI_RECT    rc;
    WPI_PARAM2  lparam;
    short       new_width;
    short       new_height;

    GetClientRect( node->hwnd, &rcclient );

    pointSize.x = max (1, _wpi_getwidthrect(rcclient) / node->width);
    pointSize.y = max (1, _wpi_getheightrect(rcclient) / node->height);

    if (ImgedConfigInfo.square_grid) {
        GetClientRect( ClientWindow, &rc );
        if (pointSize.y * node->height > _wpi_getheightrect(rc) ) {
            pointSize.y = pointSize.x;
        } else {
            pointSize.x = pointSize.y;
        }
        /*
         * I add 1 to cause ResizeChild to resize the window.
         */
        new_width = (short)(pointSize.x*node->width+1);
        new_height = (short)(pointSize.y*node->height+1);
        lparam = WPI_MAKEP2(new_width, new_height);
        ResizeChild(lparam, node->hwnd, TRUE );
    }
    BlowupImage( node->hwnd, NULL );
} /* ResetDrawArea */

/*
 * SetDrawTool - Sets the appropriate tool type.
 */
void SetDrawTool( int tool_type )
{
    toolType = tool_type;
} /* SetDrawTool */

/*
 * SetBrushSize - sets the size of the brush for the brush tool option.
 */
void SetBrushSize( short new_size )
{
    ImgedConfigInfo.brush_size = new_size;
    WriteSetSizeText( WIE_NEWBRUSHSIZE, new_size, new_size );
} /* SetBrushSize */

static void SetGridSize( int x, int y )
{
    WriteSetSizeText( WIE_NEWGRIDSIZE, x, y );
} /* SetGridSize */

/*
 * RepaintDrawArea - handles the WM_PAINT message.
 */
void RepaintDrawArea( HWND hwnd )
{
    WPI_PRES            pres;
    PAINTSTRUCT         ps;

    if (hwnd) {
        pres = _wpi_beginpaint( hwnd, NULL, &ps );
        BlowupImage( hwnd, pres );
        _wpi_endpaint( hwnd, pres, &ps );
    }
} /* RepaintDrawArea */

/*
 * GetPointSize - returns the point size.
 */
WPI_POINT GetPointSize( HWND hwnd )
{
    WPI_POINT   pt = {0, 0};
    img_node    *node;
    WPI_RECT    rc;
    int         width;
    int         height;

    node = SelectImage( hwnd );
    if (!node) return( pt );

    GetClientRect( hwnd, &rc );
    width = _wpi_getwidthrect(rc);
    height = _wpi_getheightrect(rc);
    pt.x = width / node->width;
    pt.y = height / node->height;

    if( pt.x == 0 ) {
        pt.x = 1;
    }
    if( pt.y == 0 ) {
        pt.y = 1;
    }
    return( pt );
} /* GetPointSize */

/*
 * ResizeChild - resizes the draw area of the child window.
 */
void ResizeChild( WPI_PARAM2 lparam, HWND hwnd, BOOL firsttime )
{
    short       min_width;
    short       width;
    short       height;
    img_node    *node;
    short       x_adjustment;
    short       y_adjustment;
    short       new_width;
    short       new_height;
    WPI_POINT   point_size;
    WPI_POINT   max;
    HWND        frame;
    HMENU       hmenu;
    WPI_RECT    rc;
    WPI_RECT    rect;

    frame = _wpi_getframe( hwnd );
    width = LOWORD( lparam );
#ifdef __OS2_PM__
    height = SHORT2FROMMP( lparam );
#else
    height = HIWORD( lparam );
#endif

    GetClientRect( hwnd, &rc );
    GetWindowRect( hwnd, &rect );
    if( frame ) {
        GetClientRect( frame, &rc );
        GetWindowRect( frame, &rect );
    }

    max.x = _wpi_getsystemmetrics( SM_CXSCREEN );
    max.y = _wpi_getsystemmetrics( SM_CYSCREEN );

    x_adjustment = _wpi_getwidthrect( rect ) - _wpi_getwidthrect( rc );
    y_adjustment = _wpi_getheightrect( rect ) - _wpi_getheightrect( rc );

    #if 1
        min_width = MIN_DRAW_WIN_WIDTH;
    #else
        min_width = (short)(2*_wpi_getsystemmetrics(SM_CXSIZE)) +
                    x_adjustment +
                    8; // fudge factor to allow some of title bar to show
    #endif

#ifdef __OS2_PM__
    ++y_adjustment;
#endif

    node = SelectImage( hwnd );
    if (!node) return;

    // the following assumes that max.x >> min_width
    point_size.x = min( max.x / node->width, width / node->width );
    point_size.x = max( min_width / node->width + 1, point_size.x );

    point_size.y = min( max.y / node->height, height / node->height );
    point_size.y = max( 1, point_size.y );

    if( ImgedConfigInfo.square_grid ) {
        point_size.x = min( point_size.x, point_size.y );
        if( point_size.x < ( min_width / node->width + 1 ) ) {
            point_size.x = min_width / node->width + 1;
        }
        if( point_size.x > ( max.y / node->height ) ) {
            hmenu = GetMenu( _wpi_getframe(HMainWindow) );
            CheckSquareGrid( hmenu );
        } else {
            point_size.y = point_size.x;
        }
    }

    new_width = (short)(point_size.x * node->width + x_adjustment);
    new_height = (short)(point_size.y * node->height + y_adjustment);

    pointSize = point_size;

    if( (pointSize.x*node->width != width) ||
        (pointSize.y*node->height != height) ) {
        SetWindowPos( frame, HWND_TOP, 0, 0, new_width, new_height,
                        SWP_SIZE | SWP_ZORDER | SWP_NOMOVE | SWP_SHOWWINDOW);
        SetGridSize( pointSize.x, pointSize.y );
    }
    if( !firsttime ) {
        BlowupImage( node->hwnd, NULL );
    }
} /* ResizeChild */

/*
 * CheckSquareGrid - checks the square grid item
 */
void CheckSquareGrid( HMENU hmenu )
{
    if ( _wpi_isitemchecked(hmenu, IMGED_SQUARE) ) {
        _wpi_checkmenuitem(hmenu, IMGED_SQUARE, MF_UNCHECKED, FALSE);
        ImgedConfigInfo.square_grid = FALSE;
        PrintHintTextByID( WIE_SQUAREGRIDOFF, NULL );
    } else {
        _wpi_checkmenuitem( hmenu, IMGED_SQUARE, MF_CHECKED, FALSE );
        ImgedConfigInfo.square_grid = TRUE;
        PrintHintTextByID( WIE_SQUAREGRIDON, NULL );
    }
} /* CheckSquareGrid */

/*
 * MaximizeCurrentChild - makes the current edit window as large as it can
 *                        possibly be.
 */
void MaximizeCurrentChild( void )
{
    short       max_width;
    short       max_height;
    WPI_POINT   max_pt_size;
    WPI_RECT    client;
    WPI_RECT    rect;
    WPI_RECT    rc;
    img_node    *node;
    short       x_adjustment;
    short       y_adjustment;

    node = GetCurrentNode();
    if (!node) return;

    GetClientRect( node->hwnd, &rc );
    GetWindowRect( node->hwnd, &rect );

    x_adjustment = _wpi_getwidthrect( rect ) - _wpi_getwidthrect( rc );
    y_adjustment = _wpi_getheightrect( rect ) - _wpi_getheightrect( rc );

    GetClientRect( ClientWindow, &client );
    max_width = (short)( _wpi_getwidthrect(client) );
    max_height = (short)( _wpi_getheightrect(client) );

    max_pt_size.x = max_width / node->width;
    max_pt_size.y = max_height / node->height;

    if( ImgedConfigInfo.square_grid ) {
        max_pt_size.x = min( max_pt_size.x, max_pt_size.y );
        max_pt_size.y = max_pt_size.x;
    }

    if( max_pt_size.x <= pointSize.x && max_pt_size.y <= pointSize.y ) {
        return;
    }

    max_width = max_pt_size.x * node->width + x_adjustment;
    max_height = max_pt_size.y * node->height + y_adjustment;

    SetWindowPos( node->hwnd, HWND_TOP, 0, 0, max_width, max_height,
                        SWP_SIZE | SWP_MOVE | SWP_ZORDER | SWP_SHOWWINDOW );
} /* MaximizeCurrentChild */

