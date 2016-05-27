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
* Description:  ELF dumping routines.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>

#include "wdglb.h"
#include "wdfunc.h"


#ifdef __BIG_ENDIAN__
    #define NATIVE_ENDIAN   2
#else
    #define NATIVE_ENDIAN   1
#endif

static  const_string_table elf_exe_msg[] = {
    "2file type (i.e. object, executable file)    = ",
    "2required architecture                       = ",
    "4version of the file                         = ",
    "4program entry point                         = ",
    "4program header offset                       = ",
    "4section header offset                       = ",
    "4processor specific flags                    = ",
    "2ELF header size                             = ",
    "2program header entry size                   = ",
    "2number of program header entries            = ",
    "2section header entry size                   = ",
    "2number of section header entries            = ",
    "2section name string table index             = ",
    NULL
};

static  const_string_table elf_prog_msg[] = {
    "4type of segment                             = ",
    "4offset of segment from beginning of file    = ",
    "4segment virtual address                     = ",
    "4segment physical address                    = ",
    "4size of segment in file                     = ",
    "4size of segment in memory                   = ",
    "4flags                                       = ",
    "4segment align value (in mem & file)         = ",
    NULL
};

static  const_string_table elf_sec_msg[] = {
    "4name of the section                         = ",
    "4section type                                = ",
    "4flags                                       = ",
    "4starting address of section in image        = ",
    "4start of section in file                    = ",
    "4size of section in file                     = ",
    "4section header table index link             = ",
    "4extra information                           = ",
    "4address alignment                           = ",
    "4entry size for sects with fixed sized ents  = ",
    NULL
};


/*
 * Dump the segment type.
 */
static void dmp_hdr_type( unsigned_16 type )
/******************************************/
{
    Wdputs( "file type:                                  " );
    switch( type ) {
    case ET_NONE:
        Wdputs( "No file type" );
        break;
    case ET_REL:
        Wdputs( "Relocatable file" );
        break;
    case ET_EXEC:
        Wdputs( "Executable file" );
        break;
    case ET_DYN:
        Wdputs( "Shared object file" );
        break;
    case ET_CORE:
        Wdputs( "Core file" );
        break;
    case ET_LOPROC:
        Wdputs( "Processor specific" );
        break;
    }
    Wdputslc( "\n" );
}

/*
 * Dump the string table section.
 */
static void dmp_sec_strtab( unsigned_32 offset, unsigned_32 size )
/****************************************************************/
{
    char    *string_table, *ptr;

    string_table = Wmalloc( size );
    Wlseek( offset );
    Wread( string_table, size );

    ptr = string_table;
    while( ptr < (string_table + size) ) {
        if( *ptr ) {
            Puthex( ptr - string_table, 8 );
            Wdputslc( ": " );
            Wdputs( ptr );
            Wdputslc( "\n" );
            ptr += strlen( ptr );
        } else {
            ptr++;
        }
    }

    free( string_table );
}

#define     ELF_ROUND   (sizeof( Elf32_Word ) - 1)

/*
 * Dump a note section.
 */
static void dmp_sec_note( unsigned_32 offset, unsigned_32 size )
/**************************************************************/
{
    Elf_Note        note;
    unsigned_32     read = 0;
    unsigned_32     skip;
    char            *ptr;

    Wlseek( offset );
    while( read < size ) {
        Wdputslc( "\n" );
        Wread( &note, sizeof( note ) );
        read += sizeof( note );
        if( Byte_swap ) {
            SWAP_32( note.n_namesz );
            SWAP_32( note.n_descsz );
            SWAP_32( note.n_type );
        }
        ptr = Wmalloc( note.n_namesz );
        Wread( ptr, note.n_namesz );
        Wdputs( "    note name:                              " );
        Wdputs( ptr );
        Wdputslc( "\n" );
        Wdputs( "    descriptor length:                      " );
        Puthex( note.n_descsz, 8 );
        Wdputslc( "H\n" );
        Wdputs( "    note type:                              " );
        switch( note.n_type ) {
        case NT_PRSTATUS:
            Wdputs( "process status" );
            break;
        case NT_FPREGSET:
            Wdputs( "floating-point registers" );
            break;
        case NT_PRPSINFO:
            Wdputs( "process info" );
            break;
        default:
            Wdputs( "unknown (" );
            Puthex( note.n_type, 8 );
            Wdputs( "H)" );
        }
        Wdputslc( "\n" );
        free( ptr );

        /* Calculate rounded up note name length */
        skip = (note.n_namesz + ELF_ROUND) & ~ELF_ROUND;
        read += skip;
        Wlseek( offset + read );
        /* Calculate rounded up note descriptor length */
        skip = (note.n_descsz + ELF_ROUND) & ~ELF_ROUND;
        read += skip;
        Wlseek( offset + read );
    }
}

