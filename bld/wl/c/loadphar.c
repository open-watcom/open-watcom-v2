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
* Description:  LOADPHAR : routines for creating phar lap load files.
*
****************************************************************************/

#include <string.h>
#include "linkstd.h"
#include "exephar.h"
#include "pharlap.h"
#include "alloc.h"
#include "msg.h"
#include "reloc.h"
#include "wlnkmsg.h"
#include "proc386.h"
#include "virtmem.h"
#include "objcalc.h"
#include "dbgall.h"
#include "fileio.h"
#include "loadfile.h"

#define HEAD_SIZE 0x180
#define RTP_SIZE  0x080

typedef enum {
    DR_IS_CODE  = 0x1,
    DR_IS_APP   = 0x2,
    DR_IS_USER  = 0x4,
    DR_BASE     = 0x8,
    DR_TSS      = 0x10
} desc_flags;


static unsigned_32 WritePharData( unsigned file_pos )
/***************************************************/
{
    group_entry         *group;
    outfilelist *       fnode;
    bool                repos;

    DEBUG(( DBG_BASE, "Writing data" ));
    OrderGroups( CompareOffsets );
    CurrSect = Root;        // needed for WriteInfo.
    fnode = Root->outfile;
    fnode->file_loc = file_pos;
    Root->u.file_loc = file_pos;
    Root->sect_addr = Groups->grp_addr;
    for( group = Groups; group != NULL; group = group->next_group ) {
        repos = WriteDOSGroup( group );
        if( repos ) {
            SeekLoad( fnode->file_loc );
        }
    }
    return( fnode->file_loc - file_pos );
}

static unsigned_32 WritePharRelocs( void )
/****************************************/
// write the relocations.
{
    RELOC_INFO  *temp;

    temp = Root->reloclist;             // don't want to modify original
    return DumpMaxRelocList( &temp, 0 );
}

static void WritePharSimple( unsigned_32 start )
/**********************************************/
{
    simple_header   header;
    unsigned_32     file_size;
    unsigned_32     header_size;
    unsigned_32     extra;
    unsigned_32     temp;

    if( FmtData.type & MK_PHAR_REX ) {
        SeekLoad( start + sizeof(simple_header) );
        extra = start + sizeof( simple_header ) + WritePharRelocs();
        header_size = MAKE_PARA( extra );
        PadLoad( header_size - extra );
    } else {
        SeekLoad( start + MAKE_PARA( sizeof(simple_header) ) );
        header_size = MAKE_PARA( sizeof(simple_header) );    // para align.
    }
    file_size = header_size + WritePharData( start + header_size );
    DBIWrite();
    if( FmtData.type & MK_PHAR_SIMPLE ) {
        _HostU16toTarg( SIMPLE_SIGNATURE, header.signature );
    } else {
        _HostU16toTarg( REX_SIGNATURE, header.signature );
    }
    _HostU16toTarg( file_size % 512U, header.mod_size );
    _HostU16toTarg( (file_size + 511U) / 512U, header.file_size );
    _HostU16toTarg( Root->relocs, header.num_relocs );
    _HostU16toTarg( header_size / 16, header.hdr_size );
    extra = MemorySize() - file_size + header_size + 0xfff;
    temp = FmtData.u.phar.mindata + extra;
    if( temp < FmtData.u.phar.mindata ) temp = 0xffffffff;
    _HostU16toTarg( temp >> 12, header.min_data );
    temp = FmtData.u.phar.maxdata + extra;
    if( temp < FmtData.u.phar.maxdata ) temp = 0xffffffff;
    _HostU16toTarg( temp >> 12, header.max_data );
    _HostU32toTarg( StackAddr.off, header.ESP );
    _HostU16toTarg( 0, header.checksum );
    _HostU32toTarg( StartInfo.addr.off, header.EIP );
    _HostU16toTarg( 0x1E, header.reloc_offset );
    _HostU16toTarg( 0, header.overlay_num );
    /* allow header size to exceed 1M */
    _HostU16toTarg( (header_size / (16*0x10000ul)) + 1, header.always_one );
    SeekLoad( start );
    WriteLoad( &header, sizeof( simple_header ) );
    if( FmtData.type & MK_PHAR_SIMPLE ) {
        PadLoad( 2 );                   // header occupies a full paragraph.
    }
}

static void WriteDescriptor( unsigned_32 base, unsigned_32 limit,
                             desc_flags flags )
/****************************************************************/
{
    descriptor  desc;

    desc.baselow = base;
    desc.basemid = base >> 16;
    desc.basehigh = base >> 24;
    desc.bits1 = 0;
    desc.bits2 = 0;
    if( flags & DR_BASE ) {
        desc.bits1 |= DESC_READWRITE | DESC_PRESENT;
    }
    if( flags & DR_TSS ) {
        desc.bits1 |= DESC_PRESENT | DESC_TSS;
    }
    if( flags & DR_IS_CODE ) {
        desc.bits1 |= DESC_CODE;
    }
    if( flags & DR_IS_APP ) {
        desc.bits1 |= DESC_APPLICATION;
    }
    if( flags & DR_IS_USER ) {
        desc.bits2 |= DESC_GENERAL;
    }
    if( limit > 0 ) limit--;
    if( limit >= (1024*1024UL) ) {
        limit >>= 12;
        desc.bits2 |= DESC_GRANULARITY_BIT;
    }
    desc.limitlow = limit;
    desc.bits2 |= (limit >> 16) & DESC_LIMIT_HIGH_MASK;
    WriteLoad( &desc, sizeof(descriptor) );
}

