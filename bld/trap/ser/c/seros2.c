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
* Description:  OS/2 serial port access functions.
*
****************************************************************************/


#include <dos.h>
#define INCL_DOSSEMAPHORES
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOSPROCESS
#define INCL_DOSINFOSEG
#define INCL_DOSERRORS
#define INCL_DOSMISC
#include <wos2.h>
#include "bool.h"
#include "serial.h"
#include "serlink.h"
#include "trpimp.h"
#include "trperr.h"

int                             CurrentBaud;
long                            MSecsAtZero;
HFILE                           ComPort = 0;
#ifdef _M_I86
GINFOSEG                        __far *GInfoSeg;
LINFOSEG                        __far *LInfoSeg;
#endif

#define BSIZE                   1024            /* must be power of 2 */
char                            ReadBuff[BSIZE];
int                             ReadBuffRemove;
int                             ReadBuffAdd;

#ifdef _M_I86
#define READER_STACKSIZE        2048
byte                            ReaderStack[READER_STACKSIZE];
ULONG                           ReadSemaphore;
#else
#define READER_STACKSIZE        8192
HEV                             ReadSemaphore;
#endif
TID                             ReaderId;
bool                            OverRun;
bool                            BlockTransmission;

extern int                      MaxBaud;

#define SERIAL          0x0001

#define SETLINECTRL     0x0042
#define DATA_BITS_8     0x08
#define STOP_BITS_1     0x00
#define PARITY_NONE     0x00

#define SETBAUDRATE     0x0041

#define GETCOMMEVENT    0x0072
#define LAST_CHAR_SENT  0x0004
#define CHAR_AVAILABLE  0x0001
#define ERROR_DETECTED  0x0080

#define FLUSH           0x000B
#define INPUT           0x0001
#define OUTPUT          0x0002

void ResetTimerTicks( void )
{
#ifdef _M_I86
    MSecsAtZero = GInfoSeg->msecs;
#else
    DosQuerySysInfo( QSV_MS_COUNT, QSV_MS_COUNT, &MSecsAtZero, sizeof( MSecsAtZero ) );
#endif
}


unsigned GetTimerTicks( void )
{
#ifdef _M_I86
    return( ( GInfoSeg->msecs - MSecsAtZero ) / MILLISEC_PER_TICK );
#else
    ULONG  ulMsecs;

    DosQuerySysInfo( QSV_MS_COUNT, QSV_MS_COUNT, &ulMsecs, sizeof( ulMsecs ) );
    return( ( ulMsecs - MSecsAtZero ) / MILLISEC_PER_TICK );
#endif
}


void ClearCom( void )
{
    BYTE        command;
#ifdef _M_I86

    command = 0;
    DosDevIOCtl( 0L, &command, INPUT, FLUSH, ComPort );
    command = 0;
    DosDevIOCtl( 0L, &command, OUTPUT, FLUSH, ComPort );
#else
    ULONG       ulParmLen;
    USHORT      data;
    ULONG       ulDataLen;

    command = 0;
    ulParmLen = sizeof( command );
    ulDataLen = sizeof( data );
    DosDevIOCtl( ComPort, IOCTL_GENERAL, DEV_FLUSHINPUT,
        &command, sizeof( command ), &ulParmLen,
        &data, sizeof( data ), &ulDataLen );
    command = 0;
    ulParmLen = sizeof( command );
    ulDataLen = sizeof( data );
    DosDevIOCtl( ComPort, IOCTL_GENERAL, DEV_FLUSHOUTPUT,
        &command, sizeof( command ), &ulParmLen,
        &data, sizeof( data ), &ulDataLen );
#endif
}

