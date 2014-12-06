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


#include <stdlib.h>
#include "_srcmgt.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgerr.h"
#include "dbgadget.h"
#include "dbghook.h"
#include "dui.h"
#include "srcmgt.h"
#include "dbgscrn.h"


extern a_window         *WndMain;
extern char             WndNameTab[];

extern unsigned long    CueLine( cue_handle *ch );
extern GUICALLBACK      WndMainEventProc;
extern void             TellWinHandle( void );
extern void             DlgNewProg( void );
extern void             InitToolBar( void );
extern void             InitMemWindow( void );
extern void             InitAboutMessage( void );
extern void             InitIOWindow( void );
extern void             InitMenus( void );
extern void             InitHelp( void );
extern void             InitGadget( void );
extern void             InitPaint( void );
extern void             InitBrowse( void );
extern void             InitFont( void );
extern void             InitFileMap( void );
extern void             FiniMacros( void );
extern void             FiniBrowse( void );
extern void             FiniPaint( void );
extern void             FiniGadget( void );
extern void             FiniFont( void );
extern void             FiniAboutMessage( void );
extern void             FiniMenus( void );
extern void             FiniIOWindow( void );
extern void             FiniMemWindow( void );
extern void             FiniToolBar( void );
extern void             FiniFileMap( void );
extern void             WndDlgFini( void );
extern void             DoProcPending( void );
extern void             SetTargMenuItems( void );
extern void             SetBrkMenuItems( void );
extern void             SetIOMenuItems( void );
extern void             DoInput( void );
extern void             *OpenSrcFile(cue_handle *);
extern bool             GUIIsDBCS( void );
extern unsigned         EnvLkup( char *src, char *dst, unsigned );
extern void             PopErrBox( char *buff );
extern void             KillDebugger( int ret_code );
extern char             *Format( char *, char *, ... );

extern void             InitSuppServices( void );

extern void             AsyncNotify( void );
extern void             RunThreadNotify( void );

extern unsigned         ScanCmd( char *cmd_table );
extern void             WndMsgBox( char *text );
extern bool             WndDlgTxt( char *text );
extern void             WndInfoBox( char *text );
extern void             WndUser( void );
extern void             WndDebug( void );
extern a_window         *WndClassInspect( wnd_class class );
extern char             *WndLoadString( int i );
extern void             WndFlushKeys( void );
extern void             PlayDead( bool );
extern void             WndSysEnd( bool pause );
extern void             WndSysStart( void );
extern void             ProcPendingPaint( void );
extern bool             DlgInfoRelease( void );
extern bool             VarInfoRelease( void );
extern void             WndSrcOrAsmInspect( address );
extern void             WndAddrInspect( address );
extern void             RemovePoint( brkp *bp );

#define TIMER_MS        250

void GUITimer( void )
{
    AsyncNotify();
    RunThreadNotify();
}

#if defined( __NT__ ) && defined( __GUI__ )

#define TIMER_ID        200

extern void GUIStartTimer( gui_window *wnd, int id, int msec );
extern void GUIStopTimer( gui_window *wnd, int id );

static void StartTimer( void )
{
    GUIStartTimer( 0, TIMER_ID, TIMER_MS );
}

static void StopTimer( void )
{
    GUIStopTimer( 0, TIMER_ID );
}

#elif defined( __RDOS__ )

extern void uitimer( void ( *proc )(), int ms );

static void StartTimer( void )
{
    uitimer( GUITimer, TIMER_MS );
}

static void StopTimer( void )
{
    uitimer( 0, 0 );
}

#else

static void StartTimer( void )
{
}

static void StopTimer( void )
{
}

#endif


void DUIUpdate( update_list flags )
{
    if( flags & UP_NEW_PROGRAM ) {
        SetTargMenuItems();
        SetIOMenuItems();
        SetBrkMenuItems();
    } else if( flags & UP_BREAK_CHANGE ) {
        SetBrkMenuItems();
    }
}

void DUIStatusText( char *text )
{
    WndStatusText( text );
}

void DUIMsgBox( char *text )
{
    WndMsgBox( text );
}

bool DUIDlgTxt( char *text )
{
    return( WndDlgTxt( text ) );
}

void DUIInfoBox( char *text )
{
    WndInfoBox( text );
}

void DUIStop( void )
{
    WndStop();
}

bool DUIClose( void )
{
    return( WndFini() );
}

void DUIInit( void )
{
    CmdHistory = WndInitHistory();
    SrchHistory = WndInitHistory();
    InitToolBar();
    InitMemWindow();
    InitAboutMessage();
    InitIOWindow();
    InitMenus();
    WndInit( LIT( The_WATCOM_Debugger ) );
    _SwitchOff( SW_ERROR_STARTUP );
#if defined(__GUI__)
    TellWinHandle();
#endif
    if( WndMain != NULL ) WndSetIcon( WndMain, &MainIcon );
    StartTimer();
    InitHelp();
    InitGadget();
    InitPaint();
    InitBrowse();
    InitFont();
}

