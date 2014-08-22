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


#include "vi.h"
#include <stddef.h>

#ifndef NOXMS

#include "dosx.h"
#include "xmem.h"
#include "fcbmem.h"

xms_struct              XMSCtrl;
static unsigned long    *xmsPtrs;

static int  xmsRead( long, void *, int );
static int  xmsWrite( long, void *, int );

int XMSBlockTest( unsigned short blocks )
{
    if( !XMSCtrl.inuse ) {
        return( ERR_NO_XMS_MEMORY );
    }
    if( XMSBlocksInUse + blocks > TotalXMSBlocks ) {
        return( ERR_NO_XMS_MEMORY );
    }
    return( ERR_NO_ERR );

} /* XMSBlockTest */

void XMSBlockRead( long addr, void *buff, unsigned len )
{
    xmsRead( addr, buff, len );

} /* XMSBlockRead */

void XMSBlockWrite( long addr, void *buff, unsigned len )
{
    xmsWrite( addr, buff, len );

} /* XMSBlockWrite */

int XMSGetBlock( long *addr )
{
    int         i;
    long        found = NULL;

    i = XMSBlockTest( 1 );
    if( i ) {
        return( i );
    }
    XMSBlocksInUse++;
    for( i = 0; i < TotalXMSBlocks; i++ ) {
        if( xmsPtrs[i] != NULL ) {
            found = xmsPtrs[i];
            xmsPtrs[i] = NULL;
            break;
        }
    }
    *addr = found;
    return( ERR_NO_ERR );

} /* XMSGetBlock */

/*
 * SwapToXMSMemory - move an fcb to extended memory from memory
 */
int SwapToXMSMemory( fcb *fb )
{
    int         i, len;
    long        found = NULL;

    i = XMSGetBlock( &found );
    if( i ) {
        return( i );
    }
    len = MakeWriteBlock( fb );
    xmsWrite( found, WriteBuffer, len );

    /*
     * finish up
     */
    fb->xmemaddr = found;
    fb->in_xms_memory = true;
    return( ERR_NO_ERR );

} /* SwapToXMSMemory */

/*
 * SwapToMemoryFromXMSMemory - bring data back from extended memory
 */
int SwapToMemoryFromXMSMemory( fcb *fb )
{
    int len;

    len = FcbSize( fb );
    xmsRead( fb->xmemaddr, ReadBuffer, len );
    GiveBackXMSBlock( fb->xmemaddr );
    return( RestoreToNormalMemory( fb, len ) );

} /* SwapToMemoryFromXMSMemory */

static void *xmsControl;

/*
 * xmsAlloc - allocate some xms memory
 */
static unsigned long xmsAlloc( int size )
{
    xms_addr            h;
    U_INT               handle, new_size, page_request;
    unsigned long       offset;

    size = (size + 0x03) & ~0x03;
    if( XMSCtrl.exhausted ) {
        return( NULL );
    }

    if( XMSCtrl.offset + size > XMSCtrl.size ) {
        if( XMSCtrl.small_block ) {
            return( NULL );
        }

        /* align offset to 1k boundary */
        offset = (XMSCtrl.offset + 0x03ff) & ~0x03ff;
        if( offset < XMSCtrl.size ) {
            /* reallocate the block to eliminate internal fragmentation */
            new_size = offset / 0x0400;
            _XMSReallocate( &xmsControl, XMSCtrl.handles[XMSCtrl.next_handle - 1],
                            new_size );
        }

        page_request = XMS_MAX_BLOCK_SIZE_IN_K;
        for( ;; ) {

            if( _XMSAllocate( &xmsControl, page_request,
                (unsigned short *) &handle ) != 0 ) {
                break;
            }

            XMSCtrl.small_block = true;
            page_request -= XMS_BLOCK_ADJUST_SIZE_IN_K;
            if( page_request == 0 ) {
                XMSCtrl.exhausted = true;
                return( NULL );
            }

        }

        XMSCtrl.offset = 0;
        XMSCtrl.size = page_request * 0x0400L;
        XMSCtrl.handles[XMSCtrl.next_handle++] = handle;
        if( XMSCtrl.offset + size > XMSCtrl.size ) {
            return( NULL );
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
    U_INT       size;
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

    xmsPtrs = MemAlloc( sizeof( long ) * MaxXMSBlocks );

    for( i = 0; i < MaxXMSBlocks; i++ ) {
        xmsPtrs[i] = xmsAlloc( MAX_IO_BUFFER );
        if( xmsPtrs[i] == NULL ) {
            break;
        }
        h.external = xmsPtrs[i];
        TotalXMSBlocks++;
    }
    xmsPtrs = realloc( xmsPtrs, TotalXMSBlocks * sizeof( long ) );
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
    U_INT       handle;

    if( !XMSCtrl.inuse ) {
        return;
    }
    while( XMSCtrl.next_handle > 0 ) {
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
        XMSCtrl.next_handle--;
    }
    XMSCtrl.inuse = false;

} /* XMSFini */

/*
 * xmsRead - read from XMS memory
 */
static int xmsRead( long addr, void *buff, int size )
{
    xms_addr            h;
    U_INT               offset;
    xms_move_descriptor control;
    void                *dest;

    if( addr == NULL ) {
        return( -1 );
    }
    size = (size + 1) & ~1;

    h.external = addr;
    offset = h.internal.offset << 2;

    if( h.internal.handle == XMS_HMA_HANDLE ) {

        dest = MK_FP( XMS_HMA_SEGMENT, offset );
        if( _XMSEnableA20( &xmsControl ) == 0 ) {
            return( -1 );
        }
        _XMSCopyWords( dest, buff, size >> 1 );
        if( _XMSDisableA20( &xmsControl ) == 0 ) {
            return( -1 );
        }

    } else {

        control.size = size;
        control.dest_handle = XMS_REAL_HANDLE;
        control.dest_offset.real = buff;
        control.src_handle = h.internal.handle;
        control.src_offset.extended = offset;
        if( _XMSMove( &xmsControl, &control ) == 0 ) {
            return( -1 );
        }

    }
    return( size );

} /* xmsRead */

/*
 * xmsWrite - write some XMS memory
 */
static int xmsWrite( long addr, void *buff, int size )
{
    xms_addr            h;
    xms_move_descriptor control;
    U_INT               offset;
    void                *dest;

    if( addr == NULL ) {
        return( -1 );
    }
    size = (size + 1) & ~1;
    h.external = addr;
    offset = h.internal.offset << 2;
    if( h.internal.handle == XMS_HMA_HANDLE ) {

        dest = MK_FP( XMS_HMA_SEGMENT, offset );
        if( _XMSEnableA20( &xmsControl ) == 0 ) {
            return( -1 );
        }
        _XMSCopyWords( buff, dest, size >> 1 );
        if( _XMSDisableA20( &xmsControl ) == 0 ) {
            return( -1 );
        }

    } else {

        control.size = size;
        control.src_handle = XMS_REAL_HANDLE;
        control.src_offset.real = buff;
        control.dest_handle = h.internal.handle;
        control.dest_offset.extended = offset;
        if( _XMSMove( &xmsControl, &control ) == 0 ) {
            return( -1 );
        }

    }
    return( size );

} /* xmsWrite */

/*
 * GiveBackXMSBlock - return an XMS block to the pool
 */
void GiveBackXMSBlock( long addr )
{
    int i;

    for( i = 0; i < TotalXMSBlocks; i++ ) {
        if( xmsPtrs[i] == NULL ) {
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
    MaxXMSBlocks = i;
    MaxXMSBlocks /= (MAX_IO_BUFFER / 1024);

} /* XMSBlockInit */

#endif
