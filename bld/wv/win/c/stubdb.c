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
* Description:  Debugger stub functions.
*
****************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <process.h>
#include <windows.h>
#include "_srcmgt.h"
#include "dbgdata.h"
#include "dbgmem.h"
#include "dbglit.h"
#include "mad.h"
#include "dui.h"
#include "dbgvar.h"
#include "srcmgt.h"
#include "dbgscrn.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgutil.h"

extern void             DoCmd(char*);
extern bool             InsMemRef( mad_disasm_data *dd );
extern address          GetCodeDot();
extern void             *WndAsmInspect(address addr);
extern void             DebugMain();
extern void             DebugFini();
extern void             DoInput(void);
extern void             DlgCmd( void );
extern address          GetCodeDot();
extern void             ExecTrace( trace_cmd_type type, debug_level level );
extern unsigned         Go( bool );
extern void             *OpenSrcFile(cue_handle *);

volatile bool           BrkPending;

static char             *CmdData;
static bool             Done;


void WndMemInit( void )
{
}

void InitScreen( void )
{
}

void FiniScreen( void )
{
}

unsigned ConfigScreen( void )
{
    return( 0 );
}

bool DUIClose()
{
    Done = TRUE;
    return( TRUE );
}

var_info        Locals;
HANDLE          Requestsem;
HANDLE          Requestdonesem;

static void DumpLocals()
{
#if 0
    address     addr;
    int         row;
    int         depth;
    var_node    *v;

    if( !_IsOn( SW_TASK_RUNNING ) ) {
        VarErrState();
        VarInfoRefresh( VAR_LOCALS, &Locals, &addr, NULL );
        VarOkToCache( &Locals, TRUE );
    }
    for( row = 0;; ++row ) {
        v = VarGetDisplayPiece( &Locals, row, VAR_PIECE_GADGET, &depth );
        if( v == NULL ) break;
        v = VarGetDisplayPiece( &Locals, row, VAR_PIECE_NAME, &depth );
        v = VarGetDisplayPiece( &Locals, row, VAR_PIECE_VALUE, &depth );
        switch( v->gadget ) {
        case VARGADGET_NONE:
            printf( "  " );
            break;
        case VARGADGET_OPEN:
            printf( "+ " );
            break;
        case VARGADGET_CLOSE:
            printf( "- " );
            break;
        case VARGADGET_POINTS:
            printf( "->" );
            break;
        case VARGADGET_UNPOINTS:
            printf( "<-" );
            break;
        }
        VarBuildName( v, TRUE );
        printf( " %-20s %s\n", TxtBuff, v->value );
    }
    if( !_IsOn( SW_TASK_RUNNING ) ) {
        VarOkToCache( &Locals, FALSE );
        VarOldErrState();
    }
#endif
}

static void DumpSource()
{
    char        buff[256];
    DIPHDL( cue, ch );

    if( _IsOn( SW_TASK_RUNNING ) ) {
        printf( "I don't know where the task is.  It's running\n" );
    }
    if( DeAliasAddrCue( NO_MOD, GetCodeDot(), ch ) == SR_NONE ||
        !DUIGetSourceLine( ch, buff, sizeof( buff ) ) ) {
        UnAsm( GetCodeDot(), buff, sizeof( buff ) );
    }
    printf( "%s\n", buff );
}

enum {
    REQ_NONE,
    REQ_BYE,
    REQ_GO,
    REQ_TRACE_OVER,
    REQ_TRACE_INTO
} Req = REQ_NONE;

bool RequestDone;

DWORD WINAPI ControlFunc( void *parm )
{
    parm = parm;
    do {
        WaitForSingleObject( Requestsem, INFINITE ); // wait for Request
        switch( Req ) {
        case REQ_GO:
            Go( TRUE );
            break;
        case REQ_TRACE_OVER:
            ExecTrace( TRACE_OVER, DbgLevel );
            break;
        case REQ_TRACE_INTO:
            ExecTrace( TRACE_INTO, DbgLevel );
            break;
        }
        DoInput();
        _SwitchOff( SW_TASK_RUNNING );
        ReleaseSemaphore( Requestdonesem, 1, NULL ); // signal req done
    } while( Req != REQ_BYE );
    return( 0 ); // thread over!
}

