/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include <string.h>
#include "linkstd.h"
#include "reloc.h"
#include "fileio.h"
#include "spillio.h"
#include "loadfile.h"
#include "overlays.h"
#include "exeflat.h"


/* note: if either of these two structures get any bigger, the magic constants
 * in the RLIDX_* macros will have to change to ensure that no allocation > 64k
 * occurs. */

typedef struct reloc_info_struct {
    struct reloc_info_struct *next;
    size_t              sizeleft;
    spilladdr           loc;
} reloc_info_struct;

#define RELOC_PAGE_SIZE 512

unsigned        FmtRelocSize;
#ifdef _QNX
reloc_info      FloatFixups;
#endif

static bool     SpillAreas( AREASECT *area, bool (*rtn)( section * ) );

void ResetReloc( void )
/*********************/
{
#ifdef _QNX
    FloatFixups = NULL;
#endif
}

static reloc_info AllocRelocInfo( void )
/***************************************
 * allocate a relocation information block
 */
{
    reloc_info      reloclist;

    reloclist = _PermAlloc( sizeof( reloc_info_struct ) );       /* allocate more */
    reloclist->sizeleft = RELOC_PAGE_SIZE;
    reloclist->loc.spilled = false;
    reloclist->loc.u.addr = MemAlloc( RELOC_PAGE_SIZE );
    if( reloclist->loc.u.addr == NULL ) {
        reloclist->loc.u.spill = SpillAlloc( RELOC_PAGE_SIZE );
        reloclist->loc.spilled = true;
    }
    return( reloclist );
}

static void *OS2PagedRelocInit( offset size, int unitsize )
/**********************************************************
 * For some OS/2 formats we have to split up the structure off the reloclist
 * field up into small bits to ensure that we don't get structure allocations
 * > 64K. This is stored basically as a 2-d array
 */
{
    void **     mem;
    void **     start;
    offset      pageidx;
    offset      idxhigh;
    unsigned    idxlow;
    unsigned    allocsize;

    pageidx = OSF_PAGE_COUNT( size );
    idxhigh = OSF_RLIDX_HIGH( pageidx );
    mem = _PermAlloc( ( idxhigh + 1 ) * sizeof( void * ) );
    start = mem;
    allocsize = OSF_RLIDX_MAX * unitsize;
    while( idxhigh-- > 0 ) {
        *mem = MemAllocSafe( allocsize );
        memset( *mem, 0, allocsize );
        mem++;
    }
    idxlow = OSF_RLIDX_LOW( pageidx );
    if( idxlow != 0 ) {
        allocsize = idxlow * unitsize;
        *mem = MemAllocSafe( allocsize );
        memset( *mem, 0, allocsize );
    }
    return( start );
}

static os2_reloc_header **OS2FlatRelocInit( offset size )
/********************************************************
 * initialize relocations for OS2 flat memory manager.
 */
{
    return( (os2_reloc_header **)OS2PagedRelocInit( size, sizeof( os2_reloc_header ) ) );
}

static void *PERelocInit( offset size )
/**************************************
 * initialize relocations for PE executable format
 */
{
    return( OS2PagedRelocInit( size, sizeof( reloc_info ) ) );
}

static void DoWriteReloc( void *lst, const void *reloc, size_t size )
/*******************************************************************/
{
    reloc_info      *reloclist_head = (reloc_info *)lst;
    reloc_info      reloclist;
    size_t          offset;

    reloclist = *reloclist_head;
    if( reloclist == NULL ) {
        reloclist = AllocRelocInfo();
        reloclist->next = NULL;
        *reloclist_head = reloclist;
    }
    if( reloclist->sizeleft < size ) {     /* if no space */
        reloclist = AllocRelocInfo();
        reloclist->next = *reloclist_head;
        *reloclist_head = reloclist;
    }
    offset = RELOC_PAGE_SIZE - reloclist->sizeleft;
    if( reloclist->loc.spilled ) {
        SpillWrite( reloclist->loc.u.spill, offset, reloc, size );
    } else {
        memcpy( reloclist->loc.u.addr + offset, reloc, size );
    }
    reloclist->sizeleft -= size;
}

