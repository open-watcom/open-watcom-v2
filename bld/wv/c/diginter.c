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


#include "dbgdefn.h"
#include "dbgmem.h"
#include "machtype.h"
#include "dbgmem.h"
#include "dbgio.h"
#include "digtypes.h"
#include "digcli.h"

extern unsigned         RemoteMachineData( address addr, unsigned, unsigned, const void *, unsigned, void * );

void *DIGCLIENT DIGCliAlloc( unsigned amount )
{
    void        *p;

    _Alloc( p, amount );
    return( p );
}

void *DIGCLIENT DIGCliRealloc( void *p, unsigned amount )
{
    _Realloc( p, amount );
    return( p );
}

void DIGCLIENT DIGCliFree( void *p )
{
    _Free( p );
}

dig_fhandle DIGCLIENT DIGCliOpen( char const *name, dig_open mode )
{
    return( FileOpen( name, mode ) );
}

unsigned long DIGCLIENT DIGCliSeek( dig_fhandle h, unsigned long p,
                                dig_seek k )
{
    return( SeekStream( h, p, k ) );
}

unsigned DIGCLIENT DIGCliRead( dig_fhandle h, void *b , unsigned s )
{
    return( ReadStream( h, b, s ) );
}

unsigned DIGCLIENT DIGCliWrite( dig_fhandle h, const void *b, unsigned s )
{
    return( WriteStream( h, b, s ) );
}

void DIGCLIENT DIGCliClose( dig_fhandle h )
{
    FileClose( h );
}

void DIGCLIENT DIGCliRemove( char const *name, dig_open mode )
{
    FileRemove( name, mode );
}

unsigned DIGCLIENT DIGCliMachineData( address addr, unsigned info_type,
                        unsigned in_size,  const void *in,
                        unsigned out_size, void *out )
{
    return( RemoteMachineData( addr, info_type, in_size, in, out_size, out ) );
}
