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


#include <windows.h>
#include <string.h>
#include "drawmenu.h"
#include "button.h"
#include "room.h"
#include "threed.h"
#include "win1632.h"

static three_d_handle hThree_d;

static RECT Draw_area;

static int Auto_run = 0;

#if !defined(__WINDOWS_386__)
#define _lparam_pointer( lp )(void FAR *) lp
#else

#define _lparam_pointer( lp ) MK_FP32( (void *)lp )

// Show how to load & use a 16-bit DLL from a 32-bit Windows program

static HINSTANCE hThreedDLL = NULL;
static FARPROC three_d_init_Proc = NULL;
static FARPROC three_d_fini_Proc = NULL;
static FARPROC three_d_begin_Proc = NULL;
static FARPROC three_d_box_Proc = NULL;
static FARPROC three_d_draw_Proc = NULL;
static FARPROC three_d_end_Proc = NULL;
static FARPROC three_d_get_Proc = NULL;
static FARPROC three_d_set_Proc = NULL;

void ENTRYPOINT three_d_init()
// Use the DLL initialization entry point to load the DLL and get the
// address of the entry points
{
    hThreedDLL = LoadLibrary( "win_3d.dll" );
    if( hThreedDLL < HINSTANCE_ERROR ) {
        hThreedDLL = NULL;
        return;
    }
    three_d_init_Proc  = GetProcAddress( hThreedDLL, "three_d_init" );
    three_d_fini_Proc  = GetProcAddress( hThreedDLL, "three_d_fini" );
    three_d_begin_Proc = GetProcAddress( hThreedDLL, "three_d_begin" );
    three_d_box_Proc   = GetProcAddress( hThreedDLL, "three_d_box" );
    three_d_draw_Proc  = GetProcAddress( hThreedDLL, "three_d_draw" );
    three_d_end_Proc   = GetProcAddress( hThreedDLL, "three_d_end" );
    three_d_get_Proc   = GetProcAddress( hThreedDLL, "three_d_get" );
    three_d_set_Proc   = GetProcAddress( hThreedDLL, "three_d_set" );

    // Invoke the real DLL entry point
    _Call16( three_d_init_Proc, "" );
}

void ENTRYPOINT three_d_fini()
// Use the DLL finish routine to free up the DLL
{
    if( hThreedDLL == NULL ) return;
    _Call16( three_d_fini_Proc, "" );
    FreeLibrary( hThreedDLL );
    hThreedDLL = NULL;
}

three_d_handle ENTRYPOINT three_d_begin( HDC dc, RECT * draw_area )
{
    if( hThreedDLL == NULL ) return( 0 );
    return( (short) _Call16( three_d_begin_Proc, "wp", dc, draw_area ) );
}

void ENTRYPOINT three_d_box( three_d_handle handle,
                  float x1, float x2, float y1, float y2, float z1, float z2,
                  unsigned red, unsigned green, unsigned blue )
{
    if( hThreedDLL == NULL ) return;
    // Use a trick to pass floats rather than as doubles
    // (_Call16 is a varargs function, which makes all float
    //  parameters passed as doubles)
    #define FLT(x)      (*(long *) &(x))
    _Call16( three_d_box_Proc, "wddddddwww", handle,
                FLT(x1), FLT(x2), FLT(y1), FLT(y2), FLT(z1), FLT(z2),
                red, green, blue );
}

void ENTRYPOINT three_d_draw( three_d_handle handle )
{
    if( hThreedDLL == NULL ) return;
    _Call16( three_d_draw_Proc, "w", handle );
}

void ENTRYPOINT three_d_end( three_d_handle handle )
{
    if( hThreedDLL == NULL ) return;
    _Call16( three_d_end_Proc, "w", handle );
}

// Attribute setting & changing

int ENTRYPOINT three_d_get( three_d_handle handle, three_d_attr attr )
{
    if( hThreedDLL == NULL ) return( 0 );
    return( (short) _Call16( three_d_get_Proc, "ww", handle, attr ) );
}

void ENTRYPOINT three_d_set( three_d_handle handle, three_d_attr attr,
                int new_val )
{
    if( hThreedDLL == NULL ) return;
    _Call16( three_d_set_Proc, "www", handle, attr, new_val );
}

#endif // __WINDOWS_386__

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

