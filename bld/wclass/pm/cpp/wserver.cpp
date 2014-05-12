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
#include "wddeobj.hpp"
#include "wserver.hpp"

#define CLIENT_BLOCK    8

static CONVCONTEXT ConvContext = { sizeof( CONVCONTEXT ), 0 };


MRESULT EXPENTRY serverWindowProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 ) {
/**********************************************************************************/

    WServer *server = (WServer *)WinQueryWindowPtr( hwnd, 0 );
    switch( msg ) {
    case WM_DDE_INITIATE: { // new client
        DDEINIT *ddei = (PDDEINIT)mp2;
        if( (strcmp( server->_serverName, (char *)ddei->pszAppName ) == 0) &&
            (strcmp( server->_topic, (char *)ddei->pszTopic ) == 0) ) {
            WinDdeRespond( (HWND)mp1, hwnd,
                           (PSZ)(const char *)server->_serverName,
                           (PSZ)(const char *)server->_topic, &ConvContext );
            server->addClient( (HWND)mp1 );
        }
        DosFreeMem( ddei );
        return( (MRESULT)true );
    } case WM_DDE_REQUEST: { // request from client
        DDESTRUCT *ddes = (PDDESTRUCT)mp2;
        server->request( (HWND)mp1, ddes );
        DosFreeMem( ddes );
        break;
    } case WM_DDE_TERMINATE: {
        WinDdePostMsg( (HWND)mp1, server->_serverWindow, WM_DDE_TERMINATE, NULL, 0 );
        break;
    } default:
        return( server->_prevServerProc( hwnd, msg, mp1, mp2 ) );
    }
    return( 0 );
}


WEXPORT WServer::WServer( const char *server, const char *topic,
                          WObject* owner, sbc notify )
        : _owner( NULL )
        , _notify( NULL )
        , _topic( topic )
        , _numClients( 0 )
        , _maxClients( 0 )
        , _clientList( NULL )
        , _serverName( server )
        , _prevServerProc( NULL )
        , _serverWindow( NULLHANDLE ) {
/*************************************/

    ULONG       style = 0;

    _serverWindow = WinCreateStdWindow( HWND_DESKTOP, 0, &style, WC_FRAME,
                                        NULL, 0, NULLHANDLE, 0, NULL );
    if( _serverWindow != NULLHANDLE ) {
        _owner = owner;
        _notify = notify;
        _prevServerProc = WinSubclassWindow( _serverWindow, (PFNWP)serverWindowProc );
        WinSetWindowPtr( _serverWindow, 0, this );
    }
}


WEXPORT WServer::~WServer() {
/***************************/

    if( _serverWindow != NULLHANDLE ) {
        WinDestroyWindow( _serverWindow );
    }
    for( int i = 0; i < _numClients; ++i ) {
        WinDdePostMsg( _clientList[i], _serverWindow, WM_DDE_TERMINATE, NULL, 0 );
    }
    delete _clientList;
}


void WServer::request( HWND client, DDESTRUCT *ddes ) {
/*****************************************************/

    DDESTRUCT   *dde;

    if( (_owner != NULL) && (_notify != NULL) ) {
        WString* reply = (_owner->*_notify)( (char *)DDES_PSZITEMNAME( ddes ) );
        if( reply != NULL ) {
            dde = WDDEObject::makeDDEObject( client, (char *)DDES_PSZITEMNAME( ddes ),
                                             DDE_FACK, DDEFMT_TEXT,
                                             reply->gets(), reply->size() );
            WinDdePostMsg( client, _serverWindow, WM_DDE_DATA, dde, DDEPM_RETRY );
            return;
        }
    }
    dde = WDDEObject::makeDDEObject( client, (char *)DDES_PSZITEMNAME( ddes ), 0, 0, NULL, 0 );
    WinDdePostMsg( client, _serverWindow, WM_DDE_ACK, dde, DDEPM_RETRY );
}


void WServer::addClient( HWND client ) {
/**************************************/

    if( _numClients >= _maxClients ) {
        HWND *new_client_list = new HWND[_maxClients + CLIENT_BLOCK];
        if( new_client_list == NULL ) return;
        for( int i = 0; i < _numClients; ++i ) {
            new_client_list[i] = _clientList[i];
        }
        if( _clientList != NULL ) {
            delete _clientList;
        }
        _clientList = new_client_list;
    }
    _clientList[_numClients] = client;
    ++_numClients;
}
