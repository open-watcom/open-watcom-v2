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
* Description:  Debugger mainline.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgerr.h"
#include "dbglit.h"
#include "spawn.h"
#include "dui.h"
#include "strutil.h"
#include "dbgscan.h"
#include "madinter.h"
#include "dbgutil.h"
#include "dbgsrc.h"
#include "trapglbl.h"
#include "dbgcmdln.h"
#include "dbglog.h"
#include "dbgmain.h"
#include "dbginvk.h"
#include "dbghook.h"
#include "dbgcall.h"
#include "dbgshow.h"
#include "dbgbrk.h"
#include "dbgpend.h"
#include "dbgprint.h"
#include "dbgsys.h"
#include "dbgprog.h"
#include "dbgvar.h"
#include "dbgtrace.h"
#include "dbgdll.h"
#include "dbgexdat.h"
#include "dbgmodfy.h"
#include "dbgmisc.h"
#include "dipimp.h"
#include "dipinter.h"
#include "dbgreg.h"


// This list of extern functions is in alphabetic order.:
extern void             FingClose( void );
extern void             FingFront( void );
extern void             FingOpen( void );
extern void             FiniAboutMessage( void );
extern void             FiniBrowse( void );
extern void             FiniFont( void );
extern void             FiniIOWindow( void );
extern void             FiniLook( void );
extern void             FiniMacros( void );
extern void             FiniMemWindow( void );
extern void             FiniMenus( void );
extern void             FiniPaint( void );
extern void             FiniToolBar( void );
extern void             GrabHandlers( void );
extern void             InitAboutMessage( void );
extern void             InitBrowse( void );
extern void             InitFont( void );
extern void             InitHelp( void );
extern void             InitIOWindow( void );
extern void             InitLook( void );
extern void             InitMemWindow( void );
extern void             InitMenus( void );
extern void             InitPaint( void );
extern void             InitToolBar( void );
extern void             LangSetFini( void );
extern bool             LangSetInit( void );
extern void             PathFini( void );
extern void             PathInit( void );
extern void             PredefFini( void );
extern void             PredefInit( void );
extern void             ProcAccel( void );
extern void             ProcCapture( void );
extern void             ProcDisplay( void );
extern void             ProcFont( void );
extern void             ProcHelp( void );
extern void             ProcPaint( void );
extern void             ProcSet( void );
extern void             ProcView( void );
extern void             RecordFini( void );
extern void             RecordInit( void );
extern void             RestoreHandlers( void );
extern void             StartupErr( const char * );
extern void             Suicide( void );
extern void             SupportFini( void );
extern void             SymCompFini( void );
extern void             SysFileInit( void );
extern bool             TBreak( void );
extern void             WndMemInit( void );

extern int              ScanSavePtr;

OVL_EXTERN void         ProcNil( void );


#define pick( a, b, c ) extern void b( void );
#include "_dbgcmd.h"
#undef pick

static const char CmdNameTab[] = {
    #define pick( a, b, c ) c
    #include "_dbgcmd.h"
    #undef pick
};

static void ( * const CmdJmpTab[] )( void ) = {
    #define pick( a, b, c ) &b,
    #include "_dbgcmd.h"
    #undef pick
};


char *GetCmdName( wd_cmd cmd )
{
    static char buff[MAX_CMD_NAME + 1];

    GetCmdEntry( CmdNameTab, (int)cmd, buff );
    return( buff );
}


/*
 * DebugInit -- mainline for initialization
 */

void DebugInit( void )
{
    NestedCallLevel = 0;
    UpdateFlags = 0;
    _SwitchOn( SW_ERROR_STARTUP );
    _SwitchOn( SW_CHECK_SOURCE_EXISTS );
    SET_NIL_ADDR( NilAddr );
    TxtBuff  = &DbgBuffers[0];
    *TxtBuff = '\0';
    NameBuff = &DbgBuffers[TXT_LEN+1];
    *NameBuff = '\0';
    CurrRadix = DefRadix = 10;
    DbgLevel = MIX;
    ActiveWindowLevel = MIX;
    _SwitchOn( SW_BELL );
    _SwitchOn( SW_FLIP );
    _SwitchOn( SW_RECURSE_CHECK );
    _SwitchOff( SW_ADDING_SYMFILE );
    _SwitchOff( SW_TASK_RUNNING );
    RecordInit();
    LogInit();
    InitMADInfo();
    InitMachState();
    PathInit();
    InitDbgInfo();
    InitTrap( TrapParms );
    if( !LangSetInit() ) {
        FiniTrap();
        StartupErr( LIT_ENG( STARTUP_Loading_PRS ) );
    }
    if( !InitCmd() ) {
        FiniTrap();
        StartupErr( LIT_ENG( ERR_NO_MEMORY ) );
    }
    InitScan();
    InitLook();
    InitBPs();
    InitSource();
    InitDLLList();
    DUIInit();
    InitHook();
    VarDisplayInit();
}

