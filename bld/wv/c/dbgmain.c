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


#include "dbgdefn.h"
#include "dbgtoken.h"
#include "dbgerr.h"
#include "dbgtoggl.h"
#include "dbginp.h"
#include "dbginfo.h"
#include "dbgreg.h"
#include "dbglit.h"
#include "spawn.h"
#include "dui.h"
#include <string.h>


extern void             ProcSystem(void);

extern void             StartupErr(char *);
extern char             *ScanPos(void);
extern unsigned int     ScanLen(void);
extern unsigned int     ScanCmd(char *);
extern void             InitBPs(void);
extern void             InitMachState(void);
extern void             InitLook(void);
extern void             InitScan(void);
extern void             InitSource(void);
extern void             InitDLLList(void);
extern void             ProfileInvoke(char *);
extern void             SysFileInit(void);
extern void             PathInit(void);
extern void             ProcBreak(void);
extern void             ProcCall(void);
extern void             ProcCapture(void);
extern void             ProcDisplay(void);
extern void             ProcDo(void);
extern void             ProcError(void);
extern void             ProcExamine(void);
extern void             ProcFlip(void);
extern void             ProcGo(void);
extern void             ProcHelp(void);
extern void             ProcIf(void);
extern void             ProcInvoke(void);
extern void             ProcLog(void);
extern void             ProcAccel(void);
extern void             ProcNew(void);
extern void             ProcPaint(void);
extern void             ProcFont(void);
extern void             ProcPrint(void);
extern void             ProcQuit(void);
extern void             ProcRegister(void);
extern void             ProcRemark(void);
extern void             ProcSet(void);
extern void             ProcShow(void);
extern void             ProcThread(void);
extern void             ProcTrace(void);
extern void             FiniTrace(void);
extern void             ProcView(void);
extern void             ProcWhile(void);
extern void             ProcWindow(void);
extern void             ProcHook(void);
extern void             InitHook(void);
extern void             FiniHook(void);
extern void             VarDisplayInit(void);
extern void             VarDisplayFini(void);
extern void             InitDbgInfo(void);
extern void             FiniDbgInfo(void);
extern void             InitMADInfo(void);
extern void             FiniMADInfo(void);

extern void             Scan(void);
extern void             PredefInit(void);
extern void             WndDlgFini(void);
extern void             LogInit(void);
extern void             LogFini(void);
extern void             ReleaseProgOvlay( bool );
extern void             Ring(void);
extern char             *Format(char *,char *,... );
extern bool             TBreak();
extern void             InitTrap(char *);
extern void             FiniTrap(void);
extern void             LoadProg(void);
extern bool             LangSetInit(void);
extern void             LangSetFini(void);
extern void             SupportFini(void);
extern bool             PurgeInpStack(void);
extern void             ClearInpStack(input_type  );
extern void             PopInpStack(void);
extern char             *ReScan(char *);
extern void             RecordInit(void);
extern void             RecordFini(void);
extern void             FiniMachState(void);
extern void             PredefFini(void);
extern void             FiniScan(void);
extern void             FiniLook(void);
extern void             FiniSource(void);
extern void             FiniDLLList(void);
extern void             FiniCall(void);
extern void             PointFini();
extern void             PathFini(void);
extern void             FingOpen(void);
extern void             FingFront(void);
extern void             FingClose(void);
extern bool             InitCmd( void );
extern void             FiniCmd( void );
extern void             InitLocalInfo( void );
extern void             FiniLocalInfo( void );
extern  void            FiniMacros(void);
extern  void            FiniLiterals(void);
extern void             InitLiterals(void);
extern  void            FiniToolBar(void);
extern void             InitToolBar(void);
extern  void            FiniMenus(void);
extern void             InitMenus(void);
extern  void            FiniMemWindow(void);
extern void             InitMemWindow(void);
extern  void            FiniIOWindow(void);
extern void             InitIOWindow(void);
extern  void            FiniAboutMessage(void);
extern void             InitAboutMessage(void);
extern  void            FiniPaint(void);
extern  void            FiniFont(void);
extern void             InitFont(void);
extern void             InitPaint(void);
extern void             InitScreen(void);
extern void             FiniScreen(void);
extern void             GrabHandlers(void);
extern void             RestoreHandlers(void);
extern void             ProcCmd(void);
extern void             InitGadget();
extern void             FiniGadget();
extern void             InitBrowse(void);
extern void             FiniBrowse(void);
extern void             FreezeInpStack();
extern char             *GetCmdEntry( char *tab, int index, char *buff );
extern void             SymCompFini();
extern void             InitHelp();
extern void             PushCmdList( cmd_list *cmds );
extern cmd_list         *AllocCmdList(char *,unsigned int );
extern void             FreeCmdList(cmd_list *);
extern void             Suicide();
extern void             ProcInput(void);


