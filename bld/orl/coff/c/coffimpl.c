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
#include "roundmac.h"
#include "implcoff.h"

#include "clibext.h"


/*
 * note before you use this for anything make sure that the values below
 * are large enough for your purposes (particularly the num sections and symbols.
 * I put in some safety code in the string table because if the user declares
 * long function names (very long like > 512) it is possible that 1k will not
 * suffice for the string table size in import libraries.  the number of sections
 * and symbols however are fixed and suffice for import libraries
 */

#define MAX_NUM_COFF_LIB_SECTIONS       8
#define MAX_NUM_COFF_LIB_SYMBOLS        32
#define INIT_MAX_SIZE_COFF_STRING_TABLE 1024

typedef struct {
    coff_file_header    header;
    coff_section_header sections[MAX_NUM_COFF_LIB_SECTIONS];
    coff_symbol         symbols[MAX_NUM_COFF_LIB_SYMBOLS];
    unsigned_32         string_table_size;
    unsigned_32         max_string_table_size;
    char                *string_table;
    coff_file_handle    coff_file_hnd;
} coff_lib_file;

typedef struct {
    coff_import_object_name_type    type;
    int                             processor;
    char                            *DLLName;
    char                            *symName;
    char                            *exportedName;
    unsigned_32                     time_date_stamp;
    unsigned_16                     ordinal;
} import_sym;

/*
 * IMPORT_DESCRIPT optional header
 */

static void InitCoffFile( coff_lib_file *c_file )
{
    c_file->string_table = _ClientAlloc( c_file->coff_file_hnd, INIT_MAX_SIZE_COFF_STRING_TABLE );
    c_file->max_string_table_size = INIT_MAX_SIZE_COFF_STRING_TABLE;

}

static void SetCoffFile( coff_lib_file *c_file, int processor,
     unsigned_32 time_stamp, unsigned_16 opt_hdr_size)
{
    c_file->header.cpu_type = (uint_16)processor;
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
    strcpy( c_file->string_table + c_file->string_table_size, name );
    c_file->string_table_size += (unsigned_32)len;
}

static char *formatDigit( char *buf, size_t len, unsigned long value )
{
    ldiv_t x;

    if( len > 0 && value > 0 ) {
        x = ldiv( value, 10 );
        buf = formatDigit( buf, len - 1, x.quot );
        *buf++ = x.rem + '0';
    }
    return( buf );
}

static signed_16 AddCoffSection( coff_lib_file *c_file, const char *name, unsigned_32 size,
    unsigned_16 num_relocs, unsigned_32 flags )
{
    coff_section_header *section;
    size_t              len;

    /*
     * Sections numbering has index base 1
     */
    section = c_file->sections + c_file->header.num_sections++;
    len = strlen( name );
    memset( section, 0, COFF_SECTION_HEADER_SIZE );
    if( len > COFF_SEC_NAME_LEN ) {
        section->name[0] = '/';
        formatDigit( section->name + 1, COFF_SEC_NAME_LEN - 1, c_file->string_table_size + 4 );
        AddCoffString( c_file, name, len );
    } else {
        strncpy( section->name, name, COFF_SEC_NAME_LEN );
    }
    section->size = size;
    section->num_relocs = num_relocs;
    section->flags = flags;
    return( c_file->header.num_sections );
}

static coff_quantity AddCoffSymbol( coff_lib_file *c_file, signed_16 sec_num, const char *name,
    unsigned_32 value, unsigned_16 type, unsigned_8 class, unsigned_8 num_aux )
/**********************************************************************************************
 * sec_num is 1-based
 */
{
    coff_symbol     *sym;
    size_t          len;

    /*
     * Symbols numbering has index base 0
     */
    sym = c_file->symbols + c_file->header.num_symbols;
    len = strlen( name );
    if( len > COFF_SYM_NAME_LEN ) {
        sym->name.non_name.zeros = 0;
        sym->name.non_name.offset = c_file->string_table_size + 4;
        AddCoffString( c_file, name, len );
    } else {
        strncpy( sym->name.name_string, name, COFF_SYM_NAME_LEN );
    }
    sym->value = value;
    sym->sec_num = sec_num; /* 1-based, 0 is special value */
    sym->type = type;
    sym->storage_class = class;
    sym->num_aux = num_aux;
    return( c_file->header.num_symbols++ );
}

static coff_quantity AddCoffSymbolWithPrefix( coff_lib_file *c_file, signed_16 sec_num, const char *prefix,
    const char *name, unsigned_32 value, unsigned_16 type, unsigned_8 class, unsigned_8 num_aux )
