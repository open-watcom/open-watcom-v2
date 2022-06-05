/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OS/2 2.x trap file internal declarations.
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

typedef void(*excfn)();

/* Stack layout for calling Dos32LoadModule */
typedef struct {
    PSZ        fail_name;           /* 32-bit flat address */
    ULONG      fail_len;
    PSZ        mod_name;            /* 32-bit flat address */
    PHMODULE   phmod;               /* 32-bit flat address */
    HMODULE    hmod;
    CHAR       load_name[2];
} loadstack_t;

extern void         WriteRegs( uDB_t * );
extern void         ReadRegs( uDB_t * );
extern void         WriteLinear( void *data, ULONG lin, USHORT size );
extern void         ReadLinear( void *data, ULONG lin, USHORT size );
extern USHORT       WriteBuffer( void *data, USHORT segv, ULONG offv, USHORT size );
extern char         *GetExceptionText( void );
extern ULONG        MakeItFlatNumberOne( USHORT seg, ULONG offset );
extern ULONG        MakeItSegmentedNumberOne( USHORT seg, ULONG offset );
extern ULONG        MakeSegmentedPointer( ULONG val );
extern int          GetDos32Debug( char *err );
extern void         SetTaskDirectories( void );
extern bool         DebugExecute( uDB_t *buff, ULONG cmd, bool );
extern int          IsUnknownGDTSeg( USHORT seg );

extern void         LoadHelperDLL( void );
extern void         EndLoadHelperDLL( void );

extern bool         ExpectingAFault;
extern scrtype      Screen;
extern PID          Pid;
extern bool         AtEnd;
extern ULONG        SID;
extern bool         Remote;
extern char         UtilBuff[BUFF_SIZE];
extern HFILE        SaveStdIn;
extern HFILE        SaveStdOut;
extern bool         CanExecTask;
extern ULONG        ExceptNum;
extern HMODULE      ThisDLLModHandle;
//extern uDB_t        Buff;
extern USHORT       FlatCS;
extern USHORT       FlatDS;

extern bool         CausePgmToLoadHelperDLL( ULONG startLinear );
extern long         TaskExecute( excfn rtn );
extern bool         TaskReadWord( USHORT seg, ULONG off, USHORT *data );
extern bool         TaskWriteWord( USHORT seg, ULONG off, USHORT data );
extern void         TaskPrint( byte *data, unsigned len );
extern void         TaskReadXMMRegs( struct x86_xmm *xmm_regs );
extern void         TaskWriteXMMRegs( struct x86_xmm *xmm_regs );

extern void         AppSession( void );
extern void         DebugSession( void );
extern void         SetBrkPending( void );

//#define DEBUG_OUT

#ifdef DEBUG_OUT
extern void         Out( char *str );
extern void         OutNum( ULONG i );
#else
#define Out( a )
#define OutNum( a )
#endif
