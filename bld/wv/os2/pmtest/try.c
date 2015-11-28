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


#define INCL_WINWINDOWMGR
#define INCL_WINFRAMEMGR
#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSMEMMGR
#define INCL_DOSSIGNALS
#undef INCL_DOSINFOSEG
#undef INCL_DOSPROCESS
#define INCL_WINSWITCHLIST
#include "os2.h"
#define DBSEM_USER_SPECIFIED
#include "dbgapi.h"
#include <stddef.h>
#include <process.h>
#include <stdio.h>
#include "bsexcpt.h"
//#include "pmwin.h"
PID             Pid = 0;
ULONG          SID;
uDB_t           Buff;
ULONG           ExceptNum;

#define DBG_N_Breakpoint        -100
#define DBG_N_SStep             -101
#define DBG_N_Signal            -102

void DebugExecute( uDB_t *buff, ULONG cmd )
{
    EXCEPTIONREPORTRECORD       ex;
    ULONG                       value;
    ULONG                       stopvalue;
    ULONG                       notify=0;
//    bool                        got_second_notification;
    ULONG                       fcp;
    CONTEXTRECORD               fcr;

    buff->Cmd = cmd;
    value = buff->Value;
    if( cmd == DBG_C_Go ) {
        value = 0;
    }
    stopvalue = XCPT_CONTINUE_EXECUTION;
//    got_second_notification = false;
    if( cmd == DBG_C_Stop ) {
        stopvalue = XCPT_CONTINUE_STOP;
    }

    while( 1 ) {

        buff->Value = value;
        buff->Cmd = cmd;
        DosDebug( buff );

        value = stopvalue;
        cmd = DBG_C_Continue;

        /*
         * handle the preemptive notifications
         */
        switch( buff->Cmd ) {
        case DBG_N_ModuleLoad:
//          RecordModHandle( buff->Value );
            break;
        case DBG_N_ModuleFree:
            break;
        case DBG_N_NewProc:
            break;
        case DBG_N_ProcTerm:
            value = XCPT_CONTINUE_STOP;         /* halt us */
            notify = DBG_N_ProcTerm;
            break;
        case DBG_N_ThreadCreate:
            break;
        case DBG_N_ThreadTerm:
            break;
        case DBG_N_AliasFree:
            break;
        case DBG_N_Exception:
            if( buff->Value == DBG_X_STACK_INVALID ) {
                value = XCPT_CONTINUE_SEARCH;
                break;
            }
            fcp = buff->Len;
            if( buff->Value == DBG_X_PRE_FIRST_CHANCE ) {
                ExceptNum = buff->Buffer;
                if( ExceptNum == XCPT_BREAKPOINT ) {
                    notify = DBG_N_Breakpoint;
                    value = XCPT_CONTINUE_STOP;
                    break;
                } else if( ExceptNum == XCPT_SINGLE_STEP ) {
                    notify = DBG_N_SStep;
                    value = XCPT_CONTINUE_STOP;
                    break;
                }
            }
            //
            // NOTE: Going to second chance causes OS/2 to report the
            //       exception in the debugee.  However, if you report
            //       the fault at the first chance notification, the
            //       debugee's own fault handlers will not get invoked!
            //
            value = XCPT_CONTINUE_SEARCH;
            break;
        default:
            if( notify != 0 ) {
                buff->Cmd = notify;
            }
            return;
        }
    }
}

extern long __far16 __pascal WinQueueFromID( long, short pid, short tid );
extern long __far16 __pascal WinReplyMsg( long, long, long, long );
extern long __far16 __pascal WinThreadAssocQueue( long, long );
extern long __far16 __pascal WinQuerySendMsg( long, long, long, void* );

HAB                     A;
HMQ                     Q;
HWND                    W;
int                     ThreadGone;

void HelpingHand( void *foo )
{
    QMSG qmsg;                          /* Message from message queue   */
    short       ok;
    HMQ hmq;

    foo=foo;
    while( Q == 0 ) {
//      printf( "watchdog here\n" );
        fflush( stdout );
        DosSleep( 10 );
    }
    ok = WinThreadAssocQueue( A, Q );
    printf( "associate queue ok=%d, hab=%8.8x, hmq=%8.8x\n", ok, A, Q );
    fflush( stdout );
    while( Q != 0 ) {
        fflush( stdout );
//      if( WinPeekMsg( A, &qmsg, 0L, 0, 0, PM_REMOVE ) ) {
        if( WinGetMsg( A, &qmsg, 0L, 0, 0 ) ) {
            hmq = WinQueryWindowULong( qmsg.hwnd, QWL_HMQ );
            printf( "MSG hwnd=%8.8x msg=%d\n", qmsg.hwnd, qmsg.msg );
            fflush( stdout );
            if( Q == hmq ) {
                DosBeep( 1000, 250 );
                printf( "interesting\r\n" );
                fflush( stdout );
                WinDefWindowProc( qmsg.hwnd, qmsg.msg, qmsg.mp1, qmsg.mp2 );
            } else {
                DosBeep( 500, 250 );
                printf( "boring\r\n" );
                fflush( stdout );
                WinDispatchMsg( A, &qmsg );
            }
        }
    }
    ok = WinThreadAssocQueue( A, 0 );
    printf( "queue dissociated %d\n", ok );
    fflush( stdout );
    ThreadGone=1;
    _endthread();
}

