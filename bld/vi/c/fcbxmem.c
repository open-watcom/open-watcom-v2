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

#ifndef NOXTD

#include "dosx.h"
#include "xmem.h"
#include "fcbmem.h"

extern void interrupt XMemIntHandler( void );

static descriptor GDT[] = {
    { 0, 0, 0 },    /* dummy segment */
    { 0, 0, 0 },    /* data segment */
    { 0, 0, 0 },    /* source segment */
    { 0, 0, 0 },    /* target segment */
    { 0, 0, 0 },    /* BIOS code segment */
    { 0, 0, 0 }     /* stack segment */
};

xtd_struct XMemCtrl;

static void xmemWrite( long , void *, unsigned );
static void xmemRead( long , void *, unsigned );
static bool checkVDISK( flat_address * );

/*
 * SwapToExtendedMemory - move an fcb to extended memory from memory
 */
int SwapToExtendedMemory( fcb *fb )
{
    long        addr;
    int         len;

    if( !XMemCtrl.inuse ) {
        return( ERR_NO_EXTENDED_MEMORY );
    }

    /*
     * dump the fcb
     */
    if( !GetNewBlock( &addr, XMemBlocks, XMemBlockArraySize )) {
        return( ERR_NO_EXTENDED_MEMORY );
    }
    XMemCtrl.allocated++;
    addr += XMemCtrl.offset;
    len = MakeWriteBlock( fb );
    xmemWrite( addr, WriteBuffer, len );

    /*
     * finish up
     */
    fb->xmemaddr = addr;
    fb->in_extended_memory = TRUE;
    return( ERR_NO_ERR );

} /* SwapToExtendedMemory */

/*
 * SwapToMemoryFromExtendedMemory - bring data back from extended memory
 */
int SwapToMemoryFromExtendedMemory( fcb *fb )
{
    int len;

    len = FcbSize( fb );
    xmemRead( fb->xmemaddr, ReadBuffer, len );
    GiveBackXMemBlock( fb->xmemaddr );

    return( RestoreToNormalMemory( fb, len ) );

} /* SwapToMemoryFromExtendedMemory */

/*
 * XMemInit - initialize extended memory
 */
void XMemInit( void )
{
    long        blocks, rc;
    int         j, i, extra;
    U_INT       amount;

    /*
     * init
     */
    XMemCtrl.inuse = FALSE;
    if( !EditFlags.ExtendedMemory ) {
        return;
    }

    /*
     * get amount of extended memory out there
     */
    checkVDISK( &XMemCtrl.offset );
    rc = _XtdGetSize();
    if( rc < 0 ) {
        return;
    }

    amount = rc;
    if( amount <= 16 ) return;      /* to allow for ibm cache bug */
    amount -= 16;

    XMemCtrl.amount_left = (((unsigned long)amount) * 1024 +
                    XMEM_MEMORY_START) - XMemCtrl.offset;
    if( XMemCtrl.amount_left <= 0 ) {
        return;
    }

    XMemCtrl.xtd_vector = DosGetVect( XMEM_INTERRUPT );
    DosSetVect( XMEM_INTERRUPT, XMemIntHandler );
    XMemCtrl.inuse = TRUE;

    /*
     * build allocation blocks
     */
    blocks = XMemCtrl.amount_left / (long) MAX_IO_BUFFER;
    XMemBlockArraySize = (int) (blocks >> 3);
    extra = (int) (blocks - ((long) (XMemBlockArraySize) << 3));
    if( extra > 0 ) {
        XMemBlockArraySize++;
    }
    XMemBlocks = MemAlloc( XMemBlockArraySize + 1 );
    for( i = 0; i < XMemBlockArraySize; i++ ) {
        XMemBlocks[i] = 0xff;
    }

    /*
     * if we have extra blocks, add them to the last entry
     */
    if( extra > 0 ) {
        j = 0x80;
        i = j;
        while( --extra > 0 ) {
            j >>= 1;
            i |= j;
        }
        XMemBlocks[XMemBlockArraySize - 1] = (char) i;
    }

    XMemCtrl.allocated = 0;

} /* XMemInit */

/*
 * XMemFini - finish up with extended memory
 */
void XMemFini( void )
{
    void        *verify;

    if( !XMemCtrl.inuse ) {
        return;
    }
    verify = DosGetVect( XMEM_INTERRUPT );
    if( verify != XMemIntHandler ) {
        if( verify == XMemCtrl.xtd_vector ) {
            return;
        }
    }
    DosSetVect( XMEM_INTERRUPT, XMemCtrl.xtd_vector );
    XMemCtrl.inuse = FALSE;

} /* XMemFini */

/*
 * xmemRead - read from extended memory
 */
static void xmemRead( long addr, void *buff, unsigned size )
{
    flat_address        source, target;

    size = (size + 1) & ~1;
    source = addr;
    GDT[GDT_SOURCE].address = GDT_RW_DATA | source;
    GDT[GDT_SOURCE].length = size;
    target = MAKE_LINEAR( buff );
    GDT[GDT_TARGET].address = GDT_RW_DATA | target;
    GDT[GDT_TARGET].length = size;

    _XtdMoveMemory( &GDT, size >> 1 );

} /* xmemRead */

/*
 * xmemWrite - write to extended memory
 */
static void xmemWrite( long addr, void *buff, unsigned size )
{
    flat_address        source, target;

    size = (size + 1) & ~1;
    target = addr;
    GDT[GDT_TARGET].address = GDT_RW_DATA | target;
    GDT[GDT_TARGET].length = size;
    source = MAKE_LINEAR( buff );
    GDT[GDT_SOURCE].address = GDT_RW_DATA | source;
    GDT[GDT_SOURCE].length = size;

    _XtdMoveMemory( &GDT, size >> 1 );

} /* xmemWrite */

/*
 * checkVDISK - test for resident vdisk
 */
static bool checkVDISK( flat_address *start )
{
    int                 i;
    void                *value;
    char                *name;
    flat_address        *avail;
    static char         _vdisk[] = "VDISK  V";

    *start = XMEM_MEMORY_START;
    value = DosGetVect( VDISK_INTERRUPT );
    name = MK_FP( FP_SEG( value ), VDISK_NAME_OFFSET );
    for( i = 0; i <= 7; i++ ) {
        if( name[i] != _vdisk[i] ) {
            return( FALSE );
        }
    }
    avail = MK_FP( FP_SEG( value ), VDISK_AVAIL_OFFSET );
    *start = *avail & GDT_ADDR;
    return( TRUE );

} /* checkVDISK */

/*
 * GiveBackXMemBlock - return some extended memory
 */
void GiveBackXMemBlock( long addr )
{
    GiveBackBlock( addr - XMemCtrl.offset, XMemBlocks );
    XMemCtrl.allocated--;

} /* GiveBackXMemBlock */
#endif