void RunRequest( int req )
{
    if( _IsOn( SW_TASK_RUNNING ) ) return;
    WaitForSingleObject( Requestdonesem, INFINITE ); // wait for last request to finish
    Req = req;
    _SwitchOn( SW_TASK_RUNNING );
    ReleaseSemaphore( Requestsem, 1, NULL ); // tell worker to go
}

int main( int argc, char **argv )
{
    char        buff[256];
    DWORD       tid;
    HANDLE      hThread;

    MemInit();
    SetErrorMode( SEM_FAILCRITICALERRORS );
    getcmd( buff );
    CmdData = buff;
    DebugMain();
    _SwitchOff( SW_ERROR_STARTUP );
    DoInput();
    VarInitInfo( &Locals );
    Requestsem = CreateSemaphore( NULL, 0, 1, NULL );
    Requestdonesem = CreateSemaphore( NULL, 0, 1, NULL );
    ReleaseSemaphore( Requestdonesem, 1, NULL ); // signal req done
    hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)ControlFunc, NULL, 0, &tid );
    if (hThread == NULL) {
        MessageBox( NULL, "Error creating thread!", "Stubugger", MB_APPLMODAL+MB_OK );
    }
    while( !Done ) {
        DlgCmd();
    }
    CloseHandle( Requestsem );
    CloseHandle( Requestdonesem );
    DebugFini();
    RunRequest( REQ_BYE );
    MemFini();
    return( 0 );
}

void DlgCmd()
{
    char        buff[256];

    printf( "DBG>" );
    fflush( stdout );
    gets( buff );
    if( buff[0] != '\0' && buff[1] == '\0' ) {
        switch( tolower( buff[0] ) ) {
        case 'u':
            WndAsmInspect( GetCodeDot() );
            break;
        case 's':
            DumpSource();
            break;
        case 'l':
            DumpLocals();
            break;
        case 'i':
            RunRequest( REQ_TRACE_INTO );
            break;
        case 'o':
            RunRequest( REQ_TRACE_OVER );
            break;
        case 'g':
            RunRequest( REQ_GO );
            break;
        case 'x':
            if( _IsOn( SW_REMOTE_LINK ) ) {
                printf( "Can't break remote task!\n" );
            } else {
                HANDLE hmod;
                FARPROC proc;
                hmod = GetModuleHandle( TrapParms );
                proc = GetProcAddress( hmod, (LPSTR)5 );
                if( proc != NULL ) proc();
                CloseHandle( hmod );
            }
            // break the task
            break;
        default:
            printf( "Error - unrecognized command\n" );
        }
    } else {
        DoCmd( DupStr( buff ) );
        DoInput();
    }
}

extern char *DUILoadString( int id )
{
    char        buff[256];
    char        *ret;
    int         size;

    size = LoadString( GetModuleHandle( NULL ), id, buff, sizeof( buff ) );
    buff[size]='\0';
    ret = DbgAlloc( size+1 );
    strcpy( ret, buff );
    return( ret );
}

void DUIMsgBox( const char *text )
{
    printf( "MSG %s\n", text );
}

bool DUIDlgTxt( const char *text )
{
    printf( "DLG %s\n", text );
    return( TRUE );
}

void DUIInfoBox( const char *text )
{
    printf( "INF %s\n", text );
}

void DUIErrorBox( const char *text )
{
    printf( "ERR %s\n", text );
}

void DUIStatusText( const char *text )
{
    printf( "STA %s\n", text );
}

bool DlgGivenAddr( char *title, address *value )
{
    // needed when segment's don't map (from new/sym command)
    return( FALSE );
}
void DlgNewWithSym( const char *title, char *buff, int buff_len )
{
    // used by print command with no arguments
}
bool DlgUpTheStack()
{
    // used when trying to trace, but we've unwound the stack a bit
    return( FALSE );
}
bool DlgAreYouNuts( unsigned long mult )
{
    // used when too many break on write points are set
    return( FALSE );
}
bool DlgBackInTime()
{
    // used when trying to trace, but we've backed up over a call or asynch
    return( FALSE );
}
bool DlgIncompleteUndo()
{
    // used when trying to trace, but we've backed up over a call or asynch
    return( FALSE );
}
bool DlgBreak( address addr )
{
    // used when an error occurs in the break point expression or it is entered wrong
    return( FALSE );
}

