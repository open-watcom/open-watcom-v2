/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include <sys/osinfo.h>
#include <sys/dev.h>
#include <sys/kernel.h>
#include "bool.h"
#include "serial.h"
#include "serlink.h"
#include "trpimp.h"
#include "trperr.h"

int                                     CurrentBaud;
unsigned long                           MSecsAtZero;
unsigned long                           StartSecs;
struct _timesel                         __far *SysTime;
int                                     ComPort = 0;
struct termios                          SavePort;
struct termios                          CurrPort;
bool                                    HadError;

char                                    BlockBuff[300];
int                                     BlockIndex;

unsigned long                           NSecScale;

extern int                              MaxBaud;


#define GET_MSECS (SysTime->nsec / NSecScale + (SysTime->seconds-StartSecs) * 1000)

void ResetTimerTicks( void )
{
    MSecsAtZero = GET_MSECS;
}


unsigned GetTimerTicks( void )
{
    return( ( GET_MSECS - MSecsAtZero ) / MILLISEC_PER_TICK );
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

    timeout = (ticks * MILLISEC_PER_TICK) / 100;
    if( ticks > 0 && timeout == 0 )
        timeout = 1;

    if( dev_read( ComPort, &data, 1, 0, timeout, 0, 0, 0 ) != 1 ) {
        return( SDATA_NO_DATA );
    }
    if( data == 0xff ) {
        dev_read( ComPort, &data, 1, 0, timeout, 0, 0, 0 );
        if( data == 0xff )
            return( data );
        /* a transmission error has occured */
        HadError = true;
        dev_read( ComPort, &data, 1, 0, timeout, 0, 0, 0 );
        return( SDATA_NO_DATA );
    }
    return( data );
}


int GetByte( void )
{
    return( WaitByte( 0 ) );
}


speed_t Rate[] = {
        B115200,
        B57600,
        B38400,
        B19200,
        B9600,
        B4800,
        B2400,
        B1200,
        B0 };


bool Baud( int index )
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
    int         nid;
    char        ch;
    const char  *parm;
    char        *start;
    static char name[PATH_MAX + 1];

    parm = (spec == NULL) ? "" : *spec;
    nid = 0;
    port = 0;
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
        if( ch == ',' ) {
            nid = port;
            if( nid > 255 )
                return( TRP_QNX_invalid_node_number );
            port = 0;
            for( ;; ) {
                ++parm;
                ch = *parm;
                if( ch < '0' || ch > '9' )
                    break;
                port = port * 10 + (ch-'0');
            }
        }
        if( port == 0 )
            port = 1;
        if( port > 99 )
            return( TRP_ERR_invalid_serial_port_number );
        if( spec != NULL )
            *spec = parm;
        strcpy( name, "//___/dev/ser__" );
        name[4] = nid % 10 + '0';
        nid /= 10;
        name[3] = nid % 10 + '0';
        nid /= 10;
        name[2] = nid % 10 + '0';
        if( ComPort != 0 ) {
        name[14] = port % 10 + '0';
        name[13] = port / 10 + '0';
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
        Yield();
    }
}


char *InitSys( void )
{
    struct _osinfo  osinfo;

    qnx_osinfo( 0, &osinfo );
    SysTime = _MK_FP( osinfo.timesel, 0 );
    NSecScale = (osinfo.version >= 410) ? 1000000 : (64UL*1024)*10;
    StartSecs = SysTime->seconds;
    CurrentBaud = -1;
    BlockIndex = -1;
    return( NULL );
}


void ResetSys( void )
{
}
