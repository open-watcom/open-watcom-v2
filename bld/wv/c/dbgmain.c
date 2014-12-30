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
#include "trpcapb.h"
#include "strutil.h"
#include "dbgscan.h"


// This list of extern functions is in alphabetic order.:
extern cmd_list         *AllocCmdList( const char *,size_t );
extern void             ClearInpStack( input_type  );
extern void             FingClose( void );
extern void             FingFront( void );
extern void             FingOpen( void );
extern void             FiniAboutMessage( void );
extern void             FiniBrowse( void );
extern void             FiniCall( void );
extern void             FiniCmd( void );
extern void             FiniDLLList( void );
extern void             FiniDbgInfo( void );
extern void             FiniFont( void );
extern void             FiniGadget( void );
extern void             FiniHook( void );
extern void             FiniIOWindow( void );
extern void             FiniLiterals( void );
extern void             FiniLocalInfo( void );
extern void             FiniLook( void );
extern void             FiniMADInfo( void );
extern void             FiniMachState( void );
extern void             FiniMacros( void );
extern void             FiniMemWindow( void );
extern void             FiniMenus( void );
extern void             FiniPaint( void );
extern void             FiniScreen( void );
extern void             FiniSource( void );
extern void             FiniToolBar( void );
extern void             FiniTrace( void );
extern void             FiniTrap( void );
extern void             FreeCmdList( cmd_list * );
extern void             FreezeInpStack( void );
extern char             *GetCmdEntry( const char *tab, int index, char *buff );
extern trap_shandle     GetSuppId( char * );
extern void             GrabHandlers( void );
extern void             InitAboutMessage( void );
extern void             InitBPs( void );
extern void             InitBrowse( void );
extern bool             InitCmd( void );
extern void             InitDLLList( void );
extern void             InitDbgInfo( void );
extern void             InitFont( void );
extern void             InitGadget( void );
extern void             InitHelp( void );
extern void             InitHook( void );
extern void             InitIOWindow( void );
extern void             InitLiterals( void );
extern void             InitLocalInfo( void );
extern void             InitLook( void );
extern void             InitMADInfo( void );
extern void             InitMachState( void );
extern void             InitMemWindow( void );
extern void             InitMenus( void );
extern void             InitPaint( void );
extern void             InitScreen( void );
extern void             InitSource( void );
extern void             InitToolBar( void );
extern void             InitTrap( const char * );
extern void             LangSetFini( void );
extern bool             LangSetInit( void );
extern void             LoadProg( void );
extern void             LogFini( void );
extern void             LogInit( void );
extern void             PathFini( void );
extern void             PathInit( void );
extern void             PointFini( void );
extern void             PopInpStack( void );
extern void             PredefFini( void );
extern void             PredefInit( void );
extern void             ProcAccel( void );
extern void             ProcBreak( void );
extern void             ProcCall( void );
extern void             ProcCapture( void );
extern void             ProcCmd( void );
extern void             ProcDisplay( void );
extern void             ProcDo( void );
extern void             ProcError( void );
extern void             ProcExamine( void );
extern void             ProcFlip( void );
extern void             ProcFont( void );
extern void             ProcGo( void );
extern void             ProcHelp( void );
extern void             ProcHook( void );
extern void             ProcIf( void );
extern void             ProcInput( void );
extern void             ProcInvoke( void );
extern void             ProcLog( void );
extern void             ProcNew( void );
extern void             ProcPaint( void );
extern void             ProcPrint( void );
extern void             ProcQuit( void );
extern void             ProcRegister( void );
extern void             ProcRemark( void );
extern void             ProcSet( void );
extern void             ProcShow( void );
extern void             ProcSystem( void );
extern void             ProcThread( void );
extern void             ProcTrace( void );
extern void             ProcView( void );
extern void             ProcWhile( void );
extern void             ProcWindow( void );
extern void             ProfileInvoke( char * );
extern bool             PurgeInpStack( void );
extern void             PushCmdList( cmd_list *cmds );
extern void             RecordFini( void );
extern void             RecordInit( void );
extern void             ReleaseProgOvlay( bool );
extern void             RestoreHandlers( void );
extern void             RingBell( void );
extern void             StartupErr( const char * );
extern void             Suicide( void );
extern void             SupportFini( void );
extern void             SymCompFini( void );
extern void             SysFileInit( void );
extern bool             TBreak( void );
extern void             VarDisplayFini( void );
extern void             VarDisplayInit( void );
extern void             WndDlgFini( void );
extern void             WndMemInit( void );

extern int              ScanSavePtr;

extern bool             Supports8ByteBreakpoints;
extern bool             SupportsExactBreakpoints;

OVL_EXTERN void         ProcNil( void );


