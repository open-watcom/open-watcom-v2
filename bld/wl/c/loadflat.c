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
* Description:  Utilities for creation of OS/2 flat (LX) executable files.
*
****************************************************************************/


#include <string.h>
#include "linkstd.h"
#include "reloc.h"
#include "exeos2.h"
#include "exeflat.h"
#include "loados2.h"
#include "loadfile.h"
#include "specials.h"
#include "alloc.h"
#include "virtmem.h"
#include "fileio.h"
#include "impexp.h"
#include "ring.h"
#include "dbgall.h"
#include "vxd_ddb.h"

#define STUB_ALIGN 16

#define PAGE_COUNT( size )  (((size)+(OSF_DEF_PAGE_SIZE-1))>>OSF_PAGE_SHIFT)
#define PAGEMAP_BUF_SIZE (MAX_HEADROOM / sizeof(map_entry) * (unsigned long)OSF_DEF_PAGE_SIZE)


static unsigned NumberBuf( unsigned_32 *start, unsigned_32 limit,
                           map_entry *buf )
/**************************************************************/
/* fill a buffer with consecutive numbers */
{
    unsigned    size;
    unsigned_32 num;
    unsigned    shift;

    num = PAGE_COUNT(limit);
    if( FmtData.type & (MK_OS2_LE|MK_WIN_VXD) ) {
        size = num * sizeof( le_map_entry );
        while( num > 0 ) {
            *start += 1;
            buf->le.page_num[2] = *start;
            buf->le.page_num[1] = *start >> 8;
            buf->le.page_num[0] = *start >> 16;
            buf->le.flags = PAGE_VALID; //NYI: have to figure out how to fill in
            buf = (map_entry *)((char *)buf + sizeof(le_map_entry));
            num--;
        }
    } else {
        shift = FmtData.u.os2.segment_shift;
        size = num * sizeof( lx_map_entry );
        while( limit > 0 ) {
            buf->lx.page_offset = *start >> shift;
            if( limit > OSF_DEF_PAGE_SIZE ) {
                buf->lx.data_size = OSF_DEF_PAGE_SIZE;
                limit -= OSF_DEF_PAGE_SIZE;
            } else {
                buf->lx.data_size = ROUND_SHIFT( limit, shift );
                limit = 0;
            }
            *start += buf->lx.data_size;
            buf->lx.flags = PAGE_VALID; //NYI: have to figure out how to fill in
            buf = (map_entry *)((char *)buf + sizeof(lx_map_entry));
        }
    }
    return( size );
}

