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
* Description:  Watcom wdebug VxD for Win16 interface.
*
****************************************************************************/


#define ERR_SERVER_EXISTS               -1
#define ERR_NO_MORE_CONVS               -2
#define ERR_NO_SUCH_SERVER              -3
#define ERR_ALREADY_ACCESSED_SERVER     -4
#define ERR_HAVE_NOT_ACCESSED_SERVER    -5
#define ERR_NOT_THAT_SERVER             -6
#define ERR_NO_SUCH_CONV                -7
#define ERR_CONV_NOT_STARTED            -8
#define ERR_NOT_A_SERVER                -9
#define ERR_NOT_CONNECTED               -10
#define ERR_NO_SUCH_ID                  -11
#define ERR_COULD_NOT_BLOCK             -12
#define ERR_COULD_NOT_RESUME            -13
#define ERR_TIMED_OUT                   -14
#define ERR_HAS_CLIENTS                 -15

#define WGOD_ASYNCH_STOP_INT            257

#define BLOCK                           1
#define NO_BLOCK                        0
#define WGOD_VERSION                    0x666

#define EMU_FSAVE                       1
#define EMU_FRSTOR                      0

typedef unsigned short _word;
typedef unsigned long _dword;

typedef struct {
_word ICCodeSeg;
_word ICCodeOff;
_word ICDataSeg;
_word ICDataOff;
_word ICStackSeg;
_word ICStackOff;
} icb_data;

typedef struct interrupt_struct {
_dword  EAX;
_dword  EBX;
_dword  ECX;
_dword  EDX;
_dword  EDI;
_dword  ESI;
_dword  EFlags;
_dword  EBP;
_dword  EIP;
_dword  ESP;
_word   SS;
_word   CS;
_word   DS;
_word   ES;
_word   FS;
_word   GS;
_word   InterruptNumber;
} interrupt_struct;

extern _word DS( void );
extern _word CS( void );
extern void TimeSlice( void );
extern short CheckWin386Debug( void );                      /* 00 */
extern short CopyMemory386( _word, _dword, _word, _dword, _word ); /* 01 */
extern void GetDescriptor( short, void __far * );           /* 02 */
extern _dword GetLimit( short );                            /* 03 */
extern short GetDebugRegister( short, _dword __far * );     /* 04 */
extern short SetDebugRegister( short, _dword __far * );     /* 05 */
extern _word InitSampler( void __far *, _word, _word );     /* 06 */
extern void QuitSampler( _dword __far * );                  /* 07 */
extern _word StartSampler( void );                          /* 08 */
extern void StopSampler( _dword __far * );                  /* 09 */
extern void GetCurrTick( _dword __far * );                  /* 0a */
extern void SetTimerTick( _word );                          /* 0b */
extern _word GetTimerTick( void );                          /* 0c */
extern _word GetSampleCount( void );                        /* 0d */
extern void GetSample0Tick( _dword __far * );               /* 0e */
extern int RegisterName( char __far * );                    /* 0f */
extern int AccessName( char __far *, _dword __far *);       /* 10 */
extern int UnregisterName( char __far * );                  /* 11 */
extern int UnaccessName( char __far * );                    /* 12 */
extern int StartConv( _dword );                             /* 13 */
extern int LookForConv( _dword __far * );                   /* 14 */
extern int EndConv( _dword );                               /* 15 */
extern _dword ConvGet( _dword, void __far *, _word, _word  ); /* 16 */
extern _word ConvPut( _dword, const void __far *, _word, _word  ); /* 17 */
extern _word IsConvAck( _dword );                           /* 18 */
extern void MyID( _dword __far * );                         /* 19 */
extern _word SetExecutionFocus( _dword );                   /* 1a */
extern int WhatHappened( void );                            /* 1b */
extern _word ConvGetTimeOut( _dword, void __far *, _word, _word  ); /* 1c */
extern _word ConvPutTimeOut( _dword, void __far *, _word, _word  ); /* 1d */
extern short EMUInit( void );                               /* 1e */
extern short EMUShutdown( void );                           /* 1f */
extern short EMURegister( _word, _dword );                  /* 20 */
extern short EMUUnRegister( _word );                        /* 21 */
extern short FPUPresent( void );                            /* 22 */
extern short EMUSaveRestore( _word, void __far *, _word );  /* 23 */
extern void PauseSampler( void );                           /* 24 */
extern void UnPauseSampler( void );                         /* 25 */
extern void EGAWrite( unsigned, char, char );               /* 26 */
extern char VGARead( unsigned, char );                      /* 27 */
extern void DisableVideo( unsigned );                       /* 28 */
extern short RegisterInterruptCallback( void __far *,
                void __far *, void __far * );               /* 29 */
