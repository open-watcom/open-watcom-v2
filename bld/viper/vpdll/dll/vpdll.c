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
#include <stdio.h>
#include <ddeml.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "vpdll.h"

#define WAIT_TIMEOUT            60000
#define DDE_TOPIC               "project"
#define DDE_SERVICE             "wat_winmakerdll"
#define VP_FILE_EXT             ".VPD"

static BOOL     dllInUse;
static BOOL     PASCAL (*clientFn)( DWORD msg, DWORD parm1, DWORD parm2 );
static DWORD    ddeInst;
static HCONV    ideConv;
static char     *curTarget;
static HSZ      topicHsz;
static HSZ      serviceHsz;

int DLL_EXPORT LibMain( HINSTANCE hinst, WORD dataseg, WORD heapsize, LPSTR cmdline ) {
    hinst = hinst;
    dataseg = dataseg;
    heapsize = heapsize;
    cmdline = cmdline;
    return( 1 );
}

int DLL_EXPORT WEP( int res )
{
    res = res;
    return( 1 );
}

DWORD DLL_EXPORT VPDLL_GetVersion( void ) {
    return( VPDLL_VERSION );
}

static void setExtension( const char *path, const char *ext, char *buf ) {
    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];
    char        fname[_MAX_FNAME];

    _splitpath( path, drive, dir, fname, NULL );
    _makepath( buf, drive, dir, fname, ext );
}

static HDDEDATA mkData( BOOL rc, UINT fmt, HSZ item ) {
    HDDEDATA    ret;

    if( rc ) {
        ret = DdeCreateDataHandle( ddeInst, "err", 4, 0, item, fmt, 0 );
    } else {
        ret = DdeCreateDataHandle( ddeInst, "ok", 3, 0, item, fmt, 0 );
    }
    return( ret );
}

HDDEDATA __export CALLBACK DdeProc( UINT type, UINT fmt, HCONV conv,
                                    HSZ hsz1, HSZ hsz2, HDDEDATA hdata,
                                    DWORD data1, DWORD data2 ) {

    char        *buf;
    unsigned    len;
    char        vpfile[_MAX_PATH];
    BOOL        rc;

    hdata = hdata;
    data2 = data2;
    data1 = data1;
    conv = conv;
    switch( type ) {
    case XTYP_CONNECT:
        if( hsz1 == topicHsz && hsz2 == serviceHsz ) {
//          MessageBox( NULL, "connection established", "vpdll", MB_OK );
            return( TRUE );
        } else {
            return( FALSE );
        }
    case XTYP_REQUEST:
        len = DdeQueryString( ddeInst, hsz2, NULL, 0, CP_WINANSI );
        if( len != 0 ) {
            buf = alloca( len + 1 );
            len = DdeQueryString( ddeInst, hsz2, buf, len + 1, CP_WINANSI );
            if( len != 0 ) {
                switch( buf[0] ) {
                case 'g':
                    if( buf[1] == 'c' ) {
                        rc = clientFn( VPDLL_GEN_CODE, 1, 0 );
                        return( mkData( rc, fmt, hsz2 ) );
                    }
                    break;
                case 'n':
                    if( buf[1] == 'p' ) {
                        if( curTarget == NULL || strcmp( buf+3, curTarget ) ) {
                            setCurTarget( buf + 3 );
                            setExtension( curTarget, VP_FILE_EXT, vpfile );
                            rc = clientFn( VPDLL_NEW_PROJECT, (DWORD)vpfile, 0 );
                            return( mkData( rc, fmt, hsz2 ) );
                        } else {
                            return( mkData( FALSE, fmt, hsz2 ) );
                        }
                    }
                    break;
                case 's':
                    if( buf[1] == 'd' ) {
                        clientFn( VPDLL_SHUT_DOWN, 0, 0 );
                        return( NULL );
                    }
                    break;
                case 't':
                    if( buf[1] == 'f' ) {
                        clientFn( VPDLL_TO_FRONT, 0, 0 );
                        return( NULL );
                    }
                    break;
                }
            }
        }
        break;
    }
    return( NULL );
}

static void setCurTarget( char *targ ) {
    if( curTarget != NULL ) {
        free( curTarget );
    }
    curTarget = malloc( strlen( targ ) + 1 );
    strcpy( curTarget, targ );
}

