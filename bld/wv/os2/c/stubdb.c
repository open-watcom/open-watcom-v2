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


#include "_srcmgt.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <process.h>
#define INCL_DOS
#include <os2.h>
#include "dbgdefn.h"
#include "dbgmem.h"
#include "dbginfo.h"
#include "bool.h"
#include "ambigsym.h"
#include "dbgtoggl.h"
#include "dbglit.h"
#include "mad.h"
#include "dui.h"
#include "dbgvar.h"
#include "dbgstk.h"
#include "srcmgt.h"
#include "trapaccs.h"


extern void             FlushEOC( void );
extern char             *DupStr( char * );
extern void             DoCmd( char * );
extern bool             InsMemRef( mad_disasm_data *dd );
extern address          GetCodeDot( void );
extern void             *WndAsmInspect( address addr );
extern void             DebugMain( void );
extern void             DebugFini( void );
extern void             DoInput( void );
extern void             UnAsm( address addr, unsigned max, char *buff );
extern char             *DupStr( char * );
extern bool             DUIGetSourceLine( cue_handle *ch, char *buff, unsigned len );
extern void             ExecTrace( trace_cmd_type type, debug_level level );
extern unsigned         Go( bool );
extern void             *OpenSrcFile( cue_handle * );
extern int              FReadLine( void *, int, int, char *, int );
extern void             FDoneSource( void * );
extern void             ExprValue( stack_entry * );

extern debug_level      DbgLevel;
extern char             *TxtBuff;
extern unsigned char    CurrRadix;
static bool             Done;
extern char             *TrpFile;
extern char             *CmdData;
extern stack_entry      *ExprSP;

unsigned                NumLines;
unsigned                NumColumns;

char *_LITDOS_invalid_function = "invalid function";
char *_LITDOS_file_not_found = "file not found";
char *_LITDOS_path_not_found = "path not found";
char *_LITDOS_too_many_open_files = "too many open files";
char *_LITDOS_access_denied = "access denied";
char *_LITDOS_invalid_handle = "invalid handle";
char *_LITDOS_memory_control = "memory control";
char *_LITDOS_insufficient_memory = "insufficient memory";
char *_LITDOS_invalid_address = "invalid address";
char *_LITDOS_invalid_environment = "invalid environment";
char *_LITDOS_invalid_format = "invalid format";
char *_LITDOS_invalid_access_code = "invalid access code";
char *_LITDOS_invalid_data = "invalid data";
char *_LITDOS_invalid_drive = "invalid drive";
char *_LITDOS_remove_cd = "remove cd";
char *_LITDOS_not_same_device = "not same device";

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

bool DUIClose( void )
{
    Done = TRUE;
    return( TRUE );
}

// The following routine is cut & pasted verbatim from dbgwvar.c
// (which we really don't want to drag in here)
var_node *VarGetDisplayPiece( var_info *i, int row, int piece, int *pdepth, int *pinherit )
{
    var_node    *row_v;
    var_node    *v;

    if( piece >= VAR_PIECE_LAST ) return( NULL );
    if( VarFirstNode( i ) == NULL ) return( NULL );
    if( row >= VarRowTotal( i ) ) return( NULL );
    row_v = VarFindRowNode( i, row );
    if( !row_v->value_valid ) {
        VarSetValue( row_v, LIT( Quest_Marks ) );
        row_v->value_valid = FALSE;
    }
    if( !row_v->gadget_valid ) {
        VarSetGadget( row_v, VARGADGET_NONE );
        row_v->gadget_valid = FALSE;
    }
    v = row_v;
    if( piece == VAR_PIECE_NAME ||
        ( piece == VAR_PIECE_GADGET && row_v->gadget_valid ) ||
        ( piece == VAR_PIECE_VALUE && row_v->value_valid ) ) {
        VarError = FALSE;
    } else if( !_IsOn( SW_TASK_RUNNING ) ) {
        if( row == i->exprsp_cacherow && i->exprsp_cache != NULL ) {
            VarError = FALSE;
            v = i->exprsp_cache;
        } else if( row == i->exprsp_cacherow && i->exprsp_cache_is_error ) {
            VarError = TRUE;
            v = NULL;
        } else {
            VarErrState();
            v = VarFindRow( i, row );
            VarOldErrState();
            i->exprsp_cacherow = row;
            i->exprsp_cache = v;
            i->exprsp_cache_is_error = VarError;
        }
        if( v == NULL ) {
            if( !VarError ) return( NULL );
            v = row_v;
        }
        VarNodeInvalid( v );
        VarErrState();
        ExprValue( ExprSP );
        VarSetGadget( v, VarGetGadget( v ) );
        VarSetOnTop( v, VarGetOnTop( v ) );
        VarSetValue( v, VarGetValue( i, v ) );
        VarOldErrState();
        VarDoneRow( i );
    }
    VarGetDepths( i, v, pdepth, pinherit );
    return( v );
}

