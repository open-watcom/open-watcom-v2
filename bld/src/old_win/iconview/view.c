#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include "view.h"
#include "icon.h"
#include "mdi.h"

char     FrameClass[32]="FrameClass";
char     IconClass[32]="IconClass";

LONG _EXPORT FAR PASCAL FrameProc( HWND, unsigned, WORD, LONG );
LONG _EXPORT FAR PASCAL MdiIconProc( HWND, unsigned, WORD, LONG );
static BOOL InitApplication( HANDLE );
static BOOL InitInstance( int );
static void DoCmdLine( LPSTR cmdline );

HWND            FrameWindow;    /* The classic MDI frame window     */
HWND            ClientWindow;   /* And the client window attached   */
HANDLE          Instance;       /* Convenient to have handy         */

/*
 * WinMain - initialization, parsing of the command line, message loop
 */

int PASCAL WinMain( HANDLE hinst, HANDLE prev_inst, LPSTR cmdline, int cmdshow )
{
    MSG         msg;

    Instance = hinst;
    prev_inst = prev_inst;      /* shut up warning */
#ifdef __WINDOWS_386__
    sprintf( FrameClass,"FrameClass%d", hinst );
    sprintf( IconClass,"IconClass%d", hinst );
#else
    if( !prev_inst )
#endif
        if( !InitApplication( hinst ) ) return( FALSE );

    if( !InitInstance( cmdshow ) ) return( FALSE );

    DoCmdLine( cmdline );

    while( GetMessage( &msg, NULL, NULL, NULL ) ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    return( msg.wParam );

} /* WinMain */


/*
 * DoCmdLine: This routine parses the command line by looking for anything
 * which doesn't consist of white space and interpreting it as either a filename
 * of an icon to load or a switch. This icon will be loaded as a normal sized
 * MDI window unless the -m or -i flags were specified, in which case they are
 * shown in an initial maximized or iconized state.
 */

static void DoCmdLine( LPSTR cmdline )
{
    char        command[255], buffer[80];
    char        *p, *s;
    long        open_flags = 0;
    HWND        hwnd;
    FILE        *fp;

    _fstrcpy( command, cmdline );
    for( p = command; *p; p++ ) {
        if( !isspace( *p ) ) {
            if( *p == '-' || *p == '/' ) {
                switch( *++p ) {
                case 'm':
                    open_flags = WS_MAXIMIZE;
                    break;
                case 'i':
                    open_flags = WS_ICONIC;
                    break;
                default:
                    wsprintf( buffer, "Unknown option - %c", *p );
                    if( MessageBox( FrameWindow, buffer, NULL,
                        MB_OKCANCEL ) == IDCANCEL ) {
                        return;
                    }
                    open_flags = 0;
                }
            } else {
                for( s = p; *s && !isspace( *s ); s++ );
                *s = '\0';      /* we have a filename so display it and quit */
                fp = fopen( p, "rb" );
                if( fp == NULL ) {
                    wsprintf( buffer, "Could not open icon file %s.", p );
                    if( MessageBox( FrameWindow, buffer, NULL,
                        MB_OKCANCEL ) == IDOK ) {
                        p++;            /* skip the null we stuck in there */
                        continue;
                    }
                    return;
                }
                hwnd = MdiReadIcon( fp, p, p, open_flags );
                fclose( fp );
                ShowWindow( hwnd, SW_SHOW );
                UpdateWindow( hwnd );
                return;
            }
        }
    }
} /* DoCmdLine */

/*
 * InitApplication - register the frame and mdi child window classes
 */

static BOOL InitApplication( HANDLE inst )
{
    WNDCLASS    wc;
    BOOL        ret_code;

    /*
     * set up and register the frame window class
     */

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (LPVOID) FrameProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = inst;
    wc.hIcon = LoadIcon( inst, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = COLOR_APPWORKSPACE + 1;
    wc.lpszMenuName = "IconMenu";
    wc.lpszClassName = FrameClass;
    ret_code = RegisterClass( &wc );

    if( !ret_code ) return( FALSE );

    /*
     * set up and register the icon (MDI child) window class
     */

    wc.lpfnWndProc = (LPVOID) MdiIconProc;
    wc.cbWndExtra = sizeof( void far * );   /* assume worst case */
    wc.lpszMenuName = NULL;
    wc.lpszClassName = IconClass;
    ret_code = RegisterClass( &wc );

    return( ret_code );

} /* InitApplication */

/*
 * InitInstance - do work required for every instance of the application:
 *                create the frame window, initialize data
 */

static BOOL InitInstance( int cmdshow )
{
    /*
     * create the frame window
     */
    FrameWindow = CreateWindow(
        FrameClass,             /* class */
        "WATCOM Icon Viewer",   /* caption */
        WS_OVERLAPPEDWINDOW,    /* style */
        CW_USEDEFAULT,          /* init. x pos */
        CW_USEDEFAULT,          /* init. y pos */
        CW_USEDEFAULT,          /* init. x size */
        CW_USEDEFAULT,          /* init. y size */
        NULL,                   /* parent window */
        NULL,                   /* menu handle */
        Instance,               /* program handle */
        NULL                    /* create parms */
        );

    if( !FrameWindow ) return( FALSE );

    /*
     * display window
     */
    ShowWindow( FrameWindow, cmdshow );
    UpdateWindow( FrameWindow );

    return( TRUE );

} /* InitInstance */

/*
 * AboutProc - processes messages for the about dialogue.
 */
BOOL _EXPORT FAR PASCAL AboutProc( HWND hwnd, unsigned msg,
                                                WORD wparam, LONG lparam )
{
    lparam = lparam;                    /* turn off warning */

    switch( msg ) {
    case WM_INITDIALOG:
        return( TRUE );

    case WM_COMMAND:
        if( wparam == IDOK ) {
            EndDialog( hwnd, TRUE );
            return( TRUE );
        }
        break;
    }
    return( FALSE );

} /* AboutProc */

/*
 * FrameProc is the window procedure for the Frame window. It handles tasks
 * such as opening files and killing off children. All menu commands are
 * handled in the WM_COMMAND section of this procedure.
 */

LONG _EXPORT FAR PASCAL FrameProc( HWND hwnd, unsigned msg,
                                    WORD wparam, LONG lparam )
{
    CLIENTCREATESTRUCT      ccs;
    FARPROC                 proc;
    HWND                    active_MDI_child;
    HDC                     hdc;
    HMENU                   menu;

    switch( msg ) {
    case WM_CREATE:
        ccs.hWindowMenu = GetSubMenu( GetMenu( hwnd ), WINDOW_MENU_POS );
        ccs.idFirstChild = IDM_FIRSTCHILD;
        ClientWindow = CreateWindow( "mdiclient", 0L,
            WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
            0, 0, 0, 0, hwnd, 0, Instance, (LPSTR) &ccs );
        SendMessage( hwnd, WM_COMMAND, IDM_COLORONCOLOR, 0L );
        break;
    case WM_COMMAND:
        switch( wparam ) {
        case IDM_OPEN:
            /* open a new MDI icon */
            MdiOpenIcon();
            break;
        case IDM_CLOSE:
            active_MDI_child = SendMessage( ClientWindow,
                                                WM_MDIGETACTIVE, 0, 0L );
            if( active_MDI_child ) {
                SendMessage( ClientWindow, WM_MDIDESTROY, active_MDI_child, 0L);
            }
            break;
        case IDM_EXIT:
            DestroyWindow( hwnd );
            break;
        case IDM_ABOUT:
            proc = MakeProcInstance( AboutProc, Instance );
            DialogBox( Instance, "AboutBox", hwnd, proc );
            FreeProcInstance( proc );
            break;
        case IDM_TILE:
            SendMessage( ClientWindow, WM_MDITILE, 0, 0L );
            break;
        case IDM_CASCADE:
            SendMessage( ClientWindow, WM_MDICASCADE, 0, 0L );
            break;
        case IDM_ARRANGE:
            SendMessage( ClientWindow, WM_MDIICONARRANGE, 0, 0L );
            break;
        case IDM_SWITCHICON:
            active_MDI_child = SendMessage( ClientWindow,
                                                WM_MDIGETACTIVE, 0, 0L );
            if( active_MDI_child ) {
                SendMessage( active_MDI_child, msg, wparam, lparam );
            }
            break;
        case IDM_BLACKONWHITE:
        case IDM_WHITEONBLACK:
        case IDM_COLORONCOLOR:
            hdc = GetDC( hwnd );
            menu = GetSubMenu( GetMenu( hwnd ), OPTION_MENU_POS );
            CheckMenuItem( menu, IDM_BLACKONWHITE, MF_BYCOMMAND |
                ( (wparam == IDM_BLACKONWHITE) ? MF_CHECKED : MF_UNCHECKED ) );
            CheckMenuItem( menu, IDM_COLORONCOLOR, MF_BYCOMMAND |
                ( (wparam == IDM_COLORONCOLOR) ? MF_CHECKED : MF_UNCHECKED ) );
            CheckMenuItem( menu, IDM_WHITEONBLACK, MF_BYCOMMAND |
                ( (wparam == IDM_WHITEONBLACK) ? MF_CHECKED : MF_UNCHECKED ) );
            SetStretchBltMode( hdc, wparam );
            ReleaseDC( hwnd, hdc );
            break;
        default:
            return( DefFrameProc( hwnd, ClientWindow, msg, wparam, lparam ) );
        }
        break;
    case WM_QUERYENDSESSION:

        /*
         * Normally, the careful MDI programmer would use EnumChildWindows
         * here to query each child document to see if he can be closed down
         * or not, but we know an icon can be closed so we just do it.
         */
         return( (LONG) TRUE );

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;
    default:
        return( DefFrameProc( hwnd, ClientWindow, msg, wparam, lparam ) );
    }
    return( 0L );
} /* FrameProc */
