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
* Description:  OS/2 1.x serial port interface.
*
****************************************************************************/


#include <stddef.h>
#include <dos.h>
#define INCL_DOSSEMAPHORES
#define INCL_DOSDEVICES
#define INCL_DOSPROCESS
#define INCL_DOSINFOSEG
#include <os2.h>
#include "serial.h"
#include "trpimp.h"
#include "trperr.h"

int                                     CurrentBaud;
long                                    MSecsAtZero;
GINFOSEG                                far *GInfoSeg;
LINFOSEG                                far *LInfoSeg;
HFILE                                   ComPort = 0;

#define BSIZE   1024                    /* must be power of 2 */
char                                    ReadBuff[BSIZE];
int                                     ReadBuffRemove;
int                                     ReadBuffAdd;

#define READER_STACKSIZE                2048
char                                    ReaderStack[READER_STACKSIZE];
TID                                     ReaderId;
ULONG                                   ReadSemaphore;
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
    MSecsAtZero = GInfoSeg->msecs;
}


unsigned WaitCount( void )
{
    return( ( GInfoSeg->msecs - MSecsAtZero ) / MILLISEC_PER_TICK );
}


void ClearCom( void )
{
    BYTE        command;

    command = 0;
    DosDevIOCtl( 0L, &command, INPUT, FLUSH, ComPort );
    command = 0;
    DosDevIOCtl( 0L, &command, OUTPUT, FLUSH, ComPort );
}

static void WaitTransmit( void )
{
    USHORT      event;

    do {
        DosDevIOCtl( (PVOID)&event, (PVOID)0L, GETCOMMEVENT, SERIAL, ComPort );
    } while( ( event & LAST_CHAR_SENT ) == 0 );
}

void SendByte( int value )
{
    USHORT      written = 0;

    DosWrite( ComPort, (BYTE *)&value, 1, &written );
    if( !BlockTransmission ) WaitTransmit();
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

void far Reader( void )
{
    int         data;
    USHORT      read;
    int         new_index;

    OverRun = FALSE;
    for( ;; ) {
        DosRead( ComPort, &data, 1, &read );
        if( read == 1 ) {
            new_index = ReadBuffAdd + 1;
            new_index &= BSIZE-1;
            if( new_index == ReadBuffRemove ) {
                OverRun = TRUE;
            } else {
                ReadBuff[ ReadBuffAdd ] = data;
                ReadBuffAdd = new_index;
            }
            DosSemClear( &ReadSemaphore );
        }
    }
}


int WaitByte( unsigned ticks )
{
    int         data;

    data = 0;
    if( ReadBuffAdd == ReadBuffRemove ) {
        if( DosSemSetWait( &ReadSemaphore, ticks*MILLISEC_PER_TICK ) != 0 ) {
            return( NO_DATA );
        }
    }
    data = ReadBuff[ ReadBuffRemove ];
    ReadBuffRemove++;
    ReadBuffRemove &= BSIZE-1;
    return( data );
}


int GetByte( void )
{
    return( WaitByte( 0 ) );
}


USHORT Rate[] = {
        0,              /* os2 can't handle 115200 */
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
    USHORT      rc;

    if( index == MIN_BAUD ) return( TRUE );
    if( index == CurrentBaud ) return( TRUE );
    temp = Rate[index];
    rc = DosDevIOCtl( (PVOID)0L, (PVOID)&temp, SETBAUDRATE, SERIAL, ComPort );
    if( rc != 0 ) {
        return( FALSE );
    }
    lc[ 0 ] = DATA_BITS_8;
    lc[ 1 ] = PARITY_NONE;
    lc[ 2 ] = STOP_BITS_1;
    rc = DosDevIOCtl( (PVOID)0L, (PVOID)&lc, SETLINECTRL, SERIAL, ComPort );
    if( rc != 0 ) {
        return( FALSE );
    }
    CurrentBaud = index;
    return( TRUE );
}


char *ParsePortSpec( char * *spec )
{
    char    *parm;
    USHORT      action;
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
    USHORT      event;
    bool        over_run;

    DosDevIOCtl( (PVOID)&event, (PVOID)0L, GETCOMMEVENT, SERIAL, ComPort );
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
    SEL         global;
    SEL         local;
    USHORT      rc;

    if( MaxBaud == 0 ) {
        MaxBaud = 3; /* 19200 -- see table */
    }
    rc = DosGetInfoSeg( &global, &local );
    if( rc != 0 ) return( TRP_OS2_no_info );
    GInfoSeg = MK_FP( global, 0 );
    LInfoSeg = MK_FP( local, 0 );
    rc = DosSetPrty( PRTYS_THREAD, PRTYC_TIMECRITICAL,
                     0, LInfoSeg->tidCurrent );
    if( rc != 0 ) return( TRP_OS2_cannot_set_thread_priority );
    ReadBuffAdd = 0;
    ReadBuffRemove = 0;
    ReadSemaphore = 0;
    rc = DosCreateThread( Reader, &ReaderId, ReaderStack+READER_STACKSIZE );
    if( rc != 0 ) return( TRP_OS2_cannot_create_helper_thread );
    rc = DosSetPrty( PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, ReaderId );
    if( rc != 0 ) return( TRP_OS2_cannot_set_thread_priority );
    CurrentBaud = -1;
    return( NULL );
}


void ResetSys( void )
{
}
