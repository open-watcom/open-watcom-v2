/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
#define ERR_NO_MORE_CONVERSATIONS       -2
#define ERR_NO_SUCH_SERVER              -3
#define ERR_ALREADY_ACCESSED_SERVER     -4
#define ERR_HAVE_NOT_ACCESSED_SERVER    -5
#define ERR_NOT_THAT_SERVER             -6
#define ERR_NO_SUCH_CONVERSATION        -7
#define ERR_CONVERSATION_NOT_STARTED    -8
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

extern _word DS( void );
#pragma aux DS = \
        "mov    ax,ds" \
    __value [__ax]

extern _word CS( void );
#pragma aux CS = \
        "mov    ax,cs" \
    __value [__ax]

/* 00 */ extern short   CheckWin386Debug( void );
/* 01 */ extern short   CopyMemory386( _word, _dword, _word, _dword, _word );
/* 02 */ extern void    GetDescriptor( short, void __far * );
/* 03 */ extern _dword  GetLimit( short );
/* 04 */ extern short   GetDebugRegister( short, _dword __far * );
/* 05 */ extern short   SetDebugRegister( short, _dword __far * );
/* 06 */ extern _word   InitSampler( void __far *, _word, _word );
/* 07 */ extern void    QuitSampler( _dword __far * );
/* 08 */ extern _word   StartSampler( void );
/* 09 */ extern void    StopSampler( _dword __far * );
/* 0a */ extern void    GetCurrTick( _dword __far * );
/* 0b */ extern void    SetTimerTick( _word );
/* 0c */ extern _word   GetTimerTick( void );
/* 0d */ extern _word   GetSampleCount( void );
/* 0e */ extern void    GetSample0Tick( _dword __far * );
/* 0f */ extern int     RegisterName( char __far *name );
/* 10 */ extern int     AccessName( char __far *name, _dword __far *id);
/* 11 */ extern int     UnregisterName( char __far *name );
/* 12 */ extern int     UnaccessName( char __far *name );
/* 13 */ extern int     StartConversation( _dword id );
/* 14 */ extern int     LookForConversation( _dword __far *id );
/* 15 */ extern int     EndConversation( _dword id );
/* 16 */ extern _dword  ConversationGet( _dword id, void __far *buff, _word len, _word attr );
/* 17 */ extern _word   ConversationPut( _dword id, const void __far *buff, _word len, _word attr );
/* 18 */ extern _word   IsConversationAck( _dword id );
/* 19 */ extern void    MyID( _dword __far *VM_id );
/* 1a */ extern _word   SetExecutionFocus( _dword VM_id );
/* 1b */ extern int     WhatHappened( void );
/* 1c */ extern _word   ConversationGetTimeOut( _dword id, void __far *buff, _word len, _word timeout );
/* 1d */ extern _word   ConversationPutTimeOut( _dword id, const void __far *buff, _word len, _word timeout );
/* 1e */ extern short   EMUInit( void );
/* 1f */ extern short   EMUShutdown( void );
/* 20 */ extern short   EMURegister( _word, _dword );
/* 21 */ extern short   EMUUnRegister( _word );
/* 22 */ extern short   FPUPresent( void );
/* 23 */ extern short   EMUSaveRestore( _word, void __far *, _word );
/* 24 */ extern void    PauseSampler( void );
/* 25 */ extern void    UnPauseSampler( void );
/* 26 */ extern void    EGAWrite( unsigned, char, char );
/* 27 */ extern char    VGARead( unsigned, char );
/* 28 */ extern void    DisableVideo( unsigned );
/* 29 */ extern short   RegisterInterruptCallback( void __far *callback, void __far *data, void __far *stack );
/* 2a */ extern void    UnRegisterInterruptCallback( void __far *callback );
/* 2b */ extern short   GetInterruptCallback( icb_data __far *callback );
/* 2c */ extern short   RestartFromInterrupt( void );
/* 2d */ extern short   Is32BitSel( short );
/* 2e */ extern long    GetVMId( void );
/* 2f */ extern short   HookIDT( void __far * );
/* 30 */ extern short   IDTFini( void );
/* 31 */ extern short   IDTInit( unsigned );
/* 32 */ extern short   ConversationPutPending( void );
/* 33 */ extern short   UseHotKey( int );
/* 34 */ extern short   RaiseInterruptInVM( _dword VM_id, _word intno );

#pragma aux CheckWin386Debug = \
        "mov    ax,0fa00h" \
        "int 2fh" \
    __value [__ax]

