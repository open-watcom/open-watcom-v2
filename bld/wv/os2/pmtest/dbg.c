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


extern DebugIt( void *crap );
extern Spinner( void *crap );
#define STACK_SIZE (60*1024)

/*********************** Start of the hello.c ****************************
 * Adapted from a sample program provided by IBM Corporation
 * with additions below.
 *************************************************************************/
#define INCL_WIN
#define INCL_GPI

#define INCL_WINWINDOWMGR
#define INCL_WINFRAMEMGR
#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSMEMMGR
#define INCL_DOSSIGNALS
#undef INCL_DOSINFOSEG
#undef INCL_DOSPROCESS
#define INCL_WINSWITCHLIST
#include <os2.h>                        /* PM header file               */
#include <string.h>                     /* ANSI string header file      */
#include "hello.h"                      /* Resource symbolic identifiers*/
#define DBSEM_USER_SPECIFIED
#include "dbgapi.h"
#include <stddef.h>
#include <process.h>
#include <stdio.h>
#include "bsexcpt.h"

#define STRINGLENGTH 20                 /* Length of string             */

extern __far16 __pascal WinLockInput( ULONG, USHORT );

/*
 * Function prototypes
 */
MRESULT EXPENTRY MyWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

                                        /* Define parameters by type    */
HAB  Hab;                               /* PM anchor block handle       */
CHAR szHello[STRINGLENGTH];             /* String parameters, set in    */
CHAR sz1[STRINGLENGTH];                 /* the processing of WM_CREATE, */
CHAR sz2[STRINGLENGTH];                 /* and used in the processing   */
CHAR sz3[STRINGLENGTH];                 /* of WM_COMMAND, in window     */
CHAR szString[STRINGLENGTH];            /* procedure.                   */
PSZ  pszErrMsg;

/**************************************************************************
 *
 *  Name       : main()
 *
 *  Description: Initializes the process for OS/2 PM services and
 *               process the application message queue until a
 *               WM_QUIT message is received.  It then destroys all
 *               OS/2 PM resources and terminates.
 *
 *  Concepts   : - obtains anchor block handle and creates message queue
 *               - creates the main frame window which creates the
 *                   main client window
 *               - polls the message queue via Get/Dispatch Msg loop
 *               - upon exiting the loop, exits
 *
 *  API's      :   WinInitialize
 *                 WinCreateMsgQueue
 *                 WinTerminate
 *                 WinSetWindowPos
 *                 WinSetWindowText
 *                 WinRegisterClass
 *                 WinCreateStdWindow
 *                 WinGetMsg
 *                 WinDispatchMsg
 *                 WinDestroyWindow
 *                 WinDestroyMsgQueue
 *
 *  Parameters :  [none]
 *
 *  Return     :  1 - if successful execution completed
 *                0 - if error
 *
 *************************************************************************/
INT main (VOID)
{
  HWND hwndClient = NULLHANDLE;         /* Client area window handle    */
  HWND hwndFrame = NULLHANDLE;          /* Frame window handle          */
  QMSG qmsg;                            /* Message from message queue   */
  ULONG flCreate;                       /* Window creation control flags*/
  int have;
  TID tid;
HMQ  Hmq;                               /* Message queue handle         */

  if ((Hab = WinInitialize(0)) == 0L) /* Initialize PM     */
     AbortHello(hwndFrame, hwndClient); /* Terminate the application    */

  if ((Hmq = WinCreateMsgQueue( Hab, 0 )) == 0L)/* Create a msg queue */
     AbortHello(hwndFrame, hwndClient); /* Terminate the application    */

  if (!WinRegisterClass(                /* Register window class        */
     Hab,                               /* Anchor block handle          */
     (PSZ)"MyWindow",                   /* Window class name            */
     (PFNWP)MyWindowProc,               /* Address of window procedure  */
     CS_SIZEREDRAW,                     /* Class style                  */
     0                                  /* No extra window words        */
     ))
     AbortHello(hwndFrame, hwndClient); /* Terminate the application    */

   flCreate = FCF_STANDARD &            /* Set frame control flags to   */
             ~FCF_SHELLPOSITION;        /* standard except for shell    */
                                        /* positioning.                 */

  if ((hwndFrame = WinCreateStdWindow(
               HWND_DESKTOP,            /* Desktop window is parent     */
               0,                       /* STD. window styles           */
               &flCreate,               /* Frame control flag           */
               "MyWindow",              /* Client window class name     */
               "",                      /* No window text               */
               0,                       /* No special class style       */
               (HMODULE)0L,           /* Resource is in .EXE file     */
               ID_WINDOW,               /* Frame window identifier      */
               &hwndClient              /* Client window handle         */
               )) == 0L)
     AbortHello(hwndFrame, hwndClient); /* Terminate the application    */

    WinSetWindowText(hwndFrame, "Debugger");

  if (!WinSetWindowPos( hwndFrame,      /* Shows and activates frame    */
                   HWND_TOP,            /* window at position 100, 100, */
                   100, 100, 200, 200,  /* and size 200, 200.           */
                   SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW
                 ))
     AbortHello(hwndFrame, hwndClient); /* Terminate the application    */

/*
 * Get and dispatch messages from the application message queue
 * until WinGetMsg returns FALSE, indicating a WM_QUIT message.
 */

  while( WinGetMsg( Hab, &qmsg, NULL, 0, 0 ) ) {
      WinDispatchMsg( Hab, &qmsg );
  }
  WinDestroyWindow(hwndFrame);           /* Tidy up...                   */
  WinDestroyMsgQueue( Hmq );             /* Tidy up...                   */
  WinTerminate( Hab );                   /* Terminate the application    */
  return( 1 );
} /* End of main */

