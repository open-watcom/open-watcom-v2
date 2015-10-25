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
#include <stdlib.h>
#include <stdio.h>
#include <ddeml.h>
#include <assert.h>
#include "weditcw.h"
#include "wedit.h"
#include "dllmain.h"

static  HCONV       hConv = 0;
static  DWORD       idInstance = 0L;
static  HINSTANCE   hInstance = NULL;
static  BOOL        bDdemlInitialized = FALSE;
static  BOOL        bConnected = FALSE;
static  BOOL        bAppSpawned = FALSE;
static  FARPROC     lpDdeProc;

void doReset( void )
{
    // reset for another connect
    hConv = NULL;
    bDdemlInitialized = FALSE;
    bConnected = FALSE;
    bAppSpawned = FALSE;
    FreeProcInstance( lpDdeProc );
}

HDDEDATA __export FAR PASCAL DdeCallback( UINT wType, UINT wFmt, HCONV hConv,
                    HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1,
                    DWORD dwData2 )
{
    wFmt = wFmt;
    hConv = hConv;
    hData = hData;
    dwData1 = dwData1;
    dwData2 = dwData2;

    switch( wType ) {
        case XTYP_DISCONNECT:
            // user may have killed vi, or maybe this is a response to
            // our ddeuninitialize.  either way, reset for next connect
            if( bConnected ) {
                doReset();
            }
            return( NULL );
    }
    return( NULL );
}

int extern __export FAR PASCAL EDITConnect( void )
{
#ifdef __WINDOWS__
    char    *szProg = "CW.EXE";
#elif __NT__
    char    *szProg = "CW32.EXE";
#else
    #error Unsupport operating system.
#endif

    char    *szService = "Codewright";
    char    *szTopic = "CurrentBuffer";
    HSZ     hszService, hszTopic;
    int     rc = 0;

    if( bConnected ) {
        return( TRUE );
    }

    // initialize our idInstance in ddeml
    if( !bDdemlInitialized ) {
        lpDdeProc = MakeProcInstance( (FARPROC) DdeCallback, hInstance );
        if( DdeInitialize( &idInstance, (PFNCALLBACK) lpDdeProc,
                           APPCMD_CLIENTONLY, 0L ) != DMLERR_NO_ERROR ) {
            return( FALSE );
        }
        bDdemlInitialized = TRUE;
    }

    // get handles to access strings
    hszService = DdeCreateStringHandle( idInstance, szService, CP_WINANSI );
    hszTopic= DdeCreateStringHandle( idInstance, szTopic, CP_WINANSI );

    // attempt connection
    hConv = DdeConnect( idInstance, hszService, hszTopic,
                        (PCONVCONTEXT) NULL );
    if( hConv == 0 ) {
        // run editor (magically grabs focus)
        // DEBUGGERS NOTE: if debugging through here, give cw a moment
        //                 to load up as it won't respond to our
        //                 DDEConnect right away
        rc = WinExec( szProg, SW_RESTORE );
        if( rc >= 32 ) {
            // editor is up - try again (otherwise give up)
            hConv = DdeConnect( idInstance, hszService, hszTopic,
                                (PCONVCONTEXT) NULL );
        }
    }

    DdeFreeStringHandle( idInstance, hszService );
    DdeFreeStringHandle( idInstance, hszTopic );

    if( hConv == 0 ) {
        return( FALSE );
    }

    bConnected = TRUE;

    if( rc > 32 ) {
        // we spawned & connected ok
        bAppSpawned = TRUE;
    }

    return( TRUE );
}

BOOL doExecute( char *szCommand )
{
    HDDEDATA    hddeData;

    hddeData = DdeClientTransaction( (LPBYTE)szCommand, strlen( szCommand ) + 1,
                                     hConv, NULL, CF_TEXT, XTYP_EXECUTE,
                                     5000, NULL );
    return( hddeData != 0 );
}

int extern __export FAR PASCAL EDITFile( LPSTR szFile, LPSTR szHelpFile )
{
    char        szCommand[ 128 ];
    BOOL        rc;

    if( !bConnected ) {
        return( FALSE );
    }

    if( strlen( szFile ) > 115 ) {
        return( FALSE );
    }
    sprintf( szCommand, "BufEditFile %s", szFile );
    rc = doExecute( szCommand );

    if( szHelpFile == NULL || strlen( szHelpFile ) > 109 ) {
        return( rc );
    }
    sprintf( szCommand, "CWHelpDefaultName %s", szHelpFile );
    rc |= doExecute( szCommand );

    return( rc );
}

