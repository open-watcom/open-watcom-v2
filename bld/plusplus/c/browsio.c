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
* Description:  Browse database file I/O.
*
****************************************************************************/


#include "plusplus.h"

#include <stdio.h>
#include <setjmp.h>
#include <stdarg.h>
#include <errno.h>

#include "preproc.h"
#include "memmgr.h"
#include "srcfile.h"
#include "dwio.h"
#include "iosupp.h"
#include "cgsegid.h"
#include "hfile.h"
#include "dw.h"
#include "exeelf.h"

#if defined(__UNIX__)
 #include <unistd.h>
#else
 #include <direct.h>
#endif


typedef struct cppdw_section {
    DWFILE          *file;
    unsigned long   offset;
    unsigned long   length;
} CPPDW_SECTION;
static CPPDW_SECTION dw_sections[DW_DEBUG_MAX];

// -- code to generate ELF output ------------------------------------------
//
// note: the pre-initialized fields in these structures assume the following
//       layout in the file
//              elf_header
//              string_table
//              .debug_abbrev
//              .debug_info
//              .WATCOM_references
//              .debug_line
//              .debug_macinfo
//              section_header_index0
//              section_header_string_table
//              section_header_template( .debug_abbrev )
//              section_header_template( .debug_info )
//              section_header_template( .WATCOM_references )
//              section_header_template( .debug_line )
//              section_header_template( .debug_macinfo )
//

static Elf32_Ehdr elf_header = {
    { ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3,
      ELFCLASS32, ELFDATA2LSB, EV_CURRENT },
    ET_DYN,
    EM_386,
    EV_CURRENT,
    0,
    0,
    0,//<offset of section table>=sizeof(Elf32_Ehdr)+sizeof(string_table)+sizes of 5 sections
    0,
    sizeof( Elf32_Ehdr ),
    sizeof( Elf32_Phdr ),
    0,
    sizeof( Elf32_Shdr ),
    7,
    1
};

#define STR_NAME0           "\0"
#define STR_SHSTRTAB        ".shstrtab\0"
#define STR_DBG_ABBREV      ".debug_abbrev\0"
#define STR_DBG_INFO        ".debug_info\0"
#define STR_DBG_REF         ".WATCOM_references\0"
#define STR_DBG_LINE        ".debug_line\0"
#define STR_DBG_MACINFO     ".debug_macinfo\0"
#define OFF_STR_NAME0       (0)
#define OFF_STR_SHSTRTAB    (OFF_STR_NAME0 + sizeof( STR_NAME0 ) - 1)
#define OFF_STR_DBG_ABBREV  (OFF_STR_SHSTRTAB + sizeof( STR_SHSTRTAB ) - 1)
#define OFF_STR_DBG_INFO    (OFF_STR_DBG_ABBREV + sizeof( STR_DBG_ABBREV ) - 1)
#define OFF_STR_DBG_REF     (OFF_STR_DBG_INFO + sizeof( STR_DBG_INFO ) - 1)
#define OFF_STR_DBG_LINE    (OFF_STR_DBG_REF + sizeof( STR_DBG_REF ) - 1)
#define OFF_STR_DBG_MACINFO (OFF_STR_DBG_LINE + sizeof( STR_DBG_LINE ) - 1)
#define OFF_STR_MAX         (OFF_STR_DBG_MACINFO + sizeof( STR_DBG_MACINFO ) - 1)
static char const string_table[OFF_STR_MAX + 1] = {
    STR_NAME0
    STR_SHSTRTAB
    STR_DBG_ABBREV
    STR_DBG_INFO
    STR_DBG_REF
    STR_DBG_LINE
    STR_DBG_MACINFO
};
static unsigned const string_table_offsets[] = {
    OFF_STR_DBG_ABBREV,
    OFF_STR_DBG_INFO,
    OFF_STR_DBG_REF,
    OFF_STR_DBG_LINE,
    OFF_STR_DBG_MACINFO
};

static Elf32_Shdr const section_header_index0 = {
    0, SHT_NULL, 0, 0, 0, 0, SHN_UNDEF, 0, 0, 0
};

static Elf32_Shdr const section_header_string_table = {
    OFF_STR_SHSTRTAB,
    SHT_STRTAB,
    0,
    0,
    sizeof( Elf32_Ehdr ),
    sizeof( string_table ),
    SHN_UNDEF,
    0,
    0,
    0
};

static Elf32_Shdr section_header_template = {
    0,//<index of name in string section>
    SHT_PROGBITS,
    0,
    0,
    0,//<offset of section in file>
    0,//<size of section>
    SHN_UNDEF,
    0,
    0,
    0
};

static void mywrite( FILE *fp, void *data, size_t len ) {
    size_t wroteSize;
    wroteSize = fwrite( data, 1, len, fp );
    if (wroteSize < len) {
        puts( strerror( errno ) );
        CFatal( "error on write" );
    }
}

