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


#ifndef maction_class
#define maction_class

#include "wtokfile.hpp"
#include "wvlist.hpp"
#include "maccel.hpp"

WCLASS MAction : public WObject
{
    Declare( MAction )
    public:
        MAction( const char* name=NULL );
        MAction( WTokenFile& fil, WString& tok );
        ~MAction();
        int compare( const WObject* o ) const { return _name.compare( &((MAction*)o)->_name ); }
        void name( WString& n ) { n = _name; }
        const WString& name() { return _name; }
        void menuName( WString *mname );
        const WString& help() { return _help; }
        void text( WString& t );
        bool hasSwitches( bool setable ) { return _tool->hasSwitches( setable ); }
        MTool* tool() { return _tool; }
        WString& hint() { return _hint; }
        int expand( WString& command, WFileName* target, const WString& mask, WVList* states, SwMode mode );
        bool defAction() { return _defAction; }
        bool button() { return _button; }
        bool okForMask() { return _okForMask; }
        int accelKey();
        WKeyCode menuAccel();
    private:
        WString         _name;
        WString         _help;
        WPickList       _commands;
        WString         _hint;
        WString         _accelString;
        MTool*          _tool;
        MAccelerator*   _accel;
        bool            _defAction;
        bool            _button;
        bool            _okForMask;
};

#endif
