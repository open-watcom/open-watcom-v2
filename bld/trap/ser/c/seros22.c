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
* Description:  OS/2 2.x specific serial port access functions.
*
****************************************************************************/


#include <stddef.h>
#include <dos.h>
#define INCL_DOSSEMAPHORES
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#define INCL_DOSMISC
#include <os2.h>
#include "serial.h"
#include "trpimp.h"
#include "trperr.h"

int                                     CurrentBaud;
long                                    MSecsAtZero;
HFILE                                   ComPort = 0;

#define BSIZE   1024                    /* must be power of 2 */
char                                    ReadBuff[BSIZE];
int                                     ReadBuffRemove;
int                                     ReadBuffAdd;

#define READER_STACKSIZE                8192
TID                                     ReaderId;
HEV                                     ReadSemaphore;
bool                                    OverRun;
bool                                    BlockTransmission;

extern int                              MaxBaud;

#define MILLISEC_PER_TICK       55

#define SERIAL          0x0001

#define SETLINECTRL     0x0042
    #define     DATA_BITS_8     0x08
    #define STOP_BITS_1 0x00
    #define PARITY_NONE 0x00

#define SETBAUDRATE     0x0041

#define GETCOMMEVENT    0x0072
    #define LAST_CHAR_SENT      0x0004
    #define CHAR_AVAILABLE      0x0001
    #define ERROR_DETECTED      0x0080

#define FLUSH           0x000B
#define INPUT           0x0001
#define OUTPUT          0x0002

void ZeroWaitCount( void )
{
    DosQuerySysInfo( QSV_MS_COUNT, QSV_MS_COUNT, &MSecsAtZero, sizeof( MSecsAtZero ) );
}


unsigned WaitCount( void )
{
    ULONG  ulMsecs;

    DosQuerySysInfo( QSV_MS_COUNT, QSV_MS_COUNT, &ulMsecs, sizeof( ulMsecs ) );

    return( ( ulMsecs - MSecsAtZero ) / MILLISEC_PER_TICK );
}


void ClearCom( void )
{
    BYTE        command;
    ULONG       ulParmLen;
    USHORT      data;
    ULONG       ulDataLen;

    command   = 0;
    ulParmLen = sizeof( command );
    ulDataLen = sizeof( data );
    DosDevIOCtl( ComPort, IOCTL_GENERAL, DEV_FLUSHINPUT,
        &command,  sizeof( command ), &ulParmLen,
        &data,  sizeof( data ), &ulDataLen );
    command = 0;
    ulParmLen = sizeof( command );
    ulDataLen = sizeof( data );
    DosDevIOCtl( ComPort, IOCTL_GENERAL, DEV_FLUSHOUTPUT,
        &command,  sizeof( command ), &ulParmLen,
        &data,  sizeof( data ), &ulDataLen );
}

static void WaitTransmit( void )
{
    USHORT      event;
    ULONG       ulDataLen;

    do {
        ulDataLen = sizeof( event );
        DosDevIOCtl( ComPort, IOCTL_ASYNC, ASYNC_GETCOMMEVENT,
            NULL,  0, NULL,
            &event,  sizeof( event ), &ulDataLen );
    } while( ( event & LAST_CHAR_SENT ) == 0 );
}

void SendByte( int value )
{
    ULONG       written = 0;

    DosWrite( ComPort, (BYTE *)&value, 1, &written );
    if( !BlockTransmission )
        WaitTransmit();
}

void StartBlockTrans( void )
{
    BlockTransmission = TRUE;
}

void StopBlockTrans( void )
{
    if( BlockTransmission ) {
        BlockTransmission = FALSE;
        WaitTransmit();
    }
}

void _System Reader(ULONG arg)
{
    int        data;
    ULONG      read;
    int        new_index;
    APIRET     rc;

    OverRun = FALSE;
    for( ;; ) {
        rc = DosRead( ComPort, &data, 1, &read );
        if( rc != NO_ERROR )
            break;
        if( read == 1 ) {
            new_index = ReadBuffAdd + 1;
            new_index &= BSIZE-1;
            if( new_index == ReadBuffRemove ) {
                OverRun = TRUE;
            } else {
                ReadBuff[ ReadBuffAdd ] = data;
                ReadBuffAdd = new_index;
            }
            DosPostEventSem( ReadSemaphore );
        }
    }
}


