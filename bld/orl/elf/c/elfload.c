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


#include "elfload.h"

// fixme: finish making ELF SPECIFIC (see next fixme)
static void determine_file_specs( elf_file_handle elf_file_hnd, Elf32_Ehdr *e_hdr )
{
    switch( e_hdr->e_machine ) {
        case EM_M32:
            elf_file_hnd->machine_type = ORL_MACHINE_TYPE_WE_2100;
            break;
        case EM_SPARC:
            elf_file_hnd->machine_type = ORL_MACHINE_TYPE_SPARC;
            break;
        case EM_386:
            elf_file_hnd->machine_type = ORL_MACHINE_TYPE_I386;
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
        case EM_PPC_O:
            elf_file_hnd->machine_type = ORL_MACHINE_TYPE_PPC601;
            break;
        default:
            elf_file_hnd->machine_type = ORL_MACHINE_TYPE_NONE;
    }
    switch( e_hdr->e_type ) {
        case ET_REL:
            elf_file_hnd->type = ORL_FILE_TYPE_OBJECT;
            break;
        case ET_EXEC:
            elf_file_hnd->type = ORL_FILE_TYPE_EXECUTABLE;
            break;
        case ET_DYN:
            elf_file_hnd->type = ORL_FILE_TYPE_SHARED_OBJECT;
            break;
        default:
            elf_file_hnd->type = ORL_FILE_TYPE_NONE;
            break;
    }
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
// fixme:  add computer-specific (PPC) flags, if any!
}