extern void UnRegisterInterruptCallback( void __far * );    /* 2a */
extern short GetInterruptCallback( icb_data __far * );      /* 2b */
extern short RestartFromInterrupt( void );                  /* 2c */
extern short Is32BitSel( short );                           /* 2d */
extern long GetVMId( void );                                /* 2e */
extern short HookIDT( void __far * );                       /* 2f */
extern short IDTFini( void );                               /* 30 */
extern short IDTInit( unsigned );                           /* 31 */
extern short ConvPutPending( void );                        /* 32 */
extern short UseHotKey( int );                              /* 33 */
extern short RaiseInterruptInVM( _dword, _word );           /* 34 */

#pragma aux DS = \
        "mov  ax,ds" \
        value[ax];

#pragma aux CS = \
        "mov  ax,cs" \
        value[ax];

#pragma aux TimeSlice = \
        "mov ax,01680h" \
        "int 2fh" \
        modify [ax];

#pragma aux CheckWin386Debug = \
        "mov ax,0fa00h" \
        "int 2fh" \
        value[ax];

#pragma aux CopyMemory386 = \
        ".386" \
        "shl edx,16" \
        "mov dx,di" \
        "xchg bx,si" \
        "shl ebx,16" \
        "mov bx,si" \
        "mov si,ax" \
        "pop di" \
        "mov ax,0fa01h" \
        "int 2fh" \
        parm [cx] [dx di] [ax] [si bx] value[ax];

#pragma aux GetDescriptor = \
        "mov ax,0fa02h" \
        "int 2fh" \
        parm [cx] [es bx];

#pragma aux GetLimit = \
        "mov ax,0fa03h" \
        "int 2fh" \
        parm [bx] value [dx ax];

#pragma aux GetDebugRegister = \
        ".386" \
        "mov ax,0fa04h" \
        "int 2fh" \
        "mov  dword ptr es:[di], ebx" \
        parm [cx] [es di] value[ax] modify[bx];

#pragma aux SetDebugRegister = \
        ".386" \
        "mov ax,0fa05h" \
        "mov  ebx, dword ptr es:[di]" \
        "int 2fh" \
        parm [cx] [es di] value[ax] modify[bx];


#pragma aux InitSampler = \
        "mov ax,0fa06h" \
        "int 2fh" \
        parm [es bx] [cx] [dx] value[ax];

#pragma aux QuitSampler = \
        ".386" \
        "mov ax,0fa07h" \
        "int 2fh" \
        "mov dword ptr es:[bx],eax" \
        parm [es bx] modify[ax];

#pragma aux StartSampler = \
        "mov ax,0fa08h" \
        "int 2fh" \
        value [ax];

#pragma aux StopSampler =  \
        ".386" \
        "mov ax,0fa09h" \
        "int 2fh" \
        "mov dword ptr es:[bx],eax" \
        parm [es bx] modify[ax];

#pragma aux GetCurrTick =  \
        ".386" \
        "mov ax,0fa0ah" \
        "int 2fh" \
        "mov dword ptr es:[bx],eax" \
        parm [es bx] modify[ax];

#pragma aux SetTimerTick =  \
        "mov ax,0fa0bh" \
        "int 2fh" \
        parm [bx] modify[ax];

#pragma aux GetTimerTick =  \
        "mov ax,0fa0ch" \
        "int 2fh" \
        value [ax];

#pragma aux GetSampleCount =  \
        "mov ax,0fa0dh" \
        "int 2fh" \
        value [ax];

#pragma aux GetSample0Tick =  \
        ".386" \
        "mov ax,0fa0dh" \
        "int 2fh" \
        "mov dword ptr es:[bx],eax" \
        parm [es bx] modify[ax];

#pragma aux RegisterName = \
        "mov ax,0fa0fh" \
        "int 2fh" \
        parm [es bx] value [ax];

#pragma aux AccessName = \
        "mov ax,0fa10h" \
        "int 2fh" \
        parm [es bx] [di si] value[ax];

#pragma aux UnregisterName = \
        "mov ax,0fa11h" \
        "int 2fh" \
        parm [es bx] value[ax];

#pragma aux UnaccessName = \
        "mov ax,0fa12h" \
        "int 2fh" \
        parm [es bx] value[ax];

#pragma aux StartConv = \
        "mov ax,0fa13h" \
        "int 2fh" \
        parm [cx bx] value[ax];

#pragma aux LookForConv = \
        "mov ax,0fa14h" \
        "int 2fh" \
        parm[es bx] value[ax];

