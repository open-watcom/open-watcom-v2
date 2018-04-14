/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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


#include "ftnstd.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "dw.h"
#include "dwarf.h"
#include "errcod.h"
#include "cioconst.h"
#include "omodes.h"
#include "exeelf.h"
#include "browscli.h"
#include "fmemmgr.h"
#include "ferror.h"
#include "cspawn.h"
#include "sdcio.h"


#define SWAP( x, y )    {x^=y^=x^=y;}

#define MEM_INCREMENT   10240

typedef struct f77_dw_section {
    sect_typ            sec_type;
    dw_out_offset       offset;
    dw_out_offset       length;
    union {
        file_handle     fp;
        size_t          size;
    } u1;
    union {
        char            *data;
        char            *filename;
    } u2;
    dw_sectnum          sec_number;
} F77_DW_SECTION;

static F77_DW_SECTION   dw_sections[DW_DEBUG_MAX];

static sect_typ         initial_section_type;

// -- code to generate ELF output ------------------------------------------
//
// note: the pre-initialized fields in these structures assume the following
//       layout in the file
//              elf_header
//              string_table
//              .debug_abbrev
//              .debug_info
//              .debug_ref
//              .debug_line
//              .debug_macinfo
//              section_header_index0
//              section_header_string_table
//              section_header_template( .debug_abbrev )
//              section_header_template( .debug_info )
//              section_header_template( .debug_ref )
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
#define OFF_NAME0           (0)
#define OFF_SHSTRTAB        (OFF_NAME0 + sizeof( STR_NAME0 ) - 1)
#define OFF_DBG_ABBREV      (OFF_SHSTRTAB + sizeof( STR_SHSTRTAB ) - 1)
#define OFF_DBG_INFO        (OFF_DBG_ABBREV + sizeof( STR_DBG_ABBREV ) - 1)
#define OFF_DBG_REF         (OFF_DBG_INFO + sizeof( STR_DBG_INFO ) - 1)
#define OFF_DBG_LINE        (OFF_DBG_REF + sizeof( STR_DBG_REF ) - 1)
#define OFF_DBG_MACINFO     (OFF_DBG_LINE + sizeof( STR_DBG_LINE ) - 1)
#define OFF_MAX             (OFF_DBG_MACINFO + sizeof( STR_DBG_MACINFO ) - 1)

static char string_table[OFF_MAX + 1] = {
    STR_NAME0
    STR_SHSTRTAB
    STR_DBG_ABBREV
    STR_DBG_INFO
    STR_DBG_REF
    STR_DBG_LINE
    STR_DBG_MACINFO
};

static unsigned string_table_offsets[] = {
    OFF_DBG_ABBREV,
    OFF_DBG_INFO,
    OFF_DBG_REF,
    OFF_DBG_LINE,
    OFF_DBG_MACINFO
};

static Elf32_Shdr section_header_index0 = {
    0, SHT_NULL, 0, 0, 0, 0, SHN_UNDEF, 0, 0, 0
};

