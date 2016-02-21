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
* Description:  File dialog.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgmem.h"
#include "dbgerr.h"
#include "dbginvk.h"
#include "dbghook.h"
#include "dbgio.h"
#include <stdlib.h>
#include "dbgscrn.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgshow.h"
#include "dbgbrk.h"
#include "remcore.h"
#include "dbgevent.h"
#include "dbgwinsp.h"
#include "dlgfile.h"
#include "wndhelp.h"


#if defined(__UNIX__)
  #define ALLFILES      "All Files\0*\0"
#else
  #define ALLFILES      "All Files\0*.*\0"
#endif

static char *SourceFilter = {
#if defined(__UNIX__)
    "C/C++ Files (*.c *.C *.cpp *.cc *.h *.hpp)\0*.c;*.C;*.cpp;*.cc;*.h;*.hpp\0"
#else
    "C/C++ Files (*.c *.cpp *.cc *.h *.hpp)\0*.c;*.cpp;*.cc;*.h;*.hpp\0"
#endif
    "Fortran Files (*.for *.f *.fi)\0*.for;*.f;*.fi\0"
    "Asm Files (*.asm *.inc)\0*.asm;*.inc\0"
    ALLFILES
};

static char *ConfigFilter = {
    "Debugger Files (*.dbg)\0*.dbg\0"
    ALLFILES
};


static char *ExeFilter = {
#if !defined(__UNIX__)
    "EXE Files (*.exe *exp *.com)\0*.exe;*.exp;*.com\0"
#endif
    ALLFILES
};

static char *SymFilter = {
#if defined(__UNIX__)
    ALLFILES
#else
    "EXE Files (*.exe *exp *.com *.dll)\0*.exe;*.exp;*.com;*.dll\0"
#endif
    "SYM Files (*.sym)\0*.sym\0"
#if !defined(__UNIX__)
    ALLFILES
#endif
};

static char *AllFilter = {
    ALLFILES
};

static  char    *LastFile;
static  char    *LastExe;
static  char    *LastDmp;
static  char    *LastCfg;
static  char    *LastBrk;
static  char    *LastRep;

static void SetLast( char **what, const char *to )
{
    _Free( *what );
    if( to == NULL )
        to = LIT_ENG( Empty );
    *what = DupStr( to );
}

void SetLastExe( const char *to )
{
    SetLast( &LastExe, to );
}

char *GetLastExe( void )
{
    return( LastExe );
}

void SetLastCfg( const char *to )
{
    SetLast( &LastCfg, TxtBuff );
}

static void WritableConfig( void )
{
    if( FindWritable( LastCfg, TxtBuff ) ) {
        SetLast( &LastCfg, TxtBuff );
    } else {
        SetLast( &LastCfg, LIT_ENG( Empty ) );
    }
}

void InitBrowse( void )
{
    SetLast( &LastFile, LIT_ENG( Empty ) );
    SetLast( &LastExe, LIT_ENG( Empty ) );
    SetLast( &LastDmp, LIT_ENG( Empty ) );
    SetLast( &LastCfg, LIT_ENG( Empty ) );
    SetLast( &LastBrk, LIT_ENG( Empty ) );
    SetLast( &LastRep, LIT_ENG( Empty ) );
}

void FiniBrowse( void )
{
    _Free( LastFile );
    _Free( LastExe );
    _Free( LastDmp );
    _Free( LastCfg );
    _Free( LastBrk );
    _Free( LastRep );
    LastFile = NULL;
    LastExe = NULL;
    LastDmp = NULL;
    LastCfg = NULL;
    LastBrk = NULL;
    LastRep = NULL;
}

static bool DoFileBrowse( char **last, char *title, char *filter, fn_flags flags )
{
    bool        rc;

    if( *last == NULL ) {
        TxtBuff[0] = NULLCHAR;
    } else {
        strcpy( TxtBuff, *last );
    }
    rc = DlgFileBrowse( title, filter, TxtBuff, TXT_LEN, flags );
    _Free( *last );
    *last = DupStr( TxtBuff );
    return( rc );
}