static unsigned_32 WritePharSegData( void )
/*****************************************/
{
    TSS                 tss;
    group_entry *       group;
    unsigned            pos;
    unsigned            size;
    desc_flags          flags;

    DEBUG(( DBG_BASE, "Writing data" ));
    OrderGroups( CompareProtSegments );
    memset( &tss, 0, sizeof(TSS) );
    tss.eip = StartInfo.addr.off;       // NYI: what about backlink ss0-2 & esp0-2 ?
    tss.esp = StackAddr.off;
    tss.cs = StartInfo.addr.seg;
    tss.ss = StackAddr.seg;
    tss.ds = DataGroup->grp_addr.seg;
    tss.ldt = 0x28;
    WriteLoad( &tss, sizeof(TSS) );
    WriteDescriptor( 0, 0, 0 );         // NULL GDT entry;
    WriteDescriptor( 0, sizeof(TSS), DR_TSS );          // TSS
    WriteDescriptor( 0, sizeof(TSS), DR_BASE|DR_IS_APP ); // TSS alias
    pos = sizeof(TSS);
    size = NUM_GDT_DESCRIPTORS * sizeof(descriptor);
    WriteDescriptor( pos, size, DR_BASE|DR_IS_APP );    // GDT
    pos += size;
    size = NUM_IDT_DESCRIPTORS * sizeof(descriptor);
    WriteDescriptor( pos, size, DR_BASE|DR_IS_APP );    // IDT
    pos += size;
    size = (NumGroups + 1) * sizeof(descriptor);
    WriteDescriptor( pos, size, DR_BASE );              //LDT
    WriteDescriptor( pos, size, DR_BASE|DR_IS_APP );    // LDT alias
    WriteDescriptor( 0, 0, 0 );         // NULL IDT entry;
    WriteDescriptor( 0, 0, 0 );         // NULL LDT entry;
    pos += size;
    for( group = Groups; group != NULL; group = group->next_group ) {
        flags = DR_BASE | DR_IS_APP | DR_IS_USER;
        if( !(group->segflags & SEG_DATA) ) {
            flags |= DR_IS_CODE;
        }
        WriteDescriptor( pos, group->totalsize, flags );
        pos += group->totalsize;
    }
    for( group = Groups; group != NULL; group = group->next_group ) {
        WriteGroupLoad( group );
        if( group->totalsize > group->size ) {  // phar lap is stupid
            PadLoad( group->totalsize - group->size );
        }
    }
    return( pos );
}

static unsigned_32  WriteRTPBlock( void )
/***************************************/
{
    symbol *        symptr;

    FmtData.u.phar.params->signature = RTP_SIGNATURE;
    if( FmtData.u.phar.breaksym != NULL ) {
        symptr = FindISymbol( FmtData.u.phar.breaksym );
        if( symptr == NULL ) {
            LnkMsg( WRN+MSG_BREAKSYM_NOT_FOUND, "s", FmtData.u.phar.breaksym );
            FmtData.u.phar.params->realbreak = 0;
        } else {
            FmtData.u.phar.params->realbreak = symptr->addr.off;
        }
    }
    WriteLoad( FmtData.u.phar.params, sizeof( rtpblock ) );
    PadLoad( RTP_SIZE - sizeof( rtpblock ) );
    return( RTP_SIZE );
}

static unsigned_32 WriteSIT( void )
/*********************************/
{
    group_entry *       group;
    seg_info_table      sit;
    unsigned_32         size;

    size = 0;
    sit.flags = 0;
    sit.base = 0;
    sit.extra = 0;
    sit.base = FmtData.base;
    for( group = Groups; group != NULL; group = group->next_group ) {
        sit.selector = group->grp_addr.seg;
        sit.extra = group->totalsize - group->size;
        WriteLoad( &sit, sizeof( seg_info_table ) );
        size += sizeof(seg_info_table);
    }
    return size;
}