static int createBrowseFile(FILE* browseFile,       /* target file */
                     CPPDW_SECTION* abbrevFile,     /* .debug_abbrev section */
                     CPPDW_SECTION* debugFile,      /* .debug_info section */
                     CPPDW_SECTION* referenceFile,  /* .WATCOM_reference section */
                     CPPDW_SECTION* lineFile,       /* .debug_line section */
                     CPPDW_SECTION* macroFile       /* .debug_macinfo section */
                    )
{
    char *ptr;
    size_t readSize;
    int fileNum;
    CPPDW_SECTION* inFile[5];
    unsigned long sectionSize;
    unsigned long sectionOffset[5];

    // write elf header
    elf_header.e_shoff = sizeof( Elf32_Ehdr )
                        + sizeof( string_table )
                        + abbrevFile->length
                        + debugFile->length
                        + referenceFile->length
                        + lineFile->length
                        + macroFile->length;
    mywrite( browseFile, (void *)&elf_header, sizeof( elf_header ) );

    // write string table
    mywrite( browseFile, (void *)string_table, sizeof( string_table ) );

    // write each of the 5 sections, tracking offset
    inFile[0] = abbrevFile;
    inFile[1] = debugFile;
    inFile[2] = referenceFile;
    inFile[3] = lineFile;
    inFile[4] = macroFile;
    sectionOffset[0] = sizeof( elf_header ) + sizeof( string_table );
    sectionOffset[1] = sectionOffset[0] + inFile[0]->length;
    sectionOffset[2] = sectionOffset[1] + inFile[1]->length;
    sectionOffset[3] = sectionOffset[2] + inFile[2]->length;
    sectionOffset[4] = sectionOffset[3] + inFile[3]->length;

    for (fileNum=0;fileNum<5;fileNum++) {
        DwioOpenInput( inFile[fileNum]->file );

        readSize = 0;
        sectionSize = inFile[fileNum]->length;
        while( sectionSize ) {
            ptr = DwioRead( inFile[fileNum]->file, &readSize );
            readSize = min( readSize, sectionSize );
            sectionSize -= readSize;
            mywrite( browseFile, (void *)ptr, readSize );
        }
        DwioCloseInputFile( inFile[fileNum]->file );
    }

    // write section_header_index0
    mywrite( browseFile, (void *)&section_header_index0, sizeof( section_header_index0 ) );

    // write section_header_string_table
    mywrite( browseFile, (void *)&section_header_string_table, sizeof( section_header_string_table ) );

    // write rest of section headers
    for (fileNum=0;fileNum<5;fileNum++) {
        section_header_template.sh_name = string_table_offsets[fileNum];
        section_header_template.sh_offset = sectionOffset[fileNum];
        section_header_template.sh_size = inFile[fileNum]->length;
        mywrite( browseFile, (void *)&section_header_template, sizeof( section_header_template ) );
    }
    return 0;
}
//---------------------------------------------------------------------------

static void dw_write( dw_sectnum section, const void *block, dw_size_t len )
/**************************************************************************/
{
#ifdef __DD__
    //int i;
    printf( "\nDW_WRITE(%d:%d): offset: %d len: %d ",
        section,
        dw_sections[section].length,
        dw_sections[section].offset,
        len );
    //for( i = 0 ; i < len; i++ ) {
    //    printf( "%02x ", (int)((char *)block)[i] );
    //}
#endif
    dw_sections[section].offset += len;
    if( dw_sections[section].offset > dw_sections[section].length ) {
        dw_sections[section].length = dw_sections[section].offset;
    }
    DwioWrite( dw_sections[section].file, (void *)block, len );
}

static long dw_tell( dw_sectnum section )
/***************************************/
{
#ifdef __DD__
    printf( "DW_TELL (%d:%d): %d\n", section,
        dw_sections[section].length,
        dw_sections[section].offset );
#endif
    return dw_sections[section].offset;
}

static void dw_reloc( dw_sectnum section, dw_relocs reloc_type, ... )
/*******************************************************************/
{
    va_list         args;
    dw_targ_addr    targ_data;
    dw_targ_seg     seg_data;
    uint_32         u32_data;
    uint            sect;
    SYMBOL          sym;

    va_start( args, reloc_type );
    switch( reloc_type ) {
    case DW_W_LABEL:
    case DW_W_DEFAULT_FUNCTION:
    case DW_W_ARANGE_ADDR:
    case DW_W_LOW_PC:
        u32_data = 0;   // NOTE: assumes little-endian byte order
        dw_write( section, &u32_data, TARGET_NEAR_POINTER );
        break;
    case DW_W_HIGH_PC:
        u32_data = 1;   // NOTE: assumes little-endian byte order
        dw_write( section, &u32_data, TARGET_NEAR_POINTER );
        break;
    case DW_W_UNIT_SIZE:
        u32_data = 1;
        dw_write( section, &u32_data, sizeof( u32_data ) );
        break;
    case DW_W_STATIC:
        sym = va_arg( args, SYMBOL );
        targ_data = 0;
        dw_write( section, &targ_data, sizeof( targ_data ) );
        break;
    case DW_W_SEGMENT:
        sym = va_arg( args, SYMBOL );
        seg_data = CgSegId( sym );
        dw_write( section, &seg_data, sizeof( seg_data ) );
        break;
    case DW_W_SECTION_POS:
        sect = va_arg( args, uint );
        u32_data = dw_tell( sect );
        dw_write( section, &u32_data, sizeof( u32_data ) );
        break;
    }
    va_end( args );
}

