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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <dos.h>
#include "winint.h"
#include "link.h"
#include "common.h"


static void listBoxOut( char *str, ... );

#ifndef __EDITOR__
static char             demoCaption[] = "Batcher Proxy";
static char             demoClass[] = "WININTCLASS";
#else
static char             demoCaption[] = "Editor Proxy";
static char             demoClass[] = "WININTECLASS";
#endif
static HANDLE           ourInstance;
static HWND             ourWindow;
static HWND             listBox;
static HWND             editControl;
static HFONT            fixedFont;
static BOOL             hasConnect;

/*
 * EnumFunc - enumerate fonts
 */
int CALLBACK EnumFunc( LPLOGFONT lf, LPTEXTMETRIC tm, UINT ftype, LPSTR data )
{
    tm = tm;
    ftype = ftype;
    data = data;

    if( !_fstricmp( lf->lfFaceName, "courier" ) ) {
        fixedFont = CreateFont(
            13,
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            lf->lfCharSet,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            lf->lfPitchAndFamily,
            lf->lfFaceName );
        return( 0 );
    }
    return( 1 );

} /* EnumFunc */

/*
 * getMonoFont - find a mono font
 */
static void getMonoFont( HDC hdc, HANDLE inst )
{
    LOGFONT     logfont;
    FARPROC     fp;

    inst = inst;

    fp = MakeProcInstance( (FARPROC) EnumFunc, inst );
    EnumFonts( hdc, NULL, (LPVOID) fp, NULL);
    FreeProcInstance( fp );

    if( fixedFont == NULL ) {
        fixedFont = GetStockObject( ANSI_FIXED_FONT );
        GetObject( fixedFont, sizeof( LOGFONT ), (LPSTR) &logfont );
        fixedFont = CreateFontIndirect( &logfont );
    }

} /* getMonoFont */

#define MAX_LINE        128
static char     lineBuff[ MAX_LINE ];
static int      lineOff;

static void listBoxOut( char *str, ... )
{
    char        buff[512];
    va_list     al;
    int         i,len,ch,pos;

    va_start( al, str );
    vsprintf( buff, str, al );
    va_end( al );

    len = strlen( buff );
    for( i=0;i<len;i++ ) {
        ch = buff[i];
        if( ch == '\n' ) {
            continue;
        }
        if( lineOff == MAX_LINE-2 || ch == '\r' ) {
            lineBuff[ lineOff ] = 0;
            pos = SendMessage( listBox, LB_ADDSTRING, 0, (LONG)(LPSTR)lineBuff );
            SendMessage( listBox, LB_SETCURSEL, pos, 0L );
            UpdateWindow( listBox );
            lineOff = 0;
            continue;
        }
        lineBuff[ lineOff++ ] = ch;
    }

} /* listBoxOut */

static LPVOID oldEditClassProc;
long CALLBACK EditSubClassProc( HWND hwnd, UINT msg, UINT wparam, LONG lparam )
{

    switch( msg ) {
    case WM_KEYDOWN:
        if( wparam == VK_RETURN ) {
            PostMessage( GetParent( hwnd ), WM_COMMAND, PRESSED_ENTER, 0 );
            return( NULL );
        }
        break;
    case WM_CHAR:
        if( wparam == '\r'|| wparam == '\t' ) {
            return( NULL ); // Ignore return and tab
        }
        break;
    }

    return( CallWindowProc( oldEditClassProc, hwnd, msg, wparam, lparam ) );

} /* SubClassProc */