static Elf32_Shdr section_header_string_table = {
    OFF_SHSTRTAB,
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

static void mywrite( file_handle fp, const void *data, size_t len, const char *filename )
/***************************************************************************************/
{
    char            err_msg[ERR_BUFF_SIZE+1];

    SDWrite( fp, data, len );
    if( SDError( fp, err_msg ) ) {
        Error( SM_IO_WRITE_ERR, filename, err_msg );
        CSuicide();
    }
}

static void chkIOErr( file_handle fp, int error, const char *filename )
/*********************************************************************/
{
    char            err_msg[ERR_BUFF_SIZE+1];

    if( SDError( fp, err_msg ) ) {
        Error( error, filename, err_msg );
        CSuicide();
    }
}

static size_t CLIRead( char *buf, size_t size, dw_sectnum sect )
/**************************************************************/
{
    if( dw_sections[sect].length - dw_sections[sect].offset < size )
        size = dw_sections[sect].length - dw_sections[sect].offset;
    if( size == 0 )
        return( 0 );
    if( ( dw_sections[sect].sec_type == FILE_SECTION ) && dw_sections[sect].u1.fp != NULL ) {
        SDRead( dw_sections[sect].u1.fp, buf, size );
        chkIOErr( dw_sections[sect].u1.fp, SM_IO_READ_ERR, "temporary file" );
    } else if( ( dw_sections[sect].sec_type == MEM_SECTION ) && dw_sections[sect].u2.data != NULL ) {
        memcpy( buf, dw_sections[sect].u2.data + dw_sections[sect].offset, size );
    } else {
        size = 0;
    }
    dw_sections[sect].offset += size;
    return( size );
}

/* output DWARF sections in following order
 * .debug_abbrev
 * .debug_info
 * .WATCOM_reference
 * .debug_line
 * .debug_macinfo
 */
static const dw_sectnum inSect[] = { DW_DEBUG_ABBREV, DW_DEBUG_INFO, DW_DEBUG_REF, DW_DEBUG_LINE, DW_DEBUG_MACINFO };
#define SECTION_COUNT   (sizeof( inSect ) / sizeof( inSect[0] ))

static int createBrowseFile( file_handle browseFile, const char *filename )
/*************************************************************************/
{
    size_t          readSize;
    int             fileNum;
    dw_out_offset   sectionSize;
    dw_out_offset   sectionOffset[SECTION_COUNT];

    // calculate sections data size
    elf_header.e_shoff = sizeof( Elf32_Ehdr ) + sizeof( string_table );
    for( fileNum = 0; fileNum < SECTION_COUNT; fileNum++ ) {
        elf_header.e_shoff += dw_sections[inSect[fileNum]].length;
    }

    // write elf header
    mywrite( browseFile, &elf_header, sizeof( elf_header ), filename );

    // write string table
    mywrite( browseFile, string_table, sizeof( string_table ), filename );

    // calculate each of the sections, tracking offset
    // write each of the sections
    sectionOffset[0] = sizeof( elf_header ) + sizeof( string_table );
    for( fileNum = 0; fileNum < SECTION_COUNT; fileNum++ ) {
        if( fileNum > 0 )
            sectionOffset[fileNum] = sectionOffset[fileNum - 1] + dw_sections[inSect[fileNum - 1]].length;
        // copy the section data
        readSize = TOKLEN;
        for( sectionSize = dw_sections[inSect[fileNum]].length; sectionSize > 0; sectionSize -= readSize ) {
            readSize = CLIRead( TokenBuff, readSize, dw_sections[inSect[fileNum]].sec_number);
            mywrite( browseFile, TokenBuff, readSize, filename );
        }
    }

    // write section_header_index0
    mywrite( browseFile, &section_header_index0, sizeof( section_header_index0 ), filename );

    // write section_header_string_table
    mywrite( browseFile, &section_header_string_table, sizeof( section_header_string_table ), filename );

    // write rest of section headers
    for( fileNum = 0; fileNum < SECTION_COUNT; fileNum++ ) {
        section_header_template.sh_name     = string_table_offsets[fileNum];
        section_header_template.sh_offset   = sectionOffset[fileNum];
        section_header_template.sh_size     = dw_sections[inSect[fileNum]].length;
        mywrite( browseFile, &section_header_template, sizeof( section_header_template ), filename );
    }
    return( 0 );
}


static void _SDWrite( file_handle fp, const void *buffer, size_t size )
{
    unsigned        amount;

    amount = INT_MAX;
    while( size > 0 ) {
        if( amount > size )
            amount = (unsigned)size;
        SDWrite( fp, buffer, amount );
        chkIOErr( fp, SM_IO_WRITE_ERR, "temporary file" );
        buffer = (char *)buffer + amount;
        size -= amount;
    }
}

static void CLIWrite( dw_sectnum sect, const void *block, size_t size )
/*********************************************************************/
{
    char            *temp;

    if( dw_sections[sect].sec_type == DEFAULT_SECTION ) {
        if( ( initial_section_type == DEFAULT_SECTION ) || ( initial_section_type == FILE_SECTION ) ) {
            dw_sections[sect].sec_type = FILE_SECTION;
            SDSetAttr( REC_FIXED | SEEK );
            temp = tmpnam( NULL );
            dw_sections[sect].u2.filename = FMemAlloc( strlen( temp ) + 1 );
            strcpy( dw_sections[sect].u2.filename, temp );
            dw_sections[sect].u1.fp = SDOpen( temp, UPDATE_FILE );
            chkIOErr( dw_sections[sect].u1.fp, SM_OPENING_FILE, temp );
        } else {
            dw_sections[sect].sec_type = initial_section_type;
            dw_sections[sect].u1.size = MEM_INCREMENT;
            dw_sections[sect].u2.data = FMemAlloc( MEM_INCREMENT );
        }
    }

    switch( dw_sections[sect].sec_type ) {
    case( MEM_SECTION ):
        if( dw_sections[sect].u1.size <= ( dw_sections[sect].offset + size ) ) {
            temp = FMemAlloc( dw_sections[sect].u1.size + MEM_INCREMENT );
            memcpy( temp, dw_sections[sect].u2.data, dw_sections[sect].u1.size );
            FMemFree( dw_sections[sect].u2.data );
            dw_sections[sect].u2.data = temp;
            dw_sections[sect].u1.size += MEM_INCREMENT;
        }
        memcpy( ( dw_sections[sect].u2.data + dw_sections[sect].offset ), block, size );
        break;
    case( FILE_SECTION ):
        _SDWrite( dw_sections[sect].u1.fp, block, size );
        break;
    default:
        Error( CP_FATAL_ERROR, "Internal browse generator error" );
        CSuicide();
    }
    dw_sections[sect].offset += size;
    if( dw_sections[sect].length < dw_sections[sect].offset ) {
        dw_sections[sect].length = dw_sections[sect].offset;
    }
}


static void CLIReloc( dw_sectnum sect, dw_reloc_type reloc_type, ... )
/********************************************************************/
{
    static char     zeros[] = { 0, 0 };
    dw_sectnum      sect_no;
    va_list         args;
    unsigned_32     u32_data;

    va_start( args, reloc_type );
    switch( reloc_type ) {
    case DW_W_LOW_PC:
    case DW_W_LABEL:
    case DW_W_DEFAULT_FUNCTION:
    case DW_W_ARANGE_ADDR:
        u32_data = 0;   // NOTE: assumes little-endian byte order
        CLIWrite( sect, &u32_data, sizeof( int ) );
        break;
    case DW_W_HIGH_PC:
        u32_data = 1;   // NOTE: assumes little-endian byte order
        CLIWrite( sect, &u32_data, sizeof( int ) );
        break;
    case DW_W_UNIT_SIZE:
        u32_data = 1;   // NOTE: assumes little-endian byte order
        CLIWrite( sect, &u32_data, sizeof( uint_32 ) );
        break;
    case DW_W_SECTION_POS:
        sect_no = va_arg( args, dw_sectnum );
        u32_data = dw_sections[sect_no].offset;
        CLIWrite( sect, &u32_data, sizeof( uint_32 ) );
        break;
    case DW_W_STATIC:
//        sym = va_arg( args, dw_sym_handle );
        u32_data = 0;
        CLIWrite( sect, &u32_data, sizeof( uint_32 ) );
        CLIWrite( sect, zeros, sizeof( zeros ) );
        break;
    case DW_W_SEGMENT:
        CLIWrite( sect, zeros, sizeof( zeros ) );
        break;
    default:
        va_end( args );
        abort();
        break;
    }
    va_end( args );
}


static void CLIZeroWrite( dw_sectnum sect, size_t size )
/******************************************************/
{
    char            *btmp;

    btmp = FMemAlloc( size + 1 );
    memset( btmp, 0, size );
    CLIWrite( sect, btmp, size );
    FMemFree( btmp );
}

static void CLISeek( dw_sectnum sect, dw_out_offset offset, int type )
/********************************************************************/
{
    size_t          temp;
    dw_out_offset   new_offset;

    new_offset = offset;
    switch( type ) {
    case DW_SEEK_CUR:
        new_offset = dw_sections[sect].offset + offset;
        break;
    case DW_SEEK_SET:
        break;
    case DW_SEEK_END:
        new_offset = dw_sections[sect].length + offset;
        break;
    }
    if( dw_sections[sect].sec_type == MEM_SECTION ) {
        if( dw_sections[sect].length < new_offset ) {
            temp = new_offset - dw_sections[sect].length;
            dw_sections[sect].offset = dw_sections[sect].length;
            CLIZeroWrite( sect, temp );
        }
    } else if( dw_sections[sect].sec_type == FILE_SECTION ) {
        if( dw_sections[sect].u1.fp == NULL ) {
            CLIZeroWrite( sect, new_offset );
        } else {
            if( dw_sections[sect].length < new_offset ) {
                SDSeek( dw_sections[sect].u1.fp, dw_sections[sect].length, 1 );
                chkIOErr( dw_sections[sect].u1.fp, SM_IO_READ_ERR, "temporary file" );
                dw_sections[sect].offset = dw_sections[sect].length;
                temp = new_offset - dw_sections[sect].length;
                CLIZeroWrite( sect, temp );
            } else {
                SDSeek( dw_sections[sect].u1.fp, new_offset, 1 );
                chkIOErr( dw_sections[sect].u1.fp, SM_IO_READ_ERR, "temporary file" );
            }
        }
    } else {
        CLIZeroWrite( sect, new_offset );
    }
    dw_sections[sect].offset = new_offset;
}

static dw_out_offset CLITell( dw_sectnum sect )
/*********************************************/
{
    return( dw_sections[sect].offset );
}


static void *CLIAlloc( size_t size )
/**********************************/
{
    void            *p;

    p = FMemAlloc( size );
    if( p == NULL && size > 0 ) {
        Error( MO_DYNAMIC_OUT );
        CSuicide();
    }
    return( p );
}


static void CLIFree( void *p )
/****************************/
{
    FMemFree( p );
}

static void CLILock( void )
/*************************/
{
}

static void CLIRewind( void )
/***************************/
{
    dw_sectnum      sect;

    for( sect = 0; sect < DW_DEBUG_MAX; sect++ ) {
        CLISeek( sect, 0, DW_SEEK_SET );
    }
}

void CLIInit( dw_funcs *cfuncs, sect_typ is_type )
/************************************************/
{
    dw_sectnum      sect;

    DWSetRtns( dw_cli_funcs, CLIReloc, CLIWrite, CLISeek, CLITell, CLIAlloc, CLIFree );
    if( cfuncs == NULL )
        return;
    *cfuncs = dw_cli_funcs;
    initial_section_type = is_type;
    for( sect = 0; sect < DW_DEBUG_MAX; sect++ ) {
        dw_sections[sect].sec_number = sect;
    }
}

void CLIDump( const char *filename )
/**********************************/
{
    file_handle     fp;

    SDSetAttr( REC_FIXED | SEEK );
    fp = SDOpen( filename, WRITE_FILE );
    chkIOErr( fp, SM_OPENING_FILE, filename );
    CLILock();
    CLIRewind();
    createBrowseFile( fp, filename );
    SDClose( fp );
}

void CLIFini( void )
/******************/
{
    dw_sectnum      sect;

    for( sect = 0; sect < DW_DEBUG_MAX; sect++ ) {
        if( ( dw_sections[sect].sec_type == FILE_SECTION ) && dw_sections[sect].u1.fp != NULL ) {
            SDClose( dw_sections[sect].u1.fp );
            if( dw_sections[sect].u2.filename != NULL ) {
                SDScratch( dw_sections[sect].u2.filename );
                FMemFree( dw_sections[sect].u2.filename );
            }
        } else if( ( dw_sections[sect].sec_type == MEM_SECTION ) && dw_sections[sect].u2.data != NULL ) {
            FMemFree( dw_sections[sect].u2.data );
        }
        memset( &dw_sections[sect], 0, sizeof( F77_DW_SECTION ) );
    }
}