/*
 * Dump a progbits section.
 */
static void dmp_sec_progbits( char *name,
    unsigned_32 offset, unsigned_32 size )
/****************************************/
{
    const uint_8    *ptr;
    uint            sect;

    if( name == NULL ) {
        Dmp_seg_data( offset, size );
    } else {
        ptr = Wmalloc( size );
        Wlseek( offset );
        Wread( (char *)ptr, size );
        sect = Lookup_section_name( name );
        Dump_specific_section( sect, ptr, size );
        free( (void *)ptr );
    }
}

/*
 * Dump the segment type.
 */
static void dmp_prog_type( unsigned_32 type )
/*******************************************/
{
    Wdputs( "segment type:                               " );
    switch( type ) {
    case PT_NULL:
        Wdputs( "unused segment" );
        break;
    case PT_LOAD:
        Wdputs( "loadable segment" );
        break;
    case PT_DYNAMIC:
        Wdputs( "dynamic linking info" );
        break;
    case PT_INTERP:
        Wdputs( "program interpreter" );
        break;
    case PT_NOTE:
        Wdputs( "comments & auxiliary info" );
        break;
    case PT_SHLIB:
        Wdputs( "unspecified semantics" );
        break;
    case PT_PHDR:
        Wdputs( "address of progam header" );
        break;
    case PT_LOPROC:
        Wdputs( "processor specific" );
        break;
    }
    Wdputslc( "\n" );
}

/*
 * Dump the section type.
 */
static void dmp_sec_type( unsigned_32 type )
/******************************************/
{
    Wdputs( "section type:                               " );
    switch( type ) {
    case SHT_NULL:
        Wdputs( "inactive" );
        break;
    case SHT_PROGBITS:
        Wdputs( "defined by program" );
        break;
    case SHT_SYMTAB:
        Wdputs( "symbol table" );
        break;
    case SHT_STRTAB:
        Wdputs( "string table" );
        break;
    case SHT_RELA:
        Wdputs( "reloc entries with explicit addends" );
        break;
    case SHT_HASH:
        Wdputs( "symbol hash table" );
        break;
    case SHT_DYNAMIC:
        Wdputs( "dynamic linking info" );
        break;
    case SHT_NOTE:
        Wdputs( "comment info" );
        break;
    case SHT_NOBITS:
        Wdputs( "no space in file" );
        break;
    case SHT_REL:
        Wdputs( "reloc entries" );
        break;
    case SHT_SHLIB:
        Wdputs( "reserved (SHT_SHLIB)" );
        break;
    case SHT_DYNSYM:
        Wdputs( "dynamic link symbol table" );
        break;
    case SHT_LOPROC:
        Wdputs( "processor specific" );
        break;
    }
    Wdputslc( "\n" );
}

/*
 * dump the program flag word
 */
static void dmp_prog_flgs( unsigned_32 flags )
/********************************************/
{
    char    name[128];

    name[0] = 0;
    if( flags & PF_X ) {
        strcat( name, " EXECUTABLE |" );
    }
    if( flags & PF_W ) {
        strcat( name, " WRITABLE |" );
    }
    if( flags & PF_R ) {
        strcat( name, " READABLE |" );
    }
    if( name[strlen(name)-1] == '|' ) {
        name[strlen(name)-1] = 0;
    }
    Wdputs( "flags = " );
    Wdputs( name );
    Wdputslc( "\n" );
}

/*
 * dump the section flag word
 */