var_info        Locals;
HEV             Requestsem;
HEV             Requestdonesem;

static void DumpLocals( void )
{
    address     addr;
    int         row;
    int         depth;
    int         inherit;
    var_node    *v;

    if( !_IsOn( SW_TASK_RUNNING ) ) {
        VarErrState();
        VarInfoRefresh( VAR_LOCALS, &Locals, &addr, NULL );
        VarOkToCache( &Locals, TRUE );
    }
    for( row = 0;; ++row ) {
        v = VarGetDisplayPiece( &Locals, row, VAR_PIECE_GADGET, &depth, &inherit );
        if( v == NULL ) break;
        v = VarGetDisplayPiece( &Locals, row, VAR_PIECE_NAME, &depth, &inherit );
        v = VarGetDisplayPiece( &Locals, row, VAR_PIECE_VALUE, &depth, &inherit );
        switch( v->gadget ) {
        case VARGADGET_NONE:
            printf( "  " );
            break;
        case VARGADGET_OPEN:
            printf( "+ " );
            break;
        case VARGADGET_CLOSED:
            printf( "- " );
            break;
        case VARGADGET_POINTS:
            printf( "->" );
            break;
        case VARGADGET_UNPOINTS:
            printf( "<-" );
            break;
        }
        VarBuildName( &Locals, v, TRUE );
        printf( " %-20s %s\n", TxtBuff, v->value );
    }
    if( !_IsOn( SW_TASK_RUNNING ) ) {
        VarOkToCache( &Locals, FALSE );
        VarOldErrState();
    }
}