#pragma aux CopyMemory386 = \
        ".386" \
        "shl    edx,16" \
        "mov    dx,di" \
        "xchg   bx,si" \
        "shl    ebx,16" \
        "mov    bx,si" \
        "mov    si,ax" \
        "pop    di" \
        "mov    ax,0fa01h" \
        "int 2fh" \
    __parm [__cx] [__dx __di] [__ax] [__si __bx] \
    __value [__ax]

#pragma aux GetDescriptor = \
        "mov    ax,0fa02h" \
        "int 2fh" \
    __parm [__cx] [__es __bx]

#pragma aux GetLimit = \
        "mov    ax,0fa03h" \
        "int 2fh" \
    __parm [__bx] \
    __value [__dx __ax]

#pragma aux GetDebugRegister = \
        ".386" \
        "mov    ax,0fa04h" \
        "int 2fh" \
        "mov    dword ptr es:[di], ebx" \
    __parm [__cx] [__es __di] \
    __value [__ax] \
    __modify [__bx]

#pragma aux SetDebugRegister = \
        ".386" \
        "mov    ax,0fa05h" \
        "mov    ebx, dword ptr es:[di]" \
        "int 2fh" \
    __parm [__cx] [__es __di] \
    __value [__ax] \
    __modify [__bx]


#pragma aux InitSampler = \
        "mov    ax,0fa06h" \
        "int 2fh" \
    __parm [__es __bx] [__cx] [__dx] \
    __value [__ax]

#pragma aux QuitSampler = \
        ".386" \
        "mov    ax,0fa07h" \
        "int 2fh" \
        "mov    dword ptr es:[bx],eax" \
    __parm [__es __bx] \
    __modify [__ax]

#pragma aux StartSampler = \
        "mov    ax,0fa08h" \
        "int 2fh" \
    __value [__ax]

#pragma aux StopSampler =  \
        ".386" \
        "mov    ax,0fa09h" \
        "int 2fh" \
        "mov    dword ptr es:[bx],eax" \
    __parm [__es __bx] \
    __modify [__ax]

#pragma aux GetCurrTick =  \
        ".386" \
        "mov    ax,0fa0ah" \
        "int 2fh" \
        "mov    dword ptr es:[bx],eax" \
    __parm [__es __bx] \
    __modify [__ax]

#pragma aux SetTimerTick =  \
        "mov    ax,0fa0bh" \
        "int 2fh" \
    __parm [__bx] \
    __modify [__ax]

#pragma aux GetTimerTick =  \
        "mov    ax,0fa0ch" \
        "int 2fh" \
    __value [__ax]

#pragma aux GetSampleCount =  \
        "mov    ax,0fa0dh" \
        "int 2fh" \
    __value [__ax]

#pragma aux GetSample0Tick =  \
        ".386" \
        "mov    ax,0fa0dh" \
        "int 2fh" \
        "mov  dword ptr es:[bx],eax" \
    __parm [__es __bx] __modify [__ax]

#pragma aux RegisterName = \
        "mov    ax,0fa0fh" \
        "int 2fh" \
    __parm [__es __bx] \
    __value [__ax]

#pragma aux AccessName = \
        "mov    ax,0fa10h" \
        "int 2fh" \
    __parm [__es __bx] [__di __si] \
    __value [__ax]

#pragma aux UnregisterName = \
        "mov    ax,0fa11h" \
        "int 2fh" \
    __parm [__es __bx] \
    __value [__ax]

#pragma aux UnaccessName = \
        "mov    ax,0fa12h" \
        "int 2fh" \
    __parm [__es __bx] \
    __value [__ax]

#pragma aux StartConversation = \
        "mov    ax,0fa13h" \
        "int 2fh" \
    __parm [__cx __bx] \
    __value [__ax]

#pragma aux LookForConversation = \
        "mov    ax,0fa14h" \
        "int 2fh" \
    __parm [__es __bx] \
    __value [__ax]

#pragma aux EndConversation = \
        "mov    ax,0fa15h" \
        "int 2fh" \
    __parm [__cx __bx] \
    __value [__ax]

#pragma aux ConversationGet = \
        "mov    ax,0fa16h" \
        "int 2fh" \
    __parm [__cx __bx] [__es __dx] [__si] [__di] \
    __value [__dx __ax]

#pragma aux ConversationPut = \
        "mov    ax,0fa17h" \
        "int 2fh" \
    __parm [__cx __bx] [__es __dx] [__si] [__di] \
    __value [__ax]

#pragma aux IsConversationAck = \
        "mov    ax,0fa18h" \
        "int 2fh" \
    __parm [__cx __bx] \
    __value [__ax]

