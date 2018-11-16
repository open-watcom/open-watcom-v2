/**************************************************************************
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
* Description:  Emulate long format of import library members in memory.
*               It is derived from short format of the import library members
*               in source .LIB file for each entry.
*               Emulated data is saved into coff_file_hnd structure.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include "walloca.h"
#include "coffimpl.h"
#include "cofforl.h"

#include "clibext.h"


/* note before you use this for anything make sure that the values below
are large enough for your purposes (particularly the num sections and symbols.
I put in some safety code in the string table because if the user declares
long function names (very long like > 512) it is possible that 1k will not
suffice for the string table size in import libraries.  the number of sections
and symbols however are fixed and suffice for import libraries  */

#define MAX_NUM_COFF_LIB_SECTIONS       8
#define MAX_NUM_COFF_LIB_SYMBOLS        32
#define INIT_MAX_SIZE_COFF_STRING_TABLE 1024

typedef struct {
    coff_file_header    header;
    coff_section_header section[MAX_NUM_COFF_LIB_SECTIONS];
    coff_symbol         symbol[MAX_NUM_COFF_LIB_SYMBOLS];
    unsigned_32         string_table_size;
    unsigned_32         max_string_table_size;
    char                *string_table;
    coff_file_handle    coff_file_hnd;
} coff_lib_file;

typedef struct {
    coff_import_object_name_type    type;
    short                           processor;
    char                            *DLLName;
    long                            ordinal;
    char                            *symName;
    char                            *exportedName;
    unsigned_32                     time_date_stamp;
} import_sym;

//IMPORT_DESCRIPT optional header

static unsigned_8 CoffImportAxpText[] = {
 0x00,0x00,0x7F,0x27,0x00,0x00,0x7B,0xA3,0x00,0x00,0xFB,0x6B
};

static unsigned_8 CoffImportPpcText[] = {
 0x00,0x00,0x62,0x81,0x00,0x00,0x8B,0x81,0x04,0x00,0x41,0x90,0xA6,0x03,0x89,0x7D,
 0x04,0x00,0x4B,0x80,0x20,0x04,0x80,0x4E
};

static unsigned_8 CoffImportPpcPdata[] = {
 0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
 0x0D,0x00,0x00,0x00
};

static unsigned_8 CoffImportX86Text[] = {
 0xFF,0x25,0x00,0x00,0x00,0x00
};

static void InitCoffFile( coff_lib_file *c_file )
{
    c_file->string_table = _ClientAlloc( c_file->coff_file_hnd, INIT_MAX_SIZE_COFF_STRING_TABLE );
    c_file->max_string_table_size = INIT_MAX_SIZE_COFF_STRING_TABLE;

}

static void SetCoffFile( coff_lib_file *c_file, short processor,
     unsigned_32 time_stamp, unsigned_16 opt_hdr_size)
{
    c_file->header.cpu_type = processor;
    c_file->header.num_sections = 0;
    c_file->header.time_stamp = time_stamp;
    c_file->header.num_symbols = 0;
    c_file->header.opt_hdr_size = opt_hdr_size;
    c_file->header.flags = COFF_IMAGE_FILE_32BIT_MACHINE;
    c_file->string_table_size = 0;
}

static void FiniCoffLibFile( coff_lib_file *c_file )
{
    _ClientFree( c_file->coff_file_hnd, c_file->string_table );
}

static void AddCoffString( coff_lib_file *c_file, const char *name, size_t len )
{
    char    *x;

    len++;  // add space for terminator character
    if( ( c_file->string_table_size + len ) >= c_file->max_string_table_size ) {
        c_file->max_string_table_size *= 2;
        x = _ClientAlloc( c_file->coff_file_hnd, c_file->max_string_table_size );
        if( x == NULL )
            return;
        memcpy( x, c_file->string_table, c_file->string_table_size );
        _ClientFree( c_file->coff_file_hnd, c_file->string_table );
        c_file->string_table = x;
    }
    memcpy( c_file->string_table + c_file->string_table_size, name, len );
    c_file->string_table_size += (unsigned_32)len;
}

static signed_16 AddCoffSection( coff_lib_file *c_file, const char *name, unsigned_32 size,
    unsigned_16 num_relocs, unsigned_32 flags )
{
    coff_section_header *section;
    size_t              len;

// Sections numbering has index base 1

    section = c_file->section + c_file->header.num_sections;
    len = strlen( name );
    memset( section, 0, COFF_SECTION_HEADER_SIZE );
    if( len > COFF_SEC_NAME_LEN ) {
        section->name[0] = '/';
        ultoa( c_file->string_table_size + 4, section->name + 1, 10 );
        AddCoffString( c_file, name, len );
    } else {
        memcpy( section->name, name, len );
    }
    section->size = size;
    section->num_relocs = num_relocs;
    section->flags = flags;
    return( ++(c_file->header.num_sections) );
}