void ProcConfigFile( void )
{
    input_stack *inp;
    invokes     *inv;

    ReqEOC();
    for( inp = InpStack; inp != NULL; inp = inp->link ) {
        if( inp->type & INP_CMD_FILE ) {
            inv = inp->handle;
            SetLastCfg( inv->name );
            break;
        }
    }
}

static bool AskIfKillPB( void )
{
    return( WndDisplayMessage( LIT_DUI( WARN_Kill_PowerBuilder ), LIT_ENG( Empty ),
                             GUI_YES_NO ) == GUI_RET_YES );
}

static bool     WndDead = false;
bool WndShutDownHook( void )
{

    if( WndDead )
        return( true );
    WndDead = true;
    HookNotify( true, HOOK_QUIT );
    if( _IsOn( SW_POWERBUILDER ) && _IsOn( SW_HAVE_TASK ) && !AskIfKillPB() ) {
        WndDead = false;
        return( false );
    }
    WritableConfig();
    if( _IsOn( SW_AUTO_SAVE_CONFIG ) && LastCfg != NULL && *LastCfg != NULLCHAR ) {
        SaveConfigToFile( LastCfg );
        SaveMainWindowPos();
    }
    FiniHelp();
#if defined(__GUI__) && defined(__OS2__)
    KillProgOvlay(); // must be done before windows are shut down
#endif
    return( true );
}


#define OFN_FLAGS( writing ) \
    ( writing ? (FN_HIDEREADONLY|FN_ISSAVE|FN_OVERWRITEPROMPT) : 0 )

bool ConfigSave( bool writing )
{
    WritableConfig();
    if( DoFileBrowse( &LastCfg, LIT_DUI( Configuration_File_Name ), ConfigFilter, OFN_FLAGS( writing ) ) ) {
        if( writing ) {
            SaveConfigToFile( TxtBuff );
            SaveMainWindowPos();
        } else {
            RestoreConfigFromFile( TxtBuff );
        }
        return( true );
    }
    return( false );
}


bool BreakSave( bool writing )
{
    if( DoFileBrowse( &LastBrk, LIT_DUI( Breakpoint_File_Name ), ConfigFilter, OFN_FLAGS( writing ) ) ) {
        if( writing ) {
            SaveBreaksToFile( TxtBuff );
        } else {
            RestoreBreaksFromFile( TxtBuff );
        }
        return( true );
    }
    return( false );
}


bool ReplaySave( bool writing )
{
    if( OkToSaveReplay() ) {
        if( DoFileBrowse( &LastRep, LIT_DUI( Replay_File_Name ), ConfigFilter, OFN_FLAGS( writing ) ) ) {
            if( writing ) {
                SaveReplayToFile( TxtBuff );
            } else {
                RestoreReplayFromFile( TxtBuff );
            }
            return( true );
        }
    }
    return( false );
}


extern void FileBrowse( void )
{
    if( DoFileBrowse( &LastFile, LIT_DUI( Enter_File_Name ), SourceFilter, OFN_FLAGS( 0 ) )){
        WndFileInspect( TxtBuff, false );
    }
}


extern bool ExeBrowse( void )
{
    return( DoFileBrowse( &LastExe, LIT_DUI( Program_Name ), ExeFilter, OFN_FLAGS( 0 ) ) );
}


extern bool SymBrowse( char **name )
{
    return( DoFileBrowse( name, LIT_DUI( Symbol_File ), SymFilter, OFN_FLAGS( 0 ) ) );
}


extern bool AllBrowse( char *name )
{
    return( DlgFileBrowse( LIT_DUI( Enter_File_Name ), AllFilter, name, TXT_LEN, OFN_FLAGS( 0 ) ) );
}


extern char *GetDmpName( void )
{
    bool        rc;

    rc = DoFileBrowse( &LastDmp, LIT_DUI( Enter_File_Name ), AllFilter, OFN_FLAGS( 1 ) );
    if( !rc )
        return( NULL );
    return( LastDmp );
}
