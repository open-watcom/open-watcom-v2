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


#include <os.h>
#include <debug.h>
#include <stddef.h>
#include <limits.h>
#include "os.h"
#include "service.h"
#include "servmgr.h"
#include "instlmgr.h"
#include "sio.h"
#include "serial.h"
#include "dbgdefn.h"


#define MILLISEC_PER_TICK               (55)
OS_MILLISECONDS                         MSecsAtZero;
OBJECT                                  theSioPort;
char                                    BlockBuff[300];
int                                     BlockIndex;
extern int                              MaxBaud;
int                                     CurrentBaud;
bool                                    HadError;
OS_SEMA_ID                              SerialInputSema;
OS_SEMA_ID                              SerialOutputSema;
unsigned                                SyncSlop = SEC(1)/4;

void PutNum( long i )
{
    DPrintf( "%d\n", i );
}

void ZeroWaitCount()
{
    MSecsAtZero = OSPowerUpTime();
}


unsigned WaitCount()
{
    return( ( OSPowerUpTime() - MSecsAtZero ) / MILLISEC_PER_TICK );
}


void ClearCom()
{
    SIO_INPUT_BUFFER_STATUS     ibs;
    SIO_OUTPUT_BUFFER_STATUS    obs;

    ObjCallWarn( msgStreamFlush, theSioPort, 0 );
    ObjCallWarn( msgSioOutputBufferStatus, theSioPort, &obs );
    ObjCallWarn( msgSioOutputBufferStatus, theSioPort, &ibs );
    if( obs.bufferChars || ibs.bufferChars ) {
        Debugf( "ClearCom of output (%d) input(%d)",
                obs.bufferChars, ibs.bufferChars );
    }
    ObjCallWarn( msgSioInputBufferFlush, theSioPort, 0 );
    ObjCallWarn( msgSioOutputBufferFlush, theSioPort, 0 );
}


void SerialWrite( char *buff, int len )
{
    STREAM_READ_WRITE           rw;
    SIO_OUTPUT_BUFFER_STATUS    obs;

    rw.numBytes = len;
    rw.pBuf = buff;
    ObjCallWarn( msgStreamWrite, theSioPort, &rw );
    ObjCallWarn( msgStreamFlush, theSioPort, &rw );
    ObjCallWarn( msgSioOutputBufferStatus, theSioPort, &obs );
    if( obs.bufferChars != 0 ) { // wait to finish
        Debugf( "Wait for output to finish" );
        OSSemaWait( SerialOutputSema, osInfiniteTime );
        OSSemaSet( SerialOutputSema );
    }
}


void SendByte( int value )
{

    //Debugf( "SendByte( %d )", value );
    if( BlockIndex >= 0 ) {
        BlockBuff[BlockIndex++] = value;
    } else {
        SerialWrite( (char *)&value, 1 );
    }
}

void StartBlockTrans()
{
    //Debugf( "StartBlockTrans" );
    BlockIndex = 0;
}

void StopBlockTrans()
{
    //Debugf( "StopBlockTrans" );
    if( BlockIndex > 0 ) {
        SerialWrite( &BlockBuff, BlockIndex );
    }
    BlockIndex = -1;
}


int GetByte()
{
    STREAM_READ_WRITE           rw;
    char                        data;
    STATUS                      s;

    rw.numBytes = 1;
    rw.pBuf = &data;
    s = ObjCallWarn( msgStreamRead, theSioPort, &rw );
    if( s == stsOK ) {
        //Debugf( "GetByte %d", data );
        return( data );
    }
    HadError = TRUE;
    return( NO_DATA );
}


int WaitByte( unsigned ticks )
{
    STREAM_READ_WRITE_TIMEOUT   rwt;
    char                        data;
    STATUS                      s;

    rwt.numBytes = 1;
    rwt.pBuf = &data;
    rwt.timeOut = ticks*MILLISEC_PER_TICK;
    s = ObjectCall( msgStreamReadTimeOut, theSioPort, &rwt );
    if( s == stsOK ) {
        return( data );
    }
    return( NO_DATA );
#if 0
    STATUS                      s;
    SIO_INPUT_BUFFER_STATUS     ibs;

    ObjCallWarn( msgSioInputBufferStatus, theSioPort, &ibs );
    if( ibs.bufferChars != 0 ) return( GetByte() );
    s = OSSemaWait( SerialInputSema, ticks*MILLISEC_PER_TICK );
    ticks=ticks;
    OSSemaSet( SerialInputSema );
    if( s == stsOK ) return( GetByte() );
    return( NO_DATA );
#endif
}


U32 Rate[] = {
        0,
        0,
        38400,
        19200,
        9600,
        4800,
        2400,
        1200,
        0 };


