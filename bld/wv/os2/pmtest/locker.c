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


#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <string.h>
#include "locker.h"

extern __pascal WinLockInput( HMQ, BOOL );

HAB  Hab;
HFILE InStream;

void UnLockIt()
{
    WinLockInput( 0, 0 );
}

void LockIt()
{
    WinLockInput( 0, 1 );
}

ServiceRequests( void *crap )
{
    crap=crap;
    for( ;; ) {
    }
}

MRESULT EXPENTRY MyWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    USHORT command;
    HPS    hps;                 /* Presentation Space handle    */
    RECTL  rc;                  /* Rectangle coordinates        */

    switch( msg ) {

    case WM_CREATE:
        break;

    case WM_COMMAND:
        command = SHORT1FROMMP(mp1);    /* Extract the command value    */
        switch (command) {
        case ID_LOCK:
            LockIt();
            break;
        case ID_UNLOCK:
        //        WinInvalidateRegion( hwnd, 0L, false );
            UnLockIt();
            break;
        case ID_EXITPROG:
            WinPostMsg( hwnd, WM_CLOSE, (MPARAM)0, (MPARAM)0 );
            break;
        default:
            return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
        }
        break;

    case WM_ERASEBACKGROUND:
        return( false );

    case WM_PAINT:
        hps = WinBeginPaint( hwnd, 0L, &rc );
        WinEndPaint( hps );                    /* Drawing is complete   */
        break;

    case WM_CLOSE:
        WinPostMsg( hwnd, WM_QUIT, (MPARAM)0,(MPARAM)0 );/* Cause termination*/
        break;

    default:
        return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );

    }
    return( false );
}

INT main( int argc, char **argv )
{
    HWND hwndClient = NULLHANDLE;               /* Client area window handle    */
    HWND hwndFrame = NULLHANDLE;                /* Frame window handle          */
    QMSG qmsg;                          /* Message from message queue   */
    ULONG flCreate;                     /* Window creation control flags*/
    HMQ  hmq;
    TID tid;

    InStream = 0;
    if ((Hab = WinInitialize(0)) == 0L) {
        AbortLocker(hwndFrame, hwndClient);
    }

    if ((hmq = WinCreateMsgQueue( Hab, 0 )) == 0L) {
        AbortLocker(hwndFrame, hwndClient); /* Terminate the application        */
    }

    if (!WinRegisterClass(              /* Register window class        */
        Hab,                            /* Anchor block handle          */
        (PSZ)"MyWindow",                        /* Window class name            */
        (PFNWP)MyWindowProc,            /* Address of window procedure  */
        CS_SIZEREDRAW,                  /* Class style                  */
        0                                       /* No extra window words        */
        )) {
        AbortLocker(hwndFrame, hwndClient); /* Terminate the application        */
    }

    flCreate = FCF_TITLEBAR | FCF_SYSMENU | FCF_MENU | FCF_MINMAX | FCF_ICON
          /*| FCF_SIZEBORDER | FCF_ACCELTABLE | FCF_TASKLIST*/;
    if ((hwndFrame = WinCreateStdWindow(
               HWND_DESKTOP,            /* Desktop window is parent     */
               0,
               &flCreate,               /* Frame control flag           */
               "MyWindow",              /* Client window class name     */
               "",                      /* No window text               */
               0,                       /* No special class style       */
               (HMODULE)0L,           /* Resource is in .EXE file     */
               ID_WINDOW,               /* Frame window identifier      */
               &hwndClient              /* Client window handle         */
               )) == 0L) {
        AbortLocker(hwndFrame, hwndClient); /* Terminate the application        */
    }

    WinSetWindowText(hwndFrame, "WATCOM Debugger PM Helper");

    if (!WinSetWindowPos( hwndFrame,    /* Shows and activates frame    */
                   HWND_TOP,            /* window at position 100, 100, */
                   0, 0, 0, 0,
                   SWP_SHOW | SWP_MAXIMIZE
                 )) {
        AbortLocker(hwndFrame, hwndClient); /* Terminate the application        */
    }

    DosCreateThread( &tid, ServiceRequests, 0, 0, 10*1024 );
    while( WinGetMsg( Hab, &qmsg, 0L, 0, 0 ) ) {
        WinDispatchMsg( Hab, &qmsg );
    }
    UnLockIt();
    WinDestroyWindow(hwndFrame);
    WinDestroyMsgQueue( hmq );
    WinTerminate( Hab );
    return( 1 );
}

VOID AbortLocker(HWND hwndFrame, HWND hwndClient)
{
   PERRINFO  pErrInfoBlk;
   PSZ       pszOffSet;
   PSZ  pszErrMsg;

   DosBeep(100,10);
   if( (pErrInfoBlk = WinGetErrorInfo(Hab)) != (PERRINFO)NULL ) {
      pszOffSet = ((PSZ)pErrInfoBlk) + pErrInfoBlk->offaoffszMsg;
      pszErrMsg = ((PSZ)pErrInfoBlk) + *((PSHORT)pszOffSet);
      if((INT)hwndFrame && (INT)hwndClient) {
         WinMessageBox(HWND_DESKTOP,         /* Parent window is desk top */
                       hwndFrame,            /* Owner window is our frame */
                       (PSZ)pszErrMsg,       /* PMWIN Error message       */
                       "Error Message",      /* Title bar message         */
                       MSGBOXID,             /* Message identifier        */
                       MB_MOVEABLE | MB_CUACRITICAL | MB_CANCEL ); /* Flags */
      }
      WinFreeErrorInfo(pErrInfoBlk);
   }
   WinPostMsg(hwndClient, WM_QUIT, (MPARAM)NULL, (MPARAM)NULL);
}
