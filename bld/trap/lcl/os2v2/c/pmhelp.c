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
* Description:  PM Helper for the character mode debuggers. It is used when
*               the debugger runs in a FS session and the debuggee is
*               a PM program.
*
****************************************************************************/


#define INCL_BASE
#define INCL_WIN
#define INCL_GPI
#define INCL_GPIPRIMITIVES              /* Selectively include   */
#define INCL_WINFRAMEMGR                /* relevant parts of     */
#define INCL_WINSYS                     /* the PM headers        */
#define INCL_DOSPROCESS
#include <os2.h>
#include <string.h>
#include <stdio.h>
#include "wdpmhelp.h"
#include "trperr.h"

/* "Secret" PM APIs useful to a debugger */
extern BOOL APIENTRY WinLockInput(HMQ, BOOL);
extern BOOL APIENTRY WinThreadAssocQueue(HAB, HMQ);

#define WDPMHLP_WNDCLASS "WDPMHelper"

static HAB             Hab;
static HMQ             Hmq;
static HFILE           InStream;
static HFILE           OutStream;
static HWND            hwndClient;
static HWND            hwndFrame;
static BOOL            Locked = FALSE;

static PID             PidDebugee;  // These two seem unused?
static TID             TidDebugee;

#if 0
    static HWND            FocusWnd;
    static HWND            ActiveWnd;
#endif


#ifdef DEBUG
    char Message[256] = { "All is well" };
    static void Say(char *str) {
        if (str != NULL)
            strcpy(Message, str);
        WinInvalidateRegion(hwndClient, 0L, FALSE);
    }
#else
    #define Say( x )
#endif


static VOID AbortLocker(HWND hwndFrame, HWND hwndClient)
{
    PERRINFO     pErrInfoBlk;
    PSZ          pszOffSet;
    PSZ          pszErrMsg;

    DosBeep(100, 10);
    if ((pErrInfoBlk = WinGetErrorInfo(Hab)) != (PERRINFO)NULL) {
        pszOffSet = ((PSZ)pErrInfoBlk) + pErrInfoBlk->offaoffszMsg;
        pszErrMsg = ((PSZ)pErrInfoBlk) + *((PSHORT)pszOffSet);
        if ((INT)hwndFrame && (INT)hwndClient) {
            WinMessageBox(HWND_DESKTOP,              /* Parent window is desktop  */
                          hwndFrame,                 /* Owner window is our frame */
                          (PSZ)pszErrMsg,            /* PMWIN Error message       */
                          TRP_The_WATCOM_Debugger,   /* Title bar message         */
                          MSGBOXID,                  /* Message identifier        */
                          MB_MOVEABLE | MB_CUACRITICAL | MB_CANCEL); /* Flags */
        }
        WinFreeErrorInfo(pErrInfoBlk);
    }
    WinPostMsg(hwndClient, WM_QUIT, (MPARAM)NULL, (MPARAM)NULL);
}


#define AbortIf(x) if (x) AbortLocker(hwndFrame, hwndClient)


/* This bit is very, very tricky. If we lock the PM and the user switches  */
/* to PM, there's a good chance he/she will be stranded with no way back!  */
static void UnLockIt( void )
{
    if (Locked) {
        WinThreadAssocQueue(Hab, Hmq);
        WinLockInput(0, 0);
        WinThreadAssocQueue(Hab, NULLHANDLE);
        Locked = FALSE;
    }
}

static VOID APIENTRY CleanUp( void )
{
    UnLockIt();
    DosExitList(EXLST_EXIT, (PFNEXITLIST)CleanUp);
}

static void LockIt( void )
{
    if (!Locked) {
        WinThreadAssocQueue(Hab, Hmq);
        WinLockInput(0, 1);
        WinThreadAssocQueue(Hab, NULLHANDLE);
        Locked = TRUE;
    }
}

static void SwitchBack( void )
{
    ULONG       written;
    static      pmhelp_packet data;

    data.command = PMHELP_SWITCHBACK;
    DosWrite(OutStream, &data, sizeof(data), &written);
}


static VOID APIENTRY ServiceRequests(VOID)
{
    ULONG               len;
    pmhelp_packet       data;

#ifdef DEBUG
    /* We don't need a message queue to post messages */
    HAB                 habThread;
    HMQ                 hmqThread;

    habThread = WinInitialize(NULL);
    hmqThread = WinCreateMsgQueue(habThread, 0);
#endif

    for ( ;; ) {
        if (DosRead(InStream, &data, sizeof(data), &len) != 0)
            break;

        if (len != sizeof(data))
            break;

        switch (data.command) {
            case PMHELP_LOCK:
                PidDebugee = data.pid;
                TidDebugee = data.tid;
                WinPostMsg(hwndClient, WM_COMMAND, MPFROM2SHORT(ID_LOCK, 0), 0);
                break;

            case PMHELP_UNLOCK:
                PidDebugee = data.pid;
                TidDebugee = data.tid;
                WinPostMsg(hwndClient, WM_COMMAND, MPFROM2SHORT(ID_UNLOCK, 0), 0);
                break;

            case PMHELP_EXIT:
                WinPostMsg(hwndClient, WM_QUIT, 0, 0); /* Cause termination*/
                break;

            default:
                Say("Received Unknown Command");
        }
    }
    Say("Pipe Read Failed");
#ifdef DEBUG
    WinDestroyMsgQueue(hmqThread);
    WinTerminate(habThread);
#endif
}

