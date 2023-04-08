/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Write COFF style imports.
*
****************************************************************************/


#include "wlib.h"
#include "walloca.h"
#include <assert.h>
#include <error.h>
#include "coff.h"
#include "coffwrt.h"
#include "roundmac.h"
#include "implcoff.h"

#include "clibext.h"


/*
 * Microsoft PE/COFF .idata section description
 *
 * following sub-sections are defined in object file for linker
 * to link DLL entries into executable
 *
 * structures are described in Microsoft PE/COFF documentation
 * and in Windows header files winnt.h
 *
 * .idata$2     Import Directory Table (IDT) is array of IMAGE_IMPORT_DESCRIPTOR's
 * .idata$3     terminator (null entry) of IDT (.idata$2)
 * .idata$4     Import Lookup Table (ILT) is array of array of pointers to Import Name Table (INT)
 * .idata$5     Import Address Table (IAT) is array of array of pointers to INT
 * .idata$6     Import Name Table (INT) is array of names with hints and DLL name
 *
 */

typedef struct name_len {
    size_t      len;
    char        *name;
} name_len;

static void InitCoffFile( coff_lib_file *c_file )
{
    c_file->string_table = MemAlloc( INIT_MAX_SIZE_COFF_STRING_TABLE );
    c_file->max_string_table_size = INIT_MAX_SIZE_COFF_STRING_TABLE;

}

static void SetCoffFile( coff_lib_file *c_file, sym_file *sfile )
{
    switch( sfile->import->processor ) {
    case WL_PROC_AXP:
        c_file->header.cpu_type = COFF_IMAGE_FILE_MACHINE_ALPHA;
        break;
    case WL_PROC_MIPS:
        c_file->header.cpu_type = COFF_IMAGE_FILE_MACHINE_R4000;
        break;
    case WL_PROC_PPC:
        c_file->header.cpu_type = COFF_IMAGE_FILE_MACHINE_POWERPC;
        break;
    case WL_PROC_X64:
        c_file->header.cpu_type = COFF_IMAGE_FILE_MACHINE_AMD64;
        break;
    case WL_PROC_X86:
    default:
        c_file->header.cpu_type = COFF_IMAGE_FILE_MACHINE_I386;
        break;
    }
    c_file->header.num_sections = 0;
    c_file->header.time_stamp = sfile->arch.date;
    c_file->header.num_symbols = 0;
    c_file->header.opt_hdr_size = 0;
    c_file->header.flags = COFF_IMAGE_FILE_32BIT_MACHINE;
    c_file->string_table_size = 0;
}

static void FiniCoffLibFile( coff_lib_file *c_file )
{
    MemFree( c_file->string_table );
}

static void AddCoffString( coff_lib_file  *c_file, const char *name, size_t len )
{
    len++;  /* add space for null terminating character */
    if( ( c_file->string_table_size + len ) >= c_file->max_string_table_size ) {
        c_file->max_string_table_size *= 2;
        c_file->string_table = MemRealloc( c_file->string_table, c_file->max_string_table_size );
    }
    strcpy( c_file->string_table + c_file->string_table_size, name );
    c_file->string_table_size += len;
}

static signed_16 AddCoffSection( coff_lib_file *c_file, const char *name, unsigned_32 size,
    unsigned_16 num_relocs, unsigned_32 flags )
/******************************************************************************************
 * return section number (1-based)
 */
{
    coff_section_header     *section;
    size_t                  len;

    section = c_file->sections + c_file->header.num_sections++;
    len = strlen( name );
    memset( section, 0, COFF_SECTION_HEADER_SIZE );
    if( len > COFF_SEC_NAME_LEN ) {
        sprintf( section->name, "/%lu", (unsigned long)( c_file->string_table_size + 4 ) );
        AddCoffString( c_file, name, len );
    } else {
        strncpy( section->name, name, COFF_SEC_NAME_LEN );
    }
    section->size = size;
    section->num_relocs = num_relocs;
    section->flags = flags;
    return( c_file->header.num_sections );
}

static unsigned AddCoffSymbol( coff_lib_file *c_file, signed_16 sec_num, const char *name,
    unsigned_32 value, unsigned_16 type, unsigned_8 class, unsigned_8 num_aux )