static void WaitTransmit( void )
{
    USHORT      event;

    do {
#ifdef _M_I86
        DosDevIOCtl( (PVOID)&event, (PVOID)0L, GETCOMMEVENT, SERIAL, ComPort );
#else
        ULONG       ulDataLen;

        ulDataLen = sizeof( event );
        DosDevIOCtl( ComPort, IOCTL_ASYNC, ASYNC_GETCOMMEVENT,
            NULL, 0, NULL, &event, sizeof( event ), &ulDataLen );
#endif
    } while( (event & LAST_CHAR_SENT) == 0 );
}

void SendByte( int value )
{
    OS_UINT     written = 0;

    DosWrite( ComPort, (BYTE *)&value, 1, &written );
    if( !BlockTransmission ) {
        WaitTransmit();
    }
}

void StartBlockTrans( void )
{
    BlockTransmission = true;
}

void StopBlockTrans( void )
{
    if( BlockTransmission ) {
        BlockTransmission = false;
        WaitTransmit();
    }
}

#ifdef _M_I86
static void FAR Reader( void )
#else
static void APIENTRY Reader( ULONG arg )
#endif
{
    int         data;
    OS_UINT     read;
    int         new_index;

#ifndef _M_I86
    /* unused parameters */ (void)arg;
#endif

    OverRun = false;
    for( ;; ) {
        if( DosRead( ComPort, &data, 1, &read ) )
            break;
        if( read == 1 ) {
            new_index = ReadBuffAdd + 1;
            new_index &= BSIZE - 1;
            if( new_index == ReadBuffRemove ) {
                OverRun = true;
            } else {
                ReadBuff[ReadBuffAdd] = data;
                ReadBuffAdd = new_index;
            }
#ifdef _M_I86
            DosSemClear( &ReadSemaphore );
#else
            DosPostEventSem( ReadSemaphore );
#endif
        }
    }
}