static coff_quantity AddCoffSymbol( coff_lib_file *c_file, const char *name, unsigned_32 value,
    signed_16 sec_num,  unsigned_16 type, unsigned_8 class, unsigned_8 num_aux )
{
    coff_symbol *sym;
    size_t      len;

// Symbols numbering has index base 0

    sym = c_file->symbol + c_file->header.num_symbols;
    len = strlen( name );
    if( len > COFF_SYM_NAME_LEN ) {
        sym->name.non_name.zeros = 0;
        sym->name.non_name.offset = c_file->string_table_size + 4;
        AddCoffString( c_file, name, len );
    } else {
        memset( sym->name.name_string, 0, COFF_SYM_NAME_LEN );
        memcpy( sym->name.name_string, name, len );
    }
    sym->value = value;
    sym->sec_num = sec_num;
    sym->type = type;
    sym->storage_class = class;
    sym->num_aux = num_aux;
    return( c_file->header.num_symbols++ );
}

static coff_quantity AddCoffSymSec( coff_lib_file *c_file, unsigned_8 selection, signed_16 section_no )
{
    coff_sym_section    *sym;
    char                name[9];
    coff_section_header *section;
    coff_quantity       symbol_no;

    section = c_file->section + section_no - 1;
    memcpy( name, section->name, 8 );
    name[8] = '\0';
    symbol_no = AddCoffSymbol( c_file, name, 0x0, section_no, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_STATIC, 1 );
    sym = (coff_sym_section *)( c_file->symbol + c_file->header.num_symbols );
    sym->length = section->size;
    sym->num_relocs = section->num_relocs;
    sym->num_line_numbers = 0;
    sym->checksum = 0;
    if( selection == COFF_IMAGE_COMDAT_SELECT_ASSOCIATIVE ) {
        sym->number = section_no;
    } else {
        sym->number = 0;
    }
    sym->selection = selection;
    c_file->header.num_symbols++;
    return( symbol_no );
}

#define IMPLIB_HDR_SIZE (sizeof( size_t ) + sizeof( long ))
#define IMPLIB_LEN      (*(size_t *)coff_file_hnd->implib_data)
#define IMPLIB_POS      (*(long *)( coff_file_hnd->implib_data + sizeof( size_t ) ))
#define IMPLIB_DATA     (coff_file_hnd->implib_data + IMPLIB_HDR_SIZE)

static int DataImpLibInit( coff_file_handle coff_file_hnd )
{
    if( coff_file_hnd->implib_data == NULL ) {
        coff_file_hnd->implib_data = _ClientAlloc( coff_file_hnd, IMPLIB_HDR_SIZE );
        if( coff_file_hnd->implib_data == NULL )
            return( ORL_OUT_OF_MEMORY );
        IMPLIB_LEN = IMPLIB_HDR_SIZE;
        IMPLIB_POS = 0;
    }
    return( ORL_OKAY );
}

static int DataImpLibFini( coff_file_handle coff_file_hnd )
{
    if( coff_file_hnd->implib_data == NULL ) {
        _ClientFree( coff_file_hnd, coff_file_hnd->implib_data );
        coff_file_hnd->implib_data = NULL;
    }
    return( ORL_OKAY );
}

static int AddDataImpLib( coff_file_handle coff_file_hnd, const void *buff, size_t len )
{
    char    *x;

    x = _ClientAlloc( coff_file_hnd, IMPLIB_LEN + len );
    if( x == NULL )
        return( ORL_OUT_OF_MEMORY );
    memcpy( x, coff_file_hnd->implib_data, IMPLIB_LEN );
    memcpy( x + IMPLIB_LEN, buff, len );
    _ClientFree( coff_file_hnd, coff_file_hnd->implib_data );
    coff_file_hnd->implib_data = x;
    IMPLIB_LEN += len;
    return( ORL_OKAY );
}

void *ImportLibRead( coff_file_handle coff_file_hnd, size_t len )
{
    IMPLIB_POS += (long)len;
    return( IMPLIB_DATA + IMPLIB_POS - len );
}

