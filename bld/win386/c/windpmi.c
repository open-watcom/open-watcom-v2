/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DPMI function wrappers for Win386.
*
****************************************************************************/


#include <stddef.h>
#include <windows.h>
#include "winext.h"
#include "_windpmi.h"
#include "windpmi.h"
#include "windata.h"


#define MAX_CACHE       48
#define MAX_SELECTORS   8192

typedef struct {
    WORD        sel;
    WORD        limit;
    DWORD       base;
    char        in_use;
} alias_cache_entry;

typedef struct memblk {
    struct memblk *next;
    DWORD       handle;
    DWORD       addr;
    DWORD       size;
} memblk;

static WORD                     WrapAround;
static WORD                     hugeIncrement;
static WORD                     firstCacheSel,lastCacheSel;
static WORD                     cacheUseCount;
static WORD                     StackCacheSel;
static DWORD                    StackBase, StackBase_64K;
static alias_cache_entry        aliasCache[MAX_CACHE];
static WORD                     currSelCount;
static char                     SelBitArray[MAX_SELECTORS / 8];
static memblk                   *MemBlkList;

static unsigned char BitMask[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
/*
 * addToSelList - add selector to list of non-cached selectors
 */
static void addToSelList( WORD sel )
{
    SelBitArray[sel >> 6] |= BitMask[(sel >> 3) & 7];
    currSelCount++;

} /* addToSelList */

/*
 * removeFromSelList - remove selector from list of non-cached selectors
 */
static void removeFromSelList( WORD sel )
{
    SelBitArray[sel >> 6] &= ~ BitMask[(sel >> 3) & 7];
    currSelCount--;

} /* removeFromSelList */

/*
 * _DPMIGetAliases - get alias descriptors for some memory
 */
WORD _DPMIGetAliases( DWORD offset, DWORD __far *res, WORD cnt)
{
    long                rc;
    WORD                sel,i;
    DWORD               limit,base;
    alias_cache_entry   *ace;

    if( offset == 0L ) {
        *res = 0L;
        return( 0 );
    }

    /*
     * optimize alias requests.  If we are only asking for 1 alias,
     * then we look for a free cache entry.  The selector cache is
     * a collection of pre-allocated selectors with their access rights
     * pre-set and a default limit of 64K.  Thus, we only need to set
     * the base address and, very rarely, reset the limit (this only
     * happens when we are aliasing a chunk in the last 64K of the
     * 32-bit segment).
     */
    if( cnt == 1 ) {
        if( offset < StackBase_64K  &&  offset >= StackBase ) {
            *res = (((DWORD)StackCacheSel) << 16) + offset - StackBase;
            return( 0 );
        }
        if( cacheUseCount < MAX_CACHE ) {
            ace = &aliasCache[0];
            for( i = 0; i < MAX_CACHE; i++ ) {
                if( !ace->in_use ) {
                    base = DataSelectorBase + offset;
                    if( base != ace->base ) {
                        ace->base = base;
                        DPMISetSegmentBaseAddress( ace->sel, base );
                    }
                    *res = ((DWORD)ace->sel) << 16;
                    ace->in_use = TRUE;
                    cacheUseCount++;
                    return( 0 );
                }
                ace += 1;
            }
        }
    }
    if( cnt == 0 ) {
        cnt = 1;
    }

    /*
     * get a descriptor
     */
    *res = 0L;
    rc = DPMIAllocateLDTDescriptors( cnt );
    if( rc < 0L ) {
        return( 666 );
    }
    sel = (WORD)rc;
    *res = ((DWORD)sel) << 16;
    limit = cnt * 0x10000 - 1;

    for( i = 0; i < cnt; i++ ) {

#if 0
        /*
         * We no longer restrict the limit to being 64K, since
         * Windows sets the limits of their huge selectors to be
         * for the entire linear space, not just 64K
         */
        if( limit > 0xFFFF ) {
            limit = 0xFFFF;
        }
#endif

        /*
         * set new limit, address, and access rights
         */
        if( limit > 1024L * 1024L ) {
            DPMISetDescriptorAccessRights( sel, DPL + ACCESS_DATA16BIG );
        } else {
            DPMISetDescriptorAccessRights( sel, DPL + ACCESS_DATA16 );
        }
        DPMISetSegmentBaseAddress( sel, DataSelectorBase + offset );
        DPMISetSegmentLimit( sel, limit );
        addToSelList( sel );
        sel += hugeIncrement;
        offset += 0x10000;
        limit  -= 0x10000;
    }

    return( 0 );

} /* _DPMIGetAliases */

/*
 * _DPMIGetAlias - get alias descriptor for some memory
 */
WORD _DPMIGetAlias( DWORD offset, DWORD __far *res )
{
    return( _DPMIGetAliases( offset, res, 1 ) );

} /* _DPMIGetAlias */

/*
 * _DPMIFreeAlias - free alias descriptor
 */
void _DPMIFreeAlias( WORD sel )
{
    alias_cache_entry   *ace;

    if( sel == 0 || sel == StackCacheSel ) {
        return;
    }
    if( sel >= firstCacheSel && sel <= lastCacheSel ) {
        ace = &aliasCache[( sel - firstCacheSel ) / hugeIncrement];
        if( ace->in_use ) {
            ace->in_use = FALSE;
            cacheUseCount--;
        }
        return;
    }
    removeFromSelList( sel );
    DPMIFreeLDTDescriptor( sel );

} /* _DPMIFreeAlias */

WORD _DPMIGetHugeAlias( DWORD offset, DWORD __far *res, DWORD size )
{
    DWORD       no64k;

    no64k = Align64K( size );
    return( _DPMIGetAliases( offset, res, 1 + (WORD)( no64k / 0x10000L ) ) );
}

void _DPMIFreeHugeAlias( DWORD desc, DWORD size )
{
    DWORD       no64k;
    WORD        cnt,sel,i;

    sel = desc >> 16;
    if( sel == 0 ) {
        return;
    }
    no64k = Align64K( size );
    cnt = 1 + (WORD)( no64k / 0x10000L );
    for( i = 0; i < cnt; i++ ) {
        removeFromSelList( sel );
        DPMIFreeLDTDescriptor( sel );
        sel += hugeIncrement;
    }
}

/*
 * __DPMI fns are the ones called by the 32-bit application
 */
unsigned short __pascal __far __DPMIGetAlias( unsigned long offset, unsigned long __far *res )
{
    return( _DPMIGetAlias( offset, res ) );
}

void __pascal __far __DPMIFreeAlias( unsigned long desc )
{
    _DPMIFreeAlias( desc );
}

unsigned short __pascal __far __DPMIGetHugeAlias( unsigned long offset, unsigned long __far *res, unsigned long size )
{
    return( _DPMIGetHugeAlias( offset, res, size ) );
}

void __pascal __far __DPMIFreeHugeAlias( unsigned long desc, unsigned long size )
{
    _DPMIFreeHugeAlias( desc, size );
}

/*
 * setLimitAndAddr - set the limit and address of a 32-bit selector
 */
static void setLimitAndAddr( WORD sel, DWORD addr, DWORD len, WORD type )
{

    DPMISetSegmentBaseAddress( sel, addr );
    --len;
    if( len >= 1024L * 1024L ) {
        if( type == ACCESS_CODE ) {
            DPMISetDescriptorAccessRights( sel, DPL + ACCESS_CODE32BIG );
        } else {
            DPMISetDescriptorAccessRights( sel, DPL + ACCESS_DATA32BIG );
        }
    } else {
        if( type == ACCESS_CODE ) {
            DPMISetDescriptorAccessRights( sel, DPL + ACCESS_CODE32SMALL );
        } else {
            DPMISetDescriptorAccessRights( sel, DPL + ACCESS_DATA32SMALL );
        }
    }
    DPMISetSegmentLimit( sel, len );

} /* setLimitAndAddr */

/*
 * InitFlatAddrSpace - initialize flat address space
 */
WORD InitFlatAddrSpace( DWORD baseaddr, DWORD len )
{
    long        rc;
    WORD        sel;
    descriptor  desc;

    hugeIncrement = DPMIGetNextSelectorIncrementValue();
    /*
     * get a code selector pointing to the memory
     */
    rc = DPMIAllocateLDTDescriptors( 1 );
    if( rc < 0L ) {
        return( 4 );
    }
    sel = (WORD)rc;
    CodeEntry.seg = sel;
    setLimitAndAddr( sel, baseaddr, len, ACCESS_CODE );
    CodeSelectorBase = baseaddr;

    /*
     * get a data and stack selector pointing to the memory
     */
    rc = DPMIAllocateLDTDescriptors( 2 );
    if( rc < 0L ) {
        DPMIFreeLDTDescriptor( sel );
        return( 4 );
    }
    sel = (WORD)rc;
    DataSelector = sel;
    setLimitAndAddr( sel, baseaddr, len, ACCESS_DATA );
    StackSelector = sel + hugeIncrement;
//    setLimitAndAddr( StackSelector, baseaddr, StackSize, ACCESS_DATA );
//      The code generator sometimes uses EBP as general purpose
//      register for accessing data that is not in the STACK segment
//      so we must access the same space as DS
    setLimitAndAddr( StackSelector, baseaddr, len, ACCESS_DATA );
    WrapAround = FALSE;
    if( DPMIGetDescriptor( DataSelector, &desc ) == 0 ) {
        if( desc.lim_16_19 == 0x0F && desc.lim_0_15 == 0xFFFF ) {
            WrapAround = TRUE;
        } else {
            WrapAround = FALSE;
        }
    }
    return( 0 );

} /* InitFlatAddrSpace */

/*
 * _DPMIGet32 - get a 32-bit segment
 */
WORD _DPMIGet32( dpmi_mem_block _FAR *adata, DWORD len )
{
    int         rc;

    /*
     * the return codes 4 and 5 are the same as WINMEM32.DLL's return
     * codes.  Hysterical raisins.
     */

    /*
     * get memory region
     */
#ifdef DLL32
    rc = _fDPMIAllocateMemoryBlock( adata, len );
#else
    rc = DPMIAllocateMemoryBlock( adata, len );
#endif
    if( rc ) {
        return( 5 );
    }
    return( 0 );

} /* _DPMIGet32 */

/*
 * _DPMIFree32 - free a 32-bit handle
 */
void _DPMIFree32( DWORD handle )
{
    DPMIFreeLDTDescriptor( DataSelector );
    DPMIFreeLDTDescriptor( StackSelector );
    DPMIFreeLDTDescriptor( CodeEntry.seg );
    DPMIFreeMemoryBlock( handle );

} /* _DPMIFree32 */

/*
 * __DPMIAlloc - allocate a new block of memory
 */
unsigned long __pascal __far __DPMIAlloc( unsigned long size )
{
    dpmi_mem_block  adata;
    memblk          *p;

    for( ;; ) {
        if( _DPMIGet32( &adata, size ) ) {
            adata.linear = DataSelectorBase;        // cause NULL to be returned
            break;
        }
        p = (memblk *)LocalAlloc( LMEM_FIXED, sizeof( memblk ) );
        if( p == NULL ) {
            DPMIFreeMemoryBlock( adata.handle );
            adata.linear = DataSelectorBase;        // cause NULL to be returned
            break;
        }
        p->next = MemBlkList;
        p->handle = adata.handle;
        p->addr   = adata.linear;
        p->size   = size;
        MemBlkList = p;
        if( WrapAround || adata.linear >= DataSelectorBase ) {
            break;
        }
        // if we are on NT or OS/2, try again until we get a memory
        // block with address higher than our DataSelectorBase
    }
    if( !WrapAround && MemBlkList != NULL ) {
        /* free up any memory allocated that is below DataSelectorBase */
        while( (p = MemBlkList->next) != NULL ) {
            if( p->addr >= DataSelectorBase )
                break;
            DPMIFreeMemoryBlock( p->handle );
            MemBlkList->next = p->next;
            LocalFree( (HLOCAL)p );
        }
        p = MemBlkList;
        if( p->addr < DataSelectorBase ) {
            DPMIFreeMemoryBlock( p->handle );
            MemBlkList = p->next;
            LocalFree( (HLOCAL)p );
        }
    }
    return( adata.linear - DataSelectorBase ); // return address of memory block
}

/*
 * __DPMIFree - free a block of memory allocated by __DPMIAlloc
 */
unsigned short __pascal __far __DPMIFree( unsigned long addr )
{
    memblk      *p;
    memblk      *prev;

    addr += DataSelectorBase;           // add base address
    prev = NULL;
    for( p = MemBlkList; p != NULL; p = p->next ) {
        if( p->addr == addr ) {
            DPMIFreeMemoryBlock( p->handle );
            if( prev == NULL ) {
                MemBlkList = p->next;
            } else {
                prev->next = p->next;
            }
            LocalFree( (HLOCAL)p );
            return( 0 );                // indicate success
        }
        prev = p;
    }
    return( -1 );                       // indicate error
}

void FreeDPMIMemBlocks( void )
{
    memblk      *p;

    while( (p = MemBlkList) != NULL ) {
        MemBlkList = p->next;
        DPMIFreeMemoryBlock( p->handle );
        LocalFree( (HLOCAL)p );
    }
}

/*
 * GetDataSelectorInfo - collect data about current 32-bit data segment
 */
void GetDataSelectorInfo( void )
{
    DataSelectorBase = DPMIGetSegmentBaseAddress( DataSelector );

} /* GetDataSelectorInfo */

/*
 * InitSelectorCache - allocate the selector cache
 */
int InitSelectorCache( void )
{
    long        rc;
    int         i;
    WORD        sel;

    rc = DPMIAllocateLDTDescriptors( MAX_CACHE + 2 );
    if( rc < 0L ) {
        return( rc );
    }
    firstCacheSel = (WORD)rc;
    sel = (WORD)rc;
    for( i = 0; i < MAX_CACHE + 2; i++ ) {
        if( i < MAX_CACHE ) {
            aliasCache[i].sel = sel;
            aliasCache[i].limit = 0xFFFF;
            aliasCache[i].base = 0L;
            aliasCache[i].in_use = FALSE;
            lastCacheSel = sel;
        }
        DPMISetDescriptorAccessRights( sel, DPL + ACCESS_DATA16 );
        DPMISetSegmentLimit( sel, 0xFFFF );
        sel += hugeIncrement;
    }
    StackCacheSel = lastCacheSel + hugeIncrement;
    Int21Selector = StackCacheSel + hugeIncrement;
    StackBase = 0;
    if( SaveSP > 0x10000 ) {
        StackBase = SaveSP - 0x10000;
    }
    StackBase_64K = SaveSP;
    DPMISetSegmentBaseAddress( StackCacheSel, DataSelectorBase + StackBase );
    return( 0 );

} /* InitSelectorCache */

/*
 * FiniSelectorCache - clean up the selector cache
 */
void FiniSelectorCache( void )
{
    int i;

    for( i = 0; i < MAX_CACHE; i++ ) {
        if( aliasCache[i].sel != NULL ) {
            DPMIFreeLDTDescriptor( aliasCache[i].sel );
        }
    }
    DPMIFreeLDTDescriptor( StackCacheSel );
    DPMIFreeLDTDescriptor( Int21Selector );

} /* FiniSelectorCache */


/*
 * FiniSelList - free selector list array
 */
void FiniSelList( void )
{
    int             i;
    int             j;
    WORD            sel;
    unsigned char   mask;

    i = currSelCount;
    j = 0;
    while( i > 0 ) {
        if( SelBitArray[j] != 0 ) {
            mask = SelBitArray[j];
            sel = (j << (3 + 3)) | (firstCacheSel & 7);
            while( mask != 0 ) {
                if( mask & 1 ) {
                    DPMIFreeLDTDescriptor( sel );
                    --i;
                    if( i == 0 ) {
                        return;
                    }
                }
                sel += 8;
                mask = mask >> 1;
            }
        }
        ++j;
    }
} /* FiniSelList */
