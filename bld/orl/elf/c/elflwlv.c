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


#include <assert.h>
#include "elflwlv.h"
#include "orlhash.h"

orl_return ElfCreateSymbolHandles( elf_sec_handle elf_sec_hnd )
{
    int                 loop;
    int                 num_syms;
    elf_symbol_handle   current;
    Elf32_Sym *         current_sym;

    num_syms = elf_sec_hnd->size / sizeof( Elf32_Sym );
    elf_sec_hnd->assoc.sym.symbols = (elf_symbol_handle) _ClientSecAlloc( elf_sec_hnd, sizeof( elf_symbol_handle_struct ) * num_syms );
    if( !(elf_sec_hnd->assoc.sym.symbols) ) return( ORL_OUT_OF_MEMORY );
    current = elf_sec_hnd->assoc.sym.symbols;
    current_sym = (Elf32_Sym *) elf_sec_hnd->contents;
    for( loop = 0; loop < num_syms; loop++ ) {
        if( current_sym->st_name == 0 ) {
            current->name = NULL;
        } else {
            current->name = &(elf_sec_hnd->assoc.sym.string_table->contents[current_sym->st_name]);
        }
        current->file_format = ORL_ELF;
        current->elf_file_hnd = elf_sec_hnd->elf_file_hnd;
        current->symbol = current_sym;
        switch( ELF32_ST_BIND( current->symbol->st_info ) ) {
            case STB_LOCAL:
                current->binding = ORL_SYM_BINDING_LOCAL;
                break;
            case STB_GLOBAL:
                current->binding = ORL_SYM_BINDING_GLOBAL;
                break;
            case STB_WEAK:
                current->binding = ORL_SYM_BINDING_WEAK;
                break;
            default:
                current->binding = ORL_SYM_BINDING_NONE; // other?
                break;
        }
        switch( ELF32_ST_TYPE( current->symbol->st_info ) ) {
            case STT_OBJECT:
                current->type = ORL_SYM_TYPE_OBJECT;
                break;
            case STT_FUNC:
                current->type = ORL_SYM_TYPE_FUNCTION;
                break;
            case STT_SECTION:
                current->type = ORL_SYM_TYPE_SECTION;
                break;
            case STT_FILE:
                current->type = ORL_SYM_TYPE_FILE;
                break;
            default:
                current->type = ORL_SYM_TYPE_NONE; // ?
                break;
        }
        switch( current->symbol->st_shndx ) {
            case SHN_ABS:
                current->type |= ORL_SYM_TYPE_ABSOLUTE;
                break;
            case SHN_COMMON:
                current->type |= ORL_SYM_TYPE_COMMON;
                break;
            case SHN_UNDEF:
                current->type |= ORL_SYM_TYPE_UNDEFINED;
                break;
        }
        current++;
        current_sym++;
    }
    return( ORL_OKAY );
}

orl_return ElfBuildSecNameHashTable( elf_file_handle elf_file_hnd )
{
    int                                         loop;
    orl_return                                  error;

    elf_file_hnd->sec_name_hash_table = ORLHashTableCreate( elf_file_hnd->elf_hnd->funcs, SEC_NAME_HASH_TABLE_SIZE, ORL_HASH_STRING, (orl_hash_comparison_func) stricmp );
    if( !(elf_file_hnd->sec_name_hash_table) ) {
        return( ORL_OUT_OF_MEMORY );
    }
    for( loop = 0; loop < elf_file_hnd->num_sections; loop++ ) {
        error = ORLHashTableInsert( elf_file_hnd->sec_name_hash_table, (orl_hash_value) elf_file_hnd->elf_sec_hnd[loop]->name, elf_file_hnd->elf_sec_hnd[loop] );
        if( error != ORL_OKAY ) return( error );
    }
    return( ORL_OKAY );
}

static orl_reloc_type convertPPCReloc( elf_reloc_type elf_type ) {
    switch( elf_type ) {
        case R_PPC_NONE:
            return( ORL_RELOC_TYPE_ABSOLUTE );
        case R_PPC_ADDR32:
            return( ORL_RELOC_TYPE_WORD_32 );
        case R_PPC_ADDR24:
            return( ORL_RELOC_TYPE_WORD_24 );
        case R_PPC_ADDR16:
            return( ORL_RELOC_TYPE_WORD_16 );
        case R_PPC_ADDR16_LO:
            return( ORL_RELOC_TYPE_HALF_LO );
        case R_PPC_ADDR16_HI:
            return( ORL_RELOC_TYPE_HALF_HI );
        case R_PPC_ADDR16_HA:
            return( ORL_RELOC_TYPE_HALF_HA );
        case R_PPC_ADDR14:
            return( ORL_RELOC_TYPE_WORD_14 );
        case R_PPC_ADDR14_BRTAKEN:
        case R_PPC_ADDR14_BRNTAKEN:
            // fixme
            return( ORL_RELOC_TYPE_WORD_14 );
        case R_PPC_REL24:
            return( ORL_RELOC_TYPE_REL_24 );
        case R_PPC_REL14:
            return( ORL_RELOC_TYPE_REL_14 );
        case R_PPC_REL14_BRTAKEN:
        case R_PPC_REL14_BRNTAKEN:
            // fixme
            return( ORL_RELOC_TYPE_REL_14 );
        case R_PPC_GOT16:
            return( ORL_RELOC_TYPE_GOT_16 );
        case R_PPC_GOT16_LO:
            return( ORL_RELOC_TYPE_GOT_16_LO );
        case R_PPC_GOT16_HI:
            return( ORL_RELOC_TYPE_GOT_16_HI );
        case R_PPC_GOT16_HA:
            return( ORL_RELOC_TYPE_GOT_16_HA );
        case R_PPC_PLTREL24:
            return( ORL_RELOC_TYPE_PLTREL_24 );
        case R_PPC_COPY:
            return( ORL_RELOC_TYPE_NONE );
        case R_PPC_GLOB_DAT:
            return( ORL_RELOC_TYPE_WORD_32 );
        case R_PPC_JMP_SLOT:
            // fixme
            return( ORL_RELOC_TYPE_NONE );
        case R_PPC_RELATIVE:
            return( ORL_RELOC_TYPE_SEC_REL );
        case R_PPC_LOCAL24PC:
            // fixme
            return( ORL_RELOC_TYPE_REL_24 );
        case R_PPC_UADDR32:
            return( ORL_RELOC_TYPE_WORD_32 );
        case R_PPC_UADDR16:
            return( ORL_RELOC_TYPE_WORD_16 );
        case R_PPC_REL32:
            return( ORL_RELOC_TYPE_REL_32 );
        case R_PPC_PLT32:
            return( ORL_RELOC_TYPE_PLT_32 );
        case R_PPC_PLTREL32:
            return( ORL_RELOC_TYPE_PLTREL_32 );
        case R_PPC_PLT16_LO:
            return( ORL_RELOC_TYPE_PLT_16_LO );
        case R_PPC_PLT16_HI: return( ORL_RELOC_TYPE_PLT_16_HI );
        case R_PPC_PLT16_HA: return( ORL_RELOC_TYPE_PLT_16_HA );
    }
    return( ORL_RELOC_TYPE_NONE );
}