bool Baud( int index )
{
    U32         temp;
    STATUS      s;

    if( index == MIN_BAUD ) return( TRUE );
    if( index == CurrentBaud ) return( TRUE );
    temp = Rate[ index ];
    if( temp == 0 ) return( FALSE );

    s = ObjCallWarn( msgStreamFlush, theSioPort, 0 );
    if( s != stsOK ) return( FALSE );

    s = ObjCallWarn( msgSioInputBufferFlush, theSioPort, 0 );
    if( s != stsOK ) return( FALSE );

    s = ObjCallWarn( msgSioOutputBufferFlush, theSioPort, 0 );
    if( s != stsOK ) return( FALSE );

    s = ObjCallWarn( msgSioBaudSet, theSioPort, (P_ARGS)temp );
    if( s != stsOK ) return( FALSE );

    CurrentBaud = index;
    Debugf( "Baud(%d)", Rate[ CurrentBaud ] );
    return( TRUE );
}


char *ParsePortSpec( char * *spec )
{
    spec=spec;
    return( NULL );
}


void DonePort()
{
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

    if( timer_ticks < 0 ) {
        Debugf( "Wait(%d)", timer_ticks );
        return;
    }

    wait_time = WaitCount() + timer_ticks;
    while( WaitCount() < wait_time ) OSTaskDelay( 0 );
}


#if 0
STATUS PrintList( UID what, char *wn )
{
    STATUS              s;
    LIST                list;
    LIST_ENTRY          le;
    IM_GET_SET_NAME     name;
    U16                 n;
    char                buff[80];

    Debugf( "Trying to get the list '%s' %8.8x", wn, what );

    ObjCallRet( msgIMGetList, what, &list, s );
    ObjCallRet(msgListNumItems, list, &n, s );
    for( le.position = 0; le.position < n ; ++le.position ) {

        ObjCallRet( msgListGetItem, list, &le, s );
        if( s != stsOK ) break;
        name.handle = le.item;
        name.pName = buff;
        ObjCallRet( msgIMGetName, what, &name, s );
        Debugf( "Got name '%s'", buff );
    }
    return( stsOK );
}

#define pp( x ) PrintList( x, #x );

pp( theMILDevices );
pp( theParallelDevices );
pp( theAppleTalkDevices );
pp( theSerialDevices );
pp( thePrinterDevices );
pp( thePrinters );
pp( theSendableServices );
pp( theTransportHandlers );
pp( theLinkHandlers );
pp( theHWXEngines );
pp( theModems );
pp( theHighSpeedPacketHandlers );
#endif