int WaitByte( unsigned ticks )
{
    int         data;
#ifndef _M_I86
    ULONG       ulPostCount;
#endif

    data = 0;
    if( ReadBuffAdd == ReadBuffRemove ) {
#ifdef _M_I86
        if( DosSemSetWait( &ReadSemaphore, ticks * MILLISEC_PER_TICK ) != 0 ) {
#else
        if( DosWaitEventSem( ReadSemaphore, ticks * MILLISEC_PER_TICK ) != 0 ) {
#endif
            return( SDATA_NO_DATA );
        }
    }
#ifndef _M_I86
    DosResetEventSem( ReadSemaphore, &ulPostCount );
#endif
    data = ReadBuff[ReadBuffRemove];
    ReadBuffRemove++;
    ReadBuffRemove &= BSIZE - 1;
    return( data );
}


int GetByte( void )
{
    return( WaitByte( 0 ) );
}


UINT Rate[] = {
#ifdef _M_I86
        0,              /* 16-bit OS/2 can't handle 115200 */
#else
        115200,
#endif
        57600,
        38400,
        19200,
        9600,
        4800,
        2400,
        1200,
        0
};


bool Baud( int index )
{
    USHORT      temp;
    BYTE        lc[3];
#ifndef _M_I86
    BYTE        command;
    ULONG       ulParmLen;
#endif

    if( index == MIN_BAUD )
        return( true );
    if( index == CurrentBaud )
        return( true );
    temp = Rate[index];
#ifdef _M_I86
    if( DosDevIOCtl( (PVOID)0L, (PVOID)&temp, SETBAUDRATE, SERIAL, ComPort ) )
#else
    command   = 0;
    ulParmLen = sizeof( temp );
    if( DosDevIOCtl( ComPort, IOCTL_ASYNC, ASYNC_SETBAUDRATE,
        &temp, sizeof( temp ), &ulParmLen, NULL, 0, NULL ) )
#endif
        return( false );
    lc[0] = DATA_BITS_8;
    lc[1] = PARITY_NONE;
    lc[2] = STOP_BITS_1;
#ifdef _M_I86
    if( DosDevIOCtl( (PVOID)0L, (PVOID)&lc, SETLINECTRL, SERIAL, ComPort ) )
#else
    ulParmLen = sizeof( lc );
    if( DosDevIOCtl( ComPort, IOCTL_ASYNC, ASYNC_SETLINECTRL,
        &lc, sizeof( lc ), &ulParmLen, NULL, 0, NULL ) )
#endif
        return( false );
    CurrentBaud = index;
    return( true );
}


char *ParsePortSpec( const char **spec )
{
    const char  *parm;
    OS_UINT     action;
    char        port;
    static char name[] = "com?";

    parm = (spec == NULL) ? "" : *spec;

    port = '1';
    if( *parm >= '1' && *parm <= '9' ) {
        port = *parm++;
    }
    if( *parm != '\0' && *parm != '.' )
        return( TRP_ERR_invalid_serial_port_number );
    if( spec != NULL )
        *spec = parm;
    if( ComPort != 0 ) {
        DosClose( ComPort );
        ComPort = 0;
    }
    name[sizeof( name ) - 2] = port;
    if( DosOpen( (PSZ)name, &ComPort, &action, 0,
            FILE_NORMAL,
            OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
            OPEN_SHARE_DENYREADWRITE | OPEN_ACCESS_READWRITE,
            0 ) ) {
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
    USHORT      event;
    bool        over_run;
#ifdef _M_I86

    DosDevIOCtl( (PVOID)&event, (PVOID)0L, GETCOMMEVENT, SERIAL, ComPort );
#else
    ULONG       ulDataLen;

    ulDataLen = sizeof( event );
    DosDevIOCtl( ComPort, IOCTL_ASYNC, ASYNC_GETCOMMEVENT,
        NULL, 0, NULL, &event, sizeof( event ), &ulDataLen );
#endif
    over_run = OverRun;
    OverRun = false;
    return( over_run || (event & ERROR_DETECTED) );
}


void ClearLastChar( void )
{
}


void Wait( unsigned timer_ticks )
{
    unsigned wait_time;

    wait_time = GetTimerTicks() + timer_ticks;
    while( GetTimerTicks() < wait_time ) {
        DosSleep( MILLISEC_PER_TICK / 2 );      /* half a timer tick */
    }
}


char *InitSys( void )
{
#ifdef _M_I86
    SEL         sel_global;
    SEL         sel_local;
#endif

    if( MaxBaud == 0 ) {
        MaxBaud = 3; /* 19200 -- see table */
    }
#ifdef _M_I86
    if( DosGetInfoSeg( &sel_global, &sel_local ) )
#else
    if( DosCreateEventSem( NULL, &ReadSemaphore, 0, false ) )
#endif
        return( TRP_OS2_no_info );
#ifdef _M_I86
    GInfoSeg = _MK_FP( sel_global, 0 );
    LInfoSeg = _MK_FP( sel_local, 0 );
    if( DosSetPrty( PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, LInfoSeg->tidCurrent ) )
#else
    if( DosSetPriority( PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, 0 ) )
#endif
        return( TRP_OS2_cannot_set_thread_priority );
    ReadBuffAdd = 0;
    ReadBuffRemove = 0;
#ifdef _M_I86
    ReadSemaphore = 0;
    if( DosCreateThread( Reader, &ReaderId, ReaderStack + READER_STACKSIZE ) )
#else
    if( DosCreateThread( &ReaderId, Reader, 0, CREATE_READY, READER_STACKSIZE ) )
#endif
        return( TRP_OS2_cannot_create_helper_thread );
#ifdef _M_I86
    if( DosSetPrty( PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, ReaderId ) )
#else
    if( DosSetPriority( PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, ReaderId ) )
#endif
        return( TRP_OS2_cannot_set_thread_priority );
    CurrentBaud = -1;
    return( NULL );
}


void ResetSys( void )
{
#ifndef _M_I86
    DosCloseEventSem( ReadSemaphore );
    ReadSemaphore = 0;
#endif
}
