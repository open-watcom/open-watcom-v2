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


#include "wserver.hpp"

static WServer* _server = NULL;


HDDEDATA _WEXPORT CALLBACK serverCallback( UINT type, UINT fmt,
                                           HCONV /*hconv*/, HSZ hsz1,
                                           HSZ hsz2, HDDEDATA /*hdata*/,
                                           DWORD /*dwdata1*/,
                                           DWORD /*dwdata2*/ ) {
/**************************************************************/

    if( _server != NULL ) {
        switch( type ) {
        case XTYP_REGISTER:
            return( (HDDEDATA)TRUE );
        case XTYP_CONNECT:
            return( (HDDEDATA)_server->xtConnect( hsz1, hsz2 ) );
        case XTYP_REQUEST:
            return( (HDDEDATA)_server->xtRequest( fmt, hsz1, hsz2 ) );
        }
    }
    return( (HDDEDATA)NULL );
}

#define INITFLAGS (APPCMD_FILTERINITS | CBF_FAIL_ADVISES | CBF_FAIL_EXECUTES \
                                | CBF_FAIL_POKES | CBF_SKIP_CONNECT_CONFIRMS \
                                | CBF_SKIP_REGISTRATIONS \
                                | CBF_SKIP_UNREGISTRATIONS)


WEXPORT WServer::WServer( const char *service, const char *topic, WObject* owner, sbc notify )
        : _procid( 0 )
        , _service( 0 )
        , _topic( 0 )
        , _ok( FALSE ) {
/**********************/

    if( _server == NULL ) {
        _server = this;
        _owner = owner;
        _notify = notify;
        _procInst = MakeProcInstance( (FARPROC)serverCallback, GUIMainHInst );
        if( !DdeInitialize( &_procid, (PFNCALLBACK)_procInst, INITFLAGS, 0L ) ) {
            _service = DdeCreateStringHandle( _procid, (char *)service, CP_WINANSI );
            _topic = DdeCreateStringHandle( _procid, (char *)topic, CP_WINANSI );
            if( DdeNameService( _procid, _service, 0, DNS_REGISTER ) ) {
                _ok = TRUE;
            }
        }
    }
}


WEXPORT WServer::~WServer() {
/***************************/

    if( _server != NULL ) {
        if( DdeNameService( _procid, _service, 0, DNS_UNREGISTER ) ) {
        }
        DdeFreeStringHandle( _procid, _service );
        DdeFreeStringHandle( _procid, _topic );
        DdeUninitialize( _procid );
        _procid = NULL;
        FreeProcInstance( _procInst );
        _service = NULL;
        _topic = NULL;
        _server = NULL;
    }
}


bool WEXPORT WServer::xtConnect( HSZ htopic, HSZ hservice ) {
/***********************************************************/

    if( (hservice == _service) && (htopic == _topic) ) {
        return( TRUE );
    }
    return( FALSE );
}


HDDEDATA WEXPORT WServer::xtRequest( UINT fmt, HSZ /*htopic*/, HSZ hitem ) {
/**************************************************************************/

    HDDEDATA hdata = NULL;
    if( (_owner != NULL) && (_notify != NULL) ) {
        int len = (int)DdeQueryString( _procid, hitem, (LPSTR)NULL, 0L, CP_WINANSI );
        char *request = new char[ len+1 ];
        DdeQueryString( _procid, hitem, request, len+1, CP_WINANSI );
        WString *reply = (_owner->*_notify)( request );
        if( reply != NULL ) {
            hdata = DdeCreateDataHandle( _procid, (unsigned char *)(const char *)*reply, reply->size()+1, 0, hitem, fmt, FALSE );
            delete reply;
        }
        delete request;
        return( hdata );
    }
    return( NULL );
}