STATUS  InitCommunications( OBJECT self )
{
#if 0
    STATUS              s;
    IM_FIND             imf;
    SM_BIND             sm;
    SM_OPEN             so;
    SM_SET_OWNER        sw;
    SIO_INIT            si;
    SIO_LINE_CONTROL_SET sl;
    SIO_FLOW_CONTROL_SET sf;
    SIO_EVENT_SET       es;
    SIO_METRICS         smt;

    Debugf( "Trying to get COM1" );
    imf.pName = "COM1";
    ObjCallRet( msgIMFind, theSerialDevices, &imf, s );

    Debugf( "Found handle %8.8x", imf.handle );

    sm.handle = imf.handle;
    sm.caller = self;

    Debugf( "Bind" );
    ObjCallRet( msgSMBind, theSerialDevices, &sm, s );

    Debugf( "Get ownership" );
    sw.handle = imf.handle;
    sw.owner = self;
    ObjCallRet( msgSMSetOwner, theSerialDevices, &sw, s );

    so.handle = imf.handle;
    so.caller = self;
    Debugf( "Open" );
    ObjCallRet( msgSMOpen,  theSerialDevices, &so, s );

    theSioPort = so.service;
    Debugf( "Got serial port id %8.8x", theSioPort );

    Debugf( "msgSioInit" );
    si.inputSize = 512;
    si.outputSize = 512;
    ObjCallRet( msgSioInit, theSioPort, &si, s );

    Debugf( "msgSioLineControlSet" );
    sl.dataBits = sioEightBits;
    sl.stopBits = sioOneStopBit;
    sl.parity = sioNoParity;
    ObjCallRet( msgSioLineControlSet, theSioPort, &sl, s );

    Debugf( "msgSioFlowControlSet" );
    sf.flowControl = sioTxNone | sioRxNone;
    ObjCallRet( msgSioFlowControlSet, theSioPort, &sf, s );

    Debugf( "msgSioBaudSet (%d)", Rate[LOW_BAUD] );
    ObjCallRet( msgSioBaudSet, theSioPort, (P_ARGS)Rate[ LOW_BAUD ], s );

    OSSemaCreate( &SerialInputSema );
    OSSemaSet( SerialInputSema );
    OSSemaCreate( &SerialOutputSema );
    OSSemaSet( SerialOutputSema );
    es.eventMask = sioEventTxBufferEmpty | sioEventRxChar | sioEventRxError;

    es.client = self;
    ObjCallRet( msgSioEventSet, theSioPort, &es, s );

    ObjCallRet( msgSioGetMetrics, theSioPort, &smt, s );
    Debugf( "Initialized %d baud, %d in, %d out", smt.baud,
            smt.bufferSize.inputSize, smt.bufferSize.outputSize );

    return( stsOK );
#endif
    STATUS              s;
    SM_BIND             sm;
    SM_OPEN             so;
    SM_SET_OWNER        sw;
    LIST                list;
    LIST_ENTRY          le;
    IM_GET_SET_NAME     name;
    U16                 n;
    SIO_INIT            si;
    SIO_LINE_CONTROL_SET sl;
    SIO_FLOW_CONTROL_SET sf;
    SIO_EVENT_SET       es;
    SIO_METRICS         smt;
    char                buff[80];


    ObjectCall( msgIMGetList, theSerialDevices, &list );
    ObjectCall( msgListNumItems, list, &n );

    le.position = 0;

    ObjCallRet( msgListGetItem, list, &le, s );
    name.handle = le.item;
    name.pName = buff;

    ObjCallRet( msgIMGetName, theSerialDevices, &name, s );
    Debugf( "Got serial device '%s'", buff );

    sm.handle = le.item;
    sm.caller = self;

    Debugf( "Bind" );
    ObjCallRet( msgSMBind, theSerialDevices, &sm, s );

    Debugf( "Get ownership" );
    sw.handle = le.item;
    sw.owner = self;
    ObjCallRet( msgSMSetOwner, theSerialDevices, &sw, s );

    Debugf( "Open it" );
    so.handle = le.item;
    so.caller = self;
    ObjCallRet( msgSMOpen,  theSerialDevices, &so, s );

    theSioPort = so.service;
    Debugf( "Got parallel port id %8.8x", theSioPort );

    Debugf( "msgSioInit" );
    si.inputSize = 512;
    si.outputSize = 512;
    ObjCallRet( msgSioInit, theSioPort, &si, s );

    Debugf( "msgSioLineControlSet" );
    sl.dataBits = sioEightBits;
    sl.stopBits = sioOneStopBit;
    sl.parity = sioNoParity;
    ObjCallRet( msgSioLineControlSet, theSioPort, &sl, s );

    Debugf( "msgSioFlowControlSet" );
    sf.flowControl = sioTxNone | sioRxNone;
    ObjCallRet( msgSioFlowControlSet, theSioPort, &sf, s );

    Debugf( "msgSioBaudSet (%d)", Rate[LOW_BAUD] );
    ObjCallRet( msgSioBaudSet, theSioPort, (P_ARGS)Rate[ LOW_BAUD ], s );

    OSSemaCreate( &SerialInputSema );
    OSSemaSet( SerialInputSema );
    OSSemaCreate( &SerialOutputSema );
    OSSemaSet( SerialOutputSema );
    es.eventMask = sioEventTxBufferEmpty | sioEventRxChar | sioEventRxError;

    es.client = self;
    ObjCallRet( msgSioEventSet, theSioPort, &es, s );

    ObjCallRet( msgSioGetMetrics, theSioPort, &smt, s );
    Debugf( "Initialized %d baud, %d in, %d out", smt.baud,
            smt.bufferSize.inputSize, smt.bufferSize.outputSize );

    return( stsOK );
}

MsgHandlerWithTypes( SerialEventNotify, P_SIO_EVENT_HAPPENED, P_IDATA )
{
    if( pArgs->eventMask & sioEventTxBufferEmpty ) {
        //Debugf( "Output buffer empty" );
        OSSemaReset( SerialOutputSema );
    }
    if( pArgs->eventMask & sioEventRxChar ) {
        //Debugf( "Input buffer non-empty" );
        OSSemaReset( SerialInputSema );
    }
    if( pArgs->eventMask & sioEventRxError ) {
        //Debugf( "ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR" );
    }
    return stsOK;

    MsgHandlerParametersNoWarning;
}

char *InitSys( void )
{
    MaxBaud = 0;
    CurrentBaud = -1;
    BlockIndex = -1;
//  OSTaskPrioritySet( OSThisTask(), osThisTaskOnly, osHighPriority, 40 );
    return( NULL );
}



void ResetSys( void )
{
}