static void draw_stuff( HWND hwnd )
{
    HDC                 win_dc;
    PAINTSTRUCT         paint;
    RECT                intersect;

    win_dc = BeginPaint( hwnd, &paint );
    if( IntersectRect( &intersect, &paint.rcPaint, &Draw_area ) ) {
        hThree_d = three_d_begin( win_dc, &Draw_area );
        draw_room();
        three_d_draw( hThree_d );
        three_d_end( hThree_d );
    }
    EndPaint( hwnd, &paint );
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

    Draw_area.top += 2;
    offset = Draw_area.left + 2;
    max_height= 0;
    for( index = 0; buttons[index]; ++ index, offset += btn_width - 1 ) {
        #if 0
        if( index == 0 ) {
             add_button( hwnd, Draw_area.top, offset, buttons[index], NULL, &btn_height );
        } else
        #endif
        add_button( hwnd, Draw_area.top, offset, buttons[index], &btn_width, &btn_height );
        if( max_height < btn_height ) max_height = btn_height;
    }
    // Remove buttons from drawing area
    Draw_area.top += max_height + 2;
}

LRESULT CALLBACK main_proc(
    HWND                hwnd,
    UINT                msg,
    WPARAM              wparam,
    LPARAM              lparam
) {
    WORD                cmd;

    switch( msg ) {

    case WM_CREATE: {
        GetClientRect( hwnd, &Draw_area );
        make_buttons( hwnd );
        break;
    }

    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( LOWORD( cmd ) ) {

        case IDM_EXIT:
            PostQuitMessage( 0 );
            return( NULL );

        case IDM_ROTATE_LEFT:
            add_wrap( THREE_D_HORZ_ANGLE, -15, 0, 360 );
            break;
        case IDM_ROTATE_RIGHT:
            add_wrap( THREE_D_HORZ_ANGLE, 15, 0, 360 );
            break;
        case IDM_ROTATE_UP:
            add_range( THREE_D_VERT_ANGLE, 5, -90, 90 );
            break;
        case IDM_ROTATE_DOWN:
            add_range( THREE_D_VERT_ANGLE, -5, -90, 90 );
            break;
        case IDM_MOVE_IN:
            add_range(THREE_D_ZOOM, 10, 5, 200 );
            break;
        case IDM_MOVE_OUT:
            add_range( THREE_D_ZOOM, -10, 5, 200 );
            break;
        case IDM_MORE_PERSPECTIVE:
            add_range( THREE_D_PERSPECTIVE, 10, 5, 200 );
            break;
        case IDM_LESS_PERSPECTIVE:
            add_range( THREE_D_PERSPECTIVE, -10, 5, 200 );
            break;
        case IDM_MORE_CONTRAST:
            add_range( THREE_D_CONTRAST, 10, 0, 100 );
            break;
        case IDM_LESS_CONTRAST:
            add_range( THREE_D_CONTRAST, -10, 0, 100 );
            break;
        case IDM_MORE_BRIGHTNESS:
            add_range( THREE_D_BRIGHTNESS, 10, 0, 100 );
            break;
        case IDM_LESS_BRIGHTNESS:
            add_range( THREE_D_BRIGHTNESS, -10, 0, 100 );
            break;
        case IDM_LIGHT_HORZ_LEFT:
            three_d_set( hThree_d, THREE_D_LIGHT_HORZ, 1 );
            break;
        case IDM_LIGHT_HORZ_MIDDLE:
            three_d_set( hThree_d, THREE_D_LIGHT_HORZ, 2 );
            break;
        case IDM_LIGHT_HORZ_RIGHT:
            three_d_set( hThree_d, THREE_D_LIGHT_HORZ, 3 );
            break;
        case IDM_LIGHT_VERT_TOP:
            three_d_set( hThree_d, THREE_D_LIGHT_VERT, 1 );
            break;
        case IDM_LIGHT_VERT_MIDDLE:
            three_d_set( hThree_d, THREE_D_LIGHT_VERT, 2 );
            break;
        case IDM_LIGHT_VERT_BOTTOM:
            three_d_set( hThree_d, THREE_D_LIGHT_VERT, 3 );
            break;
        case IDM_LIGHT_DEPTH_FRONT:
            three_d_set( hThree_d, THREE_D_LIGHT_DEPTH, 1 );
            break;
        case IDM_LIGHT_DEPTH_MIDDLE:
            three_d_set( hThree_d, THREE_D_LIGHT_DEPTH, 2 );
            break;
        case IDM_LIGHT_DEPTH_BACK:
            three_d_set( hThree_d, THREE_D_LIGHT_DEPTH, 3 );
            break;
        default:
            ;
        }
        InvalidateRect( hwnd, &Draw_area, cmd >= IDM_FIRST_REQUIRING_CLEAR );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return( 0 );

    case WM_PAINT:
        draw_stuff( hwnd );
        if( Auto_run ) {
            if( Auto_run == 5 ) {
                PostQuitMessage( 0 );
            } else {
                long count;
                int will_be_one;
                will_be_one = 1;
                for(count = 0; count < 1000000; ++ count ) will_be_one ^= 1;

                Auto_run += will_be_one;
                PostMessage( hwnd, WM_COMMAND,
                                GET_WM_COMMAND_MPS( IDM_ROTATE_LEFT, 0, 0 ) );
            }
        }
        return( 0 );

    case WM_SIZE:
        if( wparam != SIZEICONIC ) {
            int old_left = Draw_area.left;

            GetClientRect( hwnd, &Draw_area );
            Draw_area.left = old_left;
            InvalidateRect( hwnd, NULL, TRUE );
            return( 0 );
        }
        break;

    case WM_MEASUREITEM:
        measure_button( hwnd, wparam, _lparam_pointer( lparam ) );
        return( TRUE );

    case WM_DRAWITEM:
        draw_button( wparam, _lparam_pointer( lparam ) );
        return( TRUE );

    }

    return( DefWindowProc( hwnd, msg, wparam, lparam ) );
}

