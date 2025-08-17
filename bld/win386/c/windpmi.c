/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DPMI function wrappers for Win386 (16-bit code).
*
****************************************************************************/


#include <stddef.h>
#include <windows.h>
#include "bool.h"
#include "winext.h"
#include "_windpmi.h"
#include "windpmi.h"
#include "windata.h"

/* Strange Windows 3.0 "386 enhanced mode" DPMI bug:
 * If ES, FS, or GS contain the selector being freed,
 * it will crash dump to DOS within the DPMI call to free an LDT descriptor.
 * The best way to avoid that crash dump is to zero those specific segment registers before the DPMI call.
 * --J,C. 2025/08/14 */
extern void Clear_ES_FS_GS( void );
#pragma aux Clear_ES_FS_GS = \
    "xor ax,ax" \
    "mov es,ax" \
    "mov fs,ax" \
    "mov gs,ax" \
__modify __exact [__ax __es __fs __gs]

#define MAX_CACHE       48
#define MAX_SELECTORS   8192

typedef struct {
    WORD        sel;
    WORD        limit;
    DWORD       base;
    bool        in_use;
} alias_cache_entry;

typedef struct memblk {
    struct memblk *next;
    DWORD       handle;
    DWORD       addr;
    DWORD       size;
} memblk;

static bool                     WrapAround;
static WORD                     hugeIncrement;
static WORD                     firstCacheSel;
static WORD                     lastCacheSel;
static WORD                     cacheUseCount;
static WORD                     StackCacheSel;
static DWORD                    StackBase;
static DWORD                    StackBase_64K;
static alias_cache_entry        aliasCache[MAX_CACHE];
static WORD                     currSelCount;
static BYTE                     SelBitArray[MAX_SELECTORS / 8];
static memblk                   *MemBlkList;

