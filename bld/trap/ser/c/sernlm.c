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
* Description:  NLM serial link core.
*
****************************************************************************/


#include <dos.h>
#include "bool.h"
#include "miniproc.h"
#include "debugme.h"
#include "nwtypes.h"
#include "serial.h"
#include "serlink.h"
#include "aio.h"
#include "trpimp.h"
#include "trperr.h"


extern struct ResourceTagStructure      *TimerTag;
extern struct LoadDefinitionStruct      *MyNLMHandle;

int                                     PortNumber = -1;
struct TimerDataStructure               TimerData;

static int                              ComPortHandle;
static struct ResourceTagStructure      *SerialTag;
static unsigned                         TimerTicks;
static baud_index                       CurrentBaud;

static int Divisor[] = {
    #define BAUD_ENTRY(x,v,d)   AIO_BAUD_ ## x,
    BAUD_ENTRIES
    #undef BAUD_ENTRY
    0
};

static void Tick( LONG dummy )
{
    dummy = dummy;
    TimerTicks++;
    CScheduleInterruptTimeCallBack( &TimerData );
}


static void SetupTimerData( void )
{
    TimerData.TCallBackProcedure = Tick;
    TimerData.TCallBackEBXParameter = 0;
    TimerData.TCallBackWaitTime = 1;
    TimerData.TResourceTag = TimerTag;
}


void ResetTimerTicks( void )
{
    TimerTicks = 0;
}


unsigned GetTimerTicks( void )
{
    return( TimerTicks );
}


void ClearCom( void )
{
    AIOFlushBuffers( ComPortHandle,
                      AIO_FLUSH_READ_BUFFER+AIO_FLUSH_WRITE_BUFFER );
}


char *InitSys( void )
{
    SetupTimerData();
    CScheduleInterruptTimeCallBack( &TimerData );
    CurrentBaud = UNDEF_BAUD;
    return( NULL );
}


void ResetSys( void )
{
    SetupTimerData();
    CCancelInterruptTimeCallBack( &TimerData );
}


void SendByte( int value )
{
    LONG count;

    AIOWriteData( ComPortHandle, (char *)&value, 1, &count );
    do {
        AIOWriteStatus( ComPortHandle, &count, NULL );
    } while( count != 0 );
}


void StartBlockTrans( void )
{
}

void StopBlockTrans( void )
{
}

int GetByte( void )
{
    int         data;
    LONG        count;

    AIOReadStatus( ComPortHandle, &count, NULL );
    if( count == 0 ) {
        data = SDATA_NO_DATA;
    } else {
        data = 0;
        AIOReadData( ComPortHandle, (char *)&data, 1, &count );
    }
    return( data );
}

bool Baud( baud_index index )
{
    LONG        rc;

    if( index == MODEM_BAUD )
        return( true );
    if( index == CurrentBaud )
        return( true );
    rc = AIOConfigurePort( ComPortHandle, Divisor[index], AIO_DATA_BITS_8,
                    AIO_STOP_BITS_1, AIO_PARITY_NONE,
                    AIO_HARDWARE_FLOW_CONTROL_OFF );
    if( rc != AIO_SUCCESS ) {
        return( false );
    }
    CurrentBaud = index;
    return( true );
}


char *ParsePortSpec( const char **spec )
{
    const char  *parm;
    int         port;
    int         com_num;
    int         rc;
    int         hardware_type;
    int         board_number;
    int         num;
    int         *var;

    parm = (spec == NULL) ? "" : *spec;

    board_number = AIO_BOARD_NUMBER_WILDCARD;
    port = -1;
    com_num = 1;
    for( ;; ) {
        if( *parm == 'b' ) {
            var = &board_number;
            ++parm;
        } else if( *parm == 'p' ) {
            var = &port;
            ++parm;
        } else if( !(*parm >= '0' && *parm <= '9') ) {
            break;
        } else {
            var = &com_num;
        }
        if( !(*parm >= '0' && *parm <= '9') ) {
            return( TRP_ERR_invalid_serial_port_number );
        }
        num = 0;
        do {
            num = num * 10 + (*parm - '0');
            ++parm;
        } while( *parm >= '0' && *parm <= '9' );
        *var = num;
    }
    if( *parm != '\0' && *parm != '.' )
        return( TRP_ERR_invalid_serial_port_number );
    if( spec != NULL )
        *spec = parm;

    if( PortNumber != -1 ) {
        AIOReleasePort( ComPortHandle );
    }
    if( SerialTag == 0 ) {
        SerialTag = AllocateResourceTag( MyNLMHandle,
                                        (BYTE *)"Debug Server Serial IO",
                                        ASYNCIOSignature );
    }
    if( port == -1 )
        port = com_num - 1;
    num = port;
    hardware_type = AIO_COMX_TYPE;
    switch( rc = AIOAcquirePortWithRTag( &hardware_type, &board_number,
                                    &port, &ComPortHandle, (LONG)SerialTag ) ) {
    case AIO_SUCCESS:
        break;
    case AIO_QUALIFIED_SUCCESS:
        if( port == num )
            break;
        /* fall through */
    case AIO_TYPE_NUMBER_INVALID:
    case AIO_BOARD_NUMBER_INVALID:
    case AIO_PORT_NUMBER_INVALID:
    case AIO_RTAG_INVALID:
    case AIO_PORT_NOT_AVAILABLE:
    case AIO_FAILURE:
        return( TRP_ERR_serial_port_not_available );
    default:
      {
        #undef static
        static char num[2];

        rc = -rc;
        num[0] = rc / 10 + '0';
        num[1] = rc % 10;
        num[2] = 0;
        return( num );
      }
    }
    PortNumber = port;
    return( NULL );
}


void DonePort( void )
{
    if( PortNumber != -1 ) {
        AIOReleasePort( ComPortHandle );
        PortNumber = -1;
    }
}

bool CheckPendingError( void )
{
    return( false );                    // NYI -- waiting for Rich Jeske
}

void ClearLastChar( void )
{
}