void WriteReloc( group_entry *group, offset off, void *reloc, size_t size )
/**************************************************************************
 * write the given relocation to virtual memory
 */
{
#ifdef _OS2
    unsigned_32         idx;

    if( FmtData.type & MK_PE ) {
        reloc_info  **reloclist;
        reloc_info  *header;

        reloclist = group->g.reloclist;
        if( reloclist == NULL ) {
           reloclist = PERelocInit( group->totalsize );
           group->g.reloclist = reloclist;
        }
        idx = ( off - group->addr.off ) >> OSF_PAGE_SHIFT;
        header = &reloclist[OSF_RLIDX_HIGH( idx )][OSF_RLIDX_LOW( idx )];
        DoWriteReloc( header, reloc, size );
        group->section->relocs++;
        return;
    }
    if( FmtData.type & (MK_OS2_FLAT | MK_WIN_VXD) ) {
        os2_reloc_header    **pagelist;
        reloc_info          *header;

        pagelist = group->g.pagelist;
        if( pagelist == NULL ) {
            pagelist = OS2FlatRelocInit( group->totalsize );
            group->g.pagelist = pagelist;
        }
        idx = ( off - group->addr.off ) >> OSF_PAGE_SHIFT;
        header = &pagelist[OSF_RLIDX_HIGH( idx )][OSF_RLIDX_LOW( idx )].externals;
        switch( ((os2_flat_reloc_item *)reloc)->nr_flags & OSF_TARGET_MASK )  {
        case OSF_TARGET_INTERNAL:
            switch( ((os2_flat_reloc_item *)reloc)->nr_stype ) {
            case OSF_SOURCE_OFF_16:
            case OSF_SOURCE_OFF_32:
            case OSF_SOURCE_OFF_32_REL:
                //NYI: don't have to write this out if we can figure out
                // how to tell the loader that we're doing it.
                header = &pagelist[OSF_RLIDX_HIGH( idx )][OSF_RLIDX_LOW( idx )].internals;
                break;
            }
            break;
        }
        DoWriteReloc( header, reloc, size );
        group->section->relocs++;
        return;
    }
    if( FmtData.type & (MK_OS2_NE | MK_WIN_NE) ) {
        DoWriteReloc( &group->g.reloclist, reloc, size );
        group->section->relocs++;
        return;
    }
#endif
#ifdef _ELF
    if( FmtData.type & MK_ELF ) {
        DoWriteReloc( &group->g.reloclist, reloc, size );
        group->section->relocs++;
        return;
    }
#endif
    DoWriteReloc( &group->section->reloclist, reloc, size );
    group->section->relocs++;
}

#ifdef _QNX
void WriteQNXFloatReloc( qnx_reloc_item *item )
/*********************************************/
{
    DoWriteReloc( &FloatFixups, item, sizeof( *item ) );
}

void WriteQNXLinearReloc( group_entry *group, qnx_linear_item *item )
/*******************************************************************/
{
    DoWriteReloc( &group->g.reloclist, item, sizeof( *item ) );
}
#endif

static bool FreeRelocList( reloc_info reloclist )
/************************************************
 * free any reloc blocks pointed to by reloclist
 */
{
    for( ; reloclist != NULL; reloclist = reloclist->next ) {
        if( !reloclist->loc.spilled ) {
            MemFree( reloclist->loc.u.addr );
        }
    }
    return( false );  /* needed for OS2 generic traversal routines */
}

static void FreeRelocSect( section *sect )
/****************************************/
{
    FreeRelocList( sect->reloclist );
}

static bool TraverseRelocBlock( reloc_info *reloclist, unsigned num, bool (*fn)(reloc_info) )
/*******************************************************************************************/
{
    while( num-- > 0 ) {
        if( fn( *reloclist++ ) )
            return( true );
        if( FmtData.type & (MK_OS2_FLAT | MK_WIN_VXD) ) {
            if( fn( *reloclist++ ) ) {
                return( true );
            }
        }
    }
    return( false );
}

bool TraverseOS2RelocList( group_entry *group, bool (*fn)(reloc_info) )
/**********************************************************************
 * traverse all items in one of the big OS2 page relocation lists
 */
{
    unsigned_32         index;
    unsigned_32         highidx;
    unsigned            lowidx;
    reloc_info          **reloclist;

    reloclist = group->g.reloclist;
    if( reloclist != NULL ) {
        index = OSF_PAGE_COUNT( group->totalsize );
        for( highidx = OSF_RLIDX_HIGH( index ); highidx > 0; --highidx ) {
            if( TraverseRelocBlock( *reloclist, OSF_RLIDX_MAX, fn ) )
                return( true );
            reloclist++;
        }
        lowidx = OSF_RLIDX_LOW( index );
        if( lowidx > 0 ) {
            return( TraverseRelocBlock( *reloclist, OSF_RLIDX_LOW( index ), fn ) );
        }
    }
    return( false );
}

