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


#ifndef mrule_class
#define mrule_class

#include "wobject.hpp"
#include "wvlist.hpp"
#include "wpicklst.hpp"
#include "wfilenam.hpp"
#include "wtokfile.hpp"
#include "mtool.hpp"
#include "mcommand.hpp"
#include "wwindow.hpp"
#include "wkeydefs.hpp"

#define _MAX_RULETAG    40

WCLASS MSymbol;
WCLASS MFamily;
WCLASS MRule : public WObject
{
    Declare( MRule )
    public:
        MRule( const char* tag, MTool* );
        MRule( WTokenFile& fil, WString& tok );
        ~MRule();
        WString& tag() { return _tag; }
        MTool* tool() { return _tool; }
        const WString& help() { return _help; }
        const char* resultExt();
        bool match( WString& tag, WFileName& src, WFileName& tgt );
        bool match( WString& tag, WFileName& tgt );
        bool autodepend() { return _autodepend; }
        WString& browseSwitch() { return _browseSwitch; }
        bool autotrack() { return _autotracks.count() > 0; }
        WVList& autotracks() { return _autotracks; }
        WPickList& actions() { return _actions; }
        WPickList& symbols() { return _symbols; }
        void makeCommand( WString& s, WFileName* target, WVList* workFiles, WString& mask, WVList* stateList, SwMode mode, bool* browse=NULL );
        bool ismakeable() { return _commands.count() > 0; }
        bool enumAccel( WObject *obj, bcbk fn );
    private:
        MTool*          _tool;
        WString         _tag;
        WString         _tagMask;
        WString         _help;
        WVList          _targets;
        WVList          _sources;
        WVList          _autotracks;
        bool            _autodepend;
        WString         _browseSwitch;
        WPickList       _commands;      //<MCommand>
        WPickList       _actions;       //<MAction>
        WPickList       _symbols;       //<MSymbol>

        bool matchTarget( WFileName& tgt );
        bool matchSource( WFileName& src );
        MSymbol* expandSymbol( WString& v, const char* s, WVList* workFiles );
        void expandCommand( WString& s, MCommand& c, WFileName* target, WVList* workFiles, WString& mask, WVList* stateList, SwMode mode, bool* browse=NULL );
};

#endif
