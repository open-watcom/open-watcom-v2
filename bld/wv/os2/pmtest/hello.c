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


/*********************** Start of the hello.c ****************************
 * Adapted from a sample program provided by IBM Corporation
 * with additions below.
 *************************************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>                        /* PM header file               */
#include <string.h>                     /* ANSI string header file      */
#include "hello.h"                      /* Resource symbolic identifiers*/

#define STRINGLENGTH 20                 /* Length of string             */

extern __far16 __pascal WinLockInput( ULONG, USHORT );

/*
 * Function prototypes
 */
MRESULT EXPENTRY MyWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

                                        /* Define parameters by type    */
HMQ  hmq;                               /* Message queue handle         */
HAB  hab;                               /* PM anchor block handle       */
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

  if ((hab = WinInitialize(0)) == 0L) /* Initialize PM     */
     AbortHello(hwndFrame, hwndClient); /* Terminate the application    */

  if ((hmq = WinCreateMsgQueue( hab, 0 )) == 0L)/* Create a msg queue */
     AbortHello(hwndFrame, hwndClient); /* Terminate the application    */

  if (!WinRegisterClass(                /* Register window class        */
     hab,                               /* Anchor block handle          */
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

    WinSetWindowText(hwndFrame, "HELLO SAMPLE");

  if (!WinSetWindowPos( hwndFrame,      /* Shows and activates frame    */
                   HWND_TOP,            /* window at position 100, 100, */
                   0, 0,
                   WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN ),
                   WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN ),
                   SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW
                 ))
     AbortHello(hwndFrame, hwndClient); /* Terminate the application    */

/*
 * Get and dispatch messages from the application message queue
 * until WinGetMsg returns FALSE, indicating a WM_QUIT message.
 */

  while( WinGetMsg( hab, &qmsg, 0L, 0, 0 ) )
    WinDispatchMsg( hab, &qmsg );
  WinDestroyWindow(hwndFrame);           /* Tidy up...                   */
  WinDestroyMsgQueue( hmq );             /* Tidy up...                   */
  WinTerminate( hab );                   /* Terminate the application    */
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
  switch( msg )
  {
    case WM_CREATE:
      /*
       * Window initialization is performed here in WM_CREATE processing
       * WinLoadString loads strings from the resource file.
       */
      WinLoadString( hab, (HMODULE)0L, IDS_HELLO, STRINGLENGTH, szHello );
      WinLoadString( hab, (HMODULE)0L, IDS_1,     STRINGLENGTH, sz1     );
      WinLoadString( hab, (HMODULE)0L, IDS_2,     STRINGLENGTH, sz2     );
      WinLoadString( hab, (HMODULE)0L, IDS_3,     STRINGLENGTH, sz3     );
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
          LockIt();
          break;
        case ID_OPTION2:
          strcpy( szString, sz2 );
          WinInvalidateRegion( hwnd, 0L, FALSE );
          UnLockIt();
          break;
        case ID_OPTION3:
#pragma aux int3 = "int 3";
          int3();
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
      sprintf( buff, "hab = %8.8x, hmq = %8.8x, hwnd = %8.8x", hab, hmq, hwnd );
      hps = WinBeginPaint( hwnd, 0L, &rc );
      pt.x = 0; pt.y = 50;              /* Set the text coordinates,    */
      GpiSetColor( hps, CLR_NEUTRAL );         /* colour of the text,   */
      GpiSetBackColor( hps, CLR_BACKGROUND );  /* its background and    */
      GpiSetBackMix( hps, BM_OVERPAINT );      /* how it mixes,         */
                                               /* and draw the string...*/
      GpiCharStringAt( hps, &pt, (LONG)strlen( buff ), buff );
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
   if ((pErrInfoBlk = WinGetErrorInfo(hab)) != (PERRINFO)NULL)
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
void UnLockIt()
{
      WinLockInput( 0, 0 );
}
void LockIt()
{
      WinLockInput( 0, 1 );
}
