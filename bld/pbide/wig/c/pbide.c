/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include <ddeml.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include "pbide.h"
#include "global.h"
#include "error.h"
#include "pbmem.h"
#include "srusuprt.h"
#include "gen_cpp.h"
#include "dllmain.h"
#include "winexprt.h"
#include "wig.h"
#include "pathgrp2.h"

#include "clibext.h"


#if defined( __WINDOWS__ ) && defined( __DLL__ )
#pragma library( "ddeml.lib" );
#endif

#ifdef __NT__
    #define PBCMD "pbnt"
#else
    #define PBCMD "pb"
#endif
#define PBDDECMD "pb"


WINEXPORT BOOL CALLBACK FindWatIDEHwnd( HWND hwnd, LPARAM lparam );
WINEXPORT HDDEDATA CALLBACK DdeProc( UINT type, UINT fmt, HCONV conv,
                                    HSZ hsz1, HSZ hsz2, HDDEDATA hdata,
                                    DWORD data1, DWORD data2 );

#if 0
#define say( x, y ) { char utilbuff[256]; \
                      sprintf( utilbuff, x, y ); \
                      MessageBox( NULL, utilbuff, "PBIDE.DLL", MB_SYSTEMMODAL|MB_OK|MB_ICONINFORMATION ); }
#else
#define say( x, y )
#endif

#ifdef __NT__

int __stdcall DllMain( HINSTANCE hdll, DWORD reason, LPVOID reserved )
{
    say( "DLLMain", NULL );
#if 0
    if( reason == DLL_PROCESS_ATTACH ) {
        extern char __WD_Present;
        if( __WD_Present ) {
            extern void Int3WithSignature( char __far * );
            #pragma aux Int3WithSignature parm caller [] = \
                    "int 3" \
                    "jmp short L1" \
                    'W' 'V' 'I' 'D' 'E' 'O' \
                    "L1:"
            Int3WithSignature( "DLL Loaded" );
        }
    }
#endif
    hdll = hdll;
    reason = reason;
    reserved = reserved;
    return( 1 );   // success
}

#else

int WINAPI LibMain( HINSTANCE hmod, WORD dataseg, WORD heap, LPSTR cmdline )
{
    hmod = hmod;
    dataseg = dataseg;
    heap = heap;
    cmdline = cmdline;
    return( 1 );
}

int WINAPI WEP( int res )
{
    res = res;
    return( 1 );
}

#endif

unsigned IDE_EXPORT WatIDE_GetVersion( void )
{
    return( WAT_IDE_DLL_CUR_VER );
}

static bool matchsExist( const char *mask )
{
    DIR         *hdl;

    hdl = opendir( mask );
    if( hdl == NULL )
        return( false );
    closedir( hdl );
    return( true );
}

BOOL IDE_EXPORT WatIDE_DirIsOk( const char *dllname )
{
    char        mask[_MAX_PATH];
    pgroup2     pg;

    say( "DirIsOk %s", dllname );
    _splitpath2( dllname, pg.buffer, &pg.drive, &pg.dir, NULL, NULL );
    _makepath( mask, pg.drive, pg.dir, "*", CPP_EXT );
    if( matchsExist( mask ) )
        return( FALSE );
    _makepath( mask, pg.drive, pg.dir, "*", HPP_EXT );
    if( matchsExist( mask ) )
        return( FALSE );
    return( TRUE );
}

void IDE_EXPORT WatIDE_GetFunctionName(  FunctionRequest type, char *buf,
                                    const char *uoname, const char *fnname )
{
    say( "GetFunctionName %s", uoname );
    switch( type ) {
    case FN_GET_CONSTRUCTOR:
        GetConstructor( uoname, buf );
        break;
    case FN_GET_DESTRUCTOR:
        GetDestructor( uoname, buf );
        break;
    case FN_GET_FUNCTION:
        GenerateCoverFnName( uoname, fnname, buf );
        break;
    }
}

BOOL IDE_EXPORT WatIDE_RunWig( const char *sruname, const char *parentname,
                               WatIDEErrInfo **info )
{

    char                *argv[4];
    unsigned            argc;
    int                 rc;
    WatIDEErrInfo       *err;

    say( "RunWig %s", sruname );
    argv[0] = "";
    argv[1] = MemMalloc( strlen( sruname ) + 1 );
    strcpy( argv[1], sruname );
    argv[2] = "-r";
    argc = 3;
    if( parentname != NULL ) {
        argv[3] = MemMalloc( strlen( parentname ) + 4 );
        sprintf( argv[3], "-in=%s", parentname );
        argc++;
    }
    rc = Wigmain( argc, argv );
    err = GetErrInfo();
    if( argc > 3 )
        MemFree( argv[3] );
    MemFree( argv[1] );
    if( info != NULL ) {
        *info = err;
    } else {
        FreeErrInfo( err );
    }
    remove( sruname );
    return( rc );
}

void IDE_EXPORT WatIDE_FiniErrInfo( WatIDEErrInfo *info )
{
    FreeErrInfo( info );
}

#define IDE_WINDOW_CLASS        "GUIClass"
#define IDE_WIN_CAP_LEN         10
#define IDE_WINDOW_CAPTION      "WATCOM IDE"

static HWND     ideHwnd;

