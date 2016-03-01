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


#include "wsystem.hpp"
#include "wfilenam.hpp"


int WEXPORT WSystemService::sysExec( const char *cmd,
                                     WWindowState state,
                                     WWindowType typ ) {
/******************************************************/

    WORD        show;

    _unused( typ );
    switch( state ) {
    case WWinStateHide:
        show = SW_HIDE;
        break;
    case WWinStateShow:
        show = SW_SHOW;
        break;
    case WWinStateShowNormal:
        show = SW_SHOWNORMAL;
        break;
    case WWinStateMinimized:
        show = SW_SHOWMINIMIZED;
        break;
    case WWinStateMaximized:
        show = SW_SHOWMAXIMIZED;
        break;
    default:
        show = SW_SHOWNORMAL;
        break;
    }
    UINT old_err_mode = SetErrorMode( SEM_NOOPENFILEERRORBOX );
#ifdef __NT__
    STARTUPINFO         sinfo;
    PROCESS_INFORMATION pinfo;
    int                 rc;

    memset( &sinfo, 0, sizeof( sinfo ) );
    sinfo.cb = sizeof( sinfo );
    // set ShowWindow default value for nCmdShow parameter
    sinfo.dwFlags = STARTF_USESHOWWINDOW;
    sinfo.wShowWindow = show;
    rc = CreateProcess( NULL,
                        (char *)cmd,
                        NULL,
                        NULL,
                        FALSE,
                        0,
                        NULL,
                        NULL,
                        &sinfo,
                        &pinfo );
    if( rc ) {
        CloseHandle( pinfo.hThread );
        CloseHandle( pinfo.hProcess );
        rc = 33;
    } else {
        switch( GetLastError() ) {
        case ERROR_FILE_NOT_FOUND:
            rc = 2;
            break;
        case ERROR_PATH_NOT_FOUND:
            rc = 3;
            break;
        case ERROR_BAD_FORMAT:
            rc = 11;
            break;
        case ERROR_OUTOFMEMORY:
            rc = 8;
            break;
        default:
            rc = 0;
            break;
        }
    }
#else
    int rc = WinExec( cmd, show );
#endif
    SetErrorMode( old_err_mode );
    return( rc );
}


int WEXPORT WSystemService::sysExecBackground( const char *cmd ) {
/****************************************************************/

#if defined( __NT__ )
    PROCESS_INFORMATION info;
    STARTUPINFO         start;
    DWORD               create_flags;

    memset( &start, 0, sizeof( start ) );
    start.cb = sizeof( start );
    // set ShowWindow default value for nCmdShow parameter
    start.dwFlags = STARTF_USESHOWWINDOW;
    start.wShowWindow = SW_HIDE;
    create_flags = NORMAL_PRIORITY_CLASS;
    create_flags |= CREATE_NEW_CONSOLE;
    if( !CreateProcess( NULL, (char *)cmd, NULL, NULL, TRUE, create_flags, NULL, NULL,
                        &start, &info ) ) {
        info.dwProcessId = 0;
    }
    CloseHandle( info.hThread );
    CloseHandle( info.hProcess );
    return( info.dwProcessId );
#else
    return( sysExec( cmd, WWinStateHide, WWinTypeDefault ) );
#endif
}


void WEXPORT WSystemService::sysYield() {
/***************************************/

    MSG     msg;

    while( PeekMessage( (LPMSG)&msg, 0, 0, 0, PM_NOREMOVE ) ) {
        GetMessage( (LPMSG)&msg, 0, 0, 0 );
        TranslateMessage( (LPMSG)&msg );
        DispatchMessage( (LPMSG)&msg );
    }
}


void WEXPORT WSystemService::sysSleep( unsigned long interval ) {
/***************************************************************/

#if defined( __NT__ )
    Sleep( interval );
#else
    // stuff for Windows
    interval = interval;
#endif
}


#define BUFF_LEN 512

WModuleHandle WEXPORT WSystemService::loadLibrary( const char *lib_name ) {
/*************************************************************************/

    WModuleHandle      lib_handle;

    WFileName fn( lib_name );
    if( *fn.ext() == NULLCHAR ) {
        fn.setExt( "dll" );
    }

    UINT old_err_mode = SetErrorMode( SEM_NOOPENFILEERRORBOX );
    lib_handle = LoadLibrary( (LPSTR)(const char *)fn );
    SetErrorMode( old_err_mode );
#if !defined( __NT__ )
    if( (UINT)lib_handle < 32 ) {
        return( NULL );
    }
#endif
    return( lib_handle );
}


WProcAddr WEXPORT WSystemService::getProcAddr( WModuleHandle mod_handle,
                                               const char *proc ) {
/*****************************************************************/

    return( (WProcAddr) GetProcAddress( mod_handle, proc ) );
}


void WEXPORT WSystemService::freeLibrary( WModuleHandle mod_handle ) {
/********************************************************************/

    FreeLibrary( mod_handle );
}
