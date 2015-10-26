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
* Description:  Module to implement direct parallel port access under
*               Linux.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <unistd.h>


#define NUM_ELTS( a )   (sizeof( a ) / sizeof( a[0] ))

static unsigned short PortTest[] = { 0x378, 0x3bc, 0x278 };
static unsigned short PortAddress[NUM_ELTS( PortTest )];
static unsigned PortsFound = 0;

int NumPrinters()
{
    return( PortsFound );
}

unsigned PrnAddress( int printer )
{
    return( PortAddress[ printer ] );
}

unsigned AccessPorts( unsigned first, unsigned last )
{
    return ioperm(first,last-first+1,1) == 0;
}

void FreePorts( unsigned first, unsigned last )
{
    ioperm(first,last-first,0);
}

static int CheckForPort( int i, unsigned char value )
{
    int         j;

    outp( PortTest[ i ], value );
    for( j = 100; j != 0; j-- )
        ;
    return( inp( PortTest[ i ] ) == value );
}

char *InitSys()
{
    int i;

    PortsFound = 0;
    for( i = 0; i < NUM_ELTS( PortTest ); ++i ) {
        if (!AccessPorts(PortTest[i], PortTest[i])) {
            printf("Failed to get I/O permissions. This program must run as root!\n");
            exit(-1);
            }
        if( CheckForPort( i, 0x55 ) && CheckForPort( i, 0xaa ) ) {
            PortAddress[ PortsFound++ ] = PortTest[ i ];
        }
        FreePorts(PortTest[i], PortTest[i]);
    }
    return( NULL );
}

void FiniSys()
{
}

unsigned long Ticks()
{
    return clock() / (CLOCKS_PER_SEC / 10);
}

