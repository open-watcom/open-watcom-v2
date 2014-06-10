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


#include "wddeobj.hpp"
#include "wclient.hpp"

static CONVCONTEXT ConvContext = { sizeof( CONVCONTEXT ), 0 };


MRESULT EXPENTRY clientWindowProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 ) {
/**********************************************************************************/

    WClient *client = (WClient *)WinQueryWindowPtr( hwnd, 0 );
    switch( msg ) {
    case WM_DDE_INITIATEACK: { // response from server after WinDdeInitiate()
        DDEINIT *ddei = (PDDEINIT)mp2;
        client->_serverWindow = (HWND)mp1;
        DosFreeMem( ddei );
        client->_connected = true;
        return( (MRESULT)TRUE );
    } case WM_DDE_DATA:  // reply from server
      case WM_DDE_ACK: { // negative reply from server
        DDESTRUCT *ddes = (PDDESTRUCT)mp2;
        client->reply( (HWND)mp1, ddes );
        DosFreeMem( ddes );
        break;
    } case WM_DDE_TERMINATE: {
        WinDdePostMsg( (HWND)mp1, client->_clientWindow, WM_DDE_TERMINATE, NULL, 0 );
        break;
    } default:
        return( client->_prevClientProc( hwnd, msg, mp1, mp2 ) );
    }
    return( 0 );
}


WEXPORT WClient::WClient( WObject *owner, cbc notify )
        : _owner( owner )
        , _notify( notify )
        , _connected( false )
        , _prevClientProc( NULL )
        , _serverWindow( NULLHANDLE ) {
/*************************************/

    ULONG       style = 0;

    _clientWindow = WinCreateStdWindow( HWND_DESKTOP, 0, &style, WC_FRAME,
                                        NULL, 0, NULLHANDLE, 0, NULL );
    if( _clientWindow != NULLHANDLE ) {
        _prevClientProc = WinSubclassWindow( _clientWindow, (PFNWP)clientWindowProc );
        WinSetWindowPtr( _clientWindow, 0, this );
    }
}


WEXPORT WClient::~WClient() {
/***************************/

    disconnect();
    if( _clientWindow != NULLHANDLE ) {
        WinDestroyWindow( _clientWindow );
    }
}


bool WEXPORT WClient::connect( const char *server_name, const char *topic ) {
/***************************************************************************/

    _connected = false;
    if( WinDdeInitiate( _clientWindow, (PSZ)server_name, (PSZ)topic, &ConvContext ) ) {
        if( _connected ) {
            return( true );
        }
    }
    return( false );
}


void WEXPORT WClient::disconnect() {
/**********************************/

    if( _connected ) {
        WinDdePostMsg( _serverWindow, _clientWindow, WM_DDE_TERMINATE, NULL, 0 );
        _connected = false;
    }
}


WString * WEXPORT WClient::sendMsg( const char *msg, WClientFlags ) {
/*******************************************************************/

    DDESTRUCT   *dde;

    if( !_connected ) return( NULL );
    dde = WDDEObject::makeDDEObject( _serverWindow, msg, 0, DDEFMT_TEXT, NULL, 0 );
    WinDdePostMsg( _serverWindow, _clientWindow, WM_DDE_REQUEST, dde, DDEPM_RETRY );
    return( NULL );
}


void WClient::reply( HWND, DDESTRUCT *ddes ) {
/********************************************/

    WString     *reply;

    if( ddes->fsStatus & DDE_FACK ) {
        reply = new WString( (char *)DDES_PABDATA( ddes ) );
    } else {
        reply = NULL;
    }
    (_owner->*_notify)( (char *)reply );
    delete reply;
}


bool WEXPORT WClient::connected() {
/*********************************/

    return( _connected );
}
