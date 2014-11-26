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


#include <i86.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/osinfo.h>
#include <sys/dev.h>
#include <sys/kernel.h>
#include "serial.h"
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

#define MILLISEC_PER_TICK       55


#define GET_MSECS (SysTime->nsec / NSecScale + (SysTime->seconds-StartSecs) * 1000)

void ZeroWaitCount()
{
    MSecsAtZero = GET_MSECS;
}


unsigned WaitCount()
{
    return( ( GET_MSECS - MSecsAtZero ) / MILLISEC_PER_TICK );
}


void ClearCom()
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

void StartBlockTrans()
{
    BlockIndex = 0;
}

void StopBlockTrans()
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
    if( ticks > 0 && timeout == 0 ) timeout = 1;

    if( dev_read( ComPort, &data, 1, 0, timeout, 0, 0, 0 ) != 1 ) {
        return( SDATA_NO_DATA );
    }
    if( data == 0xff ) {
        dev_read( ComPort, &data, 1, 0, timeout, 0, 0, 0 );
        if( data == 0xff ) return( data );
        /* a transmission error has occured */
        HadError = TRUE;
        dev_read( ComPort, &data, 1, 0, timeout, 0, 0, 0 );
        return( SDATA_NO_DATA );
    }
    return( data );
}


int GetByte()
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

    if( index == MIN_BAUD ) return( TRUE );
    if( index == CurrentBaud ) return( TRUE );
    temp = Rate[index];
    if( temp == B0 ) return( FALSE );

    cfsetispeed( &CurrPort, temp );
    cfsetospeed( &CurrPort, temp );
    if( tcsetattr( ComPort, TCSADRAIN, &CurrPort ) != 0 ) {
        return( FALSE );
    }
    CurrentBaud = index;
    return( TRUE );
}


char *ParsePortSpec( const char **spec )
{
    int         port;
    int         nid;
    char        ch;
    const char  *parm;
    const char  *start;
    static char name[] = "//___/dev/ser__";

    parm = (spec == NULL) ? "" : *spec;
    nid = 0;
    port = 0;
    if( *parm == '/' ) {
        start = parm;
        do {
            ch = *++parm;
        } while( ch != '.' && ch != '\0' );
        *parm = '\0';
        if( ComPort != 0 ) {
            DonePort();
        }
        ComPort = open( start, O_RDWR );
        *parm = ch;
        if( spec != NULL ) *spec = parm;
    } else {
        for( ;; ) {
            ch = *parm;
            if( ch < '0' || ch > '9' ) break;
            port = port * 10 + (ch-'0');
            ++parm;
        }
        if( ch == ',' ) {
            nid = port;
            if( nid > 255 ) return( TRP_QNX_invalid_node_number );
            port = 0;
            for( ;; ) {
                ++parm;
                ch = *parm;
                if( ch < '0' || ch > '9' ) break;
                port = port * 10 + (ch-'0');
            }
        }
        if( port == 0 ) port = 1;
        if( port > 99 ) return( TRP_ERR_invalid_serial_port_number );
        if( spec != NULL ) *spec = parm;
        name[4] = nid % 10 + '0';
        nid /= 10;
        name[3] = nid % 10 + '0';
        nid /= 10;
        name[2] = nid % 10 + '0';

        name[ sizeof( name ) - 3 ] = port / 10 + '0';
        name[ sizeof( name ) - 2 ] = port % 10 + '0';
        if( ComPort != 0 ) {
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


void DonePort()
{
    if( ComPort != 0 ) {
        tcsetattr( ComPort, TCSADRAIN, &SavePort );
        close( ComPort );
        ComPort = 0;
    }
}


bool CheckPendingError()
{
    bool    ret;

    ret = HadError;
    HadError = FALSE;
    return( ret );
}


void ClearLastChar()
{
}


void Wait( int timer_ticks )
{
    unsigned    wait_time;

    if( timer_ticks < 0 ) return;

    wait_time = WaitCount() + timer_ticks;
    while( WaitCount() < wait_time ) Yield();
}


char *InitSys( void )
{
    struct _osinfo  osinfo;

    qnx_osinfo( 0, &osinfo );
    SysTime = MK_FP( osinfo.timesel, 0 );
    NSecScale = (osinfo.version >= 410) ? 1000000 : (64UL*1024)*10;
    StartSecs = SysTime->seconds;
    CurrentBaud = -1;
    BlockIndex = -1;
    return( NULL );
}


void ResetSys( void )
{
}
