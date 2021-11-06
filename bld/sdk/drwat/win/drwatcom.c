/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#define INCLUDE_TOOL_H
#include "drwatcom.h"
#include <i86.h>
#include "watcom.h"
#include "wdebug.h"
#include "jdlg.h"
#include "memwnd.h"
#include "drwatmem.h"
#include "wclbtool.h"


/* commonui/asm/inth.asm */
void FAR PASCAL IntHandler( void );

static char     className[] = "drwatcom";
static LPFNINTHCALLBACK     fault_fn;
static LPFNNOTIFYCALLBACK   notify_fn;

extern WORD     _STACKLOW;

static void setUpMemWnd( void ) {

    char                *fname;
    MemWndConfig        *cfginfo;

    cfginfo = MemAlloc( sizeof( MemWndConfig ) );
    SetDefMemConfig();
    GetMemWndConfig( cfginfo );
    fname = MemAlloc( strlen( cfginfo->fname ) + 1 );
    strcpy( fname, cfginfo->fname );
    _fullpath( cfginfo->fname, fname, MEMWND_MAX_FNAME );
    SetMemWndConfig( cfginfo );
    MemFree( fname );
    MemFree( cfginfo );
}

int PASCAL WinMain( HINSTANCE currinst, HINSTANCE previnst, LPSTR cmdline, int cmdshow)
{
    WNDCLASS    wc;
    MSG         msg;
    HMENU       hsysmenu;
    const char  *menuname;
    int         rc;

    /* unused parameters */ (void)cmdline; (void)cmdshow;

    /*
     * don't let two of us run!
     */
    GlobalPageLock( (HGLOBAL)_FP_SEG( IntHandler ) );
    _STACKLOW = 0;
    SetInstance( currinst );
    if( previnst ) {
        Death( STR_ONLY_ONE_INST_ALLOWED );
    }
    if( cmdline[0] == 'd' && cmdline[1] == '\0' ) {
        AlwaysRespondToDebugInChar = true;
    }

    /*
     * initialize
     */
    Instance = currinst;
    rc = 1;
    MemOpen();
    JDialogInit();
    AppName = AllocRCString( STR_APP_NAME );
    ProgramTask = GetCurrentTask();
    WindowsFlags = GetWinFlags();
    GetProfileInfo();

    wc.style = 0L;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = Instance;
    wc.hIcon = LoadIcon( Instance, "APPLICON" );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = "APPLMENU";
    wc.lpszClassName = className;
    if( !RegisterClass( &wc ) ) {
        rc = 0;
    }
    if( rc && !RegMemWndClass( Instance ) ) {
        rc = 0;
    }
    if( rc && !InitSymbols() ) {
        rc = 0;
    }
    if( rc ) {
        setUpMemWnd();
        RegDrWatcomDisasmRtns();

        /*
         * now make the main window
         */
        MainWindow = CreateWindow(
            className,          /* Window class name */
            AppName,            /* Window caption */
            WS_OVERLAPPEDWINDOW,/* Window style */
            CW_USEDEFAULT,      /* Initial X position */
            0,                  /* Initial Y position */
            CW_USEDEFAULT,      /* Initial X size */
            0,                  /* Initial Y size */
            NULL,               /* Parent window handle */
            NULL,               /* Window menu handle */
            Instance,           /* Program instance handle */
            NULL);              /* Create parameters */

        ShowWindow( MainWindow, SW_SHOWMINIMIZED );
        UpdateWindow( MainWindow );
        hsysmenu = GetSystemMenu( MainWindow, FALSE );
    //    EnableMenuItem( hsysmenu, SC_CLOSE, MF_GRAYED );
        AppendMenu( hsysmenu, MF_SEPARATOR, 0,NULL );
        menuname = GetRCString( STR_CLEAR_ALERT );
        AppendMenu( hsysmenu, MF_ENABLED, MENU_CLEAR_ALERT, menuname );
        menuname = GetRCString( STR_LOG_CUR_STATE );
        AppendMenu( hsysmenu, MF_ENABLED, MENU_LOG_CURRENT_STATE, menuname );
        menuname = GetRCString( STR_LOG_OPTIONS );
        AppendMenu( hsysmenu, MF_ENABLED, MENU_LOG_OPTIONS, menuname );
        menuname = GetRCString( STR_TASK_CONTROL );
        AppendMenu( hsysmenu, MF_ENABLED, MENU_DUMP_A_TASK, menuname );

        /*
         * set up handlers
         */
        fault_fn = MakeProcInstance_INTH( IntHandler, Instance );
        if( !InterruptRegister( NULL, fault_fn ) ) {
            Death( STR_CANT_HOOK_INTER );
        }
        notify_fn = MakeProcInstance_NOTIFY( NotifyHandler, Instance );
        if( !NotifyRegister( NULL, notify_fn, NF_NORMAL | NF_RIP ) ) {
            Death( STR_CANT_HOOK_NOTIF );
        }

        /*
         * get 32-bit support
         */
        Start386Debug();
        if( WDebug386 ) {
            LBPrintf( ListBox, STR_WILL_USE_WDEBUG_386 );
        }

        while( GetMessage( &msg, NULL, 0, 0 ) ) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Death( STR_DEATH_NO_MSG );
    }
    MemClose();
    return( rc );

}  /* WinMain */

/*
 * Death - end of the pm debugger
 */
void Death( msg_id msgid, ... )
{
    char        tmp[128];
    va_list     args;
    const char  *str;


    InterruptUnRegister( NULL );
    if( fault_fn != NULL ) {
        FreeProcInstance_INTH( fault_fn );
    }
    NotifyUnRegister( NULL );
    if( notify_fn != NULL ) {
        FreeProcInstance_NOTIFY( notify_fn );
    }
    Done386Debug();

    if( msgid != STR_DEATH_NO_MSG ) {
        va_start( args, msgid );
        str = GetRCString( msgid );
        vsprintf( tmp, str, args );
        va_end( args );
        if( AppName == NULL ) {
            AppName = AllocRCString( STR_APP_NAME );
        }
        MessageBox( NULL, tmp, AppName, MB_OK );
    }
    JDialogFini();
    FiniSymbols();
    FreeRCString( AppName );
    GlobalPageUnlock( (HGLOBAL)_FP_SEG( IntHandler ) );
    exit( 1 );

} /* Death */


static int alertedState = 0;

static void newIcon( HWND hwnd, HICON icon )
{
    SetClassWord( hwnd, GCW_HICON, (WORD)icon );
    if( IsIconic( hwnd ) ) {
        ShowWindow( hwnd, SW_HIDE );
        ShowWindow( hwnd, SW_SHOWMINNOACTIVE );
    }
}

#if 0
static void newWallpaper( char *paper )
{
    if( SystemParametersInfo( SPI_SETDESKWALLPAPER, 0, paper, 0 ) ) {
        InvalidateRect( GetDesktopWindow(), NULL, TRUE );
    }
}
#endif

void Alert( void )
{
    if( !alertedState ) {
        alertedState = TRUE;
        newIcon( MainWindow, LoadIcon( Instance, "ALERTICON" ) );
        // newWallpaper( "alert.bmp" );
    }
}

void ClearAlert( void )
{
    if( alertedState ) {
        alertedState = FALSE;
        newIcon( MainWindow, LoadIcon( Instance, "APPLICON" ) );
        // newWallpaper( "clear.bmp" );
    }
}
