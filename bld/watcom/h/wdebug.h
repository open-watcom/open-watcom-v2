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
extern short CheckWin386Debug( void );                  /* 00 */
extern void GetDescriptor( short, void far * );         /* 02 */
/* CopyMemory - needs assembler helper */               /* 03 */
extern short GetDebugRegister( short, _dword far * );   /* 04 */
extern short SetDebugRegister( short, _dword far * );   /* 05 */
extern _word InitSampler( void far *, _word, _word );   /* 06 */
extern QuitSampler( _dword far * );                     /* 07 */
extern _word StartSampler( void );                      /* 08 */
extern void StopSampler( _dword far * );                /* 09 */
extern void GetCurrTick( _dword far * );                /* 0a */
extern void SetTimerTick( _word );                      /* 0b */
extern _word GetTimerTick( void );                      /* 0c */
extern _word GetSampleCount( void );                    /* 0d */
extern void GetSample0Tick( _dword far * );             /* 0e */
extern int RegisterName( char far * );                  /* 0f */
extern int AccessName( char far *, _dword far *);       /* 10 */
extern int UnregisterName( char far * );                /* 11 */
extern int UnaccessName( char far * );                  /* 12 */
extern int StartConv( _dword );                         /* 13 */
extern int LookForConv( _dword far * );                 /* 14 */
extern int EndConv( _dword );                           /* 15 */
extern _dword ConvGet( _dword, void far *, _word, _word  ); /* 16 */
extern _word ConvPut( _dword, const void far *, _word, _word  ); /* 17 */
extern _word IsConvAck( _dword );                       /* 18 */
extern void MyID( _dword far * );                       /* 19 */
extern _word SetExecutionFocus( _dword );               /* 1a */
extern int WhatHappened( void );                        /* 1b */
extern _word ConvGetTimeOut( _dword, void far *, _word, _word  ); /* 1c */
extern _word ConvPutTimeOut( _dword, void far *, _word, _word  ); /* 1d */
extern short EMUInit( void );                           /* 1e */
extern short EMUShutdown( void );                       /* 1f */
extern short EMURegister( _word, _dword );              /* 20 */
extern short EMUUnRegister( _word );                    /* 21 */
extern short FPUPresent( void );                        /* 22 */
extern short EMUSaveRestore( _word, void far *, _word );/* 23 */
extern void PauseSampler( void );                       /* 24 */
extern void UnPauseSampler( void );                     /* 25 */
extern void EGAWrite( unsigned, char, char );           /* 26 */
extern char VGARead( unsigned, char );                  /* 27 */
extern void DisableVideo( unsigned );                   /* 28 */
extern short RegisterInterruptCallback( void far *,
                void far *, void far * );               /* 29 */
extern void UnRegisterInterruptCallback( void far * );  /* 2a */
extern short GetInterruptCallback( icb_data far * );    /* 2b */
extern short RestartFromInterrupt( void );              /* 2c */
extern short Is32BitSel( short );                       /* 2d */
extern long GetVMId( void );                            /* 2e */
extern short HookIDT( void far * );                     /* 2f */
extern short IDTFini( void );                           /* 30 */
extern short IDTInit( unsigned );                       /* 31 */
extern short ConvPutPending( void );                    /* 32 */
extern short UseHotKey( int );                          /* 33 */
extern short RaiseInterruptInVM( _dword, _word );       /* 34 */

#pragma aux DS = \
        0x8C 0xD8       /* mov  ax,ds */ \
        value[ax];

#pragma aux CS = \
        0x8C 0xC8       /* mov  ax,cs */ \
        value[ax];

#pragma aux TimeSlice = \
        0xB8 0x80 0x16  /* mov ax,01680 */ \
        0xcd 0x2f       /* int 2f */ \
        modify [ax];

#pragma aux CheckWin386Debug = \
        0xB8 0x00 0xfa  /* mov ax,0fa00 */ \
        0xcd 0x2f       /* int 2f */ \
        value[ax];

#pragma aux GetDescriptor = \
        0xb8 0x02 0xfa  /* mov ax,0fa02 */ \
        0xcd 0x2f       /* int 2f */ \
        parm [cx] [es bx];

#pragma aux GetDebugRegister = \
        0xB8 0x04 0xfa          /* mov ax,0fa04 */ \
        0xcd 0x2f               /* int 2f */ \
        0x26 0x66 0x89 0x1D     /* mov  dword ptr es:[di], ebx */ \
        parm [cx] [es di] value[ax] modify[bx];

#pragma aux SetDebugRegister = \
        0xB8 0x05 0xfa          /* mov ax,0fa05 */ \
        0x26 0x66 0x8B 0x1D     /* mov  ebx, dword ptr es:[di] */ \
        0xcd 0x2f               /* int 2f */ \
        parm [cx] [es di] value[ax] modify[bx];


