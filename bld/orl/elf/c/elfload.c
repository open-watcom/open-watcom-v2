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
* Description:  Top level ELF loading code, parses ELF headers.
*
****************************************************************************/


#include "elfload.h"
#include "elforl.h"


// fixme: finish making ELF SPECIFIC (see next fixme)

static void fix_ehdr_byte_order( elf_file_handle elf_file_hnd, Elf32_Ehdr *e_hdr )
{
    // note that one of the branches will always get compiled out,
    // depending on host endianness
    if( elf_file_hnd->flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
        CONV_BE_16( e_hdr->e_type );
        CONV_BE_16( e_hdr->e_machine );
        CONV_BE_32( e_hdr->e_version );
        CONV_BE_32( e_hdr->e_entry );
        CONV_BE_32( e_hdr->e_phoff );
        CONV_BE_32( e_hdr->e_shoff );
        CONV_BE_32( e_hdr->e_flags );
        CONV_BE_16( e_hdr->e_ehsize );
        CONV_BE_16( e_hdr->e_phentsize );
        CONV_BE_16( e_hdr->e_phnum );
        CONV_BE_16( e_hdr->e_shentsize );
        CONV_BE_16( e_hdr->e_shnum );
        CONV_BE_16( e_hdr->e_shstrndx );
    } else {
        CONV_LE_16( e_hdr->e_type );
        CONV_LE_16( e_hdr->e_machine );
        CONV_LE_32( e_hdr->e_version );
        CONV_LE_32( e_hdr->e_entry );
        CONV_LE_32( e_hdr->e_phoff );
        CONV_LE_32( e_hdr->e_shoff );
        CONV_LE_32( e_hdr->e_flags );
        CONV_LE_16( e_hdr->e_ehsize );
        CONV_LE_16( e_hdr->e_phentsize );
        CONV_LE_16( e_hdr->e_phnum );
        CONV_LE_16( e_hdr->e_shentsize );
        CONV_LE_16( e_hdr->e_shnum );
        CONV_LE_16( e_hdr->e_shstrndx );
    }
}


static void fix_ehdr64_byte_order( elf_file_handle elf_file_hnd, Elf64_Ehdr *e_hdr )
{
    // note that one of the branches will always get compiled out,
    // depending on host endianness
    if( elf_file_hnd->flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
        CONV_BE_16( e_hdr->e_type );
        CONV_BE_16( e_hdr->e_machine );
        CONV_BE_32( e_hdr->e_version );
        SCONV_BE_64( e_hdr->e_entry );
        SCONV_BE_64( e_hdr->e_phoff );
        SCONV_BE_64( e_hdr->e_shoff );
        CONV_BE_32( e_hdr->e_flags );
        CONV_BE_16( e_hdr->e_ehsize );
        CONV_BE_16( e_hdr->e_phentsize );
        CONV_BE_16( e_hdr->e_phnum );
        CONV_BE_16( e_hdr->e_shentsize );
        CONV_BE_16( e_hdr->e_shnum );
        CONV_BE_16( e_hdr->e_shstrndx );
    } else {
        CONV_LE_16( e_hdr->e_type );
        CONV_LE_16( e_hdr->e_machine );
        CONV_LE_32( e_hdr->e_version );
        SCONV_LE_64( e_hdr->e_entry );
        SCONV_LE_64( e_hdr->e_phoff );
        SCONV_LE_64( e_hdr->e_shoff );
        CONV_LE_32( e_hdr->e_flags );
        CONV_LE_16( e_hdr->e_ehsize );
        CONV_LE_16( e_hdr->e_phentsize );
        CONV_LE_16( e_hdr->e_phnum );
        CONV_LE_16( e_hdr->e_shentsize );
        CONV_LE_16( e_hdr->e_shnum );
        CONV_LE_16( e_hdr->e_shstrndx );
    }
}