static void createChildWindows( HWND parent )
{
    listBox = CreateWindow(
        "LISTBOX",          /* Window class name */
        "Messages",         /* Window caption */
        WS_CHILD | LBS_NOTIFY | WS_VSCROLL | WS_BORDER,/* Window style */
        LISTBOX_X,          /* Initial X position */
        LISTBOX_Y,          /* Initial Y position */
        0,              /* Initial X size */
        0,              /* Initial Y size */
        parent,         /* Parent window handle */
        (HANDLE) LIST_BOX,                  /* Window menu handle */
        ourInstance,        /* Program instance handle */
        NULL);              /* Create parameters */

    ShowWindow( listBox, SW_NORMAL );
    UpdateWindow( listBox );
    SendMessage( listBox, WM_SETFONT, (UINT) fixedFont, 0L );

    editControl = CreateWindow(
        "EDIT",     /* Window class name */
        "",         /* Window caption */
        WS_CHILD | ES_AUTOHSCROLL| WS_BORDER,/* Window style */
        0,          /* Initial X position */
        0,          /* Initial Y position */
        0,              /* Initial X size */
        0,              /* Initial Y size */
        parent,         /* Parent window handle */
        (HANDLE) EDIT_CONTROL,              /* Window menu handle */
        ourInstance,        /* Program instance handle */
        NULL);              /* Create parameters */

    ShowWindow( editControl, SW_NORMAL );
    UpdateWindow( editControl );
    oldEditClassProc = (FARPROC) GetWindowLong( editControl, GWL_WNDPROC );
    SetWindowLong( editControl, GWL_WNDPROC, (LONG) EditSubClassProc );

} /* createChildWindows */

/*
 * resizeChildWindows - make list box new size, based on height/width of parent
 *                client area.
 */
void resizeChildWindows( WORD width, WORD height )
{

    height = height-LISTBOX_Y-5;
    if( height < LISTBOX_Y ) height = LISTBOX_Y;
    width -= 2* LISTBOX_X;

    MoveWindow( listBox, LISTBOX_X, LISTBOX_Y, width, height, TRUE );
    MoveWindow( editControl, LISTBOX_X, LISTBOX_Y-40, width, 25, TRUE );

} /* resizeChildWindows */

BOOL __export FAR PASCAL AboutDlgProc( HWND hwnd, UINT msg,
                                UINT wparam, LONG lparam )
{
    lparam = lparam;

    switch( msg ) {
    case WM_INITDIALOG:
        return( TRUE );

    case WM_COMMAND:
        if( LOWORD( wparam ) == IDOK ) {
            EndDialog( hwnd, TRUE );
            return( TRUE );
        }
        break;

    case WM_CLOSE:
        EndDialog( hwnd, TRUE );
        return( TRUE );
    }
    return( FALSE );

} /* AboutDlgProc */

static void paintWindow( HWND  hwnd )
{
    HDC         hdc;
    PAINTSTRUCT ps;

    hdc = BeginPaint( hwnd, &ps );
    EndPaint( hwnd, &ps );

} /* paintWindow */

#ifndef __EDITOR__
static int hasOL;
#endif

LONG __export FAR PASCAL WindowProc( HWND hwnd, UINT msg,
                                     UINT wparam, LONG lparam )
{
    FARPROC     fp;
    char        buff[MAX_BUFF+1];
    int         len;
    HDC         hdc;
    MSG         peek;

    switch( msg ) {
    case WM_CREATE:
        hdc = GetDC( hwnd );
        getMonoFont( hdc, ourInstance );
        ReleaseDC(hwnd, hdc);
        createChildWindows( hwnd );
        break;
    case WM_TIMER:
        if( VxDConnect() ) {
            KillTimer( ourWindow, TIMER_ID );
            while( PeekMessage( &peek, ourWindow, WM_TIMER, WM_TIMER, PM_REMOVE ) );
            listBoxOut( "Got connection!\r\n" );
            hasConnect = TRUE;
        } else {
            listBoxOut( "No connection\r\n" );
        }
        break;
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case PRESSED_ENTER:
            len = GetWindowText( editControl, buff, MAX_BUFF-1 );
            SetWindowText( editControl, "" );
            if( hasConnect ) {
#ifndef __EDITOR__
                if( !stricmp( buff, ":ol" ) ) {
                    if( hasOL ) {
                        VxDPut( END_OPEN_LIST, sizeof( END_OPEN_LIST )+1 );
                        hasOL = 0;
                    } else {
                        VxDPut( NEW_OPEN_LIST, sizeof( NEW_OPEN_LIST )+1 );
                        hasOL = 1;
                    }
                    VxDGet( buff, sizeof( buff ) );
                } else if( hasOL ) {
                    VxDPut( buff, strlen( buff )+1 );
                    VxDGet( buff, sizeof( buff ) );
                } else {
#endif
                    VxDPut( buff, len+1 );
                    while( 1 ) {
                        len = VxDGet( buff, sizeof( buff ) );
                        buff[len] = 0;
                        if( !strnicmp( buff, GET_REAL_NAME, sizeof( GET_REAL_NAME )-1 ) ) {
                            listBoxOut( "REQUEST: %s\r\n", &buff[ sizeof( GET_REAL_NAME ) ] );
                            sprintf( buff,"y.c" );
                            VxDPut( buff, strlen( buff ) + 1 );
                            continue;
                        }
                        if( !stricmp( buff,TERMINATE_COMMAND_STR ) ) {
                            break;
                        }
                        listBoxOut( "(%d,%d): %s", len, strlen( buff ), buff );
                    }
#ifndef __EDITOR__
                }
#endif
            }
            break;

        case MENU_CTRL_BREAK:
            VxDRaiseInterrupt( 0x1b );
            break;

        case MENU_ABOUT:
            fp = MakeProcInstance( AboutDlgProc, ourInstance );
            DialogBox( ourInstance,"AboutBox", hwnd, (LPVOID) fp );
            FreeProcInstance( fp );
            break;

        case MENU_EXIT:
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        }
        break;
    case WM_PAINT:
        paintWindow( hwnd );
        break;

    case WM_SIZE:
        resizeChildWindows( LOWORD( lparam ), HIWORD( lparam ) );
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );

    case WM_CLOSE:
        VxDPut( TERMINATE_CLIENT_STR, sizeof( TERMINATE_CLIENT_STR )+1 );
        while( 1 ) {
            if( VxDUnLink() == 0 ) {
                break;
            }
            Yield();
        }
        DestroyWindow( hwnd );
        break;
    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0L );

} /* WindowProc */


