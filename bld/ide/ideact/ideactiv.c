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


#include <string.h>
#define INCLUDE_COMMDLG_H
#include <wwindows.h>
#include "ideactiv.h"
#include "wclbproc.h"


/* Local Windows CALLBACK function prototypes */
WINEXPORT BOOL CALLBACK FindWatIDEHwnd( HWND hwnd, LPARAM lparam );

#define IDE_WINDOW_CLASS        "GUIClass"
#define IDE_WIN_CAP_LEN         15
#define IDE_WINDOW_CAPTION      "Open Watcom IDE"

WINEXPORT BOOL CALLBACK FindWatIDEHwnd( HWND hwnd, LPARAM lparam )
{
    char        buf[256];
    BOOL        *found;
    int         len;

    lparam = lparam;
    len = GetClassName( hwnd, buf, sizeof( buf ) );
    buf[len] = '\0';
    if( strcmp( buf, IDE_WINDOW_CLASS ) == 0 ) {
        GetWindowText( hwnd, buf, sizeof( buf ) );
        if( strncmp( buf, IDE_WINDOW_CAPTION, IDE_WIN_CAP_LEN ) == 0 ) {
            SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
            found = (BOOL *)lparam;
            *found = TRUE;
            return( FALSE );
        }
    }
    return( TRUE );
}

void StartIDE( HANDLE instance, BOOL dospawn )
{
    BOOL        found;
    WNDENUMPROC wndenumproc;

    found = FALSE;
    wndenumproc = MakeProcInstance_WNDENUM( FindWatIDEHwnd, instance );
    EnumWindows( wndenumproc, (LPARAM)&found );
    FreeProcInstance_WNDENUM( wndenumproc );
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