bool DUIInfoRelease()
{
    // used when we're low on memory
    return( FALSE );
}
void DUIUpdate( update_list flags )
{
    // flags indicates what conditions have changed.  They should be saved
    // until an appropriate time, then windows updated accordingly
}
void DUIStop()
{
    // close down the UI - we're about to change modes.
}
void DUIFini()
{
    // finish up the UI
}
void DUIInit()
{
    // Init the UI
}
extern void DUIFreshAll()
{
    // refresh all screens - initialization has been done
    UpdateFlags = 0;
}
extern bool DUIStopRefresh( bool stop )
{
    // temporarily turn off/on screen refreshing, cause we're going to run a
    // big command file and we don't want flashing.
    return( FALSE );
}
extern void DUIShow()
{
    // show the main screen - the splash page has been closed
}
extern void DUIWndUser()
{
    // switch to the program screen
}
extern void DUIWndDebug()
{
    // switch to the debugger screen
}
extern void DUIShowLogWindow()
{
    // bring up the log window, cause some printout is coming
}
extern int DUIGetMonitorType()
{
    // stub for old UI
    return( 1 );
}
extern int DUIScreenSizeY()
{
    // stub for old UI
    return( 0 );
}
extern int DUIScreenSizeX()
{
    // stub for old UI
    return( 0 );
}
extern void DUIArrowCursor()
{
    // we're about to suicide, so restore the cursor to normal
}
bool DUIAskIfAsynchOk()
{
    // we're about to try to replay across an asynchronous event.  Ask user
    return( FALSE );
}
extern void DUIFlushKeys()
{
    // we're about to suicide - clear the keyboard typeahead
}
extern void DUIPlayDead( bool dead )
{
    // the app is about to run - make the debugger play dead
}
extern void DUISysEnd( bool pause )
{
    // done calling system();
}
extern void DUISysStart()
{
    // about to call system();
}
extern void DUIRingBell()
{
    // ring ring (error)
}
extern int DUIDisambiguate( const ambig_info *ambig, int count )
{
    // the expression processor detected an ambiguous symbol.  Ask user which one
    return( 0 );
}
extern void *DUIHourGlass( void *x )
{
    return( x );
}
void ProcAccel()
{
    // stub for old UI
    FlushEOC();
}
void ProcCapture()
{
    // stub for old UI
    FlushEOC();
}
void ProcDisplay()
{
    // stub for old UI
    FlushEOC();
}
void ProcFont()
{
    // stub for old UI
    FlushEOC();
}
void ProcHelp()
{
    // stub for old UI
    FlushEOC();
}
void ProcInternal()
{
    // stub for old UI
    FlushEOC();
}
void ProcPaint()
{
    // stub for old UI
    FlushEOC();
}
void ProcView()
{
    // stub for old UI
    FlushEOC();
}
void ProcWindow()
{
    // stub for old UI
    FlushEOC();
}
void ProcConfigFile()
{
    // called when main config file processed
    FlushEOC();
}
void ConfigDisp()
{
    // stub for old UI
}
void ConfigFont()
{
    // stub for old UI
}
void ConfigPaint()
{
    // stub for old UI
}
extern void DClickSet()
{
    // stub for old UI
    FlushEOC();
}
extern void DClickConf()
{
    // stub for old UI
}
extern void InputSet()
{
    // stub for old UI
    FlushEOC();
}
extern void InputConf()
{
    // stub for old UI
}
extern void MacroSet()
{
    // stub for old UI
    FlushEOC();
}
extern void MacroConf()
{
    // stub for old UI
}
extern  void    FiniMacros()
{
    // stub for old UI
}
int TabIntervalGet()
{
    // stub for old UI
    return( 0 );
}
void TabIntervalSet( int new )
{
    // stub for old UI
}
extern void TabSet()
{
    // stub for old UI
    FlushEOC();
}
extern void TabConf()
{
    // stub for old UI
}
extern void SearchSet()
{
    // stub for old UI
    FlushEOC();
}
extern void SearchConf()
{
    // stub for old UI
}
extern void FingClose()
{
    // open a splash page
}
extern void FingOpen()
{
    // close the splash page
}
extern void AsmChangeOptions()
{
    // assembly window options changed
}
extern void RegChangeOptions()
{
    // reg window options changed
}
extern void VarChangeOptions()
{
    // var window options changed
}
extern void FuncChangeOptions()
{
    // func window options changed
}
extern void GlobChangeOptions()
{
    // glob window options changed
}
extern void ModChangeOptions()
{
    // mod window options changed
}
extern void WndVarInspect(char*buff)
{
}
extern void *WndAsmInspect(address addr)
{
    // used by examine/assembly command
    int         i;
    char        buff[256];
    mad_disasm_data     *dd;

    _AllocA( dd, MADDisasmDataSize() );
    for( i = 0; i < 10; ++i ) {
        MADDisasm( dd, &addr, 0 );
        MADDisasmFormat( dd, MDP_ALL, CurrRadix, buff, sizeof( buff ) );
        InsMemRef( dd );
        printf( "%-40s%s\n", buff, TxtBuff );
    }
    return( NULL );
}
extern void *WndSrcInspect( address addr )
{
    // used by examine/source command
    return( NULL );
}
extern void WndMemInspect( address addr, char *next, unsigned len,
                           mad_type_handle type )
{
    // used by examine/byte/word/etc command
}
extern void WndIOInspect(address*addr,mad_type_handle type)
{
    // used by examine/iobyte/ioword/etc command
}
extern void GraphicDisplay()
{
    // used by print/window command
}
extern void VarUnMapScopes()
{
    // unmap variable scopes - prog about to restart
}
extern void VarReMapScopes()
{
    // remap variable scopes - prog about to restart
}
extern void VarFreeScopes()
{
    // free variable scope info
}
extern void SetLastExe( char *name )
{
    // remember last exe debugged name
}
extern void CaptureError()
{
    // error in capture command (stub)
}
extern void DUIProcPendingPaint(void)
{
    // a paint command was issued - update the screen (stub)
}
void VarSaveWndToScope( void *wnd )
{
}
void VarRestoreWndFromScope( void *wnd )
{
}