/* Internal - to be moved */
bool CapabilitiesGet8ByteBreakpointSupport( void );
bool CapabilitiesGetExactBreakpointSupport( void );
bool CapabilitiesSet8ByteBreakpointSupport( bool status );
bool CapabilitiesSetExactBreakpointSupport( bool status );

#define pick( a, b, c ) extern void b( void );
#include "dbgcmd.h"
#undef pick

static const char CmdNameTab[] = {
    #define pick( a, b, c ) c
    #include "dbgcmd.h"
    #undef pick
};



static void ( * const CmdJmpTab[] )( void ) = {
    &ProcNil,
    #define pick( a, b, c ) &b,
    #include "dbgcmd.h"
    #undef pick
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

OVL_EXTERN void ProcNil( void )
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
        RingBell();
        RingBell();
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

void ChkBreak( void )
{
    if( TBreak() ) Error( ERR_NONE, LIT( ERR_DBG_INTERRUPT ) );
}


/*
 * ProcACmd -- process a command
 */


void ProcACmd( void )
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
    FiniScreen();
    FiniMADInfo();
    FiniTrace();
    RestoreHandlers();
    _Free( TrapParms );
    FiniLiterals();
    FiniLocalInfo();
}

/*
 *  Find if the current trap file supports the capabilities service
 */
trap_shandle    SuppCapabilitiesId = 0;

bool InitCapabilities( void )
{
    /* Always reset in case of trap switch */
    Supports8ByteBreakpoints = FALSE;
    SupportsExactBreakpoints = FALSE;

    SuppCapabilitiesId = GetSuppId( CAPABILITIES_SUPP_NAME );
    if( SuppCapabilitiesId == 0 ) 
        return( FALSE );

    CapabilitiesGet8ByteBreakpointSupport();
    CapabilitiesGetExactBreakpointSupport();

    if( Supports8ByteBreakpoints )
        CapabilitiesSet8ByteBreakpointSupport( TRUE );

    if( SupportsExactBreakpoints && _IsOn( SW_BREAK_ON_WRITE ) )
        CapabilitiesSetExactBreakpointSupport( TRUE );

    return( TRUE );
}

#define SUPP_CAPABILITIES_SERVICE( in, request )   \
        in.supp.core_req        = REQ_PERFORM_SUPPLEMENTARY_SERVICE;    \
        in.supp.id              = SuppCapabilitiesId;       \
        in.req                  = request;

bool CapabilitiesGet8ByteBreakpointSupport()
{
    capabilities_get_8b_bp_req  acc;
    capabilities_get_8b_bp_ret  ret;

    if( SuppCapabilitiesId == 0 ) 
        return( FALSE );

    SUPP_CAPABILITIES_SERVICE( acc, REQ_CAPABILITIES_GET_8B_BP );

    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.err != 0 ) {
        return( FALSE );
    } else {
        Supports8ByteBreakpoints = TRUE;   /* The trap supports 8 byte breakpoints */
        return( TRUE );
    }
}

bool CapabilitiesSet8ByteBreakpointSupport( bool status )
{
    capabilities_set_8b_bp_req  acc;
    capabilities_set_8b_bp_ret  ret;

    if( SuppCapabilitiesId == 0 ) 
        return( FALSE );

    SUPP_CAPABILITIES_SERVICE( acc, REQ_CAPABILITIES_SET_8B_BP );
    acc.status = status ? TRUE : FALSE;

    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.err != 0 ) {
        return( FALSE );
    } else {
        Supports8ByteBreakpoints = ret.status ? TRUE : FALSE;
        return( TRUE );
    }
}

bool CapabilitiesGetExactBreakpointSupport( void )
{
    capabilities_get_8b_bp_req  acc;
    capabilities_get_8b_bp_ret  ret;


    if( SuppCapabilitiesId == 0 ) 
        return( FALSE );

    SUPP_CAPABILITIES_SERVICE( acc, REQ_CAPABILITIES_GET_EXACT_BP );

    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.err != 0 ) {
        return( FALSE );
    } else {
        /* The trap may support it, but it is not possible currently */
        SupportsExactBreakpoints = ret.status ? TRUE : FALSE;        
        return( TRUE );
    }
}

bool CapabilitiesSetExactBreakpointSupport( bool status )
{
    capabilities_set_8b_bp_req  acc;
    capabilities_set_8b_bp_ret  ret;

    if( SuppCapabilitiesId == 0 ) 
        return( FALSE );

    SUPP_CAPABILITIES_SERVICE( acc, REQ_CAPABILITIES_SET_EXACT_BP );
    acc.status = status ? TRUE : FALSE;

    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.err != 0 ) {
        return( FALSE );
    } else {
        _SwitchSet( SW_BREAK_ON_WRITE, ret.status ? TRUE : FALSE ); 
        return( TRUE );
    }
}
