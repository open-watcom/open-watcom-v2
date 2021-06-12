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
* Description:  XMS management routines for DOS.
*
****************************************************************************/


#include "vi.h"
#include <stddef.h>

#if defined( USE_XMS )

#include <i86.h>
#include "dosx.h"
#include "xmem.h"
#include "fcbmem.h"


#define XMS_IO_ERROR    ((size_t)-1)

xms_struct              XMSCtrl;
static xhandle          *xmsPtrs;

static size_t   xmsRead( xhandle, void __far *, size_t );
static size_t   xmsWrite( xhandle, void __far *, size_t );

vi_rc XMSBlockTest( unsigned short blocks )
{
    if( !XMSCtrl.inuse ) {
        return( ERR_NO_XMS_MEMORY );
    }
    if( XMSBlocksInUse + blocks > TotalXMSBlocks ) {
        return( ERR_NO_XMS_MEMORY );
    }
    return( ERR_NO_ERR );

} /* XMSBlockTest */

void XMSBlockRead( xhandle addr, void __far *buff, size_t len )
{
    xmsRead( addr, buff, len );

} /* XMSBlockRead */

void XMSBlockWrite( xhandle addr, void __far *buff, size_t len )
{
    xmsWrite( addr, buff, len );

} /* XMSBlockWrite */

vi_rc XMSGetBlock( xhandle *addr )
{
    vi_rc       rc;
    xhandle     found;
    int         i;

    rc = XMSBlockTest( 1 );
    if( rc == ERR_NO_ERR ) {
        XMSBlocksInUse++;
        found = 0;
        for( i = 0; i < TotalXMSBlocks; i++ ) {
            if( xmsPtrs[i] != 0 ) {
                found = xmsPtrs[i];
                xmsPtrs[i] = 0;
                break;
            }
        }
        *addr = found;
    }
    return( rc );

} /* XMSGetBlock */

/*
 * SwapToXMSMemory - move an fcb to extended memory from memory
 */
vi_rc SwapToXMSMemory( fcb *fb )
{
    vi_rc       rc;
    size_t      len;
    xhandle     found;

    found = 0;
    rc = XMSGetBlock( &found );
    if( rc == ERR_NO_ERR ) {
        len = MakeWriteBlock( fb );
        xmsWrite( found, WriteBuffer, len );

        /*
         * finish up
         */
        fb->xblock.handle = found;
        fb->in_xms_memory = true;
    }
    return( rc );

} /* SwapToXMSMemory */

/*
 * SwapToMemoryFromXMSMemory - bring data back from extended memory
 */
vi_rc SwapToMemoryFromXMSMemory( fcb *fb )
{
    size_t  len;

    len = FcbSize( fb );
    xmsRead( fb->xblock.handle, ReadBuffer, len );
    GiveBackXMSBlock( fb->xblock.handle );
    return( RestoreToNormalMemory( fb, len ) );

} /* SwapToMemoryFromXMSMemory */

static void *xmsControl;

/*
 * xmsAlloc - allocate some xms memory
 */
static xhandle xmsAlloc( int size )
{
    xms_addr            h;
    unsigned            handle, new_size, page_request;
    unsigned long       offset;

    if( XMSCtrl.exhausted ) {
        return( 0 );
    }

    size = ROUNDUP( size, 4 );
    if( XMSCtrl.offset + size > XMSCtrl.size ) {
        if( XMSCtrl.small_block ) {
            return( 0 );
        }

        /* align offset to 1k boundary */
        offset = ROUNDUP( XMSCtrl.offset, 0x400 );
        if( offset < XMSCtrl.size ) {
            /* reallocate the block to eliminate internal fragmentation */
            new_size = offset / 0x0400;
            _XMSReallocate( &xmsControl, XMSCtrl.handles[XMSCtrl.next_handle - 1], new_size );
        }

        page_request = XMS_MAX_BLOCK_SIZE_IN_K;
        for( ;; ) {
            if( _XMSAllocate( &xmsControl, page_request, (unsigned short *)&handle ) != 0 ) {
                break;
            }
            XMSCtrl.small_block = true;
            page_request -= XMS_BLOCK_ADJUST_SIZE_IN_K;
            if( page_request == 0 ) {
                XMSCtrl.exhausted = true;
                return( 0 );
            }
        }

        XMSCtrl.offset = 0;
        XMSCtrl.size = page_request * 0x0400L;
        XMSCtrl.handles[XMSCtrl.next_handle++] = handle;
        if( XMSCtrl.offset + size > XMSCtrl.size ) {
            return( 0 );
        }
    }

    h.internal.offset = XMSCtrl.offset >> 2;
    h.internal.handle = XMSCtrl.handles[XMSCtrl.next_handle - 1];

    XMSCtrl.offset += size;

    return( h.external );

} /* xmsAlloc */

/*
 * XMSInit - start up xms services
 */
