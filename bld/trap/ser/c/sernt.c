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
* Description:  Win32 Serial handling routines.
*
****************************************************************************/


#include <windows.h>
#include <stdio.h>
#include "trpimp.h"
#include "trperr.h"
#include "serial.h"

#define BREAK_TIME      4       // In DOS timer ticks, I think (55ms)

int ErrorFlag;
int BreakFlag;
static int currentBaudRateIndex;
static DWORD lastTickReset;

static HANDLE hSerial = INVALID_HANDLE_VALUE;
static int comPortNumber = 1;
static char comPortName[64];

//////////////////////////////////////////////////////////////////////////
// Read and write caches to reduce the number of ReadFile and WriteFile
// calls which have to occur.
//
static BYTE writeCache[1024];
static size_t writeCacheLevel;
static bool bBlockWriteMode;

static BYTE readCache[1024];
static size_t readCacheIndex;
static DWORD readCacheLevel;


void Trace(const char* fmt, ...)
{
    va_list         va;
    static char     traceBuffer[1000];

    va_start( va, fmt );

    vsprintf( traceBuffer, fmt, va );
    OutputDebugString( traceBuffer );

    va_end( va );
}

void ZeroWaitCount( void )
{
    lastTickReset = GetTickCount();
}


unsigned WaitCount( void )
{
    return( (GetTickCount() - lastTickReset) / 55 );
}


char *InitSys( void )
{
    DCB             devCon;
    char            deviceFileName[64];
    COMMTIMEOUTS    timeouts = { MAXDWORD, MAXDWORD, 1, 0, 0 };

    if( comPortNumber )
        sprintf( deviceFileName, "\\\\.\\COM%d", comPortNumber );
    else
        strncpy( deviceFileName, comPortName, sizeof( deviceFileName ) );

    Trace( "InitSys: '%s'\n", deviceFileName );

    currentBaudRateIndex = -1;

    ZeroWaitCount();

    hSerial = CreateFile( deviceFileName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0L,
        NULL );

    if( INVALID_HANDLE_VALUE == hSerial ) {
        Trace( "InitSys: CreateFile failed '%s'\n", deviceFileName );
        return NULL;
    }

    // Set up comm parameters if it's a real port and not a pipe
    if( comPortNumber ) {
        // Set up a big RX buffer
        if( !SetupComm( hSerial, 1000, 1000 ) ) {
            // This odd circumstance seems to occur if the port has been
            // assigned to a printer
            Trace( "InitSys: Setupcom failed '%s'\n", deviceFileName );
            CloseHandle( hSerial );
            hSerial = INVALID_HANDLE_VALUE;
            return NULL;
        }
        
        // Configure the serial port
        GetCommState(hSerial, &devCon);
        
        devCon.BaudRate = 9600;
        devCon.ByteSize = 8;
        devCon.Parity = NOPARITY;
        devCon.StopBits = ONESTOPBIT;
        devCon.fParity = FALSE;
        
        devCon.fDsrSensitivity = FALSE;
        devCon.fDtrControl = FALSE;
        devCon.fRtsControl = RTS_CONTROL_DISABLE;
        devCon.fOutxCtsFlow = FALSE;
        devCon.fOutxDsrFlow = FALSE;
        devCon.fInX = FALSE;
        devCon.fOutX = FALSE;
        SetCommState(hSerial, &devCon);
        
        SetCommTimeouts(hSerial, &timeouts);
        
        EscapeCommFunction(hSerial, SETDTR);
        EscapeCommFunction(hSerial, SETRTS);
    }

    return( NULL );
}


void ResetSys( void )
{
    if(hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle( hSerial );
        hSerial = INVALID_HANDLE_VALUE;
    }
}

bool Terminate( void )
{
    ResetSys();
    return( TRUE );
}

static void FlushWriteCache( void )
{
    if( writeCacheLevel > 0 ) {
        DWORD nBytesWritten;
        WriteFile( hSerial, writeCache, writeCacheLevel, &nBytesWritten, NULL );
        writeCacheLevel = 0;
    }
}

static void InsertWriteCacheByte( BYTE newByte )
{
    if( writeCacheLevel >= sizeof( writeCache) ) {
        FlushWriteCache();
    }
    writeCache[writeCacheLevel++] = newByte;
}

void SendByte( int value )
{
    InsertWriteCacheByte(value);
    if( !bBlockWriteMode ) {
        FlushWriteCache();
    }

    //  Trace("Ser: Wrote 0x%x\n", outByte);
}

void StartBlockTrans( void )
{
    bBlockWriteMode = TRUE;
}

void StopBlockTrans( void )
{
    bBlockWriteMode = FALSE;
    FlushWriteCache();
}


int GetByte( void )
{
    if( readCacheIndex < readCacheLevel ) {
        return readCache[readCacheIndex++];
    } else {
        // Cache is empty
        readCacheIndex = readCacheLevel = 0;
        if( ReadFile( hSerial, readCache, sizeof( readCache ), &readCacheLevel, NULL ) ) {
            if( readCacheLevel > 0) {
                return readCache[readCacheIndex++];
            }
        }
    }
    return -1;
}


void ClearCom( void )
{
    writeCacheLevel = 0;
}


void SendABreak( void )
{
    EscapeCommFunction( hSerial, SETBREAK );
    Sleep( BREAK_TIME * 55 );
    EscapeCommFunction( hSerial, CLRBREAK );
}

bool TestForBreak( void )
{
    DWORD       errors;
    COMSTAT     comStat;

    if( ClearCommError( hSerial, &errors, &comStat ) ) {
        if( errors & CE_BREAK ) {
            return TRUE;
        }
    }
    return( FALSE );
}


int Divisor[] = { 1, 2, 3, 6, 12, 24, 48, 96, 0 };

bool Baud( int index )
{
    DCB devCon;

    ErrorFlag = 0;
    BreakFlag = 0;
    if( index == MIN_BAUD ) {
        Trace("Ser: Modem flag set\n");
        return( TRUE );
    }

    if( index == currentBaudRateIndex ) return( TRUE );

    GetCommState(hSerial, &devCon);
    devCon.BaudRate = 115200 / Divisor[index];
    Trace("Ser: Baud set: %d\n", devCon.BaudRate);
    SetCommState(hSerial, &devCon);

    currentBaudRateIndex = index;

    return( TRUE );
}

char *ParsePortSpec( const char **spec )
{
    comPortNumber = 1;
    if( spec != NULL ) {
        char ch = **spec;

        if( ch >= '1' && ch <= '9' ) {
            comPortNumber = ch - '0';
            ch = *++*spec;
        } else if( ch == '\\' ) {
            char    *d;

            comPortNumber = 0;
            d = comPortName;
            while( **spec )
                *d++ = *(*spec)++;
            ch = '\0';
        }
        if( ch != '\0' && ch != '.' )
            return( TRP_ERR_invalid_serial_port_number );
    }
    return( NULL );
}


void DonePort( void )
{
}


bool CheckPendingError( void )
{
    int old_error;

    old_error = ErrorFlag;
    ErrorFlag = 0;
    return( old_error != 0 );
}


void ClearLastChar( void )
{
    // Wait for the output buffer to empty
    FlushWriteCache();
    for( ;; ) {
        DWORD       errors;
        COMSTAT     comStat;

        if( !ClearCommError( hSerial, &errors, &comStat ) ) {
            break;
        }
        if( comStat.cbOutQue == 0 ) {
            break;
        }
        Sleep( 0 );
    }
}
