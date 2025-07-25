/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS protected mode debugging test app (16-bit executable).
*
****************************************************************************/


#include <stdlib.h>
#include <malloc.h>
#include <conio.h>
#include <sys/types.h>
#include "tinyio.h"
#include "dbginit.h"
#include "wio.h"


#define RM_STACK_SIZE   ( 8 * 1024 )
#define MAX_STATE_SIZE  100
#define NB_VECTORS      256
#define SAVE_STATE      0
#define RESTORE_STATE   1
#define NULLFAR         0L
#define _debug( s )     { cputs( s );                       \
                          cputs( "\n\rPress a key\n\r" );   \
                          getch();                          \
                        }
#define _debug16( s, n ) { char buff[5];                    \
                           cputs( s );                      \
                           _debug( itoa( n, buff, 16 ) );   \
                         }
#define _debug32( s, n ) { char buff[9];                    \
                           cputs( s );                      \
                           _debug( ltoa( n, buff, 16 ) );   \
                         }

#define GET_ENV_FROM_PSP()  (*(addr_seg __far *)_MK_FP(_psp, 0x2c))

typedef struct {
    uint_16     limit;
    uint_32     base;
} DTreg;

unsigned                Envseg;
DTreg                   GDT;
unsigned                LDT;
DTreg                   IDT;
void                    __far *OldPMHandler;
char                    PMProcCalled = 0;

static struct SREGS     RMRegs;
static struct SREGS     PMRegs;
static struct SREGS     SaveRegs;
static unsigned         StateSize;
static char             RMStateMem[MAX_STATE_SIZE];
static char             RMStack[RM_STACK_SIZE];
static void             __far *OldRMStackPtr;
static void             __far *OldRMHandler;
static void             __far *SavePMState;
static void             __far *RawRMtoPMAddr;
static void             __far *RawPMtoRMAddr;
static void             __far *RMVTable[NB_VECTORS];
static void             __far *PMVTable[NB_VECTORS];

extern void             StoreDTs( DTreg *, unsigned *, DTreg * );
extern void __far       *GetPModeAddr( unsigned * );
extern int              EnterPMode( void __far *, unsigned );
extern int              GetRawAddrs( void __far **, void __far ** );
extern void interrupt   PM66Handler( void );
extern void             PMProc( void );
extern void             SaveState( unsigned, void *, void __far * );
extern void             DoRawSwitch( void __far *, void __far *, unsigned,
                                     struct SREGS * );

#pragma aux DoRawSwitch \
        parm routine [] \
        modify exact [ax bx cx dx si di];

#pragma aux SaveState \
    parm [ax] [di] [cx bx] \
    modify exact [ax bx cx di];

extern void __far *SwitchStacks( void __far *, void __far ** );
#pragma aux SwitchStacks = \
    "test bx, bx    ",  \
    "jz   L1        ",  \
    "mov  [bx], sp  ",  \
    "mov  [bx+2], ss",  \
    "L1:            ",  \
    "mov  ss, dx    ",  \
    "mov  sp, ax    "   \
    parm [ax dx] [bx]   \
    modify exact [sp];

extern void DoInt66( unsigned, unsigned );
#pragma aux DoInt66 =   \
    "xor  bx, bx "      \
    "push bp     "      \
    "int  0x66   "      \
    "pop  bp     "      \
    parm [ax] [dx]      \
    modify exact [ax bx cx dx si di];

extern void DPMIFini( void );
#pragma aux DPMIFini aborts = \
    "mov  ah, 0x4c"     \
    "mov  al, 0   "     \
    "int  0x21    "     \
    modify exact [ax];


static void save_vects( void __far **rmvtable, void __far **pmvtable )
{
    int                 fhandle;
    int                 intnb;

    for( intnb = 0; intnb < NB_VECTORS; ++intnb ) {
        rmvtable[intnb] = DPMIGetRealModeInterruptVector( intnb );
        pmvtable[intnb] = DPMIGetPMInterruptVector( intnb );
    }
    fhandle = open( "vtable", O_BINARY | O_CREAT | O_TRUNC | O_WRONLY, PMODE_RW );
    if( fhandle <= 0 ) {
        _debug16( "error: fhandle <= 0, fhandle=", fhandle );
    } else {
        write( fhandle, rmvtable, NB_VECTORS * sizeof( *rmvtable ) );
        write( fhandle, pmvtable, NB_VECTORS * sizeof( *pmvtable ) );
        write( fhandle, &RMRegs.cs, sizeof( RMRegs.ds ) );
        write( fhandle, &RMRegs.ds, sizeof( RMRegs.ds ) );
        close( fhandle );
    }
}

