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


#include "variety.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "strdup.h"
#include "win.h"
#ifdef __NT__
    #include <ctype.h>
#endif


static char *mainClass;
extern char __WinTitleBar[20];          /* Text for window title bar */

static BOOL firstInstance( HANDLE );
static int windowsInit( HANDLE, int );


#if defined( __NT__ )

_WCRTLINK int   __InitDefaultWin()
{
    char        *str;
    HANDLE      inst;

    str = __clib_strdup( GetCommandLine() );
    while( !isspace( *str ) && *str != 0 ) str++;
    while( isspace( *str ) ) str++;
    inst = GetModuleHandle( NULL );
    if( !firstInstance( inst ) ) return( FALSE );
    if( !windowsInit( inst, SW_SHOWDEFAULT ) ) return( FALSE );
    _InitFunctionPointers();
    return( 1 );
}

_WCRTLINK void  __FiniDefaultWin() {}

#else
#pragma aux __init_default_win "*";
char    __init_default_win;
#endif

_WCRTLINK extern int        ___Argc;    /* argument count */
_WCRTLINK extern char **    ___Argv;    /* argument vector */

/*
 * DefaultWinMain - main windows entry point
 */
_WCRTLINK int PASCAL DefaultWinMain( HANDLE inst, HANDLE previnst,
        LPSTR cmd, int show, int (*pmain)( int, char ** ) )
{
    int rc;

    previnst = previnst;
    cmd = cmd;
    if( !firstInstance( inst ) ) return( FALSE );
    if( !windowsInit( inst, show ) ) return( FALSE );
    _InitFunctionPointers();

    rc = pmain( ___Argc, ___Argv );

    _WindowsExit();
    return( rc );

} /* DefaultWinMain */


/*
 * firstInstance - initialization at startup
 */
static BOOL firstInstance( HANDLE inst)
{
    char        tmp[128];
    BOOL        rc;
    WNDCLASS    wc;
    HMENU       smf,smh;

    /*
     * set up class names
     */
    sprintf( tmp,"WATCLASS%d", inst );
    mainClass = malloc( strlen( tmp ) + 1 );
    if( mainClass == NULL ) return( FALSE );
    strcpy( mainClass, tmp );
    sprintf( tmp,"WATSUBCLASS%d", inst );
    _ClassName = malloc( strlen( tmp ) + 1 );
    if( _ClassName == NULL ) return( FALSE );
    strcpy( _ClassName, tmp );

    /*
     * make a menu (this way, we don't need resources)
     */
    smf = CreateMenu();
    if( smf == NULL ) return( FALSE );
    AppendMenu( smf, MF_ENABLED, MSG_WRITE, "&Save As ..." );
    AppendMenu( smf, MF_ENABLED, MSG_SETCLEARINT,
                        "Set &Lines Between Auto-Clears ..." );
    AppendMenu( smf, MF_SEPARATOR, 0,NULL );
    AppendMenu( smf, MF_ENABLED, MSG_EXIT, "E&xit" );

    smh = CreateMenu();
    if( smh == NULL ) return( FALSE );
    AppendMenu( smh, MF_ENABLED, MSG_ABOUT, "&About..." );

    _SubMenuEdit = CreateMenu();
    if( _SubMenuEdit == NULL ) return( FALSE );
    AppendMenu( _SubMenuEdit, MF_ENABLED, MSG_FLUSH, "&Clear" );
    AppendMenu( _SubMenuEdit, MF_ENABLED, MSG_COPY, "&Copy" );

    _SubMenuWindows = CreateMenu();

    _MainMenu = CreateMenu();
    if( _MainMenu == NULL ) return( FALSE );
    AppendMenu( _MainMenu, MF_POPUP, (UINT) smf, "&File" );
    AppendMenu( _MainMenu, MF_POPUP, (UINT) _SubMenuEdit, "&Edit" );
    AppendMenu( _MainMenu, MF_POPUP, (UINT) _SubMenuWindows, "&Windows" );
    AppendMenu( _MainMenu, MF_POPUP, (UINT) smh, "&Help" );

    /*
     * register window classes
     */
    wc.style = NULL;
    wc.lpfnWndProc = (LPVOID) _MainDriver;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = inst;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( GRAY_BRUSH );
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = mainClass;

    rc = RegisterClass( &wc );
    if( !rc ) return( FALSE );

    wc.style = NULL;
    wc.lpfnWndProc = (LPVOID) _MainDriver;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = inst;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = _ClassName;

    rc = RegisterClass( &wc );
    if( !rc ) return( FALSE );
    return( TRUE );

} /* firstInstance */

/*
 * windowsInit - windows-specific initialization
*/
static int windowsInit( HANDLE inst, int showcmd )
{
    LOGFONT     logfont;
    WORD        x,y;

    /*** Create a font to use ***/
    #ifdef _MBCS
        if( __IsDBCS ) {
            _FixedFont = GetStockObject( SYSTEM_FONT );
        } else {
            _FixedFont = GetStockObject( SYSTEM_FIXED_FONT );
        }
    #else
        _FixedFont = GetStockObject( SYSTEM_FIXED_FONT );
    #endif
    GetObject( _FixedFont, sizeof(LOGFONT), (LPSTR) &logfont );
    _FixedFont = CreateFontIndirect( &logfont );

    x = GetSystemMetrics( SM_CXSCREEN );
    y = GetSystemMetrics( SM_CYSCREEN );

    _InitMainWindowData( inst );

    _MainWindow = CreateWindow(
        mainClass,                      /* our class                  */
        __WinTitleBar,                  /* Text for window title bar  */
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,  /* Window style.      */
        0,                              /* horizontal position.       */
        0,                              /* vertical position.         */
        x,                              /* width.                     */
        y,                              /* height.                    */
        NULL,                           /* parent                     */
        _MainMenu,                      /* menu handle                */
        inst,                           /* owner of window            */
        NULL                            /* extra data pointer         */
    );

    if( !_MainWindow ) {
        return( FALSE );
    }

    /*
     * display the window
     */
    ShowWindow( _MainWindow, showcmd );
    UpdateWindow( _MainWindow );

    /*
     * create standard IO window - takes output from stdout, stderr and
     *                             input from stdin
     */
    _NewWindow( "Standard IO", stdin->_handle, stdout->_handle,
                        stderr->_handle, -1 );
    return( TRUE );

} /* windowsInit */
