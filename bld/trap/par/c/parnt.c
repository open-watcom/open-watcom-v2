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
#include "trpimpxx.h"
#include "parlink.h"


static HANDLE   PortHdl = INVALID_HANDLE_VALUE;

#define NUM_ELTS( a )   (sizeof( a ) / sizeof( a[0] ))

static unsigned short PortTest[] = { 0x378, 0x3bc, 0x278 };
static unsigned short PortAddress[NUM_ELTS( PortTest )];
static unsigned PortsFound = 0;

/* Forward declarations */
void FiniSys( void );

bool TRAPENTRY TrapTellTerminate( void )
{
        FiniSys();
        return( TRUE );
}

int NumPrinters( void )
{
    return( PortsFound );
}

unsigned PrnAddress( int printer )
{
    return( PortAddress[ printer ] );
}

unsigned AccessPorts( unsigned first, unsigned last )
{
        // We have direct I/O port access
        return( 1 );
}

void FreePorts( unsigned first, unsigned last )
{
    first = first;
    last = last;
}

static int CheckForPort( int i, unsigned char value )
{
    int         j;

    outp( PortTest[ i ], value );
    for( j = 100; j != 0; j-- )
        ;
    return( inp( PortTest[ i ] ) == value );
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
                if ( PortHdl == INVALID_HANDLE_VALUE )
                        return( TRP_ERR_cannot_access_parallel_ports );
        }
        PortsFound = 0;
        for( i = 0; i < NUM_ELTS( PortTest ); ++i ) {
                if( CheckForPort( i, 0x55 ) && CheckForPort( i, 0xaa ) ) {
                        PortAddress[ PortsFound++ ] = PortTest[ i ];
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
