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


#include <string.h>
#include "global.h"
#include "dis.h"
#include "pdata.h"
#include "buffer.h"
#include "print.h"
#include "hashtabl.h"
#include "main.h"
#include "formasm.h"

typedef struct {
    uint_32     begin_address;
    uint_32     end_address;
    uint_32     exception_handler;
    uint_32     handler_data;
    uint_32     prolog_end;
} descriptor_struct;

extern hash_table           HandleToRefListTable;
extern char                 LabelChar;
extern char                 *CommentString;
extern dis_format_flags     DFormat;
extern section_list_struct  Sections;

static orl_reloc            *pdataReloc;

static void doDescriptorRelocs( ref_entry *r_entry, orl_sec_offset offset, uint_32 address )
{
    /* Skip over pair relocs */
    while( (*r_entry) && ((*r_entry)->type == ORL_RELOC_TYPE_PAIR || (*r_entry)->offset < offset ) ) {
        (*r_entry) = (*r_entry)->next;
    }
    if( (*r_entry) && (*r_entry)->offset == offset ) {
        HandleRefInData( *r_entry, &address, FALSE );
        (*r_entry) = (*r_entry)->next;
    } else {
        BufferStore("%08X", address );
    }
}

static void printDescriptor( orl_sec_offset offset,
                             descriptor_struct *descriptor, ref_entry *r_entry )
{
    char *PreString;

    if( DFormat & DFF_ASM ) {
        PreString = "\t\t.long\t";
    } else {
        PreString = "\t";
    }
    BufferStore("%s", PreString );
    doDescriptorRelocs( r_entry, offset, descriptor->begin_address );
    BufferAlignToTab(6);
    BufferStore("%s ", CommentString );
    BufferMsg( BEGIN_ADDRESS );
    BufferStore(": %d\n", descriptor->begin_address );

    BufferStore("%s", PreString );
    doDescriptorRelocs( r_entry, offset+4, descriptor->end_address );
    BufferAlignToTab(6);
    BufferStore("%s ", CommentString );
    BufferMsg( END_ADDRESS );
    BufferStore(": %d\n", descriptor->end_address );

    BufferPrint();

    BufferStore("%s", PreString );
    doDescriptorRelocs( r_entry, offset+8, descriptor->exception_handler );
    BufferAlignToTab(6);
    BufferStore("%s ", CommentString );
    BufferMsg( EXCEPTION_HANDLER );
    BufferStore(": %d\n", descriptor->exception_handler );

    BufferStore("%s", PreString );
    doDescriptorRelocs( r_entry, offset+12, descriptor->handler_data );
    BufferAlignToTab(6);
    BufferStore("%s ", CommentString );
    BufferMsg( HANDLER_DATA );
    BufferStore(": %d\n", descriptor->handler_data );

    BufferStore("%s", PreString );
    doDescriptorRelocs( r_entry, offset+16, descriptor->prolog_end );
    BufferAlignToTab(6);
    BufferStore("%s ", CommentString );
    BufferMsg( PROLOG_END );
    BufferStore(": %d\n\n", descriptor->prolog_end );

    BufferPrint();
}

orl_return StoreReloc( orl_reloc * reloc )
{
    pdataReloc = reloc;
    return( ORL_OKAY );
}

return_val DumpPDataSection( section_ptr sec, unsigned_8 *contents,
                        orl_sec_size size, unsigned pass )
{
    orl_sec_offset      loop;
    hash_data *         data_ptr;
    ref_list            r_list;
    ref_entry           r_entry;
    descriptor_struct   descriptor;
    bool                is32bit;

    if( pass == 1 ) return( OKAY );
    if( size == 0 ) return( OKAY );

    is32bit = ( size >= 0x10000 );

    data_ptr = HashTableQuery( HandleToRefListTable, (hash_value)sec->shnd );
    r_entry = NULL;
    if( *data_ptr ) {
        r_list = (ref_list)*data_ptr;
        if( r_list ) {
            r_entry = r_list->first;
        }
    }
    BufferConcatNL();
    PrintHeader( sec );
    BufferConcatNL();
    for( loop = 0; loop < size; loop += sizeof( descriptor_struct ) ) {
        if( r_entry == NULL )
            break;
        memcpy( &descriptor, contents + loop, sizeof( descriptor_struct ) );
        if( DFormat & DFF_ASM ) {
            BufferStore( "\t\t" );
            BufferStore( "%s %04X\t\t", CommentString, loop );
        } else {
            PrintLinePrefixAddress( loop, is32bit );
            BufferAlignToTab( PREFIX_SIZE_TABS );
            BufferStore( "%s ", CommentString );
        }
        BufferMsg( PROCEDURE_DESCRIPTOR );

        /* Skip over pair relocs */
        while( r_entry && (r_entry->type == ORL_RELOC_TYPE_PAIR || r_entry->offset < loop) ) {
            r_entry = r_entry->next;
        }
        switch( r_entry->label->type ) {
        case LTYP_EXTERNAL_NAMED:
            BufferStore( "%s", r_entry->label->label.name );
            break;
        case LTYP_NAMED:
        case LTYP_SECTION:
        case LTYP_GROUP:
            BufferStore( "%s", r_entry->label->label.name );
            break;
        default:
            BufferStore( "%c$%d", LabelChar, r_entry->label->label.number );
            break;
        }
        BufferConcatNL();
        printDescriptor( loop, &descriptor, &r_entry );
    }
    return( OKAY );
}