int WaitByte( unsigned ticks )
{
    int         data;
    ULONG       ulPostCount;

    data = 0;
    if( ReadBuffAdd == ReadBuffRemove ) {
        if( DosWaitEventSem( ReadSemaphore, ticks * MILLISEC_PER_TICK ) != 0 ) {
            return( NO_DATA );
        }
    }
    DosResetEventSem( ReadSemaphore, &ulPostCount );
    data = ReadBuff[ ReadBuffRemove ];
    ReadBuffRemove++;
    ReadBuffRemove &= BSIZE-1;
    return( data );
}


int GetByte( void )
{
    return( WaitByte( 0 ) );
}


ULONG Rate[] = {
        115200,
        57600,
        38400,
        19200,
        9600,
        4800,
        2400,
        1200,
        0 };


bool Baud( int index )
{
    USHORT      temp;
    BYTE        lc[3];
    APIRET      rc;
    BYTE        command;
    ULONG       ulParmLen;

    if( index == MIN_BAUD )
        return( TRUE );
    if( index == CurrentBaud )
        return( TRUE );
    temp = Rate[index];

    command   = 0;
    ulParmLen = sizeof( temp );
    rc = DosDevIOCtl( ComPort, IOCTL_ASYNC, ASYNC_SETBAUDRATE,
        &temp,  sizeof( temp ), &ulParmLen,
        NULL,  0, NULL );
    if( rc != 0 ) {
        return( FALSE );
    }
    lc[ 0 ] = DATA_BITS_8;
    lc[ 1 ] = PARITY_NONE;
    lc[ 2 ] = STOP_BITS_1;
    ulParmLen = sizeof( lc );
    rc = DosDevIOCtl( ComPort, IOCTL_ASYNC, ASYNC_SETLINECTRL,
        &lc,  sizeof( lc ), &ulParmLen,
        NULL,  0, NULL );
    if( rc != 0 ) {
        return( FALSE );
    }
    CurrentBaud = index;
    return( TRUE );
}


char *ParsePortSpec( char * *spec )
{
    char        *parm;
    ULONG       action;
    char        port;
    static char name[] = "com?";

    parm = (spec == NULL) ? "" : *spec;

    port = '1';
    if( *parm >= '1' && *parm <= '9' ) {
        port = *parm++;
    }
    if( *parm != '\0' && *parm != '.' ) return( TRP_ERR_invalid_serial_port_number );
    if( spec != NULL ) *spec = parm;
    if( ComPort != 0 ) {
        DosClose( ComPort );
        ComPort = 0;
    }
    name[sizeof( name ) - 2] = port;
    if( DosOpen( (PSZ)name, &ComPort, &action, 0, 0, 1, 0x12, 0 ) ) {
        ComPort = 0;
        return( TRP_ERR_serial_port_not_available );
    }
    return( NULL );
}


void DonePort( void )
{
    if( ComPort != 0 ) {
//      DosClose( ComPort ); // Can't do this ... OS/2 blocks us forever
        ComPort = 0;
    }
}


bool CheckPendingError( void )
{
    bool        over_run;
    USHORT      event;
    ULONG       ulDataLen;

    ulDataLen = sizeof( event );
    DosDevIOCtl( ComPort, IOCTL_ASYNC, ASYNC_GETCOMMEVENT,
        NULL,  0, NULL,
        &event,  sizeof( event ), &ulDataLen );

    over_run = OverRun;
    OverRun = FALSE;
    return( over_run || ( event & ERROR_DETECTED ) );
}


void ClearLastChar( void )
{
}


void Wait( unsigned timer_ticks )
{
    unsigned wait_time;

    wait_time = WaitCount() + timer_ticks;
    while( WaitCount() < wait_time ) {
        DosSleep( MILLISEC_PER_TICK/2 );        /* half a timer tick */
    }
}


char *InitSys( void )
{
    APIRET      rc;

    if( MaxBaud == 0 ) {
        MaxBaud = 3; /* 19200 -- see table */
    }
    rc = DosCreateEventSem( NULL, &ReadSemaphore, 0, FALSE );
    if( rc != 0 )
        return( TRP_OS2_no_info );
    rc = DosSetPriority( PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, 0 );
    if( rc != 0 )
        return( TRP_OS2_cannot_set_thread_priority );
    ReadBuffAdd = 0;
    ReadBuffRemove = 0;
    rc = DosCreateThread( &ReaderId, Reader, 0,
        CREATE_READY, READER_STACKSIZE );
    if( rc != 0 )
        return( TRP_OS2_cannot_create_helper_thread );
    rc = DosSetPriority( PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, ReaderId );
    if( rc != 0 )
        return( TRP_OS2_cannot_set_thread_priority );
    CurrentBaud = -1;
    return( NULL );
}


void ResetSys( void )
{
    DosCloseEventSem( ReadSemaphore );
    ReadSemaphore = 0;
}
