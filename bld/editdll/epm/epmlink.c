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


//
// EPMLINK      : interface DLL for communicating to EPM (OS/2 system editor)
//

#include <process.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#define INCL_WINDDE
#define INCL_WINWINDOWMGR
#define INCL_WININPUT
#include <os2.h>

typedef struct errstr {
    ULONG       errorline;
    ULONG       offset;
    ULONG       length;
    ULONG       magic;
} errstr;

typedef struct initiate_data {
    ULONG       errorcount;
    errstr      errors[1];
    ULONG       liblength;
    char        libname[1];
} initiate_data;

typedef struct goto_data {
    ULONG       errorline;
    ULONG       offset;
    ULONG       resourceid;
    ULONG       magic;
    ULONG       textlength;
    char        errortext[1];
} goto_data;

static CONVCONTEXT      ConvContext = { sizeof( CONVCONTEXT ), 0 };

static PFNWP            prevClientProc;
static HWND             hwndDDE;        // window handle for DDE session

#define _Editor         "EPM.EXE"

typedef struct message {
    int         row;
    int         col;
    int         len;
    int         resourceid;
    char        error[1];
} message;

typedef struct session {
    struct session      *link;
    HWND                hwnd;
    message             *msg;
    char                connected;
    char                *file_name;
    char                *help_library;
    char                data[2];        // room for 2 NULLCHAR's
} session;

static session          *SessionList;
static session          *CurrSession;
static unsigned         AllocatedBlocks;
static char             StartingSessionInProgress;

#define RESOURCE_ID_BASE        19999


static void *MemAlloc( unsigned size ) {
/**************************************/

    void        *p;

    p = malloc( size );
    if( p != NULL ) {
        ++AllocatedBlocks;
    }
    return( p );
}


static void MemFree( void *p ) {
/******************************/

    free( p );
    --AllocatedBlocks;
}


static session *FindSession( char *fn ) {
/***************************************/

    session     *sess;

    for( sess = SessionList; sess != NULL; sess = sess->link ) {
        if( strcmp( sess->file_name, fn ) == 0 ) return( sess );
    }
    return( NULL );
}


static session *FindSessionByHWND( HWND hwnd ) {
/**********************************************/

    session     *sess;

    for( sess = SessionList; sess != NULL; sess = sess->link ) {
        if( sess->hwnd == hwnd ) return( sess );
    }
    return( NULL );
}


static session *NewSession( char *fn, char *hlib ) {
/**************************************************/

    int         len_fn;
    int         len_hlib;
    session     *new_session;

    len_fn = strlen( fn );
    if( hlib == NULL ) {
        hlib = "";
    }
    len_hlib = strlen( hlib );
    new_session = MemAlloc( sizeof( session ) + len_fn + len_hlib );
    if( new_session != NULL ) {
        // WM_DDE_INITIATE handler assumes that the new
        // session is inserted at the head of the list
        new_session->file_name = &new_session->data[0];
        new_session->help_library = &new_session->data[len_fn+1];
        memcpy( new_session->file_name, fn, len_fn + 1 );
        memcpy( new_session->help_library, hlib, len_hlib + 1 );
        new_session->hwnd = NULLHANDLE;
        new_session->connected = FALSE;
        new_session->msg = NULL;
        new_session->link = NULL;
    }
    return( new_session );
}


static void FreeSession( session *sess ) {
/****************************************/

    if( sess->msg != NULL ) {
        MemFree( sess->msg );
    }
    MemFree( sess );
}


static void DeleteSession( HWND hwnd ) {
/**************************************/

    session     **owner;
    session     *curr;

    owner = &SessionList;
    for(;;) {
        curr = *owner;
        if( curr->hwnd == hwnd ) break;
        owner = &curr->link;
    }
    *owner = curr->link;
    FreeSession( curr );
}


static void LinkSession( session *sess ) {
/****************************************/

    sess->link = SessionList;
    SessionList = sess;
}


static void FreeSessions() {
/**************************/

    session     *link;

    while( SessionList != NULL ) {
        link = SessionList->link;
        FreeSession( SessionList );
        SessionList = link;
    }
}


static void InitSessions() {
/**************************/

    SessionList = NULL;
}


static void Connect() {
/*********************/

    WinDdeInitiate( hwndDDE, "WB Editor", CurrSession->file_name, &ConvContext );
}


static message *BuildMsg( long lRow, int nCol, int nLen,
                          int resourceid, char *error_msg ) {
/***********************************************************/

    message     *msg;

    if( error_msg == NULL ) {
        msg = MemAlloc( sizeof( message ) );
    } else {
        msg = MemAlloc( sizeof( message ) + strlen( error_msg ) );
    }
    if( msg != NULL ) {
        msg->row = lRow;
        msg->col = nCol - 1;
        msg->len = nLen;
        msg->resourceid = RESOURCE_ID_BASE + resourceid;
        if( error_msg == NULL ) {
            msg->error[0] = '\0';
        } else {
            strcpy( msg->error, error_msg );
        }
    }
    return( msg );
}