static void dmp_sec_flgs( unsigned_32 flags )
/*******************************************/
{
    char    name[128];

    name[0] = 0;
    if( flags & SHF_WRITE ) {
        strcat( name, " WRITABLE |" );
    }
    if( flags & SHF_ALLOC ) {
        strcat( name, " ALLOC_SPACE |" );
    }
    if( flags & SHF_EXECINSTR ) {
        strcat( name, " EXEC_INSTR |" );
    }
    if( name[strlen(name)-1] == '|' ) {
        name[strlen(name)-1] = 0;
    }
    Wdputs( "flags = " );
    Wdputs( name );
    Wdputslc( "\n" );
}

/*
 * byte swap ELF section header
 */
static void swap_shdr( Elf32_Shdr *elf_sec )
/******************************************/
{
    if( Byte_swap ) {
        SWAP_32( elf_sec->sh_name );
        SWAP_32( elf_sec->sh_type );
        SWAP_32( elf_sec->sh_flags );
        SWAP_32( elf_sec->sh_addr );
        SWAP_32( elf_sec->sh_offset );
        SWAP_32( elf_sec->sh_size );
        SWAP_32( elf_sec->sh_link );
        SWAP_32( elf_sec->sh_info );
        SWAP_32( elf_sec->sh_addralign );
        SWAP_32( elf_sec->sh_entsize );
    }
}

/*
 * byte swap ELF program header
 */
static void swap_phdr( Elf32_Phdr *elf_prog )
/*******************************************/
{
    if( Byte_swap ) {
        /* Byte swap program header */
        SWAP_32( elf_prog->p_type );
        SWAP_32( elf_prog->p_offset );
        SWAP_32( elf_prog->p_vaddr );
        SWAP_32( elf_prog->p_paddr );
        SWAP_32( elf_prog->p_filesz );
        SWAP_32( elf_prog->p_memsz );
        SWAP_32( elf_prog->p_flags );
        SWAP_32( elf_prog->p_align );
    }
}

static void set_dwarf( unsigned_32 start )
/****************************************/
{
    Elf32_Shdr      elf_sec;
    unsigned_32     offset;
    char            *string_table;
    int             i;
    uint            sect;
    unsigned_32     sectsizes[DR_DEBUG_NUM_SECTS];
    unsigned_32     sections[DR_DEBUG_NUM_SECTS];

    // grab the string table, if it exists
    if( !Elf_head.e_shstrndx ) {
        return; // no strings no dwarf
    }
    if( Elf_head.e_shnum == 0 ) {
        return; // no sections no dwarf
    }
    memset( sections, 0, DR_DEBUG_NUM_SECTS * sizeof( unsigned_32 ) );
    memset( sectsizes, 0, DR_DEBUG_NUM_SECTS * sizeof( unsigned_32 ) );
    offset = Elf_head.e_shoff
           + Elf_head.e_shstrndx * Elf_head.e_shentsize+start;
    Wlseek( offset );
    Wread( &elf_sec, sizeof( Elf32_Shdr ) );
    swap_shdr( &elf_sec );
    string_table = Wmalloc( elf_sec.sh_size );
    Wlseek( elf_sec.sh_offset + start );
    Wread( string_table, elf_sec.sh_size );
    for( i = 0; i < Elf_head.e_shnum; i++ ) {
        Wlseek( Elf_head.e_shoff + i * Elf_head.e_shentsize + start );
        Wread( &elf_sec, sizeof( Elf32_Shdr ) );
        swap_shdr( &elf_sec );
        if( elf_sec.sh_type == SHT_PROGBITS ) {
            sect = Lookup_section_name( &string_table[elf_sec.sh_name] );
            if ( sect < DW_DEBUG_MAX ) {
                sections[sect] = elf_sec.sh_offset + start;
                sectsizes[sect] = elf_sec.sh_size;
            }
        }
    }
    free( string_table );
    for( i = 0; i < DR_DEBUG_NUM_SECTS; i += 1 ) {
        Sections[i].cur_offset = 0;
        Sections[i].max_offset = sectsizes[i];

        if( sectsizes[i] != 0 ) {
            Wlseek( sections[i] );
            Sections[i].data = Wmalloc( sectsizes[i] );
            if( Sections[i].data == NULL ) {
                Wdputslc( "Not enough memory\n" );
                exit( 1 );
            }
            Wread( Sections[i].data, sectsizes[i] );
        }
    }
}