static void WritePharExtended( unsigned_32 start )
/************************************************/
{
    extended_header header;
    unsigned_32     file_size;
    unsigned_32     image_offset;
    unsigned_32     temp;
    unsigned_32     extra;

    SeekLoad( start + HEAD_SIZE );
    file_size = HEAD_SIZE;
    if( FmtData.type & MK_PHAR_MULTISEG ) {
        _HostU32toTarg( file_size, header.sit_offset );
        temp = WriteSIT();
        _HostU32toTarg( temp, header.sit_size );
        file_size += temp;
        _HostU16toTarg( sizeof(seg_info_table), header.sit_entry_size );
        _HostU32toTarg( file_size, header.reloc_offset );
        temp = WritePharRelocs();
        _HostU32toTarg( temp, header.reloc_size );
        file_size += temp;
    } else {
        _HostU32toTarg( 0, header.sit_offset );
        _HostU32toTarg( 0, header.sit_size );
        _HostU16toTarg( 0, header.sit_entry_size );
        _HostU32toTarg( file_size, header.reloc_offset );
        _HostU32toTarg( 0, header.reloc_size );
    }
    _HostU32toTarg( file_size, header.rtp_offset );
    temp = WriteRTPBlock();
    _HostU32toTarg( temp, header.rtp_size );
    file_size += temp;
    image_offset = file_size;
    if( FmtData.type & MK_PHAR_MULTISEG ) {
        file_size += WritePharSegData();
    } else {
        file_size += WritePharData( start + image_offset );
    }
    DBIWrite();
    _HostU16toTarg( EXTENDED_SIGNATURE, header.signature );
    if( FmtData.type & MK_PHAR_MULTISEG ) {
        _HostU16toTarg( PHAR_FORMAT_SEGMENTED, header.format_level );
    } else {
        _HostU16toTarg( PHAR_FORMAT_FLAT, header.format_level );
    }
    _HostU16toTarg( HEAD_SIZE, header.header_size );
    _HostU32toTarg( file_size, header.file_size );
    _HostU16toTarg( 0, header.checksum );
    _HostU32toTarg( image_offset, header.load_offset );
    _HostU32toTarg( file_size - image_offset, header.load_size );
    _HostU32toTarg( 0, header.sym_offset );
    _HostU32toTarg( 0, header.sym_size );
    if( FmtData.type & MK_PHAR_MULTISEG ) {
        temp = sizeof(TSS);
        _HostU32toTarg( temp, header.gdt_offset );
        extra = NUM_GDT_DESCRIPTORS * sizeof(descriptor);
        _HostU32toTarg( extra, header.gdt_size );
        temp += extra;
        extra = NUM_IDT_DESCRIPTORS * sizeof(descriptor);
        _HostU32toTarg( temp, header.idt_offset );
        _HostU32toTarg( extra, header.idt_size );
        temp += extra;
        extra = (NumGroups + 1) * sizeof(descriptor);
        _HostU32toTarg( temp, header.ldt_offset );
        _HostU32toTarg( extra, header.ldt_size );
        _HostU32toTarg( 0, header.tss_offset );
        _HostU32toTarg( sizeof(TSS), header.tss_size );
        _HostU32toTarg( 0, header.min_extra );
        _HostU32toTarg( 0, header.max_extra );
        _HostU16toTarg( StackAddr.seg, header.SS );
        _HostU16toTarg( StartInfo.addr.seg, header.CS );
        _HostU16toTarg( 0x28, header.LDT );
        _HostU16toTarg( 0x8, header.TSS );
    } else {
        _HostU32toTarg( 0, header.gdt_offset );
        _HostU32toTarg( 0, header.gdt_size );
        _HostU32toTarg( 0, header.idt_offset );
        _HostU32toTarg( 0, header.idt_size );
        _HostU32toTarg( 0, header.ldt_offset );
        _HostU32toTarg( 0, header.ldt_size );
        _HostU32toTarg( 0, header.tss_offset );
        _HostU32toTarg( 0, header.tss_size );
        extra = MemorySize() - file_size + image_offset;
        temp = FmtData.u.phar.mindata + extra;
        if( temp < FmtData.u.phar.mindata ) temp = 0xffffffff;
        _HostU32toTarg( temp, header.min_extra );
        temp = FmtData.u.phar.maxdata + extra;
        if( temp < FmtData.u.phar.maxdata ) temp = 0xffffffff;
        _HostU32toTarg( temp, header.max_extra );
        _HostU16toTarg( 0, header.SS );
        _HostU16toTarg( 0, header.CS );
        _HostU16toTarg( 0, header.LDT );
        _HostU16toTarg( 0, header.TSS );
    }
    _HostU32toTarg( FmtData.base, header.offset );
    _HostU32toTarg( StackAddr.off, header.ESP );
    _HostU32toTarg( StartInfo.addr.off, header.EIP );
    _HostU16toTarg( 0, header.flags );    // packing not yet implemented.
    _HostU32toTarg( 0, header.reserved1 );
    _HostU32toTarg( StackSize, header.stack_size );
    header.mem_req = header.load_size;
    SeekLoad( start );
    WriteLoad( &header, sizeof( extended_header ) );
    PadLoad( HEAD_SIZE - sizeof( extended_header ) );
}

void FiniPharLapLoadFile( void )
/*************************************/
{
    unsigned_32 start;

    start = AppendToLoadFile( FmtData.u.phar.stub );
    if( FmtData.type & (MK_PHAR_FLAT|MK_PHAR_MULTISEG) ) {
        WritePharExtended( start );
    } else {
        WritePharSimple( start );
    }
}
