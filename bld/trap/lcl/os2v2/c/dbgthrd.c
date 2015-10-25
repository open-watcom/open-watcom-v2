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
* Description:  Debugger thread issuing DosDebug calls.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <env.h>
#define INCL_PM
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_WINSYS
#define INCL_WINHOOKS
#define INCL_WINMESSAGEMGR
#include <os2.h>
#include <os2dbg.h>
#include "dosdebug.h"
#include "softmode.h"
#include "trperr.h"
#include "dbgthrd.h"

static uDB_t            *DebugReqBuff;
static uDB_t            StopBuff;
static unsigned int     DebugReqResult;
static HEV              DebugReqSem = NULLHANDLE;
static HEV              DebugDoneSem = NULLHANDLE;
static HEV              StopDoneSem = NULLHANDLE;
static BOOL             InDosDebug;

#define STACK_SIZE      32768
#define MAX_PAINTS      100
#define MAX_CLASS_NAME  80

static void StopApplication(void)
{
    StopBuff.Cmd = DBG_C_Stop;
    DosDebug(&StopBuff);
    DosPostEventSem(StopDoneSem);
    DosExit(EXIT_THREAD, 0);
}

static void CantDoIt(void)
{
     WinMessageBox(HWND_DESKTOP, HwndDebugger,
        TRP_WIN_no_can_do,
        TRP_The_WATCOM_Debugger, 1, MB_ERROR | MB_SYSTEMMODAL | MB_OK);
}


ULONG CallDosDebug(uDB_t *buff)
{
    QMSG        qmsg;
    int         num_paints;
    int         i;
    struct {
        RECTL   rcl;
        HWND    hwnd;
    }           paints[MAX_PAINTS];
    HPS         ps;
    char        class_name[MAX_CLASS_NAME];
    TID         tid;
    ULONG       ulCount;

    if (!IsPMDebugger()) {
        return DosDebug(buff);
    }

    switch (buff->Cmd) {
        case DBG_C_ClearWatch:
        case DBG_C_Freeze:
        case DBG_C_LinToSel:
        case DBG_C_NumToAddr:
        case DBG_C_ReadCoRegs:
        case DBG_C_ReadMemBuf:
        case DBG_C_ReadMem_D:
        case DBG_C_ReadReg:
        case DBG_C_SelToLin:
        case DBG_C_SetWatch:
        case DBG_C_ThrdStat:
        case DBG_C_WriteCoRegs:
        case DBG_C_WriteMemBuf:
        case DBG_C_WriteMem_D:
        case DBG_C_WriteReg:
            return DosDebug(buff);
    }
    switch (buff->Cmd) {
        case DBG_C_Go:
        case DBG_C_SStep:
        case DBG_C_Term:
            ReleaseQueue(buff->Pid, buff->Tid);
    }
    DebugReqBuff = buff;
    StopBuff = *buff;
    DosResetEventSem(DebugDoneSem, &ulCount);
    DosPostEventSem(DebugReqSem);
    num_paints = 0;
    if (IsPMDebugger()) {
        while (WinGetMsg(HabDebugger, &qmsg, 0L, 0, 0) || InDosDebug) {
            WinQueryClassName(qmsg.hwnd, MAX_CLASS_NAME, class_name);
            switch (qmsg.msg) {
            case WM_CHAR:
                if ((SHORT1FROMMP(qmsg.mp1) & KC_VIRTUALKEY) &&
                    (SHORT2FROMMP(qmsg.mp2) == VK_BREAK)) {
                    ULONG    ulCount;

                    SetBrkPending();
                    DosCreateThread(&tid, (PFNTHREAD)StopApplication, NULLHANDLE, 0, STACK_SIZE);
                    DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, 10, tid);
                    WakeThreads(StopBuff.Pid);
                    DosWaitEventSem(StopDoneSem, SEM_INDEFINITE_WAIT);
                    DosResetEventSem(StopDoneSem, &ulCount);
                }
                break;
            case WM_COMMAND:
                CantDoIt();
                break;
            default:
                if (strcmp(class_name, "GUIClass") == 0 ||
                    strcmp(class_name, "WTool") == 0) {
                    switch (qmsg.msg) {
                    case WM_PAINT:
                        if (num_paints >= MAX_PAINTS)
                            --num_paints;
                        paints[num_paints].hwnd = qmsg.hwnd;
                        ps = WinBeginPaint(qmsg.hwnd, 0, &paints[num_paints].rcl);
                        GpiErase(ps);
                        WinEndPaint(ps);
                        num_paints++;
                        break;
                    case WM_BUTTON1DOWN:
                    case WM_BUTTON2DOWN:
                    case WM_BUTTON3DOWN:
                        CantDoIt();
                        break;
                    case WM_MOUSEMOVE:
                    {
                        HPOINTER hourglass = WinQuerySysPointer( HWND_DESKTOP, SPTR_WAIT, FALSE );
                        if (WinQueryPointer(HWND_DESKTOP) != hourglass) {
                            WinSetPointer(HWND_DESKTOP, hourglass);
                        }
                        break;
                    }
                    default:
                        WinDefWindowProc(qmsg.hwnd, qmsg.msg, qmsg.mp1, qmsg.mp2);
                    }
                } else {
                    WinDispatchMsg(HabDebugger, &qmsg);
                }
            }
        }
    } else {
        DosWaitEventSem(DebugDoneSem, SEM_INDEFINITE_WAIT);
    }
    switch (buff->Cmd) {
        case DBG_N_Exception:
        case DBG_N_AsyncStop:
        case DBG_N_Watchpoint:
            AssumeQueue(buff->Pid, buff->Tid);
            break;
    }
    for (i = 0; i < num_paints; ++i) {
        WinInvalidateRect(paints[i].hwnd, &paints[i].rcl, FALSE);
    }
    return DebugReqResult;
}

static VOID APIENTRY DoDebugRequests(ULONG arg)
{
    ULONG   ulCount;

    for ( ; ; ) {
        DosWaitEventSem(DebugReqSem, SEM_INDEFINITE_WAIT);
        DosResetEventSem(DebugReqSem, &ulCount);
        InDosDebug = TRUE;
        DebugReqResult = DosDebug(DebugReqBuff);
        InDosDebug = FALSE;
        if (IsPMDebugger()) {
            WinPostMsg(HwndDebugger, WM_QUIT, 0, 0);
        } else {
            DosPostEventSem(DebugDoneSem);
        }
    }
}

VOID InitDebugThread( VOID )
{
    TID                 tid;
    ULONG               ulCount;

    if (StopDoneSem == NULLHANDLE)
        DosCreateEventSem(NULL, &StopDoneSem, 0, FALSE);

    if (DebugReqSem == NULLHANDLE)
        DosCreateEventSem(NULL, &DebugReqSem, 0, FALSE);

    if (DebugDoneSem == NULLHANDLE)
        DosCreateEventSem(NULL, &DebugDoneSem, 0, FALSE);

    DosResetEventSem(StopDoneSem, &ulCount);
    DosResetEventSem(DebugReqSem, &ulCount);
    DosResetEventSem(DebugDoneSem, &ulCount);
    DosCreateThread(&tid, DoDebugRequests, NULLHANDLE, 0, STACK_SIZE);
    DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, tid);
}
