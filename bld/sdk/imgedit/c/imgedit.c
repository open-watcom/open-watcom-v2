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
* Description:  Image Editor mainline.
*
****************************************************************************/


#include "imgedit.h"
#include <shellapi.h>
#include <io.h>
#include <stdarg.h>
#include "title.h"
#include "iedde.h"
#include "wrdll.h"
#ifdef __WATCOMC__
#include "clibint.h"
#endif

#ifdef __NT__
    #pragma library( "shell32.lib" )
#endif

#define DDE_OPT     "-DDE"
#define NEW_OPT     "/n"
#define NOTITLE_OPT "/notitle"
#define FUSION_OPT  "/fusion"

static char     className[] = "watimgedit";
static HICON    hBitmapIcon;
static HICON    hIconIcon;
static HICON    hCursorIcon;
static HCURSOR  handCursor;
static HANDLE   hAccel;

#if defined( __NT__ )
static HBRUSH   hBkBrush;
#endif

BOOL OpenNewFiles = FALSE;
BOOL FusionCalled = FALSE;
BOOL NoTitleScreen = FALSE;

/* set the WRES library to use compatible functions */
WResSetRtns( open, close, read, write, lseek, tell, MemAlloc, MemFree );

/*
 * imgEditInit - initialization
 */
static BOOL imgEditInit( HANDLE currinst, HANDLE previnst, int cmdshow )
{
    WNDCLASS    wc;
    HMENU       menu;
    HDC         hdc;
    BOOL        maximized;
    int         show_state;

    hdc = GetDC( NULL );
    ColorPlanes = GetDeviceCaps( hdc, PLANES );
    BitsPerPixel = GetDeviceCaps( hdc, BITSPIXEL );
    ReleaseDC( NULL, hdc );

    Instance = currinst;
    IEInitErrors( currinst );
    IEInitGlobalStrings();

    if( ImgedIsDDE ) {
        menu = LoadMenu( Instance, "IMGEDDDEMENU" );
    } else {
        menu = LoadMenu( Instance, "IMGEDMENU" );
    }

    hBitmapIcon = LoadIcon( Instance, "BitmapIcon" );
    hIconIcon = LoadIcon( Instance, "IconIcon" );
    hCursorIcon = LoadIcon( Instance, "CursorIcon" );
    hAccel = LoadAccelerators( Instance, "Accelerators" );

    IECtl3dInit( Instance );

#if defined( __NT__ )
    hBkBrush = CreateSolidBrush( GetSysColor( COLOR_BTNFACE ) );
#endif

    /*
     * set up window class
     */
    if( !previnst ) {
        wc.style = 0L;
        wc.lpfnWndProc = ImgEdFrameProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = Instance;
        wc.hIcon = LoadIcon( Instance, "APPLICON" );
        wc.hCursor = LoadCursor( (HANDLE)NULL, IDC_ARROW );
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = className;
        if( !RegisterClass( &wc ) ) {
            return( FALSE );
        }
    }

    /*
     * This is the child of the MDI frame window (of it's client window actually).
     */
    if( !previnst ) {
        wc.style = CS_BYTEALIGNWINDOW | CS_CLASSDC | CS_DBLCLKS;
        wc.lpfnWndProc = DrawAreaWinProc;
        wc.cbClsExtra = sizeof( HCURSOR );
        wc.cbWndExtra = 0;
        wc.hInstance = Instance;
        wc.hIcon = hBitmapIcon;
        wc.hCursor = LoadCursor( (HANDLE)NULL, IDC_ARROW );
        wc.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
        wc.lpszMenuName = NULL;
        wc.lpszClassName = DrawAreaClassB;
        if( !RegisterClass( &wc ) ) {
            return( FALSE );
        }
    }

    if( !previnst ) {
        wc.style = CS_BYTEALIGNWINDOW | CS_CLASSDC | CS_DBLCLKS;
        wc.lpfnWndProc = DrawAreaWinProc;
        wc.cbClsExtra = sizeof( HCURSOR );
        wc.cbWndExtra = 0;
        wc.hInstance = Instance;
        wc.hIcon = hIconIcon;
        wc.hCursor = LoadCursor( (HANDLE)NULL, IDC_ARROW );
        wc.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
        wc.lpszMenuName = NULL;
        wc.lpszClassName = DrawAreaClassI;
        if( !RegisterClass( &wc ) ) {
            return( FALSE );
        }
    }

    if( !previnst ) {
        wc.style = CS_BYTEALIGNWINDOW | CS_CLASSDC | CS_DBLCLKS;
        wc.lpfnWndProc = DrawAreaWinProc;
        wc.cbClsExtra = sizeof( HCURSOR );
        wc.cbWndExtra = 0;
        wc.hInstance = Instance;
        wc.hIcon = hCursorIcon;
        wc.hCursor = LoadCursor( (HANDLE)NULL, IDC_ARROW );
        wc.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
        wc.lpszMenuName = NULL;
        wc.lpszClassName = DrawAreaClassC;
        if( !RegisterClass( &wc ) ) {
            return( FALSE );
        }
    }

    if( !previnst ) {
        wc.style = 0L;
        wc.lpfnWndProc = ViewWindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = Instance;
        wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
        wc.hCursor = LoadCursor( (HANDLE)NULL, IDC_ARROW );
        wc.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
        wc.lpszMenuName = NULL;
        wc.lpszClassName = ViewWinClass;
        if( !RegisterClass( &wc ) ) {
            return( FALSE );
        }
    }

    if( !previnst ) {
        wc.style = 0L;
        wc.lpfnWndProc = ColorPalWinProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = Instance;
        wc.hIcon = NULL;
        wc.hCursor = LoadCursor( (HANDLE)NULL, IDC_ARROW );
#if defined( __NT__ )
        wc.hbrBackground = hBkBrush;
#else
        wc.hbrBackground = (HBRUSH)GetStockObject( LTGRAY_BRUSH );
#endif
        wc.lpszMenuName = NULL;
        wc.lpszClassName = PaletteClass;
        if( !RegisterClass( &wc ) ) {
            return( FALSE );
        }
    }

    handCursor = LoadCursor( Instance, "HandCursor" );
    if( !previnst ) {
        wc.style = CS_DBLCLKS;
        wc.lpfnWndProc = ColorsWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = Instance;
        wc.hIcon = NULL;
        wc.hCursor = handCursor;
#if defined( __NT__ )
        wc.hbrBackground = hBkBrush;
#else
        wc.hbrBackground = (HBRUSH)GetStockObject( LTGRAY_BRUSH );
#endif
        wc.lpszMenuName = NULL;
        wc.lpszClassName = "ColorsClass";
        if( !RegisterClass( &wc ) ) {
            return( FALSE );
        }
    }

    if( !previnst ) {
        wc.style = CS_DBLCLKS;
        wc.lpfnWndProc = ScreenWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = Instance;
        wc.hIcon = NULL;
        wc.hCursor = handCursor;
#if defined( __NT__ )
        wc.hbrBackground = hBkBrush;
#else
        wc.hbrBackground = (HBRUSH)GetStockObject( LTGRAY_BRUSH );
#endif
        wc.lpszMenuName = NULL;
        wc.lpszClassName = "ScreenClass";
        if( !RegisterClass( &wc ) ) {
            return( FALSE );
        }
    }

    if( !previnst ) {
        wc.style = 0L;
        wc.lpfnWndProc = CurrentWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = Instance;
        wc.hIcon = NULL;
        wc.hCursor = LoadCursor( (HANDLE)NULL, IDC_ARROW );
#if defined( __NT__ )
        wc.hbrBackground = hBkBrush;
#else
        wc.hbrBackground = (HBRUSH)GetStockObject( LTGRAY_BRUSH );
#endif
        wc.lpszMenuName = NULL;
        wc.lpszClassName = "CurrentClass";
        if( !RegisterClass( &wc ) ) {
            return( FALSE );
        }
    }

    if( !previnst ) {
        wc.style = 0L;
        wc.lpfnWndProc = BitmapPickProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = Instance;
        wc.hIcon = NULL;
        wc.hCursor = NULL;
#if defined( __NT__ )
        wc.hbrBackground = hBkBrush;
#else
        wc.hbrBackground = (HBRUSH)GetStockObject( LTGRAY_BRUSH );
#endif
        wc.lpszMenuName = NULL;
        wc.lpszClassName = BitmapPickClass;
        if( !RegisterClass( &wc ) ) {
            return( FALSE );
        }
    }

    /*
     * Now make the main window.
     */
    LoadImgedConfig();
    maximized = ImgedConfigInfo.ismaximized;

    InitPalette();
    HMainWindow = CreateWindow(
        className,                              /* Window class name */
        IEAppTitle,                             /* Window caption */
        WS_OVERLAPPEDWINDOW | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
        | WS_DLGFRAME,                          /* Window style */
        ImgedConfigInfo.x_pos,                  /* Initial X position */
        ImgedConfigInfo.y_pos,                  /* Initial Y position */
        ImgedConfigInfo.width,                  /* Initial X size */
        ImgedConfigInfo.height,                 /* Initial Y size */
        (HWND)NULL,                             /* Parent window handle */
        (HMENU)menu,                            /* Window menu handle */
        Instance,                               /* Program instance handle */
        NULL );                                 /* Create parameters */

    if( HMainWindow == NULL ) {
        return( FALSE );
    }

    if( maximized ) {
        if( cmdshow == SW_SHOW || cmdshow == SW_SHOWNORMAL ) {
            show_state = SW_SHOWMAXIMIZED;
        } else {
            show_state = cmdshow;
        }
    } else {
        show_state = cmdshow;
    }
    ShowWindow( HMainWindow, show_state );
    UpdateWindow( HMainWindow );

    if( !ImgedIsDDE && !NoTitleScreen ) {
        DisplayTitleScreen( Instance, HMainWindow, 2000, IEAppTitle );
    }

    CreateColorPal();
    InitTools( HMainWindow );
    GrayEditOptions();

#ifdef __NT__
    DragAcceptFiles( HMainWindow, TRUE );
#endif
    //SetActiveWindow( HMainWindow );
    BringWindowToTop( HMainWindow );
    return( TRUE );

} /* imgEditInit */