static unsigned_32 WriteObjectTables( os2_flat_header *header,unsigned long loc)
/******************************************************************************/
/* write the object table and the object page map */
{
    unsigned_32     numpages;
    unsigned_32     numobjects;
    unsigned_32     size;
    unsigned_32     start;
    unsigned_32     sizeleft;
    group_entry *   group;
    object_record   objrec;
    unsigned        map_size;
    seg_leader *    leader;

    header->objtab_off = loc;
    numpages = 0;
    numobjects = 0;
    objrec.reserved = 0;
    if( DataGroup != NULL ) {
        header->autodata_obj = DataGroup->grp_addr.seg;
    } else {
        header->autodata_obj = 0;
    }
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->totalsize == 0 ) continue;   // DANGER DANGER DANGER <--!!!
        if( group->grp_addr.seg == StackAddr.seg ) {
            header->stack_obj = StackAddr.seg;
            header->esp = StackAddr.off - group->grp_addr.off;
        }
        if( group->grp_addr.seg == StartInfo.addr.seg ) {
            header->start_obj = StartInfo.addr.seg;
            header->eip = StartInfo.addr.off - group->grp_addr.off;
        }
        objrec.size = group->totalsize;
        objrec.addr = group->linear;
        objrec.flags = 0;
        /* segflags are in OS/2 V1.x format, we have to translate them
            into the appropriate V2.0 bits */
        leader = Ring2First( group->leaders );
        if( leader->info & USE_32 ) {
            objrec.flags |= OBJ_BIG;
        } else { //if( group->u.miscflags & SEG_16_ALIAS ) {
            objrec.flags |= OBJ_ALIAS_REQUIRED;
        }
        if( (group->segflags & SEG_LEVEL_MASK) != SEG_LEVEL_3 ) {
            objrec.flags |= OBJ_IOPL;
        }
        if( group->segflags & SEG_DATA ) {
            objrec.flags |= OBJ_READABLE;
            if( !(group->segflags & SEG_READ_ONLY) ) {
                objrec.flags |= OBJ_WRITEABLE;
            }
        } else {
            objrec.flags |= OBJ_EXECUTABLE;
            if( !(group->segflags & SEG_READ_ONLY) ) {
                objrec.flags |= OBJ_READABLE;
            }
        }
        if( group->segflags & SEG_DISCARD ) {
            objrec.flags |= OBJ_DISCARDABLE;
        }
        if( group->segflags & SEG_PURE || (group == DataGroup &&
                         FmtData.u.os2.flags & SINGLE_AUTO_DATA) ) {
            objrec.flags |= OBJ_SHARABLE;
        }
        if( group->segflags & SEG_PRELOAD ) {
            objrec.flags |= OBJ_HAS_PRELOAD;
        }
        if( group->segflags & SEG_CONFORMING ) {
            objrec.flags |= OBJ_CONFORMING;
        }
        if( group->segflags & SEG_INVALID ) {
            objrec.flags |= OBJ_HAS_INVALID;
        }
        objrec.flags |= (group->segflags & SEG_RESIDENT_MASK)
                                                 << SEG_RESIDENT_SHIFT;
        objrec.mapidx = numpages + 1;
        objrec.mapsize = PAGE_COUNT( group->size );
        WriteLoad( &objrec, sizeof( object_record ) );
        numpages += objrec.mapsize;
        numobjects++;
    }
    header->num_objects = numobjects;
    header->num_pages = numpages;
    size = numobjects * sizeof(object_record);
    loc += size;
    header->objmap_off = loc;
    start = 0;
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->totalsize == 0 ) continue;   // DANGER DANGER DANGER <--!!!
        sizeleft = group->size;
        while( sizeleft > PAGEMAP_BUF_SIZE ) {
            map_size = NumberBuf( &start, PAGEMAP_BUF_SIZE,
                                  (map_entry *)TokBuff );
            size += map_size;
            WriteLoad( TokBuff, map_size );
            sizeleft -= PAGEMAP_BUF_SIZE;
        }
        map_size = NumberBuf( &start, sizeleft, (map_entry *)TokBuff );
        size += map_size;
        WriteLoad( TokBuff, map_size );
    }
    return( size );
}

/*
 * NOTE: The routine DumpEntryTable in LOADOS2.C is very similar to this
 *       one, however there are enough differences to preclude the use
 *       of one routine to dump both tables. Therefore any logic bugs that
 *       occur in this routine will likely have to be fixed in the other
 *       one as well.
 */