static void restore_vects( void __far **rmvtable, void __far **pmvtable )
{
    int                 intnb;

    for( intnb = 0; intnb < NB_VECTORS; ++intnb ) {
        DPMISetRealModeInterruptVector( intnb, rmvtable[intnb] );
        DPMISetPMInterruptVector( intnb, pmvtable[intnb] );
    }
}

static void interrupt rm_66_handler( unsigned pmcs, unsigned pmds )
{
    SaveRegs = PMRegs;
    PMRegs.cs = pmcs;
    PMRegs.ds = pmds;
    PMRegs.ss = pmds;
    PMRegs.es = 0;
    SwitchStacks( &RMStack[RM_STACK_SIZE], &OldRMStackPtr );
    SaveState( SAVE_STATE, RMStateMem, SavePMState );
    _debug( "Hi from rm_66_handler" );
    PMProcCalled = 0;
    _debug( "raw switching to the debugger's protected mode" );
    DoRawSwitch( RawRMtoPMAddr, RawPMtoRMAddr, _FP_OFF( &PMProc ), &PMRegs );
    _debug( "made it back to rm_66_handler" );
    _debug( PMProcCalled ? "PMProc was called" : "PMProc was NOT called" );
    SaveState( RESTORE_STATE, RMStateMem, SavePMState );
    PMRegs = SaveRegs;
    SwitchStacks( OldRMStackPtr, NULLFAR );
}

static void hook_vects( void __far *pmaddr, void __far *rmaddr,
                        void __far **oldpmaddr, void __far **oldrmaddr )
{
    if( oldpmaddr ) {
        *oldpmaddr = DPMIGetPMInterruptVector( 0x66 );
    }
    if( DPMISetPMInterruptVector( 0x66, pmaddr ) ) {
        _debug( "error hooking protected mode vector 0x66" );
    }
    if( oldrmaddr ) {
        *oldrmaddr = (void __far *)DPMIGetRealModeInterruptVector( 0x66 );
    }
    if( DPMISetRealModeInterruptVector( 0x66, rmaddr ) ) {
        _debug( "error hooking real mode vector 0x66" );
    }
}

static void start_shell( void )
{
    void                *statemem;

    statemem = alloca( StateSize );
    SaveState( SAVE_STATE, statemem, SavePMState );
    _debug( "reached real mode, starting up task.exe" );
    _fork( "task.exe", 8 );
    //_fork( "", 0 );
    _debug( "raw switching back to the debugger's protected mode" );
    SaveState( RESTORE_STATE, statemem, SavePMState );
}

static int getaddrs( void )
{
    RawPMtoRMAddr = DPMIRawPMtoRMAddr();
    RawRMtoPMAddr = (void __far *)DPMIRawRMtoPMAddr();
    SavePMState = (void __far *)DPMISavePMStateAddr();
    StateSize = DPMISaveStateSize();
    return( ( RawPMtoRMAddr != NULLFAR ) && ( RawRMtoPMAddr != NULLFAR ) &&
            ( SavePMState != NULLFAR ) );
}

extern void main( void )
{
    unsigned            dpmisize;
    void                *dpmimem;
    unsigned            dpmiseg;
    void                __far *switchaddr;

    segread( &RMRegs );
    Envseg = GET_ENV_FROM_PSP();
    switchaddr = GetPModeAddr( &dpmisize );
    dpmimem = malloc( dpmisize + 15 );
    dpmiseg = _FP_SEG( dpmimem ) + ( _FP_OFF( dpmimem ) + 15 ) / 16;
    if( ( switchaddr == NULLFAR ) || ( dpmimem == NULL ) ) {
        _debug( "error, DPMI host is not present" );
    } else if( !EnterPMode( switchaddr, dpmiseg ) ) {
        _debug( "could not enter protected mode" );
    } else {
        _debug( "entered protected mode" );
        if( !getaddrs() ) {
            _debug( "could not get raw switch and state save addresses" );
        } else {

            segread( &PMRegs );
            hook_vects( _MK_FP( PMRegs.cs, _FP_OFF( &PM66Handler ) ),
                        &rm_66_handler, &OldPMHandler, &OldRMHandler );
            _debug( "doing an int 0x66" );
            DoInt66( PMRegs.cs, PMRegs.ds );
            _debug( "completed int 0x66" );
            _debug( "raw switching to real mode" );
            save_vects( RMVTable, PMVTable );
            DoRawSwitch( RawPMtoRMAddr, RawRMtoPMAddr,
                         _FP_OFF( &start_shell ), &RMRegs );
            _debug( "made it back to the debugger's protected mode" );
            restore_vects( RMVTable, PMVTable );
            hook_vects( OldPMHandler, OldRMHandler, NULLFAR, NULLFAR );
        }
        _debug( "press any key to terminate debugger" );
        DPMIFini();
    }
}
