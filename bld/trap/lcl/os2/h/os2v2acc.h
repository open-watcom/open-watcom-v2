/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#define _RetCodes( retblk, rc, value ) \
    { \
        USHORT return_code; \
        return_code = rc; \
        if( return_code == 0 ) { \
            retblk->ivalue = value; \
        } else { \
            retblk->ivalue = 0xffff0000 | return_code; \
        } \
    }

#define _RetCodesNoReturn( retblk, rc ) \
    { \
        USHORT return_code; \
        return_code = rc; \
        if( return_code == 0 ) { \
            retblk->ivalue = 0; \
        } else { \
            retblk->ivalue = 0xffff0000 | return_code; \
        } \
    }

typedef void (*excfn)();

typedef struct {
        USHORT  phmod[2];               /* offset-segment */
        USHORT  mod_name[2];            /* offset-segment */
        USHORT  fail_len;
        PSZ     fail_name;              /* offset-segment */
        USHORT  hmod;
        CHAR    load_name[2];
} loadstack_t;

typedef struct watch {
    addr48_ptr  addr;
    dword       value;
    int         len;
} watch;

extern PVOID Automagic( unsigned short );
#pragma aux Automagic = \
        "sub  sp,ax"    \
        "mov  ax,sp"    \
        "mov  dx,ss"    \
    __parm __caller [__ax] \
    __value         [__ax __dx] \
    __modify        [__sp]

#define MAX_WATCHES     32

extern bool             ExpectingAFault;
extern scrtype          Screen;
extern PID              Pid;
extern bool             AtEnd;
extern USHORT           SID;
extern bool             Remote;
extern char             UtilBuff[BUFF_SIZE];
extern HFILE            SaveStdIn;
extern HFILE            SaveStdOut;
extern bool             CanExecTask;
extern HMODULE          __far *ModHandles;
extern unsigned         NumModHandles;
extern unsigned         CurrModHandle;
extern ULONG            ExceptNum;
extern HMODULE          ThisDLLModHandle;
//extern dos_debug        Buff;
extern USHORT           FlatCS;
extern USHORT           FlatDS;

extern void     WriteRegs( dos_debug __far * );
extern void     ReadRegs( dos_debug __far * );
extern void     RecordModHandle( ULONG value );
extern void     WriteLinear( PVOID data, ULONG lin, USHORT size );
extern void     ReadLinear( PVOID data, ULONG lin, USHORT size );
extern USHORT   WriteBuffer( PBYTE data, USHORT segv, ULONG offv, USHORT size );
extern char __far *GetExceptionText( void );
extern ULONG    MakeItFlatNumberOne( USHORT seg, ULONG offset );
extern PVOID    MakeItSegmentedNumberOne( USHORT seg, ULONG offset );
extern PVOID    MakeSegmentedPointer( ULONG val );
extern int      GetDos32Debug( char __far *err );
extern void     SetTaskDirectories( void );
extern bool     DebugExecute( dos_debug __far *buff, ULONG cmd, bool );
extern int      IsUnknownGDTSeg( USHORT seg );

extern void     LoadThisDLL( void );
extern void     EndLoadThisDLL( void );

extern ULONG    MakeItFlatNumberOne( USHORT seg, ULONG offset );
extern ULONG    MakeLocalPtrFlat( PVOID ptr );

extern bool CausePgmToLoadThisDLL( ULONG startLinear );
extern long TaskExecute( excfn rtn );
extern void DoOpen( char __far *name, int mode, int flags );
#pragma aux DoOpen __parm [__dx __ax] [__bx] [__cx]
extern void DoClose( HFILE hdl );
#pragma aux DoClose __parm [__ax]
extern void DoDupFile( HFILE old, HFILE new );
#pragma aux DoDupFile __parm [__ax] [__dx]
extern void DoWritePgmScrn( char __far *buff, USHORT len );
#pragma aux DoWritePgmScrn __parm [__dx __ax] [__bx]
extern bool TaskReadWord( USHORT seg, ULONG off, USHORT __far *data );
extern bool TaskWriteWord( USHORT seg, ULONG off, USHORT data );
extern void TaskPrint( PBYTE data, unsigned len );

extern void AppSession( void );
extern void DebugSession( void );
extern void SetBrkPending( void );

//#define DEBUG_OUT

#ifdef DEBUG_OUT
extern void Out( char *str );
extern void OutNum( ULONG i );
#else
#define Out( a )
#define OutNum( a )
#endif
