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

static void *OS2PagedRelocInit( offset size, unsigned unitsize )
/***************************************************************
 * allocate simple array for all pages
 */
{
    void        *mem;
    unsigned    allocsize;

    allocsize = unitsize * OSF_PAGE_COUNT( size );
    mem = MemAllocSafe( allocsize );
    memset( mem, 0, allocsize );
    return( mem );
}

static os2_reloc_header *OS2FlatRelocInit( offset size )
/*******************************************************
 * initialize relocations for OS2 flat memory manager.
 */
{
    return( (os2_reloc_header *)OS2PagedRelocInit( size, sizeof( os2_reloc_header ) ) );
}

static reloc_info *PERelocInit( offset size )
/********************************************
 * initialize relocations for PE executable format
 */
{
    return( (reloc_info *)OS2PagedRelocInit( size, sizeof( reloc_info ) ) );
}

static void DoWriteReloc( reloc_info *reloclist_head, const void *reloc, size_t size )
/************************************************************************************/
{
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
    unsigned        idx;

    if( FmtData.type & MK_PE ) {
        reloc_info  *reloclist_array;
        reloc_info  *reloclist_head;

        reloclist_array = group->g.reloclist_array;
        if( reloclist_array == NULL ) {
           reloclist_array = PERelocInit( group->totalsize );
           group->g.reloclist_array = reloclist_array;
        }
        idx = ( off - group->addr.off ) >> OSF_PAGE_SHIFT;
        reloclist_head = &reloclist_array[idx];
        DoWriteReloc( reloclist_head, reloc, size );
        group->section->relocs++;
        return;
    }
    if( FmtData.type & (MK_OS2_FLAT | MK_WIN_VXD) ) {
        os2_reloc_header    *pagelist_array;
        reloc_info          *reloclist_head;

        pagelist_array = group->g.pagelist_array;
        if( pagelist_array == NULL ) {
            pagelist_array = OS2FlatRelocInit( group->totalsize );
            group->g.pagelist_array = pagelist_array;
        }
        idx = ( off - group->addr.off ) >> OSF_PAGE_SHIFT;
        reloclist_head = &pagelist_array[idx].externals;
        switch( ((os2_flat_reloc_item *)reloc)->nr_flags & OSF_TARGET_MASK )  {
        case OSF_TARGET_INTERNAL:
            switch( ((os2_flat_reloc_item *)reloc)->nr_stype ) {
            case OSF_SOURCE_OFF_16:
            case OSF_SOURCE_OFF_32:
            case OSF_SOURCE_OFF_32_REL:
                //NYI: don't have to write this out if we can figure out
                // how to tell the loader that we're doing it.
                reloclist_head = &pagelist_array[idx].internals;
                break;
            }
            break;
        }
        DoWriteReloc( reloclist_head, reloc, size );
        group->section->relocs++;
        return;
    }
    if( FmtData.type & MK_NE ) {
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

static void FreeRelocListSect( section *sect )
/********************************************/
{
    FreeRelocList( sect->reloclist );
}

bool TraverseOS2RelocList( group_entry *group, bool (*fn)(reloc_info) )
/**********************************************************************
 * traverse all items in one of the big OS2 page relocation lists
 */
{
    unsigned        numpages;

    numpages = OSF_PAGE_COUNT( group->totalsize );
    if( FmtData.type & (MK_OS2_FLAT | MK_WIN_VXD) ) {
        os2_reloc_header    *pagelist_array;

        pagelist_array = group->g.pagelist_array;
        if( pagelist_array != NULL ) {
            while( numpages-- > 0 ) {
                if( fn( pagelist_array->externals ) )
                    return( true );
                if( fn( pagelist_array->internals ) )
                    return( true );
                pagelist_array++;
            }
        }
    } else { /* FmtData.type & MK_PE */
        reloc_info  *reloclist_array;

        reloclist_array = group->g.reloclist_array;
        if( reloclist_array != NULL ) {
            while( numpages-- > 0 ) {
                if( fn( *reloclist_array++ ) ) {
                    return( true );
                }
            }
        }
    }
    return( false );
}

static void FreeGroupRelocs( group_entry *group )
/***********************************************/
{
#if !defined( _OS2 ) && !defined( _ELF ) && !defined( _QNX )
    /* unused parameters */ (void)group;
#endif

    if( (LinkState & LS_MAKE_RELOCS) == 0 ) {
        return;
    }
#ifdef _OS2
    if( FmtData.type & (MK_OS2_FLAT | MK_WIN_VXD | MK_PE) ) {
        TraverseOS2RelocList( group, FreeRelocList );
        if( group->g.reloclist_array != NULL ) {
            MemFree( group->g.reloclist_array );
        }
        return;
    }
    if( FmtData.type & MK_NE ) {
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
    group_entry     *group;

    if( (LinkState & LS_MAKE_RELOCS) == 0 )
        return;
    if( FmtData.type & (MK_ELF | MK_OS2_FLAT | MK_WIN_VXD | MK_PE | MK_NE | MK_QNX) ) {
        for( group = Groups; group != NULL; group = group->next ) {
            FreeGroupRelocs( group );
        }
    } else if( Root != NULL ) {
        WalkAllSects( FreeRelocListSect );
    }
#ifdef _QNX
    if( FmtData.type & MK_QNX ) {
        FreeRelocList( FloatFixups );
        FreeRelocListSect( Root );
    }
#endif
}

unsigned RelocSize( reloc_info reloclist )
/*****************************************
 * find the size of all the relocations stored here
 */
{
    unsigned        size;

    size = 0;
    for( ; reloclist != NULL; reloclist = reloclist->next ) {
        size += RELOC_PAGE_SIZE - reloclist->sizeleft;
    }
    return( size );
}

unsigned DumpMaxRelocList( reloc_info *reloclist_head, unsigned max )
/********************************************************************
 * write the given reloc information list to loadfile
 */
{
    unsigned        size;
    unsigned        total;
    reloc_info      reloclist;

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

unsigned WalkRelocList( reloc_info *reloclist_head, bool (*fn)( void *data, size_t size, void *ctx ), void *ctx )
/****************************************************************************************************************
 * walk the given reloc information list and call user fn for each reloc
 */
{
    size_t          size;
    unsigned        total;
    reloc_info      reloclist;
    bool            quit;

    quit = false;
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
        total += (unsigned)size;
    }
    *reloclist_head = reloclist;
    return( total );
}

void SetRelocSize( void )
/***********************/
{
#ifdef _OS2
    if( FmtData.type & ( MK_NE | MK_OS2_FLAT | MK_WIN_VXD ) ) {
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
    group_entry     *group;

    if( (LinkState & LS_FMT_DECIDED) == 0 )
        return( false );
    if( FmtData.type & (MK_OS2_FLAT | MK_WIN_VXD | MK_PE) ) {
        for( group = Groups; group != NULL; group = group->next ) {
            if( TraverseOS2RelocList( group, SpillRelocList ) ) {
                return( true );
            }
        }
    } else if( FmtData.type & (MK_NE | MK_QNX) ) {
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