void DUIFini( void )
{
    StopTimer();
    WndFiniHistory( SrchHistory );
    WndFiniHistory( CmdHistory );
    FiniMacros();
    FiniBrowse();
    FiniPaint();
    FiniGadget();
    FiniFont();
    FiniAboutMessage();
    FiniMenus();
    FiniIOWindow();
    FiniMemWindow();
    FiniToolBar();
    WndDlgFini();
}

void DUIFreshAll( void )
{
    WndFreshAll();
}

bool DUIStopRefresh( bool ok )
{
    return( WndStopRefresh( ok ) );
}

void DUIShow( void )
{
    WndDebug();
    WndShowAll();
    WndShowWndMain();
    WndMainEventProc( WndGui( WndMain ), GUI_NO_EVENT, NULL );
    if( _IsOff( SW_HAVE_TASK ) && _IsOff( SW_PROC_ALREADY_STARTED ) ) {
        DlgNewProg();
    }
}

void DUIWndUser( void )
{
    WndUser();
}

void DUIWndDebug( void )
{
    WndDebug();
}

void DUIShowLogWindow( void )
{
    WndClassInspect( WND_DIALOGUE );
}

wnd_class ReqWndName( void )
{
    wnd_class   class;

    class = ScanCmd( WndNameTab );
    if( class == 0 ) Error( ERR_LOC, LIT( ERR_BAD_WIND_NAME ) );
    return( class-1 );
}

int DUIGetMonitorType( void )
{
    if( GUIIsGUI() ) {
        return( 1 );
    } else {
        gui_system_metrics  metrics;
        GUIGetSystemMetrics( &metrics );
        return( metrics.colour != 0 );
    }
}

int DUIScreenSizeY( void )
{
    return( WndScreen.y );
}

int DUIScreenSizeX( void )
{
    return( WndScreen.x );
}

void DUIErrorBox( char *buff )
{
    WndDisplayMessage( buff, LIT( Error ), GUI_INFORMATION + GUI_SYSTEMMODAL );
}

void DUIArrowCursor( void )
{
    WndArrowCursor();
}

char *DUILoadString( int i )
{
    return( WndLoadString( i ) );
}

bool DUIAskIfAsynchOk( void )
{
    return( WndDisplayMessage( LIT( WARN_Asynch_Event ), LIT( Empty ), GUI_YES_NO ) == GUI_RET_YES );
}

void DUIFlushKeys( void )
{
    WndFlushKeys();
}

void DUIPlayDead( bool dead )
{
    PlayDead( dead );
}

void DUISysEnd( bool pause )
{
    WndSysEnd( pause );
}

void DUISysStart( void )
{
    WndSysStart();
}

void DUIRingBell( void )
{
    Ring_Bell();
}

void DUIProcPendingPaint( void )
{
    ProcPendingPaint();
}

bool DUIInfoRelease( void )
{
    if( DlgInfoRelease() ) return( TRUE );
    if( VarInfoRelease() ) return( TRUE );
    return( FALSE );
}

void *DUIHourGlass( void *x )
{
    return( WndHourGlass( x ) );
}

void WndDoInput( void )
{
    InitSuppServices();
    DoInput();
}

void DUIEnterCriticalSection( void )
{
}

void DUIExitCriticalSection( void )
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
    return( GUIIsDBCS() );
}

unsigned DUIEnvLkup( char *name, char *buff, unsigned buff_len )
{
    return( EnvLkup( name, buff, buff_len ) );
}

void DUIDirty( void )
{
    WndDirty( NULL );
}

void DUISrcOrAsmInspect( address addr )
{
    WndSrcOrAsmInspect( addr );
}

void DUIAddrInspect( address addr )
{
    WndAddrInspect( addr );
}

void DUIRemoveBreak( brkp *bp )
/***********************************/
{
    RemovePoint( bp );
}


void StartupErr( char *err )
/**************************/
{
    PopErrBox( err );
    KillDebugger(1);
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

bool DUISymbolsCancelled( void )
/******************************/
{
    return( FALSE );
}

bool DUIImageLoaded( image_entry *image, bool load,
                     bool already_stopping, bool *force_stop )
/************************************************************/
{
    already_stopping=already_stopping;
    force_stop= force_stop;
    if( load ) {
        Format( TxtBuff, "%s '%s'", LIT( DLL_Loaded ), image->image_name );
    } else {
        Format( TxtBuff, "%s '%s'", LIT( DLL_UnLoaded ), image->image_name );
    }
    DUIDlgTxt( TxtBuff );
    return( FALSE );
}