static BOOL firstInstance( void )
{
    WNDCLASS    wc;
    BOOL        rc;


    if( !VxDPresent() ) {
        MessageBox( NULL,"Cannot run, WDEBUG.386 not present!",
                        "Startup Error", MB_OK | MB_TASKMODAL );
        return( FALSE );
    }


    /*
     * set up and register window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (LPVOID) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = ourInstance;
    wc.hIcon = LoadIcon( ourInstance, "DemoIcon" );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = "DemoMenu";
    wc.lpszClassName = demoClass;
    rc = RegisterClass( &wc );

    return( rc );

} /* firstInstance */

static BOOL anyInstance( void )
{
    int         sysx,sysy;
    const char  *res;

    sysx = GetSystemMetrics( SM_CXSCREEN );
    sysy = GetSystemMetrics( SM_CYSCREEN );

    /*
     * create main window
     */
    ourWindow = CreateWindow(
        demoClass,              /* class */
        demoCaption,
        WS_OVERLAPPEDWINDOW,    /* style */
        sysx/4,                 /* init. x pos */
        sysy/4,                 /* init. y pos */
        sysx/2,                 /* init. x size */
        sysy/2,                 /* init. y size */
        NULL,                   /* parent window */
        NULL,                   /* menu handle */
        ourInstance,            /* program handle */
        NULL                    /* create parms */
        );

    if( !ourWindow ) return( FALSE );

    ShowWindow( ourWindow, SW_NORMAL );
    UpdateWindow( ourWindow );
#ifndef __EDITOR__
    res = VxDLink( LINK_NAME );
#else
    res = VxDLink( EDITOR_LINK_NAME );
#endif
    if( res != NULL ) {
        MessageBox( NULL,res,"Link Error", MB_OK | MB_TASKMODAL );
        return( FALSE );
    }
    SetTimer( ourWindow, TIMER_ID, 500, 0L );

    return( TRUE );

} /* anyInstance */


int PASCAL WinMain( HANDLE this_inst, HANDLE prev_inst, LPSTR cmdline,
                    int cmdshow )
{
    MSG         msg;

    prev_inst = prev_inst;
    cmdline = cmdline;
    cmdshow = cmdshow;
    ourInstance = this_inst;

    if( !firstInstance() ) return( FALSE );
    if( !anyInstance() ) return( FALSE );

    while( GetMessage( &msg, NULL, NULL, NULL ) ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    return( msg.wParam );

} /* WinMain */
