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

#define IDE_WINDOW_CLASS        "GUIClass"
#define IDE_WIN_CAP_LEN         10
#define IDE_WINDOW_CAPTION      "WATCOM IDE"

BOOL __export CALLBACK FindWatIDEHwnd( HWND hwnd, LPARAM lparam ) {
    char        buf[256];
    BOOL        *found;

    lparam = lparam;
    GetClassName( hwnd, buf, sizeof( buf ) );
    if( !strcmp( buf, "GUIClass" ) ) {
        GetWindowText( hwnd, buf, sizeof( buf ) );
        if( !strncmp( buf, IDE_WINDOW_CAPTION, IDE_WIN_CAP_LEN ) ) {
            SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0,
                          SWP_NOMOVE | SWP_NOSIZE );
            found = (BOOL *)lparam;
            *found = TRUE;
            return( FALSE );
        }
    }
    return( TRUE );
}

void StartIDE( HANDLE instance, BOOL dospawn ) {
    BOOL        found;
    FARPROC     fp;

    found = FALSE;
    instance = instance;
    fp = MakeProcInstance( FindWatIDEHwnd, instance );
    EnumWindows( FindWatIDEHwnd, (LPARAM)&found );
    FreeProcInstance( fp );
    if( !found && dospawn ) {
        WinExec( "IDE.EXE", SW_SHOW );
    }
}

#ifdef STAND_ALONE
int PASCAL WinMain( HANDLE currinst, HANDLE previnst, LPSTR cmdline, int cmdshow )
{
    previnst = previnst;
    cmdline = cmdline;
    cmdshow = cmdshow;

    StartIDE( currinst, TRUE );
    return( 0 );
}
#endif
