/*================================================================\
|                                                                 |
|      OS/2 Physical Device Driver Example Code                   |
|                  for Watcom C/C++ 10.x                          |
|                                                                 |
|  STRATINI.C (Discardable portion of driver)                     |
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
#include "hrdev.h"

// Ensure that the Initialization code located at the end of the driver

#pragma code_seg ( "_INITCODE" ) ;
#pragma data_seg ( "_INITDATA", "INITDATA" ) ;

extern VOID ClkInter_(VOID);    // entry to Clock Interrupt handler

extern USHORT  OffFinalCS;
extern USHORT  OffFinalDS;

static char CopyRightMsg[] = {
    "\r\nHigh Resolution Timer - Version 1.0\r\n"
    "Adapted for Watcom C/C++ by WATCOM International Corp.\r\n"
    "from 'An OS/2 High Resolution Software Timer'\r\n"
    "IBM Personal Systems Developer (Fall 1991)\r\n"
    "Courtesy of Code Blazers, Inc. 1991\r\n\n"
};

extern VOID StratInit( REQP_INIT FAR *rp )
{
    USHORT ret;

    DosWrite( 1, STRV( CopyRightMsg ), &ret );
    DevHlp = rp->in.devhlp;                         // save far pointer to DevHlp
    _disable();
    outp( i8253CtrlZeroOrTwo, i8253CmdInitCtrZero );// set timer 0 to mode 2
    outp( i8253CounterZero, 0 );                    // set count reg LSB
    outp( i8253CounterZero, 0 );                    // and MSB to 0
    _enable();
    /*
     * Use the 'SetTimer' DevHlp to chain our timer tick routine
     */
    if( DevSetTimer( &ClkInter_ ) != 0 ) {
        DosWrite( 1, STRV( "ERROR couldn't add timer\r\n" ), &ret );
        rp->out.units = 0;
        rp->out.finalcs = 0;
        rp->out.finalds = 0;
        rp->header.status = RPERR_GENERAL;
    } else {
        rp->out.finalcs = FP_OFF( &OffFinalCS );    // set pointers to
        rp->out.finalds = FP_OFF( &OffFinalDS );    //discardable code/data
    }
    rp->header.status |=  RPDONE;
}