/*********************************************************************************************************
 * sec_num is 1-based
 */
{
    char        *buffer;
    size_t      len;

    len = strlen( prefix );
    buffer = alloca( len + strlen( name ) + 1 );
    strcpy( strcpy( buffer, prefix ) + len, name );
    return( AddCoffSymbol( c_file, sec_num, name, value, type, class, num_aux ) );
}

static coff_quantity AddCoffSymSec( coff_lib_file *c_file, signed_16 sec_num, unsigned_8 selection )
/***************************************************************************************************
 * sec_num is 1-based
 */
{
    coff_sym_section    *sym;
    char                name[COFF_SEC_NAME_LEN + 1];
    coff_section_header *section;
    coff_quantity       symb_idx;

    /*
     * get section name
     * section number is 1-based, use 0-based index
     */
    section = c_file->sections + sec_num - 1;
    strncpy( name, section->name, COFF_SEC_NAME_LEN );
    name[COFF_SEC_NAME_LEN] = '\0';
    /*
     * add section symbol record
     */
    symb_idx = AddCoffSymbol( c_file, sec_num, name, 0x0, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_STATIC, 1 );
    /*
     * add section auxiliary record
     */
    sym = (coff_sym_section *)( c_file->symbols + c_file->header.num_symbols++ );
    sym->length = section->size;
    sym->num_relocs = section->num_relocs;
    sym->num_line_numbers = 0;
    sym->checksum = 0;
    if( selection == COFF_IMAGE_COMDAT_SELECT_ASSOCIATIVE ) {
        sym->number = sec_num;  /* 1-based, 0 is special value */
        sym->high_number = 0;
    } else {
        sym->number = 0;
        sym->high_number = 0;
    }
    sym->selection = selection;
    /*
     * return section symbol index
     */
    return( symb_idx );
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

static void AddStringPadding( coff_file_handle coff_file_hnd, const char *name, size_t len )
{
    AddDataImpLib( coff_file_hnd, name, (int)len );
    if( len & 1 ) {
        AddDataImpLib( coff_file_hnd, "\0", 1 );
    }
}

static void CreateCoffFileHeader( coff_file_handle coff_file_hnd, coff_lib_file *c_file )
{
    coff_quantity   i;
    unsigned_32     d_ptr;

    d_ptr = sizeof( coff_file_header ) + c_file->header.opt_hdr_size + c_file->header.num_sections * COFF_SECTION_HEADER_SIZE;
    for( i = 0; i < c_file->header.num_sections; i++ ) {
        c_file->sections[i].rawdata_ptr = d_ptr;
        d_ptr += c_file->sections[i].size;
        c_file->sections[i].reloc_ptr = d_ptr;
        d_ptr += c_file->sections[i].num_relocs * COFF_RELOC_SIZE;
    }
    c_file->header.sym_table = d_ptr;
    AddDataImpLib( coff_file_hnd, &( c_file->header ), sizeof( coff_file_header ) );
}

static void CreateCoffSections( coff_file_handle coff_file_hnd, coff_lib_file *c_file )
{
    AddDataImpLib( coff_file_hnd, c_file->sections, c_file->header.num_sections * COFF_SECTION_HEADER_SIZE );
}

static void CreateCoffSymbols( coff_file_handle coff_file_hnd, coff_lib_file *c_file )
{
    AddDataImpLib( coff_file_hnd, c_file->symbols, c_file->header.num_symbols * COFF_SYM_SIZE );
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

static void AddDataImportTablesNamed( coff_file_handle coff_file_hnd, import_sym *import, unsigned symb_hints )
{
    char        nulls[8];
    unsigned_16 type;
    unsigned    thunk_size;

    switch( import->processor ) {
    case COFF_IMAGE_FILE_MACHINE_ALPHA:
        type = COFF_IMAGE_REL_ALPHA_REFLONGNB;
        break;
    case COFF_IMAGE_FILE_MACHINE_R3000:
    case COFF_IMAGE_FILE_MACHINE_R4000:
        type = COFF_IMAGE_REL_MIPS_REFWORDNB;
        break;
    case COFF_IMAGE_FILE_MACHINE_POWERPC:
        type = COFF_IMAGE_REL_PPC_ADDR32NB;
        break;
    case COFF_IMAGE_FILE_MACHINE_AMD64:
        type = COFF_IMAGE_REL_AMD64_ADDR32NB;
        break;
    case COFF_IMAGE_FILE_MACHINE_I386:
        type = COFF_IMAGE_REL_I386_DIR32NB;
        break;
    }
    thunk_size = 4;
    if( import->processor == COFF_IMAGE_FILE_MACHINE_AMD64 ) {
        thunk_size = 8;
    }
    memset( nulls, 0, sizeof( nulls ) );
    /*
     * .idata$5 section data - name
     */
    AddDataImpLib( coff_file_hnd, nulls, thunk_size );
    /*
     * .idata$5 relocations records
     */
    CreateCoffReloc( coff_file_hnd, 0x0000, symb_hints, type );
    /*
     * .idata$4 section data - name
     */
    AddDataImpLib( coff_file_hnd, nulls, thunk_size );
    /*
     * .idata$4 relocations records
     */
    CreateCoffReloc( coff_file_hnd, 0x0000, symb_hints, type );
}

static void AddDataImportTablesOrdinal( coff_file_handle coff_file_hnd, import_sym *import )
{
    unsigned_64 import_table_entry;

    switch( import->processor ) {
    case COFF_IMAGE_FILE_MACHINE_AMD64:
        import_table_entry.u._64[0] = import->ordinal | COFF_IMAGE_ORDINAL_FLAG64;
        /*
         * .idata$5 section data - ordinal
         */
        AddDataImpLib( coff_file_hnd, &import_table_entry.u._64[0], 8 );
        /*
         * .idata$4 section data - ordinal
         */
        AddDataImpLib( coff_file_hnd, &import_table_entry.u._64[0], 8 );
        return;
    case COFF_IMAGE_FILE_MACHINE_ALPHA:
    case COFF_IMAGE_FILE_MACHINE_R3000:
    case COFF_IMAGE_FILE_MACHINE_R4000:
    case COFF_IMAGE_FILE_MACHINE_POWERPC:
    case COFF_IMAGE_FILE_MACHINE_I386:
    default:
        break;
    }
    import_table_entry.u._32[0] = import->ordinal | COFF_IMAGE_ORDINAL_FLAG32;
    /*
     * .idata$5 section data - ordinal
     */
    AddDataImpLib( coff_file_hnd, &import_table_entry.u._32[0], 4 );
    /*
     * .idata$4 section data - ordinal
     */
    AddDataImpLib( coff_file_hnd, &import_table_entry.u._32[0], 4 );
}

static int CoffCreateImport( coff_file_handle coff_file_hnd, import_sym *import )
{
    coff_lib_file   c_file;
    unsigned_16     ordinal;
    char            nulls[8];
    size_t          symbol_name_len;
    coff_quantity   symb_name;
    coff_quantity   symb_imp_name;
    coff_quantity   symb_hints;
    coff_quantity   symb_toc;
    char            *DLLSymbolName;
    size_t          dllsymbol_name_len;
    signed_16       sec_num;
    bool            is_named;
    unsigned_32     thunk_section_align;
    unsigned        thunk_size;

    c_file.coff_file_hnd = coff_file_hnd;
    InitCoffFile( &c_file );

    symbol_name_len = strlen( import->exportedName );
    DLLSymbolName = alloca( symbol_name_len + 1 );
    strcpy( DLLSymbolName, import->exportedName );
    DLLSymbolName = getImportName( DLLSymbolName, import->type );
    dllsymbol_name_len = strlen( DLLSymbolName );

    thunk_size = 4;
    thunk_section_align = COFF_IMAGE_SCN_ALIGN_4BYTES;
    if( import->processor == COFF_IMAGE_FILE_MACHINE_AMD64 ) {
        thunk_size = 8;
        thunk_section_align = COFF_IMAGE_SCN_ALIGN_8BYTES;
    }

    SetCoffFile( &c_file, import->processor, import->time_date_stamp, 0 );
    /*
     * .text section header
     */
    switch( import->processor ) {
    case COFF_IMAGE_FILE_MACHINE_ALPHA:
        sec_num = AddCoffSection( &c_file, ".text", sizeof( CoffImportAxpText ), 3, COFF_IMAGE_SCN_ALIGN_16BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_CODE | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_EXECUTE );
        break;
    case COFF_IMAGE_FILE_MACHINE_R3000:
    case COFF_IMAGE_FILE_MACHINE_R4000:
        sec_num = AddCoffSection( &c_file, ".text", sizeof( CoffImportMipsText ), 3, COFF_IMAGE_SCN_ALIGN_16BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_CODE | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_EXECUTE );
        break;
    case COFF_IMAGE_FILE_MACHINE_POWERPC:
        sec_num = AddCoffSection( &c_file, ".text", sizeof( CoffImportPpcText ), 1, COFF_IMAGE_SCN_ALIGN_4BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_CODE | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_EXECUTE );
        break;
    case COFF_IMAGE_FILE_MACHINE_AMD64:
        sec_num = AddCoffSection( &c_file, ".text", sizeof( CoffImportX64Text ), 1, COFF_IMAGE_SCN_ALIGN_2BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_CODE | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_EXECUTE );
        break;
    case COFF_IMAGE_FILE_MACHINE_I386:
        sec_num = AddCoffSection( &c_file, ".text", sizeof( CoffImportX86Text ), 1, COFF_IMAGE_SCN_ALIGN_2BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_CODE | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_EXECUTE );
        break;
    default:
        return( ORL_ERROR );
    }
    AddCoffSymSec( &c_file, sec_num, COFF_IMAGE_COMDAT_SELECT_NODUPLICATES );
    if( import->processor == COFF_IMAGE_FILE_MACHINE_POWERPC ) {
        symb_name = AddCoffSymbolWithPrefix( &c_file, sec_num, "..", import->exportedName, 0x0, COFF_IMAGE_SYM_TYPE_FUNCTION, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
    } else {
        symb_name = AddCoffSymbol( &c_file, sec_num, import->exportedName, 0x0, COFF_IMAGE_SYM_TYPE_FUNCTION, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
    }

    is_named = ( import->type != COFF_IMPORT_OBJECT_ORDINAL );
    if( import->processor == COFF_IMAGE_FILE_MACHINE_POWERPC ) {
        /*
         * .pdata section header
         */
        sec_num = AddCoffSection( &c_file, ".pdata", sizeof( CoffImportPpcPdata ), 4, COFF_IMAGE_SCN_ALIGN_1BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ );
        AddCoffSymSec( &c_file, sec_num, COFF_IMAGE_COMDAT_SELECT_ASSOCIATIVE );
        /*
         * .reldata section header
         */
        sec_num = AddCoffSection( &c_file, ".reldata", 8, 2, COFF_IMAGE_SCN_ALIGN_8BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ |  COFF_IMAGE_SCN_MEM_WRITE );
        if( is_named ) {
            AddCoffSymSec( &c_file, sec_num, COFF_IMAGE_COMDAT_SELECT_NODUPLICATES );
        }
        AddCoffSymbol( &c_file, sec_num, import->exportedName, 0x0, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
    }

    /*
     * .idata$5 section header
     */
    sec_num = AddCoffSection( &c_file, ".idata$5", thunk_size, ( is_named ) ? 1 : 0, thunk_section_align | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_WRITE );
    AddCoffSymSec( &c_file, sec_num, COFF_IMAGE_COMDAT_SELECT_NODUPLICATES );
    symb_imp_name = AddCoffSymbolWithPrefix( &c_file, sec_num, "__imp_", import->exportedName, 0x0, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
    /*
     * .idata$4 section header
     */
    sec_num = AddCoffSection( &c_file, ".idata$4", thunk_size, ( is_named ) ? 1 : 0, thunk_section_align | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_WRITE );
    AddCoffSymSec( &c_file, sec_num, COFF_IMAGE_COMDAT_SELECT_ASSOCIATIVE );
    /*
     * .idata$6 section header
     */
    if( is_named ) {
        sec_num = AddCoffSection( &c_file, ".idata$6", sizeof( ordinal ) + __ROUND_UP_SIZE_EVEN( dllsymbol_name_len + 1 ), 0, COFF_IMAGE_SCN_ALIGN_2BYTES | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_WRITE );
        symb_hints = AddCoffSymSec( &c_file, sec_num, COFF_IMAGE_COMDAT_SELECT_ASSOCIATIVE );
    }
    /*
     * other symbols
     */
    symb_toc = 0;
    if( import->processor == COFF_IMAGE_FILE_MACHINE_POWERPC ) {
        symb_toc = AddCoffSymbol( &c_file, 0, ".toc", 0x0, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
    }
    AddCoffSymbolWithPrefix( &c_file, 0, "__IMPORT_DESCRIPTOR_", import->DLLName, 0x0, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
    /*
     * COFF file header
     */
    CreateCoffFileHeader( coff_file_hnd, &c_file );
    /*
     * COFF sections header
     */
    CreateCoffSections( coff_file_hnd, &c_file );
    /*
     * COFF section data
     */
    switch( import->processor ) {
    case COFF_IMAGE_FILE_MACHINE_ALPHA:
        /*
         * .text section data
         */
        AddDataImpLib( coff_file_hnd, CoffImportAxpText, sizeof( CoffImportAxpText ) );
        /*
         * .text relocations records
         */
        CreateCoffReloc( coff_file_hnd, 0x0000, symb_imp_name, COFF_IMAGE_REL_ALPHA_REFHI );
        CreateCoffReloc( coff_file_hnd, 0x0000, symb_name, COFF_IMAGE_REL_ALPHA_PAIR );
        CreateCoffReloc( coff_file_hnd, 0x0004, symb_imp_name, COFF_IMAGE_REL_ALPHA_REFLO );
        break;
    case COFF_IMAGE_FILE_MACHINE_R3000:
    case COFF_IMAGE_FILE_MACHINE_R4000:
        /*
         * .text section data
         */
        AddDataImpLib( coff_file_hnd, CoffImportMipsText, sizeof( CoffImportMipsText ) );
        /*
         * .text relocations records
         */
        CreateCoffReloc( coff_file_hnd, 0x0000, symb_imp_name, COFF_IMAGE_REL_MIPS_REFHI );
        CreateCoffReloc( coff_file_hnd, 0x0000, symb_name, COFF_IMAGE_REL_MIPS_PAIR );
        CreateCoffReloc( coff_file_hnd, 0x0004, symb_imp_name, COFF_IMAGE_REL_MIPS_REFLO );
        break;
    case COFF_IMAGE_FILE_MACHINE_POWERPC:
        /*
         * .text section data
         */
        AddDataImpLib( coff_file_hnd, CoffImportPpcText, sizeof( CoffImportPpcText ) );
        /*
         * .text relocations records
         */
        CreateCoffReloc( coff_file_hnd, 0x0000, symb_imp_name, COFF_IMAGE_REL_PPC_TOCREL14 | COFF_IMAGE_REL_PPC_TOCDEFN );
        /*
         * .pdata section data
         */
        AddDataImpLib( coff_file_hnd, CoffImportPpcPdata, sizeof( CoffImportPpcPdata ) );
        /*
         * .pdata relocations records
         */
        CreateCoffReloc( coff_file_hnd, 0x0000, symb_name, COFF_IMAGE_REL_PPC_ADDR32 );
        CreateCoffReloc( coff_file_hnd, 0x0004, symb_name, COFF_IMAGE_REL_PPC_ADDR32 );
        CreateCoffReloc( coff_file_hnd, 0x0010, symb_name, COFF_IMAGE_REL_PPC_ADDR32 );
        CreateCoffReloc( coff_file_hnd, 0x80410004, symb_name, COFF_IMAGE_REL_PPC_SECTION );
        /*
         * .reldata section data
         */
        memset( nulls, 0, sizeof( nulls ) );
        AddDataImpLib( coff_file_hnd, nulls, 8 );
        /*
         * .reldata relocations records
         */
        CreateCoffReloc( coff_file_hnd, 0x0000, symb_name, COFF_IMAGE_REL_PPC_ADDR32 );
        CreateCoffReloc( coff_file_hnd, 0x0004, symb_toc, COFF_IMAGE_REL_PPC_ADDR32 );
        break;
    case COFF_IMAGE_FILE_MACHINE_AMD64:
        /*
         * .text section data
         */
        AddDataImpLib( coff_file_hnd, CoffImportX64Text, sizeof( CoffImportX64Text ) );
        /*
         * .text relocations records
         */
        CreateCoffReloc( coff_file_hnd, 0x0002, symb_imp_name, COFF_IMAGE_REL_AMD64_ADDR32NB );
        break;
    case COFF_IMAGE_FILE_MACHINE_I386:
        /*
         * .text section data
         */
        AddDataImpLib( coff_file_hnd, CoffImportX86Text, sizeof( CoffImportX86Text ) );
        /*
         * .text relocations records
         */
        CreateCoffReloc( coff_file_hnd, 0x0002, symb_imp_name, COFF_IMAGE_REL_I386_DIR32 );
        break;
    }
    /*
     * .idata$5 and .idata$4 sections and relocations data
     */
    if( is_named ) {
        AddDataImportTablesNamed( coff_file_hnd, import, symb_hints );
    } else {
        AddDataImportTablesOrdinal( coff_file_hnd, import );
    }
    if( is_named ) {
        /*
         * .idata$6 section data - name
         */
        ordinal = import->ordinal;
        AddDataImpLib( coff_file_hnd, &ordinal, sizeof( ordinal ) );
        AddStringPadding( coff_file_hnd, DLLSymbolName, dllsymbol_name_len + 1 );
    }
    /*
     * COFF symbol table
     */
    CreateCoffSymbols( coff_file_hnd, &c_file );
    /*
     * COFF strings table
     */
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