void DUIEnterCriticalSection()
{
}

void DUIExitCriticalSection()
{
}

void DUIInitLiterals()
{
}

void DUIFiniLiterals()
{
}

bool DUIGetSourceLine( cue_handle *ch, char *buff, unsigned len )
{
    void        *viewhndl;

    viewhndl = OpenSrcFile( ch );
    if( viewhndl == NULL ) return( FALSE );
    buff[ FReadLine( viewhndl, CueLine( ch ), 0, buff, len )] = '\0';
    FDoneSource( viewhndl );
    return( TRUE );
}

bool DUIIsDBCS()
{
    return( FALSE );
}

extern unsigned EnvLkup( const char *name, char *buff, unsigned buff_len );

unsigned DUIEnvLkup( const char *name, char *buff, unsigned buff_len )
{
    return( EnvLkup( name, buff, buff_len ) );
}

void DUIDirty()
{
}


void StartupErr( const char *err )
/********************************/
{
    printf( "Fatal error: %s", err );
    exit( 1 );
}

void DUISrcOrAsmInspect( address addr )
{
}

void DUIAddrInspect( address addr )
{
}

extern void RemovePoint( brkp *bp );
extern void DUIRemoveBreak( brkp *bp )
/************************************/
{
    RemovePoint( bp );
}

extern void SetMADMenuItems( void )
/**********************************/
{
}

extern void FPUChangeOptions( void )
/**********************************/
{
}

extern void MMXChangeOptions( void )
/**********************************/
{
}

extern void XMMChangeOptions( void )
/**********************************/
{
}

bool DUIImageLoaded( image_entry *image, bool load,
                     bool already_stopping, bool *force_stop )
/************************************************************/
{
    char buff[256];

    already_stopping=already_stopping;
    force_stop= force_stop;
    if( load ) {
        sprintf( buff, "%s '%s'", LIT( DLL_Loaded ), image->image_name );
    } else {
        sprintf( buff, "%s '%s'", LIT( DLL_UnLoaded ), image->image_name );
    }
    DUIDlgTxt( buff );
    return( FALSE );
}

void DUICopySize( void *cookie, unsigned long size )
/**************************************************/
{
    size = size;
    cookie = cookie;
}

void DUICopyCopied( void *cookie, unsigned long size )
/****************************************************/
{
    size = size;
    cookie = cookie;
}

bool DUICopyCancelled( void * cookie )
/************************************/
{
    cookie = cookie;
    return( FALSE );
}

unsigned DUIDlgAsyncRun( void )
/*****************************/
{
    return( 0 );
}

void DUISetNumLines( int num )
{
    num = num;
}

void DUISetNumColumns( int num )
{
    num = num;
}

void DUIInitRunThreadInfo( void )
{
}

#if defined( __NT__ )
const char *CheckForPowerBuilder( const char *name )
{
    return( name );
}
#endif