#pragma aux InitSampler = \
        0xb8 0x06 0xfa  /* mov ax,0fa06 */ \
        0xcd 0x2f       /* int 2f */ \
        parm [es bx] [cx] [dx] value[ax];

#pragma aux QuitSampler = \
        0xb8 0x07 0xfa          /* mov ax,0fa07 */ \
        0xcd 0x2f               /* int 2f */ \
        0x26 0x66 0x89 0x07     /* mov dword ptr es:[bx],eax */ \
        parm [es bx] modify[ax];

#pragma aux StartSampler = \
        0xb8 0x08 0xfa  /* mov ax,0fa08 */ \
        0xcd 0x2f       /* int 2f */ \
        value [ax];

#pragma aux StopSampler =  \
        0xb8 0x09 0xfa          /* mov ax,0fa09 */ \
        0xcd 0x2f               /* int 2f */ \
        0x26 0x66 0x89 0x07     /* mov dword ptr es:[bx],eax */ \
        parm [es bx] modify[ax];

#pragma aux GetCurrTick =  \
        0xb8 0x0a 0xfa          /* mov ax, 0fa0a */ \
        0xcd 0x2f               /* int 2f */ \
        0x26 0x66 0x89 0x07     /* mov dword ptr es:[bx],eax */ \
        parm [es bx] modify[ax];

#pragma aux SetTimerTick =  \
        0xb8 0x0b 0xfa  /* mov ax, 0fa0b */ \
        0xcd 0x2f       /* int 2f */ \
        parm [bx] modify[ax];

#pragma aux GetTimerTick =  \
        0xb8 0x0c 0xfa  /* mov ax, 0fa0c */ \
        0xcd 0x2f       /* int 2f */ \
        value [ax];

#pragma aux GetSampleCount =  \
        0xb8 0x0d 0xfa  /* mov ax, 0fa0d */ \
        0xcd 0x2f       /* int 2f */ \
        value [ax];

#pragma aux GetSample0Tick =  \
        0xb8 0x0e 0xfa          /* mov ax, 0fa0e */ \
        0xcd 0x2f               /* int 2f */ \
        0x26 0x66 0x89 0x07     /* mov dword ptr es:[bx],eax */ \
        parm [es bx] modify[ax];

#pragma aux RegisterName = \
        0xb8 0x0f 0xfa  /* mov ax, 0fa0f */ \
        0xcd 0x2f       /* int 2f */ \
        parm [es bx] value [ax];

#pragma aux AccessName = \
        0xb8 0x10 0xfa  /* mov ax, 0fa10 */ \
        0xcd 0x2f       /* int 2f */ \
        parm [es bx] [di si] value[ax];

#pragma aux UnregisterName = \
        0xb8 0x11 0xfa  /* mov ax, 0fa11 */ \
        0xcd 0x2f       /* int 2f */ \
        parm [es bx] value[ax];

#pragma aux UnaccessName = \
        0xb8 0x12 0xfa  /* mov ax, 0fa12 */ \
        0xcd 0x2f       /* int 2f */ \
        parm [es bx] value[ax];

#pragma aux StartConv = \
        0xb8 0x13 0xfa  /* mov ax, 0fa13 */ \
        0xcd 0x2f       /* int 2f */ \
        parm [cx bx] value[ax];

#pragma aux LookForConv = \
        0xb8 0x14 0xfa  /* mov ax, 0fa14 */ \
        0xcd 0x2f       /* int 2f */ \
        parm[es bx] value[ax];

#pragma aux EndConv = \
        0xb8 0x15 0xfa  /* mov ax, 0fa15 */ \
        0xcd 0x2f       /* int 2f */ \
        parm[cx bx] value[ax];

#pragma aux ConvGet = \
        0xb8 0x16 0xfa  /* mov ax, 0fa16 */ \
        0xcd 0x2f       /* int 2f */ \
        parm[cx bx] [es dx] [si] [di] value[dx ax];

#pragma aux ConvPut = \
        0xb8 0x17 0xfa  /* mov ax, 0fa17 */ \
        0xcd 0x2f       /* int 2f */ \
        parm[cx bx] [es dx] [si] [di] value[ax];

#pragma aux IsConvAck = \
        0xb8 0x18 0xfa  /* mov ax, 0fa18 */ \
        0xcd 0x2f       /* int 2f */ \
        parm [cx bx] value[ax];

#pragma aux MyID =  \
        0xb8 0x19 0xfa          /* mov ax, 0fa19 */ \
        0xcd 0x2f               /* int 2f */ \
        0x26 0x66 0x89 0x07     /* mov dword ptr es:[bx],eax */ \
        parm [es bx] modify[ax];

#pragma aux SetExecutionFocus = \
        0xb8 0x1a 0xfa  /* mov ax, 0fa1a */ \
        0xcd 0x2f       /* int 2f */ \
        parm [cx bx] value[ax];