static DDESTRUCT *MakeDDEObject( HWND hwnd, char *item_name,
                                 USHORT fsStatus, USHORT usFormat,
                                 void *data, int data_len ) {
/***********************************************************/

    DDESTRUCT   *dde;
    int         item_len;
    PID         pid;
    TID         tid;

    if( item_name != NULL ) {
        item_len = strlen( item_name ) + 1;
    } else {
        item_len = 1;
    }
    if( !DosAllocSharedMem( (PPVOID)&dde, NULL, sizeof( DDESTRUCT ) +
                            item_len + data_len,
                            PAG_COMMIT | PAG_READ | PAG_WRITE | OBJ_GIVEABLE ) ) {
        WinQueryWindowProcess( hwnd, &pid, &tid );
        DosGiveSharedMem( (PVOID)&dde, pid, PAG_READ | PAG_WRITE );
        dde->cbData = data_len;
        dde->fsStatus = fsStatus;
        dde->usFormat = usFormat;
        dde->offszItemName = sizeof( DDESTRUCT );
        if( (data_len != 0) && (data != NULL) ) {
            dde->offabData = (USHORT)(sizeof( DDESTRUCT ) + item_len);
        } else {
            dde->offabData = 0;
        }
        if( item_name != NULL ) {
            strcpy( (char *)DDES_PSZITEMNAME( dde ), item_name );
        } else {
            strcpy( (char *)DDES_PSZITEMNAME( dde ), "" );
        }
        if( data != NULL ) {
            memcpy( DDES_PABDATA( dde ), data, data_len );
        }
        return( dde );
    }
    return( NULL );
}


static char SendData( message *msg, char send_init ) {
/****************************************************/

    initiate_data       *idata;
    goto_data           *gdata;
    DDESTRUCT           *dde;

    gdata = MemAlloc( sizeof( goto_data ) + strlen( msg->error ) );
    if( gdata == NULL ) {
        return( FALSE );
    }
    if( send_init ) {
        idata = MemAlloc( sizeof( initiate_data ) +
                        strlen( CurrSession->help_library ) );
        if( idata == NULL ) {
            MemFree( gdata );
            return( FALSE );
        }
        idata->errorcount = 1;
        idata->errors[0].errorline = msg->row;
        idata->errors[0].offset = msg->col;
        idata->errors[0].length = msg->len;
        idata->errors[0].magic = 0;
        idata->liblength = strlen( CurrSession->help_library );
        strcpy( idata->libname, CurrSession->help_library );
        dde = MakeDDEObject( CurrSession->hwnd, "Initialize", 0, DDEFMT_TEXT,
                             idata, sizeof( initiate_data ) + idata->liblength );
        WinDdePostMsg( CurrSession->hwnd, hwndDDE, WM_DDE_EXECUTE, dde,
                       DDEPM_RETRY );
    }
    gdata->errorline = msg->row;
    gdata->offset = msg->col;
    gdata->resourceid = msg->resourceid;
    gdata->magic = 0;
    gdata->textlength = strlen( msg->error );
    strcpy( gdata->errortext, msg->error );
    dde = MakeDDEObject( CurrSession->hwnd, "Goto", 0, DDEFMT_TEXT,
                         gdata, sizeof( goto_data ) + gdata->textlength );
    WinDdePostMsg( CurrSession->hwnd, hwndDDE, WM_DDE_EXECUTE, dde,
                   DDEPM_RETRY );
    if( send_init ) {
        MemFree( idata );
    }
    MemFree( gdata );
    return( TRUE );
}