void XMSInit( void )
{
    unsigned    size;
    int         i;
    xms_addr    h;

    XMSCtrl.inuse = false;
    if( !EditFlags.ExtendedMemory ) {
        return;
    }
    XMSCtrl.exhausted = false;
    XMSCtrl.next_handle = 0;
    XMSCtrl.small_block = false;
    if( _XMSInstalled() != XMS_INSTALLED ) {
        return;
    }

    xmsControl = _XMSControl();
    if( _XMSVersion( &xmsControl ) < XMS_REQD_VERSION ) {
        return;
    }
    if( _XMSOK( &xmsControl ) & XMS_ERROR ) {
        return;
    }

    size = _XMSQuerySize( &xmsControl );
    if( size == 0 ) {
        return;
    }

    i = _XMSRequestHMA( &xmsControl, XMS_APPLICATION_AMT );
    if( i ) {
        XMSCtrl.handles[XMSCtrl.next_handle++] = XMS_HMA_HANDLE;
        XMSCtrl.offset = XMS_HMA_INITIAL_OFFSET;
        XMSCtrl.size = XMS_HMA_BLOCK_SIZE;
    } else {
        XMSCtrl.offset = XMS_MAX_BLOCK_SIZE;
        XMSCtrl.size = XMS_MAX_BLOCK_SIZE;
    }

    xmsPtrs = _MemAllocArray( xhandle, EditVars.MaxXMSBlocks );

    for( i = 0; i < EditVars.MaxXMSBlocks; i++ ) {
        xmsPtrs[i] = xmsAlloc( MAX_IO_BUFFER );
        if( xmsPtrs[i] == 0 ) {
            break;
        }
        h.external = xmsPtrs[i];
        TotalXMSBlocks++;
    }
    xmsPtrs = realloc( xmsPtrs, TotalXMSBlocks * sizeof( xhandle ) );
    if( xmsPtrs == NULL ) {
        return;
    }

    XMSCtrl.inuse = true;

} /* XMSInit */

/*
 * XMSFini - close down XMS services
 */
void XMSFini( void )
{
    unsigned    handle;

    if( !XMSCtrl.inuse ) {
        return;
    }
    for( ; XMSCtrl.next_handle > 0; XMSCtrl.next_handle-- ) {
        handle = XMSCtrl.handles[XMSCtrl.next_handle - 1];
        if( handle == XMS_HMA_HANDLE ) {
            if( _XMSReleaseHMA( &xmsControl ) == 0 ) {
                break;
            }
        } else {
            if( _XMSFree( &xmsControl, handle ) == 0 ) {
                break;
            }
        }
    }
    XMSCtrl.inuse = false;

} /* XMSFini */

/*
 * xmsRead - read from XMS memory
 */
static size_t xmsRead( xhandle addr, void __far *buff, size_t size )
{
    xms_addr            h;
    unsigned            offset;
    xms_move_descriptor control;
    void                *dest;

    if( addr == 0 ) {
        return( XMS_IO_ERROR );
    }
    size = ROUNDUP( size, 2 );

    h.external = addr;
    offset = h.internal.offset << 2;

    if( h.internal.handle == XMS_HMA_HANDLE ) {
        dest = _MK_FP( XMS_HMA_SEGMENT, offset );
        if( _XMSEnableA20( &xmsControl ) == 0 ) {
            return( XMS_IO_ERROR );
        }
        _XMSCopyWords( dest, buff, size >> 1 );
        if( _XMSDisableA20( &xmsControl ) == 0 ) {
            return( XMS_IO_ERROR );
        }
    } else {
        control.size = size;
        control.dest_handle = XMS_REAL_HANDLE;
        control.dest_offset.real = buff;
        control.src_handle = h.internal.handle;
        control.src_offset.extended = offset;
        if( _XMSMove( &xmsControl, &control ) == 0 ) {
            return( XMS_IO_ERROR );
        }
    }
    return( size );

} /* xmsRead */

/*
 * xmsWrite - write some XMS memory
 */
static size_t xmsWrite( xhandle addr, void __far *buff, size_t size )
{
    xms_addr            h;
    xms_move_descriptor control;
    unsigned            offset;
    void                *dest;

    if( addr == 0 ) {
        return( XMS_IO_ERROR );
    }
    size = ROUNDUP( size, 2 );
    h.external = addr;
    offset = h.internal.offset << 2;
    if( h.internal.handle == XMS_HMA_HANDLE ) {

        dest = _MK_FP( XMS_HMA_SEGMENT, offset );
        if( _XMSEnableA20( &xmsControl ) == 0 ) {
            return( XMS_IO_ERROR );
        }
        _XMSCopyWords( buff, dest, size >> 1 );
        if( _XMSDisableA20( &xmsControl ) == 0 ) {
            return( XMS_IO_ERROR );
        }

    } else {

        control.size = size;
        control.src_handle = XMS_REAL_HANDLE;
        control.src_offset.real = buff;
        control.dest_handle = h.internal.handle;
        control.dest_offset.extended = offset;
        if( _XMSMove( &xmsControl, &control ) == 0 ) {
            return( XMS_IO_ERROR );
        }

    }
    return( size );

} /* xmsWrite */

/*
 * GiveBackXMSBlock - return an XMS block to the pool
 */
void GiveBackXMSBlock( xhandle addr )
{
    int i;

    for( i = 0; i < TotalXMSBlocks; i++ ) {
        if( xmsPtrs[i] == 0 ) {
            xmsPtrs[i] = addr;
            break;
        }
    }
    XMSBlocksInUse--;

} /* GiveBackXMSBlock */

/*
 * XMSBlockInit
 */
void XMSBlockInit( int i )
{
    if( XMSCtrl.inuse ) {
        return;
    }
    EditVars.MaxXMSBlocks = i;
    EditVars.MaxXMSBlocks /= (MAX_IO_BUFFER / 1024);

} /* XMSBlockInit */

#endif