/*
 * Dump the section data.
 */
static void dmp_sec_data( char *name,
    unsigned_32 type, unsigned_32 offset, unsigned_32 size )
/**********************************************************/
{
    if( size == 0 ) {
        return;
    }
    switch( type ) {
    case SHT_NULL:
        Dmp_seg_data( offset, size );
        break;
    case SHT_PROGBITS:
        dmp_sec_progbits( name, offset, size );
        break;
    case SHT_SYMTAB:
        Dmp_seg_data( offset, size );
        break;
    case SHT_STRTAB:
        dmp_sec_strtab( offset, size );
        break;
    case SHT_RELA:
        Dmp_seg_data( offset, size );
        break;
    case SHT_HASH:
        Dmp_seg_data( offset, size );
        break;
    case SHT_DYNAMIC:
        Dmp_seg_data( offset, size );
        break;
    case SHT_NOTE:
        dmp_sec_note( offset, size );
        break;
    case SHT_NOBITS:
        Dmp_seg_data( offset, size );
        break;
    case SHT_REL:
        Dmp_seg_data( offset, size );
        break;
    case SHT_SHLIB:
        Dmp_seg_data( offset, size );
        break;
    case SHT_DYNSYM:
        Dmp_seg_data( offset, size );
        break;
    case SHT_LOPROC:
        Dmp_seg_data( offset, size );
        break;
    }
}

/*
 * Dump the program and section headers.
 */