#pragma aux EndConv = \
        "mov ax,0fa15h" \
        "int 2fh" \
        parm[cx bx] value[ax];

#pragma aux ConvGet = \
        "mov ax,0fa16h" \
        "int 2fh" \
        parm[cx bx] [es dx] [si] [di] value[dx ax];

#pragma aux ConvPut = \
        "mov ax,0fa17h" \
        "int 2fh" \
        parm[cx bx] [es dx] [si] [di] value[ax];

#pragma aux IsConvAck = \
        "mov ax,0fa18h" \
        "int 2fh" \
        parm [cx bx] value[ax];

#pragma aux MyID =  \
        ".386" \
        "mov ax,0fa19h" \
        "int 2fh" \
        "mov dword ptr es:[bx],eax" \
        parm [es bx] modify[ax];

#pragma aux SetExecutionFocus = \
        "mov ax,0fa1ah" \
        "int 2fh" \
        parm [cx bx] value[ax];

#pragma aux WhatHappened = \
        "mov ax,0fa1bh" \
        "int 2fh" \
        value[ax];

#pragma aux ConvGetTimeOut = \
        "mov ax,0fa1ch" \
        "int 2fh" \
        parm[cx bx] [es dx] [si] [di] value[ax];

#pragma aux ConvPutTimeOut = \
        "mov ax,0fa1dh" \
        "int 2fh" \
        parm[cx bx] [es dx] [si] [di] value[ax];

#pragma aux EMUInit = \
        "mov ax,0fa1eh" \
        "int 2fh" \
        value[ax];

#pragma aux EMUShutdown = \
        "mov ax,0fa1fh" \
        "int 2fh" \
        value[ax];

#pragma aux EMURegister = \
        "mov ax,0fa20h" \
        "int 2fh" \
        parm[dx] [cx bx] value[ax];

#pragma aux EMUUnRegister = \
        "mov ax,0fa21h" \
        "int 2fh" \
        parm [dx] value[ax];

#pragma aux FPUPresent = \
        "mov ax,0fa22h" \
        "int 2fh" \
        value[ax];

#pragma aux EMUSaveRestore = \
        "mov ax,0fa23h" \
        "int 2fh" \
        parm [dx] [cx bx] [si] value[ax];

#pragma aux PauseSampler = \
        "mov ax,0fa24h" \
        "int 2fh" \
        modify[ax];

#pragma aux UnPauseSampler = \
        "mov ax,0fa25h" \
        "int 2fh" \
        modify[ax];

#pragma aux EGAWrite = \
        "mov ax,0fa26h" \
        "int 2fh" \
        parm [dx] [bl] [bh] modify[ax];

#pragma aux VGARead = \
        "mov ax,0fa27h" \
        "int 2fh" \
        parm [dx] [bl] value[al];

#pragma aux DisableVideo = \
        "mov ax,0fa28h" \
        "int 2fh" \
        parm [dx] modify[ax];

#pragma aux RegisterInterruptCallback = \
        "mov ax,0fa29h" \
        "int 2fh" \
        parm [cx bx] [es dx] [di si] value[ax];

#pragma aux UnRegisterInterruptCallback = \
        "mov ax,0fa2ah" \
        "int 2fh" \
        parm [cx bx] modify[ax];

#pragma aux GetInterruptCallback = \
        "mov ax,0fa2bh" \
        "int 2fh" \
        parm [cx bx] modify[ax];

#pragma aux RestartFromInterrupt = \
        "mov ax,0fa2ch" \
        "int 2fh" \
        modify[ax]

#pragma aux Is32BitSel = \
        "mov ax,0fa2dh" \
        "int 2fh" \
        parm [bx] modify[ax];

#pragma aux GetVMId = \
        "mov ax,0fa2eh" \
        "int 2fh" \
        value [dx ax];

#pragma aux HookIDT = \
        "mov    ax,0fa2fh" \
        "int 2fh" \
        parm [cx bx] modify[ax];

#pragma aux IDTFini = \
        "mov    ax,0fa30h" \
        "int 2fh" \
        modify[ax]

#pragma aux IDTInit = \
        "mov    ax,0fa31h" \
        "int 2fh" \
        parm [bx] modify[ax];

#pragma aux ConvPutPending = \
        "mov    ax,0fa32h" \
        "int 2fh" \
        value[ax];

#pragma aux UseHotKey = \
        "mov    ax,0fa33h" \
        "int    02fh" \
        parm[bx] modify[ax];

#pragma aux RaiseInterruptInVM = \
        "mov    ax,0fa34h" \
        "int    02fh" \
        parm[cx bx] [dx] modify[ax];
