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


#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSMEMMGR
#define INCL_DOSSIGNALS
#define INCL_DOSSESSIONMGR
#define INCL_DOSPROCESS
#define INCL_WIN
#define INCL_GPI
#define INCL_GPIPRIMITIVES              /* Selectively include          */
#define INCL_WINFRAMEMGR                /* relevant parts of            */
#define INCL_WINSYS                     /* the PM header file           */
#define INCL_DOSPROCESS                 /* the PM header file           */
#include <os2.h>
#include <string.h>
#include <stdio.h>
#include "wdpmhelp.h"
#include "trperr.h"

#ifdef USE_16_BIT_API
//extern BOOL __far16 __pascal WinThreadAssocQueue( HAB, HMQ );
//extern __far16 __pascal WinLockInput( ULONG, USHORT );
#else
extern BOOL APIENTRY WinLockInput( HMQ, BOOL );
extern BOOL APIENTRY WinThreadAssocQueue( HAB, HMQ );
#endif



HAB             Hab;
HMQ             Hmq;
HFILE           InStream;
HFILE           OutStream;
ULONG           DebuggerSID;
HWND            hwndClient;
HWND            hwndFrame;
HWND            FocusWnd;
HWND            ActiveWnd;
int             Locked;

PID             PidDebugee;
TID             TidDebugee;


#ifdef DEBUG
    char Message[ 256 ] = { "All is well" };
    static void Say( char *str )
    {
        if( str != NULL ) strcpy( Message, str );
        WinInvalidateRegion( hwndClient, 0L, FALSE );
    }
#else
    #define Say( x )
#endif

void UnLockIt( void )
{
    if( Locked ) {
        WinThreadAssocQueue( Hab, Hmq );
        WinLockInput( 0,0 );
        WinThreadAssocQueue( Hab, NULL );
        Locked = 0;
    }
}

VOID APIENTRY CleanUp( void )
{
    UnLockIt();
    DosExitList( EXLST_EXIT, (PFNEXITLIST)CleanUp );
}

void LockIt( void )
{
    if( !Locked ) {
        WinThreadAssocQueue( Hab, Hmq );
        WinLockInput( 0,1 );
        WinThreadAssocQueue( Hab, NULL );
        Locked = 1;
    }
}

static void SwitchBack( void )
{
    USHORT      written;

    static pmhelp_packet data = { PMHELP_SWITCHBACK };
    DosWrite( OutStream, &data, sizeof( data ), &written );
}


VOID APIENTRY ServiceRequests( VOID )
{
    USHORT              len;
    pmhelp_packet       data;

    WinCreateMsgQueue( Hab, 0 );
    for( ;; ) {
        if( DosRead( InStream, &data, sizeof( data ), &len ) != 0 ) break;
        if( len != sizeof( data ) ) break;
        switch( data.command ) {
        case PMHELP_LOCK:
            PidDebugee = data.pid;
            TidDebugee = data.tid;
            LockIt();
            break;
        case PMHELP_UNLOCK:
            PidDebugee = data.pid;
            TidDebugee = data.tid;
            UnLockIt();
            break;
        case PMHELP_EXIT:
            WinPostMsg( hwndClient, WM_QUIT, 0, 0 );/* Cause termination*/
            break;
        }
        WinInvalidateRegion( hwndClient, 0L, FALSE );
    }
    Say( "Read Failed" );
}

MRESULT EXPENTRY MyWindowProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
    HPS    hps;
    RECTL  rc;

    switch( msg ) {

    case WM_CREATE:
        break;

    case WM_COMMAND:
        switch( SHORT1FROMMP( mp1 ) ) {
        case ID_UNLOCK:
            Say( "Unlocked" );
            UnLockIt();
            if( FocusWnd != NULL ) {
                WinSetFocus( HWND_DESKTOP, FocusWnd );
            }
            WinSetActiveWindow( HWND_DESKTOP, hwndClient );
            if( ActiveWnd != NULL ) {
                WinSetActiveWindow( HWND_DESKTOP, ActiveWnd );
            }
            break;
        case ID_SWITCH:
            Say( "Switched" );
            SwitchBack();
            break;
        case ID_EXITPROG:
            WinPostMsg( hwnd, WM_CLOSE, (MPARAM)0, (MPARAM)0 );
            break;
        default:
            return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
        }
        break;

    case WM_ERASEBACKGROUND:
        return( (MRESULT)TRUE );

    case WM_PAINT:
        hps = WinBeginPaint( hwnd, 0L, &rc );
#ifdef DEBUG
        {
            POINTL pt;


            pt.x = 0; pt.y = 50;
            GpiSetColor( hps, CLR_NEUTRAL );
            GpiSetBackColor( hps, CLR_BACKGROUND );
            GpiSetBackMix( hps, BM_OVERPAINT );
            GpiCharStringAt( hps, &pt, (LONG)strlen( Message ), Message );
        }
#endif
        WinEndPaint( hps );
        break;

    case WM_CLOSE:
        WinPostMsg( hwnd, WM_QUIT, 0, 0 );
        break;

    case WM_DESTROY:
        UnLockIt();
        // fall thru

    default:
        return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );

    }
    return( FALSE );
}