extern char             *InitCmdList;
extern char             *TrpFile;
extern char             *CmdStart;
extern char             *NameBuff;
extern char             *TxtBuff;
extern tokens           CurrToken;
extern unsigned char    DefRadix;
extern unsigned char    CurrRadix;
extern debug_level      DbgLevel;
extern debug_level      ActiveWindowLevel;
extern char             DbgBuffers[];
extern int              ScanSavePtr;
extern address          NilAddr;
extern char             *InvokeFile;

OVL_EXTERN void         ProcNil(void);




#define pick( a, b, c ) extern void b(void);
#include "dbgcmd.h"

static char CmdNameTab[] = {
    #define pick( a, b, c ) c
    #include "dbgcmd.h"
};



static void (* const CmdJmpTab[])() = {
    &ProcNil,
    #define pick( a, b, c ) &b,
    #include "dbgcmd.h"
};


char *GetCmdName( int index )
{
    static char buff[ MAX_CMD_NAME+1 ];
    GetCmdEntry( CmdNameTab, index, buff );
    return( buff );
}


/*
 * DebugInit -- mainline for initialization
 */

void DebugInit()
{
    _SwitchOn( SW_ERROR_STARTUP );
    _SwitchOn( SW_CHECK_SOURCE_EXISTS );
    SET_NIL_ADDR( NilAddr );
    TxtBuff  = &DbgBuffers[0];
    NameBuff = &DbgBuffers[TXT_LEN+1];
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
    InitTrap( TrpFile );
    if( !LangSetInit() ) {
        FiniTrap();
        StartupErr( LIT( STARTUP_Loading_PRS ) );
    }
    if( !InitCmd() ) {
        FiniTrap();
        StartupErr( LIT( ERR_NO_MEMORY ) );
    }
    InitScan();
    InitLook();
    InitBPs();
    InitSource();
    InitDLLList();
    InitScreen();
    DUIInit();
    InitHook();
    VarDisplayInit();
}

/*
 * ProcNil -- process NIL command
 */

OVL_EXTERN void ProcNil()
{
    if( ScanLen() == 0 ) Scan();
    Error( ERR_NONE, LIT( ERR_BAD_COMMAND ), ScanPos(), ScanLen() );
}


/*
 *
 */

void ReportTask( task_status task, unsigned code )
{
    switch( task ) {
    case TASK_NEW:
        _SwitchOn( SW_HAVE_TASK );
        DUIStatusText( LIT( New_Task ) );
        DUIDlgTxt( LIT( New_Task ) );
        break;
    case TASK_NOT_LOADED:
        _SwitchOff( SW_HAVE_TASK );
        Format( TxtBuff, LIT( Task_Not_Loaded ), code );
        DUIMsgBox( TxtBuff );
        Ring(); Ring();
        break;
    case TASK_NONE:
        _SwitchOff( SW_HAVE_TASK );
        DUIStatusText( LIT( No_Task ) );
        DUIDlgTxt( LIT( No_Task ) );
        break;
    }
}


/*
 * ChkBreak -- report an error if there is a pending user interrupt
 */

void ChkBreak()
{
    if( TBreak() ) Error( ERR_NONE, LIT( ERR_DBG_INTERRUPT ) );
}


/*
 * ProcACmd -- process a command
 */


void ProcACmd()
{
    unsigned cmd;

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
        if( cmd == 0 && _IsOn( SW_IMPLICIT ) ) {
            ProcInvoke();
        } else {
            (*CmdJmpTab[ cmd ])();
        }
        break;
    }
    ScanSavePtr = 0; /* clean up previous ScanSave locations */
}


OVL_EXTERN void Profile()
{
    if( InvokeFile != NULL ) {
        ProfileInvoke( InvokeFile );
        _Free( InvokeFile );
        InvokeFile = NULL;
        ProcInput();
    }
}

static void PushInitCmdList()
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


void DebugMain()
{
    bool        save;

    GrabHandlers();
    SysFileInit();
    InitLiterals();
    InitLocalInfo();
    ProcCmd();

    Spawn( &DebugInit );
    FingOpen();
    DUIFreshAll();

    LoadProg();

    save = DUIStopRefresh( TRUE );
    FreezeInpStack();
    _SwitchOn( SW_RUNNING_PROFILE );
    Spawn( &Profile );          /* run profile command file */
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


void DebugFini()
{
    PointFini();
    #if !(defined(__GUI__) && defined(__OS2__))
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
    FiniScreen();
    FiniMADInfo();
    FiniTrace();
    RestoreHandlers();
    _Free( TrpFile );
    FiniLiterals();
    FiniLocalInfo();
}