int extern __export FAR PASCAL EDITLocateError( long lRow, int iCol,
                                    int iLen, int idResource, LPSTR szErrmsg )
{
    char        szCommand[ 100 ];
//    int         len;
    BOOL        rc;

    if( !bConnected ) {
        return( FALSE );
    }

    sprintf( szCommand, "MovAbs %ld %d", lRow, iCol );
    rc = doExecute( szCommand );
#if 0
    rc |= doExecute( "MarkBeginSel SELECTION_EXCLUSIVE" );
    sprintf( szCommand, "MovRight %d", iLen );
    rc |= doExecute( szCommand );
    rc |= doExecute( "MarkEndSel" );
#endif

    if( szErrmsg ) {
        sprintf( szCommand, "MsgError %.*s",
                 sizeof( szCommand ) - 10, szErrmsg );
        rc |= doExecute( szCommand );
    }

    if( idResource != 0 ) {
        idResource += 20000;
        sprintf( szCommand, "CWHelp %d HELP_CONTEXT", idResource );
        rc |= doExecute( szCommand );
    }

    return( rc );
}

int extern __export FAR PASCAL EDITLocate( long lRow, int iCol, int iLen )
{
    return( EDITLocateError( lRow, iCol, iLen, 0, NULL ) );
}

int extern __export FAR PASCAL EDITShowWindow( int iCmdShow )
{
    char        szCommand[ 80 ];
    BOOL        rc;
    HDDEDATA    hData;
    HSZ         hszItem;
    DWORD       cbDataLen;
    char        *szData;
    DWORD       dwFlags;

    if( !bConnected ) {
        return( FALSE );
    }

    // ensure iCmdShow is SW_RESTORE or SW_MINIMIZE only
    if( iCmdShow == EDIT_RESTORE || iCmdShow == EDIT_SHOWNORMAL ) {
        iCmdShow = EDIT_RESTORE;
    } else if( iCmdShow != EDIT_MINIMIZE ) {
        return( FALSE );
    }

    // get system flags
    rc = doExecute( "SysQFlags" );
    if( rc == FALSE ) {
        return( rc );
    }
    hszItem = DdeCreateStringHandle( idInstance, "ExecReturn", CP_WINANSI );
    hData = DdeClientTransaction( NULL, 0, hConv, hszItem, CF_TEXT,
                                  XTYP_REQUEST, 5000, NULL );
    DdeFreeStringHandle( idInstance, hszItem );
    if( hData == 0 ) {
        return( FALSE );
    }

    szData = (char *)DdeAccessData( hData, &cbDataLen );
    dwFlags = atol( szData );
    DdeUnaccessData( hData );

    // change minimize bit of system flags
    if( iCmdShow == EDIT_MINIMIZE ) {
        dwFlags |= SYSFLAG_WIN_MINIMIZED;
    } else {
        dwFlags &= ~SYSFLAG_WIN_MINIMIZED;
    }

    // store results
    sprintf( szCommand, "SysSetFlags %ld", dwFlags );
    return( doExecute( szCommand ) );
}

BOOL CALLBACK EnumWnd( HWND hwnd, LPARAM lParam )
{
    char    *szWantClass = CW_CLASS;
    char    szThisClass[ sizeof( CW_CLASS ) ];

    if( GetClassName( hwnd, szThisClass, sizeof( szThisClass ) ) + 1 !=
            sizeof( szThisClass ) ) {
        // if not correct length, definitely no match
        return( TRUE );
    }
    if( strcmp( szThisClass, szWantClass ) == 0 ) {
        // gotcha - store handle & stop enumeration
        *( (HWND *)lParam ) = hwnd;
        return( FALSE );
    }
    return( TRUE );
}

int extern __export FAR PASCAL EDITDisconnect( void )
{
//    DWORD       idTransaction;
    WNDENUMPROC lpEnumWnd;
    HWND        hwndCodewright = NULL;


    if( !bConnected ) {
        return( TRUE );
    }

    // kill our dde connection
    DdeUninitialize( idInstance );
    idInstance = 0;

    if( bAppSpawned ) {
        // look for a window with class name szClassName
        lpEnumWnd = (WNDENUMPROC) MakeProcInstance( (FARPROC) EnumWnd,
                                                     hInstance );
        EnumWindows( lpEnumWnd, (LPARAM)&hwndCodewright );
        FreeProcInstance( (FARPROC) lpEnumWnd );

        if( hwndCodewright != NULL ) {
            // found a window called CodeWright - make a half-hearted
            // attempt at killing it (user could possibly Cancel)
            SendMessage( hwndCodewright, WM_CLOSE, 0, 0L );
        }
    }

    // fry rest of our flags, all ready for next connect
    doReset();

    return( TRUE );
}

#ifdef __NT__

BOOL WINAPI DllMain( HINSTANCE hInst, DWORD reason, LPVOID res )
{
    res = res;
    reason = reason;
    hInstance = hInst;
    return( 1 );
}

#else

int WINAPI LibMain( HINSTANCE hInst, WORD wDataSeg, WORD wHeapSize, LPSTR lpszCmdLine )
{
    wDataSeg = wDataSeg;
    wHeapSize = wHeapSize;
    lpszCmdLine = lpszCmdLine;
    hInstance = hInst;
    return( 1 );
}

int WINAPI WEP( int q )
{
    q = q;
    return( 1);
}

#endif
