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


#include <stdlib.h>
#include "wsystem.hpp"
#include "wstring.hpp"
#include "vwmclien.hpp"
#include "wfilenam.hpp"

#define VWM_WAIT_TIMEOUT        60000

VWinMakerClient::VWinMakerClient()
    :_client( NULL ),
    _connected( FALSE )
{
}

VWinMakerClient::~VWinMakerClient() {
#ifndef __OS2__
    if( _connected ) {
        _client->disconnect();
        delete _client;
        _client = NULL;
        _connected = FALSE;
    }
#endif
}

void VWinMakerClient::Notify( char* msg )
{
#ifndef __OS2__
    if( strieq( msg, "disconnect" ) ) {
        _client->disconnect();
        _connected = FALSE;
    }
#else
    msg = msg;
#endif
}

bool VWinMakerClient::startWinMaker( void ) {

#ifndef __OS2__
    WString     cmd;
    char        *env;

    env = getenv( "bluesky" );
    if( env == NULL ) {
        cmd.printf( "wvp.exe /ide" );
    } else {
        cmd.printf( "%s /ide", env );
    }
    return( WSystemService::sysExec( cmd.gets(), WWinStateShow, 0 ) > 32 );
#else
    return( TRUE );
#endif
}

bool VWinMakerClient::Connect( void ) {

#ifndef __OS2__
    unsigned    i;

    if( !_connected && _client != NULL ) {
        delete _client;
        _client = NULL;
    }
    _client = new WClient( this, (cbc)&VWinMakerClient::Notify );
    if( _client->connect( "wat_winmakerdll", "project" ) ) {
        _connected = TRUE;
    } else {
        if( startWinMaker() ) {
            for( i=0; i < 5; ) {
                WSystemService::sysYield();
                if( _client->connect( "wat_winmakerdll", "project" ) ) {
                    _connected = TRUE;
                    break;
                }
                WSystemService::sysSleep( 1000 );
            }
            if( !_connected ) {
                delete _client;
                _client = NULL;
            }
        }
    }
    return( !_connected );
#else
    return( TRUE );
#endif
}

bool VWinMakerClient::SetProject( WString &name ) {

#ifndef __OS2__
    WString     cmd;
    WString     *ret;
    bool        rc;

    if( _connected ) {
        cmd.printf( "np %s", (const char *)name );
        _client->setTimeOut( VWM_WAIT_TIMEOUT );
        ret = _client->sendMsg( cmd.gets(), CS_WANTREPLY );
        rc = FALSE;
        if( ret == NULL || *ret == "err" ) rc = TRUE;
        if( ret != NULL ) delete ret;
    } else {
        rc = TRUE;
    }
    return( rc );
#else
    name = name;
    return( TRUE );
#endif
}

bool VWinMakerClient::GenerateCode( void ) {

#ifndef __OS2__
    WString     *ret;
    bool        rc;

    if( _connected ) {
        _client->setTimeOut( VWM_WAIT_TIMEOUT );
        ret = _client->sendMsg( "gc", CS_WANTREPLY );
        rc = FALSE;
        if( ret == NULL || *ret == "err" ) rc = TRUE;
        if( ret != NULL ) delete ret;
    } else {
        rc = FALSE;
    }
    return( rc );
#else
    return( FALSE );
#endif
}

bool VWinMakerClient::ShutDown( void ) {
#ifndef __OS2__
    if( _connected ) {
        _client->setTimeOut( 0 );
        _client->sendMsg( "sd" );
        return( FALSE );
    }
#endif
    return( TRUE );
}

bool VWinMakerClient::Run( WString &project ) {

#ifndef __OS2__
    WString     cmd;
    WFileName   fn( project );

    if( _connected ) {
        _client->setTimeOut( 0 );
        _client->sendMsg( "tf" );
        return( SetProject( project ) );
    } else if( !Connect() ) {
        return( SetProject( project ) );
    } else {
        return( TRUE );
    }
#else
    project = project;
    return( TRUE );
#endif
}