static void dw_seek( dw_sectnum section, long offset, uint mode )
/***************************************************************/
{
    switch( mode ) {
    case DW_SEEK_SET:
        break;
    case DW_SEEK_CUR:
        offset = dw_sections[section].offset + offset;
        break;
    case DW_SEEK_END:
        offset = dw_sections[section].length - offset;
        break;
    }
#ifdef __DD__
    printf( "DW_SEEK (%d:%d): offset: %d\n",
        section,
        dw_sections[section].length,
        offset );
#endif
    if( dw_sections[section].offset != offset ) {
        DwioSeek( dw_sections[section].file, offset );
        dw_sections[section].offset = offset;
        if( dw_sections[section].offset > dw_sections[section].length ) {
            dw_sections[section].length = dw_sections[section].offset;
        }
    }
}

static void *dw_alloc( size_t size )
/**********************************/
{
    return CMemAlloc( size );
}

static void dw_free( void *ptr )
/******************************/
{
    CMemFree( ptr );
}

extern dw_client DwarfInit( void )
/********************************/
{
    dw_init_info    info;
    dw_cu_info      cu;
    char            dir[_MAX_PATH2];
    char            fname[_MAX_PATH];
    char            *full_fname;
    int             i;
    char            *incbuf = NULL;
    char            *inccurr;
    unsigned        incsize;
    dw_client       client;

    DwioInit();
    for( i = 0 ; i < DW_DEBUG_MAX ; i++ ) {
        dw_sections[i].file = DwioCreateFile();
        dw_sections[i].offset = 0;
        dw_sections[i].length = 0;
    }
    HFileListStart();
    incsize = HFileListSize();
    if( incsize != 0 ) {
        incbuf = CMemAlloc( incsize );
        inccurr = incbuf;
        for(;;) {
            HFileListNext( inccurr );
            if( *inccurr == '\0' ) break;
            inccurr = strend( inccurr ) + 1;
        }
        incsize = inccurr - incbuf;
    }
    info.language = DWLANG_CPP;
    info.compiler_options = DW_CM_BROWSER;
    info.producer_name = "WATCOM C++ V1";
    memcpy( info.exception_handler, Environment, sizeof( jmp_buf ) );
    info.funcs.reloc = &dw_reloc;
    info.funcs.write = &dw_write;
    info.funcs.seek = &dw_seek;
    info.funcs.tell = &dw_tell;
    info.funcs.alloc = &dw_alloc;
    info.funcs.free = &dw_free;

    client = DWInit( &info );
    if( client == NULL ) {
        CFatal( "dwarf: error in DWInit()" );
    }
    getcwd( dir, sizeof( dir ) ),
    full_fname = IoSuppFullPath( WholeFName, fname, sizeof( fname ) );
    cu.source_filename = full_fname;
    cu.directory       = dir;
    cu.flags           = 1;
    cu.offset_size     = TARGET_NEAR_POINTER;
    cu.segment_size    = 0;
    cu.model           = DW_MODEL_NONE;
    cu.inc_list        = incbuf;
    cu.inc_list_len    = incsize;
    cu.dbg_pch         = 0;


    DWBeginCompileUnit( client, &cu );
    if( incsize != 0 ) {
        CMemFree( incbuf );
    }
    return( client );
}

extern void DwarfFini( dw_client  client )
/****************************************/
{
    int     i;
    int     status;
    char    *out_fname;
    FILE    *out_file;

    if( !CompFlags.emit_browser_info ) return;

    DWEndCompileUnit( client );
    DWFini( client );

    // close after writing
    for( i = 0 ; i < DW_DEBUG_MAX ; i++ ) {
        DwioCloseOutputFile( dw_sections[i].file );
    }

    out_fname = IoSuppOutFileName( OFT_MBR );
    out_file = SrcFileFOpen( out_fname, SFO_WRITE_BINARY );
    if( out_file == NULL ) {
        puts( strerror( errno ) );
        puts( out_fname );
        CFatal( "dwarf: unable to open file for writing" );
    }

    // concatenate files
    if( createBrowseFile( out_file,
                          &dw_sections[DW_DEBUG_ABBREV],
                          &dw_sections[DW_DEBUG_INFO],
                          &dw_sections[DW_DEBUG_REF],
                          &dw_sections[DW_DEBUG_LINE],
                          &dw_sections[DW_DEBUG_MACINFO] ) ) {
        puts( strerror( errno ) );
        CFatal( "dwarf: error in merging browse files" );
    }

    status = SrcFileFClose( out_file );
    if( status ) {
        puts( strerror( errno ) );
        puts( out_fname );
        CFatal( "dwarf: unable to close file" );
    }

    // delete
    for( i = 0 ; i < DW_DEBUG_MAX ; i++ ) {
        DwioFreeFile( dw_sections[i].file );
    }
    DwioFini();
}