static unsigned long DumpFlatEntryTable( void )
/*********************************************/
/* Dump the entry table to the file */
{
    entry_export        *start;
    entry_export        *place;
    entry_export        *prev;
    unsigned            prevord;
    unsigned long       size;
    unsigned            gap;
    unsigned            entries;
    union {
        flat_bundle_prefix      real;
        flat_null_prefix        null;
    }                   prefix;
    flat_bundle_entry32 bundle_item;
    flat_bundle_entryfwd bundle_fwd;

    size = 0;
    start = FmtData.u.os2.exports;
    if( start != NULL ) {
        prevord = 0;
        place = start;
        while( place != NULL ) {
            gap = place->ordinal - prevord;
            if( gap > 1 ) {  // fill in gaps in ordinals.
                gap--;       // fix 'off by 1' problem.
                prefix.null.b32_cnt = 0xFF;
                prefix.null.b32_type = FLT_BNDL_EMPTY;   // Null bundles.
                while( gap > 0xff ) {
                    WriteLoad( &prefix.null, sizeof( prefix.null ) );
                    gap -= 0xFF;
                    size += sizeof( prefix.null );
                }
                prefix.null.b32_cnt = (unsigned_8) gap;
                WriteLoad( &prefix.null, sizeof( prefix.null ) );
                size += sizeof( prefix.null );
            }
            // now get a bundle of ordinals.
            entries = 1;
            prev = start = place;
            place = place->next;
            while( place != NULL ) {
                if( entries >= 0xff ) break;
                if( place->addr.seg != start->addr.seg ) break;
                if( place->ordinal - prev->ordinal > 1 ) {
                    break;    // ordinal can't be put in this bundle.
                }
                entries++;
                prev = place;
                place = place->next;
            }
            prefix.real.b32_cnt = entries;
            if (start->addr.seg == 0xFFFF) {
                // Forwarder entry
                prefix.real.b32_type = FLT_BNDL_ENTRYFWD;
                prefix.real.b32_obj = 0;
                size += entries * sizeof( flat_bundle_entryfwd );
            } else {
                // 32-bit entry
                prefix.real.b32_type = FLT_BNDL_ENTRY32;
                prefix.real.b32_obj = start->addr.seg;
                size += entries * sizeof( flat_bundle_entry32 );
            }
            prevord = prev->ordinal;
            WriteLoad( &prefix.real, sizeof( prefix.real ) );
            size += sizeof( prefix.real );
            for( ; entries > 0; --entries, start = start->next ) {
                if (start->addr.seg == 0xFFFF) {
                    // Forwarder entry
                    dll_sym_info   *dll = start->sym->p.import;

                    bundle_fwd.e32_flags = 0;
                    bundle_fwd.modord    = dll->m.modnum->num;
                    if (dll->isordinal) {
                        bundle_fwd.e32_flags |= 1;
                        bundle_fwd.value     = dll->u.ordinal;
                    } else {
                        bundle_fwd.value     = dll->u.entry->num;
                    }
                    WriteLoad( &bundle_fwd, sizeof( flat_bundle_entryfwd ) );
                } else {
                    // 32-bit entry
                    bundle_item.e32_flags = (start->iopl_words << IOPL_WORD_SHIFT);
                    if( start->isexported ) bundle_item.e32_flags |= ENTRY_EXPORTED;
                    bundle_item.e32_offset = start->addr.off;
                    WriteLoad( &bundle_item, sizeof( flat_bundle_entry32 ) );
                }
            }
        }
    }
    PadLoad( 1 );
    return( size + 1 );
}

static unsigned_32 WriteRelocSize( void *** reloclist, unsigned_32 size,
                                                        unsigned limit )
/**********************************************************************/
{
    void **     rptr;

    if( reloclist != NULL ) {
        rptr = *reloclist;
    } else {
        rptr = NULL;
    }
    while( limit > 0 ) {
        WriteLoad( &size, sizeof( unsigned_32 ) );
        if( rptr != NULL ) {
            /* first one for external fixups */
            size += RelocSize( *rptr++ );
            /* second for internals */
            size += RelocSize( *rptr++ );
        }
        limit--;
    }
    return( size );
}

static unsigned_32 WriteFixupTables( os2_flat_header *header, unsigned long loc)
/******************************************************************************/
/* dump the fixup page table and the fixup record table */
{
    unsigned_32     size;
    unsigned_32     numpages;
    unsigned_32     numentries;
    group_entry *   group;
    void ***        reloclist;
    unsigned_32     highidx;
    unsigned        lowidx;

    header->fixpage_off = loc;
    numentries = 0;
    size = 0;
    for( group = Groups; group != NULL; group = group->next_group ) {
        reloclist = group->g.grp_relocs;
        numpages = PAGE_COUNT( group->size );
        numentries += numpages;
        highidx = OSF_RLIDX_HIGH(numpages);
        while( highidx > 0 ) {
            size = WriteRelocSize( reloclist, size, OSF_RLIDX_MAX );
            highidx--;
            if( reloclist != NULL ) reloclist++;
        }
        lowidx = OSF_RLIDX_LOW(numpages);
        if( lowidx > 0 ) {
            size = WriteRelocSize( reloclist, size, lowidx );
        }
    }
    WriteLoad( &size, sizeof( unsigned_32 ) );
    ++numentries;
    /* now that the page table is dumped, do the fixups. */
    header->fixrec_off = loc + numentries * sizeof( unsigned_32 );
    size += numentries * sizeof( unsigned_32 );
    for( group = Groups; group != NULL; group = group->next_group ) {
        TraverseOS2RelocList( group, DumpRelocList );
    }
    return( size );
}

