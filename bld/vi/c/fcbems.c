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


#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include "vi.h"

#ifndef NOEMS
#include "dosx.h"
#include "xmem.h"
#include "fcbmem.h"


ems_struct EMSCtrl;
static unsigned long *emsPtrs;
static int emsRead( long, void *, int );
static int emsWrite( long, void *, int );

int EMSBlockTest( unsigned short blocks )
{
    if( !EMSCtrl.inuse ) {
        return( ERR_NO_EMS_MEMORY );
    }
    if( EMSBlocksInUse +blocks > TotalEMSBlocks ) {
        return( ERR_NO_EMS_MEMORY );
    }
    return( ERR_NO_ERR );

}

void EMSBlockRead( long addr, void *buff, unsigned len )
{
    emsRead( addr, buff, len );
} /* EMSBlockRead */

int EMSBlockWrite( long addr, void *buff, unsigned len )
{
    return( emsWrite( addr, buff, len ) );

} /* EMSBlockWrite */

int EMSGetBlock( long *addr )
{
    int         i;
    long        found=NULL;

    i = EMSBlockTest( 1 );
    if( i ) {
        return( i );
    }
    EMSBlocksInUse++;
    for( i=0;i<TotalEMSBlocks;i++ ) {
        if( emsPtrs[i] != NULL ) {
            found = emsPtrs[i];
            emsPtrs[i] = NULL;
            break;
        }
    }
    *addr = found;
    return( ERR_NO_ERR );

} /* EMSGetBlock */

/*
 * SwapToEMSMemory - move an fcb to extended memory from memory
 */
int SwapToEMSMemory( fcb *fb )
{
    int         i,len;
    long        found;

    i = EMSGetBlock( &found );
    if( i ) {
        return( i );
    }
    len = MakeWriteBlock( fb );
    emsWrite( found, WriteBuffer, len );

    /*
     * finish up
     */
    fb->xmemaddr = found;
    fb->in_ems_memory = TRUE;
    return( ERR_NO_ERR );

} /* SwapToEMSMemory */

/*
 * SwapToMemoryFromEMSMemory - bring data back from extended memory
 */
int SwapToMemoryFromEMSMemory( fcb *fb )
{
    int len;

    len = FcbSize( fb );
    emsRead( fb->xmemaddr, ReadBuffer, len );
    GiveBackEMSBlock( fb->xmemaddr );
    return( RestoreToNormalMemory( fb, len ) );

} /* SwapToMemoryFromEMSMemory */

/*
 * eMSAlloc - allocate some expanded memory
 */
