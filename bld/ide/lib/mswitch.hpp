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


#ifndef mswitch_class
#define mswitch_class

#include "wtokfile.hpp"
#include "wvlist.hpp"

typedef unsigned char SwMode;
#define SWMODE_RELEASE  0
#define SWMODE_DEBUG    1
#define SWMODE_COUNT    2

WCLASS MState;
WCLASS MSwitch : public WObject
{
    Declare( MSwitch )
    public:
        MSwitch ( const char* name=NULL ) : _text( name ) {}
        MSwitch( WTokenFile& fil, WString& tok );
        ~MSwitch() {}
        int panel() { return _panel; }
        void name( WString& name ) { name = _text; }
        WString& text() { return _text; }
        void getTag( WString& tag );
        void getCompatibleTag( WString& tag );
        virtual void displayText( WString& s );

        MSwitch* addSwitch( WVList& list, const char* mask );
        virtual void getText( WString& str, WVList* states, SwMode mode ) = 0;
        virtual void getText( WString& str, MState* state ) = 0;
        virtual WString& on() =0;
    protected:
        void findStates( WVList* states, WVList& found );
    private:
        int             _panel;
        WString         _mask;
        WString         _text;
        WString         _compatibleText;
};

#endif
