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


#ifndef __WINDOWS_INCLUDED__
#define __WINDOWS_INCLUDED__
#include <windows.h>
#include <toolhelp.h>
#endif
#include "wdebug.h"
#include "segmem.h"
#include "mythelp.h"
#include "di386.h"

#define MAX_SAMPLES     10000
#define TIMER_ID        666


typedef struct {
    WORD        wEnvSeg;
    LPSTR       lpCmdLine;
    LPVOID      lpCmdShow;
    DWORD       dwReserved;
} parm_data;

typedef struct {
    WORD        always2;
    WORD        nCmdShow;
} command_data;

typedef struct {
    DWORD       offset;
    WORD        seg;
} samp_save;

typedef struct {
    bool                TaskStarted;
    bool                TaskEnded;
    long                SampOffset;
    int                 SampleHandle;
    WORD                SampleIndex;
    WORD                SampleCount;
    WORD                LastSampleIndex;
    DWORD               CurrTick;
    WORD                FarWriteProblem;
    info_struct         Info;
    bool                LostData;
    char                SampName[256];
    bool                ShopClosed;
} shared_data;

/*
 * globals
 */
extern volatile WORD            near WaitForFirst;
extern volatile WORD            near IsSecondOK;
extern HWND                     near MainWindowHandle;
extern HWND                     near OutputWindow;
extern HANDLE                   InstanceHandle,PrevInstance;
extern samp_save                far * near SampSave;
extern shared_data              far * near SharedMemory;
extern WORD                     SleepTime;
extern DWORD                    TotalTime;
extern HANDLE                   SampledProg;
extern BOOL                     WDebug386;
extern interrupt_struct         near IntData;
extern seg_offset               CommonAddr;
extern HANDLE                   WaitForInt3;
extern HANDLE                   WaitForInt1;

/* somewhere in the common part of the sampler */
extern int sample_main( char far * );
extern void REPORT_TYPE report();
extern void             SaveSamples( void );
extern int              SampWrite( void FAR_PTR *, unsigned );
extern void             WriteCodeLoad( seg_offset, char *, samp_block_kinds );
extern void             WriteAddrMap( seg, seg, off );
extern void             WriteMark( char FAR_PTR *str, seg_offset where );
extern unsigned         GetNumber(unsigned,unsigned,char**,unsigned);
extern void             AllocSamples( unsigned );
extern void             SetTimerRate( char ** );
extern void             fatal(void);
extern int              MessageLoop( void );
extern void             RecordCGraph( void );

extern void Usage( void );
extern void FAR_PTR * alloc( int );

/* getidata.c */
int GetIData( HINSTANCE inst, void near *data, unsigned int size );

/* inth.asm */
void FAR PASCAL IntHandler( void );

/* libload.c */
void HandleLibLoad( int type, HANDLE hmod );

/* notify.c */
BOOL __export FAR PASCAL NotifyHandler( WORD id, DWORD data );
void HandleNotify( WORD wparam, DWORD lparam );

/* sampwin.c */
void FlushSamples( WORD );
void CloseShop( void );
DWORD WinGetCurrTick( void );

/* winmain.c */
void MyOutput(char*, ...);
char *ResName( char * );

/* winopts.c */
BOOL GetFileName( HANDLE, HANDLE, char * );
