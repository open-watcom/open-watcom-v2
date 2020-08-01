/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
#define INCL_DOS
#include <os2.h>
#include "_srcmgt.h"
#include "dbgdata.h"
#include "dbgmem.h"
#include "dbglit.h"
#include "mad.h"
#include "dui.h"
#include "dbgvar.h"
#include "dbgstk.h"
#include "srcmgt.h"
#include "trapaccs.h"
#include "dbgscrn.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgutil.h"
#include "dbgsrc.h"
#include "dbgexec.h"
#include "dbgexpr2.h"
#include "dbgmain.h"
#include "dbgbrk.h"
#include "dbgass.h"
#include "dbgpend.h"
#include "envlkup.h"
#include "dbgcmd.h"
#include "dbgtrace.h"
#include "trpld.h"
#include "dipimp.h"
#include "dipinter.h"
#include "dbgdot.h"
#include "dlgcmd.h"
#include "dbgwintr.h"
#include "dbgchopt.h"
#include "dbgsetfg.h"
#include "dbginsp.h"
#include "dbgwvar1.h"
#include "dlgfile.h"
#include "wndmenu1.h"
#include "dbgwset1.h"
#include "dlgbreak.h"


extern char             *CmdData;

/*************************************************/
/* TODO! review all these prototypes declaration if they are local(static) or external */

bool DlgNewWithSym( const char *title, char *buff, size_t buff_len );
bool DlgUpTheStack( void );
bool DlgAreYouNuts( unsigned long mult );
bool DlgBackInTime( bool warn );
bool DlgIncompleteUndo( void );
void ProcAccel( void );
void ProcDisplay( void );
void ProcFont( void );
void ProcHelp( void );
void ProcInternal( void );
void ProcPaint( void );
void ProcView( void );
void ProcConfigFile( void );
void ConfigDisp( void );
void ConfigFont( void );
void ConfigPaint( void );
int TabIntervalGet( void );
void TabIntervalSet( int new );
void PopErrBox( const char *buff );
/*************************************************/

unsigned                NumLines;
unsigned                NumColumns;

static bool             Done;

unsigned DUIConfigScreen( void )
{
    return( 0 );
}

bool DUIClose( void )
{
    Done = true;
    return( true );
}

