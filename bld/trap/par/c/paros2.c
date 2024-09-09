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
* Description:  OS/2 specific parallel port access routines.
*
****************************************************************************/


#include <stddef.h>
#define INCL_DOSINFOSEG
#define INCL_DOSDEVICES
#define INCL_DOSPROCESS
#define INCL_DOSMISC
#include <wos2.h>
#include <i86.h>
#include "bool.h"
#include "parlink.h"
#include "portio.h"
#include "parfind.h"


#ifdef _M_I86
GINFOSEG    __far *GInfoSeg;
#endif

static unsigned short   PortTest[] = { PORT_ADDRESSES };
static unsigned short   PortAddress[ACOUNT( PortTest )] = { 0 };
static int              PortsFound = 0;

int NumPrinters( void )
{
    return( PortsFound );
}

bool AccessPorts( unsigned first, unsigned count )
{
    if( count > 0 )
        return( DosPortAccess( 0, 0, first, first + count - 1 ) == 0 );
    return( true );
}

void FreePorts( unsigned first, unsigned count )
{
    if( count > 0 ) {
        DosPortAccess( 0, 1, first, first + count - 1 );
    }
}

static int CheckForPort( int i, unsigned char value )
{
#ifdef _M_I86
    int         j;
#endif

    output_port( PortTest[i], value );
#ifdef _M_I86
    for( j = 100; j > 0; j-- )
        {}
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
    for( i = 0; i < ACOUNT( PortTest ); ++i ) {
        if( AccessPorts( PortTest[i], 1 ) ) {
            if( CheckForPort( i, 0x55 ) && CheckForPort( i, 0xaa ) ) {
                PortAddress[PortsFound++] = PortTest[i];
            }
            FreePorts( PortTest[i], 1 );
        }
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