/*
 * parseCmdLine - parse the command line to see if there is an image to open right away
 */
static void parseCmdLine( int count, char **cmdline )
{
    char        fname[_MAX_PATH];
    int         i;

    for( i = 1; i < count; i++ ) {
        if( !stricmp( cmdline[i], DDE_OPT ) ) {
            continue;
        }
        if( !stricmp( cmdline[i], NEW_OPT ) ) {
            continue;
        }
        if( !stricmp( cmdline[i], NOTITLE_OPT ) ) {
            continue;
        }
        if( !stricmp( cmdline[i], FUSION_OPT ) ) {
            continue;
        }
        strcpy( fname, cmdline[i] );
        OpenFileOnStart( fname );
    }

} /* parseCmdLine */

/*
 * parseArgs
 */
static void parseArgs( int count, char **cmdline )
{
    int         i;

    for( i = 1; i < count; i++ ) {
        if( !stricmp( cmdline[i], DDE_OPT ) ) {
            ImgedIsDDE = TRUE;
            continue;
        }
        if( !stricmp( cmdline[i], NEW_OPT ) ) {
            OpenNewFiles = TRUE;
            continue;
        }
        if( !stricmp( cmdline[i], NOTITLE_OPT ) ) {
            NoTitleScreen = TRUE;
            continue;
        }
        if( !stricmp( cmdline[i], FUSION_OPT ) ) {
            FusionCalled = TRUE;
            NoTitleScreen = TRUE;
            continue;
        }
    }

} /* parseArgs */

