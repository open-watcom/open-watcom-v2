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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <i86.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sched.h>
#include "bool.h"
#include "serial.h"
#include "serlink.h"
#include "trpimp.h"
#include "trperr.h"


static baud_index                              CurrentBaud;
static int                                     ComPort = 0;
static struct termios                          SavePort;
static struct termios                          CurrPort;
static bool                                    HadError;

static char                                    BlockBuff[300];
static int                                     BlockIndex;

static clock_t                          start_clock;

static speed_t Rate[] = {
    #define BAUD_ENTRY(x,v,d)   B ## x,
    BAUD_ENTRIES
    #undef BAUD_ENTRY
    B0
};


void ResetTimerTicks( void )
{
    start_clock = clock();
}


unsigned GetTimerTicks( void )
{
    return( clock() - start_clock );
}


void ClearCom( void )
{
    tcflush( ComPort, TCIOFLUSH );
}


void SendByte( int value )
{
    if( BlockIndex >= 0 ) {
        BlockBuff[BlockIndex++] = value;
    } else {
        write( ComPort, &value, 1 );
        tcdrain( ComPort );
    }
}

void StartBlockTrans( void )
{
    BlockIndex = 0;
}

void StopBlockTrans( void )
{
    if( BlockIndex > 0 ) {
        write( ComPort, BlockBuff, BlockIndex );
        tcdrain( ComPort );
    }
    BlockIndex = -1;
}


int WaitByte( unsigned ticks )
{
    unsigned char       data;
    unsigned            timeout;
    int                 rc;

    timeout = (ticks * MILLISEC_PER_TICK) / 100;
    if( ticks > 0 && timeout == 0 )
        timeout = 1;    /* 0.1 sec */

    CurrPort.c_cc[VTIME] = timeout;
    rc = tcsetattr( ComPort, TCSADRAIN, &CurrPort );
//    return( TRP_ERR_could_not_set_serial_port_characteristics );
    CurrPort.c_cc[VTIME] = 1;
    if( rc != 0 ) {
        rc = SDATA_NO_DATA;
    } else {
        if( read( ComPort, &data, 1 ) != 1 ) {
            rc = SDATA_NO_DATA;
        } else {
            rc = data;
            if( data == 0xff ) {
                read( ComPort, &data, 1 );
                rc = data;
                if( data != 0xff ) {
                    /* a transmission error has occured */
                    HadError = true;
                    read( ComPort, &data, 1 );
                    rc = SDATA_NO_DATA;
                }
            }
        }
        tcsetattr( ComPort, TCSADRAIN, &CurrPort );
//        return( TRP_ERR_could_not_set_serial_port_characteristics );
    }
    return( rc );
}


int GetByte( void )
{
    return( WaitByte( 0 ) );
}


bool Baud( baud_index index )
{
    speed_t     temp;

    if( index == MIN_BAUD )
        return( true );
    if( index == CurrentBaud )
        return( true );
    temp = Rate[index];
    if( temp == B0 )
        return( false );

    cfsetispeed( &CurrPort, temp );
    cfsetospeed( &CurrPort, temp );
    if( tcsetattr( ComPort, TCSADRAIN, &CurrPort ) != 0 ) {
        return( false );
    }
    CurrentBaud = index;
    return( true );
}


char *ParsePortSpec( const char **spec )
{
    int         port;
    char        ch;
    const char  *parm;
    char        *start;
    static char name[PATH_MAX + 1];

    parm = (spec == NULL) ? "" : *spec;
    port = -1;
    if( *parm == '/' ) {
        start = name;
        do {
            *start++ = *parm++;
            ch = *parm;
        } while( ch != '.' && ch != '\0' );
        *start = '\0';
        if( ComPort != 0 ) {
            DonePort();
        }
        ComPort = open( name, O_RDWR );
        if( spec != NULL ) {
            *spec = parm;
        }
    } else {
        for( ;; ) {
            ch = *parm;
            if( ch < '0' || ch > '9' )
                break;
            port = port * 10 + ( ch - '0' );
            ++parm;
        }
        if( port == -1 )
            port = 0;
        if( port > 99 )
            return( TRP_ERR_invalid_serial_port_number );
        if( spec != NULL )
            *spec = parm;
        strcpy( name, "/dev/ttyS" );
        if( ComPort != 0 ) {
            if( port > 9 ) {
                name[9] = port / 10 + '0';
                name[10] = port % 10 + '0';
            } else {
                name[9] = port % 10 + '0';
            }
            DonePort();
        }
        ComPort = open( name, O_RDWR );
    }
    if( ComPort < 0 ) {
        ComPort = 0;
        return( (errno == ENOENT) ?
                TRP_ERR_serial_port_does_not_exist : TRP_ERR_serial_port_not_available );
    }
    tcgetattr( ComPort, &SavePort );
    CurrPort = SavePort;
    CurrPort.c_iflag = PARMRK;
    CurrPort.c_oflag = 0;
    CurrPort.c_cflag = CREAD | CS8;
    CurrPort.c_lflag = 0;
    CurrPort.c_cc[VMIN] = 0;    /* non-blocking read */
    CurrPort.c_cc[VTIME] = 1;   /* timeout 0.1 sec */
    if( tcsetattr( ComPort, TCSADRAIN, &CurrPort ) != 0 ) {
        return( TRP_ERR_could_not_set_serial_port_characteristics );
    }
    return( NULL );
}


void DonePort( void )
{
    if( ComPort != 0 ) {
        tcsetattr( ComPort, TCSADRAIN, &SavePort );
        close( ComPort );
        ComPort = 0;
    }
}


bool CheckPendingError( void )
{
    bool    ret;

    ret = HadError;
    HadError = false;
    return( ret );
}


void ClearLastChar( void )
{
}


void Wait( unsigned timer_ticks )
{
    unsigned    wait_time;

    wait_time = GetTimerTicks() + timer_ticks;
    while( GetTimerTicks() < wait_time ) {
        sched_yield();
    }
}


char *InitSys( void )
{
    CurrentBaud = UNDEF_BAUD;
    BlockIndex = -1;
    return( NULL );
}


void ResetSys( void )
{
}
