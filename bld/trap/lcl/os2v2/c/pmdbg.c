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


#define INCL_GPIPRIMITIVES              /* Selectively include          */
#define INCL_WINFRAMEMGR                /* relevant parts of            */
#define INCL_WINSYS                     /* the PM header file           */
#define INCL_DOSPROCESS                 /* the PM header file           */

#include <os2.h>                        /* PM header file               */
#include <string.h>                     /* C/2 string functions         */
#include "pmdbg.h"                     /* Resource symbolic identifiers*/
extern InitIt( char *trp, HAB, HWND );
extern int LoadIt();
extern int RunIt();
extern KillIt();
extern FiniIt();
extern char TellHardMode( char );
extern BOOL APIENTRY WinLockInput( HWND, USHORT );
enum {
    RUNNING,
    LOADED,
    BROKE,
    TERMINATED,
    NONE,
} State = NONE;

int HardMode = 0;
int InHardMode = 0;

#define HARD_POS 17
char *IsHardMode[] = {
    "                              ",
    "*** Hard Mode ***             "
};

char *WhatItIs[] = {
    "Task Running                                            ",
    "Task Loaded                                             ",
    "Hit Break Point                                         ",
    "Task Completed                                          ",
    "No Task                                                 ",
};

#define STRINGLENGTH 20                 /* Length of string             */

/************************************************************************/
/* Function prototypes                                                  */
/************************************************************************/
MRESULT EXPENTRY MyWindowProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 );
void main( void );

                                        /* Define parameters by type    */
HAB  Hab;                               /* PM anchor block handle       */

/**********************  Start of main procedure  ***********************/
void main(  )
{
  HMQ  hmq;                             /* Message queue handle         */
  HWND hwndClient;                      /* Client area window handle    */
  HWND hwndFrame;                       /* Frame window handle          */
  QMSG qmsg;                            /* Message from message queue   */
  ULONG flCreate;                       /* Window creation control flags*/

  Hab = WinInitialize( NULL );          /* Initialize PM                */
  hmq = WinCreateMsgQueue( Hab, 0 );    /* Create a message queue       */

  WinRegisterClass(                     /* Register window class        */
     Hab,                               /* Anchor block handle          */
     "MyWindow",                        /* Window class name            */
     MyWindowProc,                      /* Address of window procedure  */
     CS_SIZEREDRAW,                     /* Class style                  */
     0                                  /* No extra window words        */
     );

  flCreate = FCF_STANDARD &             /* Set frame control flags to   */
             ~FCF_SHELLPOSITION;        /* standard except for shell    */
                                        /* positioning.                 */

   hwndFrame = WinCreateStdWindow(
               HWND_DESKTOP,            /* Desktop window is parent     */
               0L,                      /* No frame styles              */
               &flCreate,               /* Frame control flag           */
               "MyWindow",              /* Client window class name     */
               "",                      /* No window text               */
               0L,                      /* No special class style       */
               NULL,                    /* Resource is in .EXE file     */
               ID_WINDOW,               /* Frame window identifier      */
               &hwndClient              /* Client window handle         */
               );

  WinSetWindowPos( hwndFrame,           /* Shows and activates frame    */
                   HWND_TOP,            /* window at position 100, 100, */
                   100, 100, 400, 200,  /* and size 200, 200.           */
                   SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW
                 );


/************************************************************************/
/* Get and dispatch messages from the application message queue         */
/* until WinGetMsg returns FALSE, indicating a WM_QUIT message.         */
/************************************************************************/
  while( WinGetMsg( Hab, &qmsg, NULL, 0, 0 ) ) {
    WinDispatchMsg( Hab, &qmsg );
  }

  WinDestroyWindow( hwndFrame );        /* Tidy up...                   */
  WinDestroyMsgQueue( hmq );            /* and                          */
  WinTerminate( Hab );                  /* terminate the application    */
}
/***********************  End of main procedure  ************************/

void ReDraw( HWND hwnd )
{
  WinInvalidateRegion( hwnd, NULL, FALSE );
}