/*****************************************************************************************
 * input sec_num is 1-based
 *
 * return symbol index (0-based)
 */
{
    coff_symbol _WCUNALIGNED    *sym;
    size_t                      len;

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

static unsigned AddCoffSymSec( coff_lib_file *c_file, signed_16 sec_num, unsigned_8 selection )
/**********************************************************************************************
 * input sec_num is 1-based
 *
 * return symbol index (0-based)
 */
{
    coff_sym_section _WCUNALIGNED   *sym;
    char                            name[COFF_SEC_NAME_LEN + 1];
    coff_section_header             *section;
    unsigned                        symb_idx;

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
    } else {
        sym->number = 0;
    }
    sym->high_number = 0;
    sym->selection = selection;
    /*
     * return section symbol index (0-based)
     */
    return( symb_idx );
}

static void WriteStringPadding( libfile io, const char *name, size_t len )
{
    LibWrite( io, name, len );
    if( len & 1 ) {
        LibWrite( io, "\0", 1 );
    }
}


static void WriteCoffFileHeader( libfile io, coff_lib_file *c_file )
{
    unsigned            i;
    unsigned_32         d_ptr;

    d_ptr = COFF_FILE_HEADER_SIZE + c_file->header.opt_hdr_size
            + c_file->header.num_sections * COFF_SECTION_HEADER_SIZE;
    for( i = 0; i < c_file->header.num_sections; i++ ) {
        c_file->sections[i].rawdata_ptr = d_ptr;
        d_ptr += c_file->sections[i].size;
        c_file->sections[i].reloc_ptr = d_ptr;
        d_ptr += c_file->sections[i].num_relocs * COFF_RELOC_SIZE;
    }
    c_file->header.sym_table = d_ptr;
    LibWrite( io, &( c_file->header ), COFF_FILE_HEADER_SIZE );
}

static void WriteCoffSections( libfile io, coff_lib_file *c_file )
{
    LibWrite( io, c_file->sections, c_file->header.num_sections * COFF_SECTION_HEADER_SIZE );
}

static void WriteCoffSymbols( libfile io, coff_lib_file *c_file )
{
    LibWrite( io, c_file->symbols, c_file->header.num_symbols * COFF_SYM_SIZE );
}

static void WriteCoffReloc( libfile io, unsigned_32 offset, unsigned_32 sym_tab_index, unsigned_16 type )
{
    //output is buffered so no point in putting COFF_RELOC struct
    LibWrite( io, &offset, sizeof( offset ) );
    LibWrite( io, &sym_tab_index, sizeof( sym_tab_index ) );
    LibWrite( io, &type, sizeof( type ) );
}

static void WriteCoffStringTable( libfile io, coff_lib_file *c_file )
{
    c_file->string_table_size += 4;
    LibWrite( io, &( c_file->string_table_size ), 4 );
    LibWrite( io, c_file->string_table, c_file->string_table_size - 4 );
}

static void WriteCoffOptHeader( libfile io, sym_file *sfile )
{
    coff_opt_hdr64  _opt_hdr;
    union {
        coff_opt_hdr64  *h64;
        coff_opt_hdr32  *h32;
    } opt_hdr;

    opt_hdr.h64 = &_opt_hdr;
    memset( opt_hdr.h64, 0, sizeof( *opt_hdr.h64 ) );
    switch( sfile->import->processor ) {
    case WL_PROC_X64:
        opt_hdr.h64->magic = COFF_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
        opt_hdr.h64->l_major = 2;
        opt_hdr.h64->l_minor = 0x32;
        opt_hdr.h64->section_align = 0x1000;
        opt_hdr.h64->file_align = 0x0200;
        opt_hdr.h64->os_major = 1;
        opt_hdr.h64->stack_reserve.u._32[I64LO32] = 0x00100000;
        opt_hdr.h64->stack_reserve.u._32[I64HI32] = 0;
        opt_hdr.h64->stack_commit.u._32[I64LO32]  = 0x00001000;
        opt_hdr.h64->stack_commit.u._32[I64HI32]  = 0;
        opt_hdr.h64->heap_reserve.u._32[I64LO32]  = 0x00100000;
        opt_hdr.h64->heap_reserve.u._32[I64HI32]  = 0;
        opt_hdr.h64->heap_commit.u._32[I64LO32]   = 0x00001000;
        opt_hdr.h64->heap_commit.u._32[I64HI32]   = 0;
        opt_hdr.h64->data_directories = 0x10;
        break;
    case WL_PROC_AXP:
    case WL_PROC_MIPS:
    case WL_PROC_PPC:
    case WL_PROC_X86:
    default:
        opt_hdr.h32->magic = COFF_IMAGE_NT_OPTIONAL_HDR32_MAGIC;
        opt_hdr.h32->l_major = 2;
        opt_hdr.h32->l_minor = 0x32;
        opt_hdr.h32->section_align = 0x1000;
        opt_hdr.h32->file_align = 0x0200;
        opt_hdr.h32->os_major = 1;
        opt_hdr.h32->stack_reserve = 0x00100000;
        opt_hdr.h32->stack_commit  = 0x00001000;
        opt_hdr.h32->heap_reserve  = 0x00100000;
        opt_hdr.h32->heap_commit   = 0x00001000;
        opt_hdr.h32->data_directories = 0x10;
        break;
    }
    switch( sfile->import->processor ) {
    case WL_PROC_PPC:
        opt_hdr.h32->l_minor = 0x3c;
        break;
    case WL_PROC_AXP:
    case WL_PROC_MIPS:
    case WL_PROC_X64:
    case WL_PROC_X86:
    default:
        break;
    }
    if( sfile->import->processor == WL_PROC_X64 ) {
        LibWrite( io, opt_hdr.h64, sizeof( *opt_hdr.h64 ) );
    } else {
        LibWrite( io, opt_hdr.h32, sizeof( *opt_hdr.h32 ) );
    }
}