static long eMSAlloc( U_INT size )
{
    unsigned char       handle;
    ems_addr            h;

    size = ( size + 1 ) & ~1;
    if( size > EMS_MAX_PAGE_SIZE || EMSCtrl.exhausted ) {
        return( NULL );
    }
    if( EMSCtrl.offset + size > EMS_MAX_PAGE_SIZE ) {
        ++EMSCtrl.logical;
        if( EMSCtrl.logical == EMSCtrl.max_logical ) {
            EMSCtrl.max_logical = EMS_MAX_LOGICAL_PAGES;
            while( 1 ) {
                if( _EMSAllocateMemory( EMSCtrl.max_logical, &handle ) == 0 )  {
                    break;
                }
                --EMSCtrl.max_logical;
                if( EMSCtrl.max_logical == 0 ) {
                    EMSCtrl.exhausted = TRUE;
                    return( NULL );
                }
            }
            EMSCtrl.logical = 0;
            EMSCtrl.handles[ EMSCtrl.allocated++ ] = handle;
        }
        EMSCtrl.offset = 0;
    }
    h.internal.offset = EMSCtrl.offset;
    h.internal.logical = EMSCtrl.logical;
    h.internal.handle = EMSCtrl.handles[EMSCtrl.allocated-1];

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

    EMSCtrl.inuse = FALSE;
    if( !EditFlags.ExtendedMemory ) {
        return;
    }

    EMSCtrl.exhausted = FALSE;
    vect = DosGetVect( EMS_INTERRUPT );
    check = MK_FP( FP_SEG( vect ), EMS_INTERRUPT_OFFSET );
    for( i=0;i<=7;i++ ) {
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
    for( i=0; i<EMS_MAX_PHYSICAL_PAGES; i++ ) {
        EMSCtrl.physical[ i ].used = FALSE;
    }

    emsPtrs = MemAlloc( sizeof( long ) * MaxEMSBlocks );

    for( i=0;i<MaxEMSBlocks;i++ ) {
        emsPtrs[i] = eMSAlloc( MAX_IO_BUFFER );
        if( emsPtrs[i] == NULL ) {
            break;
        }
        h.external = emsPtrs[i];
        TotalEMSBlocks++;
    }
    emsPtrs = MemReAlloc( emsPtrs, TotalEMSBlocks * sizeof( long ) );

    EMSCtrl.inuse = TRUE;


} /* EMSInit */

/*
 * EMSFini - release expanded memory
 */
void EMSFini( void )
{
    U_INT       curr;

    if( ! EMSCtrl.inuse ) {
        return;
    }
    curr = EMSCtrl.allocated;
    while( curr ) {
        curr--;
        _EMSReleaseMemory( EMSCtrl.handles[ curr ] );
    }
    EMSCtrl.allocated = 0;
    EMSCtrl.inuse = FALSE;

} /* EMSFini */

/*
 * locatePhysicalPage - find a phyiscal page, given a logical one
 */
static bool locatePhysicalPage( unsigned char h, unsigned char l, unsigned char *p)
{
    unsigned char       i,free;

    free = EMS_MAX_PHYSICAL_PAGES;
    for( i = 0; i < EMS_MAX_PHYSICAL_PAGES; ++i ) {
        if( EMSCtrl.physical[ i ].used == 0 ) {
            free = i;
            continue;
        } /* if */
        if( EMSCtrl.physical[ i ].handle != h ) {
            continue;
        }
        if( EMSCtrl.physical[ i ].logical == l ) {
            *p = i;
            return( TRUE );
        }
    }
    if( free == EMS_MAX_PHYSICAL_PAGES ) {
        return( FALSE );
    }
    *p = free;
    return( FALSE );

} /* locatePhysicalPage */

/*
 * emsAccess - access some expanded memory
 */
static void *emsAccess( ems_addr x )
{
    unsigned char       handle,logical,physical;
    U_INT               offset;

    if( x.external == NULL ) {
        return( NULL );
    }
    logical = x.internal.logical;
    handle = x.internal.handle;
    if( locatePhysicalPage( handle, logical, &physical ) == 0 ) {
        if( _EMSMapMemory( handle, logical, physical ) != 0 ) {
            return( NULL );
        }
        EMSCtrl.physical[ physical ].handle = handle;
        EMSCtrl.physical[ physical ].logical = logical;
        EMSCtrl.physical[ physical ].used = TRUE;
    }
    offset = x.internal.offset;
    offset += physical * EMS_MAX_PAGE_SIZE;
    return( MK_FP( EMSCtrl.seg, offset ) );

} /* emsAccess */

/*
 * emsRelease - release some EMS memory
 */
static void emsRelease( ems_addr x )
{
    unsigned char       handle,logical,physical;

    if( x.external == NULL ) {
        return;
    }
    logical = x.internal.logical;
    handle = x.internal.handle;
    if( locatePhysicalPage( handle, logical, &physical ) == 0 ) {
        return;
    }
    EMSCtrl.physical[ physical ].used = FALSE;

} /* emsRelease */

/*
 * emsRead - read some expanded memory
 */
static int emsRead( long addr, void *buff, int size )
{
    void        *ptr;
    ems_addr    h;

    h.external = addr;
    ptr = emsAccess( h );
    if( ptr == NULL ) {
        return( -1 );
    }
    memcpy( buff,ptr, size );
    emsRelease( h );
    return( size );

} /* emsRead */

/*
 * emsWrite - write expanded memory
 */
static int emsWrite( long addr, void *buff, int size )
{
    void        *ptr;
    ems_addr    h;

    h.external = addr;
    ptr = emsAccess( h );
    if( ptr == NULL ) {
        return( -1 );
    }
    memcpy( ptr, buff, size );
    emsRelease( h );
    return( size );

} /* emsWrite */

/*
 * GiveBackEMSBlock - return an EMS block to the pool
 */
void GiveBackEMSBlock( long addr )
{
    int i;

    for( i=0;i<TotalEMSBlocks;i++ ) {
        if( emsPtrs[i] == NULL ) {
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
    MaxEMSBlocks = i;
    MaxEMSBlocks /= (MAX_IO_BUFFER/1024);

} /* EMSBlockInit */

#endif
