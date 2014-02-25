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


#ifndef mcommand_class
#define mcommand_class

#include "wstring.hpp"
#include "wvlist.hpp"
#include "wtokfile.hpp"

#define BMACRO "$<"
#define EMACRO '>'

enum {
    EXECUTE_NORMAL = 0,
    EXECUTE_EDITOR,
    EXECUTE_BATCH,
    EXECUTE_MAKE,
    EXECUTE_BROWSE,
    EXECUTE_HELP,
    EXECUTE_TOUCH_ALL
};

WCLASS WFileName;
WCLASS MTool;
WCLASS MCommand : public WString
{
    Declare( MCommand )
    public:
        WEXPORT MCommand( WTokenFile& fil, WString& tok );
        WEXPORT MCommand( const char* str=NULL );
        WEXPORT ~MCommand() {}
        void name( WString& n ) const { n = *this; }
        WEXPORT const WString& mask() const { return _mask; }
        MCommand& WEXPORT operator=( const MCommand& );
        int WEXPORT expand( WString& command, WFileName* target, MTool* tool, const WString& mask, WVList* stateList, SwMode mode, WString* browseSwitch=NULL ) const;
    private:
        bool expandSwitches( WString& v, WVList& list, WVList* stateList, WString* browseSwitch, SwMode mode ) const;
        WString    _mask;
};

#endif