VOID AbortLocker( HWND hwndFrame, HWND hwndClient )
{
   PERRINFO     pErrInfoBlk;
   PSZ          pszOffSet;
   PSZ          pszErrMsg;

   DosBeep( 100, 10 );
   if( ( pErrInfoBlk = WinGetErrorInfo(Hab) ) != (PERRINFO)NULL ) {
      pszOffSet = ((PSZ)pErrInfoBlk) + pErrInfoBlk->offaoffszMsg;
      pszErrMsg = ((PSZ)pErrInfoBlk) + *((PSHORT)pszOffSet);
      if( (INT)hwndFrame && (INT)hwndClient ) {
         WinMessageBox(HWND_DESKTOP,         /* Parent window is desk top */
                       hwndFrame,            /* Owner window is our frame */
                       (PSZ)pszErrMsg,       /* PMWIN Error message       */
                       TRP_The_WATCOM_Debugger,      /* Title bar message         */
                       MSGBOXID,             /* Message identifier        */
                       MB_MOVEABLE | MB_CUACRITICAL | MB_CANCEL ); /* Flags */
      }
      WinFreeErrorInfo( pErrInfoBlk );
   }
   WinPostMsg( hwndClient, WM_QUIT, (MPARAM)NULL, (MPARAM)NULL );
}


#define AbortIf( x ) if( x ) AbortLocker( hwndFrame, hwndClient )

#define STACK_SIZE 8192
static char     stack[STACK_SIZE];
INT main( int argc, char **argv )
{
    QMSG qmsg;                          /* Message from message queue   */
    ULONG flCreate;                     /* Window creation control flags*/
    TID tid;
    ULONG height;
    ULONG width;

    DosExitList( EXLST_ADD, (PFNEXITLIST)CleanUp );
    if( argc >= 3 ) {
        InStream = *argv[1] - ADJUST_HFILE;
        OutStream = *argv[2] - ADJUST_HFILE;
    }
    AbortIf( ( Hab = WinInitialize( 0 )) == 0L );
    AbortIf( ( Hmq = WinCreateMsgQueue( Hab, 0 ) ) == 0L );

    AbortIf( !WinRegisterClass( Hab, (PSZ)"MyWindow", (PFNWP)MyWindowProc,
                                CS_SIZEREDRAW, 0 ) );
    flCreate = FCF_TITLEBAR | FCF_MENU | FCF_SIZEBORDER
             | FCF_ACCELTABLE | FCF_SHELLPOSITION | FCF_TASKLIST;
    height = WinQuerySysValue( HWND_DESKTOP, SV_CYMENU )
           + 2*WinQuerySysValue( HWND_DESKTOP, SV_CYBORDER )
           + 2*WinQuerySysValue( HWND_DESKTOP, SV_CYSIZEBORDER )
           + WinQuerySysValue( HWND_DESKTOP, SV_CYTITLEBAR );
    AbortIf( ( hwndFrame = WinCreateStdWindow( HWND_DESKTOP, 0L,
               &flCreate, "MyWindow", "", 0L,
               0, ID_WINDOW, &hwndClient ) ) == 0L );
    WinSetWindowText( hwndFrame, TRP_The_WATCOM_Debugger );

    width = WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
    AbortIf( !WinSetWindowPos( hwndFrame, HWND_TOP, 0,
                   WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN ) - height,
                   width / 3,
                   height, SWP_MOVE | SWP_SHOW | SWP_SIZE | SWP_ACTIVATE ) );
    AbortIf( DosCreateThread( (PFNTHREAD)ServiceRequests, &tid, stack+STACK_SIZE ) );
    while( WinGetMsg( Hab, &qmsg, 0L, 0, 0 ) ) {
        WinDispatchMsg( Hab, &qmsg );
    }
    WinDestroyWindow(hwndFrame);
    WinDestroyMsgQueue( Hmq );
    WinTerminate( Hab );
    return( 1 );
}