static unsigned AddCoffSymbolWithPrefix( coff_lib_file *c_file, unsigned_16 sec_num, const char *prefix, name_len *modName, unsigned_16 type )
{
    char *buffer;
    size_t len;

    len = strlen( prefix );
    buffer = alloca( len + modName->len + 1 );
    strcpy( strcpy( buffer, prefix ) + len, modName->name );
    return( AddCoffSymbol( c_file, sec_num, buffer, 0x0, type, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 ) );
}

static unsigned AddCoffSymbolNullThunkData( coff_lib_file *c_file, unsigned_16 sec_num, name_len *modName )
{
    char *buffer;

    buffer = alloca( 1 + modName->len + sizeof( "_NULL_THUNK_DATA" ) );
    buffer[0] = 0x7f;
    strcpy( strcpy( buffer + 1, modName->name ) + modName->len, "_NULL_THUNK_DATA" );
    return( AddCoffSymbol( c_file, sec_num, buffer, 0x0, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 ) );
}

static void WriteImportDescriptor( libfile io, sym_file *sfile, coff_lib_file *c_file, name_len *modName, name_len *dllName, bool long_format )
{
    unsigned_16     type;
    char            nulls[0x14];
    signed_16       sec_num;
    unsigned        symb_dllname;
    unsigned        symb_origthunk;
    unsigned        symb_thunk;

    /*
     * section no. 1
     */
    sec_num = AddCoffSection( c_file, ".idata$2", 0x14, 3, COFF_IMAGE_SCN_ALIGN_4BYTES
        | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_WRITE );
    AddCoffSymbolWithPrefix( c_file, sec_num, "__IMPORT_DESCRIPTOR_", modName, COFF_IMAGE_SYM_TYPE_NULL );
    AddCoffSymbol( c_file, sec_num, ".idata$2", 0xC0000040, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_SECTION, 0 );
    /*
     * section no. 2
     */
    sec_num = AddCoffSection( c_file, ".idata$6", __ROUND_UP_SIZE_EVEN( dllName->len + 1 ), 0, COFF_IMAGE_SCN_ALIGN_2BYTES
        | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_WRITE );
    symb_dllname = AddCoffSymbol( c_file, sec_num, ".idata$6", 0x0, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_STATIC, 0 );
    symb_origthunk = AddCoffSymbol( c_file, 0, ".idata$4", 0xC0000040, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_SECTION, 0 );
    symb_thunk = AddCoffSymbol( c_file, 0, ".idata$5", 0xC0000040, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_SECTION, 0 );
    AddCoffSymbol( c_file, 0, "__NULL_IMPORT_DESCRIPTOR", 0x0, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
    AddCoffSymbolNullThunkData( c_file, 0, modName );
    /*
     * write data
     */
    if( long_format ) {
        if( sfile->import->processor == WL_PROC_X64 ) {
            c_file->header.opt_hdr_size = (unsigned_16)sizeof( coff_opt_hdr64 );
        } else {
            c_file->header.opt_hdr_size = (unsigned_16)sizeof( coff_opt_hdr32 );
        }
    }
    WriteCoffFileHeader( io, c_file );
    if( long_format ) {
        WriteCoffOptHeader( io, sfile );
    }
    WriteCoffSections( io, c_file );
    /*
     * write data section no. 1
     */
    switch( sfile->import->processor ) {
    case WL_PROC_AXP:
        type = COFF_IMAGE_REL_ALPHA_REFLONGNB;
        break;
    case WL_PROC_MIPS:
        type = COFF_IMAGE_REL_MIPS_REFWORDNB;
        break;
    case WL_PROC_PPC:
        type = COFF_IMAGE_REL_PPC_IFGLUE;
        break;
    case WL_PROC_X64:
        type = COFF_IMAGE_REL_AMD64_ADDR32NB;
        break;
    case WL_PROC_X86:
    default:
        type = COFF_IMAGE_REL_I386_DIR32NB;
        break;
    }
    memset( nulls, 0, sizeof( nulls ) );
    LibWrite( io, nulls, 0x14 );
    WriteCoffReloc( io, 0x000c, symb_dllname, type );
    WriteCoffReloc( io, 0x0000, symb_origthunk, type );
    WriteCoffReloc( io, 0x0010, symb_thunk, type );
    /*
     * write data section no. 2
     */
    WriteStringPadding( io, dllName->name, dllName->len + 1 );
    WriteCoffSymbols( io, c_file );
    WriteCoffStringTable( io, c_file );
}

static void WriteNullImportDescriptor( libfile io, coff_lib_file *c_file )
{
    char        nulls[0x14];
    signed_16   sec_num;

    /*
     * section no. 1
     */
    sec_num = AddCoffSection( c_file, ".idata$3", 0x14, 0, COFF_IMAGE_SCN_ALIGN_4BYTES
        | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_WRITE );
    AddCoffSymbol( c_file, sec_num, "__NULL_IMPORT_DESCRIPTOR", 0x0, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
    /*
     * write data
     */
    WriteCoffFileHeader( io, c_file );
    WriteCoffSections( io, c_file );
    /*
     * write data section no. 1
     */
    memset( nulls, 0 , sizeof( nulls ) );
    LibWrite( io, nulls, 0x14 );
    WriteCoffSymbols( io, c_file );
    WriteCoffStringTable( io, c_file );
}

static void WriteNullThunkData( libfile io, sym_file *sfile, coff_lib_file *c_file, name_len *modName )
{
    char            nulls[8];
    signed_16       sec_num;
    unsigned_32     thunk_section_align;
    unsigned        thunk_size;

    thunk_size = 4;
    thunk_section_align = COFF_IMAGE_SCN_ALIGN_4BYTES;
    if( sfile->import->processor == WL_PROC_X64 ) {
        thunk_size = 8;
        thunk_section_align = COFF_IMAGE_SCN_ALIGN_8BYTES;
    }
    /*
     * section no. 1
     */
    sec_num = AddCoffSection( c_file, ".idata$5", thunk_size, 0, thunk_section_align
        | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_WRITE );
    AddCoffSymbolNullThunkData( c_file, sec_num, modName );
    /*
     * section no. 2
     */
    sec_num = AddCoffSection( c_file, ".idata$4", thunk_size, 0, thunk_section_align
        | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_WRITE );
    /*
     * write data
     */
    WriteCoffFileHeader( io, c_file );
    WriteCoffSections( io, c_file );
    /*
     * write data section no. 1 + 2
     */
    memset( nulls, 0 , sizeof( nulls ) );
    LibWrite( io, nulls, 8 );
    WriteCoffSymbols( io, c_file );
    WriteCoffStringTable( io, c_file );
}

static void WriteShortImportEntry( libfile io, sym_file *sfile, name_len *symName, name_len *dllName )
/*****************************************************************************************************
 * write short format entry
 */
{
    coff_import_object_header   obj_hdr;

    obj_hdr.sig1 = COFF_IMPORT_OBJECT_HDR_SIG1;
    obj_hdr.sig2 = COFF_IMPORT_OBJECT_HDR_SIG2;
    obj_hdr.version = 0;
    switch( sfile->import->processor ) {
    case WL_PROC_AXP:
        obj_hdr.machine = COFF_IMAGE_FILE_MACHINE_ALPHA;
        break;
    case WL_PROC_MIPS:
        obj_hdr.machine = COFF_IMAGE_FILE_MACHINE_R4000;
        break;
    case WL_PROC_PPC:
        obj_hdr.machine = COFF_IMAGE_FILE_MACHINE_POWERPC;
        break;
    case WL_PROC_X64:
        obj_hdr.machine = COFF_IMAGE_FILE_MACHINE_AMD64;
        break;
    case WL_PROC_X86:
    default:
        obj_hdr.machine = COFF_IMAGE_FILE_MACHINE_I386;
        break;
    }
    obj_hdr.time_date_stamp = sfile->arch.date;
    obj_hdr.object_type = COFF_IMPORT_OBJECT_CODE;
    obj_hdr.reserved = 0;
    if( sfile->import->type == NAMED ) {
        obj_hdr.oh.hint = 0;
        if( sfile->import->processor == WL_PROC_X86 ) {
            obj_hdr.name_type = COFF_IMPORT_OBJECT_NAME_UNDECORATE;
        } else {
            obj_hdr.name_type = COFF_IMPORT_OBJECT_NAME;
        }
    } else {
        obj_hdr.oh.ordinal = sfile->import->u.sym.ordinal;
        obj_hdr.name_type = COFF_IMPORT_OBJECT_ORDINAL;
    }
    obj_hdr.size_of_data = symName->len + 1 + dllName->len + 1;
    LibWrite( io, &obj_hdr, sizeof( obj_hdr ) );
    LibWrite( io, symName->name, symName->len + 1 );
    LibWrite( io, dllName->name, dllName->len + 1 );
}

static void WriteCoffImportTablesNamed( libfile io, sym_file *sfile, unsigned symb_hints )
{
    char        nulls[8];
    unsigned    thunk_size;
    unsigned_16 type;

    switch( sfile->import->processor ) {
    case WL_PROC_AXP:
        type = COFF_IMAGE_REL_ALPHA_REFLONGNB;
        break;
    case WL_PROC_MIPS:
        type = COFF_IMAGE_REL_MIPS_REFWORDNB;
        break;
    case WL_PROC_PPC:
        type = COFF_IMAGE_REL_PPC_ADDR32NB;
        break;
    case WL_PROC_X64:
        type = COFF_IMAGE_REL_AMD64_ADDR32NB;
        break;
    default:
    case WL_PROC_X86:
        type = COFF_IMAGE_REL_I386_DIR32NB;
        break;
    }
    thunk_size = 4;
    if( sfile->import->processor == WL_PROC_X64 ) {
        thunk_size = 8;
    }
    memset( nulls, 0, sizeof( nulls ) );
    /*
     * write data section no. 2(4)
     */
    LibWrite( io, nulls, thunk_size );
    WriteCoffReloc( io, 0x0000, symb_hints, type );
    /*
     * write data section no. 3(5)
     */
    LibWrite( io, nulls, thunk_size );
    WriteCoffReloc( io, 0x0000, symb_hints, type );
}

static void WriteCoffImportTablesOrdinal( libfile io, sym_file *sfile )
{
    unsigned_64 import_table_entry;

    switch( sfile->import->processor ) {
    case WL_PROC_X64:
        import_table_entry.u._64[0] = sfile->import->u.sym.ordinal | COFF_IMAGE_ORDINAL_FLAG64;
        /*
         * write data section no. 2(4)
         */
        LibWrite( io, &import_table_entry.u._64[0], 8 );
        /*
         * write data section no. 3(5)
         */
        LibWrite( io, &import_table_entry.u._64[0], 8 );
        return;
    case WL_PROC_AXP:
    case WL_PROC_MIPS:
    case WL_PROC_PPC:
    case WL_PROC_X86:
    default:
        break;
    }
    import_table_entry.u._32[0] = sfile->import->u.sym.ordinal | COFF_IMAGE_ORDINAL_FLAG32;
    /*
     * write data section no. 2(4)
     */
    LibWrite( io, &import_table_entry.u._32[0], 4 );
    /*
     * write data section no. 3(5)
     */
    LibWrite( io, &import_table_entry.u._32[0], 4 );
}

static void WriteLongImportEntry( libfile io, sym_file *sfile, coff_lib_file *c_file,
    name_len *symName, name_len *modName, name_len *exportedName )
/**************************************************************************************
 * write long format entry
 */
{
    signed_16       sec_num;
    char            nulls[8];
    unsigned_16     ordinal;
    unsigned_32     thunk_section_align;
    unsigned        symb_name;
    unsigned        symb_imp_name;
    unsigned        symb_toc;
    unsigned        symb_hints;
    unsigned        thunk_size;
    bool            is_named;

    thunk_size = 4;
    thunk_section_align = COFF_IMAGE_SCN_ALIGN_4BYTES;
    if( sfile->import->processor == WL_PROC_X64 ) {
        thunk_size = 8;
        thunk_section_align = COFF_IMAGE_SCN_ALIGN_8BYTES;
    }
    is_named = ( sfile->import->type == NAMED );
    /*
     * section no. 1
     */
    switch( sfile->import->processor ) {
    case WL_PROC_AXP:
        sec_num = AddCoffSection( c_file, ".text", sizeof( CoffImportAxpText ), 3, COFF_IMAGE_SCN_ALIGN_16BYTES
            | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_CODE
            | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_EXECUTE );
        break;
    case WL_PROC_MIPS:
        sec_num = AddCoffSection( c_file, ".text", sizeof( CoffImportMipsText ), 3, COFF_IMAGE_SCN_ALIGN_16BYTES
            | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_CODE
            | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_EXECUTE );
        break;
    case WL_PROC_PPC:
        sec_num = AddCoffSection( c_file, ".text", sizeof( CoffImportPpcText ), 1, COFF_IMAGE_SCN_ALIGN_4BYTES
            | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_CODE
            | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_EXECUTE );
        break;
    case WL_PROC_X64:
        sec_num = AddCoffSection( c_file, ".text", sizeof( CoffImportX64Text ), 1, COFF_IMAGE_SCN_ALIGN_2BYTES
            | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_CODE
            | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_EXECUTE );
        break;
    default:
        sfile->import->processor = WL_PROC_X86;
        /* fall through */
    case WL_PROC_X86:
        sec_num = AddCoffSection( c_file, ".text", sizeof( CoffImportX86Text ), 1, COFF_IMAGE_SCN_ALIGN_2BYTES
            | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_CODE
            | COFF_IMAGE_SCN_MEM_READ | COFF_IMAGE_SCN_MEM_EXECUTE );
        break;
    }
    AddCoffSymSec( c_file, sec_num, COFF_IMAGE_COMDAT_SELECT_NODUPLICATES );
    if( sfile->import->processor == WL_PROC_PPC ) {
        symb_name = AddCoffSymbolWithPrefix( c_file, sec_num, "..", symName, COFF_IMAGE_SYM_TYPE_FUNCTION );
    } else {
        symb_name = AddCoffSymbol( c_file, sec_num, symName->name, 0x0, COFF_IMAGE_SYM_TYPE_FUNCTION, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
    }
    if( sfile->import->processor == WL_PROC_PPC ) {
        /*
         * section no. 2
         */
        sec_num = AddCoffSection( c_file, ".pdata", sizeof( CoffImportPpcPdata ), 4, COFF_IMAGE_SCN_ALIGN_1BYTES
            | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA
            | COFF_IMAGE_SCN_MEM_READ );
        AddCoffSymSec( c_file, sec_num, COFF_IMAGE_COMDAT_SELECT_ASSOCIATIVE );
        /*
         * section no. 3
         */
        sec_num = AddCoffSection( c_file, ".reldata", 0x8, 2, COFF_IMAGE_SCN_ALIGN_8BYTES
            | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA
            | COFF_IMAGE_SCN_MEM_READ |  COFF_IMAGE_SCN_MEM_WRITE );
        if( is_named ) {
            AddCoffSymSec( c_file, sec_num, COFF_IMAGE_COMDAT_SELECT_NODUPLICATES );
        }
        AddCoffSymSec( c_file, sec_num, COFF_IMAGE_COMDAT_SELECT_NODUPLICATES );
        AddCoffSymbol( c_file, sec_num, symName->name, 0x0, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
    }
    /*
     * section no. 2(4)
     */
    sec_num = AddCoffSection( c_file, ".idata$5", thunk_size, ( is_named ) ? 1 : 0, thunk_section_align
        | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA
        | COFF_IMAGE_SCN_MEM_READ |  COFF_IMAGE_SCN_MEM_WRITE );
    AddCoffSymSec( c_file, sec_num, COFF_IMAGE_COMDAT_SELECT_NODUPLICATES );
    symb_imp_name = AddCoffSymbolWithPrefix( c_file, sec_num, "__imp_", symName, COFF_IMAGE_SYM_TYPE_NULL );
    /*
     * section no. 3(5)
     */
    sec_num = AddCoffSection( c_file, ".idata$4", thunk_size, ( is_named ) ? 1 : 0, thunk_section_align
        | COFF_IMAGE_SCN_LNK_COMDAT | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA
        | COFF_IMAGE_SCN_MEM_READ |  COFF_IMAGE_SCN_MEM_WRITE );
    AddCoffSymSec( c_file, sec_num, COFF_IMAGE_COMDAT_SELECT_ASSOCIATIVE );
    if( is_named ) {
        /*
         * section no. 4(6)
         */
        sec_num = AddCoffSection( c_file, ".idata$6", sizeof( ordinal ) + __ROUND_UP_SIZE_EVEN( exportedName->len + 1 ),
            0, COFF_IMAGE_SCN_ALIGN_2BYTES | COFF_IMAGE_SCN_LNK_COMDAT
            | COFF_IMAGE_SCN_CNT_INITIALIZED_DATA | COFF_IMAGE_SCN_MEM_READ
            | COFF_IMAGE_SCN_MEM_WRITE );
        symb_hints = AddCoffSymSec( c_file, sec_num, COFF_IMAGE_COMDAT_SELECT_ASSOCIATIVE );
    }
    symb_toc = 0;
    if( sfile->import->processor == WL_PROC_PPC ) {
        symb_toc = AddCoffSymbol( c_file, 0, ".toc", 0x0, COFF_IMAGE_SYM_TYPE_NULL, COFF_IMAGE_SYM_CLASS_EXTERNAL, 0 );
    }
    AddCoffSymbolWithPrefix( c_file, 0, "__IMPORT_DESCRIPTOR_", modName, COFF_IMAGE_SYM_TYPE_NULL );
    /*
     * write data
     */
    WriteCoffFileHeader( io, c_file );
    WriteCoffSections( io, c_file );
    switch( sfile->import->processor ) {
    case WL_PROC_AXP:
        /*
         * write data section no. 1
         */
        LibWrite( io, CoffImportAxpText, sizeof( CoffImportAxpText ) );
        WriteCoffReloc( io, 0x0000, symb_imp_name, COFF_IMAGE_REL_ALPHA_REFHI );
        WriteCoffReloc( io, 0x0000, symb_name, COFF_IMAGE_REL_ALPHA_PAIR );
        WriteCoffReloc( io, 0x0004, symb_imp_name, COFF_IMAGE_REL_ALPHA_REFLO );
        break;
    case WL_PROC_MIPS:
        /*
         * write data section no. 1
         */
        LibWrite( io, CoffImportMipsText, sizeof( CoffImportMipsText ) );
        WriteCoffReloc( io, 0x0000, symb_imp_name, COFF_IMAGE_REL_MIPS_REFHI );
        WriteCoffReloc( io, 0x0000, symb_name, COFF_IMAGE_REL_MIPS_PAIR );
        WriteCoffReloc( io, 0x0004, symb_imp_name, COFF_IMAGE_REL_MIPS_REFLO );
        break;
    case WL_PROC_PPC:
        /*
         * write data section no. 1
         */
        LibWrite( io, CoffImportPpcText, sizeof( CoffImportPpcText ) );
        WriteCoffReloc( io, 0x0000, symb_imp_name, COFF_IMAGE_REL_PPC_TOCREL14 | COFF_IMAGE_REL_PPC_TOCDEFN );
        /*
         * write data section no. 2
         */
        LibWrite( io, CoffImportPpcPdata, sizeof( CoffImportPpcPdata ) );
        WriteCoffReloc( io, 0x0000, symb_name, COFF_IMAGE_REL_PPC_ADDR32 );
        WriteCoffReloc( io, 0x0004, symb_name, COFF_IMAGE_REL_PPC_ADDR32 );
        WriteCoffReloc( io, 0x0010, symb_name, COFF_IMAGE_REL_PPC_ADDR32 );
        WriteCoffReloc( io, 0x80410004, symb_name, COFF_IMAGE_REL_PPC_SECTION );
        /*
         * write data section no. 3
         */
        memset( nulls, 0, sizeof( nulls ) );
        LibWrite( io, nulls, 8 );
        WriteCoffReloc( io, 0x0000, symb_name, COFF_IMAGE_REL_PPC_ADDR32 );
        WriteCoffReloc( io, 0x0004, symb_toc, COFF_IMAGE_REL_PPC_ADDR32 );
        break;
    case WL_PROC_X64:
        /*
         * write data section no. 1
         */
        LibWrite( io, CoffImportX64Text, sizeof( CoffImportX64Text ) );
        WriteCoffReloc( io, 0x0002, symb_imp_name, COFF_IMAGE_REL_AMD64_ADDR32 );
        break;
    default:
        sfile->import->processor = WL_PROC_X86;
        /* fall through */
    case WL_PROC_X86:
        /*
         * write data section no. 1
         */
        LibWrite( io, CoffImportX86Text, sizeof( CoffImportX86Text ) );
        WriteCoffReloc( io, 0x0002, symb_imp_name, COFF_IMAGE_REL_I386_DIR32 );
        break;
    }
    /*
     * write data section no. 2(4) and 3(5)
     */
    if( is_named ) {
        WriteCoffImportTablesNamed( io, sfile, symb_hints );
    } else {
        WriteCoffImportTablesOrdinal( io, sfile );
    }
    if( is_named ) {
        /*
         * write data section no. 4(6)
         */
        ordinal = sfile->import->u.sym.ordinal;
        LibWrite( io, &ordinal, sizeof( ordinal ) );
        WriteStringPadding( io, exportedName->name, exportedName->len + 1 );
    }
    WriteCoffSymbols( io, c_file );
    WriteCoffStringTable( io, c_file );
}

void CoffWriteImport( libfile io, sym_file *sfile, bool long_format )
/*******************************************************************/
{
    coff_lib_file               c_file;
    name_len                    dllName;
    name_len                    symName;
    name_len                    exportedName;
    name_len                    modName;

    /*
     * We are being extremely cautious in the following lines of code
     * up to the switch statement.
     */
    dllName.len = 0;
    dllName.name = sfile->import->DLLName;
    if( dllName.name != NULL ) {
        dllName.len = strlen( dllName.name );
    }
    modName.len = 0;
    modName.name = DupStr( MakeFName( sfile->import->DLLName ) );
    if( modName.name != NULL ) {
        modName.len = strlen( modName.name );
    }
    symName.len = 0;
    symName.name = sfile->import->u.sym.symName;
    if( symName.name != NULL ) {
        symName.len = strlen( symName.name );
    }
    exportedName.len = 0;
    exportedName.name = sfile->import->u.sym.exportedName;
    if( exportedName.name == NULL ) {
        exportedName.name = symName.name; // use this instead
    }
    if( exportedName.name != NULL ) {
        exportedName.len = strlen( exportedName.name );
    }
    switch( sfile->import->type ) {
    case IMPORT_DESCRIPTOR:
        if( modName.len == 0 )
            FatalError( ERR_CANT_DO_IMPORT, "AR", "NO DLL NAME" );
        break;
    case NULL_THUNK_DATA:
        if( modName.len == 0 )
            FatalError( ERR_CANT_DO_IMPORT, "AR", "NO DLL NAME" );
        break;
    case ORDINAL:
    case NAMED:
        if( dllName.len == 0 )
            FatalError( ERR_CANT_DO_IMPORT, "AR", "NO DLL NAME" );
        if( symName.len == 0 )
            FatalError( ERR_CANT_DO_IMPORT, "AR", "NO SYMBOL NAME" );
        if( exportedName.len == 0 && sfile->import->type == NAMED ) {
            FatalError( ERR_CANT_DO_IMPORT, "AR", "NO EXPORTED NAME" );
        }
        break;
    case NULL_IMPORT_DESCRIPTOR:
    default:
        break;
    }
    InitCoffFile( &c_file );
    SetCoffFile( &c_file, sfile );
    switch( sfile->import->type ) {
    case IMPORT_DESCRIPTOR:
        WriteImportDescriptor( io, sfile, &c_file, &modName, &dllName, long_format );
        break;
    case NULL_IMPORT_DESCRIPTOR:
        WriteNullImportDescriptor( io, &c_file );
        break;
    case NULL_THUNK_DATA:
        WriteNullThunkData( io, sfile, &c_file, &modName );
        break;
    case ORDINAL:
    case NAMED:
        if( long_format ) {
            WriteLongImportEntry( io, sfile, &c_file, &symName, &modName, &exportedName );
        } else {
            WriteShortImportEntry( io, sfile, &symName, &dllName );
        }
        break;
    default:
        break;
    }
    FiniCoffLibFile( &c_file );
    MemFree( modName.name );
}