/**************************************************************************
 *
 *  Name       : MyWindowProc
 *
 *  Description: The window procedure associated with the client area in
 *               the standard frame window. It processes all messages
 *               either sent or posted to the client area, depending on
 *               the message command and parameters.
 *
 *  Concepts   :
 *
 *  API's      :   WinLoadString
 *                 WinInvalidateRegion
 *                 WinPostMsg
 *                 WinDefWindowProc
 *                 WinBeginPaint
 *                 GpiSetColor
 *                 GpiSetBackColor
 *                 GpiSetBackMix
 *                 GpiCharStringAt
 *                 WinEndPaint
 *
 *  Parameters :  hwnd = window handle
 *                msg = message code
 *                mp1 = first message parameter
 *                mp2 = second message parameter
 *
 *  Return     :  depends on message sent
 *
 *************************************************************************/
MRESULT EXPENTRY MyWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  TID   tid;
  switch( msg )
  {
    case WM_CREATE:
      /*
       * Window initialization is performed here in WM_CREATE processing
       * WinLoadString loads strings from the resource file.
       */
      WinLoadString( Hab, (HMODULE)0L, IDS_HELLO, STRINGLENGTH, szHello );
      WinLoadString( Hab, (HMODULE)0L, IDS_1,     STRINGLENGTH, sz1     );
      WinLoadString( Hab, (HMODULE)0L, IDS_2,     STRINGLENGTH, sz2     );
      WinLoadString( Hab, (HMODULE)0L, IDS_3,     STRINGLENGTH, sz3     );
      strcpy( szString, szHello );      /* Copy text Hello into szString*/
      break;

    case WM_COMMAND:
      /*
       * When the user chooses option 1, 2, or 3 from the Options pull-
       * down, the text string is set to 1, 2, or 3, and
       * WinInvalidateRegion sends a WM_PAINT message.
       * When Exit is chosen, the application posts itself a WM_CLOSE
       * message.
       */
      {
      USHORT command;                   /* WM_COMMAND command value     */
      command = SHORT1FROMMP(mp1);      /* Extract the command value    */
      switch (command)
      {
        case ID_OPTION1:
          strcpy( szString, sz1 );
          WinInvalidateRegion( hwnd, 0L, FALSE );
          strcpy( szString, "Task Started?" );
          DosCreateThread( &tid, (PFNTHREAD)DebugIt, 0, 0, STACK_SIZE );
          break;
        case ID_OPTION2:
          strcpy( szString, sz2 );
          WinInvalidateRegion( hwnd, 0L, FALSE );
          break;
        case ID_OPTION3:
          strcpy( szString, sz3 );
          WinInvalidateRegion( hwnd, 0L, FALSE );
          break;
        case ID_EXITPROG:
          WinPostMsg( hwnd, WM_CLOSE, (MPARAM)0, (MPARAM)0 );
          break;
        default:
          return WinDefWindowProc( hwnd, msg, mp1, mp2 );
      }

      break;
      }
    case WM_ERASEBACKGROUND:
      /*
       * Return TRUE to request PM to paint the window background
       * in SYSCLR_WINDOW.
       */
      return (MRESULT)( TRUE );
    case WM_PAINT:
      /*
       * Window contents are drawn here in WM_PAINT processing.
       */
      {
      HPS    hps;                       /* Presentation Space handle    */
      RECTL  rc;                        /* Rectangle coordinates        */
      POINTL pt;                        /* String screen coordinates    */
      char   buff[256];
                                        /* Create a presentation space  */
      hps = WinBeginPaint( hwnd, 0L, &rc );
      pt.x = 0; pt.y = 50;              /* Set the text coordinates,    */
      GpiSetColor( hps, CLR_NEUTRAL );         /* colour of the text,   */
      GpiSetBackColor( hps, CLR_BACKGROUND );  /* its background and    */
      GpiSetBackMix( hps, BM_OVERPAINT );      /* how it mixes,         */
                                               /* and draw the string...*/
      GpiCharStringAt( hps, &pt, (LONG)strlen( szString ), szString );
      WinEndPaint( hps );                      /* Drawing is complete   */
      break;
      }
    case WM_CLOSE:
      /*
       * This is the place to put your termination routines
       */
      WinPostMsg( hwnd, WM_QUIT, (MPARAM)0,(MPARAM)0 );/* Cause termination*/
      break;
    default:
      /*
       * Everything else comes here.  This call MUST exist
       * in your window procedure.
       */

      return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  return (MRESULT)FALSE;
} /* End of MyWindowProc */