static void DumpSource( void )
{
    char        buff[256];
    DIPHDL( cue, ch );

    if( _IsOn( SW_TASK_RUNNING ) ) {
        printf( "I don't know where the task is. It's running\n" );
    }
    if( DeAliasAddrCue( NO_MOD, GetCodeDot(), ch ) == SR_NONE ||
        !DUIGetSourceLine( ch, buff, sizeof( buff ) ) ) {
        UnAsm( GetCodeDot(), sizeof( buff ), buff );
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

VOID APIENTRY ControlFunc( ULONG parm )
{
    ULONG   ulCount;

    parm = parm;
    do {
        DosWaitEventSem( Requestsem, SEM_INDEFINITE_WAIT ); // wait for Request
        DosResetEventSem( Requestsem, &ulCount );
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
        DosPostEventSem( Requestdonesem );
    } while( Req != REQ_BYE );
    return; // thread over!
}

void RunRequest( int req )
{
    ULONG   ulCount;

    if( _IsOn( SW_TASK_RUNNING ) ) return;
    DosWaitEventSem( Requestdonesem, SEM_INDEFINITE_WAIT ); // wait for last request to finish
    DosResetEventSem( Requestdonesem, &ulCount );
    Req = req;
    _SwitchOn( SW_TASK_RUNNING );
    DosPostEventSem( Requestsem ); // tell worker to go
}

void DlgCmd( void )
{
    char        buff[256];

    printf( "DBG>" );
    fflush( stdout );   // not really necessary
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
                HMODULE hmod;
                PFN     proc = NULL;

                DosQueryModuleHandle( TrpFile, &hmod );
                DosQueryProcAddr( hmod, 5, 0, &proc );
//                if( proc != NULL )
//                    proc();
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

int main( int argc, char **argv )
{
    char        buff[256];
    TID         tid;
    APIRET      rc;

    MemInit();
    getcmd( buff );
    CmdData = buff;
    DebugMain();
    _SwitchOff( SW_ERROR_STARTUP );
    DoInput();
    VarInitInfo( &Locals );
    DosCreateEventSem( NULL, &Requestsem, 0, FALSE );
    DosCreateEventSem( NULL, &Requestdonesem, 0, FALSE );
    DosPostEventSem( Requestdonesem ); // signal req done
    rc = DosCreateThread( &tid, ControlFunc, 0, 0, 32768 );
    if( rc != 0 ) {
        printf( "Stubugger: Error creating thread!\n" );
    }
    while( !Done ) {
        DlgCmd();
    }
    DosCloseEventSem( Requestsem );
    DosCloseEventSem( Requestdonesem );
    DebugFini();
    RunRequest( REQ_BYE );
    MemFini();
    return( 0 );
}

// Minimalist DUI callback routines

extern char *DUILoadString( int id )
{
    char        buff[256];
    char        *ret;
    int         size;

    size = WinLoadString( 0, NULLHANDLE, id, sizeof( buff ), buff );
//    size = LoadString( GetModuleHandle( NULL ), id, buff, sizeof( buff ) );
    buff[size]='\0';
    ret = DbgAlloc( size+1 );
    strcpy( ret, buff );
    return( ret );
}

void DUIMsgBox( char *text )
{
    printf( "MSG %s\n", text );
}

bool DUIDlgTxt( char *text )
{
    printf( "DLG %s\n", text );
    return( TRUE );
}

void DUIInfoBox( char *text )
{
    printf( "INF %s\n", text );
}

void DUIErrorBox( char *text )
{
    printf( "ERR %s\n", text );
}

void DUIStatusText( char *text )
{
    printf( "STA %s\n", text );
}

bool DlgGivenAddr( char *title, address *value )
{
    // needed when segment's don't map (from new/sym command)
    return( FALSE );
}

void DlgNewWithSym( char *text, char *buff, int buff_len )
{
    // used by print command with no arguments
}

bool DlgUpTheStack( void )
{
    // used when trying to trace, but we've unwound the stack a bit
    return( FALSE );
}

bool DlgAreYouNuts( unsigned long mult )
{
    // used when too many break on write points are set
    return( FALSE );
}

bool DlgBackInTime( void )
{
    // used when trying to trace, but we've backed up over a call or asynch
    return( FALSE );
}

bool DlgIncompleteUndo( void )
{
    // used when trying to trace, but we've backed up over a call or asynch
    return( FALSE );
}

bool DlgBreak( address addr )
{
    // used when an error occurs in the break point expression or it is entered wrong
    return( FALSE );
}

bool DUIInfoRelease( void )
{
    // used when we're low on memory
    return( FALSE );
}
void DUIUpdate( update_list flags )
{
    // flags indicates what conditions have changed.  They should be saved
    // until an appropriate time, then windows updated accordingly
}

void DUIStop( void )
{
    // close down the UI - we're about to change modes.
}

void DUIFini( void )
{
    // finish up the UI
}

void DUIInit( void )
{
    // Init the UI
}

extern void DUIFreshAll( void )
{
    // refresh all screens - initialization has been done
}

extern bool DUIStopRefresh( bool stop )
{
    // temporarily turn off/on screen refreshing, cause we're going to run a
    // big command file and we don't want flashing.
    return( FALSE );
}

extern void DUIShow( void )
{
    // show the main screen - the splash page has been closed
}

extern void DUIWndUser( void )
{
    // switch to the program screen
}

extern void DUIWndDebug( void )
{
    // switch to the debugger screen
}

extern void DUIShowLogWindow( void )
{
    // bring up the log window, cause some printout is coming
}

extern int DUIGetMonitorType( void )
{
    // stub for old UI
    return( 1 );
}

extern int DUIScreenSizeY( void )
{
    // stub for old UI
    return( 0 );
}

extern int DUIScreenSizeX( void )
{
    // stub for old UI
    return( 0 );
}

extern void DUIArrowCursor( void )
{
    // we're about to suicide, so restore the cursor to normal
}

bool DUIAskIfAsynchOk( void )
{
    // we're about to try to replay across an asynchronous event.  Ask user
    return( FALSE );
}

extern void DUIFlushKeys( void )
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

extern void DUISysStart( void )
{
    // about to call system();
}

extern void DUIRingBell( void )
{
    // ring ring (error)
}

extern int DUIDisambiguate( ambig_info *ambig, int count )
{
    // the expression processor detected an ambiguous symbol.  Ask user which one
    return( 0 );
}

extern void *DUIHourGlass( void *x )
{
    return( x );
}

void ProcAccel( void )
{
    // stub for old UI
    FlushEOC();
}

void ProcCapture( void )
{
    // stub for old UI
    FlushEOC();
}

void ProcDisplay( void )
{
    // stub for old UI
    FlushEOC();
}

void ProcFont( void )
{
    // stub for old UI
    FlushEOC();
}

void ProcHelp( void )
{
    // stub for old UI
    FlushEOC();
}

#ifdef DBG_DBG
void ProcInternal( void )
{
    // stub for old UI
    FlushEOC();
}
#endif

void ProcPaint( void )
{
    // stub for old UI
    FlushEOC();
}

void ProcView( void )
{
    // stub for old UI
    FlushEOC();
}

void ProcWindow( void )
{
    // stub for old UI
    FlushEOC();
}

void ProcConfigFile( void )
{
    // called when main config file processed
    FlushEOC();
}

void ConfigDisp( void )
{
    // stub for old UI
}

void ConfigFont( void )
{
    // stub for old UI
}

void ConfigPaint( void )
{
    // stub for old UI
}

extern void DClickSet( void )
{
    // stub for old UI
    FlushEOC();
}

extern void DClickConf( void )
{
    // stub for old UI
}

extern void InputSet( void )
{
    // stub for old UI
    FlushEOC();
}

extern void InputConf( void )
{
    // stub for old UI
}

extern void MacroSet( void )
{
    // stub for old UI
    FlushEOC();
}

extern void MacroConf( void )
{
    // stub for old UI
}

extern  void    FiniMacros( void )
{
    // stub for old UI
}
int TabIntervalGet( void )
{
    // stub for old UI
    return( 0 );
}
void TabIntervalSet( int new )
{
    // stub for old UI
}

extern void TabSet( void )
{
    // stub for old UI
    FlushEOC();
}

extern void TabConf( void )
{
    // stub for old UI
}

extern void SearchSet( void )
{
    // stub for old UI
    FlushEOC();
}

extern void SearchConf( void )
{
    // stub for old UI
}

extern void FingClose( void )
{
    // close the splash page
}

extern void FingOpen( void )
{
    // open a splash page
}

extern void AsmChangeOptions( void )
{
    // assembly window options changed
}

extern void RegChangeOptions( void )
{
    // reg window options changed
}

extern void VarChangeOptions( void )
{
    // var window options changed
}

extern void FuncChangeOptions( void )
{
    // func window options changed
}

extern void GlobChangeOptions( void )
{
    // glob window options changed
}

extern void ModChangeOptions( void )
{
    // mod window options changed
}

extern void WndVarInspect( char *buff )
{
}

extern void *WndAsmInspect( address addr )
{
    // used by examine/assembly command
    int         i;
    char        buff[256];
    mad_disasm_data     *dd;

    _AllocA( dd, MADDisasmDataSize() );
    for( i = 0; i < 10; ++i ) {
        MADDisasm( dd, &addr, 0 );
        MADDisasmFormat( dd, MDP_ALL, CurrRadix, sizeof( buff ), buff );
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

extern void GraphicDisplay( void )
{
    // used by print/window command
}

extern void VarUnMapScopes( struct image_entry *img )
{
    // unmap variable scopes - prog about to restart
}

extern void VarReMapScopes( struct image_entry *img )
{
    // remap variable scopes - prog about to restart
}

extern void VarFreeScopes( void )
{
    // free variable scope info
}

extern void SetLastExe( char *name )
{
    // remember last exe debugged name
}

extern void CaptureError( void )
{
    // error in capture command (stub)
}

extern void DUIProcPendingPaint( void )
{
    // a paint command was issued - update the screen (stub)
}

void VarSaveWndToScope( void *wnd )
{
}

void VarRestoreWndFromScope( void *wnd )
{
}

void PopErrBox( char *buff )
{
    printf( "%s: %s\n", buff, LIT( Debugger_Startup_Error ) );
//    MessageBox( (HWND) NULL, buff, LIT( Debugger_Startup_Error ),
//            MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
}

void DUIEnterCriticalSection( void )
{
}

void DUIExitCriticalSection( void )
{
}

void DUIInitLiterals( void )
{
}

void DUIFiniLiterals( void )
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

bool DUIIsDBCS( void )
{
    return( FALSE );
}

extern unsigned EnvLkup( char *src, char *dst, unsigned );

unsigned DUIEnvLkup( char *src, char *dst, unsigned max_len )
{
    return( EnvLkup( src, dst, max_len ) );
}

void DUIDirty( void )
{
}


void StartupErr( char *err )
/**************************/
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

extern void RemovePoint( void *bp );
extern void DUIRemoveBreak( void *bp )
/***********************************/
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

unsigned OnAnotherThread( unsigned(*rtn)(unsigned,void *,unsigned,void *), unsigned in_len, void *in, unsigned out_len, void *out )
{
    return( rtn( in_len, in, out_len, out ) );
}
