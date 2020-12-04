/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#include "cvars.h"
#include <stdarg.h>
#include "wio.h"
#include "exeelf.h"
#include "browsio.h"
#include "dwarfid.h"


#define C_DWARF_BUFSIZE 4096

typedef struct c_dw_section {
    unsigned        bufcount;
    char            **bufptrs;
    dw_out_offset   offset;
    dw_out_offset   length;
} C_DW_SECTION;

static uint_32      relocValues[DW_W_MAX];

static C_DW_SECTION dw_sections[DW_DEBUG_MAX];

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
    { ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3, ELFCLASS32, ELFDATA2LSB, EV_CURRENT },
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

static char string_table[OFF_STR_MAX + 1] = {
    STR_NAME0
    STR_SHSTRTAB
    STR_DBG_ABBREV
    STR_DBG_INFO
    STR_DBG_REF
    STR_DBG_LINE
    STR_DBG_MACINFO
};

static unsigned string_table_offsets[] = {
    OFF_STR_DBG_ABBREV,
    OFF_STR_DBG_INFO,
    OFF_STR_DBG_REF,
    OFF_STR_DBG_LINE,
    OFF_STR_DBG_MACINFO
};

static Elf32_Shdr section_header_index0 = {
    0, SHT_NULL, 0, 0, 0, 0, SHN_UNDEF, 0, 0, 0
};

