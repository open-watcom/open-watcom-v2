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
#include "dbgio.h"
#include "digtypes.h"
#include "digcli.h"
#include "trptypes.h"
#include "remcore.h"


void *DIGCLIENTRY( Alloc )( size_t amount )
{
    void        *p;

    _Alloc( p, amount );
    return( p );
}

void *DIGCLIENTRY( Realloc )( void *p, size_t amount )
{
    _Realloc( p, amount );
    return( p );
}

void DIGCLIENTRY( Free )( void *p )
{
    _Free( p );
}

dig_fhandle DIGCLIENTRY( Open )( char const *name, dig_open mode )
{
    file_handle fh;

    fh = FileOpen( name, mode );
    if( fh == NIL_HANDLE )
        return( DIG_NIL_HANDLE );
    return( (dig_fhandle)fh );
}

unsigned long DIGCLIENTRY( Seek )( dig_fhandle dfh, unsigned long p, dig_seek k )
{
    return( SeekStream( (file_handle)dfh, p, k ) );
}

size_t DIGCLIENTRY( Read )( dig_fhandle dfh, void *b , size_t s )
{
    return( ReadStream( (file_handle)dfh, b, s ) );
}

size_t DIGCLIENTRY( Write )( dig_fhandle dfh, const void *b, size_t s )
{
    return( WriteStream( (file_handle)dfh, b, s ) );
}

void DIGCLIENTRY( Close )( dig_fhandle dfh )
{
    FileClose( (file_handle)dfh );
}

void DIGCLIENTRY( Remove )( char const *name, dig_open mode )
{
    FileRemove( name, mode );
}

unsigned DIGCLIENTRY( MachineData )( address addr, dig_info_type info_type,
                        dig_elen in_size,  const void *in,
                        dig_elen out_size, void *out )
{
    return( RemoteMachineData( addr, (uint_8)info_type, in_size, in, out_size, out ) );
}
