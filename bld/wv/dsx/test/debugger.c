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


#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <conio.h>
#include <i86.h>
#include <sys/types.h>
#include <fcntl.h>
#include <io.h>
#include "tinyio.h"

#define _PUSH_BP        0x55
#define _POP_BP         0x5d
#define RM_STACK_SIZE   ( 8 * 1024 )
#define MAX_STATE_SIZE  100
#define BUFF_SIZE       10
#define NB_VECTORS      256
#define SAVE_STATE      0
#define RESTORE_STATE   1
#define NULLFAR         0L
#define _debug( s )     { cputs( s );                           \
                          cputs( "\n\rPress a key\n\r" );       \
                          getch();                              \
                        }
#define _debug16( s, n ){ char buff[ 5 ];               \
                          cputs( s );                   \
                          _debug( itoa( n, buff, 16 ) );\
                        }
#define _debug32( s, n ){ char buff[ 9 ];               \
                          cputs( s );                   \
                          _debug( ltoa( n, buff, 16 ) );\
                        }

typedef struct {
    uint_16     limit;
    uint_32     base;
} DTreg;

unsigned                Envseg;
DTreg                   GDT;
unsigned                LDT;
DTreg                   IDT;
void                    far *OldPMHandler;
char                    PMProcCalled = 0;

static struct SREGS     RMRegs;
static struct SREGS     PMRegs;
static struct SREGS     SaveRegs;
static unsigned         StateSize;
static char             RMStateMem[ MAX_STATE_SIZE ];
static char             RMStack[ RM_STACK_SIZE ];
static void             far *OldRMStackPtr;
static void             far *OldRMHandler;
static void             far *SavePMState;
static void             far *RawRMtoPMAddr;
static void             far *RawPMtoRMAddr;
static void             far *RMVTable[ NB_VECTORS ];
static void             far *PMVTable[ NB_VECTORS ];

extern void             StoreDTs( DTreg *, unsigned *, DTreg * );
extern int              _fork( char *, unsigned );
extern far              *GetPModeAddr( unsigned * );
extern int              EnterPMode( void far *, unsigned );
extern int              GetRawAddrs( void far **, void far ** );
extern void interrupt   PM66Handler( void );
extern void             PMProc( void );
extern void             SaveState( unsigned, void *, void far * );
extern void             DoRawSwitch( void far *, void far *, unsigned,
                                     struct SREGS * );

#pragma aux DoRawSwitch \
        parm routine [] \
        modify exact [ax bx cx dx si di];

#pragma aux SaveState \
    parm [ax] [di] [cx bx] \
    modify exact [ax bx cx di];

extern void far *SwitchStacks( void far *, void far ** );
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
    _XOR_BX_BX          \
    _PUSH_BP            \
    _INT 0x66           \
    _POP_BP             \
    parm [ax] [dx]      \
    modify exact [ax bx cx dx si di];

extern void DPMIFini( void );
#pragma aux DPMIFini aborts = \
    _MOV_AH 0x4c        \
    _MOV_AL 0x00        \
    _INT_21             \
    modify exact [ax];


static void save_vects( void far **rmvtable, void far **pmvtable )
{
    int                 fhandle;
    int                 intnb;

    for( intnb = 0; intnb < NB_VECTORS; ++intnb ) {
        rmvtable[ intnb ] = (void far *)TinyDPMIGetRealVect( intnb );
        pmvtable[ intnb ] = TinyDPMIGetProtectVect( intnb );
    }
    fhandle = open( "vtable", O_BINARY | O_CREAT | O_TRUNC | O_WRONLY );
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

static void restore_vects( void far **rmvtable, void far **pmvtable )
{
    int                 intnb;

    for( intnb = 0; intnb < NB_VECTORS; ++intnb ) {
        TinyDPMISetRealVect( intnb, FP_SEG( rmvtable[ intnb ] ),
                             FP_OFF( rmvtable[ intnb ] ) );
        TinyDPMISetProtectVect( intnb, pmvtable[ intnb ] );
    }
}

static void interrupt rm_66_handler( unsigned pmcs, unsigned pmds )
{
    SaveRegs = PMRegs;
    PMRegs.cs = pmcs;
    PMRegs.ds = pmds;
    PMRegs.ss = pmds;
    PMRegs.es = 0;
    SwitchStacks( &RMStack[ RM_STACK_SIZE ], &OldRMStackPtr );
    SaveState( SAVE_STATE, RMStateMem, SavePMState );
    _debug( "Hi from rm_66_handler" );
    PMProcCalled = 0;
    _debug( "raw switching to the debugger's protected mode" );
    DoRawSwitch( RawRMtoPMAddr, RawPMtoRMAddr, FP_OFF( &PMProc ), &PMRegs );
    _debug( "made it back to rm_66_handler" );
    _debug( PMProcCalled ? "PMProc was called" : "PMProc was NOT called" );
    SaveState( RESTORE_STATE, RMStateMem, SavePMState );
    PMRegs = SaveRegs;
    SwitchStacks( OldRMStackPtr, NULLFAR );
}

static void hook_vects( void far *pmaddr, void far *rmaddr,
                        void far **oldpmaddr, void far **oldrmaddr )
{
    if( oldpmaddr ) {
        *oldpmaddr = TinyDPMIGetProtectVect( 0x66 );
    }
    if( TinyDPMISetProtectVect( 0x66, pmaddr ) ) {
        _debug( "error hooking protected mode vector 0x66" );
    }
    if( oldrmaddr ) {
        *oldrmaddr = (void far *)TinyDPMIGetRealVect( 0x66 );
    }
    if( TinyDPMISetRealVect( 0x66, FP_SEG( rmaddr ), FP_OFF( rmaddr ) ) ) {
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
    RawPMtoRMAddr = TinyDPMIRawPMtoRMAddr();
    RawRMtoPMAddr = (void far *)TinyDPMIRawRMtoPMAddr();
    SavePMState = (void far *)TinyDPMISavePMStateAddr();
    StateSize = TinyDPMISaveStateSize();
    return( ( RawPMtoRMAddr != NULLFAR ) && ( RawRMtoPMAddr != NULLFAR ) &&
            ( SavePMState != NULLFAR ) );
}

extern void main( void )
{
    unsigned            dpmisize;
    void                *dpmimem;
    unsigned            dpmiseg;
    void                far *switchaddr;

    segread( &RMRegs );
    Envseg = *(unsigned far *)MK_FP( _psp, 0x2c );
    switchaddr = GetPModeAddr( &dpmisize );
    dpmimem = malloc( dpmisize + 15 );
    dpmiseg = FP_SEG( dpmimem ) + ( FP_OFF( dpmimem ) + 15 ) / 16;
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
            hook_vects( MK_FP( PMRegs.cs, FP_OFF( &PM66Handler ) ),
                        &rm_66_handler, &OldPMHandler, &OldRMHandler );
            _debug( "doing an int 0x66" );
            DoInt66( PMRegs.cs, PMRegs.ds );
            _debug( "completed int 0x66" );
            _debug( "raw switching to real mode" );
            save_vects( RMVTable, PMVTable );
            DoRawSwitch( RawPMtoRMAddr, RawRMtoPMAddr,
                         FP_OFF( &start_shell ), &RMRegs );
            _debug( "made it back to the debugger's protected mode" );
            restore_vects( RMVTable, PMVTable );
            hook_vects( OldPMHandler, OldRMHandler, NULLFAR, NULLFAR );
        }
        _debug( "press any key to terminate debugger" );
        DPMIFini();
    }
}
