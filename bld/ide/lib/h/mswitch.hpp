/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef mswitch_class
#define mswitch_class

#include "idecfg.h"
#include "wtokfile.hpp"
#include "wvlist.hpp"
#include "wobjfile.hpp"

typedef unsigned char SwMode;
#define SWMODE_RELEASE  0
#define SWMODE_DEBUG    1
#define SWMODE_COUNT    2

WCLASS MState;
WCLASS MTool;
WCLASS MSwitch : public WObject
{
    Declare( MSwitch )
    public:
        MSwitch( const char* id=NULL ) { _id = id; fixup(); }
        MSwitch( WTokenFile& fil, WString& tok );
        ~MSwitch() {}
        int panel() const { return( _panel ); }
        const char* id() const { return( _id ); }
        void getId( WString& id ) { id = _id; }
        void getTag( WString& tag ) { tag = _mask; tag.concat( _id ); }
        bool isSetable() const { return( _panel >= 0 ); }
        bool isTagEqual( const char* tag, int kludge=0 ) const;
        MSwitch* addSwitch( WVList& list, const char* mask );
        virtual void addOptText( WString& s );
        virtual void getText( WString& str, WVList* states, SwMode mode ) = 0;
        virtual void getText( WString& str, MState* state ) = 0;
        WString& on() { return( _on ); }
        bool state( SwMode m ) { return( _state[m] ); }
        void state( SwMode m, bool state ) { _state[m] = state; }
#ifndef NOPERSIST
        void WEXPORT readState( WObjectFile& p, SwMode m ) { p.readObject( &_state[m] ); }
        void WEXPORT writeState( WObjectFile& p, SwMode m ) { p.writeObject( _state[m] ); }
        void WEXPORT copyState( SwMode md, SwMode ms ) { _state[md] = _state[ms]; }
#endif
    protected:
        void findStates( WVList* states, WVList& found );
        void fixup() { _idlen = _id.size(); }
    private:
        int             _panel;
        WString         _mask;
        WString         _id;
        WString         _on;
        int             _idlen;
        bool            _state[SWMODE_COUNT];
};

#endif