static unsigned WriteDataPages( unsigned long loc )
/*************************************************/
/* write the enumerated data pages */
{
    group_entry *group;
    unsigned    last_page;
    unsigned    size;

    last_page = 0;
    for( group = Groups; group != NULL; group = group->next_group) {
        if( group->size != 0 ) {
            if( last_page != 0 ) {
                if( FmtData.type & (MK_OS2_LE|MK_WIN_VXD) ) {
                    size = OSF_DEF_PAGE_SIZE - last_page;
                } else {
                    size = ROUND_SHIFT(last_page, FmtData.u.os2.segment_shift)
                             - last_page;
                }
                PadLoad( size );
                loc += size;
            }
            WriteGroupLoad( group );
            loc += group->size;
            last_page = group->size & (OSF_DEF_PAGE_SIZE-1);
        }
    }
    if( last_page == 0 ) {
        last_page = OSF_DEF_PAGE_SIZE;
    } else if( !(FmtData.type & (MK_OS2_LE|MK_WIN_VXD)) ) {
        PadLoad( ROUND_SHIFT( last_page, FmtData.u.os2.segment_shift )
                             - last_page );
    }
    return( last_page );
}

void SetHeaderVxDInfo(os2_flat_header *exe_head)
/**********************************************/
/* setup VxD specific info in the header */
{
    entry_export *exp;
    vxd_ddb      ddb;

    exp = FmtData.u.os2.exports;
    if(( exp != NULL ) && ( exp->sym != NULL )) {
        ReadInfo( (exp->sym->p.seg)->u1.vm_ptr, &ddb, sizeof( ddb ) );
        exe_head->r.vxd.device_ID = ddb.req_device_number;
        exe_head->r.vxd.DDK_version = ddb.SDK_version;
    }

}

