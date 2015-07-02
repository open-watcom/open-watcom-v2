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


#define NOAUTOPROCS
#define NOCOVERSENDS
#define NOCOVERRC
#define INCLUDE_COMMDLG_H
#define INCLUDE_DDEML_H
#define INCLUDE_MMSYSTEM_H
#define INCLUDE_TOOLHELP_H
#define INCLUDE_PENWIN_H
#include <stdarg.h>
#include <windows.h>

#define MAGICCOOKIE 0xdeadbeefL

#define PROC_16         16
#define PROC_32         32

/* NOAUTOPROCS, copy of prototypes from _win386.h */
typedef void            (__far *REALFARPROC)(void);
/* */

struct wndprocs {
    struct wndprocs     *next;
    LONG                procaddr;
    HWND                hwnd;
    unsigned short      proctype;       // PROC_16 or PROC_32
};

extern void         *UserProcs[];

extern void         *SetProc( FARPROC fp, int type );
extern BOOL         TryAlias( HWND hwnd, WORD msg, LONG *lparam );
extern void         *RealProcAddr( FARPROC fp );
extern DWORD _WCFAR *_DataSelectorSizeAddr;
extern CALLBACKPTR  vGetCallbackRoutine( PROCPTR fn, va_list vl );

/* NOAUTOPROCS, copy of prototypes from _win386.h */
extern FARPROC  PASCAL _Cover_MakeProcInstance( REALFARPROC proc, HINSTANCE inst );
extern BOOL     PASCAL _CoverChooseColor( LPCHOOSECOLOR pcc );
extern BOOL     PASCAL _CoverChooseFont( LPCHOOSEFONT pcf );
extern HWND     PASCAL _CoverFindText( LPFINDREPLACE pfr );
extern BOOL     PASCAL _CoverGetOpenFileName( LPOPENFILENAME pofn );
extern BOOL     PASCAL _CoverGetSaveFileName( LPOPENFILENAME pofn );
extern BOOL     PASCAL _CoverPrintDlg( LPPRINTDLG ppd );
extern HWND     PASCAL _CoverReplaceText( LPFINDREPLACE pfr );
extern HDDEDATA PASCAL _Cover_DdeClientTransaction( LPBYTE lpvdata, DWORD cbdata, HCONV hconv, HSZ hszitem, WORD ufmt, WORD utype, DWORD utimeout, LPDWORD lpuresult );
extern WORD     PASCAL _Cover_DdeInitialize( LPDWORD pidInst, PFNCALLBACK p, DWORD afCmd, DWORD ulRes);
extern BOOL     PASCAL _Cover_mciSetYieldProc(UINT uDeviceID, YIELDPROC fpYieldProc, DWORD dwYieldData);
extern LPMMIOPROC PASCAL _Cover_mmioInstallIOProc(FOURCC fccIOProc, LPMMIOPROC pIOProc, DWORD dwFlags);
extern BOOL     PASCAL _Cover_NotifyRegister(HTASK hTask, LPFNNOTIFYCALLBACK lpfn, WORD wFlags);
/* */

/* PENWIN prototypes */
extern void __InitPenWinRC( LPRC lprc );
/* NOAUTOPROCS, copy of prototypes from _win386.h */
extern BOOL PASCAL _Cover_CorrectWriting( HWND hwnd, LPSTR lpstr, UINT cb, LPRC lprc, DWORD dw1, DWORD dw2 );
extern BOOL PASCAL _Cover_DictionarySearch( LPRC lprc, LPSYE lpsye, int i1, LPSYV lpsyv, int i2 );
extern UINT PASCAL _Cover_EnumSymbols(LPSYG lpsyg, WORD maxstr, ENUMPROC lpfn, LPVOID data );
extern REC  PASCAL _Cover_ProcessWriting( HWND hwnd, LPRC lprc );
extern REC  PASCAL _Cover_Recognize( LPRC lprc );
extern REC  PASCAL _Cover_RecognizeData( LPRC lprc, HPENDATA hpd );
extern BOOL PASCAL _Cover_SetPenHook(HKP hkpop, LPFNRAWHOOK lpfn );
extern BOOL PASCAL _Cover_TrainInk(LPRC lprc, HPENDATA hpd, LPSYV lpsvy );
/* */