static void fix_shdr_byte_order( elf_file_handle elf_file_hnd, Elf32_Shdr *e_shdr )
{
    // note that one of the branches will always get compiled out,
    // depending on host endianness
    if( elf_file_hnd->flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
        CONV_BE_32( e_shdr->sh_name );
        CONV_BE_32( e_shdr->sh_type );
        CONV_BE_32( e_shdr->sh_flags );
        CONV_BE_32( e_shdr->sh_addr );
        CONV_BE_32( e_shdr->sh_offset );
        CONV_BE_32( e_shdr->sh_size );
        CONV_BE_32( e_shdr->sh_link );
        CONV_BE_32( e_shdr->sh_info );
        CONV_BE_32( e_shdr->sh_addralign );
        CONV_BE_32( e_shdr->sh_entsize );
    } else {
        CONV_LE_32( e_shdr->sh_name );
        CONV_LE_32( e_shdr->sh_type );
        CONV_LE_32( e_shdr->sh_flags );
        CONV_LE_32( e_shdr->sh_addr );
        CONV_LE_32( e_shdr->sh_offset );
        CONV_LE_32( e_shdr->sh_size );
        CONV_LE_32( e_shdr->sh_link );
        CONV_LE_32( e_shdr->sh_info );
        CONV_LE_32( e_shdr->sh_addralign );
        CONV_LE_32( e_shdr->sh_entsize );
    }
}


static void fix_shdr64_byte_order( elf_file_handle elf_file_hnd, Elf64_Shdr *e_shdr )
{
    // note that one of the branches will always get compiled out,
    // depending on host endianness
    if( elf_file_hnd->flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
        CONV_BE_32( e_shdr->sh_name );
        CONV_BE_32( e_shdr->sh_type );
        SCONV_BE_64( e_shdr->sh_flags );
        SCONV_BE_64( e_shdr->sh_addr );
        SCONV_BE_64( e_shdr->sh_offset );
        SCONV_BE_64( e_shdr->sh_size );
        CONV_BE_32( e_shdr->sh_link );
        CONV_BE_32( e_shdr->sh_info );
        SCONV_BE_64( e_shdr->sh_addralign );
        SCONV_BE_64( e_shdr->sh_entsize );
    } else {
        CONV_LE_32( e_shdr->sh_name );
        CONV_LE_32( e_shdr->sh_type );
        SCONV_LE_64( e_shdr->sh_flags );
        SCONV_LE_64( e_shdr->sh_addr );
        SCONV_LE_64( e_shdr->sh_offset );
        SCONV_LE_64( e_shdr->sh_size );
        CONV_LE_32( e_shdr->sh_link );
        CONV_LE_32( e_shdr->sh_info );
        SCONV_LE_64( e_shdr->sh_addralign );
        SCONV_LE_64( e_shdr->sh_entsize );
    }
}


// This function needs to be run before anything else, because it determines
// how the rest of the data is to be interpreted
static void determine_file_class( elf_file_handle elf_file_hnd, Elf32_Ehdr *e_hdr )
{
    elf_file_hnd->flags = 0;
    switch( e_hdr->e_ident[EI_DATA] ) {
    case ELFLITTLEENDIAN:
        elf_file_hnd->flags |= ORL_FILE_FLAG_LITTLE_ENDIAN;
        break;
    case ELFBIGENDIAN:
        elf_file_hnd->flags |= ORL_FILE_FLAG_BIG_ENDIAN;
        break;
    }
    switch( e_hdr->e_ident[EI_CLASS] ) {
    case ELFCLASS32:
        elf_file_hnd->flags |= ORL_FILE_FLAG_32BIT_MACHINE;
        break;
    case ELFCLASS64:
        elf_file_hnd->flags |= ORL_FILE_FLAG_64BIT_MACHINE;
        break;
    }
}


