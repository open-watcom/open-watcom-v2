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
* Description:  Drawing routines for OS/2.
*
****************************************************************************/


#define INCL_PM

#include <os2.h>
#include <string.h>
#include "drawmenu.h"
#include "button.h"
#include "room.h"
#include "threed.h"

static HWND hwnd;

static HAB Main_hab;

static HMQ Main_hmq;

static three_d_handle hThree_d;

static RECTL Draw_area;

#ifdef DRAW_ALL_AT_ONCE
static HDC Hdc;
static HPS Mem_dc;
static HBITMAP Draw_bitmap = NULLHANDLE;
static HBITMAP Old_bitmap = NULLHANDLE;
#endif


static int Button_height;

static int Auto_run = 0;

static void add_wrap( three_d_attr attr, int add, int minimum, int maximum )
{
    int         start;

    start = three_d_get( hThree_d, attr );
    start += add;
    while( start >= maximum ) {
        start -= maximum - minimum;
    }
    while( start < minimum ) {
        start += maximum - minimum;
    }
    three_d_set( hThree_d, attr, start );
}

static void add_range( three_d_attr attr, int add, int minimum, int maximum )
{
    int         start;

    start = three_d_get( hThree_d, attr );
    start += add;
    if( start >= maximum ) {
        three_d_set( hThree_d, attr, maximum );
    } else if( start <= minimum ) {
        three_d_set( hThree_d, attr, minimum );
    } else {
        three_d_set( hThree_d, attr, start );
    }
}

void draw_box(
    float       x1,
    float       x2,
    float       y1,
    float       y2,
    float       z1,
    float       z2,
    unsigned    red,
    unsigned    green,
    unsigned    blue )
{
    three_d_box( hThree_d, x1, x2, y1, y2, z1, z2, red, green, blue );
}

void new_display( void )
{
#ifdef DRAW_ALL_AT_ONCE
    GpiSetBitmap( Mem_dc, Old_bitmap );
    GpiDeleteBitmap( Draw_bitmap );
    Draw_bitmap = NULLHANDLE;
    GpiDestroyPS( Mem_dc );
    DevCloseDC( Hdc );
#endif
}