int ImportLibSeek( coff_file_handle coff_file_hnd, long pos, int where )
{
    if( where == SEEK_SET ) {
        IMPLIB_POS = pos;
    } else if( where == SEEK_CUR ) {
        IMPLIB_POS += pos;
    } else {
        IMPLIB_POS = (long)IMPLIB_LEN - IMPLIB_HDR_SIZE - pos;
    }
    return( 0 );
}


static void CreateCoffFileHeader( coff_file_handle coff_file_hnd, coff_lib_file *c_file )
{
    coff_quantity   i;
    unsigned_32     d_ptr;

    d_ptr = sizeof( coff_file_header ) + c_file->header.opt_hdr_size + c_file->header.num_sections * COFF_SECTION_HEADER_SIZE;
    for( i = 0; i < c_file->header.num_sections; i++ ) {
        c_file->section[i].rawdata_ptr = d_ptr;
        d_ptr += c_file->section[i].size;
        c_file->section[i].reloc_ptr = d_ptr;
        d_ptr += c_file->section[i].num_relocs * COFF_RELOC_SIZE;
    }
    c_file->header.sym_table = d_ptr;
    AddDataImpLib( coff_file_hnd, &( c_file->header ), sizeof( coff_file_header ) );
}

static void CreateCoffSections( coff_file_handle coff_file_hnd, coff_lib_file *c_file )
{
    AddDataImpLib( coff_file_hnd, c_file->section, c_file->header.num_sections * COFF_SECTION_HEADER_SIZE );
}

static void CreateCoffSymbols( coff_file_handle coff_file_hnd, coff_lib_file *c_file )
{
    AddDataImpLib( coff_file_hnd, c_file->symbol, c_file->header.num_symbols * COFF_SYM_SIZE );
}

static void CreateCoffReloc( coff_file_handle coff_file_hnd, coff_sec_offset sec_offset, coff_quantity sym_tab_index, unsigned_16 type )
{
    //output is buffered so no point in putting COFF_RELOC struct
    AddDataImpLib( coff_file_hnd, &sec_offset, sizeof( sec_offset ) );
    AddDataImpLib( coff_file_hnd, &sym_tab_index, sizeof( sym_tab_index ) );
    AddDataImpLib( coff_file_hnd, &type, sizeof( type ) );
}

static void CreateCoffStringTable( coff_file_handle coff_file_hnd, coff_lib_file *c_file )
{
    c_file->string_table_size += 4;
    AddDataImpLib( coff_file_hnd, &( c_file->string_table_size ), 4 );
    AddDataImpLib( coff_file_hnd, c_file->string_table, c_file->string_table_size - 4 );
}

static char *getImportName( char *src, coff_import_object_name_type type )
{
    char    *end;

/*
I got following information from Microsoft about name type and name conversion.

    COFF_IMPORT_OBJECT_ORDINAL = 0,          // Import by ordinal
    COFF_IMPORT_OBJECT_NAME = 1,             // Import name == public symbol name.
    COFF_IMPORT_OBJECT_NAME_NO_PREFIX = 2,   // Import name == public symbol name skipping leading ?, @, or optionally _.
    COFF_IMPORT_OBJECT_NAME_UNDECORATE = 3,  // Import name == public symbol name skipping leading ?, @, or optionally _
                                        // and truncating at first @
*/
    // Note:
    // COFF_IMPORT_OBJECT_NAME_NO_PREFIX is used for C symbols with underscore as prefix
    // COFF_IMPORT_OBJECT_NAME_UNDECORATE is used for __stdcall and __fastcall name mangling
    // __stdcall uses underscore as prefix and @nn as suffix
    // __fastcall uses @ as prefix and @nn as suffix

    // this solution is stupid, probably it needs improvement
    // there is no more information from Microsoft

    if( *src != 0 ) {
        switch( type ) {
        case COFF_IMPORT_OBJECT_ORDINAL:
        case COFF_IMPORT_OBJECT_NAME:
            break;
        case COFF_IMPORT_OBJECT_NAME_UNDECORATE:
            // remove suffix @nn or @ if any
            end = src + strlen( src );
            while( end != src ) {
                --end;
                if( *end < '0' || *end > '9' ) {
                    if( *end == '@' ) {
                        *end = 0;
                    }
                    break;
                }
            }
            // fall through
        case COFF_IMPORT_OBJECT_NAME_NO_PREFIX:
            // remove prefix @ or _ if any
            if(( *src == '@' ) || ( *src == '_' ))
                src++;
            break;
        }
    }
    return( src );
}

