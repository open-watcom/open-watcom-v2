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
* Description:  Windows editor mainline.
*
****************************************************************************/


#include "vi.h"
// #include "ole2def.h"
#include "font.h"
#include "color.h"
#include "ldstr.h"

window_id       Root;
window_id       EditContainer;
HINSTANCE       InstanceHandle;
char            near EditorName[] = "Open Watcom Text Editor";
static int      showHow;

extern BOOL RegisterMainWindow( HANDLE );
extern int  (*_main_entry_)( char *, char * );

extern HWND     hColorbar, hFontbar, hSSbar;

static BOOL initApplication( void )
{
    RegisterMainWindow( InstanceHandle );
    // RegisterContainerWindow( InstanceHandle );

    // Init ldstr.c
    SetInstance( InstanceHandle );
    return( TRUE );
}

static BOOL initInstance( void )
{
    // OLE2Init();
    InitFonts();
    InitColors();
    return( TRUE );
}

void StartWindows( void )
{
    CreateMainWindow( InstanceHandle );
    /* all other windows will be created when Root gets sized */
    if( showHow == SW_SHOWNORMAL && RootState == SIZE_MAXIMIZED ) {
        showHow = SW_SHOWMAXIMIZED;
    }
    ShowWindow( Root, showHow );
    UpdateWindow( Root );
    ShowWindow( EditContainer, SW_SHOWNORMAL );
    UpdateWindow( EditContainer );
}

void FiniInstance( void )
{
    FiniColors();
    FiniFonts();
    // OLE2Fini();
}

#ifdef DBG
void *HeapWalker( void )
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
    extern char **_argv;

    EXEName = _argv[0];
    InstanceHandle = inst;
    showHow = show;
    prev = prev;
    cmdline = cmdline;

#ifdef TRMEM
    InitTRMEM();
#endif

#ifndef __NT__
    if( prev != NULL && !HasShare() ) {
        MessageBox( NULLHANDLE, "SHARE.EXE must be loaded before starting Windows in order to run multiple instances of the editor",
                    EditorName, MB_OK );
        MyGetInstanceData( (unsigned short) prev, (void near *) &Root, sizeof( Root ) );
        SetFocus( Root );
        return( 0 );
    }
#endif

    Comspec = getenv( "COMSPEC" );
#ifdef __NT__
    VarAddGlobalStr( "OS", "winnt" );
#else
    VarAddGlobalStr( "OS", "win" );
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
    if( !BAD_ID( CurrentWindow ) ) {
        SetFocus( Root );
    }

    SetWindowCursorForReal();
    ResizeRoot();
    EditMain();

#ifdef TRMEM
    DumpTRMEM();
#endif
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
        if( !PeekMessage( &msg, (HWND)NULLHANDLE, 0, 0, PM_NOYIELD | PM_NOREMOVE ) ) {
            CloseStartupDialog();
            rc = GetMessage( &msg, (HWND)NULLHANDLE, 0, 0 );
            if( !rc ) {
                exit( msg.wParam );
            }
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }
    if( !EditFlags.KeyOverride ) {
        while( PeekMessage( &msg, (HWND)NULLHANDLE, 0, 0, PM_NOYIELD | PM_NOREMOVE ) ) {
            rc = GetMessage( &msg, (HWND)NULLHANDLE, 0, 0 );
            if( !rc ) {
                exit( msg.wParam );
            }
            if( (hColorbar == 0 || !IsDialogMessage( hColorbar, &msg )) &&
                (hSSbar == 0 || !IsDialogMessage( hSSbar, &msg )) &&
                (hFontbar == 0 || !IsDialogMessage( hFontbar, &msg )) &&
                !TranslateMDISysAccel( EditContainer, &msg ) ) {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
                if( EditFlags.KeyOverride ) {
                    break;
                }
            }
        }
    }

} /* MessageLoop */
