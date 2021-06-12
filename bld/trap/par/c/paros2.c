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
* Description:  OS/2 specific parallel port access routines.
*
****************************************************************************/


#include <stddef.h>
#define INCL_DOSINFOSEG
#define INCL_DOSDEVICES
#define INCL_DOSPROCESS
#define INCL_DOSMISC
#include <wos2.h>
#include "i86.h"
#include "parlink.h"
#include "portio.h"


#define NUM_ELTS( a )   (sizeof( a ) / sizeof( a[0] ))

#ifdef _M_I86
GINFOSEG    __far *GInfoSeg;
#endif

USHORT      PortTest[] = {
    0x378, 0x3bc, 0x278
};

USHORT      PortAddress[NUM_ELTS( PortTest )] = {
    0, 0, 0
};

USHORT      PortsFound;

int NumPrinters( void )
{
    char                num_printers;
    APIRET              rc;

#ifdef _M_I86
    rc = DosDevConfig( &num_printers, 0, 0 );
#else
    rc = DosDevConfig( &num_printers, DEVINFO_PRINTER );
#endif
    if( rc != 0 )
        return( 0 );
    if( num_printers > PortsFound )
        num_printers = PortsFound;
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
#ifdef _M_I86
    int         j;
#endif

    output_port( PortTest[i], value );
#ifdef _M_I86
    for( j = 100; j != 0; j-- )
        ;
#else
    DosSleep(1);
#endif
    return( input_port( PortTest[i] ) == value );
}


char *InitSys( void )
{
    int         i;
#ifdef _M_I86
    SEL         sel_global;
    SEL         sel_local;

    DosGetInfoSeg( &sel_global, &sel_local );
    GInfoSeg = _MK_FP( sel_global, 0 );
#endif
    PortsFound = 0;
    for( i = 0; i < NUM_ELTS( PortTest ); ++i ) {
        AccessPorts( PortTest[i], PortTest[i] );
        if( CheckForPort( i, 0x55 ) && CheckForPort( i, 0xaa ) ) {
            PortAddress[PortsFound++] = PortTest[i];
        }
        FreePorts( PortTest[i], PortTest[i] );
    }
    return( 0 );
}


void FiniSys( void )
{
}


ULONG Ticks( void )
{
#ifdef _M_I86
    return( GInfoSeg->msecs / 100 );
#else
    ULONG  ulMsecs;

    DosQuerySysInfo( QSV_MS_COUNT, QSV_MS_COUNT, &ulMsecs, sizeof( ulMsecs ) );
    return( ulMsecs / 100 );
#endif
}


unsigned PrnAddress( int printer )
{
    return( PortAddress[printer] );
}
