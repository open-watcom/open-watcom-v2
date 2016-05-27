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

#include "wdebug.h"
#include "segmem.h"
#include "mythelp.h"
#include "di386cli.h"
#include "intdata.h"


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

/*
 * globals
 */
extern volatile WORD        __near WaitForFirst;
extern volatile WORD        __near IsSecondOK;
extern HWND                 __near MainWindowHandle;
extern HWND                 __near OutputWindow;
extern HANDLE               InstanceHandle,PrevInstance;
extern samp_save __far      * __near SampSave;
extern WORD                 SleepTime;
extern DWORD                TotalTime;
extern HANDLE               SampledProg;
extern interrupt_struct     __near IntData;
extern seg_offset           CommonAddr;
extern HANDLE               WaitForInt3;
extern HANDLE               WaitForInt1;

/* somewhere in the common part of the sampler */
extern int                  sample_main( char __far * );
extern int                  MessageLoop( void );

extern void                 Usage( void );
extern void FAR_PTR         *alloc( int );

/* getidata.c */
int GetIData( HINSTANCE inst, void __near *data, unsigned int size );

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
extern bool GetFileName( HINSTANCE, int, char * );

/* fault.c */
extern WORD __cdecl FAR FaultHandler( fault_frame ff );

#endif