static void draw_stuff( HWND hwnd )
{
    HPS                 win_dc;
    RECTL               paint;
    RECTL               intersect;
#ifdef DRAW_ALL_AT_ONCE
    int                 old_top;
    int                 width, height;
    SIZEL               sizl = { 0, 0 };
    BITMAPINFOHEADER2   bmih;
    LONG                formats[24];
    POINTL              pts[3];
    LONG                old_cursor;
    LONG                hour_glass_cur;
    RECTL               interior;
    DEVOPENSTRUC        dop = { 0L, "DISPLAY", NULL, 0L,
                                0L, 0L, 0L, 0L, 0L };
#endif


    win_dc = WinBeginPaint( hwnd, 0, &paint );
    GpiCreateLogColorTable( win_dc, 0L, LCOLF_RGB, 0L, 0L, NULL );
#ifdef DRAW_ALL_AT_ONCE
    old_top = paint.yBottom;
    paint.yBottom = Draw_area.yTop;
#endif
    WinFillRect( win_dc, &paint, SYSCLR_WINDOW );
#ifdef DRAW_ALL_AT_ONCE
    paint.yBottom = old_top;
#endif
    if( WinIntersectRect( Main_hab, &intersect, &paint, &Draw_area ) ) {

#ifdef DRAW_ALL_AT_ONCE
        width = Draw_area.xRight - Draw_area.xLeft;
        height = Draw_area.yTop - Draw_area.yBottom;
        interior.xLeft = 0;
        interior.yBottom = 0;
        interior.xRight = width;
        interior.yTop = height;
        if( Draw_bitmap == NULLHANDLE ) {
            Hdc = DevOpenDC( Main_hab, OD_MEMORY, "*", 5L,
                                            (PDEVOPENDATA)&dop, NULLHANDLE );
            Mem_dc = GpiCreatePS( Main_hab, Hdc, &sizl, PU_PELS | GPIA_ASSOC );
            memset( &bmih, 0, sizeof( BITMAPINFOHEADER2 ) );
            GpiQueryDeviceBitmapFormats( Mem_dc, 24L, formats );
            bmih.cbFix = sizeof( BITMAPINFOHEADER2 );
            bmih.cx = width;
            bmih.cy = height;
            bmih.cPlanes = (USHORT) formats[0];
            bmih.cBitCount = (USHORT) formats[1];
            Draw_bitmap = GpiCreateBitmap( Mem_dc, &bmih, 0L, NULL, NULL );
            Old_bitmap = GpiSetBitmap( Mem_dc, Draw_bitmap );
            GpiCreateLogColorTable( Mem_dc, 0, LCOLF_RGB, 0, 0, NULL );
            WinFillRect( Mem_dc, &interior, SYSCLR_WINDOW );

            hour_glass_cur = WinQuerySysPointer( HWND_DESKTOP, SPTR_WAIT, FALSE );
            old_cursor = WinQueryPointer( HWND_DESKTOP );
            WinSetPointer( HWND_DESKTOP, hour_glass_cur );

            hThree_d = three_d_begin( Mem_dc, &interior );
            draw_room();
            three_d_draw( hThree_d );
            three_d_end( hThree_d );

            WinSetPointer( HWND_DESKTOP, old_cursor );
        }

        pts[0].x = Draw_area.xLeft;
        pts[0].y = Draw_area.yBottom;
        pts[1].x = Draw_area.xLeft + width;
        pts[1].y = Draw_area.yBottom + height;
        pts[2].x = 0;
        pts[2].y = 0;
        GpiBitBlt( win_dc, Mem_dc, 3, pts, ROP_SRCCOPY, BBO_IGNORE );
#else
        hThree_d = three_d_begin( win_dc, &Draw_area );
        draw_room();
        three_d_draw( hThree_d );
        three_d_end( hThree_d );
#endif

    }
    WinEndPaint( win_dc );
}

static void make_buttons( HWND hwnd )
{
    int     offset;
    int     max_height;
    int     btn_width;
    int     btn_height;
    static int buttons[] = {
        IDM_ROTATE_UP,
        IDM_MOVE_IN,
        IDM_ROTATE_LEFT,
        IDM_ROTATE_RIGHT,
        IDM_MOVE_OUT,
        IDM_ROTATE_DOWN,
        IDM_MORE_BRIGHTNESS,
        IDM_LESS_BRIGHTNESS,
        0 };
    int index;

    Draw_area.yTop -= 2;
    offset = Draw_area.xLeft + 2;
    max_height= 0;
    for( index = 0; buttons[index]; ++ index, offset += btn_width - 1 ) {
        #if 0
        if( index == 0 ) {
             add_button( hwnd, Draw_area.yTop, offset, buttons[index], NULL, &btn_height );
        } else
        #endif
        add_button( hwnd, Draw_area.yTop, offset, buttons[index], &btn_width, &btn_height );
        if( max_height < btn_height ) max_height = btn_height;
    }
    // Remove buttons from drawing area
    Draw_area.yTop -= max_height + 2;
    Button_height = max_height + 4;
}