static int CoffCreateImport( coff_file_handle coff_file_hnd, import_sym *import )
{
    unsigned_16     type;
    coff_lib_file   c_file;
    unsigned_16     ordinal;
    char            *buffer;
    union {
        char b32[4];
        char b64[8];
    } bnull;
    size_t          symbol_name_len;
    coff_quantity   symbol_text_exportedName;
    coff_quantity   symbol___imp_exportedName;
    coff_quantity   symbol_idata6 = 0;
    coff_quantity   symbol_toc = 0;
    char            *DLLSymbolName;
    size_t          dllsymbol_name_len;
    signed_16       section_no;
    size_t          len;

    c_file.coff_file_hnd = coff_file_hnd;
    InitCoffFile( &c_file );
    memset( bnull.b64, 0, sizeof( bnull.b64 ) );

    symbol_name_len = strlen( import->exportedName );
    DLLSymbolName = alloca( symbol_name_len + 1 );
    strcpy( DLLSymbolName, import->exportedName );
    DLLSymbolName = getImportName( DLLSymbolName, import->type );
    dllsymbol_name_len = strlen( DLLSymbolName );
    len = sizeof( "__IMPORT_DESCRIPTOR_" ) + strlen( import->DLLName );
    if( len < ( sizeof( "__imp_" ) + symbol_name_len ) )
        len = sizeof( "__imp_" ) + symbol_name_len;
    buffer = alloca( len );

    SetCoffFile( &c_file, import->processor, import->time_date_stamp, 0 );
    switch( import->processor ) {
    case COFF_IMAGE_FILE_MACHINE_ALPHA:
/* .text section header */
        section_no = AddCoffSection( &c_file, ".text", sizeof( CoffImportAxpText ), 3, COFF_IMAGE_SCN_ALIGN_16BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_CODE | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_EXECUTE );
        AddCoffSymSec( &c_file, COFF_IMAGE_COMDAT_SELECT_NODUPLICATES, section_no );
        symbol_text_exportedName = AddCoffSymbol( &c_file, import->exportedName, 0x0, section_no, 0x20, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
        break;
    case COFF_IMAGE_FILE_MACHINE_POWERPC:
/* .text section header */
        section_no = AddCoffSection( &c_file, ".text", sizeof( CoffImportPpcText ), 1, COFF_IMAGE_SCN_ALIGN_4BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_CODE | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_EXECUTE );
        AddCoffSymSec( &c_file, COFF_IMAGE_COMDAT_SELECT_NODUPLICATES, section_no );
        buffer[0] = '.';
        buffer[1] = '.';
        strcpy( buffer + 2, import->exportedName );
        symbol_text_exportedName = AddCoffSymbol( &c_file, buffer, 0x0, section_no, 0x20, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
/* .pdata section header */
        section_no = AddCoffSection( &c_file, ".pdata", sizeof( CoffImportPpcPdata ), 4, COFF_IMAGE_SCN_ALIGN_1BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ );
        AddCoffSymSec( &c_file, COFF_IMAGE_COMDAT_SELECT_ASSOCIATIVE, section_no );
/* .reldata section header */
        section_no = AddCoffSection( &c_file, ".reldata", sizeof( bnull.b64 ), 2, (unsigned_32)(COFF_IMAGE_SCN_ALIGN_8BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ |  COFF_IMAGE_SCN_MEM_WRITE) );
        if( import->type != COFF_IMPORT_OBJECT_ORDINAL) {
            AddCoffSymSec( &c_file, COFF_IMAGE_COMDAT_SELECT_NODUPLICATES, section_no );
        }
        AddCoffSymbol( &c_file, import->exportedName, 0x0, section_no, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
        break;
    case COFF_IMAGE_FILE_MACHINE_I386:
/* .text section header */
        section_no = AddCoffSection( &c_file, ".text", sizeof( CoffImportX86Text ), 1, COFF_IMAGE_SCN_ALIGN_2BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_CODE | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_EXECUTE );
        AddCoffSymSec( &c_file, COFF_IMAGE_COMDAT_SELECT_NODUPLICATES, section_no );
        symbol_text_exportedName = AddCoffSymbol( &c_file, import->exportedName, 0x0, section_no, 0x20, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
        break;
    default:
        return( ORL_ERROR );
    }

/* .idata$5 section header */
    if( import->type == COFF_IMPORT_OBJECT_ORDINAL) {
        section_no = AddCoffSection( &c_file, ".idata$5", sizeof( bnull.b32 ), 0, (unsigned_32)(COFF_IMAGE_SCN_ALIGN_4BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ |  COFF_IMAGE_SCN_MEM_WRITE) );
    } else {
        section_no = AddCoffSection( &c_file, ".idata$5", sizeof( bnull.b32 ), 1, (unsigned_32)(COFF_IMAGE_SCN_ALIGN_4BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ |  COFF_IMAGE_SCN_MEM_WRITE) );
    }
    AddCoffSymSec( &c_file, COFF_IMAGE_COMDAT_SELECT_NODUPLICATES, section_no );
    strcpy( buffer, "__imp_" );
    strcat( buffer, import->exportedName );
    symbol___imp_exportedName = AddCoffSymbol( &c_file, buffer, 0x0, section_no, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );

/* .idata$4 section header */
    if( import->type == COFF_IMPORT_OBJECT_ORDINAL) {
        section_no = AddCoffSection( &c_file, ".idata$4", sizeof( bnull.b32 ), 0, (unsigned_32)(COFF_IMAGE_SCN_ALIGN_4BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ |  COFF_IMAGE_SCN_MEM_WRITE) );
        AddCoffSymSec( &c_file, COFF_IMAGE_COMDAT_SELECT_ASSOCIATIVE, section_no );
    } else {
        section_no = AddCoffSection( &c_file, ".idata$4", sizeof( bnull.b32 ), 1, (unsigned_32)(COFF_IMAGE_SCN_ALIGN_4BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ |  COFF_IMAGE_SCN_MEM_WRITE) );
        AddCoffSymSec( &c_file, COFF_IMAGE_COMDAT_SELECT_ASSOCIATIVE, section_no );
/* .idata$6 section header */
        section_no = AddCoffSection( &c_file, ".idata$6", (unsigned_32)( sizeof( ordinal ) + ( dllsymbol_name_len | 1 ) + 1 ), 0, (unsigned_32)(COFF_IMAGE_SCN_ALIGN_2BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ |  COFF_IMAGE_SCN_MEM_WRITE) );
        symbol_idata6 = AddCoffSymSec( &c_file, COFF_IMAGE_COMDAT_SELECT_ASSOCIATIVE, section_no );
    }

/* other symbols */
    if( import->processor == COFF_IMAGE_FILE_MACHINE_POWERPC ) {
        symbol_toc = AddCoffSymbol( &c_file, ".toc", 0x0, 0, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
    }
    strcpy( buffer, "__IMPORT_DESCRIPTOR_" );
    strcat( buffer, import->DLLName );
    AddCoffSymbol( &c_file, buffer, 0x0, 0, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );

/* COFF file header */

    CreateCoffFileHeader( coff_file_hnd, &c_file );

/* COFF sections header */

    CreateCoffSections( coff_file_hnd, &c_file );

/* COFF section data */

    switch( import->processor ) {
    case COFF_IMAGE_FILE_MACHINE_ALPHA:
/* .text section data */
        AddDataImpLib( coff_file_hnd, CoffImportAxpText, sizeof( CoffImportAxpText ) );
/* .text relocations records */
        CreateCoffReloc( coff_file_hnd, 0x0, symbol___imp_exportedName, COFF_IMAGE_REL_ALPHA_REFHI );
        CreateCoffReloc( coff_file_hnd, 0x0, symbol_text_exportedName, COFF_IMAGE_REL_ALPHA_PAIR );
        CreateCoffReloc( coff_file_hnd, 0x4, symbol___imp_exportedName, COFF_IMAGE_REL_ALPHA_REFLO );
        break;
    case COFF_IMAGE_FILE_MACHINE_POWERPC:
/* .text section data */
        AddDataImpLib( coff_file_hnd, CoffImportPpcText, sizeof( CoffImportPpcText ) );
/* .text relocations records */
        CreateCoffReloc( coff_file_hnd, 0x0, symbol___imp_exportedName, COFF_IMAGE_REL_PPC_TOCREL14 | COFF_IMAGE_REL_PPC_TOCDEFN );
/* .pdata section data */
        AddDataImpLib( coff_file_hnd, CoffImportPpcPdata, sizeof( CoffImportPpcPdata ) );
/* .pdata relocations records */
        CreateCoffReloc( coff_file_hnd, 0x0, symbol_text_exportedName, COFF_IMAGE_REL_PPC_ADDR32 );
        CreateCoffReloc( coff_file_hnd, 0x4, symbol_text_exportedName, COFF_IMAGE_REL_PPC_ADDR32 );
        CreateCoffReloc( coff_file_hnd, 0x10, symbol_text_exportedName, COFF_IMAGE_REL_PPC_ADDR32 );
        CreateCoffReloc( coff_file_hnd, 0x80410004, symbol_text_exportedName, COFF_IMAGE_REL_PPC_SECTION );
/* .reldata section data */
        AddDataImpLib( coff_file_hnd, bnull.b64, sizeof( bnull.b64 ) );
/* .reldata relocations records */
        CreateCoffReloc( coff_file_hnd, 0x0, symbol_text_exportedName, COFF_IMAGE_REL_PPC_ADDR32 );
        CreateCoffReloc( coff_file_hnd, 0x4, symbol_toc, COFF_IMAGE_REL_PPC_ADDR32 );
        break;
    case COFF_IMAGE_FILE_MACHINE_I386:
/* .text section data */
        AddDataImpLib( coff_file_hnd, CoffImportX86Text, sizeof( CoffImportX86Text ) );
/* .text relocations records */
        CreateCoffReloc( coff_file_hnd, 0x2, symbol___imp_exportedName, COFF_IMAGE_REL_I386_DIR32 );
        break;
    }

    type = 0;
    ordinal = (unsigned_16)import->ordinal;
    if( import->type == COFF_IMPORT_OBJECT_ORDINAL ) {
/* .idata$5 section data - ordinal */
        AddDataImpLib( coff_file_hnd, &ordinal, sizeof( ordinal ) );
        type = 0x8000;
        AddDataImpLib( coff_file_hnd, &type, sizeof( type ) );
/* .idata$4 section data - ordinal */
        AddDataImpLib( coff_file_hnd, &ordinal, sizeof( ordinal ) );
        AddDataImpLib( coff_file_hnd, &type, sizeof( type ) );
    } else {
        switch( import->processor ) {
        case COFF_IMAGE_FILE_MACHINE_ALPHA:
            type = COFF_IMAGE_REL_ALPHA_REFLONGNB;
            break;
        case COFF_IMAGE_FILE_MACHINE_POWERPC:
            type = COFF_IMAGE_REL_PPC_ADDR32NB;
            break;
        case COFF_IMAGE_FILE_MACHINE_I386:
            type = COFF_IMAGE_REL_I386_DIR32NB;
            break;
        }
/* .idata$5 section data - name */
        AddDataImpLib( coff_file_hnd, bnull.b32, sizeof( bnull.b32 ) );
/* .idata$5 relocations records */
        CreateCoffReloc( coff_file_hnd, 0, symbol_idata6, type );
/* .idata$4 section data - name */
        AddDataImpLib( coff_file_hnd, bnull.b32, sizeof( bnull.b32 ) );
/* .idata$4 relocations records */
        CreateCoffReloc( coff_file_hnd, 0, symbol_idata6, type );
/* .idata$6 section data - name */
        AddDataImpLib( coff_file_hnd, &ordinal, sizeof( ordinal ) );
        AddDataImpLib( coff_file_hnd, DLLSymbolName, (int)( dllsymbol_name_len + 1 ) );
        if( (dllsymbol_name_len & 1) == 0 ) {
            AddDataImpLib( coff_file_hnd, "\0", 1 );
        }
    }

/* COFF symbol table */

    CreateCoffSymbols( coff_file_hnd, &c_file );

/* COFF strings table */

    CreateCoffStringTable( coff_file_hnd, &c_file );
    FiniCoffLibFile( &c_file );
    return( ORL_OKAY );
}

int convert_import_library_init( coff_file_handle coff_file_hnd )
{
    coff_import_object_header   *i_hdr;
    import_sym                  sym;

    // init import library data structure
    if( DataImpLibInit( coff_file_hnd ) != ORL_OKAY )
        return( ORL_OUT_OF_MEMORY );

    i_hdr = (coff_import_object_header *)coff_file_hnd->f_hdr_buffer;
    sym.processor = i_hdr->machine;
    sym.exportedName = ORL_PTR_READ( coff_file_hnd->coff_hnd, coff_file_hnd->fp, i_hdr->size_of_data );
    sym.DLLName = sym.exportedName + strlen( sym.exportedName ) + 1;
    sym.time_date_stamp = i_hdr->time_date_stamp;
    sym.type = i_hdr->name_type;
    sym.ordinal = i_hdr->oh.ordinal;
    return( CoffCreateImport( coff_file_hnd, &sym ) );
}

int convert_import_library_fini( coff_file_handle coff_file_hnd )
{
    return( DataImpLibFini( coff_file_hnd ) );
}
