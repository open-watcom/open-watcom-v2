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


#include <dos.h>
#include "miniproc.h"
#include "nwtypes.h"

extern struct ResourceTagStructure      *TimerTag;

static struct TimerDataStructure        TimerData;
static unsigned long                    NumTicks;

#pragma aux inp = 0xec parm routine [ dx ];
#pragma aux outp = 0xee parm routine [ dx ] [ ax ];
extern char inp( int );
extern void outp( int, int );

short PortTest[] =
{
        0x3bc, 0x378, 0x278
};


short PortAddress[3] =
{
        0,0,0
};


int CheckForPort( int i, char value )
{
    int         j;

    outp( PortTest[ i ], value );
    for( j = 100; j != 0; j-- );
    return( inp( PortTest[ i ] ) == value );
}


static void InitPorts( void )
{
    int                 i;
    int                 portnum;

    portnum = 0;
    for( i = 0; i < 3; ++i ) {
        if( CheckForPort( i, 0x55 ) && CheckForPort( i, 0xaa ) ) {
            PortAddress[ portnum++ ] = PortTest[ i ];
        }
    }
}


unsigned long Ticks( void )
{
    return( NumTicks );
}


static void Ticker( LONG dummy )
{
    dummy = dummy;
    NumTicks++;
    CScheduleInterruptTimeCallBack( &TimerData );
}


static void SetupTimerData( void )
{
    TimerData.TCallBackProcedure = Ticker;
    TimerData.TCallBackEBXParameter = 0;
    TimerData.TCallBackWaitTime = 1;
    TimerData.TResourceTag = TimerTag;
}


char *InitSys( void )
{
    NumTicks = 0;
    InitPorts();
    SetupTimerData();
    CScheduleInterruptTimeCallBack( &TimerData );
    return( 0 );
}


void FiniSys( void )
{
    SetupTimerData();
    CCancelInterruptTimeCallBack( &TimerData );
}


int NumPrinters( void )
{
    int         i;

    for( i = 0; i < 3; ++i ) {
        if( PortAddress[ i ] == 0 ) break;
    }
    return( i );
}


unsigned PrnAddress( int n )
{
    return( PortAddress[ n ] );
}

void FreePorts( unsigned first, unsigned last )
{
    first = first; last = last;
}

unsigned AccessPorts( unsigned first, unsigned last )
{
    first = first; last = last;
    return( 1 );
}
