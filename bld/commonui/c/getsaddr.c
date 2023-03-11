/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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
#include "exedos.h"
#include "getsaddr.h"


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
static bool FindNEHeader( int handle, DWORD *ne_header_off )
{
    WORD        data;

    if( !SeekRead( handle, 0, &data, sizeof( data ) )
      || data != EXESIGN_DOS ) {
        return( false );
    }

    if( !SeekRead( handle, DOS_RELOC_OFFSET, &data, sizeof( data ) )
      || !NE_HEADER_FOLLOWS( data ) ) {
        return( false );
    }

    if( !SeekRead( handle, NE_HEADER_OFFSET, ne_header_off, sizeof( *ne_header_off ) )
      || !SeekRead( handle, *ne_header_off, &data, sizeof( data ) )
      || data != EXESIGN_NE ) {
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
    DWORD       ne_header_off;
    WORD        ip;
    WORD        object;

    rc = TinyOpen( path, TIO_READ );
    if( TINY_ERROR( rc ) ) {
        return( false );
    }
    handle = TINY_INFO( rc );

    if( !FindNEHeader( handle, &ne_header_off ) ) {
        TinyClose( handle );
        return( false );
    }
    if( !SeekRead( handle, ne_header_off + 0x14, &ip, sizeof( ip ) ) ) {
        TinyClose( handle );
        return( false );
    }
    if( !SeekRead( handle, ne_header_off + 0x16, &object, sizeof( object ) ) ) {
        TinyClose( handle );
        return( false );
    }
    res->offset = (DWORD)ip;
    res->segment = object;

    TinyClose( handle );
    return( true );

} /* GetStartAddress */
