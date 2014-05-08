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


#ifndef mvswitch_class
#define mvswitch_class

#include "mswitch.hpp"
#include "wstrlist.hpp"

WCLASS MVSwitch : public MSwitch
{
    Declare( MVSwitch )
    public:
        MVSwitch( const char* name=NULL ) : MSwitch( name ) {}
        MVSwitch( WTokenFile& fil, WString& tok );
        ~MVSwitch() {}
        virtual void getText( WString& str, WVList* states, SwMode mode );
        virtual void getText( WString& str, MState* state );
        WString& value( SwMode m ) { return( _value[m] ); }
        bool state( SwMode m ) { return( _state[m] ); }
        WString& on() { return( _on ); }
        bool optional() { return( _optional ); }
    private:
        WString         _on;
        WString         _connector;
        char            _quote;
        WString         _value[SWMODE_COUNT];
        bool            _state[SWMODE_COUNT];
        bool            _optional;
        bool            _multiple;
        void    addone( WString& str, bool state, WString* value, bool& first );
        void    addValues( WString& str, WStringList& values, bool& first );
};

#endif