/*
 * imgEditFini - clean up
 */
static void imgEditFini( void )
{
#if defined( __NT__ )
    DeleteObject( hBkBrush );
#endif
    IECtl3dFini( Instance );
    DestroyIcon( hBitmapIcon );
    DestroyIcon( hIconIcon );
    DestroyIcon( hCursorIcon );
    DestroyCursor( handCursor );
    SaveImgedConfig();
    IEFiniGlobalStrings();
    FiniIconInfo();
    IEFiniErrors();

} /* imgEditFini */

/*
 * WinMain - main entry point
 */
int WINMAINENTRY WinMain( HINSTANCE currinst, HINSTANCE previnst,
                          LPSTR cmdline, int cmdshow )
{
    MSG         msg;

    cmdline = cmdline;
#if defined( __NT__ ) && !defined( __WATCOMC__ )
    _argc = __argc;
    _argv = __argv;
#endif
    WRInit();

    if( _argc > 1 ) {
        parseArgs( _argc, _argv );
    }

    if( !imgEditInit( currinst, previnst, cmdshow ) ) {
        if( ImgedIsDDE ) {
            IEDDEDumpConversation( currinst );
        }
        return( 0 );
    }

    if( ImgedIsDDE ) {
        if( IEDDEStart( currinst ) ) {
            if( !IEDDEStartConversation() ) {
                WImgEditError( WIE_DDE_INIT_FAILED, NULL );
                PostMessage( HMainWindow, WM_CLOSE, (WPARAM)1, 0 );
            }
        } else {
            WImgEditError( WIE_DDE_INIT_FAILED, NULL );
            PostMessage( HMainWindow, WM_CLOSE, (WPARAM)1, 0 );
        }
    }

    IEEnableMenuInput( TRUE );

    if( _argc > 1 ) {
        parseCmdLine( _argc, _argv );
    }

    while( GetMessage( &msg, (HWND)NULL, 0, 0 ) ) {
        if( !TranslateMDISysAccel( ClientWindow, &msg ) &&
            !TranslateAccelerator( HMainWindow, hAccel, &msg ) ) {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }

    if( ImgedIsDDE ) {
        IEDDEEndConversation();
        IEDDEEnd();
    }

    imgEditFini();
    WRFini();
    return( 1 );

} /* WinMain */
