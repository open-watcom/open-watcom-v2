/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Windows editor mainline.
*
****************************************************************************/


#include "vi.h"
#ifdef __WATCOMC__
#include <process.h>
#include <malloc.h>
#endif
#include "win.h"
// #include "ole2def.h"
#include "font.h"
#include "color.h"
#include "ldstr.h"

#include "clibext.h"


window_id       _NEAR root_window_id;
window_id       edit_container_id;
HINSTANCE       InstanceHandle;
char            _NEAR EditorName[] = "Open Watcom Text Editor";
static int      showHow;

extern int  (*_main_entry_)( char *, char * );

extern HWND     hColorbar, hFontbar, hSSbar;

static bool initApplication( void )
{
    RegisterMainWindow( InstanceHandle );
    // RegisterContainerWindow( InstanceHandle );

    // Init ldstr.c
    SetInstance( InstanceHandle );
    return( true );
}

static bool initInstance( void )
{
    // OLE2Init();
    InitFonts();
    InitColors();
    return( true );
}

void StartWindows( void )
{
    CreateMainWindow( InstanceHandle );
    /* all other windows will be created when Root gets sized */
    if( showHow == SW_SHOWNORMAL && RootState == SIZE_MAXIMIZED ) {
        showHow = SW_SHOWMAXIMIZED;
    }
    ShowWindow( root_window_id, showHow );
    UpdateWindow( root_window_id );
    ShowWindow( edit_container_id, SW_SHOWNORMAL );
    UpdateWindow( edit_container_id );
}

void FiniInstance( void )
{
    FiniColors();
    FiniFonts();
    // OLE2Fini();
}

#if defined( __WATCOMC__ ) && !defined( NDEBUG )
void _WCFAR *HeapWalker( void )
{
    struct _heapinfo    info;
    int                 status;
    char                buffer[80];

    info._pentry = NULL;
    status = _HEAPOK;
    while( status != _HEAPEND ) {
        status = _heapwalk( &info );
        sprintf( buffer, "%s black at %Fp of size %4.4X\n",
                (info._useflag == _USEDENTRY) ? "USED" : "FREE",
                info._pentry, info._size );
        if( status != _HEAPOK ) {
            return( info._pentry );
        }
    }
    return( NULL );
}
#endif

int PASCAL WinMain( HINSTANCE inst, HINSTANCE prev, LPSTR cmdline, int show )
{
    char buffer[PATH_MAX];

#if !defined( __WATCOMC__ ) && defined( __NT__ )
    _argc = __argc;
    _argv = __argv;
#endif
    EXEName = _cmdname( buffer );
    InstanceHandle = inst;
    showHow = show;
    prev = prev;
    cmdline = cmdline;

    InitMem();

#ifdef __WINDOWS__
    if( prev != NULL && !HasShare() ) {
        MessageBox( NO_WINDOW, "SHARE.EXE must be loaded before starting Windows in order to run multiple instances of the editor", EditorName, MB_OK );
        GetInstanceData( prev, (unsigned char _NEAR *)&root_window_id, sizeof( root_window_id ) );
        SetFocus( root_window_id );
        return( 0 );
    }
#endif

    Comspec = getenv( "COMSPEC" );
#ifdef __WINDOWS__
    VarAddGlobalStr( "OS", "win" );
#else
    VarAddGlobalStr( "OS", "winnt" );
#endif
    SetConfigFileName( CFG_NAME );
    ReadProfile();

    ShowStartupDialog();

    initApplication();
    if( !initInstance() ) {
        return( 0 );
    }
    InitializeEditor();
    SetSaveConfig();
    if( !BAD_ID( current_window_id ) ) {
        SetFocus( root_window_id );
    }

    SetWindowCursorForReal();
    ResizeRoot();
    EditMain();

    FiniMem();

    return( 0 );

} /* WinMain */

/*
 * MessageLoop - wait for a message to occur
 */
void MessageLoop( bool block )
{
    MSG         msg;
    UINT        rc;

    if( block ) {
        if( !PeekMessage( &msg, NO_WINDOW, 0, 0, PM_NOYIELD | PM_NOREMOVE ) ) {
            CloseStartupDialog();
            rc = GetMessage( &msg, NO_WINDOW, 0, 0 );
            if( !rc ) {
                exit( msg.wParam );
            }
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }
    if( !EditFlags.KeyOverride ) {
        while( PeekMessage( &msg, NO_WINDOW, 0, 0, PM_NOYIELD | PM_NOREMOVE ) ) {
            rc = GetMessage( &msg, NO_WINDOW, 0, 0 );
            if( !rc ) {
                exit( msg.wParam );
            }
            if( (BAD_ID( hColorbar ) || !IsDialogMessage( hColorbar, &msg )) &&
                (BAD_ID( hSSbar ) || !IsDialogMessage( hSSbar, &msg )) &&
                (BAD_ID( hFontbar ) || !IsDialogMessage( hFontbar, &msg )) &&
                !TranslateMDISysAccel( edit_container_id, &msg ) ) {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
                if( EditFlags.KeyOverride ) {
                    break;
                }
            }
        }
    }

} /* MessageLoop */