MRESULT EXPENTRY main_proc(
    HWND                hwnd,
    ULONG               msg,
    MPARAM              mp1,
    MPARAM              mp2
) {
    if( msg == WM_CREATE ) {

        PCREATESTRUCT   pcreate;

        pcreate = ( CREATESTRUCT * ) mp2;
        Draw_area.xRight = pcreate->x + pcreate->cx;
        Draw_area.yTop = pcreate->y + pcreate->cy;
        Draw_area.yBottom = pcreate->y;
        Draw_area.xLeft = pcreate->x;
//      WinQueryWindowRect( hwnd, &Draw_area );

    } else if( msg == WM_COMMAND || ( msg == WM_CONTROL &&
                                SHORT2FROMMP( mp1 ) != BN_PAINT ) ) {

        switch( SHORT1FROMMP( mp1 ) ) {

        case IDM_EXIT:
            new_display();
            WinPostMsg( hwnd, WM_QUIT, 0L, 0L );
            return( NULL );

        case IDM_ROTATE_LEFT:
            new_display();
            add_wrap( THREE_D_HORZ_ANGLE, -15, 0, 360 );
            break;
        case IDM_ROTATE_RIGHT:
            new_display();
            add_wrap( THREE_D_HORZ_ANGLE, 15, 0, 360 );
            break;
        case IDM_ROTATE_UP:
            new_display();
            add_range( THREE_D_VERT_ANGLE, 5, -90, 90 );
            break;
        case IDM_ROTATE_DOWN:
            new_display();
            add_range( THREE_D_VERT_ANGLE, -5, -90, 90 );
            break;
        case IDM_MOVE_IN:
            new_display();
            add_range(THREE_D_ZOOM, 10, 5, 200 );
            break;
        case IDM_MOVE_OUT:
            new_display();
            add_range( THREE_D_ZOOM, -10, 5, 200 );
            break;
        case IDM_MORE_PERSPECTIVE:
            new_display();
            add_range( THREE_D_PERSPECTIVE, 10, 5, 200 );
            break;
        case IDM_LESS_PERSPECTIVE:
            new_display();
            add_range( THREE_D_PERSPECTIVE, -10, 5, 200 );
            break;
        case IDM_MORE_CONTRAST:
            new_display();
            add_range( THREE_D_CONTRAST, 10, 0, 100 );
            break;
        case IDM_LESS_CONTRAST:
            new_display();
            add_range( THREE_D_CONTRAST, -10, 0, 100 );
            break;
        case IDM_MORE_BRIGHTNESS:
            new_display();
            add_range( THREE_D_BRIGHTNESS, 10, 0, 100 );
            break;
        case IDM_LESS_BRIGHTNESS:
            new_display();
            add_range( THREE_D_BRIGHTNESS, -10, 0, 100 );
            break;
        case IDM_LIGHT_HORZ_LEFT:
            new_display();
            three_d_set( hThree_d, THREE_D_LIGHT_HORZ, 1 );
            break;
        case IDM_LIGHT_HORZ_MIDDLE:
            new_display();
            three_d_set( hThree_d, THREE_D_LIGHT_HORZ, 2 );
            break;
        case IDM_LIGHT_HORZ_RIGHT:
            new_display();
            three_d_set( hThree_d, THREE_D_LIGHT_HORZ, 3 );
            break;
        case IDM_LIGHT_VERT_TOP:
            new_display();
            three_d_set( hThree_d, THREE_D_LIGHT_VERT, 1 );
            break;
        case IDM_LIGHT_VERT_MIDDLE:
            new_display();
            three_d_set( hThree_d, THREE_D_LIGHT_VERT, 2 );
            break;
        case IDM_LIGHT_VERT_BOTTOM:
            new_display();
            three_d_set( hThree_d, THREE_D_LIGHT_VERT, 3 );
            break;
        case IDM_LIGHT_DEPTH_FRONT:
            new_display();
            three_d_set( hThree_d, THREE_D_LIGHT_DEPTH, 1 );
            break;
        case IDM_LIGHT_DEPTH_MIDDLE:
            new_display();
            three_d_set( hThree_d, THREE_D_LIGHT_DEPTH, 2 );
            break;
        case IDM_LIGHT_DEPTH_BACK:
            new_display();
            three_d_set( hThree_d, THREE_D_LIGHT_DEPTH, 3 );
            break;
        default:
            ;
        }
        WinInvalidateRect( hwnd, &Draw_area,
                        SHORT1FROMMP( mp1 ) >= IDM_FIRST_REQUIRING_CLEAR );

    } else if( msg == WM_DESTROY ) {
        WinPostMsg( hwnd, WM_QUIT, 0L, 0L );
        return( 0 );

    } else if( msg == WM_PAINT ) {
        draw_stuff( hwnd );
        if( Auto_run ) {
            if( Auto_run == 5 ) {
                WinPostMsg( hwnd, WM_QUIT, 0L, 0L );
            } else {
                long count;
                int will_be_one;
                will_be_one = 1;
                for(count = 0; count < 1000000; ++ count ) will_be_one ^= 1;

                Auto_run += will_be_one;
                WinPostMsg( hwnd, WM_COMMAND,
                        MPFROMSHORT( IDM_ROTATE_LEFT ), 0L );
            }
        }
        return( 0 );

    } else if( msg == WM_SIZE ) {
        WinQueryWindowRect( hwnd, &Draw_area );
        Draw_area.yTop -= Button_height;
        WinInvalidateRect( hwnd, NULL, TRUE );
        return( 0 );

    } else if( msg == WM_MEASUREITEM ) {
        return( (MRESULT) measure_button( hwnd, mp1, mp2 ) );

    } else if( msg == WM_CONTROL && SHORT2FROMMP( mp1 ) == BN_PAINT ) {
        draw_button( mp1, mp2 );
        return( (MRESULT) TRUE );

    }

    return( (MRESULT) WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
}

static BOOL init_instance( int show )
{
    HWND        frame_hwnd;
    ULONG       flags = FCF_SIZEBORDER | FCF_TITLEBAR
                | FCF_SYSMENU | FCF_MENU | FCF_MINMAX | FCF_SHELLPOSITION;

    frame_hwnd = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE,
                &flags, "DrawDemo", "Draw a Picture", WS_VISIBLE,
                0, 200, &hwnd );

    /* If window could not be created, return "failure" */

    if( hwnd == 0 || frame_hwnd == 0 ) {
        return (FALSE);
    }

    /* Make the window visible; update its client area; and return "success" */

    WinSetWindowPos( frame_hwnd, HWND_TOP, 50, 50, 200, 200, show );

    make_buttons( hwnd );

    WinShowWindow( frame_hwnd, TRUE );
    WinUpdateWindow( frame_hwnd );
    return( TRUE );
}

