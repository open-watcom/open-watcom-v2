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

#if defined( __WINDOWS__ ) && defined( __DLL__ )
#pragma library( "ddeml.lib" );
#endif

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
#include "mem.h"
#include "srusuprt.h"
#include "gen_cpp.h"

#ifdef __NT__
    #define PBCMD "pbnt"
#else
    #define PBCMD "pb"
#endif
#define PBDDECMD "pb"

extern int Wigmain( int argc, char **argv );

#if 0
#define say( x, y ) { char utilbuff[256]; \
                      sprintf( utilbuff, x, y ); \
                      MessageBox( NULL, utilbuff, "PBIDE.DLL", MB_SYSTEMMODAL|MB_OK|MB_ICONINFORMATION ); }
#else
#define say( x, y )
#endif

#ifdef __NT__

int __stdcall DLLMain( long hdll, long reason, long reserved )
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

static BOOL matchsExist( char *mask )
{
    DIR         *hdl;

    hdl = opendir( mask );
    if( hdl == NULL ) return( FALSE );
    closedir( hdl );
    return( TRUE );
}

BOOL IDE_EXPORT WatIDE_DirIsOk( char *dllname )
{
    char        mask[_MAX_PATH];
    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];

    say( "DirIsOk %s", dllname );
    _splitpath( dllname, drive, dir, NULL, NULL );
    _makepath( mask, drive, dir, "*", ".cpp" );
    if( matchsExist( mask ) ) return( FALSE );
    _makepath( mask, drive, dir, "*", ".hpp" );
    if( matchsExist( mask ) ) return( FALSE );
    return( TRUE );
}

void IDE_EXPORT WatIDE_GetFunctionName(  FunctionRequest type, char *buf,
                                         char *uoname, char *fnname )
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

BOOL IDE_EXPORT WatIDE_RunWig( char *sruname, char *parentname,
                               WatIDEErrInfo **info )
{

    char                *argv[4];
    unsigned            argc;
    int                 rc;
    WatIDEErrInfo       *err;

    say( "RunWig %s", sruname );
    argv[0] = "";
    argv[1] = sruname;
    argv[2] = "-r";
    argc = 3;
    if( parentname != NULL ) {
        argv[3] = MemMalloc( strlen( parentname ) + 4 );
        sprintf( argv[3], "-in=%s", parentname );
        argc++;
    }
    rc = Wigmain( argc, argv );
    err = GetErrInfo();
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

BOOL __export CALLBACK FindWatIDEHwnd( HWND hwnd, LPARAM lparam )
{
    char        buf[256];

    lparam = lparam;
    GetClassName( hwnd, buf, sizeof( buf ) );
    if( !strcmp( buf, "GUIClass" ) ) {
        GetWindowText( hwnd, buf, sizeof( buf ) );
        if( !strncmp( buf, IDE_WINDOW_CAPTION, IDE_WIN_CAP_LEN ) ) {
            ideHwnd = hwnd;
            return( FALSE );
        }
    }
    return( TRUE );
}

static void mkProjectName( char *buf, char *dllname ) {
    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];
    char        fname[_MAX_FNAME];

    _splitpath( dllname, drive, dir, fname, NULL );
    _makepath( buf, drive, dir, fname, ".wpj" );
}

static char     CmdBuffer[ _MAX_PATH * 4 ];

static BOOL runIDE( char *dllname )
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


HDDEDATA __export CALLBACK DdeProc( UINT type, UINT fmt, HCONV conv,
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

static BOOL changeIDEProject( char *dllname )
{

    BOOL        err;
    DWORD       ddeinst;
    UINT        rc;
    HCONV       conv;
    HSZ         service;
    HSZ         topic;
    HSZ         project;
    char        path[_MAX_PATH + 2]; // 2 characters for the transaction id
    int         err_rc;

    err = FALSE;
    ddeinst = 0;
    conv = NULL;
    rc = DdeInitialize( &ddeinst, DdeProc,
                        APPCMD_CLIENTONLY | CBF_SKIP_ALLNOTIFICATIONS, 0L );

    if( rc != DMLERR_NO_ERROR ) err = TRUE;
    if( !err ) {
        service = DdeCreateStringHandle( ddeinst, "WAT_IDE", 0 );
        topic = DdeCreateStringHandle( ddeinst, "project", 0 );
        if( service == (HSZ)NULL || topic == (HSZ)NULL ) {
            if( service != (HSZ)NULL ) DdeFreeStringHandle( ddeinst, service );
            if( topic != (HSZ)NULL ) DdeFreeStringHandle( ddeinst, topic );
            err = TRUE;
        }
    }
    if( !err ) {
        conv = DdeConnect( ddeinst, service, topic, NULL );
        if( conv == (HSZ)NULL ) err = TRUE;
        err_rc = DdeGetLastError( ddeinst );
    }
    if( !err ) {
        strcpy( path, PBDDECMD );
        mkProjectName( path + strlen( path ), dllname );
        project = DdeCreateStringHandle( ddeinst, path, 0 );
        if( project == (HSZ)NULL ) err = TRUE;
    }
    if( !err ) {
        err = !DdeClientTransaction( NULL, 0, conv, project, CF_TEXT,
                                               XTYP_REQUEST, 60000, NULL );
    }
    if( conv != (HSZ)NULL ) DdeDisconnect( conv );
    if( ddeinst != (DWORD)NULL) DdeUninitialize( ddeinst );
    #ifdef __NT__
        if( IsWindow( ideHwnd ) ) SetForegroundWindow( ideHwnd );
    #endif
    return( err );
}

BOOL IDE_EXPORT WatIDE_RunIDE( char *dllname )
{

    BOOL        rc;

    say( "RunIDE %s", dllname );
    rc = TRUE;
    ideHwnd = NULL;
    EnumWindows( FindWatIDEHwnd, 0 );//don't need to MakeProcInstance
                                     // because this is a DLL
    if( ideHwnd != NULL ) {
        rc = changeIDEProject( dllname );
    } else {
        rc = runIDE( dllname );
    }
    return( rc );
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

