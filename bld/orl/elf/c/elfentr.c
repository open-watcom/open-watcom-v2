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


#include "elfentr.h"
#include "elfload.h"
#include "elfflhn.h"
#include "elflwlv.h"
#include "elforl.h"
#include "orlhash.h"

elf_handle ELFENTRY ElfInit( orl_funcs *funcs )
{
    elf_handle                                  elf_hnd;

    elf_hnd = (elf_handle)ORL_CLI_ALLOC( funcs, ORL_STRUCT_SIZEOF( elf_handle ) );
    if( elf_hnd != NULL ) {
        elf_hnd->funcs = funcs;
        elf_hnd->first_file_hnd = NULL;
    }
    return( elf_hnd );
}

orl_return ELFENTRY ElfFini( elf_handle elf_hnd )
{
    orl_return          return_val;

    while( elf_hnd->first_file_hnd != NULL ) {
        return_val = ElfRemoveFileLinks( elf_hnd->first_file_hnd );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    ORL_PTR_FREE( elf_hnd, elf_hnd );
    return( ORL_OKAY );
}

orl_return ELFENTRY ElfFileInit( elf_handle elf_hnd, FILE *fp, elf_file_handle *pefh )
{
    elf_file_handle     elf_file_hnd;
    orl_return          return_val;

    elf_file_hnd = (elf_file_handle)ORL_PTR_ALLOC( elf_hnd, ORL_STRUCT_SIZEOF( elf_file_handle ) );
    if( elf_file_hnd == NULL ) {
        return( ORL_OUT_OF_MEMORY );
    }
    elf_file_hnd->sec_handles = NULL;
    elf_file_hnd->fp = fp;
    elf_file_hnd->sec_name_hash_table = NULL;
    ElfAddFileLinks( elf_hnd, elf_file_hnd );
    return_val = ElfLoadFileStructure( elf_file_hnd );
    if( return_val != ORL_OKAY ) {
        ElfRemoveFileLinks( elf_file_hnd );
        elf_file_hnd = NULL;
    }
    *pefh = elf_file_hnd;
    return( return_val );
}

orl_return ELFENTRY ElfFileFini( elf_file_handle elf_file_hnd )
{
    return( ElfRemoveFileLinks( elf_file_hnd ) );
}

orl_return ELFENTRY ElfFileScan( elf_file_handle elf_file_hnd, const char *desired, orl_sec_return_func return_func )
{
    orl_hash_data_entry     data_entry;
    elf_quantity            i;
    orl_return              return_val;
    orl_hash_key            h_key;

    if( desired == NULL ) {
        /* global request */
        for( i = 0; i < elf_file_hnd->num_sections; ++i ) {
            return_val = return_func( (orl_sec_handle)elf_file_hnd->sec_handles[i] );
            if( return_val != ORL_OKAY ) {
                return( return_val );
            }
        }
    } else {
        if( elf_file_hnd->sec_name_hash_table == NULL ) {
            return_val = ElfBuildSecNameHashTable( elf_file_hnd );
            if( return_val != ORL_OKAY ) {
                return( return_val );
            }
        }
        h_key.u.string = desired;
        for( data_entry = ORLHashTableQuery( elf_file_hnd->sec_name_hash_table, h_key ); data_entry != NULL; data_entry = data_entry->next ) {
            return_val = return_func( data_entry->data.u.sec_handle );
            if( return_val != ORL_OKAY ) {
                return( return_val );
            }
        }
    }
    return( ORL_OKAY );
}

orl_machine_type ELFENTRY ElfFileGetMachineType( elf_file_handle elf_file_hnd )
{
    return( elf_file_hnd->machine_type );
}

orl_file_flags ELFENTRY ElfFileGetFlags( elf_file_handle elf_file_hnd )
{
    return( elf_file_hnd->flags );
}

orl_file_type ELFENTRY ElfFileGetType( elf_file_handle elf_file_hnd )
{
    return( elf_file_hnd->type );
}

orl_file_size ELFENTRY ElfFileGetSize( elf_file_handle elf_file_hnd )
{
    return( elf_file_hnd->size );
}

elf_sec_handle ELFENTRY ElfFileGetSymbolTable( elf_file_handle elf_file_hnd )
{
    return( elf_file_hnd->symbol_table );
}

const char * ELFENTRY ElfSecGetName( elf_sec_handle elf_sec_hnd )
{
    return( elf_sec_hnd->name );
}

orl_return ELFENTRY ElfSecGetBase( elf_sec_handle elf_sec_hnd, orl_sec_base *sec_base )
{
    sec_base->u._32[I64HI32] = elf_sec_hnd->base.u._32[I64HI32];
    sec_base->u._32[I64LO32] = elf_sec_hnd->base.u._32[I64LO32];
    return( ORL_OKAY );
}

orl_sec_size ELFENTRY ElfSecGetSize( elf_sec_handle elf_sec_hnd )
{
    return( elf_sec_hnd->size.u._32[I64LO32] );
}

orl_sec_type ELFENTRY ElfSecGetType( elf_sec_handle elf_sec_hnd )
{
    return( elf_sec_hnd->type );
}

orl_sec_flags ELFENTRY ElfSecGetFlags( elf_sec_handle elf_sec_hnd )
{
    return( elf_sec_hnd->flags );
}

orl_sec_alignment ELFENTRY ElfSecGetAlignment( elf_sec_handle elf_sec_hnd )
{
    return( elf_sec_hnd->alignment );
}

elf_sec_handle ELFENTRY ElfSecGetStringTable( elf_sec_handle elf_sec_hnd )
{
    switch( elf_sec_hnd->type ) {
    case ORL_SEC_TYPE_SYM_TABLE:
    case ORL_SEC_TYPE_DYN_SYM_TABLE:
        return( elf_sec_hnd->assoc.sym.string_table );
    case ORL_SEC_TYPE_IMPORT:
        return( elf_sec_hnd->assoc.import.string_table );
    default:
        return( NULL );
    }
}

elf_sec_handle ELFENTRY ElfSecGetSymbolTable( elf_sec_handle elf_sec_hnd )
{
    switch( elf_sec_hnd->type ) {
    case ORL_SEC_TYPE_RELOCS:
    case ORL_SEC_TYPE_RELOCS_EXPADD:
        return( elf_sec_hnd->assoc.reloc.symbol_table );
    case ORL_SEC_TYPE_EXPORT:
        return( elf_sec_hnd->assoc.export.symbol_table );
    default:
        return( NULL );
    }
}

elf_sec_handle ELFENTRY ElfSecGetRelocTable( elf_sec_handle elf_sec_hnd )
{
    switch( elf_sec_hnd->type ) {
    case ORL_SEC_TYPE_RELOCS:
    case ORL_SEC_TYPE_RELOCS_EXPADD:
    case ORL_SEC_TYPE_SYM_TABLE:
    case ORL_SEC_TYPE_DYN_SYM_TABLE:
    case ORL_SEC_TYPE_IMPORT:
    case ORL_SEC_TYPE_EXPORT:
        return( NULL );
    default:
        return( elf_sec_hnd->assoc.normal.reloc_sec );
    }
}

orl_return ELFENTRY ElfSecGetContents( elf_sec_handle elf_sec_hnd, unsigned char **buffer )
{
    if( elf_sec_hnd->contents != NULL ) {
        *buffer = elf_sec_hnd->contents;
        return( ORL_OKAY );
    }
    return( ORL_ERROR );
}

orl_return ELFENTRY ElfSecQueryReloc( elf_sec_handle elf_sec_hnd, elf_sec_offset sec_offset, orl_reloc_return_func return_func )
{
    unsigned        index;
    elf_sec_handle  reloc_sec_hnd;
    orl_reloc       reloc;
    orl_return      return_val;

    if( elf_sec_hnd->type != ORL_SEC_TYPE_PROG_BITS )
        return( ORL_ERROR );
    reloc_sec_hnd = elf_sec_hnd->assoc.normal.reloc_sec;
    if( reloc_sec_hnd == NULL )
        return( ORL_FALSE );
    if( reloc_sec_hnd->assoc.reloc.relocs == NULL ) {
        return_val = ElfCreateRelocs( elf_sec_hnd, reloc_sec_hnd );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    reloc = reloc_sec_hnd->assoc.reloc.relocs;
    return_val = ORL_FALSE;
    for( index = 0; index < reloc_sec_hnd->size.u._32[I64LO32]; index += reloc_sec_hnd->entsize.u._32[I64LO32] ) {
        if( reloc->offset == sec_offset ) {
            return_val = return_func( reloc );
            if( return_val != ORL_OKAY ) {
                return( return_val );
            }
        }
        reloc++;
    }
    return( return_val );
}

orl_return ELFENTRY ElfSecScanReloc( elf_sec_handle elf_sec_hnd, orl_reloc_return_func return_func )
{
    unsigned            index;
    elf_sec_handle      reloc_sec_hnd;
    orl_reloc           reloc;
    orl_return          return_val;

    if( elf_sec_hnd->type != ORL_SEC_TYPE_PROG_BITS ) {
        return( ORL_ERROR );
    } else if ( elf_sec_hnd->assoc.normal.reloc_sec == NULL ) {
        return( ORL_FALSE );
    }
    reloc_sec_hnd = elf_sec_hnd->assoc.normal.reloc_sec;
    if( reloc_sec_hnd->assoc.reloc.relocs == NULL ) {
        return_val = ElfCreateRelocs( elf_sec_hnd, reloc_sec_hnd );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    reloc = reloc_sec_hnd->assoc.reloc.relocs;
    return_val = ORL_FALSE;
    for( index = 0; index < reloc_sec_hnd->size.u._32[I64LO32]; index += reloc_sec_hnd->entsize.u._32[I64LO32] ) {
        return_val = return_func( reloc );
        if( return_val != ORL_OKAY )
            return( return_val );
        reloc++;
    }
    return( ORL_TRUE );
}

// One of these should be implimented O(1), but the section handles
// get reordered in ElfLoad.
orl_table_index ELFENTRY ElfCvtSecHdlToIdx( elf_sec_handle elf_sec_hnd )
{
    elf_quantity        index;
    elf_quantity        limit;
    elf_file_handle     fhdl;

    fhdl = elf_sec_hnd->elf_file_hnd;
    limit = fhdl->num_sections;
    for( index = 0; index < limit; index++ ) {
        if( fhdl->sec_handles[index] == elf_sec_hnd ) {
            return( fhdl->sec_handles[index]->index );
        }
    }
    return( 0 );
}

elf_sec_handle ELFENTRY ElfCvtIdxToSecHdl( elf_file_handle fhdl, orl_table_index idx )
{
    elf_quantity        index;
    elf_quantity        limit;

    limit = fhdl->num_sections;
    for( index = 0; index < limit; index++ ) {
        if( fhdl->sec_handles[index]->index == idx ) {
            return( fhdl->sec_handles[index] );
        }
    }
    return( 0 );
}

orl_return ELFENTRY ElfRelocSecScan( elf_sec_handle elf_sec_hnd, orl_reloc_return_func return_func )
{
    unsigned            index;
    orl_reloc           reloc;
    orl_return          return_val;

    if( elf_sec_hnd->type != ORL_SEC_TYPE_RELOCS && elf_sec_hnd->type != ORL_SEC_TYPE_RELOCS_EXPADD )
        return( ORL_ERROR );
    if( elf_sec_hnd->assoc.reloc.relocs == NULL ) {
        return_val = ElfCreateRelocs( elf_sec_hnd->assoc.reloc.orig_sec, elf_sec_hnd );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    reloc = elf_sec_hnd->assoc.reloc.relocs;
    return_val = ORL_FALSE;
    for( index = 0; index < elf_sec_hnd->size.u._32[I64LO32]; index += elf_sec_hnd->entsize.u._32[I64LO32] ) {
        return_val = return_func( reloc );
        if( return_val != ORL_OKAY )
            return( return_val );
        reloc++;
    }
    return( ORL_TRUE );
}

orl_return ELFENTRY ElfSymbolSecScan( elf_sec_handle elf_sec_hnd, orl_symbol_return_func return_func )
{
    unsigned                index;
    orl_return              return_val;
    elf_symbol_handle       elf_symbol_hnd;

    switch( elf_sec_hnd->type ) {
    case ORL_SEC_TYPE_SYM_TABLE:
    case ORL_SEC_TYPE_DYN_SYM_TABLE:
        if( elf_sec_hnd->assoc.sym.symbols == NULL ) {
            return_val = ElfCreateSymbolHandles( elf_sec_hnd );
            if( return_val != ORL_OKAY ) {
                return( return_val );
            }
        }
        elf_symbol_hnd = elf_sec_hnd->assoc.sym.symbols;
        break;
    default:
        return( ORL_ERROR );
    }
    for( index = 0; index < elf_sec_hnd->size.u._32[I64LO32]; index += elf_sec_hnd->entsize.u._32[I64LO32] ) {
        return_val = return_func( (orl_symbol_handle)elf_symbol_hnd );
        if( return_val != ORL_OKAY )
            return( return_val );
        elf_symbol_hnd++;
    }
    return( ORL_OKAY );
}

orl_return ELFENTRY ElfNoteSecScan( elf_sec_handle elf_sec_hnd, orl_note_callbacks *cbs, void *cookie )
/*****************************************************************************************************/
{
    if( elf_sec_hnd->type != ORL_SEC_TYPE_NOTE )
        return( ORL_ERROR );
    if( strcmp( elf_sec_hnd->name, ".drectve" ) != 0 )
        return( ORL_OKAY );
    if( elf_sec_hnd->size.u._32[I64LO32] == 0 )
        return( ORL_OKAY );
    return( ElfParseDrectve( (char *)elf_sec_hnd->contents, elf_sec_hnd->size.u._32[I64LO32], cbs, cookie ) );
}

const char * ELFENTRY ElfSymbolGetName( elf_symbol_handle elf_symbol_hnd )
{
    return( elf_symbol_hnd->name );
}

orl_return ELFENTRY ElfSymbolGetValue( elf_symbol_handle elf_symbol_hnd, orl_symbol_value *sym_value )
{
    sym_value->u._32[I64HI32] = elf_symbol_hnd->value.u._32[I64HI32];
    sym_value->u._32[I64LO32] = elf_symbol_hnd->value.u._32[I64LO32];
    return( ORL_OKAY );
}

orl_symbol_binding ELFENTRY ElfSymbolGetBinding( elf_symbol_handle elf_symbol_hnd )
{
    return( elf_symbol_hnd->binding );
}

orl_symbol_type ELFENTRY ElfSymbolGetType( elf_symbol_handle elf_symbol_hnd )
{
    return( elf_symbol_hnd->type );
}

unsigned char ELFENTRY ElfSymbolGetRawInfo( elf_symbol_handle elf_symbol_hnd )
{
    return( elf_symbol_hnd->info );
}

elf_sec_handle ELFENTRY ElfSymbolGetSecHandle( elf_symbol_handle elf_symbol_hnd )
{
    switch( elf_symbol_hnd->shndx ) {
    case SHN_ABS:
    case SHN_COMMON:
    case SHN_UNDEF:
        return( NULL );
    default:
        return( elf_symbol_hnd->elf_file_hnd->orig_sec_handles[elf_symbol_hnd->shndx - 1]);
    }
}

elf_symbol_handle ELFENTRY ElfSymbolGetAssociated( elf_symbol_handle elf_symbol_hnd )
{
    return( elf_symbol_hnd );
}