static void FreeGroupRelocs( group_entry *group )
/***********************************************/
{
#ifdef _OS2
    unsigned_32         highidx;
    unsigned_32         index;
    reloc_info          **reloclist;
#endif

#if !defined( _OS2 ) && !defined( _ELF ) && !defined( _QNX )
    /* unused parameters */ (void)group;
#endif

    if( (LinkState & LS_MAKE_RELOCS) == 0 ) {
        return;
    }
#ifdef _OS2
    if( FmtData.type & (MK_OS2_FLAT | MK_WIN_VXD | MK_PE) ) {
        TraverseOS2RelocList( group, FreeRelocList );
        reloclist = group->g.reloclist;
        if( reloclist != NULL ) {
            index = OSF_PAGE_COUNT( group->totalsize );
            highidx = OSF_RLIDX_HIGH( index );
            if( OSF_RLIDX_LOW( index ) != 0 ) {
                highidx++;
            }
            while( highidx-- > 0 ) {
                MemFree( *reloclist );
                reloclist++;
            }
        }
        return;
    }
    if( FmtData.type & (MK_OS2_NE | MK_WIN_NE) ) {
        FreeRelocList( group->g.reloclist );
        return;
    }
#endif
#if defined( _ELF ) || defined( _QNX )
    if( FmtData.type & (MK_QNX | MK_ELF) ) {
        FreeRelocList( group->g.reloclist );
    }
#endif
}

void FreeRelocInfo( void )
/*************************
 * free up blocks allocated for relocations
 */
{
    group_entry         *group;

    if( (LinkState & LS_MAKE_RELOCS) == 0 )
        return;
    if( FmtData.type & (MK_ELF | MK_OS2_FLAT | MK_WIN_VXD | MK_PE | MK_OS2_NE | MK_WIN_NE | MK_QNX) ) {
        for( group = Groups; group != NULL; group = group->next ) {
            FreeGroupRelocs( group );
        }
    } else if( Root != NULL ) {
        WalkAllSects( FreeRelocSect );
    }
#ifdef _QNX
    if( FmtData.type & MK_QNX ) {
        FreeRelocList( FloatFixups );
        FreeRelocSect( Root );
    }
#endif
}

unsigned_32 RelocSize( reloc_info reloclist )
/********************************************
 * find the size of all the relocations stored here
 */
{
    unsigned_32 size;

    size = 0;
    for( ; reloclist != NULL; reloclist = reloclist->next ) {
        size += RELOC_PAGE_SIZE - reloclist->sizeleft;
    }
    return( size );
}

unsigned_32 DumpMaxRelocList( reloc_info *reloclist_head, unsigned_32 max )
/**************************************************************************
 * write the given reloc information list to loadfile
 */
{
    unsigned_32         size;
    unsigned_32         total;
    reloc_info          reloclist;

    total = 0;
    for( reloclist = *reloclist_head; reloclist != NULL; reloclist = reloclist->next ) {
        size = RELOC_PAGE_SIZE - reloclist->sizeleft;
        if( ( max != 0 ) && ( total != 0 ) && ( ( total + size ) >= max ) )
            break;
        if( size ) {
            if( reloclist->loc.spilled ) {
                SpillRead( reloclist->loc.u.spill, 0, TokBuff, size );
                WriteLoad( TokBuff, size );
            } else {
                WriteLoad( reloclist->loc.u.addr, size );
            }
        }
        total += size;
    }
    *reloclist_head = reloclist;
    return( total );
}

bool DumpRelocList( reloc_info reloclist )
/****************************************/
{
    DumpMaxRelocList( &reloclist, 0 );
    return( false );            /* so traverse works */
}

unsigned_32 WalkRelocList( reloc_info *reloclist_head, bool (*fn)( void *data, size_t size, void *ctx ), void *ctx )
/*******************************************************************************************************************
 * walk the given reloc information list and call user fn for each reloc
 */
{
    size_t              size;
    unsigned_32         total;
    reloc_info          reloclist;
    bool                quit = false;

    total = 0;
    for( reloclist = *reloclist_head; reloclist != NULL; reloclist = reloclist->next ) {
        if( quit )
            break;
        size = RELOC_PAGE_SIZE - reloclist->sizeleft;
        if( size ) {
            if( reloclist->loc.spilled ) {
                SpillRead( reloclist->loc.u.spill, 0, TokBuff, size );
                quit = fn( TokBuff, size, ctx );
            } else {
                quit = fn( reloclist->loc.u.addr, size, ctx );
            }
        }
        total += (unsigned_32)size;
    }
    *reloclist_head = reloclist;
    return( total );
}