static Elf32_Shdr section_header_string_table = {
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

static void CFatal( const char *msg )
{
    printf( "%s\n", msg );
}

static void mywrite( FILE *fp, const void *data, size_t len )
{
    size_t wroteSize;

    wroteSize = fwrite( data, 1, len, fp );
    if( wroteSize < len ) {
        puts( strerror( errno ) );
        CFatal( "error on write" );
    }
}

/* output DWARF sections in following order
 * .debug_abbrev
 * .debug_info
 * .WATCOM_reference
 * .debug_line
 * .debug_macinfo
 */
static const dw_sectnum inSect[] = {
    DW_DEBUG_ABBREV,
    DW_DEBUG_INFO,
    DW_DEBUG_REF,
    DW_DEBUG_LINE,
    DW_DEBUG_MACINFO
};

#define SECTION_COUNT   ARRAY_SIZE( inSect )

static int createBrowseFile( FILE *browseFile )
{
    char            *ptr;
    size_t          bufsize;
    int             fileNum;
    unsigned        bufnum;
    dw_out_offset   sectionSize;
    dw_out_offset   sectionOffset[SECTION_COUNT];

    // calculate sections data size
    elf_header.e_shoff = sizeof( Elf32_Ehdr ) + sizeof( string_table );
    for( fileNum = 0; fileNum < SECTION_COUNT; fileNum++ ) {
        elf_header.e_shoff += dw_sections[inSect[fileNum]].length;
    }

    // write elf header
    mywrite( browseFile, &elf_header, sizeof( elf_header ) );

    // write string table
    mywrite( browseFile, string_table, sizeof( string_table ) );

    // calculate each of the sections, tracking offset
    // write each of the sections
    sectionOffset[0] = sizeof( elf_header ) + sizeof( string_table );
    for( fileNum = 0; fileNum < SECTION_COUNT; fileNum++ ) {
        if( fileNum > 0 )
            sectionOffset[fileNum] = sectionOffset[fileNum - 1] + dw_sections[inSect[fileNum - 1]].length;
        bufsize = C_DWARF_BUFSIZE;
        bufnum = 0;
        for( sectionSize = dw_sections[inSect[fileNum]].length; sectionSize > 0; sectionSize -= bufsize ) {
            if( bufsize > sectionSize )
                bufsize = sectionSize;
            ptr = dw_sections[inSect[fileNum]].bufptrs[bufnum];
            mywrite( browseFile, ptr, bufsize );
            CMemFree( ptr );
            ++bufnum;
        }
        if( dw_sections[inSect[fileNum]].bufptrs != NULL ) {
            CMemFree( dw_sections[inSect[fileNum]].bufptrs );
        }
    }

    // write section_header_index0
    mywrite( browseFile, &section_header_index0, sizeof( section_header_index0 ) );

    // write section_header_string_table
    mywrite( browseFile, &section_header_string_table, sizeof( section_header_string_table ) );

    // write rest of section headers
    for( fileNum = 0; fileNum < SECTION_COUNT; fileNum++ ) {
        section_header_template.sh_name     = string_table_offsets[fileNum];
        section_header_template.sh_offset   = sectionOffset[fileNum];
        section_header_template.sh_size     = dw_sections[inSect[fileNum]].length;
        mywrite( browseFile, &section_header_template, sizeof( section_header_template ) );
    }
    return( 0 );
}
//---------------------------------------------------------------------------

static void dw_write( dw_sectnum sect, const void *block, size_t len )
/********************************************************************/
{
    unsigned            bufnum;
    unsigned            endbufnum;
    char                **newbufptrs;
    size_t              bufsize;
    char                *bufptr;

#ifdef __DD__
    //int i;
    printf( "\nDW_WRITE(%d:%lu): offset: %lu len: %u ", sect,
        (unsigned long)dw_sections[sect].length,
        (unsigned long)dw_sections[sect].offset,
        (unsigned)len );
    //for( i = 0 ; i < len; i++ ) {
    //    printf( "%02x ", (int)((char *)block)[i] );
    //}
#endif
    bufnum = dw_sections[sect].offset / C_DWARF_BUFSIZE;
    endbufnum = ( dw_sections[sect].offset + len ) / C_DWARF_BUFSIZE;
    if( endbufnum >= dw_sections[sect].bufcount ) {
        newbufptrs = (char **)CMemAlloc( ( endbufnum + 1 ) * sizeof( char * ) );
        if( dw_sections[sect].bufptrs != NULL ) {
            memcpy( newbufptrs, dw_sections[sect].bufptrs, dw_sections[sect].bufcount * sizeof( char * ) );
            CMemFree( dw_sections[sect].bufptrs );
        }
        dw_sections[sect].bufptrs = newbufptrs;
        dw_sections[sect].bufcount = endbufnum + 1;
    }
    bufsize = C_DWARF_BUFSIZE - ( dw_sections[sect].offset % C_DWARF_BUFSIZE );
    dw_sections[sect].offset += len;
    if( dw_sections[sect].length < dw_sections[sect].offset ) {
        dw_sections[sect].length = dw_sections[sect].offset;
    }
    while( len != 0 ) {
        bufptr = dw_sections[sect].bufptrs[bufnum];
        if( bufptr == NULL ) {
            bufptr = (char *)CMemAlloc( C_DWARF_BUFSIZE );
            dw_sections[sect].bufptrs[bufnum] = bufptr;
        }
        bufptr += C_DWARF_BUFSIZE - bufsize;
        if( bufsize > len  )
            bufsize = len;
        memcpy( bufptr, block, bufsize );
        block = (char *)block + bufsize;
        len -= bufsize;
        ++bufnum;                       // advance to next buffer
        bufsize = C_DWARF_BUFSIZE;      // buffer is full size
    }
}

static dw_out_offset dw_tell( dw_sectnum sect )
/*********************************************/
{
#ifdef __DD__
    printf( "DW_TELL (%d:%lu): %lu\n", sect,
        (unsigned long)dw_sections[sect].length,
        (unsigned long)dw_sections[sect].offset );
#endif
    return( dw_sections[sect].offset );
}

static void dw_reloc( dw_sectnum sect, dw_reloc_type reloc_type, ... )
/********************************************************************/
{
    va_list         args;
    dw_targ_addr    targ_data;
    dw_targ_seg     seg_data;
    uint_32         u32_data;
    dw_sectnum      sect_no;
    SYMPTR          sym;

    va_start( args, reloc_type );
    switch( reloc_type ) {
    case DW_W_LABEL:
    case DW_W_DEFAULT_FUNCTION:
    case DW_W_ARANGE_ADDR:
    case DW_W_LOW_PC:
        u32_data = 0;   // NOTE: assumes little-endian byte order
        dw_write( sect, &u32_data, TARGET_NEAR_POINTER );
        break;
    case DW_W_HIGH_PC:
        u32_data = 1;   // NOTE: assumes little-endian byte order
        dw_write( sect, &u32_data, TARGET_NEAR_POINTER );
        break;
    case DW_W_UNIT_SIZE:
        u32_data = 1;
        dw_write( sect, &u32_data, sizeof( u32_data ) );
        break;
    case DW_W_STATIC:
        sym = va_arg( args, SYMPTR );
        targ_data = 0;
        dw_write( sect, &targ_data, sizeof( targ_data ) );
        break;
    case DW_W_SEGMENT:
        sym = va_arg( args, SYMPTR );
        seg_data = SymSegId( sym );
        dw_write( sect, &seg_data, sizeof( seg_data ) );
        break;
    case DW_W_SECTION_POS:
        sect_no = va_arg( args, dw_sectnum );
        u32_data = dw_tell( sect_no );
        dw_write( sect, &u32_data, sizeof( u32_data ) );
        break;
    default:
        break;
    }
    va_end( args );
}

static void dw_seek( dw_sectnum sect, dw_out_offset offset, int mode )
/********************************************************************/
{
    dw_out_offset new_offset;

    new_offset = offset;
    switch( mode ) {
    case DW_SEEK_SET:
        break;
    case DW_SEEK_CUR:
        new_offset = dw_sections[sect].offset + offset;
        break;
    case DW_SEEK_END:
        new_offset = dw_sections[sect].length + offset;
        break;
    }
#ifdef __DD__
    printf( "DW_SEEK (%d:%lu): offset: %lu\n", sect,
        (unsigned long)dw_sections[sect].length,
        (unsigned long)new_offset );
#endif
    if( dw_sections[sect].offset != new_offset ) {
        dw_sections[sect].offset = new_offset;
        if( dw_sections[sect].length < dw_sections[sect].offset ) {
            dw_sections[sect].length = dw_sections[sect].offset;
        }
    }
}

static void *dw_alloc( size_t size )
/**********************************/
{
    char        *p;

    p = CMemAlloc( size );
    memset( p, 0xA5, size );
    return( p );
}

static void dw_free( void *ptr )
/******************************/
{
    CMemFree( ptr );
}

dw_client DwarfInit( void )
/*************************/
{
    dw_client       client;
    dw_init_info    info;
    dw_cu_info      cu;
    size_t          incsize;
    char            *inclist;
    char            *fname;
    dw_sectnum      sect;
    static const dw_funcs cli_funcs = {
        dw_reloc,
        dw_write,
        dw_seek,
        dw_tell,
        dw_alloc,
        dw_free
    };

    for( sect = 0; sect < DW_DEBUG_MAX; ++sect ) {
        dw_sections[sect].bufcount  = 0;
        dw_sections[sect].bufptrs   = NULL;
        dw_sections[sect].offset    = 0;
        dw_sections[sect].length    = 0;
    }
    info.language = DWLANG_C;
    info.compiler_options = DW_CM_BROWSER;
    info.producer_name = DWARF_PRODUCER_ID " V1";
    memcpy( &info.exception_handler, Environment, sizeof( jmp_buf ) );
    info.funcs = cli_funcs;
    info.abbrev_sym = NULL;

    relocValues[DW_W_LOW_PC] = 0x0;
    relocValues[DW_W_HIGH_PC] = 0x1;
    relocValues[DW_W_UNIT_SIZE] = 0x1;

    client = DWInit( &info );
    if( client == NULL ) {
        CFatal( "dwarf: error in DWInit()" );
    }
    fname = FNameFullPath( FNames );
    incsize = 0;
    inclist = NULL;
#if 0
    if( HFileList != NULL ) {
        char    *p;

        incsize = strlen( HFileList ) + 1;
        inclist = CMemAlloc( incsize );
        memcpy( inclist, HFileList, incsize );
        // need to handle the case where there are multiple ';' in a row
        for( p = inclist; *p != '\0'; p++ ) {
            if( *p == ';' ) {
                *p = '\0';
            }
        }
        if( inclist[incsize - 2] == '\0' ) {
            --incsize;
        }
    }
#endif
    cu.source_filename = fname;
    cu.directory       = "";
    cu.flags           = 1;
    cu.offset_size     = TARGET_NEAR_POINTER;
    cu.segment_size    = 0;
    cu.model           = DW_MODEL_NONE;
    cu.inc_list        = inclist;
    cu.inc_list_len    = incsize;
    cu.dbg_pch         = NULL;
    DWBeginCompileUnit( client, &cu );
    CMemFree( inclist );
    DWDeclFile( client, fname );
    return( client );
}

void DwarfFini( dw_client client )
/********************************/
{
    FILE    *out_file;

    DWEndCompileUnit( client );
    DWFini( client );

    out_file = OpenBrowseFile();
    if( out_file != NULL ) {
        // concatenate files
        createBrowseFile( out_file );
        fclose( out_file );
    }
}