static BYTE         BitMask[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
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
 * _DPMI_GetAliases - get alias descriptors for some memory
 */
bool _DPMI_GetAliases( DWORD offs32, LPDWORD palias, WORD count )
{
    WORD                sel;
    WORD                i;
    DWORD               limit;
    DWORD               base;
    alias_cache_entry   *ace;
    dpmi_ret            dpmirc;

    *palias = 0L;
    if( offs32 == 0L ) {
        return( false );
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
    if( count == 1 ) {
        if( offs32 < StackBase_64K && offs32 >= StackBase ) {
            ALIAS_OFFS( palias ) = offs32 - StackBase;
            ALIAS_SEL( palias ) = StackCacheSel;
            return( false );
        }
        if( cacheUseCount < MAX_CACHE ) {
            ace = &aliasCache[0];
            for( i = 0; i < MAX_CACHE; i++ ) {
                if( !ace->in_use ) {
                    base = DataSelectorBase + offs32;
                    if( base != ace->base ) {
                        ace->base = base;
                        DPMISetSegmentBaseAddress( ace->sel, base );
                    }
                    ALIAS_SEL( palias ) = ace->sel;
                    ace->in_use = true;
                    cacheUseCount++;
                    return( false );
                }
                ace += 1;
            }
        }
    }
    if( count == 0 ) {
        count = 1;
    }

    /*
     * get a descriptor
     */
    dpmirc = DPMIAllocateLDTDescriptors( count );
    if( DPMI_ERROR( dpmirc ) ) {
        return( true );
    }
    sel = DPMI_INFO( dpmirc );
    ALIAS_SEL( palias ) = sel;
    limit = count * 0x10000 - 1;

    for( i = 0; i < count; i++ ) {

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
            DPMISetDescriptorAccessRights( sel, DPL + DESC_ACCESS_DATA16BIG );
        } else {
            DPMISetDescriptorAccessRights( sel, DPL + DESC_ACCESS_DATA16 );
        }
        DPMISetSegmentBaseAddress( sel, DataSelectorBase + offs32 );
        DPMISetSegmentLimit( sel, limit );
        addToSelList( sel );
        sel += hugeIncrement;
        offs32 += 0x10000;
        limit  -= 0x10000;
    }

    return( false );

} /* _DPMI_GetAliases */

/*
 * _DPMI_GetAlias - get alias descriptor for some memory
 */
bool _DPMI_GetAlias( DWORD offs32, LPDWORD palias )
{
    return( _DPMI_GetAliases( offs32, palias, 1 ) );

} /* _DPMI_GetAlias */

/*
 * _DPMI_FreeAlias - free alias descriptor
 */
void _DPMI_FreeAlias( DWORD alias )
{
    alias_cache_entry   *ace;
    WORD                sel;

    sel = ALIAS_SEL( &alias );
    if( sel == 0 || sel == StackCacheSel ) {
        return;
    }
    if( sel >= firstCacheSel && sel <= lastCacheSel ) {
        ace = &aliasCache[( sel - firstCacheSel ) / hugeIncrement];
        if( ace->in_use ) {
            ace->in_use = false;
            cacheUseCount--;
        }
        return;
    }
    removeFromSelList( sel );

    Clear_ES_FS_GS();
    WINDPMI_FreeLDTDescriptor( sel );

} /* _DPMI_FreeAlias */

bool _DPMI_GetHugeAlias( DWORD offs32, LPDWORD palias, DWORD size )
{
    DWORD       no64k;

    no64k = Align64K( size );
    return( _DPMI_GetAliases( offs32, palias, 1 + (WORD)( no64k / 0x10000L ) ) );
}

void _DPMI_FreeHugeAlias( DWORD alias, DWORD size )
{
    DWORD       no64k;
    WORD        count;
    WORD        sel;
    WORD        i;

    sel = ALIAS_SEL( &alias );
    if( sel == 0 ) {
        return;
    }
    no64k = Align64K( size );
    count = 1 + (WORD)( no64k / 0x10000L );
    for( i = 0; i < count; i++ ) {
        removeFromSelList( sel );
        WINDPMI_FreeLDTDescriptor( sel );
        sel += hugeIncrement;
    }
}

/*
 * WINDPMIFN( .. ) functions are the ones called by the 32-bit application
 */

bool WINDPMIFN( DPMIGetAlias )( DWORD offs32, LPDWORD palias )
{
    return( _DPMI_GetAlias( offs32, palias ) );
}

void WINDPMIFN( DPMIFreeAlias )( DWORD alias )
{
    _DPMI_FreeAlias( alias );
}

bool WINDPMIFN( DPMIGetHugeAlias )( DWORD offs32, LPDWORD palias, DWORD size )
{
    return( _DPMI_GetHugeAlias( offs32, palias, size ) );
}

void WINDPMIFN( DPMIFreeHugeAlias )( DWORD alias, DWORD size )
{
    _DPMI_FreeHugeAlias( alias, size );
}

/*
 * setLimitAndAddr - set the limit and address of a 32-bit selector
 */
static void setLimitAndAddr( WORD sel, DWORD addr, DWORD len, WORD type )
{

    DPMISetSegmentBaseAddress( sel, addr );
    --len;
    if( len >= 1024L * 1024L ) {
        if( type == DESC_ACCESS_CODE ) {
            DPMISetDescriptorAccessRights( sel, DPL + DESC_ACCESS_CODE32BIG );
        } else {
            DPMISetDescriptorAccessRights( sel, DPL + DESC_ACCESS_DATA32BIG );
        }
    } else {
        if( type == DESC_ACCESS_CODE ) {
            DPMISetDescriptorAccessRights( sel, DPL + DESC_ACCESS_CODE32SMALL );
        } else {
            DPMISetDescriptorAccessRights( sel, DPL + DESC_ACCESS_DATA32SMALL );
        }
    }
    DPMISetSegmentLimit( sel, len );

} /* setLimitAndAddr */

/*
 * InitFlatAddrSpace - initialize flat address space
 */
bool InitFlatAddrSpace( DWORD baseaddr, DWORD len )
{
    descriptor  desc;
    dpmi_ret    dpmirc;

    hugeIncrement = DPMIGetNextSelectorIncrementValue();
    /*
     * get a code selector pointing to the memory
     */
    dpmirc = DPMIAllocateLDTDescriptors( 1 );
    if( DPMI_ERROR( dpmirc ) ) {
        return( true );
    }
    CodeEntry.seg = DPMI_INFO( dpmirc );
    setLimitAndAddr( CodeEntry.seg, baseaddr, len, DESC_ACCESS_CODE );
    CodeSelectorBase = baseaddr;
    /*
     * get a data and stack selector pointing to the memory
     */
    dpmirc = DPMIAllocateLDTDescriptors( 2 );
    if( DPMI_ERROR( dpmirc ) ) {
        WINDPMI_FreeLDTDescriptor( CodeEntry.seg );
        return( true );
    }
    DataSelector = DPMI_INFO( dpmirc );
    StackSelector = DataSelector + hugeIncrement;
    /*
     *  The code generator sometimes uses EBP as general purpose
     *  register for accessing data that is not in the STACK segment
     *  so we must access the same space as DATA segment
     */
    setLimitAndAddr( DataSelector, baseaddr, len, DESC_ACCESS_DATA );
    setLimitAndAddr( StackSelector, baseaddr, len, DESC_ACCESS_DATA );
    WrapAround = false;
    if( DPMIGetDescriptor( DataSelector, &desc ) == 0 ) {
        if( GET_DESC_LIMIT_NUM( desc ) == 0x000FFFFF ) {
            WrapAround = true;
        } else {
            WrapAround = false;
        }
    }
    return( false );

} /* InitFlatAddrSpace */

/*
 * _DPMI_Get32 - get a 32-bit segment
 */
bool _DPMI_Get32( dpmi_mem_block _DLLFAR *adata, DWORD len )
{
    return( DPMIAllocateMemoryBlock( adata, len ) != 0 );

} /* _DPMI_Get32 */

/*
 * _DPMI_Free32 - free a 32-bit handle
 */
void _DPMI_Free32( DWORD handle )
{
    DPMIFreeMemoryBlock( handle );

} /* _DPMI_Free32 */

/*
 * WINDPMIFN( DPMIAlloc ) function - allocate a new block of memory
 * called by the 32-bit application
 */
DWORD WINDPMIFN( DPMIAlloc )( DWORD size )
{
    dpmi_mem_block  adata;
    memblk          *p;

    for( ;; ) {
        if( _DPMI_Get32( &adata, size ) ) {
            adata.linear = DataSelectorBase;        // cause NULL to be returned
            break;
        }
        p = (memblk *)LocalAlloc( LMEM_FIXED, sizeof( memblk ) );
        if( p == NULL ) {
            _DPMI_Free32( adata.handle );
            adata.linear = DataSelectorBase;        // cause NULL to be returned
            break;
        }
        p->next = MemBlkList;
        p->handle = adata.handle;
        p->addr   = adata.linear;
        p->size   = size;
        MemBlkList = p;
        /*
         * if we are on NT or OS/2, try again until we get a memory
         * block with address higher than our DataSelectorBase
         */
        if( WrapAround || adata.linear >= DataSelectorBase ) {
            break;
        }
    }
    if( !WrapAround && MemBlkList != NULL ) {
        /*
         * free up any memory allocated that is below DataSelectorBase
         */
        while( (p = MemBlkList->next) != NULL ) {
            if( p->addr >= DataSelectorBase )
                break;
            _DPMI_Free32( p->handle );
            MemBlkList->next = p->next;
            LocalFree( (HLOCAL)p );
        }
        p = MemBlkList;
        if( p->addr < DataSelectorBase ) {
            _DPMI_Free32( p->handle );
            MemBlkList = p->next;
            LocalFree( (HLOCAL)p );
        }
    }
    return( adata.linear - DataSelectorBase ); // return address of memory block
}

/*
 * WINDPMIFN( DPMIFree ) function - free a block of memory allocated by WINDPMIFN( DPMIAlloc )
 * called by the 32-bit application
 */
bool WINDPMIFN( DPMIFree )( DWORD addr )
{
    memblk      *p;
    memblk      *prev;

    addr += DataSelectorBase;           // add base address
    prev = NULL;
    for( p = MemBlkList; p != NULL; p = p->next ) {
        if( p->addr == addr ) {
            _DPMI_Free32( p->handle );
            if( prev == NULL ) {
                MemBlkList = p->next;
            } else {
                prev->next = p->next;
            }
            LocalFree( (HLOCAL)p );
            return( false );            // indicate success
        }
        prev = p;
    }
    return( true );                     // indicate error
}

void FreeDPMIMemBlocks( void )
{
    memblk      *p;

    while( (p = MemBlkList) != NULL ) {
        MemBlkList = p->next;
        _DPMI_Free32( p->handle );
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
bool InitSelectorCache( void )
{
    WORD        sel;
    int         i;
    dpmi_ret    dpmirc;

    dpmirc = DPMIAllocateLDTDescriptors( MAX_CACHE + 2 );
    if( DPMI_ERROR( dpmirc ) ) {
        return( true );
    }
    sel = DPMI_INFO( dpmirc );
    firstCacheSel = sel;
    for( i = 0; i < MAX_CACHE + 2; i++ ) {
        if( i < MAX_CACHE ) {
            aliasCache[i].sel = sel;
            aliasCache[i].limit = 0xFFFFL;
            aliasCache[i].base = 0L;
            aliasCache[i].in_use = false;
            lastCacheSel = sel;
        }
        DPMISetDescriptorAccessRights( sel, DPL + DESC_ACCESS_DATA16 );
        DPMISetSegmentLimit( sel, 0xFFFFL );
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
    return( false );

} /* InitSelectorCache */

/*
 * FiniSelectorCache - clean up the selector cache
 */
void FiniSelectorCache( void )
{
    int i;

    for( i = 0; i < MAX_CACHE; i++ ) {
        if( aliasCache[i].sel != NULL ) {
            WINDPMI_FreeLDTDescriptor( aliasCache[i].sel );
        }
    }
    WINDPMI_FreeLDTDescriptor( StackCacheSel );
    WINDPMI_FreeLDTDescriptor( Int21Selector );

} /* FiniSelectorCache */


/*
 * FiniSelList - free selector list array
 */
void FiniSelList( void )
{
    WORD            i;
    WORD            j;
    WORD            sel;
    BYTE            mask;

    i = currSelCount;
    j = 0;
    while( i > 0 ) {
        if( SelBitArray[j] != 0 ) {
            mask = SelBitArray[j];
            sel = (j << (3 + 3)) | (firstCacheSel & 7);
            while( mask != 0 ) {
                if( mask & 1 ) {
                    WINDPMI_FreeLDTDescriptor( sel );
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