void SetRelocSize( void )
/***********************/
{
#ifdef _OS2
    if( FmtData.type & ( MK_OS2 | MK_WIN_NE | MK_WIN_VXD ) ) {
        FmtRelocSize = sizeof( os2_reloc_item );
        return;
    }
    if( FmtData.type & MK_PE ) {
        FmtRelocSize = sizeof( pe_reloc_item );
        return;
    }
#endif
#ifdef _NOVELL
    if( FmtData.type & MK_NOVELL ) {
        FmtRelocSize = sizeof( nov_reloc_item );
        return;
    }
#endif
#ifdef _PHARLAP
    if( FmtData.type & MK_PHAR_REX ) {
        FmtRelocSize = sizeof( rex_reloc_item );
        return;
    }
    if( FmtData.type & MK_PHAR_MULTISEG ) {
        FmtRelocSize = sizeof( pms_reloc_item );
        return;
    }
#endif
#ifdef _QNX
    if( FmtData.type & MK_QNX ) {
        FmtRelocSize = sizeof( qnx_reloc_item );
        return;
    }
#endif
#ifdef _ELF
    if( FmtData.type & MK_ELF ) {
        /*
         * elf_reloc_item contains pointer to symbol which gets
         * converted later on into index into symbol table
         */
        FmtRelocSize = sizeof( elf_reloc_item );
        return;
    }
#endif
    FmtRelocSize = sizeof( dos_addr );
}

static bool SpillRelocList( reloc_info reloclist )
/*************************************************
 * spill any reloc blocks pointed to by reloclist
 */
{
    virt_mem_size   spill;

    for( ; reloclist != NULL; reloclist = reloclist->next ) {
        if( !reloclist->loc.spilled ) {
            spill = SpillAlloc( RELOC_PAGE_SIZE );
            SpillWrite( spill, 0, reloclist->loc.u.addr, RELOC_PAGE_SIZE - reloclist->sizeleft );
            MemFree( reloclist->loc.u.addr );
            reloclist->loc.u.spill = spill;
            reloclist->loc.spilled = true;
            return( true );
        }
    }
    return( false );
}


static bool SpillSectRelocList( section *sect )
/*********************************************/
{
    return( SpillRelocList( sect->reloclist ) );
}

static bool SpillSections( section *sect, bool (*rtn)( section * ) )
/******************************************************************/
{
    for( ; sect != NULL; sect = sect->next ) {
        if( rtn( sect ) )
            return( true );
        if( SpillAreas( sect->areas, rtn ) ) {
            return( true );
        }
    }
    return( false );
}

static bool SpillAreas( AREASECT *area, bool (*rtn)( section * ) )
/****************************************************************/
{
    for( ; area != NULL; area = area->next ) {
        if( SpillSections( area->sections, rtn ) ) {
            return( true );
        }
    }
    return( false );
}

bool SwapOutRelocs( void )
/************************/
{
    group_entry         *group;

    if( (LinkState & LS_FMT_DECIDED) == 0 )
        return( false );
    if( FmtData.type & (MK_OS2_FLAT | MK_WIN_VXD | MK_PE) ) {
        for( group = Groups; group != NULL; group = group->next ) {
            if( TraverseOS2RelocList( group, SpillRelocList ) ) {
                return( true );
            }
        }
    } else if( FmtData.type & (MK_OS2_NE | MK_WIN_NE | MK_QNX) ) {
        for( group = Groups; group != NULL; group = group->next ) {
            if( SpillRelocList( group->g.reloclist ) ) {
                return( true );
            }
        }
#ifdef _QNX
        if( FmtData.type & MK_QNX ) {
            if( SpillRelocList( FloatFixups ) )
                return( true );
            return( SpillSectRelocList( Root ) );
        }
#endif
    } else {
        if( SpillSectRelocList( Root ) )
            return( true );
        if( SpillAreas( Root->areas, SpillSectRelocList ) ) {
            return( true );
        }
    }
    return( false );
}