#pragma aux MyID =  \
        ".386" \
        "mov    ax,0fa19h" \
        "int 2fh" \
        "mov    dword ptr es:[bx],eax" \
    __parm [__es __bx] \
    __modify [__ax]

#pragma aux SetExecutionFocus = \
        "mov    ax,0fa1ah" \
        "int 2fh" \
    __parm [__cx __bx] \
    __value [__ax]

#pragma aux WhatHappened = \
        "mov    ax,0fa1bh" \
        "int 2fh" \
    __value [__ax]

#pragma aux ConversationGetTimeOut = \
        "mov    ax,0fa1ch" \
        "int 2fh" \
    __parm [__cx __bx] [__es __dx] [__si] [__di] \
    __value [__ax]

#pragma aux ConversationPutTimeOut = \
        "mov    ax,0fa1dh" \
        "int 2fh" \
    __parm [__cx __bx] [__es __dx] [__si] [__di] \
    __value [__ax]

#pragma aux EMUInit = \
        "mov    ax,0fa1eh" \
        "int 2fh" \
    __value [__ax]

#pragma aux EMUShutdown = \
        "mov    ax,0fa1fh" \
        "int 2fh" \
    __value [__ax]

#pragma aux EMURegister = \
        "mov    ax,0fa20h" \
        "int 2fh" \
    __parm [__dx] [__cx __bx] \
    __value [__ax]

#pragma aux EMUUnRegister = \
        "mov    ax,0fa21h" \
        "int 2fh" \
    __parm [__dx] \
    __value [__ax]

#pragma aux FPUPresent = \
        "mov    ax,0fa22h" \
        "int 2fh" \
    __value [__ax]

#pragma aux EMUSaveRestore = \
        "mov    ax,0fa23h" \
        "int 2fh" \
    __parm [__dx] [__cx __bx] [__si] \
    __value [__ax]

#pragma aux PauseSampler = \
        "mov    ax,0fa24h" \
        "int 2fh" \
    __modify [__ax]

#pragma aux UnPauseSampler = \
        "mov    ax,0fa25h" \
        "int 2fh" \
    __modify [__ax]

#pragma aux EGAWrite = \
        "mov    ax,0fa26h" \
        "int 2fh" \
    __parm [__dx] [__bl] [__bh] \
    __modify [__ax]

#pragma aux VGARead = \
        "mov    ax,0fa27h" \
        "int 2fh" \
    __parm [__dx] [__bl] \
    __value [__al]

#pragma aux DisableVideo = \
        "mov    ax,0fa28h" \
        "int 2fh" \
    __parm [__dx] \
    __modify [__ax]

#pragma aux RegisterInterruptCallback = \
        "mov    ax,0fa29h" \
        "int 2fh" \
    __parm [__cx __bx] [__es __dx] [__di __si] \
    __value [__ax]

#pragma aux UnRegisterInterruptCallback = \
        "mov    ax,0fa2ah" \
        "int 2fh" \
    __parm [__cx __bx] \
    __modify [__ax]

#pragma aux GetInterruptCallback = \
        "mov    ax,0fa2bh" \
        "int 2fh" \
    __parm [__cx __bx] \
    __modify [__ax]

#pragma aux RestartFromInterrupt = \
        "mov    ax,0fa2ch" \
        "int 2fh" \
    __modify [__ax]

#pragma aux Is32BitSel = \
        "mov    ax,0fa2dh" \
        "int 2fh" \
    __parm [__bx] \
    __modify [__ax]

#pragma aux GetVMId = \
        "mov    ax,0fa2eh" \
        "int 2fh" \
    __value [__dx __ax]

#pragma aux HookIDT = \
        "mov    ax,0fa2fh" \
        "int 2fh" \
    __parm [__cx __bx] \
    __modify [__ax]

#pragma aux IDTFini = \
        "mov    ax,0fa30h" \
        "int 2fh" \
    __modify [__ax]

#pragma aux IDTInit = \
        "mov    ax,0fa31h" \
        "int 2fh" \
    __parm [__bx] \
    __modify [__ax]

#pragma aux ConversationPutPending = \
        "mov    ax,0fa32h" \
        "int 2fh" \
    __value [__ax]

#pragma aux UseHotKey = \
        "mov    ax,0fa33h" \
        "int 2fh" \
    __parm [__bx] \
    __modify [__ax]

#pragma aux RaiseInterruptInVM = \
        "mov    ax,0fa34h" \
        "int 2fh" \
    __parm [__cx __bx] [__dx] \
    __modify [__ax]
