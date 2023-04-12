/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  RDOS Serial handling routines.
*
****************************************************************************/


#include <rdos.h>

#include <stdio.h>
#include "trpimp.h"
#include "trperr.h"
#include "serial.h"
#include "serlink.h"


static baud_index       CurrentBaud;
static int              hSerial = 0;
static int              hWait = 0;
static int              comPortNumber = 1;
static unsigned long    lastLsb;

static int Divisor[] = {
    #define BAUD_ENTRY(x,v,d)   d,
    BAUD_ENTRIES
    #undef BAUD_ENTRY
    0
};

void ResetTimerTicks( void )
{
    unsigned long msb;

    RdosGetSysTime( &msb, &lastLsb );
}

unsigned GetTimerTicks( void )
{
    unsigned long msb;
    unsigned long lsb;

    RdosGetSysTime( &msb, &lsb );

    return( (lsb - lastLsb) >> 16 );
}

void Wait( unsigned timer_ticks )
{
    RdosWaitMilli( MILLISEC_PER_TICK * timer_ticks );
}

char *InitSys( void )
{
    hSerial = RdosOpenCom( comPortNumber - 1, 9600, 'N', 8, 1, 0x1000, 0x1000 );
    if( hSerial ) {
        RdosSetDtr( hSerial );
        RdosSetRts( hSerial );
    }
    CurrentBaud = UNDEF_BAUD;
    return( NULL );
}


void ResetSys( void )
{
    if( hSerial ) {
        RdosCloseCom( hSerial );
        hSerial = 0;
    }
}

#if 0
bool Terminate( void )
{
    ResetSys();
    return( true );
}
#endif

void SendByte( int value )
{
    RdosWriteCom( hSerial, (char)value );
}

void StartBlockTrans( void )
{
}

void StopBlockTrans( void )
{
}

int WaitByte( unsigned ticks )
{
    if( !hWait ) {
        hWait = RdosCreateWait();
        RdosAddWaitForCom( hWait, hSerial, (int)(&hSerial));
    }

    if ( RdosWaitTimeout( hWait, 500 + MILLISEC_PER_TICK * ticks ) != 0 )
        return (int)RdosReadCom( hSerial );
    return SDATA_NO_DATA;
}

int GetByte( void )
{
    if( !hWait ) {
        hWait = RdosCreateWait();
        RdosAddWaitForCom( hWait, hSerial, (int)(&hSerial));
    }

    if ( RdosWaitTimeout( hWait, 50 ) != 0 )
        return (int)RdosReadCom( hSerial );
    return SDATA_NO_DATA;
}


void ClearCom( void )
{
    RdosFlushCom( hSerial );
}

#if 0
void SendABreak( void )
{
}

bool TestForBreak( void )
{
    return( false );
}
#endif

bool Baud( baud_index index )
{
    if( index == MODEM_BAUD )
        return( true );

    if( index == CurrentBaud )
        return( true );

    if( hSerial )
        RdosCloseCom( hSerial );

    if( hWait ) {
        RdosCloseWait( hWait );
        hWait = 0;
    }

    hSerial = RdosOpenCom( comPortNumber - 1, 115200 / Divisor[index], 'N', 8, 1, 0x1000, 0x1000 );

    CurrentBaud = index;

    return( true );
}

char *ParsePortSpec( const char **spec )
{
    comPortNumber = 1;
    if( spec != NULL ) {
        char ch = **spec;

        if( ch >= '1' && ch <= '9' ) {
            comPortNumber = ch - '0';
            if( comPortNumber <= RdosGetMaxComPort() ) {
                return( NULL );
            } else {
                return( TRP_ERR_invalid_serial_port_number );
            }
        }
    }
    return( NULL );
}


void DonePort( void )
{
}


bool CheckPendingError( void )
{
    return( false );
}


void ClearLastChar( void )
{
}
