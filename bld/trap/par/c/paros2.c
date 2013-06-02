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


#include <stddef.h>
#define INCL_DOSINFOSEG
#include <os2.h>
#include "i86.h"


extern unsigned far output_port( unsigned, unsigned );
extern unsigned far input_port( unsigned );

extern unsigned pascal far DosDevConfig( char far *, unsigned short,
                                         unsigned short );

extern unsigned short pascal far DosPortAccess( unsigned short reserverd,
                                                unsigned short req_release,
                                                unsigned short first_port,
                                                unsigned short last_port );

#define NUM_ELTS( a )   (sizeof( a ) / sizeof( a[0] ))

GINFOSEG                                far *GInfoSeg;

unsigned short PortTest[] = {
        0x378, 0x3bc, 0x278
};

unsigned short PortAddress[NUM_ELTS( PortTest )] = {
        0,0,0
};

unsigned PortsFound;

int NumPrinters( void )
{
    char                num_printers;
    unsigned short      rc;

    rc = DosDevConfig( &num_printers, 0, 0 );
    if( rc != 0 ) return( 0 );
    if( num_printers > PortsFound ) num_printers = PortsFound;
    return( num_printers );
}

unsigned AccessPorts( unsigned first, unsigned last )
{
    DosPortAccess( 0, 0, first, last );
    return( 1 );
}

void FreePorts( unsigned first, unsigned last )
{
    DosPortAccess( 0, 1, first, last );
}

static int CheckForPort( int i, unsigned char value )
{
    int         j;

    output_port( PortTest[ i ], value );
    for( j = 100; j != 0; j-- )
        ;
    return( input_port( PortTest[ i ] ) == value );
}


char *InitSys( void )
{
    SEL         global;
    SEL         local;
    int         i;

    DosGetInfoSeg( &global, &local );
    GInfoSeg = MK_FP( global, 0 );

    PortsFound = 0;
    for( i = 0; i < NUM_ELTS( PortTest ); ++i ) {
        AccessPorts( PortTest[i], PortTest[i] );
        if( CheckForPort( i, 0x55 ) && CheckForPort( i, 0xaa ) ) {
            PortAddress[ PortsFound++ ] = PortTest[ i ];
        }
        FreePorts( PortTest[i], PortTest[i] );
    }
    return( 0 );
}


void FiniSys( void )
{
}


unsigned long Ticks( void )
{
    return( ( GInfoSeg->msecs ) / 100 );
}


unsigned PrnAddress( int printer )
{
    return( PortAddress[ printer ] );
}
