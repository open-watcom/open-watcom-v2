/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include "alloc.h"
#include "reloc.h"
#include "fileio.h"
#include "spillio.h"
#include "loadfile.h"
#include "overlays.h"
#include "exeflat.h"


/* note: if either of these two structures get any bigger, the magic constants
 * in the RLIDX_* macros will have to change to ensure that no allocation > 64k
 * occurs. */

typedef struct reloc_info {
    struct reloc_info   *next;
    size_t              sizeleft;
    spilladdr           loc;
} reloc_info;

typedef struct {
    reloc_info          *externals; /* external and segment style fixups */
    reloc_info          *internals; /* internal, non-segment fixups */
} os2_reloc_header;

#define RELOC_PAGE_SIZE 512

unsigned        FmtRelocSize;
#ifdef _QNX
reloc_info      *FloatFixups;
#endif

static bool     SpillAreas( OVL_AREA *ovl, bool (*rtn)( section * ) );

void ResetReloc( void )
/*********************/
{
#ifdef _QNX
    FloatFixups = NULL;
#endif
}

static reloc_info *AllocRelocInfo( void )
/****************************************
 * allocate a relocation information block
 */
{
    reloc_info      *info;

    _PermAlloc( info, sizeof( reloc_info ) );       /* allocate more */
    info->sizeleft = RELOC_PAGE_SIZE;
    info->loc.spilled = false;
    _LnkAlloc( info->loc.u.addr, RELOC_PAGE_SIZE );
    if( info->loc.u.addr == NULL ) {
        info->loc.u.spill = SpillAlloc( RELOC_PAGE_SIZE );
        info->loc.spilled = true;
    }
    return( info );
}

static void *OS2PagedRelocInit( offset size, int unitsize )
/**********************************************************
 * For some OS/2 formats we have to split up the structure off the grp_relocs
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
    _PermAlloc( mem, ( idxhigh + 1 ) * sizeof( void * ) );
    start = mem;
    allocsize = OSF_RLIDX_MAX * unitsize;
    while( idxhigh-- > 0 ) {
        _ChkAlloc( *mem, allocsize );
        memset( *mem, 0, allocsize );
        mem++;
    }
    idxlow = OSF_RLIDX_LOW( pageidx );
    if( idxlow != 0 ) {
        allocsize = idxlow * unitsize;
        _ChkAlloc( *mem, allocsize );
        memset( *mem, 0, allocsize );
    }
    return( start );
}

static void *OS2FlatRelocInit( offset size )
/*******************************************
 * initialize relocations for OS2 flat memory manager. 
 */
{
    return( OS2PagedRelocInit( size, sizeof( os2_reloc_header ) ) );
}

static void *PERelocInit( offset size )
/**************************************
 * initialize relocations for PE executable format
 */
{
    return( OS2PagedRelocInit( size, sizeof( reloc_info * ) ) );
}

static void DoWriteReloc( void *lst, const void *reloc, size_t size )
/*******************************************************************/
{
    reloc_info      **list = lst;
    reloc_info      *info;
    size_t          offset;

    info = *(reloc_info **)list;
    if( info == NULL ) {
        info = AllocRelocInfo();
        info->next = NULL;
        *list = info;
    }
    if( info->sizeleft < size ) {     /* if no space */
        info = AllocRelocInfo();
        info->next = *list;
        *list = info;
    }
    offset = RELOC_PAGE_SIZE - info->sizeleft;
    if( info->loc.spilled ) {
        SpillWrite( info->loc.u.spill, offset, reloc, size );
    } else {
        memcpy( info->loc.u.addr + offset, reloc, size );
    }
    info->sizeleft -= size;
}

