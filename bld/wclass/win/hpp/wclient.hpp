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


#ifndef wclient_class
#define wclient_class

#include "system.hpp"

#include "wobject.hpp"
#include "wstring.hpp"
#include "wobjmap.hpp"

typedef int WClientFlags;

#define CS_NOFLAGS   0
#define CS_WANTREPLY 1

WCLASS WClient : public WObject {
    public:
        WEXPORT WClient( WObject* owner, cbc notify );
        WEXPORT ~WClient();
        bool WEXPORT connected() { return _connected; }
        bool WEXPORT connect( const char *service, const char *topic );
        void WEXPORT disconnect();
        WString* WEXPORT sendMsg( const char *msg,
                                  WClientFlags flags=CS_NOFLAGS );
        bool WEXPORT xtDisconnect();
        void setTimeOut( unsigned long t ) {
            _timeout = t;
            if( t == 0 ) _timeout = TIMEOUT_ASYNC;
        }
        static WObjectMap WEXPORT _convMap;
    private:
        HSZ             _service;
        HSZ             _topic;
        DWORD           _procid;
        HCONV           _hconv;
        WObject*        _owner;
        cbc             _notify;
        bool            _ok;
        bool            _connected;
        FARPROC         _procInst;
        unsigned long   _timeout;

};

#endif
