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
* Description:  EMS management routines for DOS.
*
****************************************************************************/


#include "vi.h"
#include <stddef.h>
#include <string.h>

#if defined( USE_EMS )

#include <i86.h>
#include "dosx.h"
#include "xmem.h"
#include "fcbmem.h"
#include "pragmas.h"

#define EMS_IO_ERROR    ((size_t)-1)

ems_struct              EMSCtrl;
static xhandle          *emsPtrs;

static size_t   emsRead( xhandle, void __far *, size_t );
static size_t   emsWrite( xhandle, void __far *, size_t );

vi_rc EMSBlockTest( unsigned short blocks )
{
    if( !EMSCtrl.inuse ) {
        return( ERR_NO_EMS_MEMORY );
    }
    if( EMSBlocksInUse + blocks > TotalEMSBlocks ) {
        return( ERR_NO_EMS_MEMORY );
    }
    return( ERR_NO_ERR );

} /* EMSBlockTest */

void EMSBlockRead( xhandle addr, void __far *buff, size_t len )
{
    emsRead( addr, buff, len );

} /* EMSBlockRead */

void EMSBlockWrite( xhandle addr, void __far *buff, size_t len )
{
    emsWrite( addr, buff, len );

} /* EMSBlockWrite */

vi_rc EMSGetBlock( xhandle *addr )
{
    vi_rc       rc;
    xhandle     found;
    int         i;

    rc = EMSBlockTest( 1 );
    if( rc == ERR_NO_ERR ) {
        found = 0;
        EMSBlocksInUse++;
        for( i = 0; i < TotalEMSBlocks; i++ ) {
            if( emsPtrs[i] != 0 ) {
                found = emsPtrs[i];
                emsPtrs[i] = 0;
                break;
            }
        }
        *addr = found;
    }
    return( rc );

} /* EMSGetBlock */

/*
 * SwapToEMSMemory - move an fcb to extended memory from memory
 */
vi_rc SwapToEMSMemory( fcb *fb )
{
    vi_rc       rc;
    size_t      len;
    xhandle     found;

    found = 0;
    rc = EMSGetBlock( &found );
    if( rc == ERR_NO_ERR ) {
        len = MakeWriteBlock( fb );
        emsWrite( found, WriteBuffer, len );

        /*
         * finish up
         */
        fb->xblock.handle = found;
        fb->in_ems_memory = true;
    }
    return( rc );

} /* SwapToEMSMemory */

/*
 * SwapToMemoryFromEMSMemory - bring data back from extended memory
 */
vi_rc SwapToMemoryFromEMSMemory( fcb *fb )
{
    size_t  len;

    len = FcbSize( fb );
    emsRead( fb->xblock.handle, ReadBuffer, len );
    GiveBackEMSBlock( fb->xblock.handle );
    return( RestoreToNormalMemory( fb, len ) );

} /* SwapToMemoryFromEMSMemory */

/*
 * eMSAlloc - allocate some expanded memory
 */
static xhandle eMSAlloc( unsigned size )
{
    unsigned char       handle;
    ems_addr            h;

    size = ROUNDUP( size, 2 );
    if( size > EMS_MAX_PAGE_SIZE || EMSCtrl.exhausted ) {
        return( 0 );
    }
    if( EMSCtrl.offset + size > EMS_MAX_PAGE_SIZE ) {
        ++EMSCtrl.logical;
        if( EMSCtrl.logical == EMSCtrl.max_logical ) {
            EMSCtrl.max_logical = EMS_MAX_LOGICAL_PAGES;
            for( ;; ) {
                if( _EMSAllocateMemory( EMSCtrl.max_logical, &handle ) == 0 )  {
                    break;
                }
                --EMSCtrl.max_logical;
                if( EMSCtrl.max_logical == 0 ) {
                    EMSCtrl.exhausted = true;
                    return( 0 );
                }
            }
            EMSCtrl.logical = 0;
            EMSCtrl.handles[EMSCtrl.allocated++] = handle;
        }
        EMSCtrl.offset = 0;
    }
    h.internal.offset = EMSCtrl.offset;
    h.internal.logical = EMSCtrl.logical;
    h.internal.handle = EMSCtrl.handles[EMSCtrl.allocated - 1];

    EMSCtrl.offset += size;

    return( h.external );

} /* eMSAlloc */

static char emsStr[] = "EMMXXXX0";

/*
 * EMSInit - init for EMS memory usage
 */
