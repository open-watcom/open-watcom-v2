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


#include <string.h>
#include "dbgdefn.h"
#include "dbgmem.h"
#include "dbgio.h"
#include "digtypes.h"
#include "digcli.h"
#include "trptypes.h"
#include "remcore.h"
#include "posixfp.h"


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

static obj_attrs DIG2WVOpenMode( dig_open mode )
{
    obj_attrs   oattrs;

    oattrs = 0;
    if( mode & DIG_READ ) {
        oattrs |= OP_READ;
    }
    if( mode & DIG_WRITE ) {
        oattrs |= OP_WRITE;
    }
    if( mode & DIG_CREATE ) {
        oattrs |= OP_CREATE;
    }
    if( mode & DIG_TRUNC ) {
        oattrs |= OP_TRUNC;
    }
    if( mode & DIG_APPEND ) {
        oattrs |= OP_APPEND;
    }
    if( mode & DIG_REMOTE ) {
        oattrs |= OP_REMOTE;
    }
    if( mode & DIG_LOCAL ) {
        oattrs |= OP_LOCAL;
    }
    if( mode & DIG_SEARCH ) {
        oattrs |= OP_SEARCH;
    }
    return( oattrs );
}

/*
 * Debugger use internaly file I/O based on "int" handles
 * like POSIX but it is index into array which hold real
 * OS file handles.
 * Therefore DIGCLIENTRY I/O function must be based on
 * these functions and can not use ISO stream I/O functions
 * and handles are managed as POSIX handle
 * Appropriate macros for file handle mapping must be used
 */
FILE * DIGCLIENTRY( Open )( char const *name, dig_open mode )
{
    file_handle fh;

    fh = FileOpen( name, DIG2WVOpenMode( mode ) );
    if( fh == NIL_HANDLE )
        return( NULL );
    return( POSIX2FP( fh ) );
}

int DIGCLIENTRY( Seek )( FILE *fp, unsigned long p, dig_seek k )
{
    return( SeekStream( FP2POSIX( fp ), p, k ) == ERR_SEEK );
}

unsigned long DIGCLIENTRY( Tell )( FILE *fp )
{
    return( SeekStream( FP2POSIX( fp ), 0, DIO_SEEK_CUR ) );
}

size_t DIGCLIENTRY( Read )( FILE *fp, void *b , size_t s )
{
    return( ReadStream( FP2POSIX( fp ), b, s ) );
}

size_t DIGCLIENTRY( Write )( FILE *fp, const void *b, size_t s )
{
    return( WriteStream( FP2POSIX( fp ), b, s ) );
}

void DIGCLIENTRY( Close )( FILE *fp )
{
    FileClose( FP2POSIX( fp ) );
}

void DIGCLIENTRY( Remove )( char const *name, dig_open mode )
{
    FileRemove( name, DIG2WVOpenMode( mode ) );
}

unsigned DIGCLIENTRY( MachineData )( address addr, dig_info_type info_type,
                        dig_elen in_size,  const void *in,
                        dig_elen out_size, void *out )
{
    return( RemoteMachineData( addr, (uint_8)info_type, in_size, in, out_size, out ) );
}
