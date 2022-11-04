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


#include "trpimp.h"
#include "os2trap.h"
#include "dosdebug.h"


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

extern bool         ExpectingAFault;
extern scrtype      Screen;
extern PID          Pid;
extern bool         AtEnd;
extern USHORT       SID;
extern bool         Remote;
extern char         UtilBuff[BUFF_SIZE];
extern HFILE        SaveStdIn;
extern HFILE        SaveStdOut;
extern bool         CanExecTask;
extern USHORT       TaskFS;
extern ULONG        ExceptNum;
extern HMODULE      ThisDLLModHandle;
extern dos_debug    Buff;
extern USHORT       FlatCS;
extern USHORT       FlatDS;

extern void         WriteRegs( dos_debug __far * );
extern void         ReadRegs( dos_debug __far * );
extern void         WriteLinear( PVOID data, ULONG lin, USHORT size );
extern void         ReadLinear( PVOID data, ULONG lin, USHORT size );
extern USHORT       WriteBuffer( PBYTE data, USHORT segv, ULONG offv, USHORT size );
extern PCHAR        GetExceptionText( void );
extern ULONG        MakeItFlatNumberOne( USHORT seg, ULONG offset );
extern PVOID        MakeItSegmentedNumberOne( USHORT seg, ULONG offset );
extern PVOID        MakeSegmentedPointer( ULONG val );
extern int          GetDos32Debug( PCHAR err );
extern unsigned     Call32BitDosDebug( dos_debug __far *buff );
extern void         SetTaskDirectories( void );
extern bool         DebugExecute( dos_debug __far *buff, ULONG cmd, bool );
extern int          IsUnknownGDTSeg( USHORT seg );

extern void         LoadThisDLL( void );
extern void         EndLoadThisDLL( void );

extern ULONG        MakeItFlatNumberOne( USHORT seg, ULONG offset );
extern ULONG        MakeLocalPtrFlat( PVOID ptr );

extern bool         CausePgmToLoadThisDLL( ULONG startLinear );
extern bool         TaskReadWord( USHORT seg, ULONG off, USHORT __far *data );
extern bool         TaskWriteWord( USHORT seg, ULONG off, USHORT data );
extern void         TaskPrint( PBYTE data, unsigned len );

extern void         SetBrkPending( void );

//#define DEBUG_OUT

#ifdef DEBUG_OUT
extern void         Out( char *str );
extern void         OutNum( ULONG i );
#else
#define Out( a )
#define OutNum( a )
#endif

extern void DoOpen( PCHAR name, int mode, int flags );
#pragma aux DoOpen __parm [__dx __ax] [__bx] [__cx]
extern void DoClose( HFILE hdl );
#pragma aux DoClose __parm [__ax]
extern void DoDupFile( HFILE old, HFILE new );
#pragma aux DoDupFile __parm [__ax] [__dx]
extern void DoWritePgmScrn( PCHAR buff, USHORT len );
#pragma aux DoWritePgmScrn __parm [__dx __ax] [__bx]