static void determine_file_specs( elf_file_handle elf_file_hnd, elf_half e_machine, elf_half e_type )
{
    switch( e_machine ) {
    case EM_M32:
        elf_file_hnd->machine_type = ORL_MACHINE_TYPE_WE_2100;
        break;
    case EM_SPARC:
        elf_file_hnd->machine_type = ORL_MACHINE_TYPE_SPARC;
        break;
    case EM_SPARC32PLUS:
        elf_file_hnd->machine_type = ORL_MACHINE_TYPE_SPARCPLUS;
        break;
    case EM_386:
        elf_file_hnd->machine_type = ORL_MACHINE_TYPE_I386;
        break;
    case EM_X86_64:
        elf_file_hnd->machine_type = ORL_MACHINE_TYPE_AMD64;
        break;
    case EM_68K:
        elf_file_hnd->machine_type = ORL_MACHINE_TYPE_M68000;
        break;
    case EM_88K:
        elf_file_hnd->machine_type = ORL_MACHINE_TYPE_M88000;
        break;
    case EM_860:
        elf_file_hnd->machine_type = ORL_MACHINE_TYPE_I860;
        break;
    case EM_PPC:
        elf_file_hnd->machine_type = ORL_MACHINE_TYPE_PPC601;
        break;
    case EM_MIPS:
        elf_file_hnd->machine_type = ORL_MACHINE_TYPE_R3000;
        break;
    case EM_ALPHA:
        elf_file_hnd->machine_type = ORL_MACHINE_TYPE_ALPHA;
        break;
    default:
        elf_file_hnd->machine_type = ORL_MACHINE_TYPE_NONE;
    }
    switch( e_type ) {
    case ET_REL:
        elf_file_hnd->type = ORL_FILE_TYPE_OBJECT;
        break;
    case ET_EXEC:
        elf_file_hnd->type = ORL_FILE_TYPE_EXECUTABLE;
        break;
    case ET_DYN:
        elf_file_hnd->type = ORL_FILE_TYPE_SHARED_OBJECT;
        break;
    case ET_CORE:
        elf_file_hnd->type = ORL_FILE_TYPE_CORE;
        break;
    default:
        elf_file_hnd->type = ORL_FILE_TYPE_NONE;
        break;
    }
// fixme:  add computer-specific (PPC) flags, if any!
}


static void determine_section_specs( elf_sec_handle elf_sec_hnd, elf_word sh_type, unsigned_32 sh_flags )
{
    elf_sec_hnd->flags = ORL_SEC_FLAG_READ_PERMISSION;
    switch( sh_type ) {
    case SHT_PROGBITS:
        elf_sec_hnd->type = ORL_SEC_TYPE_PROG_BITS;
        elf_sec_hnd->flags |= ORL_SEC_FLAG_INITIALIZED_DATA;
        break;
    case SHT_SYMTAB:
        elf_sec_hnd->type = ORL_SEC_TYPE_SYM_TABLE;
        break;
    case SHT_STRTAB:
        elf_sec_hnd->type = ORL_SEC_TYPE_STR_TABLE;
        break;
    case SHT_RELA:
        elf_sec_hnd->type = ORL_SEC_TYPE_RELOCS_EXPADD;
        break;
    case SHT_HASH:
        elf_sec_hnd->type = ORL_SEC_TYPE_HASH;
        break;
    case SHT_DYNAMIC:
        elf_sec_hnd->type = ORL_SEC_TYPE_DYNAMIC;
        break;
    case SHT_NOTE:
        elf_sec_hnd->type = ORL_SEC_TYPE_NOTE;
        break;
    case SHT_NOBITS:
        elf_sec_hnd->type = ORL_SEC_TYPE_NO_BITS;
        elf_sec_hnd->flags |= ORL_SEC_FLAG_UNINITIALIZED_DATA;
        break;
    case SHT_REL:
        elf_sec_hnd->type = ORL_SEC_TYPE_RELOCS;
        break;
    case SHT_DYNSYM:
        elf_sec_hnd->type = ORL_SEC_TYPE_DYN_SYM_TABLE;
        break;
    case SHT_OS:
    case SHT_OS_O:
        elf_sec_hnd->type = ORL_SEC_TYPE_PROG_BITS;
        break;
    case SHT_IMPORTS:
    case SHT_IMPORTS_O:
        elf_sec_hnd->type = ORL_SEC_TYPE_IMPORT;
        break;
    case SHT_EXPORTS:
    case SHT_EXPORTS_O:
        elf_sec_hnd->type = ORL_SEC_TYPE_EXPORT;
        break;
    case SHT_RES:
    case SHT_RES_O:
        elf_sec_hnd->type = ORL_SEC_TYPE_PROG_BITS;
        break;
    default:
        elf_sec_hnd->type = ORL_SEC_TYPE_NONE;
        break;
    }
    if( sh_flags & SHF_WRITE ) {
        elf_sec_hnd->flags |= ORL_SEC_FLAG_WRITE_PERMISSION;
    }
    if( (sh_flags & SHF_ALLOC) == 0 ) {
        elf_sec_hnd->flags |= ORL_SEC_FLAG_REMOVE;
    }
    if( sh_flags & SHF_EXECINSTR ) {
        // set execute permission also?
        elf_sec_hnd->flags |= ORL_SEC_FLAG_EXEC;
    }
}