BOOL init_app( HAB hab )
{
    /* Register the window class and return success/failure code. */
    return( WinRegisterClass( hab, "DrawDemo", (PFNWP) main_proc, NULL, 0 ) );
}

int main(
/*******/
    void
) {
    QMSG                qmsg;

    Main_hab = WinInitialize( 0 );
    if( !Main_hab ) {
        return( FALSE );
    }

    Main_hmq = WinCreateMsgQueue( Main_hab, 0 );
    if( !Main_hmq ) {
        return( FALSE );
    }

    if( !init_app( Main_hab ) ) {
        return( FALSE );
    }

    /* Perform initializations that apply to a specific instance */

    three_d_init();
    define_room();
    if (!init_instance( SWP_SHOW | SWP_ACTIVATE ) ) {
        finish_room();
        three_d_fini();
        return( FALSE );
    }

    /* Acquire and dispatch messages until a WM_QUIT message is received. */

    while( WinGetMsg( Main_hab, &qmsg, NULL, NULL, NULL) ) {
        WinDispatchMsg( Main_hab, &qmsg );
    }
    finish_room();
    three_d_fini();

    if( Main_hmq ) {
      WinDestroyMsgQueue( Main_hmq );
    }
    if( Main_hab ) {
      WinTerminate( Main_hab );
    }

    DosExit( EXIT_PROCESS, 0);

    return( SHORT1FROMMP( qmsg.mp1 ) );
}
