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


#include <except.h>
#include <stdlib.h>

#include <wfilenam.hpp>
#include <wwinmain.hpp>
#include <wstrlist.hpp>
#include <wmsgdlg.hpp>
#include <whotspot.hpp>
#include <wmsgdlg.hpp>

#include <process.h>
#include <dr.h>

#include "util.h"
#include "browse.h"
#include "util.h"
#include "mem.h"
#include "hotspots.h"

static WHotSpots hs( NumHotSpots );
extern WHotSpots * GlobalHotSpots = &hs;

static char * TerminateError = "An unrecoverable error has occured -- application terminated";

static void CatchAll()
//--------------------
{
    WMessageDialog::message( NULL, MsgError, MsgOk, TerminateError, "WATCOM Browser" );
    exit( 0xff );
}


static void WBRInit()
//-------------------
// call any initialization routines necessary
{
    set_terminate( &CatchAll );

    hs.addHotSpot( InvalidHotSpot,      "X " );
    hs.addHotSpot( CheckBoxUncheckedU,  "[ ] " );
    hs.addHotSpot( CheckBoxUncheckedD,  "[ ] " );
    hs.addHotSpot( CheckBoxCheckedU,    "[X] " );
    hs.addHotSpot( CheckBoxCheckedD,    "[X] " );

    hs.addHotSpot( FunctionOpenUp,      "F" );
    hs.addHotSpot( FunctionOpenDown,    "f" );
    hs.addHotSpot( FunctionClosedUp,    "f" );
    hs.addHotSpot( FunctionClosedDown,  "F" );

    hs.addHotSpot( ClassOpenUp,         "C" );
    hs.addHotSpot( ClassOpenDown,       "c" );
    hs.addHotSpot( ClassClosedUp,       "c" );
    hs.addHotSpot( ClassClosedDown,     "C" );

    hs.addHotSpot( EnumOpenUp,          "E" );
    hs.addHotSpot( EnumOpenDown,        "e" );
    hs.addHotSpot( EnumClosedUp,        "e" );
    hs.addHotSpot( EnumClosedDown,      "E" );

    hs.addHotSpot( TypedefOpenUp,       "T" );
    hs.addHotSpot( TypedefOpenDown,     "t" );
    hs.addHotSpot( TypedefClosedUp,     "t" );
    hs.addHotSpot( TypedefClosedDown,   "T" );

    hs.addHotSpot( VariableOpenUp,      "V" );
    hs.addHotSpot( VariableOpenDown,    "v" );
    hs.addHotSpot( VariableClosedUp,    "v" );
    hs.addHotSpot( VariableClosedDown,  "V" );

    hs.addHotSpot( MacroOpenUp,         "M" );
    hs.addHotSpot( MacroOpenDown,       "m" );
    hs.addHotSpot( MacroClosedUp,       "m" );
    hs.addHotSpot( MacroClosedDown,     "M" );

    hs.addHotSpot( LabelOpenUp,         "L" );
    hs.addHotSpot( LabelOpenDown,       "l" );
    hs.addHotSpot( LabelClosedUp,       "l" );
    hs.addHotSpot( LabelClosedDown,     "L" );

    hs.addHotSpot( OutlineLeafUp,       "O" );
    hs.addHotSpot( OutlineLeafDown,     "o" );
    hs.addHotSpot( OutlineCollapsedUp,  "C" );
    hs.addHotSpot( OutlineCollapsedDown,"c" );
    hs.addHotSpot( OutlineExpandedUp,   "E" );
    hs.addHotSpot( OutlineExpandedDown, "e" );

    hs.addHotSpot( EditorHotUp,         "E" );
    hs.addHotSpot( EditorHotDown,       "e" );

    hs.addHotSpot( SplashPage, "" );

    hs.attach();

    DRInit();
}

static void WBRFini()
//-------------------
// do any final cleanup here
{
    DRFini();
}

class mainline
{
    public:
    mainline();
    ~mainline();
};

static WWindow * topLevelWindow;

mainline::mainline()
//------------------
{
    char buf[512];
    try {
        WBRInit();
        topLevelWindow = new Browse( getcmd( buf ) );
    } catch( CauseOfDeath cause ) {
        IdentifyAssassin( cause );
    }
}

mainline::~mainline()
//-------------------
{
    delete topLevelWindow;
    WBRFini();
}

// MAINOBJECT( mainline, COORD_USER, 4096, 4096 ) // DAGLO HAZARD NYI ITB
MAINOBJECT( mainline, COORD_SCREEN, 640, 480 ) // DAGLO HAZARD