#if 0
// The following routine is cut & pasted verbatim from dbgwvar.c
// (which we really don't want to drag in here)
var_node *VarGetDisplayPiece( var_info *i, int row, wnd_piece piece, int *pdepth, int *pinherit )
{
    var_node    *row_v;
    var_node    *v;

    if( piece >= VAR_PIECE_LAST )
        return( NULL );
    if( VarFirstNode( i ) == NULL )
        return( NULL );
    if( row >= VarRowTotal( i ) )
        return( NULL );
    row_v = VarFindRowNode( i, row );
    if( !row_v->value_valid ) {
        VarSetValue( row_v, LIT_ENG( Quest_Marks ) );
        row_v->value_valid = false;
    }
    if( !row_v->gadget_valid ) {
        VarSetGadget( row_v, VARGADGET_NONE );
        row_v->gadget_valid = false;
    }
    v = row_v;
    if( piece == VAR_PIECE_NAME ||
        ( piece == VAR_PIECE_GADGET && row_v->gadget_valid ) ||
        ( piece == VAR_PIECE_VALUE && row_v->value_valid ) ) {
        VarError = false;
    } else if( _IsOff( SW_TASK_RUNNING ) ) {
        if( row == i->exprsp_cacherow && i->exprsp_cache != NULL ) {
            VarError = false;
            v = i->exprsp_cache;
        } else if( row == i->exprsp_cacherow && i->exprsp_cache_is_error ) {
            VarError = true;
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
            if( !VarError )
                return( NULL );
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
#endif

var_info        Locals;
HEV             Requestsem;
HEV             Requestdonesem;

static void DumpLocals( void )
{
    address     addr;

    if( _IsOff( SW_TASK_RUNNING ) ) {
        VarErrState();
        VarInfoRefresh( VAR_LOCALS, &Locals, &addr );
        VarOkToCache( &Locals, true );
    }
#if 0
    {
        int         row;
        var_node    *v;
        int         depth;
        int         inherit;

        for( row = 0; (v = VarGetDisplayPiece( &Locals, row, VAR_PIECE_GADGET, &depth, &inherit )) != NULL; ++row ) {
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
            VarBuildName( &Locals, v, true );
            printf( " %-20s %s\n", TxtBuff, v->value );
        }
    }
#endif
    if( _IsOff( SW_TASK_RUNNING ) ) {
        VarOkToCache( &Locals, false );
        VarOldErrState();
    }
}

static void DumpSource( void )
{
    char        buff[256];
    DIPHDL( cue, cueh );

    if( _IsOn( SW_TASK_RUNNING ) ) {
        printf( "I don't know where the task is. It's running\n" );
    }
    if( DeAliasAddrCue( NO_MOD, GetCodeDot(), cueh ) == SR_NONE ||
        !DUIGetSourceLine( cueh, buff, sizeof( buff ) ) ) {
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

static void APIENTRY ControlFunc( ULONG parm )
{
    ULONG   ulCount;

    parm = parm;
    do {
        DosWaitEventSem( Requestsem, SEM_INDEFINITE_WAIT ); // wait for Request
        DosResetEventSem( Requestsem, &ulCount );
        switch( Req ) {
        case REQ_GO:
            Go( true );
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

static void RunRequest( int req )
{
    ULONG   ulCount;

    if( _IsOn( SW_TASK_RUNNING ) )
        return;
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
    if( buff[0] != NULLCHAR && buff[1] == NULLCHAR ) {
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
//
// NYI  OS/2 InterruptProgram must be implemented and exported from TRAP DLL
//          TRAP_EXTFUNC( InterruptProgram )() procedure must call this export
//
//                TRAP_EXTFUNC( InterruptProgram )();
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

    /* unused parameters */ (void)argc; (void)argv;

    MemInit();
    getcmd( buff );
    CmdData = buff;
    DebugMain();
    _SwitchOff( SW_ERROR_STARTUP );
    DoInput();
    VarInitInfo( &Locals );
    DosCreateEventSem( NULL, &Requestsem, 0, false );
    DosCreateEventSem( NULL, &Requestdonesem, 0, false );
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

char *DUILoadString( dui_res_id id )
{
    char        buff[256];
    char        *ret;
    int         size;

    size = WinLoadString( 0, NULLHANDLE, id, sizeof( buff ), buff );
//    size = LoadString( GetModuleHandle( NULL ), id, buff, sizeof( buff ) );
    buff[size++] = NULLCHAR;
    ret = DbgAlloc( size );
    memcpy( ret, buff, size );
    return( ret );
}

void DUIFreeString( void *ptr )
{
    DbgFree( ptr );
}

void DUIMsgBox( const char *text )
{
    printf( "MSG %s\n", text );
}

bool DUIDlgTxt( const char *text )
{
    printf( "DLG %s\n", text );
    return( true );
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

bool DUIDlgGivenAddr( const char *title, address *value )
{
    /* unused parameters */ (void)title; (void)value;

    // needed when segment's don't map (from new/sym command)
    return( false );
}

bool DlgNewWithSym( const char *title, char *buff, size_t buff_len )
{
    /* unused parameters */ (void)title; (void)buff; (void)buff_len;

    // used by print command with no arguments
    return( true );
}

bool DlgUpTheStack( void )
{
    // used when trying to trace, but we've unwound the stack a bit
    return( false );
}

bool DlgAreYouNuts( unsigned long mult )
{
    /* unused parameters */ (void)mult;

    // used when too many break on write points are set
    return( false );
}

bool DlgBackInTime( bool warn )
{
    /* unused parameters */ (void)warn;

    // used when trying to trace, but we've backed up over a call or asynch
    return( false );
}

bool DlgIncompleteUndo( void )
{
    // used when trying to trace, but we've backed up over a call or asynch
    return( false );
}

bool DlgBreak( address addr )
{
    /* unused parameters */ (void)addr;

    // used when an error occurs in the break point expression or it is entered wrong
    return( false );
}

bool DUIInfoRelease( void )
{
    // used when we're low on memory
    return( false );
}

void DUIUpdate( update_flags flags )
{
    /* unused parameters */ (void)flags;

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

void DUIFreshAll( void )
{
    // refresh all screens - initialization has been done
    UpdateFlags = 0;
}

bool DUIStopRefresh( bool stop )
{
    /* unused parameters */ (void)stop;

    // temporarily turn off/on screen refreshing, cause we're going to run a
    // big command file and we don't want flashing.
    return( false );
}

void DUIShow( void )
{
    // show the main screen - the splash page has been closed
}

void DUIWndUser( void )
{
    // switch to the program screen
}

void DUIWndDebug( void )
{
    // switch to the debugger screen
}

void DUIShowLogWindow( void )
{
    // bring up the log window, cause some printout is coming
}

int DUIGetMonitorType( void )
{
    // stub for old UI
    return( 1 );
}

int DUIScreenSizeY( void )
{
    // stub for old UI
    return( 0 );
}

int DUIScreenSizeX( void )
{
    // stub for old UI
    return( 0 );
}

void DUIArrowCursor( void )
{
    // we're about to suicide, so restore the cursor to normal
}

bool DUIAskIfAsynchOk( void )
{
    // we're about to try to replay across an asynchronous event.  Ask user
    return( false );
}

void DUIFlushKeys( void )
{
    // we're about to suicide - clear the keyboard typeahead
}

void DUIPlayDead( bool dead )
{
    /* unused parameters */ (void)dead;

    // the app is about to run - make the debugger play dead
}

void DUISysEnd( bool pause )
{
    /* unused parameters */ (void)pause;

    // done calling system();
}

void DUISysStart( void )
{
    // about to call system();
}

void DUIRingBell( void )
{
    // ring ring (error)
}

bool DUIDisambiguate( const ambig_info *ambig, int num_items, int *choice )
{
    /* unused parameters */ (void)ambig; (void)num_items;

    // the expression processor detected an ambiguous symbol.  Ask user which one
    *choice = 0;
    return( true );
}

void ProcAccel( void )
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

void ProcInternal( void )
{
    // stub for old UI
    FlushEOC();
}

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

void DUIProcWindow( void )
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

void DClickSet( void )
{
    // stub for old UI
    FlushEOC();
}

void DClickConf( void )
{
    // stub for old UI
}

void InputSet( void )
{
    // stub for old UI
    FlushEOC();
}

void InputConf( void )
{
    // stub for old UI
}

void MacroSet( void )
{
    // stub for old UI
    FlushEOC();
}

void MacroConf( void )
{
    // stub for old UI
}

void    FiniMacros( void )
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
    /* unused parameters */ (void)new;

    // stub for old UI
}

void TabSet( void )
{
    // stub for old UI
    FlushEOC();
}

void TabConf( void )
{
    // stub for old UI
}

void SearchSet( void )
{
    // stub for old UI
    FlushEOC();
}

void SearchConf( void )
{
    // stub for old UI
}

void DUIFingOpen( void )
{
    // open a splash page
}

void DUIFingClose( void )
{
    // close the splash page
}

void AsmChangeOptions( void )
{
    // assembly window options changed
}

void RegChangeOptions( void )
{
    // reg window options changed
}

void VarChangeOptions( void )
{
    // var window options changed
}

void FuncChangeOptions( void )
{
    // func window options changed
}

void GlobChangeOptions( void )
{
    // glob window options changed
}

void ModChangeOptions( void )
{
    // mod window options changed
}

void WndVarInspect( const char *buff )
{
    /* unused parameters */ (void)buff;
}

//void *WndAsmInspect( address addr )
void WndAsmInspect( address addr )
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
//    return( NULL );
}

//void *WndSrcInspect( address addr )
void WndSrcInspect( address addr )
{
    /* unused parameters */ (void)addr;

    // used by examine/source command
//    return( NULL );
}

void WndMemInspect( address addr, char *next, unsigned len, mad_type_handle mth )
{
    /* unused parameters */ (void)addr; (void)next; (void)len; (void)mth;

    // used by examine/byte/word/etc command
}

void WndIOInspect( address *addr, mad_type_handle mth )
{
    /* unused parameters */ (void)addr; (void)mth;

    // used by examine/iobyte/ioword/etc command
}

void WndTmpFileInspect( const char *file )
{
    /* unused parameters */ (void)file;

    // used by capture command
}

void GraphicDisplay( void )
{
    // used by print/window command
}

void VarUnMapScopes( image_entry *img )
{
    /* unused parameters */ (void)img;

    // unmap variable scopes - prog about to restart
}

void VarReMapScopes( image_entry *img )
{
    /* unused parameters */ (void)img;

    // remap variable scopes - prog about to restart
}

void VarFreeScopes( void )
{
    // free variable scope info
}

void SetLastExe( const char *name )
{
    /* unused parameters */ (void)name;

    // remember last exe debugged name
}

void DUIProcPendingPaint( void )
{
    // a paint command was issued - update the screen (stub)
}

void PopErrBox( const char *buff )
{
    printf( "%s: %s\n", buff, LIT_ENG( Debugger_Startup_Error ) );
//    MessageBox( (HWND) NULL, buff, LIT_ENG( Debugger_Startup_Error ),
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

bool DUIGetSourceLine( cue_handle *cueh, char *buff, size_t len )
{
    void        *viewhndl;

    viewhndl = OpenSrcFile( cueh );
    if( viewhndl == NULL )
        return( false );
    len = FReadLine( viewhndl, DIPCueLine( cueh ), 0, buff, len );
    if( len == FREADLINE_ERROR )
        len = 0;
    buff[len] = NULLCHAR;
    FDoneSource( viewhndl );
    return( true );
}

bool DUIIsDBCS( void )
{
    return( false );
}

size_t DUIEnvLkup( const char *name, char *buff, size_t buff_len )
{
    return( EnvLkup( name, buff, buff_len ) );
}

void DUIDirty( void )
{
}


void DUISrcOrAsmInspect( address addr )
{
    /* unused parameters */ (void)addr;
}

void DUIAddrInspect( address addr )
{
    /* unused parameters */ (void)addr;
}

void DUIRemoveBreak( brkp *bp )
/*****************************/
{
    RemovePoint( bp );
}

void SetMADMenuItems( void )
/**************************/
{
}

void FPUChangeOptions( void )
/***************************/
{
}

void MMXChangeOptions( void )
/***************************/
{
}

void XMMChangeOptions( void )
/***************************/
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
        sprintf( buff, "%s '%s'", LIT_ENG( DLL_Loaded ), image->image_name );
    } else {
        sprintf( buff, "%s '%s'", LIT_ENG( DLL_UnLoaded ), image->image_name );
    }
    DUIDlgTxt( buff );
    return( false );
}

void DUICopySize( void *cookie, unsigned long size )
/**************************************************/
{
    /* unused parameters */ (void)cookie; (void)size;
}

void DUICopyCopied( void *cookie, unsigned long size )
/****************************************************/
{
    /* unused parameters */ (void)cookie; (void)size;
}

bool DUICopyCancelled( void * cookie )
/************************************/
{
    /* unused parameters */ (void)cookie;

    return( false );
}

unsigned DUIDlgAsyncRun( void )
/*****************************/
{
    return( 0 );
}

void DUISetNumLines( int num )
{
    /* unused parameters */ (void)num;
}

void DUISetNumColumns( int num )
{
    /* unused parameters */ (void)num;
}

void DUIInitRunThreadInfo( void )
{
}

void DUIScreenOptInit( void )
{
}

bool DUIScreenOption( const char *start, unsigned len, int pass )
{
    /* unused parameters */ (void)start; (void)len; (void)pass;

    return( true );
}

#if defined( GUI_IS_GUI )
unsigned OnAnotherThreadAccess( trap_elen in_num, in_mx_entry_p in_mx, trap_elen out_num, mx_entry_p out_mx )
{
    return( TrapAccess( in_num, in_mx, out_num, out_mx ) );
}

unsigned OnAnotherThreadSimpAccess( trap_elen in_len, in_data_p in_data, trap_elen out_len, out_data_p out_data )
{
    return( TrapSimpAccess( in_len, in_data, out_len, out_data ) );
}
#endif