#pragma aux WhatHappened = \
        0xb8 0x1b 0xfa  /* mov ax, 0fa1b */ \
        0xcd 0x2f       /* int 2f */ \
        value[ax];

#pragma aux ConvGetTimeOut = \
        0xb8 0x1c 0xfa  /* mov ax, 0fa1c */ \
        0xcd 0x2f       /* int 2f */ \
        parm[cx bx] [es dx] [si] [di] value[ax];

#pragma aux ConvPutTimeOut = \
        0xb8 0x1d 0xfa  /* mov ax, 0fa1d */ \
        0xcd 0x2f       /* int 2f */ \
        parm[cx bx] [es dx] [si] [di] value[ax];

#pragma aux EMUInit = \
        0xb8 0x1e 0xfa  /* mov ax, 0fa1e */ \
        0xcd 0x2f       /* int 2f */ \
        value[ax];

#pragma aux EMUShutdown = \
        0xb8 0x1f 0xfa  /* mov ax, 0fa1f */ \
        0xcd 0x2f       /* int 2f */ \
        value[ax];

#pragma aux EMURegister = \
        0xb8 0x20 0xfa  /* mov ax, 0fa20 */ \
        0xcd 0x2f       /* int 2f */ \
        parm[dx] [cx bx] value[ax];

#pragma aux EMUUnRegister = \
        0xb8 0x21 0xfa  /* mov ax, 0fa21 */ \
        0xcd 0x2f       /* int 2f */ \
        parm [dx] value[ax];

#pragma aux FPUPresent = \
        0xb8 0x22 0xfa  /* mov ax, 0fa22 */ \
        0xcd 0x2f       /* int 2f */ \
        value[ax];

#pragma aux EMUSaveRestore = \
        0xb8 0x23 0xfa  /* mov ax, 0fa23 */ \
        0xcd 0x2f       /* int 2f */ \
        parm [dx] [cx bx] [si] value[ax];

#pragma aux PauseSampler = \
        0xb8 0x24 0xfa  /* mov ax, 0fa24 */ \
        0xcd 0x2f       /* int 2f */;

#pragma aux UnPauseSampler = \
        0xb8 0x25 0xfa  /* mov ax, 0fa25 */ \
        0xcd 0x2f       /* int 2f */;

#pragma aux EGAWrite = \
        0xb8 0x26 0xfa  /* mov ax, 0fa26 */ \
        0xcd 0x2f       /* int 2f */ \
        parm [dx] [bl] [bh];

#pragma aux VGARead = \
        0xb8 0x27 0xfa  /* mov ax, 0fa27 */ \
        0xcd 0x2f       /* int 2f */ \
        parm [dx] [bl] value[al];

#pragma aux DisableVideo = \
        0xb8 0x28 0xfa  /* mov ax, 0fa28 */ \
        0xcd 0x2f       /* int 2f */ \
        parm [dx];

#pragma aux RegisterInterruptCallback = \
        0xb8 0x29 0xfa  /* mov ax, 0fa29 */ \
        0xcd 0x2f       /* int 2f */ \
        parm [cx bx] [es dx] [di si] value[ax];

#pragma aux UnRegisterInterruptCallback = \
        0xb8 0x2a 0xfa  /* mov ax, 0fa2a */ \
        0xcd 0x2f       /* int 2f */ \
        parm [cx bx];

#pragma aux GetInterruptCallback = \
        0xb8 0x2b 0xfa  /* mov ax, 0fa2b */ \
        0xcd 0x2f       /* int 2f */ \
        parm [cx bx];

#pragma aux RestartFromInterrupt = \
        0xb8 0x2c 0xfa  /* mov ax, 0fa2c */ \
        0xcd 0x2f       /* int 2f */

#pragma aux Is32BitSel = \
        0xb8 0x2d 0xfa  /* mov ax, 0fa2d */ \
        0xcd 0x2f       /* int 2f */ \
        parm [bx];

#pragma aux GetVMId = \
        0xb8 0x2e 0xfa  /* mov ax, 0fa2e */ \
        0xcd 0x2f       /* int 2f */ \
        value [dx ax];

#pragma aux HookIDT = \
        "mov    ax,0fa2fh" \
        "int    02fh" \
        parm [cx bx];

#pragma aux IDTFini = \
        "mov    ax,0fa30h" \
        "int    02fh";

#pragma aux IDTInit = \
        "mov    ax,0fa31h" \
        "int    02fh" \
        parm [bx];

#pragma aux ConvPutPending = \
        "mov    ax,0fa32h" \
        "int    02fh" \
        value[ax];

#pragma aux UseHotKey = \
        "mov    ax,0fa33h" \
        "int    02fh" \
        parm[bx];

#pragma aux RaiseInterruptInVM = \
        "mov    ax,0fa34h" \
        "int    02fh" \
        parm[cx bx] [dx];