void WriteReloc( group_entry *group, offset off, void *reloc, size_t size )
/**************************************************************************
 * write the given relocation to virtual memory
 */
{
#ifdef _OS2
    os2_reloc_header    **pagelist;
    reloc_info          ***reloclist;
    reloc_info          **header;
    unsigned_32         idx;

    if( FmtData.type & MK_PE ) {
        reloclist = group->g.grp_relocs;
        if( reloclist == NULL ) {
           reloclist = PERelocInit( group->totalsize );
           group->g.grp_relocs = reloclist;
        }
        idx = ( off - group->grp_addr.off ) >> OSF_PAGE_SHIFT;
        header = &reloclist[OSF_RLIDX_HIGH( idx )][OSF_RLIDX_LOW( idx )];
        DoWriteReloc( header, reloc, size );
        group->section->relocs++;
        return;
    }
    if( FmtData.type & MK_OS2_FLAT ) {
        pagelist = group->g.grp_relocs;
        if( pagelist == NULL ) {
            pagelist = OS2FlatRelocInit( group->totalsize );
            group->g.grp_relocs = pagelist;
        }
        idx = ( off - group->grp_addr.off ) >> OSF_PAGE_SHIFT;
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
    if( FmtData.type & MK_OS2_16BIT ) {
        DoWriteReloc( &group->g.grp_relocs, reloc, size );
        group->section->relocs++;
        return;
    }
#endif
#ifdef _ELF
    if( FmtData.type & MK_ELF ) {
        DoWriteReloc( &group->g.grp_relocs, reloc, size );
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
    DoWriteReloc( &group->g.grp_relocs, item, sizeof( *item ) );
}
#endif

static bool FreeRelocList( reloc_info *list )
/********************************************
 * free any reloc blocks pointed to by list
 */
{
    for( ; list != NULL; list = list->next ) {
        if( !list->loc.spilled ) {
            _LnkFree( list->loc.u.addr );
        }
    }
    return( false );  /* needed for OS2 generic traversal routines */
}

static void FreeRelocSect( section *sect )
/****************************************/
{
    FreeRelocList( sect->reloclist );
}

static bool TraverseRelocBlock( reloc_info ** reloclist, unsigned num,
                                bool (*fn)( reloc_info * ) )
/********************************************************************/
{
    while( num-- > 0 ) {
        if( fn( *reloclist++ ) )
            return( true );
        if( FmtData.type & MK_OS2_FLAT ) {
            if( fn( *reloclist++ ) ) {
                return( true );
            }
        }
    }
    return( false );
}

bool TraverseOS2RelocList( group_entry *group, bool (*fn)( reloc_info * ) )
/**************************************************************************
 * traverse all items in one of the big OS2 page relocation lists
 */
{
    unsigned_32         index;
    unsigned_32         highidx;
    unsigned            lowidx;
    reloc_info          ***reloclist;

    reloclist = group->g.grp_relocs;
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
    reloc_info          ***reloclist;
#endif

#if !defined( _OS2 ) && !defined( _ELF ) && !defined( _QNX )
    /* unused parameters */ (void)group;
#endif

    if( (LinkState & LS_MAKE_RELOCS) == 0 ) {
        return;
    }
#ifdef _OS2
    if( FmtData.type & (MK_OS2_FLAT | MK_PE) ) {
        TraverseOS2RelocList( group, FreeRelocList );
        reloclist = group->g.grp_relocs;
        if( reloclist != NULL ) {
            index = OSF_PAGE_COUNT( group->totalsize );
            highidx = OSF_RLIDX_HIGH( index );
            if( OSF_RLIDX_LOW( index ) != 0 ) {
                highidx++;
            }
            while( highidx-- > 0 ) {
                _LnkFree( *reloclist );
                reloclist++;
            }
        }
        return;
    } else if( FmtData.type & MK_OS2_16BIT ) {
        FreeRelocList( group->g.grp_relocs );
        return;
    }
#endif
#if defined( _ELF ) || defined( _QNX )
    if( FmtData.type & (MK_QNX | MK_ELF) ) {
        FreeRelocList( group->g.grp_relocs );
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
    if( FmtData.type & (MK_ELF | MK_OS2_FLAT | MK_PE | MK_OS2_16BIT | MK_QNX) ) {
        for( group = Groups; group != NULL; group = group->next_group ) {
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

unsigned_32 RelocSize( reloc_info *list )
/****************************************
 * find the size of all the relocations stored here
 */
{
    unsigned_32 size;

    size = 0;
    for( ; list != NULL; list = list->next ) {
        size += RELOC_PAGE_SIZE - list->sizeleft;
    }
    return( size );
}

unsigned_32 DumpMaxRelocList( reloc_info **head, unsigned_32 max )
/*****************************************************************
 * write the given reloc information list to loadfile
 */
{
    unsigned_32         size;
    unsigned_32         total;
    reloc_info          *list;

    total = 0;
    for( list = *head; list != NULL; list = list->next ) {
        size = RELOC_PAGE_SIZE - list->sizeleft;
        if( ( max != 0 ) && ( total != 0 ) && ( ( total + size ) >= max ) )
            break;
        if( size ) {
            if( list->loc.spilled ) {
                SpillRead( list->loc.u.spill, 0, TokBuff, size );
                WriteLoad( TokBuff, size );
            } else {
                WriteLoad( list->loc.u.addr, size );
            }
        }
        total += size;
    }
    *head = list;
    return( total );
}

bool DumpRelocList( reloc_info *list )
/************************************/
{
    DumpMaxRelocList( &list, 0 );
    return( false );            /* so traverse works */
}

unsigned_32 WalkRelocList( reloc_info **head, bool (*fn)( void *data, size_t size, void *ctx ), void *ctx )
/**********************************************************************************************************
 * walk the given reloc information list and call user fn for each reloc
 */
{
    size_t              size;
    unsigned_32         total;
    reloc_info          *list;
    bool                quit = false;

    total = 0;
    for( list = *head; list != NULL; list = list->next ) {
        if( quit )
            break;
        size = RELOC_PAGE_SIZE - list->sizeleft;
        if( size ) {
            if( list->loc.spilled ) {
                SpillRead( list->loc.u.spill, 0, TokBuff, size );
                quit = fn( TokBuff, size, ctx );
            } else {
                quit = fn( list->loc.u.addr, size, ctx );
            }
        }
        total += (unsigned_32)size;
    }
    *head = list;
    return( total );
}

void SetRelocSize( void )
/***********************/
{
#ifdef _OS2
    if( FmtData.type & ( MK_OS2 | MK_WIN_VXD ) ) {
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
#ifdef _ZDOS
    if( FmtData.type & MK_ZDOS ) {
        FmtRelocSize = sizeof( zdos_reloc_item );
        return;
    }
#endif
    FmtRelocSize = sizeof( dos_addr );
}

static bool SpillRelocList( reloc_info *list )
/*********************************************
 * spill any reloc blocks pointed to by list
 */
{
    virt_mem_size   spill;

    for( ; list != NULL; list = list->next ) {
        if( !list->loc.spilled ) {
            spill = SpillAlloc( RELOC_PAGE_SIZE );
            SpillWrite( spill, 0, list->loc.u.addr, RELOC_PAGE_SIZE - list->sizeleft );
            _LnkFree( list->loc.u.addr );
            list->loc.u.spill = spill;
            list->loc.spilled = true;
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
    for( ; sect != NULL; sect = sect->next_sect ) {
        if( rtn( sect ) )
            return( true );
        if( SpillAreas( sect->areas, rtn ) ) {
            return( true );
        }
    }
    return( false );
}

static bool SpillAreas( OVL_AREA *ovl, bool (*rtn)( section * ) )
/***************************************************************/
{
    for( ; ovl != NULL; ovl = ovl->next_area ) {
        if( SpillSections( ovl->sections, rtn ) ) {
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
    if( FmtData.type & (MK_OS2_FLAT | MK_PE) ) {
        for( group = Groups; group != NULL; group = group->next_group ) {
            if( TraverseOS2RelocList( group, SpillRelocList ) ) {
                return( true );
            }
        }
    } else if( FmtData.type & (MK_OS2_16BIT | MK_QNX) ) {
        for( group = Groups; group != NULL; group = group->next_group ) {
            if( SpillRelocList( group->g.grp_relocs ) ) {
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
