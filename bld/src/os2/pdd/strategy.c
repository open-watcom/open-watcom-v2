/*================================================================\
|                                                                 |
|      OS/2 Physical Device Driver Example Code                   |
|                  for Watcom C/C++ 10.x                          |
|                                                                 |
|  STRATEGY.C (Resident portion of driver)                        |
|                                                                 |
|  This device driver provides a high-resolution timer for OS/2   |
|                                                                 |
|  The source code was adapted from the Fall 1991 issue of        |
|  IBM Personal Systems Developer magazine.                       |
|                                                                 |
|                                                                 |
|  Adapted for Watcom C/C++ 10.x by WATCOM International Corp.    |
|  Original Author: Rick Fishman                                  |
|                   Code Blazers, Inc.                            |
|                   4113 Apricot                                  |
|                   Irvine, CA 92720                              |
|                                                                 |
\================================================================*/

#include "devdefs.h"
#include <i86.h>
#include <conio.h>
#include "devreqp.h"
#include "devaux.h"
#include "hrtimer.h"
#include "hrdev.h"


typedef union {
    USHORT w;
    char   b[2];
} tick_mask;

ULONG  DevHlp;  // DevHelp Interface Address

static TIMESTAMP ReadDataBuf = {
    0,      // millisecs
    0,      // nanosecs
    1,      // version
    0       // revision
};
static USHORT UserCount;
static USHORT Last8253;

#define NANOS_IN_TIC   840
#define AMILL          1000000

void pushf( void );
#pragma aux pushf = "pushf"  // save flags

void popf( void );
#pragma aux popf = "popf"    // restore flags

// want to do a 32 = 16*16 multiply
// without getting a runtime call
#pragma aux mul32 =  \
  "mul dx"           \
  modify [dx]        \
  parm [ax] [dx]     \
  value [ax dx]
ULONG  mul32( USHORT a, USHORT b );

/* ====================== start of interrupt handler ====================== */

static UpdateTimeStamp( USHORT new8253 )
{
    USHORT delta;
    ULONG  nanos;
    USHORT mills;
    if( Last8253 >= new8253 ) {
        delta = Last8253-new8253;
    } else {  // wrapped
        delta = 0xFFFF - new8253 + Last8253;
    }
    nanos = mul32( delta, NANOS_IN_TIC );
    Last8253 = new8253;
    nanos += ReadDataBuf.nanosecs;
    if( nanos >= AMILL ) { // overflow to millsecs
        mills = 1;
        nanos -= AMILL;             // the most we need to do this is 5 times anyways
        while( nanos >= AMILL ) {   // overflow into millisecs
            ++mills;                // try and avoid a runtime divide
            nanos -= AMILL;
        }
        ReadDataBuf.millisecs += mills;
    }
    ReadDataBuf.nanosecs = nanos;
}

/*
 * Called on each OS/2 clock tick (MC146818 chip)
 * via the SetTimer DevHlp
 */

extern VOID ClkInter( VOID )
{
    if( UserCount != 0 ) {
       tick_mask ticks;

       _disable();
        outp( i8253CtrlZeroOrTwo, i8253CmdReadCtrZero );
        ticks.b[0] = inp(i8253CounterZero );
        ticks.b[1] = inp(i8253CounterZero );
        _enable();
        UpdateTimeStamp( ticks.w );   // Update running time stamp
    }
}

//  Wrapper for ClkInter that saves/restores the flags, just in case
//  (don't add any code to this routine). This routine must do a far
//  return.

extern VOID far ClkInter_( VOID )
{
    pushf();
    ClkInter();
    popf();
}

/* ====================== end of interrupt handler ====================== */

static VOID StratNoOp( REQP_HEADER FAR *rp )
{
    rp->status = RPDONE;
}

static VOID StratRead( REQP_RWV FAR *rp )
{
        tick_mask ticks;
        PVOID     virt;

       _disable();
        outp( i8253CtrlZeroOrTwo, i8253CmdReadCtrZero );
        ticks.b[0] = inp(i8253CounterZero );
        ticks.b[1] = inp(i8253CounterZero );
        UpdateTimeStamp( ticks.w ); // Update the running time stamp
        if( rp->count < sizeof( TIMESTAMP )  ) {
            rp->count = 0;          // Caller's buffer is too small
            goto Exit;
        }
        if( DevPhysToVirt( rp->transaddr, rp->count, &virt ) != 0 ) {
            rp->count = 0;          // DevHlp failed
            goto Exit;
        }
        rp->count = sizeof( TIMESTAMP );
        (*(TIMESTAMP FAR*)virt) = ReadDataBuf;
Exit:
        _enable();
        rp->header.status |=  RPDONE;
}

static VOID StratOpen( REQP_OPENCLOSE FAR *rp )
{
        tick_mask ticks;

        _disable();
        if( UserCount == 0 ) {
            ReadDataBuf.millisecs = 0;
            ReadDataBuf.nanosecs = 0;
            outp( i8253CtrlZeroOrTwo, i8253CmdReadCtrZero );
            ticks.b[0] = inp(i8253CounterZero );
            ticks.b[1] = inp(i8253CounterZero );
            Last8253 = ticks.w;
        }
        ++UserCount;
        _enable();
        rp->header.status |=  RPDONE;
}

static VOID StratClose( REQP_OPENCLOSE FAR *rp )
{
        _disable();
        if( UserCount > 0 ) {
            --UserCount;
        }
        _enable();
        rp->header.status |=  RPDONE;
}

// Strategy entry point
//
// The strategy entry point must be declared according to the STRATEGY
// calling convention, which fetches arguments from the correct registers.


#pragma aux STRATEGY far parm [es bx];
#pragma aux (STRATEGY) Strategy;

VOID Strategy( REQP_ANY FAR *rp )
{

// Strategy routine for device set in header.c

    if( rp->header.command < RP_END ) {
        switch( rp->header.command ) {
        case RP_INIT:
            StratInit( rp );
            break;
        case RP_READ:
            StratRead( rp );
            break;
        case RP_READ_NO_WAIT:
        case RP_INPUT_STATUS:
        case RP_INPUT_FLUSH:
        case RP_WRITE:
        case RP_WRITE_VERIFY:
        case RP_OUTPUT_STATUS:
        case RP_OUTPUT_FLUSH:
            StratNoOp( rp );
            break;
        case RP_OPEN:
            StratOpen( rp );
            break;
        case RP_CLOSE:
            StratClose( rp );
            break;
        default:
            rp->header.status = RPERR_COMMAND | RPDONE;
        }
    } else {
        rp->header.status = RPERR_COMMAND | RPDONE;
    }
}