WINEXPORT BOOL CALLBACK FindWatIDEHwnd( HWND hwnd, LPARAM lparam )
{
    char        buf[256];
    int         len;

    lparam = lparam;
    len = GetClassName( hwnd, buf, sizeof( buf ) );
    buf[len] = '\0';
    if( strcmp( buf, IDE_WINDOW_CLASS ) == 0 ) {
        GetWindowText( hwnd, buf, sizeof( buf ) );
        if( strncmp( buf, IDE_WINDOW_CAPTION, IDE_WIN_CAP_LEN ) == 0 ) {
            ideHwnd = hwnd;
            return( FALSE );
        }
    }
    return( TRUE );
}

static void mkProjectName( char *buf, const char *dllname )
{
    pgroup2     pg;

    _splitpath2( dllname, pg.buffer, &pg.drive, &pg.dir, &pg.fname, NULL );
    _makepath( buf, pg.drive, pg.dir, pg.fname, WPJ_EXT );
}

static char     CmdBuffer[ _MAX_PATH * 4 ];

static bool runIDE( const char *dllname )
{

    char        path[_MAX_PATH];
    char        *ptr;
    UINT        rc;

    ptr = getenv( "WATCOM" );
    CmdBuffer[0] = '\0';
    if( ptr != NULL ) {
        strcpy( CmdBuffer, ptr );
#ifdef __NT__
        strcat( CmdBuffer, "\\binnt\\" );
#else
        strcat( CmdBuffer, "\\binw\\" );
#endif
    }
    strcat( CmdBuffer, "IDE.EXE " );
    mkProjectName( path, dllname );
    strcat( CmdBuffer, "-c pbide.cfg -" PBCMD " " );
    strcat( CmdBuffer, path );
    rc = WinExec( CmdBuffer, SW_SHOW );
    return( rc < 32 );
}


WINEXPORT HDDEDATA CALLBACK DdeProc( UINT type, UINT fmt, HCONV conv,
                                    HSZ hsz1, HSZ hsz2, HDDEDATA hdata,
                                    DWORD data1, DWORD data2 )
{
    type = type;
    fmt = fmt;
    conv = conv;
    hsz1 = hsz1;
    hsz2 = hsz2;
    hdata = hdata;
    data1 = data1;
    data2 = data2;

    return( NULL );
}

static bool changeIDEProject( const char *dllname )
{
    bool        err;
    DWORD       ddeinst;
    UINT        rc;
    HCONV       conv;
    HSZ         service;
    HSZ         topic;
    HSZ         project;
    char        path[_MAX_PATH + 2]; // 2 characters for the transaction id
    int         err_rc;

    ddeinst = 0;
    conv = NULL;
    rc = DdeInitialize( &ddeinst, DdeProc,
                        APPCMD_CLIENTONLY | CBF_SKIP_ALLNOTIFICATIONS, 0L );

    err = ( rc != DMLERR_NO_ERROR );
    if( !err ) {
        service = DdeCreateStringHandle( ddeinst, "WAT_IDE", 0 );
        topic = DdeCreateStringHandle( ddeinst, "project", 0 );
        if( service == (HSZ)NULL || topic == (HSZ)NULL ) {
            if( service != (HSZ)NULL )
                DdeFreeStringHandle( ddeinst, service );
            if( topic != (HSZ)NULL )
                DdeFreeStringHandle( ddeinst, topic );
            err = true;
        }
    }
    if( !err ) {
        conv = DdeConnect( ddeinst, service, topic, NULL );
        err = ( conv == (HSZ)NULL );
        err_rc = DdeGetLastError( ddeinst );
    }
    if( !err ) {
        strcpy( path, PBDDECMD );
        mkProjectName( path + strlen( path ), dllname );
        project = DdeCreateStringHandle( ddeinst, path, 0 );
        err = ( project == (HSZ)NULL );
    }
    if( !err ) {
        err = ( DdeClientTransaction( NULL, 0, conv, project, CF_TEXT,
                                               XTYP_REQUEST, 60000, NULL ) == 0 );
    }
    if( conv != (HSZ)NULL )
        DdeDisconnect( conv );
    if( ddeinst != (DWORD)NULL )
        DdeUninitialize( ddeinst );
#ifdef __NT__
    if( IsWindow( ideHwnd ) ) {
        SetForegroundWindow( ideHwnd );
    }
#endif
    return( err );
}

BOOL IDE_EXPORT WatIDE_RunIDE( const char *dllname )
{
    say( "RunIDE %s", dllname );
    ideHwnd = NULL;
    EnumWindows( FindWatIDEHwnd, 0 );//don't need to MakeProcInstance
                                     // because this is a DLL
    if( ideHwnd != NULL ) {
        return( changeIDEProject( dllname ) );
    } else {
        return( runIDE( dllname ) );
    }
}


BOOL IDE_EXPORT WatIDE_CloseIDE( void )
{

    say( "CloseIDE", NULL );
    ideHwnd = NULL;
    EnumWindows( FindWatIDEHwnd, 0 );   //don't need to makeprocinstance
                                        // because this is a DLL
    if( ideHwnd != NULL ) {
        PostMessage( ideHwnd, WM_CLOSE, 0, 0 );
    }
    return( ideHwnd == NULL );
}