/**************************************************************************
 *
 *  Name       : AbortHello
 *
 *  Description: Report an error returned from an API service
 *
 *  Concepts   :  use of message box to display information
 *
 *  API's      :  DosBeep
 *                WinGetErrorInfo
 *                WinMessageBox
 *                WinFreeErrorInfo
 *                WinPostMsg
 *
 *  Parameters :  hwndFrame = frame window handle
 *                hwndClient = client window handle
 *
 *  Return     :  [none]
 *
 *************************************************************************/
VOID AbortHello(HWND hwndFrame, HWND hwndClient)
{
   PERRINFO  pErrInfoBlk;
   PSZ       pszOffSet;
   void      stdprint(void);

   DosBeep(100,10);
   if ((pErrInfoBlk = WinGetErrorInfo(Hab)) != (PERRINFO)NULL)
   {
      pszOffSet = ((PSZ)pErrInfoBlk) + pErrInfoBlk->offaoffszMsg;
      pszErrMsg = ((PSZ)pErrInfoBlk) + *((PSHORT)pszOffSet);
      if((INT)hwndFrame && (INT)hwndClient)
         WinMessageBox(HWND_DESKTOP,         /* Parent window is desk top */
                       hwndFrame,            /* Owner window is our frame */
                       (PSZ)pszErrMsg,       /* PMWIN Error message       */
                       "Error Msg",          /* Title bar message         */
                       MSGBOXID,             /* Message identifier        */
                       MB_MOVEABLE | MB_CUACRITICAL | MB_CANCEL ); /* Flags */
      WinFreeErrorInfo(pErrInfoBlk);
   }
   WinPostMsg(hwndClient, WM_QUIT, (MPARAM)NULL, (MPARAM)NULL);
} /* End of AbortHello */

/*********************** End of the hello.c *******************************/
/*********************** Start of my debugger stuff ***********************/


#define DBG_N_Breakpoint        -100
#define DBG_N_SStep             -101
#define DBG_N_Signal            -102

void DebugExecute( uDB_t *buff, ULONG cmd )
{
    EXCEPTIONREPORTRECORD       ex;
    ULONG                       value;
    ULONG                       stopvalue;
    ULONG                       notify=0;
    BOOL                        got_second_notification;
    ULONG                       fcp;
    CONTEXTRECORD               fcr;
ULONG           ExceptNum;

    buff->Cmd = cmd;
    value = buff->Value;
    if( cmd == DBG_C_Go ) {
        value = 0;
    }
    stopvalue = XCPT_CONTINUE_EXECUTION;
    got_second_notification = FALSE;
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

DebugIt( void *crap )
{
    STARTDATA           start;
    int                 code;
    SWCNTRL             SW;
    HSWITCH             hswitch;
    PPIB                pib;
    PTIB                tib;
    HWND                hwndme;
    QMSG                qmsg;           /* Message from message queue   */
    int                 i;
PID             Pid = 0;
ULONG          SID;
uDB_t           Buff;
TID     tid;
HMQ     the_q;

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
        return;
    }
    DebugExecute( &Buff, DBG_C_Go );
    if( Buff.Cmd != DBG_N_Breakpoint ) {
        return;
    }
#if 0
    Buff.Cmd = DBG_C_Stop;
    DosDebug( &Buff );
    Buff.Cmd = DBG_C_ReadReg;
    DosDebug( &Buff );
    Buff.EIP++;
    Buff.Cmd = DBG_C_WriteReg;
    DosDebug( &Buff );
#endif
    the_q = WinQueueFromID( Hab, Buff.Pid, Buff.Tid );
#if 0
    for( ;; ) {
        hmq = WinQuerySendMsg( Hab, 0, Q, &qmsg );
        if( hmq == 0 ) break;
        WinReplyMsg( Hab, hmq, Q, 1 );
    }
#endif
    WinThreadAssocQueue( Hab, the_q );
    while( 1 ) {
        if( !WinGetMsg( Hab, &qmsg, 0L, 0, 0 ) ) break;
    }
    WinThreadAssocQueue( Hab, 0 );
    for( ;; ) DosSleep( 100 );
}