static orl_reloc_type convert386Reloc( elf_reloc_type elf_type ) {
    switch( elf_type ) {
    case R_386_NONE:
        return( ORL_RELOC_TYPE_ABSOLUTE );
    case R_386_32:
    case R_386_GOT32:
    case R_386_GOTOFF:
        return( ORL_RELOC_TYPE_WORD_32 );
    case R_386_PC32:
    case R_386_PLT32:
        return( ORL_RELOC_TYPE_REL_32 );
    default:
        assert( 0 );
    }
    return( ORL_RELOC_TYPE_NONE );
}

orl_reloc_type ElfConvertRelocType( elf_file_handle elf_file_hnd, elf_reloc_type elf_type ) {
    switch( elf_file_hnd->machine_type ) {
    case ORL_MACHINE_TYPE_PPC601:
        return( convertPPCReloc( elf_type ) );
    case ORL_MACHINE_TYPE_I386:
        return( convert386Reloc( elf_type ) );
    default:
        assert( 0 );
    }
    return( ORL_RELOC_TYPE_NONE );
}

orl_return ElfCreateRelocs( elf_sec_handle orig_sec, elf_sec_handle reloc_sec )
{
    orl_return          return_val;
    int                 num_relocs;
    int                 loop;
    Elf32_Rel *         rel;
    Elf32_Rela *        rela;
    orl_reloc *         o_rel;

    if( reloc_sec->assoc.reloc.symbol_table->assoc.sym.symbols == NULL ) {
        return_val = ElfCreateSymbolHandles( reloc_sec->assoc.reloc.symbol_table );
        if( return_val != ORL_OKAY ) return( return_val );
    }
    switch( reloc_sec->type ) {
        case ORL_SEC_TYPE_RELOCS:
            num_relocs = reloc_sec->size / sizeof( Elf32_Rel );
            reloc_sec->assoc.reloc.relocs = (orl_reloc *) _ClientSecAlloc( reloc_sec, sizeof( orl_reloc ) * num_relocs );
            if( reloc_sec->assoc.reloc.relocs == NULL ) return( ORL_OUT_OF_MEMORY );
            rel = (Elf32_Rel *) reloc_sec->contents;
            o_rel = (orl_reloc *) reloc_sec->assoc.reloc.relocs;
            for( loop = 0; loop < num_relocs; loop++ ) {
                o_rel->section = (orl_sec_handle) orig_sec;
                o_rel->symbol = (orl_symbol_handle) &(reloc_sec->assoc.reloc.symbol_table->assoc.sym.symbols[ELF32_R_SYM( rel->r_info )]);
                o_rel->type = ElfConvertRelocType( reloc_sec->elf_file_hnd, ELF32_R_TYPE( rel->r_info ) );
                o_rel->offset = rel->r_offset;
                o_rel->addend = 0;
                o_rel->frame = NULL;
                rel++;
                o_rel++;
            }
            break;
        case ORL_SEC_TYPE_RELOCS_EXPADD:
            num_relocs = reloc_sec->size / sizeof( Elf32_Rela );
            reloc_sec->assoc.reloc.relocs = (orl_reloc *) _ClientSecAlloc( reloc_sec, sizeof( orl_reloc ) * num_relocs );
            if( reloc_sec->assoc.reloc.relocs == NULL ) return( ORL_OUT_OF_MEMORY );
            rela = (Elf32_Rela *) reloc_sec->contents;
            o_rel = (orl_reloc *) reloc_sec->assoc.reloc.relocs;
            for( loop = 0; loop < num_relocs; loop++ ) {
                o_rel->section = (orl_sec_handle) orig_sec;
                o_rel->symbol = (orl_symbol_handle) &(reloc_sec->assoc.reloc.symbol_table->assoc.sym.symbols[ELF32_R_SYM( rela->r_info )]);
                o_rel->type = ElfConvertRelocType( reloc_sec->elf_file_hnd, ELF32_R_TYPE( rela->r_info ) );
                o_rel->offset = rela->r_offset;
                o_rel->addend = rela->r_addend;
                o_rel->frame = NULL;
                rela++;
                o_rel++;
            }
            break;
    }
    return( ORL_OKAY );
}