BOOL DLL_EXPORT VPDLL_Init( BOOL PASCAL (fn)( DWORD msg, DWORD parm1, DWORD parm2 ) ) {

    BOOL        err;
    HSZ         service;
    HSZ         topic;
    UINT        rc;

    err = FALSE;
    if( dllInUse ) {
        err = TRUE;
    }
    if( !err ) {
        ddeInst = 0;
        rc = DdeInitialize( &ddeInst, DdeProc, APPCLASS_STANDARD, 0L );
        if( rc != DMLERR_NO_ERROR ) err = TRUE;
    }
    if( !err ) {
        service = DdeCreateStringHandle( ddeInst, "WAT_IDE", 0 );
        topic = DdeCreateStringHandle( ddeInst, "project", 0 );
        if( service == NULL || topic == NULL ) {
            if( service != NULL ) DdeFreeStringHandle( ddeInst, service );
            if( topic != NULL ) DdeFreeStringHandle( ddeInst, topic );
            DdeUninitialize( ddeInst );
            err = TRUE;
        }
    }
    if( !err ) {
        ideConv = DdeConnect( ddeInst, service, topic, NULL );
        if( ideConv == NULL ) {
            err = TRUE;
            DdeDisconnect( ideConv );
            DdeUninitialize( ddeInst );
        }
    }
    if( !err ) {
        topicHsz = DdeCreateStringHandle( ddeInst, DDE_TOPIC, CP_WINANSI );
        if( topicHsz == NULL ) err = TRUE;
    }
    if( !err ) {
        serviceHsz= DdeCreateStringHandle( ddeInst, DDE_SERVICE, CP_WINANSI );
        if( topicHsz == NULL ) {
            err = TRUE;
            DdeFreeStringHandle( ddeInst, topicHsz );
        }
    }
    if( !err ) {
        err = !DdeNameService( ddeInst, serviceHsz, 0, DNS_REGISTER );
    }
    if( !err ) {
        dllInUse = TRUE;
        clientFn = fn;
        setCurTarget( "" );
    }
    return( err );
}

static BOOL sendRequest( char *str, DWORD timeout ) {
    HSZ         cmd;
    HDDEDATA    rc;
    DWORD       len;
    char        *buf;

    if( timeout == 0 ) timeout = TIMEOUT_ASYNC;
    cmd = DdeCreateStringHandle( ddeInst, str, 0 );
    if( cmd != NULL ) {
        rc = DdeClientTransaction( NULL, 0, ideConv, cmd, CF_TEXT,
                                    XTYP_REQUEST, timeout, NULL );
        if( rc == NULL ) return( TRUE );
        if( timeout == TIMEOUT_ASYNC ) {
            return( FALSE );
        } else {
            len = DdeGetData( rc, NULL, 0, 0 );
            buf = alloca( len + 1 );
            DdeGetData( rc, buf, len + 1, 0 );
            DdeFreeDataHandle( rc );
            return( strcmp( buf, "ok" ) );
        }
    }
    return( TRUE );
}

void DLL_EXPORT VPDLL_ReturnToIDE( void ) {

    if( dllInUse ) {
        sendRequest( "z0", 0 );
    }
}

BOOL DLL_EXPORT VPDLL_BeginFileList( void ) {
    char        *buf;

    if( dllInUse ) {
        buf = alloca( strlen( curTarget ) + 4 );
        sprintf( buf, "fb%s", curTarget );
        return( sendRequest( buf, WAIT_TIMEOUT ) );
    }
    return( TRUE );
}

BOOL DLL_EXPORT VPDLL_AddFile( char *fname ) {
    char        *buf;

    if( dllInUse ) {
        buf = alloca( strlen( fname )  + strlen( curTarget ) + 4 );
        sprintf( buf, "fa%s %s", curTarget, fname );
        return( sendRequest( buf, WAIT_TIMEOUT ) );
    }
    return( TRUE );
}

BOOL DLL_EXPORT VPDLL_EndFileList( void ) {
    char        *buf;

    if( dllInUse ) {
        buf = alloca( strlen( curTarget ) + 4 );
        sprintf( buf, "fe%s", curTarget );
        return( sendRequest( buf, WAIT_TIMEOUT ) );
    }
    return( TRUE );
}

void DLL_EXPORT VPDLL_VPDone( void ) {
    if( dllInUse ) {
        sendRequest( "ve", 0 );
        dllInUse = FALSE;
        clientFn = NULL;
        if( curTarget != NULL ) free( curTarget );
        DdeDisconnect( ideConv );
        DdeUninitialize( ddeInst );
    }
}