static void determine_section_specs( elf_sec_handle elf_sec_hnd, Elf32_Shdr * s_hdr )
{
    elf_sec_hnd->flags = ORL_SEC_FLAG_READ_PERMISSION;
    switch( s_hdr->sh_type ) {
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
    if( s_hdr->sh_flags & SHF_WRITE ) {
        elf_sec_hnd->flags |= ORL_SEC_FLAG_WRITE_PERMISSION;
    }
    if( !(s_hdr->sh_flags & SHF_ALLOC) ) {
        elf_sec_hnd->flags |= ORL_SEC_FLAG_REMOVE;
    }
    if( s_hdr->sh_flags & SHF_EXECINSTR ) {
        // set execute permission also?
        elf_sec_hnd->flags |= ORL_SEC_FLAG_EXEC;
    }
}

static void free_elf_sec_handles( elf_file_handle elf_file_hnd, int num_alloced )
{
    int                                 loop;

    if( elf_file_hnd->elf_sec_hnd != NULL ) {
        for( loop = 0; loop < num_alloced; loop++ ) {
            _ClientFree( elf_file_hnd, elf_file_hnd->elf_sec_hnd[loop] );
        }
        _ClientFree( elf_file_hnd, elf_file_hnd->elf_sec_hnd );
    } else {
        for( loop = 0; loop < num_alloced; loop++ ) {
            _ClientFree( elf_file_hnd, elf_file_hnd->orig_sec_hnd[loop] );
        }
    }
    _ClientFree( elf_file_hnd, elf_file_hnd->orig_sec_hnd );
}

static orl_return load_elf_sec_handles( elf_file_handle elf_file_hnd, orl_sec_offset *name_index )
{
    Elf32_Shdr *        s_hdr;
    elf_sec_handle      elf_sec_hnd;
    int                 loop;
    orl_sec_offset *    associated_index;
    orl_sec_offset *    associated2_index;

    associated_index = (orl_sec_offset *) _ClientAlloc( elf_file_hnd, sizeof( Elf32_Word ) * elf_file_hnd->num_sections );
    if( !associated_index ) return( ORL_OUT_OF_MEMORY );
    associated2_index = (orl_sec_offset *) _ClientAlloc( elf_file_hnd, sizeof( Elf32_Word ) * elf_file_hnd->num_sections );
    if( !associated2_index ) {
        _ClientFree( elf_file_hnd, associated_index );
        return( ORL_OUT_OF_MEMORY );
    }
    elf_file_hnd->symbol_table = NULL;
    elf_file_hnd->elf_sec_hnd = NULL;
    elf_file_hnd->orig_sec_hnd = (elf_sec_handle *) _ClientAlloc( elf_file_hnd, sizeof( elf_sec_handle ) * elf_file_hnd->num_sections );
    if( !( elf_file_hnd->orig_sec_hnd ) ) {
        _ClientFree( elf_file_hnd, associated_index );
        _ClientFree( elf_file_hnd, associated2_index );
        return( ORL_OUT_OF_MEMORY );
    }
    s_hdr = (Elf32_Shdr *) elf_file_hnd->s_hdr_table_buffer;
    s_hdr++; // skip over index 0
    for( loop = 0; loop < elf_file_hnd->num_sections; loop++ ) {
        elf_sec_hnd = (elf_sec_handle) _ClientAlloc( elf_file_hnd, sizeof( elf_sec_handle_struct ) );
        if( !elf_sec_hnd ) {
            free_elf_sec_handles( elf_file_hnd, loop );
            _ClientFree( elf_file_hnd, associated_index );
            _ClientFree( elf_file_hnd, associated2_index );
            _ClientFree( elf_file_hnd, elf_file_hnd->orig_sec_hnd );
            return( ORL_OUT_OF_MEMORY );
        }
        elf_file_hnd->orig_sec_hnd[loop] = elf_sec_hnd;
        name_index[loop] = s_hdr->sh_name;
        elf_sec_hnd->file_format = ORL_ELF;
        elf_sec_hnd->elf_file_hnd = elf_file_hnd;
        elf_sec_hnd->size = s_hdr->sh_size;
        elf_sec_hnd->base = s_hdr->sh_addr;
        elf_sec_hnd->index = loop + 1;
        elf_sec_hnd->offset = s_hdr->sh_offset;
        switch( s_hdr->sh_addralign ) {
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
        determine_section_specs( elf_sec_hnd, s_hdr );
        switch( elf_sec_hnd->type ) {
            case ORL_SEC_TYPE_SYM_TABLE:
                elf_file_hnd->symbol_table = elf_sec_hnd;
            case ORL_SEC_TYPE_DYN_SYM_TABLE:
            case ORL_SEC_TYPE_IMPORT:
            case ORL_SEC_TYPE_EXPORT:
                associated_index[loop] = s_hdr->sh_link - 1;
                break;
            case ORL_SEC_TYPE_RELOCS:
            case ORL_SEC_TYPE_RELOCS_EXPADD:
                associated_index[loop] = s_hdr->sh_info - 1;
                associated2_index[loop] = s_hdr->sh_link - 1;
                break;
        }
        s_hdr++;
    }
    for( loop = 0; loop < elf_file_hnd->num_sections; loop++ ) {
        elf_sec_hnd = elf_file_hnd->orig_sec_hnd[loop];
        switch( elf_sec_hnd->type ) {
            case ORL_SEC_TYPE_SYM_TABLE:
            case ORL_SEC_TYPE_DYN_SYM_TABLE:
                elf_sec_hnd->assoc.sym.string_table = elf_file_hnd->orig_sec_hnd[associated_index[loop]];
                break;
            case ORL_SEC_TYPE_IMPORT:
                elf_sec_hnd->assoc.import.string_table = elf_file_hnd->orig_sec_hnd[associated_index[loop]];
                break;
            case ORL_SEC_TYPE_EXPORT:
                elf_sec_hnd->assoc.export.symbol_table = elf_file_hnd->orig_sec_hnd[associated_index[loop]];
                break;
            case ORL_SEC_TYPE_RELOCS:
            case ORL_SEC_TYPE_RELOCS_EXPADD:
                elf_sec_hnd->assoc.reloc.symbol_table = elf_file_hnd->orig_sec_hnd[associated2_index[loop]];
                elf_sec_hnd->assoc.reloc.orig_sec = elf_file_hnd->orig_sec_hnd[associated_index[loop]];
                elf_file_hnd->orig_sec_hnd[associated_index[loop]]->assoc.normal.reloc_sec = elf_sec_hnd;
                break;
        }
    }
    elf_file_hnd->elf_sec_hnd = (elf_sec_handle *) _ClientAlloc( elf_file_hnd, sizeof( elf_sec_handle ) * elf_file_hnd->num_sections );
    if( !( elf_file_hnd->elf_sec_hnd ) ) {
        _ClientFree( elf_file_hnd, associated_index );
        _ClientFree( elf_file_hnd, associated2_index );
        _ClientFree( elf_file_hnd, elf_file_hnd->orig_sec_hnd );
        return( ORL_OUT_OF_MEMORY );
    }
    memcpy( elf_file_hnd->elf_sec_hnd, elf_file_hnd->orig_sec_hnd, sizeof( elf_sec_handle ) * elf_file_hnd->num_sections );
    _ClientFree( elf_file_hnd, associated_index );
    _ClientFree( elf_file_hnd, associated2_index );
    return( ORL_OKAY );
}

static int sec_compare( const elf_sec_handle *first_sec, const elf_sec_handle *second_sec )
{
    if( (*first_sec)->offset > (*second_sec)->offset ) {
        return( 1 );
    } else if( (*first_sec)->offset < (*second_sec)->offset ) {
        return( -1 );
    } else {
        return( 0 );
    }
}

orl_return ElfLoadFileStructure( elf_file_handle elf_file_hnd )
{
    orl_return          error;
    Elf32_Ehdr *        e_hdr;
    elf_sec_handle      elf_sec_hnd;
    elf_quantity        contents_size1;
    elf_quantity        contents_size2;
    elf_quantity        sec_header_table_size;
    int                 loop;
    orl_sec_offset *    name_index;
    char *              string_table;

    elf_file_hnd->e_hdr_buffer = _ClientRead( elf_file_hnd, sizeof(Elf32_Ehdr));
    if( !(elf_file_hnd->e_hdr_buffer) ) return( ORL_ERROR );
    e_hdr = (Elf32_Ehdr *) elf_file_hnd->e_hdr_buffer;
    determine_file_specs( elf_file_hnd, e_hdr );
    elf_file_hnd->num_sections = e_hdr->e_shnum - 1; // -1 to ignore shdr table index

    contents_size1 = e_hdr->e_shoff - e_hdr->e_ehsize;
    sec_header_table_size = e_hdr->e_shentsize * e_hdr->e_shnum;

    // e_ehsize might not be the same as sizeof(Elf32_Ehdr) (different versions)
    _ClientSeek( elf_file_hnd, e_hdr->e_ehsize - sizeof(Elf32_Ehdr), SEEK_CUR );

    if( contents_size1 > 0 ) {
        elf_file_hnd->contents_buffer1 = _ClientRead( elf_file_hnd,
                                                        contents_size1 );
        if( !(elf_file_hnd->contents_buffer1) ) {
            return( ORL_ERROR );
        }
    } else {
        elf_file_hnd->contents_buffer1 = NULL;
    }
    elf_file_hnd->s_hdr_table_buffer = _ClientRead( elf_file_hnd,
                                                        sec_header_table_size );
    if( !(elf_file_hnd->s_hdr_table_buffer) ) {
        return( ORL_ERROR );
    }
    name_index = _ClientAlloc( elf_file_hnd,
                         sizeof(orl_sec_offset) * elf_file_hnd->num_sections );
    if( !name_index ) {
        return( ORL_OUT_OF_MEMORY );
    }
    error = load_elf_sec_handles( elf_file_hnd, name_index );
    if( error != ORL_OKAY ) {
        _ClientFree( elf_file_hnd, name_index );
        return( error );
    }
    // now sort the section handles by file offset
    qsort( elf_file_hnd->elf_sec_hnd, elf_file_hnd->num_sections,
            sizeof( elf_sec_handle ), sec_compare );
//          (int(*)(const void*, const void*))sec_compare );

    elf_sec_hnd = elf_file_hnd->elf_sec_hnd[elf_file_hnd->num_sections - 1];
    contents_size2 = elf_sec_hnd->offset + elf_sec_hnd->size - e_hdr->e_shoff
                                                        - sec_header_table_size;
    if( contents_size2 > 0 ) {
        elf_file_hnd->size = elf_sec_hnd->offset + elf_sec_hnd->size;
        elf_file_hnd->contents_buffer2 = _ClientRead( elf_file_hnd,
                                                        contents_size2 );
        if( !(elf_file_hnd->contents_buffer2) ) {
            _ClientFree( elf_file_hnd, name_index );
            return( ORL_ERROR );
        }
    } else {
        elf_file_hnd->size = e_hdr->e_shoff + sec_header_table_size;
    }
    // determine contents pointers of all sections
    for( loop = 0; loop < elf_file_hnd->num_sections; loop++ ) {
        elf_sec_hnd = elf_file_hnd->elf_sec_hnd[loop];
        if( elf_sec_hnd->size > 0 ) {
            if( elf_sec_hnd->offset < e_hdr->e_shoff ) {
                elf_sec_hnd->contents = elf_file_hnd->contents_buffer1
                                + elf_sec_hnd->offset - e_hdr->e_ehsize;
            } else {
                elf_sec_hnd->contents = elf_file_hnd->contents_buffer2
                                + elf_sec_hnd->offset - e_hdr->e_shoff
                                - sec_header_table_size;
            }
        }
    }
    // determine section names
    string_table = elf_file_hnd->orig_sec_hnd[e_hdr->e_shstrndx - 1]->contents;
    for( loop = 0; loop < elf_file_hnd->num_sections; loop++ ) {
        elf_file_hnd->orig_sec_hnd[loop]->name = string_table +name_index[loop];
    }
    _ClientFree( elf_file_hnd, name_index );
    return( ORL_OKAY );
}