/*********************  Start of window procedure  **********************/
MRESULT EXPENTRY MyWindowProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
  USHORT command;                       /* WM_COMMAND command value     */
  HPS    hps;                           /* Presentation Space handle    */
  RECTL  rc;                            /* Rectangle coordinates        */
  POINTL pt;                            /* String screen coordinates    */

  switch( msg )
  {
    case WM_CREATE:
      if( State == RUNNING ) break;
      /******************************************************************/
      /* Window initialization is performed here in WM_CREATE processing*/
      /* WinLoadString loads strings from the resource file.            */
      /******************************************************************/
      InitIt( "DSTD32", Hab, hwnd );
      ReDraw( hwnd );
      break;

    case WM_COMMAND:
      /******************************************************************/
      /* When the user chooses option 1, 2, or 3 from the Options pull- */
      /* down, the text string is set to 1, 2, or 3, and                */
      /* WinInvalidateRegion sends a WM_PAINT message.                  */
      /* When Exit is chosen, the application posts itself a WM_CLOSE   */
      /* message.                                                       */
      /******************************************************************/
      command = SHORT1FROMMP(mp1);      /* Extract the command value    */
      switch (command)
      {
        case ID_LOAD:
          if( State == RUNNING ) break;
          if( State != NONE ) {
              KillIt();
              State = NONE;
          }
          if( LoadIt() ) {
              State = LOADED;
          }
          ReDraw( hwnd );
          break;
        case ID_RUN:
          if( State == RUNNING ) break;
          State = RUNNING;
          ReDraw( hwnd );
          if( InHardMode ) {
              WinLockInput( 0, FALSE );
              InHardMode = FALSE;
          }
          State = RunIt() ? BROKE : TERMINATED;
          if( HardMode ) {
              WinLockInput( 0, TRUE );
              InHardMode = TRUE;
          }
          ReDraw( hwnd );
          break;
        case ID_KILL:
          if( State == RUNNING ) break;
          if( InHardMode ) {
              WinLockInput( 0, FALSE );
              InHardMode = FALSE;
          }
          if( State != NONE ) {
              KillIt();
              State = NONE;
          }
          ReDraw( hwnd );
          break;
        case ID_HARD:
          if( State == RUNNING ) break;
          HardMode = !HardMode;
          TellHardMode( HardMode ? (char)-1 : 0 );
          ReDraw( hwnd );
          break;
        case ID_EXITPROG:
          if( InHardMode ) {
              WinLockInput( 0, FALSE );
              InHardMode = FALSE;
          }
          if( State == RUNNING ) break;
          WinPostMsg( hwnd, WM_CLOSE, 0L, 0L );
          break;
        default:
          return WinDefWindowProc( hwnd, msg, mp1, mp2 );
      }
      break;
    case WM_ERASEBACKGROUND:
      /******************************************************************/
      /* Return TRUE to request PM to paint the window background       */
      /* in SYSCLR_WINDOW.                                              */
      /******************************************************************/
      return (MRESULT)( TRUE );
    case WM_PAINT:
      /******************************************************************/
      /* Window contents are drawn here in WM_PAINT processing.         */
      /******************************************************************/
                                        /* Create a presentation space  */
      hps = WinBeginPaint( hwnd, NULL, &rc );
      pt.x = 50; pt.y = 50;             /* Set the text coordinates,    */
      GpiSetColor( hps, CLR_NEUTRAL );         /* colour of the text,   */
      GpiSetBackColor( hps, CLR_BACKGROUND );  /* its background and    */
      GpiSetBackMix( hps, BM_OVERPAINT );      /* how it mixes,         */
                                               /* and draw the string...*/
      strcpy( WhatItIs[ State ] +HARD_POS, IsHardMode[ HardMode ] );
      GpiCharStringAt( hps, &pt, (LONG)strlen( WhatItIs[ State ] ), WhatItIs[ State ] );
      WinEndPaint( hps );                      /* Drawing is complete   */
      break;
    case WM_CLOSE:
      /******************************************************************/
      /* This is the place to put your termination routines             */
      /******************************************************************/
      if( State == RUNNING ) break;
      FiniIt();
      WinPostMsg( hwnd, WM_QUIT, 0L, 0L );  /* Cause termination        */
      break;
    default:
      /******************************************************************/
      /* Everything else comes here.  This call MUST exist              */
      /* in your window procedure.                                      */
      /******************************************************************/

      return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  return FALSE;
}
/**********************  End of window procedure  ***********************/
void __STK(){}
#pragma aux __STK "*";
