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
* Description:  ELF format low level (symbols and relocations) processing.
*
****************************************************************************/


#include <assert.h>
#include "walloca.h"
#include "elflwlv.h"
#include "elforl.h"
#include "orlhash.h"

#include "clibext.h"


static void fix_sym_byte_order( elf_file_handle elf_file_hnd, Elf32_Sym *e_sym )
{
    // note that one of the branches will always get compiled out,
    // depending on host endianness
    if( elf_file_hnd->flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
        CONV_BE_32( e_sym->st_name );
        CONV_BE_32( e_sym->st_value );
        CONV_BE_32( e_sym->st_size );
        CONV_BE_16( e_sym->st_shndx );
    } else {
        CONV_LE_32( e_sym->st_name );
        CONV_LE_32( e_sym->st_value );
        CONV_LE_32( e_sym->st_size );
        CONV_LE_16( e_sym->st_shndx );
    }
}


static void fix_sym64_byte_order( elf_file_handle elf_file_hnd, Elf64_Sym *e_sym )
{
    // note that one of the branches will always get compiled out,
    // depending on host endianness
    if( elf_file_hnd->flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
        CONV_BE_32( e_sym->st_name );
        SCONV_BE_64( e_sym->st_value );
        SCONV_BE_64( e_sym->st_size );
        CONV_BE_16( e_sym->st_shndx );
    } else {
        CONV_LE_32( e_sym->st_name );
        SCONV_LE_64( e_sym->st_value );
        SCONV_LE_64( e_sym->st_size );
        CONV_LE_16( e_sym->st_shndx );
    }
}


static void fix_rel_byte_order( elf_file_handle elf_file_hnd, Elf32_Rel *e_rel )
{
    // note that one of the branches will always get compiled out,
    // depending on host endianness
    if( elf_file_hnd->flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
        CONV_BE_32( e_rel->r_offset );
        CONV_BE_32( e_rel->r_info );
    } else {
        CONV_LE_32( e_rel->r_offset );
        CONV_LE_32( e_rel->r_info );
    }
}


static void fix_rela_byte_order( elf_file_handle elf_file_hnd, Elf32_Rela *e_rela )
{
    // note that one of the branches will always get compiled out,
    // depending on host endianness
    if( elf_file_hnd->flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
        CONV_BE_32( e_rela->r_offset );
        CONV_BE_32( e_rela->r_info );
        CONV_BE_32( e_rela->r_addend );
    } else {
        CONV_LE_32( e_rela->r_offset );
        CONV_LE_32( e_rela->r_info );
        CONV_LE_32( e_rela->r_addend );
    }
}


static void fix_rel64_byte_order( elf_file_handle elf_file_hnd, Elf64_Rel *e_rel )
{
    // note that one of the branches will always get compiled out,
    // depending on host endianness
    if( elf_file_hnd->flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
        SCONV_BE_64( e_rel->r_offset );
        SCONV_BE_64( e_rel->r_info );
    } else {
        SCONV_LE_64( e_rel->r_offset );
        SCONV_LE_64( e_rel->r_info );
    }
}


static void fix_rela64_byte_order( elf_file_handle elf_file_hnd, Elf64_Rela *e_rela )
{
    // note that one of the branches will always get compiled out,
    // depending on host endianness
    if( elf_file_hnd->flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
        SCONV_BE_64( e_rela->r_offset );
        SCONV_BE_64( e_rela->r_info );
        SCONV_BE_64( e_rela->r_addend );
    } else {
        SCONV_LE_64( e_rela->r_offset );
        SCONV_LE_64( e_rela->r_info );
        SCONV_LE_64( e_rela->r_addend );
    }
}