static void free_elf_sec_handles( elf_file_handle elf_file_hnd, elf_quantity num_alloced )
{
    elf_quantity    i;

    if( elf_file_hnd->sec_handles != NULL ) {
        for( i = 0; i < num_alloced; i++ ) {
            _ClientFree( elf_file_hnd, elf_file_hnd->sec_handles[i] );
        }
        _ClientFree( elf_file_hnd, elf_file_hnd->sec_handles );
    } else {
        for( i = 0; i < num_alloced; i++ ) {
            _ClientFree( elf_file_hnd, elf_file_hnd->orig_sec_handles[i] );
        }
    }
    _ClientFree( elf_file_hnd, elf_file_hnd->orig_sec_handles );
}


static orl_return load_elf_sec_handles( elf_file_handle elf_file_hnd, elf_index *name_index, char *s_hdr )
{
    Elf32_Shdr          *s_hdr32;
    Elf64_Shdr          *s_hdr64;
    elf_sec_handle      elf_sec_hnd;
    elf_quantity        i;
    elf_index           *associated_index;
    elf_index           *associated2_index;
    unsigned_32         sh_align;
    elf_word            sh_type;
    elf_index           sh_link;
    elf_index           sh_info;
    unsigned_32         sh_flags;

    associated_index = (elf_index *)_ClientAlloc( elf_file_hnd, sizeof( elf_index ) * elf_file_hnd->num_sections );
    if( associated_index == NULL )
        return( ORL_OUT_OF_MEMORY );
    associated2_index = (elf_index *)_ClientAlloc( elf_file_hnd, sizeof( elf_index ) * elf_file_hnd->num_sections );
    if( associated2_index == NULL ) {
        _ClientFree( elf_file_hnd, associated_index );
        return( ORL_OUT_OF_MEMORY );
    }
    elf_file_hnd->symbol_table = NULL;
    elf_file_hnd->sec_handles = NULL;
    elf_file_hnd->orig_sec_handles = (elf_sec_handle *)_ClientAlloc( elf_file_hnd, sizeof( elf_sec_handle ) * elf_file_hnd->num_sections );
    if( elf_file_hnd->orig_sec_handles == NULL ) {
        _ClientFree( elf_file_hnd, associated_index );
        _ClientFree( elf_file_hnd, associated2_index );
        return( ORL_OUT_OF_MEMORY );
    }
    s_hdr += elf_file_hnd->shentsize; // skip over index 0
    for( i = 0; i < elf_file_hnd->num_sections; ++i ) {
        elf_sec_hnd = (elf_sec_handle)_ClientAlloc( elf_file_hnd, ORL_STRUCT_SIZEOF( elf_sec_handle ) );
        if( elf_sec_hnd == NULL ) {
            free_elf_sec_handles( elf_file_hnd, i );
            _ClientFree( elf_file_hnd, associated_index );
            _ClientFree( elf_file_hnd, associated2_index );
            _ClientFree( elf_file_hnd, elf_file_hnd->orig_sec_handles );
            return( ORL_OUT_OF_MEMORY );
        }
        elf_file_hnd->orig_sec_handles[i] = elf_sec_hnd;
        elf_sec_hnd->file_format = ORL_ELF;
        elf_sec_hnd->elf_file_hnd = elf_file_hnd;
        elf_sec_hnd->index = i + 1;

        if( elf_file_hnd->flags & ORL_FILE_FLAG_64BIT_MACHINE ) {
            s_hdr64 = (Elf64_Shdr *)s_hdr;
            fix_shdr64_byte_order( elf_file_hnd, s_hdr64 );
            name_index[i] = s_hdr64->sh_name;
            elf_sec_hnd->size.u._32[I64HI32] = s_hdr64->sh_size.u._32[I64HI32];
            elf_sec_hnd->size.u._32[I64LO32] = s_hdr64->sh_size.u._32[I64LO32];
            elf_sec_hnd->base.u._32[I64HI32] = s_hdr64->sh_addr.u._32[I64HI32];
            elf_sec_hnd->base.u._32[I64LO32] = s_hdr64->sh_addr.u._32[I64LO32];
            elf_sec_hnd->file_offset.u._32[I64HI32] = s_hdr64->sh_offset.u._32[I64HI32];
            elf_sec_hnd->file_offset.u._32[I64LO32] = s_hdr64->sh_offset.u._32[I64LO32];
            elf_sec_hnd->entsize.u._32[I64HI32] = s_hdr64->sh_entsize.u._32[I64HI32];
            elf_sec_hnd->entsize.u._32[I64LO32] = s_hdr64->sh_entsize.u._32[I64LO32];
            sh_align = s_hdr64->sh_addralign.u._32[I64LO32];
            sh_flags = s_hdr64->sh_flags.u._32[I64LO32];
            sh_type = s_hdr64->sh_type;
            sh_link = s_hdr64->sh_link;
            sh_info = s_hdr64->sh_info;
        } else {
            s_hdr32 = (Elf32_Shdr *)s_hdr;
            fix_shdr_byte_order( elf_file_hnd, s_hdr32 );
            name_index[i] = s_hdr32->sh_name;
            elf_sec_hnd->size.u._32[I64HI32] = 0;
            elf_sec_hnd->size.u._32[I64LO32] = s_hdr32->sh_size;
            elf_sec_hnd->base.u._32[I64HI32] = 0;
            elf_sec_hnd->base.u._32[I64LO32] = s_hdr32->sh_addr;
            elf_sec_hnd->file_offset.u._32[I64HI32] = 0;
            elf_sec_hnd->file_offset.u._32[I64LO32] = s_hdr32->sh_offset;
            elf_sec_hnd->entsize.u._32[I64HI32] = 0;
            elf_sec_hnd->entsize.u._32[I64LO32] = s_hdr32->sh_entsize;
            sh_align = s_hdr32->sh_addralign;
            sh_flags = s_hdr32->sh_flags;
            sh_type = s_hdr32->sh_type;
            sh_link = s_hdr32->sh_link;
            sh_info = s_hdr32->sh_info;
        }
        switch( sh_align ) {
        case 0:
        case 1:
            elf_sec_hnd->alignment = 0;
            break;
        case 2:
            elf_sec_hnd->alignment = 1;
            break;
        case 4:
            elf_sec_hnd->alignment = 2;
            break;
        case 8:
            elf_sec_hnd->alignment = 3;
            break;
        case 16:
            elf_sec_hnd->alignment = 4;
            break;
        case 32:
            elf_sec_hnd->alignment = 5;
            break;
        case 64:
            elf_sec_hnd->alignment = 6;
            break;
        case 128:
            elf_sec_hnd->alignment = 7;
            break;
        case 256:
            elf_sec_hnd->alignment = 8;
            break;
        }
        elf_sec_hnd->contents = NULL;
        memset( &(elf_sec_hnd->assoc), '\0', sizeof( elf_sec_hnd->assoc ) );
        determine_section_specs( elf_sec_hnd, sh_type, sh_flags );
        switch( elf_sec_hnd->type ) {
        case ORL_SEC_TYPE_SYM_TABLE:
            elf_file_hnd->symbol_table = elf_sec_hnd;
            /* fall through */
        case ORL_SEC_TYPE_DYN_SYM_TABLE:
        case ORL_SEC_TYPE_IMPORT:
        case ORL_SEC_TYPE_EXPORT:
            associated_index[i] = sh_link - 1;
            break;
        case ORL_SEC_TYPE_RELOCS:
        case ORL_SEC_TYPE_RELOCS_EXPADD:
            // Certain funky toolchains produce two reloc sections for each
            // section containing relocations (both .rel and .rela) and one of
            // them is empty. We have to ignore the empty one!
            if( elf_sec_hnd->size.u._32[I64LO32] != 0 || elf_sec_hnd->size.u._32[I64HI32] != 0 ) {
                associated_index[i] = sh_info - 1;
                associated2_index[i] = sh_link - 1;
            } else {
                associated_index[i] = (elf_index)-1;
                associated2_index[i] = (elf_index)-1;
            }
            break;
        default:
            break;
        }
        s_hdr += elf_file_hnd->shentsize;
    }
    for( i = 0; i < elf_file_hnd->num_sections; ++i ) {
        elf_sec_hnd = elf_file_hnd->orig_sec_handles[i];
        switch( elf_sec_hnd->type ) {
        case ORL_SEC_TYPE_SYM_TABLE:
        case ORL_SEC_TYPE_DYN_SYM_TABLE:
            elf_sec_hnd->assoc.sym.string_table = elf_file_hnd->orig_sec_handles[associated_index[i]];
            break;
        case ORL_SEC_TYPE_IMPORT:
            elf_sec_hnd->assoc.import.string_table = elf_file_hnd->orig_sec_handles[associated_index[i]];
            break;
        case ORL_SEC_TYPE_EXPORT:
            elf_sec_hnd->assoc.export.symbol_table = elf_file_hnd->orig_sec_handles[associated_index[i]];
            break;
        case ORL_SEC_TYPE_RELOCS:
        case ORL_SEC_TYPE_RELOCS_EXPADD:
            elf_sec_hnd->assoc.reloc.symbol_table = elf_file_hnd->orig_sec_handles[associated2_index[i]];
            // some silly people create reloc sections with no associated section
            if( associated_index[i] != (elf_index)-1 ) {
                elf_sec_hnd->assoc.reloc.orig_sec = elf_file_hnd->orig_sec_handles[associated_index[i]];
                elf_file_hnd->orig_sec_handles[associated_index[i]]->assoc.normal.reloc_sec = elf_sec_hnd;
            }
            break;
        default:
            break;
        }
    }
    elf_file_hnd->sec_handles = (elf_sec_handle *)_ClientAlloc( elf_file_hnd, sizeof( elf_sec_handle ) * elf_file_hnd->num_sections );
    if( elf_file_hnd->sec_handles == NULL ) {
        _ClientFree( elf_file_hnd, associated_index );
        _ClientFree( elf_file_hnd, associated2_index );
        _ClientFree( elf_file_hnd, elf_file_hnd->orig_sec_handles );
        return( ORL_OUT_OF_MEMORY );
    }
    memcpy( elf_file_hnd->sec_handles, elf_file_hnd->orig_sec_handles, sizeof( elf_sec_handle ) * elf_file_hnd->num_sections );
    _ClientFree( elf_file_hnd, associated_index );
    _ClientFree( elf_file_hnd, associated2_index );
    return( ORL_OKAY );
}