MRESULT EXPENTRY clientProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 ) {
/****************************************************************************/

    switch( msg ) {
    case WM_DDE_INITIATE: { // editor has started
        DDEINIT *ddei = (PDDEINIT)mp2;
        if( (strcmp( "WB Editor", (char *)ddei->pszAppName ) == 0) &&
            (stricmp( CurrSession->file_name, (char *)ddei->pszTopic ) == 0) ) {
            // make sure that we are expecting a session to be started - more
            // than one application may be using the DLL (i.e. the IDE and
            // browser)
            if( StartingSessionInProgress ) {
                CurrSession->hwnd = (HWND)mp1;
                StartingSessionInProgress = FALSE;
                Connect();
            }
        }
        DosFreeMem( ddei );
        return( (MRESULT)TRUE );
    } case WM_DDE_INITIATEACK: { // response from EPM after WinDdeInitiate()
        DDEINIT *ddei = (PDDEINIT)mp2;
        session *sess = FindSessionByHWND( (HWND)mp1 );
        sess->connected = TRUE;
        if( sess->msg != NULL ) {
            SendData( sess->msg, TRUE );
        }
        DosFreeMem( ddei );
        return( (MRESULT)TRUE );
    } case WM_DDE_ACK: { // acknowledgement from EPM
        DDESTRUCT *ddes = (PDDESTRUCT)mp2;
        DosFreeMem( ddes );
        break;
    } case WM_DDE_DATA: { // request to send "Goto" message to EPM
        session *sess = FindSessionByHWND( (HWND)mp1 );
        SendData( sess->msg, FALSE );
        break;
    } case WM_DDE_TERMINATE: {
        WinDdePostMsg( (HWND)mp1, hwndDDE, WM_DDE_TERMINATE, NULL, 0 );
        DeleteSession( (HWND)mp1 );
        break;
    } default:
        return( prevClientProc( hwnd, msg, mp1, mp2 ) );
    }
    return( 0 );
}


#pragma linkage( EDITConnect, system )
int __export EDITConnect() {
/**************************/

    ULONG       style = 0;

    if( hwndDDE != NULLHANDLE ) return( TRUE );
    hwndDDE = WinCreateStdWindow( HWND_DESKTOP, 0, &style, WC_FRAME,
                                  NULL, 0, NULLHANDLE, 0, NULL );
    if( hwndDDE == NULLHANDLE ) {
        return( FALSE );
    }
    prevClientProc = WinSubclassWindow( hwndDDE, (PFNWP)clientProc );
    return( TRUE );
}


#pragma linkage( EDITFile, system )
int __export EDITFile( char *fn, char *hlib ) {
/*********************************************/

    char        full_fn[_MAX_PATH];

    if( _fullpath( full_fn, fn, _MAX_PATH ) == NULL ) {
        return( NULL );
    }
    CurrSession = FindSession( full_fn );
    if( CurrSession == NULL ) {
        if( StartingSessionInProgress ) {
            return( FALSE );
        }
        StartingSessionInProgress = TRUE;
        // new session must be created before we start the editor so
        // that we can process the WM_DDE_INITIATE message properly
        CurrSession = NewSession( full_fn, hlib );
        if( CurrSession == NULL ) {
            return( FALSE );
        }
        if( spawnlp( P_NOWAIT, _Editor, _Editor, "/W", full_fn, NULL ) == -1 ) {
            FreeSession( CurrSession );
            CurrSession = NULL;
            return( FALSE );
        }
        LinkSession( CurrSession );
    }
    WinSetFocus( HWND_DESKTOP, CurrSession->hwnd );
    return( TRUE );
}


#pragma linkage( EDITLocateError, system )
int __export EDITLocateError( long lRow, int nCol, int nLen,
                              int resourceid, char *error_msg ) {
/***************************************************************/

    // queue the request in case the connection to the editor has
    // not fully completed
    if( CurrSession->msg != NULL ) {
        MemFree( CurrSession->msg );
    }
    CurrSession->msg = BuildMsg( lRow, nCol, nLen, resourceid, error_msg );
    if( CurrSession->msg == NULL ) {
        return( FALSE );
    }
    if( CurrSession->hwnd == NULLHANDLE ) {
        // editor session hasn't started yet
        return( FALSE );
    }
    if( !CurrSession->connected ) {
        return( FALSE );
    }
    return( SendData( CurrSession->msg, TRUE ) );
}


#pragma linkage( EDITLocate, system )
int __export EDITLocate( long lRow, int nCol, int nLen ) {
/********************************************************/

    return( EDITLocateError( lRow, nCol, nLen, 0, NULL ) );
}


#pragma linkage( EDITShowWindow, system )
int __export EDITShowWindow( int nCmdShow ) {
/*******************************************/

    nCmdShow = nCmdShow;
    return( TRUE );
}


#pragma linkage( EDITDisconnect, system )
int __export EDITDisconnect( void ) {
/***********************************/

    session     *sess;

    for( sess = SessionList; sess != NULL; sess = sess->link ) {
        WinDdePostMsg( sess->hwnd, hwndDDE, WM_DDE_TERMINATE, NULL, 0 );
    }
    if( hwndDDE != NULLHANDLE ) {
        WinDestroyWindow( hwndDDE );
        hwndDDE = NULLHANDLE;
    }
    return( TRUE );
}


int     __dll_initialize( void ) {
/********************************/

    InitSessions();
    CurrSession = NULL;
    AllocatedBlocks = 0;
    StartingSessionInProgress = FALSE;
    return( 1 );
}


int     __dll_terminate( void ) {
/*******************************/

    FreeSessions();
    CurrSession = NULL;
    return( 1 );
}
