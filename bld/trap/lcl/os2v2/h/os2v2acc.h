/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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


typedef void(*excfn)();

void   WriteRegs( uDB_t * );
void   ReadRegs( uDB_t * );
void   WriteLinear( void *data, ULONG lin, USHORT size );
void   ReadLinear( void *data, ULONG lin, USHORT size );
USHORT WriteBuffer( void *data, USHORT segv, ULONG offv, USHORT size );
char   *GetExceptionText( void );
ULONG  MakeItFlatNumberOne( USHORT seg, ULONG offset );
ULONG  MakeItSegmentedNumberOne( USHORT seg, ULONG offset );
ULONG  MakeSegmentedPointer( ULONG val );
int    GetDos32Debug( char *err );
void   SetTaskDirectories( void );
bool   DebugExecute( uDB_t *buff, ULONG cmd, bool );
int    IsUnknownGDTSeg( USHORT seg );

extern  void    LoadHelperDLL( void );
extern  void    EndLoadHelperDLL( void );

/* Stack layout for calling Dos32LoadModule */
typedef struct {
    PSZ        fail_name;           /* 32-bit flat address */
    ULONG      fail_len;
    PSZ        mod_name;            /* 32-bit flat address */
    PHMODULE   phmod;               /* 32-bit flat address */
    HMODULE    hmod;
    CHAR       load_name[2];
} loadstack_t;

//#pragma aux intrface modify [];
//#pragma aux (intrface) TrapInit;
//#pragma aux (intrface) TrapAccess;
//#pragma aux (intrface) TrapFini;

typedef struct watch_point {
    addr48_ptr  addr;
    dword       value;
    word        len;
} watch_point;

/* Maximum watchpoints */
#define MAX_WP  32

extern bool             ExpectingAFault;
extern scrtype          Screen;
extern PID              Pid;
extern bool             AtEnd;
extern ULONG            SID;
extern bool             Remote;
extern char             UtilBuff[BUFF_SIZE];
extern HFILE            SaveStdIn;
extern HFILE            SaveStdOut;
extern bool             CanExecTask;
extern HMODULE          *ModHandles;
extern unsigned         NumModHandles;
extern unsigned         CurrModHandle;
extern ULONG            ExceptNum;
extern HMODULE          ThisDLLModHandle;
//extern uDB_t          Buff;
//extern watch_point      WatchPoints[MAX_WP];
//extern short          WatchCount;
extern USHORT           FlatCS,FlatDS;

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

bool CausePgmToLoadHelperDLL( ULONG startLinear );
long TaskExecute( excfn rtn );
bool TaskReadWord( USHORT seg, ULONG off, USHORT *data );
bool TaskWriteWord( USHORT seg, ULONG off, USHORT data );
void TaskPrint( byte *data, unsigned len );
void TaskReadXMMRegs( struct x86_xmm *xmm_regs );
void TaskWriteXMMRegs( struct x86_xmm *xmm_regs );

//#define DEBUG_OUT

#ifdef DEBUG_OUT
void Out( char *str );
void OutNum( ULONG i );
#else
#define Out( a )
#define OutNum( a )
#endif

extern void AppSession( void );
extern void DebugSession( void );