static int section_compare( const void *sec1, const void *sec2 )
{
    if( (*(const elf_sec_handle *)sec1)->file_offset.u._32[I64HI32] > (*(const elf_sec_handle *)sec2)->file_offset.u._32[I64HI32] )
        return( 1 );
    if( (*(const elf_sec_handle *)sec1)->file_offset.u._32[I64HI32] < (*(const elf_sec_handle *)sec2)->file_offset.u._32[I64HI32] )
        return( -1 );
    if( (*(const elf_sec_handle *)sec1)->file_offset.u._32[I64LO32] > (*(const elf_sec_handle *)sec2)->file_offset.u._32[I64LO32] )
        return( 1 );
    if( (*(const elf_sec_handle *)sec1)->file_offset.u._32[I64LO32] < (*(const elf_sec_handle *)sec2)->file_offset.u._32[I64LO32] )
        return( -1 );
    return( 0 );
}


orl_return ElfLoadFileStructure( elf_file_handle elf_file_hnd )
{
    orl_return          return_val;
    char                *s_hdr;
    Elf32_Ehdr          *e_hdr32;
    Elf64_Ehdr          *e_hdr64;
    elf_sec_handle      elf_sec_hnd;
    elf_quantity        contents_size1;
    elf_quantity        contents_size2;
    elf_quantity        sec_header_table_size;
    elf_quantity        i;
    elf_index           *name_index;
    char                *string_table;
    elf_file_offset     shoff;
    elf_half            shnum;
    elf_half            ehsize;
    elf_half            shstrndx;

    e_hdr32 = _ClientRead( elf_file_hnd, sizeof( e_hdr32->e_ident ) );
    if( e_hdr32 == NULL )
        return( ORL_ERROR );
    determine_file_class( elf_file_hnd, e_hdr32 );
    _ClientSeek( elf_file_hnd, 0, SEEK_SET );
    if( elf_file_hnd->flags & ORL_FILE_FLAG_64BIT_MACHINE ) {
        e_hdr64 = _ClientRead( elf_file_hnd, sizeof( Elf64_Ehdr ) );
        if( e_hdr64 == NULL )
            return( ORL_ERROR );
        fix_ehdr64_byte_order( elf_file_hnd, e_hdr64 );
        shoff.u._32[I64HI32] = e_hdr64->e_shoff.u._32[I64HI32];
        shoff.u._32[I64LO32] = e_hdr64->e_shoff.u._32[I64LO32];
        shnum = e_hdr64->e_shnum;
        ehsize = e_hdr64->e_ehsize;
        shstrndx = e_hdr64->e_shstrndx;
        elf_file_hnd->shentsize = e_hdr64->e_shentsize;
        determine_file_specs( elf_file_hnd, e_hdr64->e_machine, e_hdr64->e_type );
    } else {
        e_hdr32 = _ClientRead( elf_file_hnd, sizeof( Elf32_Ehdr ) );
        if( e_hdr32 == NULL )
            return( ORL_ERROR );
        fix_ehdr_byte_order( elf_file_hnd, e_hdr32 );
        shoff.u._32[I64HI32] = 0;
        shoff.u._32[I64LO32] = e_hdr32->e_shoff;
        shnum = e_hdr32->e_shnum;
        ehsize = e_hdr32->e_ehsize;
        shstrndx = e_hdr32->e_shstrndx;
        elf_file_hnd->shentsize = e_hdr32->e_shentsize;
        determine_file_specs( elf_file_hnd, e_hdr32->e_machine, e_hdr32->e_type );
    }
    elf_file_hnd->num_sections = shnum - 1; // -1 to ignore shdr table index

    contents_size1 = shoff.u._32[I64LO32] - ehsize;
    sec_header_table_size = elf_file_hnd->shentsize * shnum;

    // e_ehsize might not be the same as sizeof( Elf32_Ehdr ) (different versions)
    _ClientSeek( elf_file_hnd, ehsize, SEEK_SET );

    if( contents_size1 > 0 ) {
        elf_file_hnd->contents_buffer1 = _ClientRead( elf_file_hnd, contents_size1 );
        if( elf_file_hnd->contents_buffer1 == NULL ) {
            return( ORL_ERROR );
        }
    } else {
        elf_file_hnd->contents_buffer1 = NULL;
    }
    s_hdr = _ClientRead( elf_file_hnd, sec_header_table_size );
    if( s_hdr == NULL )
        return( ORL_ERROR );
    name_index = _ClientAlloc( elf_file_hnd, sizeof( elf_index ) * elf_file_hnd->num_sections );
    if( name_index == NULL )
        return( ORL_OUT_OF_MEMORY );
    return_val = load_elf_sec_handles( elf_file_hnd, name_index, s_hdr );
    if( return_val != ORL_OKAY ) {
        _ClientFree( elf_file_hnd, name_index );
        return( return_val );
    }
    // now sort the section handles by file offset
    qsort( elf_file_hnd->sec_handles, elf_file_hnd->num_sections, sizeof( elf_sec_handle ), section_compare );

    elf_sec_hnd = elf_file_hnd->sec_handles[elf_file_hnd->num_sections - 1];
    contents_size2 = elf_sec_hnd->file_offset.u._32[I64LO32] + elf_sec_hnd->size.u._32[I64LO32] - shoff.u._32[I64LO32] - sec_header_table_size;
    if( contents_size2 > 0 ) {
        elf_file_hnd->size = elf_sec_hnd->file_offset.u._32[I64LO32] + elf_sec_hnd->size.u._32[I64LO32];
        elf_file_hnd->contents_buffer2 = _ClientRead( elf_file_hnd, contents_size2 );
        if( elf_file_hnd->contents_buffer2 == NULL ) {
            _ClientFree( elf_file_hnd, name_index );
            return( ORL_ERROR );
        }
    } else {
        elf_file_hnd->size = shoff.u._32[I64LO32] + sec_header_table_size;
    }
    // determine contents pointers of all sections
    for( i = 0; i < elf_file_hnd->num_sections; ++i ) {
        elf_sec_hnd = elf_file_hnd->sec_handles[i];
        if( elf_sec_hnd->size.u._32[I64LO32] > 0 ) {
            if( elf_sec_hnd->file_offset.u._32[I64LO32] < shoff.u._32[I64LO32] ) {
                elf_sec_hnd->contents = elf_file_hnd->contents_buffer1 + elf_sec_hnd->file_offset.u._32[I64LO32] - ehsize;
            } else {
                elf_sec_hnd->contents = elf_file_hnd->contents_buffer2 + elf_sec_hnd->file_offset.u._32[I64LO32] - shoff.u._32[I64LO32] - sec_header_table_size;
            }
        }
    }
    // determine section names
    string_table = (char *)elf_file_hnd->orig_sec_handles[shstrndx - 1]->contents;
    for( i = 0; i < elf_file_hnd->num_sections; ++i ) {
        elf_file_hnd->orig_sec_handles[i]->name = string_table + name_index[i];
    }
    _ClientFree( elf_file_hnd, name_index );
    return( ORL_OKAY );
}
