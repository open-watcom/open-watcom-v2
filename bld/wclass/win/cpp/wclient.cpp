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


#include "wclient.hpp"

WObjectMap WEXPORT WClient::_convMap;


HDDEDATA _WEXPORT CALLBACK clientCallback( UINT type, UINT /*fmt*/,
                                           HCONV hconv, HSZ /*hsz1*/,
                                           HSZ /*hsz2*/, HDDEDATA /*hdata*/,
                                           DWORD /*dwdata1*/,
                                           DWORD /*dwdata2*/ ) {
/**************************************************************/

    WClient* client = (WClient*)WClient::_convMap.findThis( (WHANDLE)hconv );
    if( client != NULL ) {
        switch( type ) {
        case XTYP_DISCONNECT:
            return (HDDEDATA)client->xtDisconnect();
        }
    }
    return( (HDDEDATA)NULL );
}

#define INITFLAGS (APPCMD_CLIENTONLY | CBF_SKIP_REGISTRATIONS)


WEXPORT WClient::WClient( WObject *owner, cbc notify )
    : _service( 0 )
    , _topic( 0 )
    , _owner( owner )
    , _notify( notify )
    , _procid( 0 )
    , _hconv( 0 )
    , _ok( FALSE )
    , _timeout( 5000 )
    , _connected( FALSE ) {
/*************************/

    _procInst = MakeProcInstance( (FARPROC)clientCallback, GUIMainHInst );
    if( !DdeInitialize( &_procid, (PFNCALLBACK)_procInst, INITFLAGS, 0L ) ) {
        _ok = TRUE;
    }
}


WEXPORT WClient::~WClient() {
/***************************/

    DdeUninitialize( _procid );
    FreeProcInstance( _procInst );
    _procid = NULL;
}


bool WEXPORT WClient::xtDisconnect() {
/************************************/

    _connected = FALSE;
    if( (_owner != NULL) && (_notify != NULL) ) {
        (_owner->*_notify)( "disconnect" );
    }
    return( FALSE );
}


bool WEXPORT WClient::connect( const char *service, const char *topic ) {
/***********************************************************************/

    _service = DdeCreateStringHandle( _procid, (char *)service, CP_WINANSI );
    _topic = DdeCreateStringHandle( _procid, (char *)topic, CP_WINANSI );
    _hconv = DdeConnect( _procid, _service, _topic, NULL );
    if( _hconv != NULL ) {
        _convMap.setThis( this, (WHANDLE)_hconv );
        _connected = TRUE;
        return TRUE;
    }
    DdeGetLastError( _procid );
    return( FALSE );
}


void WEXPORT WClient::disconnect() {
/**********************************/

    WClient *client = (WClient*)WClient::_convMap.findThis( (WHANDLE)_hconv );
    if( client != NULL ) {
        DdeDisconnect( _hconv );
        _convMap.clearThis( this );
        _hconv = NULL;
    }
    if( _service != NULL ) {
        DdeFreeStringHandle( _procid, _service );
        _service = NULL;
    }
    if( _topic != NULL ) {
        DdeFreeStringHandle( _procid, _topic );
        _topic = NULL;
    }
    _connected = FALSE;
}


WString * WEXPORT WClient::sendMsg( const char *msg, WClientFlags flags ) {
/*************************************************************************/

    WString *reply = NULL;
    HSZ hsz = DdeCreateStringHandle( _procid, (char *)msg, CP_WINANSI );
    HDDEDATA hdata = DdeClientTransaction( NULL, 0, _hconv, hsz, CF_TEXT,
                                           XTYP_REQUEST, _timeout, NULL );
    DdeFreeStringHandle( _procid, hsz );
    if( hdata != NULL && _timeout != TIMEOUT_ASYNC ) {
        int len = (int)DdeGetData( hdata, NULL, 0, 0 );
        char *r = new char[ len+1 ];
        DdeGetData( hdata, (unsigned char *)r, len, 0 );
        DdeFreeDataHandle( hdata );
        if( flags & CS_WANTREPLY ) {
            reply = new WString( r );
        }
        delete r;
    }
    return( reply );
}
