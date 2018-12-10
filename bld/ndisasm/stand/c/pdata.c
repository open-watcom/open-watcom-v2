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
#include "dis.h"
#include "global.h"
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

static orl_reloc            pdataReloc;

static ref_entry doDescriptorRelocs( ref_entry r_entry, dis_sec_offset offset, uint_32 address )
{
    /* Skip over pair relocs */
    for( ; r_entry != NULL; r_entry = r_entry->next ) {
        if( r_entry->type != ORL_RELOC_TYPE_PAIR && r_entry->offset >= offset ) {
            break;
        }
    }
    if( r_entry != NULL && r_entry->offset == offset ) {
        HandleRefInData( r_entry, &address, false );
        r_entry = r_entry->next;
    } else {
        BufferHexU32( 8, address );
    }
    return( r_entry );
}

static ref_entry printDescriptor( dis_sec_offset offset, descriptor_struct *descriptor, ref_entry r_entry )
{
    char        *PreString;

    if( DFormat & DFF_ASM ) {
        PreString = "\t\t.long\t";
    } else {
        PreString = "\t";
    }
    BufferConcat( PreString );
    r_entry = doDescriptorRelocs( r_entry, offset, descriptor->begin_address );
    BufferAlignToTab( 6 );
    BufferConcat( CommentString );
    BufferConcatChar( ' ' );
    BufferMsg( BEGIN_ADDRESS );
    BufferConcat( ": " );
    BufferDecimal( descriptor->begin_address );
    BufferConcatNL();
    BufferPrint();

    BufferConcat( PreString );
    r_entry = doDescriptorRelocs( r_entry, offset+4, descriptor->end_address );
    BufferAlignToTab( 6 );
    BufferConcat( CommentString );
    BufferConcatChar( ' ' );
    BufferMsg( END_ADDRESS );
    BufferConcat( ": " );
    BufferDecimal( descriptor->end_address );
    BufferConcatNL();
    BufferPrint();

    BufferConcat( PreString );
    r_entry = doDescriptorRelocs( r_entry, offset+8, descriptor->exception_handler );
    BufferAlignToTab( 6 );
    BufferConcat( CommentString );
    BufferConcatChar( ' ' );
    BufferMsg( EXCEPTION_HANDLER );
    BufferConcat( ": " );
    BufferDecimal( descriptor->exception_handler );
    BufferConcatNL();
    BufferPrint();

    BufferConcat( PreString );
    r_entry = doDescriptorRelocs( r_entry, offset+12, descriptor->handler_data );
    BufferAlignToTab( 6 );
    BufferConcat( CommentString );
    BufferConcatChar( ' ' );
    BufferMsg( HANDLER_DATA );
    BufferConcat( ": " );
    BufferDecimal( descriptor->handler_data );
    BufferConcatNL();
    BufferPrint();

    BufferConcat( PreString );
    r_entry = doDescriptorRelocs( r_entry, offset+16, descriptor->prolog_end );
    BufferAlignToTab( 6 );
    BufferConcat( CommentString );
    BufferConcatChar( ' ' );
    BufferMsg( PROLOG_END );
    BufferConcat( ": " );
    BufferDecimal( descriptor->prolog_end );
    BufferConcatNL();
    BufferConcatNL();
    BufferPrint();

    return( r_entry );
}

orl_return StoreReloc( orl_reloc reloc )
{
    pdataReloc = reloc;
    return( ORL_OKAY );
}

return_val DumpPDataSection( section_ptr section, unsigned_8 *contents, dis_sec_size size, unsigned pass )
{
    hash_data           *h_data;
    hash_key            h_key;
    dis_sec_offset      loop;
    ref_list            r_list;
    ref_entry           r_entry;
    descriptor_struct   descriptor;
    bool                is32bit;

    if( pass == 1 )
        return( RC_OKAY );
    if( size == 0 )
        return( RC_OKAY );

    is32bit = ( size >= 0x10000 );
    h_key.u.sec_handle = section->shnd;
    h_data = HashTableQuery( HandleToRefListTable, h_key );
    r_entry = NULL;
    if( h_data != NULL ) {
        r_list = h_data->u.sec_ref_list;
        if( r_list != NULL ) {
            r_entry = r_list->first;
        }
    }
    BufferConcatNL();
    PrintHeader( section );
    BufferConcatNL();
    for( loop = 0; loop < size; loop += sizeof( descriptor_struct ) ) {
        if( r_entry == NULL )
            break;
        memcpy( &descriptor, contents + loop, sizeof( descriptor_struct ) );
        if( DFormat & DFF_ASM ) {
            BufferConcat( "\t\t" );
            BufferConcat( CommentString );
            BufferConcatChar( ' ' );
            BufferHexU32( 4, loop );
            BufferConcat( "\t\t" );
        } else {
            BufferLinePrefixAddress( loop, is32bit );
            BufferAlignToTab( PREFIX_SIZE_TABS );
            BufferConcat( CommentString );
            BufferConcatChar( ' ' );
        }
        BufferMsg( PROCEDURE_DESCRIPTOR );

        /* Skip over pair relocs */
        for( ; r_entry != NULL; r_entry = r_entry->next ) {
            if( r_entry->type != ORL_RELOC_TYPE_PAIR && r_entry->offset >= loop ) {
                break;
            }
        }
        switch( r_entry->label->type ) {
        case LTYP_EXTERNAL_NAMED:
            BufferQuoteName( r_entry->label->label.name );
            break;
        case LTYP_NAMED:
        case LTYP_SECTION:
        case LTYP_GROUP:
            BufferQuoteName( r_entry->label->label.name );
            break;
        default:
            BufferLabelNum( r_entry->label->label.number );
            break;
        }
        BufferConcatNL();
        r_entry = printDescriptor( loop, &descriptor, r_entry );
    }
    return( RC_OKAY );
}