/*
 * ProcNil -- process NIL command
 */

OVL_EXTERN void ProcNil( void )
{
    if( ScanLen() == 0 ) Scan();
    Error( ERR_NONE, LIT_ENG( ERR_BAD_COMMAND ), ScanPos(), ScanLen() );
}


/*
 *
 */

void ReportTask( task_status task, error_idx code )
{
    switch( task ) {
    case TASK_NEW:
        _SwitchOn( SW_HAVE_TASK );
        DUIStatusText( LIT_ENG( New_Task ) );
        DUIDlgTxt( LIT_ENG( New_Task ) );
        break;
    case TASK_NOT_LOADED:
        _SwitchOff( SW_HAVE_TASK );
        Format( TxtBuff, LIT_ENG( Task_Not_Loaded ), code );
        DUIMsgBox( TxtBuff );
        RingBell();
        RingBell();
        break;
    case TASK_NONE:
        _SwitchOff( SW_HAVE_TASK );
        DUIStatusText( LIT_ENG( No_Task ) );
        DUIDlgTxt( LIT_ENG( No_Task ) );
        break;
    }
}


/*
 * ChkBreak -- report an error if there is a pending user interrupt
 */

void ChkBreak( void )
{
    if( TBreak() ) Error( ERR_NONE, LIT_ENG( ERR_DBG_INTERRUPT ) );
}


/*
 * ProcACmd -- process a command
 */


void ProcACmd( void )
{
    int     cmd;

    ChkBreak();
    CmdStart = ScanPos();
    switch( CurrToken ) {
    case T_CMD_SEPARATOR:
        Scan();
        break;
    case T_DIV:
        Scan();
        ProcDo();
        break;
    case T_LT:
        Scan();
        ProcInvoke();
        break;
    case T_GT:
        Scan();
        ProcLog();
        break;
    case T_QUESTION:
        Scan();
        ProcPrint();
        break;
    case T_MUL:
        Scan();
        ProcRemark();
        break;
    case T_EXCLAMATION:
        Scan();
        ProcSystem();
        break;
    case T_TILDE:
        Scan();
        ProcThread();
        break;
    default:
        cmd = ScanCmd( CmdNameTab );
        if( cmd < 0 ) {
            if( _IsOn( SW_IMPLICIT ) ) {
                ProcInvoke();
            } else {
                ProcNil();
            }
        } else {
            (*CmdJmpTab[ cmd ])();
        }
        break;
    }
    ScanSavePtr = 0; /* clean up previous ScanSave locations */
}


OVL_EXTERN void Profile( void )
{
    if( InvokeFile != NULL ) {
        ProfileInvoke( InvokeFile );
        _Free( InvokeFile );
        InvokeFile = NULL;
        ProcInput();
    }
}

static void PushInitCmdList( void )
{
    cmd_list    *cmds;

    if( InitCmdList != NULL ) {
        cmds = AllocCmdList( InitCmdList, strlen( InitCmdList ) );
        _Free( InitCmdList );
        InitCmdList = NULL;
        PushCmdList( cmds );
        FreeCmdList( cmds );
    }
}

/*
 * DebugMain -- mainline for initialization
 */


void DebugMain( void )
{
    bool        save;

    GrabHandlers();
    SysFileInit();
    WndMemInit();
    InitLiterals();
    InitLocalInfo();
    ProcCmd();

    Spawn( DebugInit );
    FingOpen();
    DUIFreshAll();

    LoadProg();

    save = DUIStopRefresh( TRUE );
    FreezeInpStack();
    _SwitchOn( SW_RUNNING_PROFILE );
    Spawn( Profile );           /* run profile command file */
    _SwitchOff( SW_RUNNING_PROFILE );
    PushInitCmdList();
    DUIStopRefresh( save );
    FingClose();
    DUIShow();
}



/*
 * DebugExit -- end the debugger (Kill! Crush! Destroy!)
 */

void DebugExit( void )
{
    if( DUIClose() ) {
        Suicide();
    }
}


void DebugFini( void )
{
    PointFini();
#if !( defined( __GUI__ ) && defined( __OS2__ ) )
    ReleaseProgOvlay( TRUE ); // see dlgfile.c
#endif
    VarDisplayFini();
    FiniHook();
    FiniCmd();
    LogFini();
    while( !PurgeInpStack() ) {
        ClearInpStack( INP_STOP_PURGE );
    }
    LangSetFini();
    SupportFini();
    FiniTrap();
    RecordFini();
    FiniMachState();
    FiniDbgInfo();
    FiniScan();
    FiniLook();
    FiniDLLList();
    FiniSource();
    FiniCall();
    PathFini();
    DUIFini();
    SymCompFini();
    FiniMADInfo();
    FiniTrace();
    RestoreHandlers();
    _Free( TrapParms );
    FiniLiterals();
    FiniLocalInfo();
}

void ProcWindow( void )
{
    DUIProcWindow();
}
