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


typedef void (*excfn)();

void WriteRegs( dos_debug __far * );
void ReadRegs( dos_debug __far * );
void RecordModHandle( ULONG value );
void WriteLinear( void __far *data, ULONG lin, USHORT size );
void ReadLinear( void __far *data, ULONG lin, USHORT size );
USHORT WriteBuffer( byte __far *data, USHORT segv, ULONG offv, USHORT size );
char __far *GetExceptionText( void );
ULONG MakeItFlatNumberOne( USHORT seg, ULONG offset );
void __far * MakeItSegmentedNumberOne( USHORT seg, ULONG offset );
void __far *MakeSegmentedPointer( ULONG val );
int GetDos32Debug( char __far *err );
void SetTaskDirectories( void );
bool DebugExecute( dos_debug __far *buff, ULONG cmd, bool );
int IsUnknownGDTSeg( USHORT seg );

extern  void    LoadThisDLL( void );
extern  void    EndLoadThisDLL( void );

extern  void    BreakPoint( ULONG );
#pragma aux     BreakPoint = 0xCC parm [ dx ax ] aborts;
extern void     bp( void );
#pragma aux     bp = 0xCC;
extern  void __far *Automagic( unsigned short );
#pragma aux     Automagic = 0x29 0xc4 /* sub sp,ax */\
                            0x89 0xe0 /* mov ax,sp */\
                            0x8c 0xd2 /* mov dx,ss */\
                            parm caller [ax] \
                            value [ax dx] \
                            modify [sp];

typedef struct {
        USHORT  phmod[2];               /* offset-segment */
        USHORT  mod_name[2];            /* offset-segment */
        USHORT  fail_len;
        PSZ     fail_name;              /* offset-segment */
        USHORT  hmod;
        CHAR    load_name[2];
} loadstack_t;

#pragma aux intrface __far modify [];
#pragma aux (intrface) TrapInit;
#pragma aux (intrface) TrapAccess;
#pragma aux (intrface) TrapFini;

typedef struct watch {
    addr48_ptr  addr;
    dword       value;
    int         len;
} watch;

#define MAX_WP  32

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
extern ULONG            __far *ModHandles;
extern USHORT           NumModHandles;
extern int              CurrModHandle;
extern ULONG            ExceptNum;
extern HMODULE          ThisDLLModHandle;
//extern dos_debug      Buff;
//extern watch          WatchPoints[ MAX_WP ];
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

bool CausePgmToLoadThisDLL( ULONG startLinear );
long TaskExecute( excfn rtn );
#pragma aux DoOpen parm [ dx ax ] [ bx ] [ cx ];
void DoOpen( char __far *name, int mode, int flags );
#pragma aux DoClose parm [ ax ];
static void DoClose( HFILE hdl );
#pragma aux DoDupFile parm [ ax ] [ dx ];
void DoDupFile( HFILE old, HFILE new );
#pragma aux DoWritePgmScrn parm [dx ax] [ bx ];
void DoWritePgmScrn( char __far *buff, USHORT len );
bool TaskReadWord( USHORT seg, ULONG off, USHORT __far *data );
bool TaskWriteWord( USHORT seg, ULONG off, USHORT data );
void TaskPrint( byte __far *data, unsigned len );

//#define DEBUG_OUT

#ifdef DEBUG_OUT
void Out( char *str );
void OutNum( ULONG i );
#else
#define Out( a )
#define OutNum( a )
#endif
