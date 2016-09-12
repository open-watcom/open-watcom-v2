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


#include "_srcmgt.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgerr.h"
#include "dbgadget.h"
#include "dbghook.h"
#include "dui.h"
#include "srcmgt.h"
#include "dbgscrn.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgsrc.h"
#include "dbgbrk.h"
#include "dbgpend.h"
#include "envlkup.h"
#include "dbgwdlg.h"
#include "wndsys.h"
#include "dlgexpr.h"
#include "dbginit.h"
#include "dbgwdisp.h"
#include "dbgwinsp.h"
#include "dbgwio.h"
#include "dbgwmem.h"
#include "dbgwpain.h"
#include "dbgwfing.h"
#include "dbgwrtrd.h"
#include "dbgwset.h"
#include "dbgwtool.h"
#include "dbgwvar.h"
#include "dlgasync.h"
#include "dlgfile.h"
#include "wndhelp.h"
#include "wndmenu.h"
#include "fingmsg.h"


extern a_window         *WndMain;
extern const char       WndNameTab[];

extern GUICALLBACK      WndMainEventProc;
extern void             DlgNewProg( void );
extern void             InitPaint( void );
extern void             InitFileMap( void );
extern void             InitScreen( void );
extern void             FiniPaint( void );
extern void             FiniFileMap( void );
extern void             FiniScreen( void );

extern void             InitSuppServices( void );

extern unsigned         ConfigScreen( void );

#define TIMER_MS        250

#if defined( __NT__ ) && defined( __GUI__ )

#include <windows.h>
#include "guitimer.h"

#define TIMER_ID        200

void GUITimer( void )
{
    AsyncNotify();
    RunThreadNotify();
}

static void StartTimer( void )
{
    GUIStartTimer( NULL, TIMER_ID, TIMER_MS );
}

static void StopTimer( void )
{
    GUIStopTimer( NULL, TIMER_ID );
}

#elif defined( __RDOS__ )

extern void uitimer( void (*proc)( void ), int ms );

void GUITimer( void )
{
    AsyncNotify();
    RunThreadNotify();
}

static void StartTimer( void )
{
    uitimer( GUITimer, TIMER_MS );
}

static void StopTimer( void )
{
    uitimer( NULL, 0 );
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

void DUIStatusText( const char *text )
{
    WndStatusText( text );
}

void DUIMsgBox( const char *text )
{
    WndMsgBox( text );
}

bool DUIDlgTxt( const char *text )
{
    return( WndDlgTxt( text ) );
}

void DUIInfoBox( const char *text )
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
    InitScreen();
    CmdHistory = WndInitHistory();
    SrchHistory = WndInitHistory();
    InitToolBar();
    InitMemWindow();
    InitAboutMessage();
    InitIOWindow();
    InitMenus();
    WndInit( LIT_DUI( The_WATCOM_Debugger ) );
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
    FiniScreen();
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

void DUIErrorBox( const char *buff )
{
    WndDisplayMessage( buff, LIT_DUI( Error ), GUI_INFORMATION + GUI_SYSTEMMODAL );
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
    return( WndDisplayMessage( LIT_DUI( WARN_Asynch_Event ), LIT_ENG( Empty ), GUI_YES_NO ) == GUI_RET_YES );
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
    if( DlgInfoRelease() )
        return( true );
    if( VarInfoRelease() )
        return( true );
    return( false );
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
    if( viewhndl == NULL )
        return( false );
    buff[FReadLine( viewhndl, DIPCueLine( ch ), 0, buff, len )] = NULLCHAR;
    FDoneSource( viewhndl );
    return( true );
}

bool DUIIsDBCS( void )
{
    return( GUIIsDBCS() );
}

size_t DUIEnvLkup( const char *name, char *buff, size_t buff_len )
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
    return( false );
}

bool DUIImageLoaded( image_entry *image, bool load,
                     bool already_stopping, bool *force_stop )
/************************************************************/
{
    already_stopping=already_stopping;
    force_stop= force_stop;
    if( load ) {
        Format( TxtBuff, "%s '%s'", LIT_ENG( DLL_Loaded ), image->image_name );
    } else {
        Format( TxtBuff, "%s '%s'", LIT_ENG( DLL_UnLoaded ), image->image_name );
    }
    DUIDlgTxt( TxtBuff );
    return( false );
}

unsigned DUIDlgAsyncRun( void )
{
    return( DlgAsyncRun() );
}

void DUISetNumLines( int num )
{
    SetNumLines( num );
}

void DUISetNumColumns( int num )
{
    SetNumColumns( num );
}

void DUIInitRunThreadInfo( void )
{
    InitRunThreadInfo();
}

void DUIScreenOptInit( void )
{
    ScreenOptInit();
}

bool DUIScreenOption( const char *start, unsigned len, int pass )
{
    return( ScreenOption( start, len, pass ) );
}

unsigned DUIConfigScreen( void )
{
    return( ConfigScreen() );
}

void DUIProcWindow( void )
{
    WndProcWindow();
}

bool DUIDlgGivenAddr( const char *title, address *value )
{
    return( DlgGivenAddr( title, value ) );
}

void DUIFingOpen( void )
{
    FingOpen();
}

void DUIFingClose( void )
{
    FingClose();
}
