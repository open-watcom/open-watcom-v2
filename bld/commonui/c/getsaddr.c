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
* Description:  Figure out start address of a NE module.
*
****************************************************************************/


#include "commonui.h"
#include "bool.h"
#include "tinyio.h"
#include "getsaddr.h"

#define EXE_NE  0x454e
#define EXE_MZ  0x5a4d

/*
 * SeekRead - seek to a specified spot in the file, and read some data
 */
static bool SeekRead( int handle, DWORD newpos, void *buff, WORD size )
{
    tiny_ret_t  rc;

    if( TINY_ERROR( TinySeek( handle, newpos, TIO_SEEK_SET ) ) ) {
        return( false );
    }
    rc = TinyRead( handle, buff, size );
    if( TINY_ERROR( rc ) ) {
        return( false );
    }
    if( TINY_INFO( rc ) != size ) {
        return( false );
    }
    return( true );

} /* SeekRead */

/*
 * FindNewHeader - get a pointer to the new executable header
 */
static bool FindNewHeader( int handle, DWORD *nh_offset )
{
    WORD        data;

    if( !SeekRead( handle, 0x00, &data, sizeof( data ) ) ) {
        return( false );
    }
    if( data != EXE_MZ ) {
        return( false );
    }

    if( !SeekRead( handle, 0x18, &data, sizeof( data ) ) ) {
        return( false );
    }
    if( data < 0x40 ) {
        return( false );
    }

    if( !SeekRead( handle, 0x3c, nh_offset, sizeof( DWORD ) ) ) {
        return( false );
    }

    if( !SeekRead( handle, *nh_offset, &data, sizeof( WORD ) ) ) {
        return( false );
    }
    if( data != EXE_NE ) {
        return( false );
    }

    return( true );

} /* FindNewHeader */

/*
 * GetStartAddress - find the segment/offset of the first
 *                   instruction in a given executable
 */
bool GetStartAddress( char *path, addr48_ptr *res )
{
    tiny_ret_t  rc;
    int         handle;
    DWORD       nh_offset;
    WORD        ip;
    WORD        object;

    rc = TinyOpen( path, TIO_READ );
    if( TINY_ERROR( rc ) ) {
        return( false );
    }
    handle = TINY_INFO( rc );

    if( !FindNewHeader( handle, &nh_offset ) ) {
        TinyClose( handle );
        return( false );
    }
    if( !SeekRead( handle, nh_offset + 0x14, &ip, sizeof( ip ) ) ) {
        TinyClose( handle );
        return( false );
    }
    if( !SeekRead( handle, nh_offset + 0x16, &object, sizeof( object ) ) ) {
        TinyClose( handle );
        return( false );
    }
    res->offset = (DWORD)ip;
    res->segment = object;

    TinyClose( handle );
    return( true );

} /* GetStartAddress */
