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


#include "dbgdefn.h"
#include "dbgwind.h"
#include "dbgmem.h"
#include <string.h>
#define INCL_SUB
#define INCL_PM
#define INCL_BASE
#define INCL_WINSYS
#define INCL_WINHOOKS
#define INCL_WINMESSAGEMGR
#include <os2.h>

extern BOOL APIENTRY WinThreadAssocQueue(HAB, HMQ);

extern HMQ      GUIPMmq;

extern void     *ExtraAlloc(unsigned);
extern void     ExtraFree(void *);
extern void     StartupErr(char *);
extern int      GUIInitMouse(int);
extern void     GUIFiniMouse(void);
extern void     TellHandles(HAB hab, HWND hwnd);
extern HAB      GUIGetHAB();
extern HWND     GUIGetSysHandle(gui_window*);
extern void     SaveMainScreen(char*);
extern void     RestoreMainScreen(char*);
extern bool     IsTrapFilePumpingMessageQueue();


unsigned                NumLines;
unsigned                NumColumns;
int                     ForceHardMode;
bool                    ToldWinHandle = FALSE;
extern a_window         *WndMain;
//TODO: see if these two event sems could be replaced by single mutex
HEV                     PumpMessageSem = NULL;
HEV                     PumpMessageDoneSem = NULL;
#define STACK_SIZE      32768


void WndInitWndMain(wnd_create_struct *info)
{
    WndInitCreateStruct(info);
    info->style |= GUI_INIT_INVISIBLE;
}

void TellWinHandle()
{
    if (!ToldWinHandle) {
        TellHandles(GUIGetHAB(), GUIGetSysHandle(WndGui(WndMain)));
        ToldWinHandle = TRUE;
    }
}

ToggleHardMode()
{
}

void RingBell()
{
    DosBeep(1000, 250);
}


/*
 * ConfigScreen -- figure out screen configuration we're going to use.
 */

unsigned ConfigScreen()
{
    return 0;
}


unsigned GetSystemDir(char *buff, int max)
/***********************************************/
{
    // inst
    PRFPROFILE                  prof;
    int                         i;
    char                        c;
    int                         len;

    prof.cchUserName = 0L;
    prof.cchSysName = 0L;

    if (PrfQueryProfile(GUIGetHAB(), &prof)) {
        if (prof.cchSysName > 0) {
            _AllocA(prof.pszSysName, prof.cchSysName);
            _AllocA(prof.pszUserName, prof.cchUserName);
            PrfQueryProfile(GUIGetHAB(), &prof);
        }

        strcpy(buff, prof.pszSysName);
        if (prof.cchSysName <= max) {
            len = prof.cchSysName;
        } else {
            len = max;
        }

        for (i = len - 1; i >= 0; i--) {
            c = buff[i];
            buff[i] = '\0';
            if (c == '\\') {
                break;
            }
        }
    } else {
        buff[0] = '\0';
    }
    return(strlen(buff));
} /* _wpi_getinidirectory */

/*
 * InitScreen
 */

VOID PumpMessageQueue( VOID )
{
    char        class_name[80];
    QMSG        qmsg;
    ERRORID     err;
    ULONG       ulCount;

    for( ;; ) {
        DosWaitEventSem(PumpMessageSem, SEM_INDEFINITE_WAIT);
        DosResetEventSem(PumpMessageSem, &ulCount);
        WinThreadAssocQueue(GUIGetHAB(), GUIPMmq);
        while (WinGetMsg(GUIGetHAB(), &qmsg, 0L, 0, 0)) {
            WinQueryClassName(qmsg.hwnd, sizeof(class_name), class_name);
            if (strcmp(class_name, "GUIClass") == 0 ||
                strcmp(class_name, "WTool") == 0) {
                WinDefWindowProc(qmsg.hwnd, qmsg.msg, qmsg.mp1, qmsg.mp2);
            } else {
                WinDispatchMsg(GUIGetHAB(), &qmsg);
            }
        }
        WinThreadAssocQueue(GUIGetHAB(), NULL);
        err = WinGetLastError(GUIGetHAB());
        DosPostEventSem(PumpMessageDoneSem);
    }
}

void InitScreen()
{
    TID                 tid;
    ULONG               ulCount;

    RestoreMainScreen("WDPM");
    DosResetEventSem(PumpMessageSem, &ulCount);
    DosResetEventSem(PumpMessageDoneSem, &ulCount);
    DosCreateThread(&tid, (PFNTHREAD)PumpMessageQueue, NULL, 0, STACK_SIZE);
    DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, tid);
}


/*
 * UsrScrnMode -- setup the user screen mode
 */

bool UsrScrnMode()
{
    return FALSE;
}


void DbgScrnMode()
{
}


/*
 * DebugScreen -- swap/page to debugger screen
 */

static HWND FocusWnd, ActiveWnd;

bool DebugScreen()
{
    if (!WndMain)
        return FALSE;
    if (FocusWnd && WinIsWindow(GUIGetHAB(), FocusWnd) &&
        FocusWnd != WinQueryFocus(HWND_DESKTOP)) {
        WinSetFocus(HWND_DESKTOP, FocusWnd);
    }
    if (ActiveWnd && WinIsWindow(GUIGetHAB(), ActiveWnd) &&
        ActiveWnd != WinQueryActiveWindow(HWND_DESKTOP)) {
        WinSetActiveWindow(HWND_DESKTOP, ActiveWnd);
    }
    return FALSE;
}


bool DebugScreenRecover()
{
    return TRUE;
}


/*
 * UserScreen -- swap/page to user screen
 */

bool UserScreen()
{
    if (!WndMain)
        return FALSE;
    FocusWnd = WinQueryFocus(HWND_DESKTOP);
    ActiveWnd = WinQueryActiveWindow(HWND_DESKTOP);
    return FALSE;
}

void SaveMainWindowPos()
{
    SaveMainScreen("WDPM");
}

void FiniScreen()
{
}


/*****************************************************************************\
 *                                                                           *
 *            Replacement routines for User Interface library                *
 *                                                                           *
\*****************************************************************************/

void *uifaralloc(unsigned size)
{
    return(ExtraAlloc(size));
}


void uifarfree(void *ptr)
{
    ExtraFree(ptr);
}

bool SysGUI()
{
    return TRUE;
}
void PopErrBox(char *buff)
{
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, buff,
                  LIT(Debugger_Startup_Error), 1001,
                  MB_MOVEABLE | MB_CUACRITICAL | MB_CANCEL);
}

unsigned OnAnotherThread(unsigned (*rtn)(), unsigned in_len, void *in, unsigned out_len, void *out)
{
    unsigned    result;
    ULONG       ulCount;

    if (!ToldWinHandle || IsTrapFilePumpingMessageQueue()) {
        return rtn(in_len, in, out_len, out);
    } else {
        if (PumpMessageDoneSem == NULL)
            DosCreateEventSem(NULL, &PumpMessageDoneSem, 0, FALSE);

        if (PumpMessageSem == NULL)
            DosCreateEventSem(NULL, &PumpMessageSem, 0, FALSE);

        DosPostEventSem(PumpMessageSem);
        result = rtn(in_len, in, out_len, out);
        WinPostMsg(GUIGetSysHandle(WndGui(WndMain)), WM_QUIT, 0, 0);
        DosWaitEventSem(PumpMessageDoneSem, SEM_INDEFINITE_WAIT);
        DosResetEventSem(PumpMessageDoneSem, &ulCount);
        return result;
    }
}
