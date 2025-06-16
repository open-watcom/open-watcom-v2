/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Parallel port debugging support for Win32.
*
****************************************************************************/


#include <windows.h>
#include <stddef.h>
#include <time.h>
#include <winioctl.h>
#include <conio.h>
#include "bool.h"
#include "dbgioctl.h"
#include "trperr.h"
#include "trpimp.h"
#include "parlink.h"
#include "parfind.h"


static HANDLE   PortHdl = INVALID_HANDLE_VALUE;

static unsigned short   PortTest[] = { PORT_ADDRESSES };
static unsigned short   PortAddress[ACOUNT( PortTest )] = { 0 };
static int              PortsFound = 0;

#if 0
/* Forward declarations */
void FiniSys( void );

bool Terminate( void )
{
    FiniSys();
    return( true );
}
#endif

int NumPrinters( void )
{
    return( PortsFound );
}

unsigned PrnAddress( int printer )
{
    return( PortAddress[printer] );
}

bool AccessPorts( unsigned first, unsigned count )
{
    /* unused parameters */ (void)first; (void)count;

    /*
     * We have direct I/O port access
     */
    return( true );
}

void FreePorts( unsigned first, unsigned count )
{
    /* unused parameters */ (void)first; (void)count;
}

static int CheckForPort( int i, unsigned char value )
{
    int         j;

    outp( PortTest[i], value );
    for( j = 100; j > 0; j-- )
        {}
    return( inp( PortTest[i] ) == value );
}

char *InitSys( void )
{
    static char name[] = "\\\\.\\DBGPORT1";
    int         i;

    if( !(GetVersion() & 0x80000000) ) {
        PortHdl = CreateFile( name,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                NULL
                                );
        if ( PortHdl == INVALID_HANDLE_VALUE ) {
            return( TRP_ERR_cannot_access_parallel_ports );
        }
    }
    PortsFound = 0;
    for( i = 0; i < ACOUNT( PortTest ); ++i ) {
        if( CheckForPort( i, 0x55 ) && CheckForPort( i, 0xaa ) ) {
            PortAddress[PortsFound++] = PortTest[i];
        }
    }
    return( NULL );
}

void FiniSys( void )
{
    if( PortHdl != INVALID_HANDLE_VALUE ) {
        CloseHandle( PortHdl );
        PortHdl = INVALID_HANDLE_VALUE;
    }
}

unsigned long Ticks( void )
{
    return( GetTickCount() / 100 );
}

// DBGPORT_IO data;
// data.port = DATAPORT;
// data.data.u8 = 0;
//DeviceIoControl( PortHdl, IOCTL_DBG_READ_PORT_U8, NULL, 0, &data, sizeof( data ), &ret_size, NULL );
// data.port = DATAPORT;
// data.data.u8 = 0xFF;
//DeviceIoControl( PortHdl, IOCTL_DBG_WRITE_PORT_U8, &data, sizeof( data ), NULL, 0, &ret_size, NULL );
// data.len =
// data.buffer =
//DeviceIoControl( PortHdl, IOCTL_DBG_REMOTE_GET,
