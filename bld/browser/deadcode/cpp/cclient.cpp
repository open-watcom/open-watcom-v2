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


#include "cclient.hpp"
#include "str.h"

CClient::CClient( HANDLE inst, Object* owner, ccbc notify )
{
        _owner = owner;
        _notify = notify;
        _client = new WClient( inst, this, (cbc)&CClient::clientNotify );
}

CClient::~CClient()
{
        delete _client;
}

void CClient::clientNotify( char* msg )
{
        (_owner->*_notify)( msg );
}

bool CClient::connect( char* service )
{
        return _client->connect( service );
}

void CClient::disconnect()
{
        _client->disconnect();
}

String* CClient::sendMsg( char* msg, WClientFlags flags )
{
        WString* r = _client->sendMsg( msg, flags );
        ifptr( r ) {
                String* s = new String( (char*)*r );
                delete r;
                return s;
        }
        return NIL;
}