void EMSInit( void )
{
    void                *vect;
    char                *check;
    unsigned            i;
    unsigned short      seg;
    unsigned char       handle;
    ems_addr            h;

    EMSCtrl.inuse = false;
    if( !EditFlags.ExtendedMemory ) {
        return;
    }

    EMSCtrl.exhausted = false;
    vect = DosGetVect( EMS_INTERRUPT );
    check = _MK_FP( _FP_SEG( vect ), EMS_INTERRUPT_OFFSET );
    for( i = 0; i <= 7; i++ ) {
        if( check[i] != emsStr[i] ) {
            return;
        }
    }
    if( _EMSStatus() != 0 ) {
        return;
    }
    if( _EMSGetPageFrame( &seg ) != 0 ) {
        return;
    }
    EMSCtrl.seg = seg;

    if( _EMSAllocateMemory( EMS_MAX_LOGICAL_PAGES, &handle ) != 0 ) {
        return;
    }
    EMSCtrl.handles[EMSCtrl.allocated] = handle;
    EMSCtrl.logical = 0;
    EMSCtrl.max_logical = EMS_MAX_LOGICAL_PAGES;
    EMSCtrl.offset = 0;
    EMSCtrl.allocated = 1;
    for( i = 0; i < EMS_MAX_PHYSICAL_PAGES; i++ ) {
        EMSCtrl.physical[i].used = false;
    }

    emsPtrs = _MemAllocArray( xhandle, EditVars.MaxEMSBlocks );

    for( i = 0; i < EditVars.MaxEMSBlocks; i++ ) {
        emsPtrs[i] = eMSAlloc( MAX_IO_BUFFER );
        if( emsPtrs[i] == 0 ) {
            break;
        }
        h.external = emsPtrs[i];
        TotalEMSBlocks++;
    }
    emsPtrs = _MemReallocArray( emsPtrs, xhandle, TotalEMSBlocks );

    EMSCtrl.inuse = true;


} /* EMSInit */

/*
 * EMSFini - release expanded memory
 */
void EMSFini( void )
{
    unsigned    curr;

    if( !EMSCtrl.inuse ) {
        return;
    }
    for( curr = EMSCtrl.allocated; curr-- > 0; ) {
        _EMSReleaseMemory( EMSCtrl.handles[curr] );
    }
    EMSCtrl.allocated = 0;
    EMSCtrl.inuse = false;

} /* EMSFini */

/*
 * locatePhysicalPage - find a phyiscal page, given a logical one
 */
static bool locatePhysicalPage( unsigned char h, unsigned char l, unsigned char *p )
{
    unsigned char       i, free;

    free = EMS_MAX_PHYSICAL_PAGES;
    for( i = 0; i < EMS_MAX_PHYSICAL_PAGES; ++i ) {
        if( !EMSCtrl.physical[i].used ) {
            free = i;
            continue;
        } /* if */
        if( EMSCtrl.physical[i].handle != h ) {
            continue;
        }
        if( EMSCtrl.physical[i].logical == l ) {
            *p = i;
            return( true );
        }
    }
    if( free == EMS_MAX_PHYSICAL_PAGES ) {
        return( false );
    }
    *p = free;
    return( false );

} /* locatePhysicalPage */

/*
 * emsAccess - access some expanded memory
 */
static void *emsAccess( ems_addr x )
{
    unsigned char       handle, logical, physical;
    unsigned            offset;

    if( x.external == 0 ) {
        return( NULL );
    }
    logical = x.internal.logical;
    handle = x.internal.handle;
    if( locatePhysicalPage( handle, logical, &physical ) == 0 ) {
        if( _EMSMapMemory( handle, logical, physical ) != 0 ) {
            return( NULL );
        }
        EMSCtrl.physical[physical].handle = handle;
        EMSCtrl.physical[physical].logical = logical;
        EMSCtrl.physical[physical].used = true;
    }
    offset = x.internal.offset;
    offset += physical * EMS_MAX_PAGE_SIZE;
    return( _MK_FP( EMSCtrl.seg, offset ) );

} /* emsAccess */

/*
 * emsRelease - release some EMS memory
 */
static void emsRelease( ems_addr x )
{
    unsigned char       handle, logical, physical;

    if( x.external == 0 ) {
        return;
    }
    logical = x.internal.logical;
    handle = x.internal.handle;
    if( locatePhysicalPage( handle, logical, &physical ) == 0 ) {
        return;
    }
    EMSCtrl.physical[physical].used = false;

} /* emsRelease */

/*
 * emsRead - read some expanded memory
 */
static size_t emsRead( xhandle addr, void __far *buff, size_t size )
{
    void        *ptr;
    ems_addr    h;

    h.external = addr;
    ptr = emsAccess( h );
    if( ptr == NULL ) {
        return( EMS_IO_ERROR );
    }
    _fmemcpy( buff, ptr, size );
    emsRelease( h );
    return( size );

} /* emsRead */

/*
 * emsWrite - write expanded memory
 */
static size_t emsWrite( xhandle addr, void __far *buff, size_t size )
{
    void        *ptr;
    ems_addr    h;

    h.external = addr;
    ptr = emsAccess( h );
    if( ptr == NULL ) {
        return( EMS_IO_ERROR );
    }
    _fmemcpy( ptr, buff, size );
    emsRelease( h );
    return( size );

} /* emsWrite */

/*
 * GiveBackEMSBlock - return an EMS block to the pool
 */
void GiveBackEMSBlock( xhandle addr )
{
    int i;

    for( i = 0; i < TotalEMSBlocks; i++ ) {
        if( emsPtrs[i] == 0 ) {
            emsPtrs[i] = addr;
            break;
        }
    }
    EMSBlocksInUse--;

} /* GiveBackEMSBlock */

/*
 * EMSBlockInit
 */
void EMSBlockInit( int i )
{
    if( EMSCtrl.inuse ) {
        return;
    }
    EditVars.MaxEMSBlocks = i;
    EditVars.MaxEMSBlocks /= (MAX_IO_BUFFER / 1024);

} /* EMSBlockInit */

#endif