orl_return ElfCreateSymbolHandles( elf_sec_handle elf_sec_hnd )
{
    unsigned            i;
    unsigned            num_syms;
    elf_symbol_handle   current;
    unsigned char       *current_sym;
    Elf32_Sym           *current_sym32;
    Elf64_Sym           *current_sym64;
    unsigned_32         st_name;

    num_syms = elf_sec_hnd->size.u._32[I64LO32] / elf_sec_hnd->entsize.u._32[I64LO32];
    elf_sec_hnd->assoc.sym.symbols = (elf_symbol_handle)_ClientSecAlloc( elf_sec_hnd, ORL_STRUCT_SIZEOF( elf_symbol_handle ) * num_syms );
    if( elf_sec_hnd->assoc.sym.symbols == NULL )
        return( ORL_OUT_OF_MEMORY );
    current = elf_sec_hnd->assoc.sym.symbols;
    current_sym = elf_sec_hnd->contents;
    for( i = 0; i < num_syms; i++ ) {
        if( elf_sec_hnd->elf_file_hnd->flags & ORL_FILE_FLAG_64BIT_MACHINE ) {
            current_sym64 = (Elf64_Sym *)current_sym;
            fix_sym64_byte_order( elf_sec_hnd->elf_file_hnd, current_sym64 );
            st_name = current_sym64->st_name;
            current->value.u._32[I64HI32] = current_sym64->st_value.u._32[I64HI32];
            current->value.u._32[I64LO32] = current_sym64->st_value.u._32[I64LO32];
            current->info = current_sym64->st_info;
            current->shndx = current_sym64->st_shndx;
        } else {
            current_sym32 = (Elf32_Sym *)current_sym;
            fix_sym_byte_order( elf_sec_hnd->elf_file_hnd, current_sym32 );
            st_name = current_sym32->st_name;
            current->value.u._32[I64HI32] = 0;
            current->value.u._32[I64LO32] = current_sym32->st_value;
            current->info = current_sym32->st_info;
            current->shndx = current_sym32->st_shndx;
        }
        if( st_name == 0 ) {
            current->name = NULL;
        } else {
            current->name = (char *)( elf_sec_hnd->assoc.sym.string_table->contents + st_name );
        }
        current->file_format = ORL_ELF;
        current->elf_file_hnd = elf_sec_hnd->elf_file_hnd;
        switch( ELF32_ST_BIND( current->info ) ) {
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
        switch( ELF32_ST_TYPE( current->info ) ) {
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
            current->type = ORL_SYM_TYPE_NOTYPE;
            break;
        }
        switch( current->shndx ) {
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
        current_sym += elf_sec_hnd->entsize.u._32[I64LO32];
    }
    return( ORL_OKAY );
}


orl_return ElfBuildSecNameHashTable( elf_file_handle elf_file_hnd )
{
    elf_quantity    i;
    orl_return      return_val;
    orl_hash_key    h_key;
    orl_hash_data   h_data;

    elf_file_hnd->sec_name_hash_table = ORLHashTableCreate( elf_file_hnd->elf_hnd->funcs, SEC_NAME_HASH_TABLE_SIZE, ORL_HASH_STRING_IGNORECASE );
    if( elf_file_hnd->sec_name_hash_table == NULL ) {
        return( ORL_OUT_OF_MEMORY );
    }
    for( i = 0; i < elf_file_hnd->num_sections; ++i ) {
        h_key.u.string = elf_file_hnd->sec_handles[i]->name;
        h_data.u.sec_handle = (orl_sec_handle)elf_file_hnd->sec_handles[i];
        return_val = ORLHashTableInsert( elf_file_hnd->sec_name_hash_table, h_key, h_data );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    return( ORL_OKAY );
}


static orl_reloc_type convertPPCReloc( elf_reloc_type elf_type )
{
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


static orl_reloc_type convert386Reloc( elf_reloc_type elf_type )
{
    switch( elf_type ) {
    case R_386_NONE:
    case R_386_COPY:
        return( ORL_RELOC_TYPE_ABSOLUTE );
    case R_386_GOT32:
        return( ORL_RELOC_TYPE_GOT_32 );
    case R_386_PLT32:
        return( ORL_RELOC_TYPE_PLT_32 );
    case R_386_JMP_SLOT:
        return( ORL_RELOC_TYPE_PLTREL_32 );
    case R_386_32:
    case R_386_GOTOFF:
        return( ORL_RELOC_TYPE_WORD_32 );
    case R_386_PC32:
    case R_386_GOTPC:
        return( ORL_RELOC_TYPE_REL_32_NOADJ );
    default:
        assert( 0 );
    }
    return( ORL_RELOC_TYPE_NONE );
}


static orl_reloc_type convertAMD64Reloc( elf_reloc_type elf_type )
{
    switch( elf_type ) {
    case R_X86_64_NONE:
    case R_X86_64_COPY:
        return( ORL_RELOC_TYPE_ABSOLUTE );
    case R_X86_64_GOT32:
        return( ORL_RELOC_TYPE_GOT_32 );
    case R_X86_64_PLT32:
        return( ORL_RELOC_TYPE_PLT_32 );
    case R_X86_64_JUMP_SLOT:
        return( ORL_RELOC_TYPE_PLTREL_32 );
    case R_X86_64_32:
    case R_X86_64_32S:
        return( ORL_RELOC_TYPE_WORD_32 );
    case R_X86_64_64:
        return( ORL_RELOC_TYPE_WORD_64 );
    case R_X86_64_PC32:
    case R_X86_64_GOTPC32:
        return( ORL_RELOC_TYPE_REL_32_NOADJ );
    default:
        assert( 0 );
    }
    return( ORL_RELOC_TYPE_NONE );
}


static orl_reloc_type convertSPARCReloc( elf_reloc_type elf_type )
{
    switch( elf_type ) {
    case R_SPARC_NONE:
    case R_SPARC_COPY:
        return( ORL_RELOC_TYPE_ABSOLUTE );
    case R_SPARC_8:
        return( ORL_RELOC_TYPE_WORD_8 );
    case R_SPARC_16:
        return( ORL_RELOC_TYPE_WORD_16 );
    case R_SPARC_32:
        return( ORL_RELOC_TYPE_WORD_32 );
    case R_SPARC_DISP8:
        return( ORL_RELOC_TYPE_REL_8 );
    case R_SPARC_DISP16:
        return( ORL_RELOC_TYPE_REL_16 );
    case R_SPARC_DISP32:
        return( ORL_RELOC_TYPE_REL_32 );
    default:
        assert( 0 );
    }
    return( ORL_RELOC_TYPE_NONE );
}


static orl_reloc_type convertMIPSReloc( elf_reloc_type elf_type )
{
    switch( elf_type ) {
    case R_MIPS_NONE:
        return( ORL_RELOC_TYPE_ABSOLUTE );
    case R_MIPS_16:
        return( ORL_RELOC_TYPE_WORD_16 );
    case R_MIPS_32:
        return( ORL_RELOC_TYPE_WORD_32 );
    case R_MIPS_REL32:
        return( ORL_RELOC_TYPE_REL_32 );
    case R_MIPS_HI16:
        return( ORL_RELOC_TYPE_HALF_HI );
    case R_MIPS_LO16:
        return( ORL_RELOC_TYPE_HALF_LO );
    case R_MIPS_GOT16:
        return( ORL_RELOC_TYPE_GOT_16 );
    case R_MIPS_CALL16:
        return( ORL_RELOC_TYPE_GOT_16 );
    case R_MIPS_26:
        return( ORL_RELOC_TYPE_WORD_26 );
    case R_MIPS_PC16:
        return( ORL_RELOC_TYPE_REL_16 );
    default:
        assert( 0 );
    }
    return( ORL_RELOC_TYPE_NONE );
}


static orl_reloc_type convertAlphaReloc( elf_reloc_type elf_type )
{
    switch( elf_type ) {
    case R_ALPHA_NONE:
    case R_ALPHA_COPY:
        return( ORL_RELOC_TYPE_ABSOLUTE );
    case R_ALPHA_REFLONG:
    case R_ALPHA_REFQUAD:
        return( ORL_RELOC_TYPE_WORD_32 );
    case R_ALPHA_GPREL32:
        return( ORL_RELOC_TYPE_GOT_32 );
    case R_ALPHA_GPRELHIGH:
        return( ORL_RELOC_TYPE_HALF_HI );
    case R_ALPHA_GPRELLOW:
        return( ORL_RELOC_TYPE_HALF_LO );
    default:
        assert( 0 );
    }
    return( ORL_RELOC_TYPE_NONE );
}


orl_reloc_type ElfConvertRelocType( elf_file_handle elf_file_hnd, elf_reloc_type elf_type )
{
    switch( elf_file_hnd->machine_type ) {
    case ORL_MACHINE_TYPE_PPC601:
        return( convertPPCReloc( elf_type ) );
    case ORL_MACHINE_TYPE_I386:
        return( convert386Reloc( elf_type ) );
    case ORL_MACHINE_TYPE_AMD64:
        return( convertAMD64Reloc( elf_type ) );
    case ORL_MACHINE_TYPE_SPARC:
    case ORL_MACHINE_TYPE_SPARCPLUS:
        return( convertSPARCReloc( elf_type ) );
    case ORL_MACHINE_TYPE_R3000:
    case ORL_MACHINE_TYPE_R4000:
        return( convertMIPSReloc( elf_type ) );
    case ORL_MACHINE_TYPE_ALPHA:
        return( convertAlphaReloc( elf_type ) );
    default:
        assert( 0 );
    }
    return( ORL_RELOC_TYPE_NONE );
}


orl_return ElfCreateRelocs( elf_sec_handle orig_sec, elf_sec_handle reloc_sec )
{
    orl_return              return_val;
    unsigned                num_relocs;
    unsigned                i;
    unsigned char           *irel;
    ORL_STRUCT( orl_reloc ) *orel;

    if( reloc_sec->assoc.reloc.symbol_table->assoc.sym.symbols == NULL ) {
        return_val = ElfCreateSymbolHandles( reloc_sec->assoc.reloc.symbol_table );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    switch( reloc_sec->type ) {
    case ORL_SEC_TYPE_RELOCS:
        num_relocs = reloc_sec->size.u._32[I64LO32] / reloc_sec->entsize.u._32[I64LO32];
        orel = _ClientSecAlloc( reloc_sec, ORL_STRUCT_SIZEOF( orl_reloc ) * num_relocs );
        reloc_sec->assoc.reloc.relocs = orel;
        if( orel == NULL )
            return( ORL_OUT_OF_MEMORY );
        irel = reloc_sec->contents;
        for( i = 0; i < num_relocs; i++ ) {
            orel->section = (orl_sec_handle)orig_sec;
            orel->frame = NULL;
            orel->addend = 0;
            if( reloc_sec->elf_file_hnd->flags & ORL_FILE_FLAG_64BIT_MACHINE ) {
                Elf64_Rel *irel64 = (Elf64_Rel *)irel;
                fix_rel64_byte_order( reloc_sec->elf_file_hnd, irel64 );
                orel->symbol = (orl_symbol_handle)( reloc_sec->assoc.reloc.symbol_table->assoc.sym.symbols + irel64->r_info.u._32[I64HI32] );
                orel->type = ElfConvertRelocType( reloc_sec->elf_file_hnd, (elf_reloc_type)irel64->r_info.u._32[I64LO32] );
                orel->offset = irel64->r_offset.u._32[I64LO32];
            } else {
                Elf32_Rel *irel32 = (Elf32_Rel *)irel;
                fix_rel_byte_order( reloc_sec->elf_file_hnd, irel32 );
                orel->symbol = (orl_symbol_handle)( reloc_sec->assoc.reloc.symbol_table->assoc.sym.symbols + ELF32_R_SYM( irel32->r_info ) );
                orel->type = ElfConvertRelocType( reloc_sec->elf_file_hnd, ELF32_R_TYPE( irel32->r_info ) );
                orel->offset = irel32->r_offset;
            }
            irel += reloc_sec->entsize.u._32[I64LO32];
            orel++;
        }
        break;
    case ORL_SEC_TYPE_RELOCS_EXPADD:
        num_relocs = reloc_sec->size.u._32[I64LO32] / reloc_sec->entsize.u._32[I64LO32];
        orel = _ClientSecAlloc( reloc_sec, ORL_STRUCT_SIZEOF( orl_reloc ) * num_relocs );
        reloc_sec->assoc.reloc.relocs = orel;
        if( orel == NULL )
            return( ORL_OUT_OF_MEMORY );
        irel = reloc_sec->contents;
        for( i = 0; i < num_relocs; i++ ) {
            orel->section = (orl_sec_handle)orig_sec;
            orel->frame = NULL;
            if( reloc_sec->elf_file_hnd->flags & ORL_FILE_FLAG_64BIT_MACHINE ) {
                Elf64_Rela *irela64 = (Elf64_Rela *)irel;
                fix_rela64_byte_order( reloc_sec->elf_file_hnd, irela64 );
                orel->symbol = (orl_symbol_handle)( reloc_sec->assoc.reloc.symbol_table->assoc.sym.symbols + irela64->r_info.u._32[I64HI32] );
                orel->type = ElfConvertRelocType( reloc_sec->elf_file_hnd, (elf_reloc_type)irela64->r_info.u._32[I64LO32] );
                orel->offset = irela64->r_offset.u._32[I64LO32];
                orel->addend = irela64->r_addend.u._32[I64LO32];
            } else {
                Elf32_Rela *irela32 = (Elf32_Rela *)irel;
                fix_rela_byte_order( reloc_sec->elf_file_hnd, irela32 );
                orel->symbol = (orl_symbol_handle)( reloc_sec->assoc.reloc.symbol_table->assoc.sym.symbols + ELF32_R_SYM( irela32->r_info ) );
                orel->type = ElfConvertRelocType( reloc_sec->elf_file_hnd, ELF32_R_TYPE( irela32->r_info ) );
                orel->offset = irela32->r_offset;
                orel->addend = irela32->r_addend;
            }
            irel += reloc_sec->entsize.u._32[I64LO32];
            orel++;
        }
        break;
    default:
        break;
    }
    return( ORL_OKAY );
}

static size_t strncspn( const char *s, const char *charset, size_t len )
{
    unsigned char   chartable[32];
    size_t          i;
    unsigned char   ch;

    memset( chartable, 0, sizeof( chartable ) );
    for( ; *charset != 0; charset++ ) {
        ch = *charset;
        chartable[ch / 8] |= 1 << ( ch % 8 );
    }
    for( i = 0; i < len; i++ ) {
        ch = s[i];
        if( chartable[ch / 8] & ( 1 << ( ch % 8 ) ) ) {
            break;
        }
    }
    return( i );
}

static const char *pstrncspn( const char *s, const char *charset, size_t *len )
{
    size_t  l;

    l = strncspn( s, charset, *len );
    *len -= l;
    return( s + l );
}

static void EatWhite( const char **contents, size_t *len )
/********************************************************/
{
    char    ch;

    ch = **contents;
    while( (ch == ' ' || ch == '\t' || ch == '=' || ch == ',') && *len > 0 ) {
        (*len)--;
        *contents += 1;
        ch = **contents;
    }
}

static orl_return ParseExport( const char **contents, size_t *len, orl_note_callbacks *cbs, void *cookie )
/********************************************************************************************************/
{
    char        *arg;
    size_t      l;

    l = strncspn( *contents, ", \t", *len );
    arg = alloca( l + 1 );
    memcpy( arg, *contents, l );
    arg[l] = 0;
    *len -= l;
    *contents += l;
    return( cbs->export_fn( arg, cookie ) );
}


static orl_return ParseDefLibEntry( const char **contents, size_t *len,
                    callback_deflib_fn *deflibentry_fn, void *cookie )
/*********************************************************************/
{
    char            *arg;
    size_t          l;
    orl_return      retval;

    for( ;; ) {
        l = strncspn( *contents, ", \t", *len );
        arg = alloca( l + 1 );
        memcpy( arg, *contents, l );
        arg[l] = 0;
        *len -= l;
        *contents += l;

        retval = deflibentry_fn( arg, cookie );
        if( retval != ORL_OKAY || **contents != ',' )
            break;
        (*contents)++;
    }
    return( retval );
}

orl_return ElfParseDrectve( const char *contents, size_t len, orl_note_callbacks *cbs, void *cookie )
/***************************************************************************************************/
{
    const char      *cmd;

    EatWhite( &contents, &len );
    while( len > 0 ) {
        if( *contents != '-' )
            break;              // - should be start of token
        contents++; len--;
        cmd = contents;
        contents = pstrncspn( contents, ":", &len );
        if( contents == NULL )
            break;
        contents++; len--;
        if( memicmp( cmd, "export", 6 ) == 0 ) {
            if( ParseExport( &contents, &len, cbs, cookie ) != ORL_OKAY ) {
                break;
            }
        } else if( memicmp( cmd, "defaultlib", 10 ) == 0 ) {
            if( ParseDefLibEntry( &contents, &len, cbs->deflib_fn, cookie ) != ORL_OKAY ) {
                break;
            }
        } else if( memicmp( cmd, "entry", 5 ) == 0 ) {
            if( ParseDefLibEntry( &contents, &len, cbs->entry_fn, cookie ) != ORL_OKAY ) {
                break;
            }
        }
        EatWhite( &contents, &len );
    }
    return( ORL_OKAY );
}