void FiniOS2FlatLoadFile( void )
/*************************************/
/* make an OS/2 flat model executable file */
{
    os2_flat_header     exe_head;
    unsigned long       curr_loc;
    unsigned long       debug_size;
    unsigned_32         stub_len;
    unsigned long       count;
    unsigned            last_page;

    memset( &exe_head, 0, sizeof( exe_head ) ); /* zero all header fields */
    stub_len = Write_Stub_File( STUB_ALIGN );
    curr_loc  = sizeof(os2_flat_header);
    SeekLoad( stub_len + sizeof(os2_flat_header) );
    curr_loc += WriteObjectTables( &exe_head, curr_loc );
    exe_head.resname_off = curr_loc;
    curr_loc += ResNonResNameTable( TRUE );  // TRUE - do resident table.
    exe_head.rsrc_off = exe_head.resname_off;
    exe_head.num_rsrcs = 0;
    exe_head.entry_off = curr_loc;
    curr_loc += DumpFlatEntryTable();
    exe_head.loader_size = curr_loc - exe_head.objtab_off;
    exe_head.moddir_off = 0;
    curr_loc += WriteFixupTables( &exe_head, curr_loc );
    exe_head.impmod_off = curr_loc;
    curr_loc += ImportModTable( &count );
    exe_head.num_impmods = count;
    PadLoad( 1 );
    curr_loc += 1;
/* The minus one following is to allow for the fact that all OS/2 V2 import
 * by name offsets should be one less than the corresponding values in
 * V1.x -- yuck. */
    exe_head.impproc_off = curr_loc - 1;
    curr_loc += ImportProcTable( &count );
    exe_head.fixup_size = curr_loc - exe_head.fixpage_off;
    curr_loc = NullAlign( 4 );    /* align to dword boundary */
    exe_head.page_off = curr_loc;
    if( FmtData.type & MK_WIN_VXD ) {
        SetHeaderVxDInfo(&exe_head);
    }
    last_page = WriteDataPages( curr_loc );
    if( FmtData.type & (MK_OS2_LE|MK_WIN_VXD) ) {
        exe_head.l.last_page = last_page;
    } else {
        exe_head.l.page_shift = FmtData.u.os2.segment_shift;
    }
    exe_head.nonres_off = PosLoad();
    exe_head.nonres_size = ResNonResNameTable( FALSE );  // FALSE = do non-res.
    if( exe_head.nonres_size == 0 ) exe_head.nonres_off = 0;
    curr_loc = PosLoad();
    DBIWrite();
/* If debug info was written, we want to mark it in the header so that
 * RC doesn't throw it away! */
    SeekEndLoad( 0 );
    debug_size =  PosLoad() - curr_loc;
    if (debug_size) {
        exe_head.debug_off = curr_loc;
        exe_head.debug_len = debug_size;
    }
    if( FmtData.type & (MK_OS2_LE|MK_WIN_VXD) ) {
        exe_head.signature = OSF_FLAT_SIGNATURE;
    } else {
        exe_head.signature = OSF_FLAT_LX_SIGNATURE;
    }
    exe_head.byte_order = OSF_386_BYTE_ORDER;
    exe_head.word_order = OSF_386_WORD_ORDER;
    exe_head.level = OSF_EXE_LEVEL;
    if( FmtData.cpu_type <= 3 ) {
        exe_head.cpu_type = OSF_CPU_386;
    } else {
        exe_head.cpu_type = OSF_CPU_486;
    }
    if( FmtData.type & MK_WIN_VXD ) {
        exe_head.os_type = OSF_WIN386_LEVEL;
    } else {
        exe_head.os_type = OSF_OS_LEVEL;
    }
    if( FmtData.minor < 10 ) FmtData.minor *= 10;
    exe_head.version = FmtData.major * 100 + FmtData.minor;
    if( FmtData.type & MK_WIN_VXD ) { // VxD flags settings
        if( FmtData.u.os2.flags & VIRT_DEVICE ) {
            exe_head.flags |= VXD_DEVICE_DRIVER_DYNAMIC;
        } else if( FmtData.u.os2.flags & PHYS_DEVICE ) {
            exe_head.flags |= VXD_DEVICE_DRIVER_STATIC;
        } else {
            exe_head.flags |= VXD_DEVICE_DRIVER_3x;
        }
//        exe_head.heapsize  = FmtData.u.os2.heapsize;
    } else { // OS/2 flags settings
        if( FmtData.u.os2.flags & PHYS_DEVICE ) {
            exe_head.flags |= OSF_PHYS_DEVICE;
        } else if( FmtData.u.os2.flags & VIRT_DEVICE ) {
            exe_head.flags |= OSF_VIRT_DEVICE;
        } else if( FmtData.dll ) {
            exe_head.flags |= OSF_IS_DLL;
            // The OS/2 loader REALLY doesn't like to have these flags set if there
            // is no entrypoint!
            if (exe_head.start_obj != 0) {
                if( FmtData.u.os2.flags & INIT_INSTANCE_FLAG ) {
                    exe_head.flags |= OSF_INIT_INSTANCE;
                }
                if( FmtData.u.os2.flags & TERM_INSTANCE_FLAG ) {
                    exe_head.flags |= OSF_TERM_INSTANCE;
                }
            }
        } else {
            // These are only relevant for EXEs
            exe_head.stacksize = StackSize;
            if( FmtData.u.os2.flags & PM_NOT_COMPATIBLE ) {
                exe_head.flags |= OSF_NOT_PM_COMPATIBLE;
            } else if( FmtData.u.os2.flags & PM_APPLICATION ) {
                exe_head.flags |= OSF_PM_APP;
            } else {
                exe_head.flags |= OSF_PM_COMPATIBLE;
            }
        }
        if( LinkState & LINK_ERROR ) {
            exe_head.flags |= OSF_LINK_ERROR;
        }
        if( FmtData.type & MK_OS2_LX
            && (FmtData.u.os2.toggle_relocs ^ FmtData.u.os2.gen_int_relocs) ) {
            exe_head.flags |= OSF_INTERNAL_FIXUPS_DONE;
        }
        exe_head.heapsize  = FmtData.u.os2.heapsize;
    }
    exe_head.page_size = OSF_DEF_PAGE_SIZE;
    exe_head.num_preload = 0;   /* NYI: we should fill in this one correctly */
    exe_head.num_inst_preload = 0;  /*NYI: should fill in correctly */
    exe_head.num_inst_demand = 0;   /*NYI: should fill in correctly */
    SeekLoad( stub_len );
    WriteLoad( &exe_head, sizeof(os2_flat_header) );
}

bool FindOS2ExportSym( symbol *sym, dll_sym_info ** dllhandle )
/********************************************************************/
{
    dll_sym_info *      dll;

    if( sym->info & SYM_EXPORTED ) {
        dll = AllocDLLInfo();
        dll->isordinal = TRUE;
        dll->m.modnum = NULL;
        dll->u.ordinal = ((entry_export *)sym->e.export)->ordinal;
        *dllhandle = dll;
        return TRUE;
    }
    return FALSE;
}