static void dmp_prog_sec( unsigned_32 start )
/*******************************************/
{
    Elf32_Phdr      elf_prog;
    Elf32_Shdr      elf_sec;
    unsigned_32     offset;
    char            *string_table;
    int             i;

    // grab the string table, if it exists
    if( Options_dmp & DEBUG_INFO ) {
        set_dwarf( start );
    }
    if( Elf_head.e_shstrndx ) {
        offset = Elf_head.e_shoff
               + Elf_head.e_shstrndx * Elf_head.e_shentsize+start;
        Wlseek( offset );
        Wread( &elf_sec, sizeof( Elf32_Shdr ) );
        swap_shdr( &elf_sec );
        string_table = Wmalloc( elf_sec.sh_size );
        Wlseek( elf_sec.sh_offset + start );
        Wread( string_table, elf_sec.sh_size );
    } else {
        string_table = 0;
    }
    if( Elf_head.e_phnum ) {
        Banner( "ELF Program Header" );
        offset = Elf_head.e_phoff + start;
        for( i = 0; i < Elf_head.e_phnum; i++ ) {
            Wdputs( "                Program Header #" );
            Putdec( i + 1 );
            Wdputslc( "\n" );
            if( start != 0 ) {
                Wdputs("File Offset:");
                Puthex( offset, 8 );
                Wdputslc( "\n");
            }
            Wlseek( offset );
            Wread( &elf_prog, sizeof( Elf32_Phdr ) );
            swap_phdr( &elf_prog );
//          elf_prog.p_offset += start; //Relocate file pos
            offset += sizeof( Elf32_Phdr );
            Data_count++;
            dmp_prog_type( elf_prog.p_type );
            Dump_header( &elf_prog, elf_prog_msg );
            dmp_prog_flgs( elf_prog.p_flags );
            if( Options_dmp & (DOS_SEG_DMP | OS2_SEG_DMP) ) {
                if( Segspec == 0 || Segspec == Data_count ) {
                    Dmp_seg_data( elf_prog.p_offset + start, elf_prog.p_filesz );
                }
            } else if( elf_prog.p_type == PT_NOTE ) {
                dmp_sec_note( elf_prog.p_offset + start, elf_prog.p_filesz );
            }
            Wdputslc( "\n" );
        }
    }
    if( Elf_head.e_shnum ) {
        Banner( "ELF Section Header" );
        offset = Elf_head.e_shoff+start;
        for( i = 0; i < Elf_head.e_shnum; i++ ) {
            Wlseek( offset );
            Wread( &elf_sec, sizeof( Elf32_Shdr ) );
            swap_shdr( &elf_sec );
//          elf_sec.sh_offset += start;  // relocate file pos
            Wdputs( "             Section Header #" );
            Putdec( i );
            if( string_table ) {
                Wdputs( " \"" );
                Wdputs( &(string_table[elf_sec.sh_name]) );
                Wdputs( "\"" );
            }
            Wdputslc( "\n" );
            if( start != 0 ) {
                Wdputs( "File Offset:" );
                Puthex( offset, 8 );
                Wdputslc( "\n" );
            }
            dmp_sec_type( elf_sec.sh_type );
            Dump_header( &elf_sec.sh_name, elf_sec_msg );
            dmp_sec_flgs( elf_sec.sh_flags );
            if( Options_dmp & FIX_DMP ) {
                if( elf_sec.sh_type==SHT_REL || elf_sec.sh_type==SHT_RELA ) {
                    Elf32_Shdr      rel_sec;
                    Elf32_Rela      elf_rela;
                    int             loc, ctr, rel_size;

                    Wdputs( "relocation information for section #" );
                    Putdec( elf_sec.sh_info );
                    Wlseek( Elf_head.e_shoff + start +
                            Elf_head.e_shentsize * elf_sec.sh_info );
                    Wread( &rel_sec, sizeof( Elf32_Shdr ) );
                    swap_shdr( &rel_sec );
                    if( string_table ) {
                        Wdputs( " \"" );
                        Wdputs( &string_table[rel_sec.sh_name] );
                        Wdputs( "\"" );
                    } else {
                        Wdputs( " no_name (no associated string table)" );
                    }
                    Wdputslc( ":\n" );
                    Wdputs( "symbol index refers to section #" );
                    Putdec( elf_sec.sh_link );
                    Wdputslc( "\n" );
                    Wdputslc( "Offset   Sym Idx  Addend   Type      Offset   Sym Idx  Addend   Type\n" );
                    rel_size = (elf_sec.sh_type == SHT_REL ? sizeof( Elf32_Rel ) : sizeof( Elf32_Rela ));
                    for( loc = 0, ctr = 0; loc < elf_sec.sh_size; loc += rel_size, ctr++ ) {
                        Wlseek( elf_sec.sh_offset + start + loc );
                        Wread( &elf_rela, rel_size );
                        Puthex( elf_rela.r_offset, 8 );
                        Wdputc( ' ' );
                        Puthex( ELF32_R_SYM( elf_rela.r_info ), 8 );
                        Wdputc( ' ' );
                        if( elf_sec.sh_type == SHT_RELA ) {
                            Puthex( elf_rela.r_addend, 8 );
                        } else {
                            Wdputs( "n/a     " );
                        }
                        Wdputc( ' ' );
                        Puthex( ELF32_R_TYPE( elf_rela.r_info ), 2 );
                        if( ctr % 2 == 1 ) {
                            Wdputslc( "\n" );
                        } else {
                            Wdputs( "        " );
                        }
                    }
                    if( ctr % 2 != 0 ) {
                        Wdputslc( "\n" );
                    }
                }
            }
            if( Options_dmp & DEBUG_INFO ) {
                Wdputslc( "\n" );
                if( string_table ) {
                    dmp_sec_data( &(string_table[elf_sec.sh_name]),
                                  elf_sec.sh_type,
                                  elf_sec.sh_offset+start,
                                  elf_sec.sh_size );
                } else {
                    dmp_sec_data( NULL,
                                  elf_sec.sh_type,
                                  elf_sec.sh_offset+start,
                                  elf_sec.sh_size );
                }
            } else if( Options_dmp & OS2_SEG_DMP ) {
                if( elf_sec.sh_size && elf_sec.sh_type != SHT_NOBITS ) {
                    Wdputslc( "Section dump:\n" );
                    Dmp_seg_data( elf_sec.sh_offset + start, elf_sec.sh_size );
                }
            }
            Wdputslc( "\n" );
            offset +=  Elf_head.e_shentsize;
        }
    }
    if( string_table ) {
        free( string_table );
    }
}