main()
{
    STARTDATA           start;
    int                 code;
    SWCNTRL             SW;
    HSWITCH             hswitch;
    PPIB                pib;
    PTIB                tib;
    HMQ                 hmq;
    HWND                hwndme;
    QMSG                qmsg;           /* Message from message queue   */
    int                 i;

    _beginthread( HelpingHand, (void*)malloc( 10000 ), 10000, (void*)NULL );
    DosGetInfoBlocks(&tib,&pib);
    start.Length = offsetof( STARTDATA, IconFile ); /* default for the rest */
    start.Related = 1;
    start.FgBg = 1;
    start.TraceOpt = 1;
    start.PgmTitle = (PSZ) "Test Session";
    start.PgmName = "HELLO.EXE";
    start.PgmInputs = "hi there";
    start.TermQ = 0;
    start.Environment = NULL;
    start.InheritOpt = 1;
    start.SessionType = SSF_TYPE_PM;
//    start.SessionType = SSF_TYPE_FULLSCREEN;
//    start.SessionType = SSF_TYPE_WINDOWABLEVIO;
    code = DosStartSession( &start, &SID, &Pid );
    Buff.Pid = Pid;
    Buff.Tid = 0;
    Buff.Cmd = DBG_C_Connect;
    Buff.Value = DBG_L_386;
    DosDebug( &Buff );

    Buff.Pid = Pid;
    Buff.Tid = 1;
    DebugExecute( &Buff, DBG_C_Stop );
    if( Buff.Cmd != DBG_N_Success ) {
        printf( "can't load task\n" );
        fflush( stdout );
        return;
    }
    printf( "Press a key to go to app\n" );
    fflush( stdout );
//  getch();
    DosSelectSession( SID );
    DebugExecute( &Buff, DBG_C_Go );
    if( Buff.Cmd != DBG_N_Breakpoint ) {
        printf( "didn't hit break\n" );
        fflush( stdout );
        return;
    }
    Buff.Cmd = DBG_C_Stop;
    DosDebug( &Buff );
    Buff.Cmd = DBG_C_ReadReg;
    DosDebug( &Buff );
    Buff.EIP++;
    Buff.Cmd = DBG_C_WriteReg;
    DosDebug( &Buff );
    DosSelectSession( 0 );
    hswitch = WinQuerySwitchHandle( 0, pib->pib_ulpid );
    WinQuerySwitchEntry( hswitch, &SW );
    A = WinQueryAnchorBlock( hwndme = SW.hwnd );
    hmq = WinQueueFromID( A, pib->pib_ulpid, 1 );
    printf( "me: hsw = %8.8x, hwnd = %8.8x, hab = %8.8x, hmq = %8.8x\n", hswitch, SW.hwnd, A, hmq );
    fflush( stdout );
    hswitch = WinQuerySwitchHandle( 0, Pid );
    WinQuerySwitchEntry( hswitch, &SW );
    W = SW.hwnd;
    A = WinQueryAnchorBlock( W );
    for( ;; ) {
        hmq = WinQuerySendMsg( A, 0, Q, &qmsg );
        if( hmq == 0 ) break;
        printf( "SND hwnd=%8.8x msg=%d\n", qmsg.hwnd, qmsg.msg );
        fflush( stdout );
        WinReplyMsg( A, hmq, Q, 1 );
    }
    hmq = WinQueueFromID( A, Buff.Pid, Buff.Tid );
    printf( "it: hsw = %8.8x, hwnd = %8.8x, hab = %8.8x, hmq = %8.8x\n", hswitch, W, A, hmq );
    fflush( stdout );
    Q = hmq;
    printf( "press any key to post message to the app\n" );
    getch();
    WinPostMsg( W, WM_NULL, (MPARAM)0,(MPARAM)0 ); // testing
//  WinPostMsg( W, WM_QUIT, (MPARAM)0,(MPARAM)0 );/* Cause termination*/
//  printf( "sleeping\n" );
//  DosSleep( 4000 );
//  printf( "app running\n" );
//  DebugExecute( &Buff, DBG_C_Go );
    for( i = 0; i < 100; ++i ) {
        DosSleep( 100 );
        if( kbhit() ) break;
    }
    Q = 0;
//  while( !ThreadGone ) DosSleep( 1 );
}