static MRESULT EXPENTRY MyWindowProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
    HPS    hps;
    RECTL  rc;

    switch (msg) {

    case WM_CREATE:
        break;

    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1)) {
           case ID_UNLOCK:
               UnLockIt();
               Say("Unlocked");
#if 0
               if (FocusWnd != NULL) {
                   WinSetFocus(HWND_DESKTOP, FocusWnd);
               }
               WinSetActiveWindow(HWND_DESKTOP, hwndClient);
               if (ActiveWnd != NULL) {
                   WinSetActiveWindow(HWND_DESKTOP, ActiveWnd);
               }
#endif
               break;

           case ID_LOCK:
               Say("Locked");
               LockIt();
               break;

           case ID_SWITCH:
               Say("Switched");
               SwitchBack();
               break;

           case ID_EXITPROG:
               WinPostMsg(hwnd, WM_CLOSE, (MPARAM)0, (MPARAM)0);
               break;

           default:
               return WinDefWindowProc(hwnd, msg, mp1, mp2);
        }
        break;

    case WM_ERASEBACKGROUND:
        return (MRESULT)TRUE;

    case WM_PAINT:
        hps = WinBeginPaint(hwnd, 0L, &rc);
#ifdef DEBUG
        {
            POINTL pt;

            pt.x = 2; pt.y = 2;
            GpiSetColor(hps, CLR_NEUTRAL);
            GpiSetBackColor(hps, CLR_BACKGROUND);
            GpiSetBackMix(hps, BM_OVERPAINT);
            GpiCharStringAt(hps, &pt, (LONG)strlen(Message), Message);
        }
#endif
        WinEndPaint(hps);
        break;

    case WM_CLOSE:
        WinPostMsg(hwnd, WM_QUIT, 0, 0);
        break;

    case WM_DESTROY:
        UnLockIt();  // Is it possible to arrive here at all if PM is locked?
        // fall thru

    default:
        return WinDefWindowProc(hwnd, msg, mp1, mp2);

    }
    return FALSE;
}


#define STACK_SIZE 16384

INT main( int argc, char **argv )
{
    QMSG    qmsg;                       /* Message from message queue   */
    ULONG   flCreate;                   /* Window creation control flags*/
    TID     tid;
    ULONG   height;
    ULONG   width;

    DosExitList(EXLST_ADD, (PFNEXITLIST)CleanUp);
    if (argc >= 3) {
        InStream  = *argv[1] - ADJUST_HFILE;
        OutStream = *argv[2] - ADJUST_HFILE;
    }
    AbortIf((Hab = WinInitialize(NULLHANDLE)) == 0L);
    AbortIf((Hmq = WinCreateMsgQueue(Hab, 0)) == 0L);

    AbortIf(!WinRegisterClass(Hab, (PSZ)WDPMHLP_WNDCLASS, (PFNWP)MyWindowProc,
                              CS_SIZEREDRAW, 0));
    flCreate = FCF_TITLEBAR | FCF_MENU | FCF_SIZEBORDER
             | FCF_ACCELTABLE | FCF_SHELLPOSITION | FCF_TASKLIST;

    height = WinQuerySysValue(HWND_DESKTOP, SV_CYMENU)
           + 2*WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER)
           + 2*WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER)
#ifdef DEBUG
           + 2*WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
#else
           + WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
#endif

    AbortIf((hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 0L,
             &flCreate, WDPMHLP_WNDCLASS, "", 0L,
             NULLHANDLE, ID_WINDOW, &hwndClient)) == 0L);

    WinSetWindowText(hwndFrame, TRP_The_WATCOM_Debugger);

    width = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
    AbortIf(!WinSetWindowPos(hwndFrame, HWND_TOP, 0,
                   WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN) - height,
                   width / 3,
                   height, SWP_MOVE | SWP_SHOW | SWP_SIZE | SWP_ACTIVATE));

    /* Spawn the thread waiting for commands from the debugger */
    AbortIf(DosCreateThread(&tid, (PFNTHREAD)ServiceRequests, 0, CREATE_READY, STACK_SIZE));

    /* Message loop */
    while (WinGetMsg(Hab, &qmsg, 0L, 0, 0)) {
        WinDispatchMsg(Hab, &qmsg);
    }
    WinDestroyWindow(hwndFrame);
    WinDestroyMsgQueue(Hmq);
    WinTerminate(Hab);
    return 1;
}
