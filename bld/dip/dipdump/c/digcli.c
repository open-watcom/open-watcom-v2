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
* Description:  Minimal implementation of DIG client routines.
*
****************************************************************************/


#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "digtypes.h"
#include "digcli.h"

#if 0
# define dprintf(a)     do { printf a; } while( 0 )
#else
# define dprintf(a)     do {} while( 0 )
#endif

/*
 * dig_fhandle can be pointer to file structure or handle number
 * therefore 0/NULL is reserved for errors
 * if handle number is used then handle must be 1 based
 */
#define PH2DFH(sh)  (dig_fhandle)(pointer_int)((sh) + 1)
#define DFH2PH(dfh) ((int)(pointer_int)(dfh) - 1)

void *DIGCLIENTRY( Alloc )( size_t amount )
{
    void    *ptr = malloc( amount );

    dprintf(( "DIGCliAlloc: amount=%#x -> %p\n", (unsigned)amount, ptr ));
    return( ptr );
}

void *DIGCLIENTRY( Realloc )( void *p, size_t amount )
{
    void    *ptr = realloc( p, amount);

    dprintf(( "DIGCliRealloc: p=%p amount=%3x -> %p\n", p, (unsigned)amount, ptr ));
    return( ptr );
}

void DIGCLIENTRY( Free )( void *p )
{
    dprintf(( "DIGCliFree: p=%p\n", p ));
    free( p );
}

dig_fhandle DIGCLIENTRY( Open )( char const *name, dig_open mode )
{
    int     fd;
    int     flgs;

    dprintf(( "DIGCliOpen: name=%p:{%s} mode=%#x\n", name, name, mode ));

    /* convert flags. */
    switch( mode & (DIG_READ | DIG_WRITE) ) {
    case DIG_READ:
        flgs = O_RDONLY;
        break;
    case DIG_WRITE:
        flgs = O_WRONLY;
        break;
    case DIG_WRITE | DIG_READ:
        flgs = O_RDWR;
        break;
    default:
        return( DIG_NIL_HANDLE );
    }
#ifdef O_BINARY
    flgs |= O_BINARY;
#endif
    if( mode & DIG_CREATE )
        flgs |= O_CREAT;
    if( mode & DIG_TRUNC )
        flgs |= O_TRUNC;
    if( mode & DIG_APPEND )
        flgs |= O_APPEND;
    /* (ignore the remaining flags) */

    fd = open( name, flgs, PMODE_RWX );

    dprintf(( "DIGCliOpen: returns %d\n", fd ));
    if( fd == -1 )
        return( DIG_NIL_HANDLE );
    return( PH2DFH( fd ) );
}

unsigned long DIGCLIENTRY( Seek )( dig_fhandle dfh, unsigned long p, dig_seek k )
{
    int     whence;
    long    off;

    switch( k ) {
    case DIG_ORG:   whence = SEEK_SET; break;
    case DIG_CUR:   whence = SEEK_CUR; break;
    case DIG_END:   whence = SEEK_END; break;
    default:
        dprintf(( "DIGCliSeek: h=%d p=%ld k=%d -> -1\n", DFH2PH( dfh ), p, k ));
        return( DIG_SEEK_ERROR );
    }

    off = lseek( DFH2PH( dfh ), p, whence );
    dprintf(( "DIGCliSeek: h=%d p=%ld k=%d -> %ld\n", DFH2PH( dfh ), p, k, off ));
    return( off );
}

size_t DIGCLIENTRY( Read )( dig_fhandle dfh, void *b , size_t s )
{
    size_t      rc;
#ifdef _WIN64
    unsigned    read_len;
    unsigned    amount;

    amount = INT_MAX;
    rc = 0;
    while( s > 0 ) {
        if( amount > s )
            amount = (unsigned)s;
        read_len = read( DFH2PH( dfh ), b, amount );
        if( read_len == (unsigned)-1 ) {
            rc = DIG_RW_ERROR;
            break;
        }
        rc += read_len;
        if( read_len != amount ) {
            break;
        }
        buffer = (char *)b + amount;
        s -= amount;
    }
#else
    rc = read( (int)h, b, s );
#endif

    dprintf(( "DIGCliRead: h=%d b=%p s=%d -> %d\n", DFH2PH( dfh ), b, (unsigned)s, (unsigned)rc ));
    return( rc );
}

size_t DIGCLIENTRY( Write )( dig_fhandle dfh, const void *b, size_t s )
{
    size_t      rc;
#ifdef _WIN64
    unsigned    write_len;
    unsigned    amount;

    amount = INT_MAX;
    rc = 0;
    while( s > 0 ) {
        if( amount > s )
            amount = (unsigned)s;
        write_len = write( DFH2PH( dfh ), b, amount );
        if( write_len == (unsigned)-1 ) {
            rc = DIG_RW_ERROR;
            break;
        }
        rc += write_len;
        if( write_len != amount ) {
            break;
        }
        buffer = (char *)b + amount;
        s -= amount;
    }
#else
    rc = write( DFH2PH( dfh ), b, s );
#endif

    dprintf(( "DIGCliWrite: h=%d b=%p s=%d -> %d\n", DFH2PH( dfh ), b, (unsigned)s, (unsigned)rc ));
    return( rc );
}

void DIGCLIENTRY( Close )( dig_fhandle dfh )
{
    dprintf(( "DIGCliClose: h=%d\n", DFH2PH( dfh ) ));
    if( close( DFH2PH( dfh ) ) ) {
        dprintf(( "DIGCliClose: h=%d failed!!\n", DFH2PH( dfh ) ));
    }
}

void DIGCLIENTRY( Remove )( char const *name, dig_open mode )
{
    dprintf(( "DIGCliRemove: name=%p:{%s} mode=%#x\n", name, name, mode ));
    unlink( name );
    mode = mode;
}

unsigned DIGCLIENTRY( MachineData )( address addr, dig_info_type info_type,
                        dig_elen in_size,  const void *in,
                        dig_elen out_size, void *out )
{
    dprintf(( "DIGCliMachineData: \n" ));
    return( 0 ); /// @todo check this out out.
}