bool Dmp_lib_head( void )
/***********************/
{
    char                sig[LIBMAG_LEN];
    Lib32_Hdr           hdr;
    unsigned long       filesize;
    unsigned long       size;
    unsigned long       Elf_off;

    Wlseek( 0 );
    Wread( sig, LIBMAG_LEN );
    if( memcmp( sig, LIBMAG, LIBMAG_LEN ) != 0 ) return 0;
    filesize = WFileSize();
    Elf_off = LIBMAG_LEN + LIB_CLASS_LEN + LIB_DATA_LEN;
    Wlseek( Elf_off );
    for( ;; ) {
        if( Elf_off + LIB_HEADER_SIZE >= filesize ) break;
        Wread( &hdr, LIB_HEADER_SIZE );
        Elf_off += LIB_HEADER_SIZE;
        hdr.lib_date[0]='\0';
        Wdputs( "lib name = " );
        Wdputs( hdr.lib_name );
        Wdputslc( "\n" );
        hdr.lib_fmag[0] = '\0';
        size = strtoul( hdr.lib_size, NULL, 10 );
        if( !Dmp_elf_header( Elf_off ) ) {
            if( strcmp( hdr.lib_name, LIB_SYMTAB_NAME ) &&
                strcmp( hdr.lib_name, LIB_LFTAB_NAME ) &&
                strcmp( hdr.lib_name, LIB_FFTAB_NAME ) ) {
                Wdputslc( "archive entry not identified\n" );
            }
            Dmp_seg_data( Elf_off, size );
            Wdputslc( "\n" );
        }
        if( size & 1 ) {
            size++;
        }
        Elf_off += size;
        Wlseek( Elf_off );
    }
    return 1;
}

/*
 * Dump the ELF header, if any.
 */
bool Dmp_elf_header( unsigned long start )
/****************************************/
{
    Wread( &Elf_head, sizeof( Elf32_Ehdr ) );
    if( memcmp( Elf_head.e_ident, ELF_SIGNATURE, ELF_SIGNATURE_LEN ) ) {
        return( 0 );
    }
    Banner( "ELF Header" );
    if( start != 0 ) {
        Wdputs( "File Offset:" );
        Puthex( start, 8 );
        Wdputslc( "\n" );
    }
    Wdputs( "class (1==32-bit objects, 2==64-bit objs)   =       " );
    Puthex( Elf_head.e_ident[EI_CLASS], 2 );
    Wdputslc( "H\ndata  (1==little-endian, 2==big-endian)     =       " );
    Puthex( Elf_head.e_ident[EI_DATA], 2 );
    Wdputslc( "H\nversion                                     =       " );
    Puthex( Elf_head.e_ident[EI_VERSION], 2 );
    Wdputslc( "H\nOS/ABI type (0==unspecified)                =       " );
    Puthex( Elf_head.e_ident[EI_OSABI], 2 );
    Wdputslc( "H\nABI version (0==unspecified)                =       " );
    Puthex( Elf_head.e_ident[EI_ABIVERSION], 2 );
    Wdputslc( "H\n" );
    if( Elf_head.e_ident[EI_DATA] != NATIVE_ENDIAN ) {
        Byte_swap = true;

        /* Byte swap ELF header */
        SWAP_16( Elf_head.e_type );
        SWAP_16( Elf_head.e_machine );
        SWAP_32( Elf_head.e_version );
        SWAP_32( Elf_head.e_entry );
        SWAP_32( Elf_head.e_phoff );
        SWAP_32( Elf_head.e_shoff );
        SWAP_32( Elf_head.e_flags );
        SWAP_16( Elf_head.e_ehsize );
        SWAP_16( Elf_head.e_phentsize );
        SWAP_16( Elf_head.e_phnum );
        SWAP_16( Elf_head.e_shentsize );
        SWAP_16( Elf_head.e_shnum );
        SWAP_16( Elf_head.e_shstrndx );
    }
    dmp_hdr_type( Elf_head.e_type );
    Dump_header( &Elf_head.e_type, elf_exe_msg );
    Wdputslc( "\n" );
    dmp_prog_sec( start );
    return( 1 );
}


/*
 * Dump the ELF header, if any.
 */
bool Dmp_elf_head( void )
/***********************/
{
    Wlseek( 0 );
    return( Dmp_elf_header( 0 ) );
}