static BOOL init_instance( HANDLE inst, int show )
{
    HWND        hwnd;

    hwnd  = CreateWindow(
        "DrawDemo",
        "Draw a Picture",
        WS_OVERLAPPEDWINDOW,            /* Window style.                      */
        CW_USEDEFAULT,                  /* Default horizontal position.       */
        CW_USEDEFAULT,                  /* Default vertical position.         */
        CW_USEDEFAULT,                  /* Default width.                     */
        CW_USEDEFAULT,                  /* Default height.                    */
        NULL,                           /* Overlapped windows have no parent. */
        NULL,                           /* Use the window class menu.         */
        inst,                           /* This instance owns this window.    */
        NULL                            /* Pointer not needed.                */
    );

    /* If window could not be created, return "failure" */

    if( hwnd == NULL ) {
        return (FALSE);
    }

    /* Make the window visible; update its client area; and return "success" */

    ShowWindow( hwnd, show );
    // ShowWindow( hwnd, SW_SHOWMAXIMIZED );
    UpdateWindow( hwnd );
    return( TRUE );
}

BOOL init_app( HANDLE inst )
{
    WNDCLASS            wc;

    /* Fill in window class structure with parameters that describe the       */
    /* main window.                                                           */

    wc.style = NULL;
    wc.lpfnWndProc = (WNDPROC) MakeProcInstance( (FARPROC) main_proc, inst );
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = inst;
    wc.hIcon = LoadIcon(NULL, "DrawIcon" );
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = "DrawMenu";
    wc.lpszClassName = "DrawDemo";

    /* Register the window class and return success/failure code. */

    return (RegisterClass(&wc));

}

int PASCAL WinMain(
/*****************/
    HANDLE              inst,
    HANDLE              prev_inst,
    LPSTR               cmd_line,
    int                 show
) {
    MSG                 msg;

    while( *cmd_line == ' ' ) ++ cmd_line;
    if( stricmp( cmd_line, "auto" ) == 0 ) {
        Auto_run = 1;
    } else {
        Auto_run = 0;
    }
    if( !prev_inst ) {
        if( !init_app( inst ) ) {
            return( FALSE );
        }
    }

    /* Perform initializations that apply to a specific instance */

    three_d_init();
    define_room();
    if (!init_instance( inst, show ) ) {
        finish_room();
        three_d_fini();
        return( FALSE );
    }

    /* Acquire and dispatch messages until a WM_QUIT message is received. */

    while( GetMessage(&msg, NULL, NULL, NULL) ) {
        // if( TranslateAccelerators( Main_wnd, haccel, &msg ) ) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    finish_room();
    three_d_fini();

    return (msg.wParam);           /* Returns the value from PostQuitMessage */
}
