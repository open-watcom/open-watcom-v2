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


#include "owlpriv.h"
#include "owreloc.h"
#include <time.h>
#include <string.h>

#define FIRST_USER_SECTION              0

// must correspond to owl_cpu enums in owl.h - first entry is for POWER PC
static uint_16 cpuTypes[] = {
    IMAGE_FILE_MACHINE_POWERPC,
    IMAGE_FILE_MACHINE_ALPHA,
    IMAGE_FILE_MACHINE_R4000 };

#define _OWLIndexToCOFFIndex( x )       ( (x) + FIRST_USER_SECTION )
#define _OWLStringIndexToCOFFStringIndex( x )   ( (x) + 4 )

#define _FuncHasDebugInfo( y )          ( (y)->x.func != NULL )

static unsigned numFuncAuxSymbols( owl_func_info *info ){
//*******************************************************
    owl_func_file  *curr;
    unsigned        num_aux_syms;
    unsigned        file_aux_syms;

    num_aux_syms = 0;
    curr = info->head;
    while( curr != NULL ){
        num_aux_syms += 2; //.file  .lf
        file_aux_syms = strlen( curr->name ) + sizeof( coff_symbol ) - 1;
        file_aux_syms /= sizeof( coff_symbol );
        num_aux_syms += file_aux_syms;
        curr = curr->next;
    }
    return( num_aux_syms );
}

static unsigned numAuxSymbols( owl_symbol_handle sym) {
//*****************************************************

    unsigned            num_aux_syms;

    num_aux_syms = 0;
    switch( sym->type ) {
    case OWL_TYPE_SECTION:
        num_aux_syms = 1;
        break;
    case OWL_TYPE_FILE:
        num_aux_syms = strlen( OWLStringText( sym->name ) ) + sizeof( coff_symbol ) - 1;
        num_aux_syms /= sizeof( coff_symbol );
        break;
    case OWL_TYPE_FUNCTION:
        if( sym->linkage != OWL_SYM_UNDEFINED ) {
            owl_func_info *info;

            info = sym->x.func;
            if( info != NULL ) {
               // for .bf+aux, .lf .ef+aux
               num_aux_syms = 6;
               num_aux_syms += numFuncAuxSymbols( info );// .file + *aux + .lf
            }
        }
        break;
    case OWL_TYPE_OBJECT:
        if( sym->linkage == OWL_SYM_WEAK ){
            num_aux_syms = 1;
        }
        break;
    }
    return( num_aux_syms );
}

static int symComdat( owl_symbol_handle sym ) {
//*********************************************

    if( sym->section != NULL ) {
        if( sym->section->comdat_sym == sym ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static owl_symbol_index symRelocRefIndex( owl_symbol_handle sym ) {
//*****************************************************************

    if( symComdat( sym ) ) {
        return( sym->comdat );
    }
    return( sym->index );
}

static void calcNumSymbols( owl_file_handle file ) {
//**************************************************

    unsigned            num_syms;
    owl_symbol_handle   sym;

    num_syms = 0;
    for( sym = file->symbol_table->head; sym != NULL; sym = sym->next ) {
        if( sym->flags & OWL_SYM_DEAD ) continue;
        sym->index = num_syms;
        num_syms += numAuxSymbols( sym ) + 1;
        if( symComdat( sym ) ) {
            sym->comdat = num_syms;
            num_syms += 1;
        }
    }
    file->num_symbols = num_syms;
}

static void calcLineNumSymbolIndices( owl_file_handle file ) {
//************************************************************

    owl_symbol_handle   sym;

    for( sym = file->symbol_table->head; sym != NULL; sym = sym->next ) {
        if( sym->flags & OWL_SYM_DEAD ) continue;
        if( sym->type != OWL_TYPE_FUNCTION ) continue;
        if( _FuncHasDebugInfo( sym ) ) {
            // Symbol has some sort of debugging info
            //      - patch up index in first linenum record
            assert( sym->section != NULL );
            assert( sym->section->linenum_buffer != NULL );
            OWLBufferSeek( sym->section->linenum_buffer, sym->x.func->linenum_offset );
            OWLBufferWrite( sym->section->linenum_buffer, (const char *)&sym->index, sizeof( sym->index ) );
        }
    }
}

static unsigned numSymbols( owl_file_handle file ) {
//**************************************************

    return( file->num_symbols );
}

static unsigned numSections( owl_file_handle file ) {
//***************************************************

    return( _OWLIndexToCOFFIndex( file->next_index ) );
}

static unsigned numRelocs( owl_section_handle section ) {
//*******************************************************

    return( section->num_relocs );
}

static unsigned numLineNums( owl_section_handle section ) {
//*********************************************************

    return( section->num_linenums );
}

static coff_offset sectionFileSize( owl_section_handle section ) {
//****************************************************************

    if( section->type & OWL_SEC_ATTR_BSS ) return( 0 );
    assert( section->size == OWLBufferSize( section->buffer ) );
    return( section->size );
}

static coff_offset sectionVirtSize( owl_section_handle section ) {
//****************************************************************

    return( section->size );
}

static void emitFileHeader( owl_file_handle file ) {
//**************************************************

    coff_file_header    header;

    header.cpu_type = cpuTypes[ file->info->cpu ];
    header.num_sections = numSections( file );
    header.time_stamp = time( NULL );
    header.sym_table = file->x.coff.symbol_table_offset;
    header.num_symbols = numSymbols( file );
    header.opt_hdr_size = 0;
//  header.flags = IMAGE_FILE_BYTES_REVERSED_LO | IMAGE_FILE_32BIT_MACHINE;
    header.flags = 0;
    if( file->type == OWL_FILE_EXE || file->type == OWL_FILE_DLL ) {
        header.flags |= IMAGE_FILE_EXECUTABLE_IMAGE;
    }
    _ClientWrite( file, (const char *)&header, sizeof( header ) );
}

static void formatSectionName( owl_section_handle section, char *buffer ) {
//*************************************************************************

    coff_offset         name_offset;
    char                name[ 8 ];
    char                *curr;
    unsigned            count;

    // this name stuff is SO hokey....
#if 0  // mjc might fix right
    count = strlen( section->name->text );
    if( count <= 8 ){
        memcpy( buffer, section->name->text, count );
    }else{
#endif
        name_offset = _OWLStringIndexToCOFFStringIndex( OWLStringOffset( section->name ) );
        buffer[ 0 ] = '/';
        count = 0;
        curr = &name[ 7 ];
        while( name_offset != 0 ) {
            *curr-- = '0' + ( name_offset % 10 );
            name_offset /= 10;
            count++;
        }
        memcpy( &buffer[ 1 ], curr + 1, count );
#if 0
    }
#endif
}

static uint_32 sectionFlags( owl_section_type type ) {
//****************************************************

    uint_32 flags = 0;

    if( type & OWL_SEC_ATTR_CODE ) flags |= IMAGE_SCN_CNT_CODE;
    if( type & OWL_SEC_ATTR_DATA ) flags |= IMAGE_SCN_CNT_INITIALIZED_DATA;
    if( type & OWL_SEC_ATTR_BSS ) flags |= IMAGE_SCN_CNT_UNINITIALIZED_DATA;
    if( type & OWL_SEC_ATTR_INFO ) flags |= IMAGE_SCN_LNK_INFO;
    if( type & OWL_SEC_ATTR_DISCARDABLE ) flags |= IMAGE_SCN_MEM_DISCARDABLE;
    if( type & OWL_SEC_ATTR_REMOVE ) flags |= IMAGE_SCN_LNK_REMOVE;
    if( type & OWL_SEC_ATTR_COMDAT ) flags |= IMAGE_SCN_LNK_COMDAT;
    if( type & OWL_SEC_ATTR_PERM_READ ) flags |= IMAGE_SCN_MEM_READ;
    if( type & OWL_SEC_ATTR_PERM_WRITE ) flags |= IMAGE_SCN_MEM_WRITE;
    if( type & OWL_SEC_ATTR_PERM_EXEC ) flags |= IMAGE_SCN_MEM_EXECUTE;
    if( type & OWL_SEC_ATTR_PERM_SHARE ) flags |= IMAGE_SCN_MEM_SHARED;
    return( flags );
}

static uint_32 sectionAlignmentFlags( owl_section_info *section ) {
//*****************************************************************

    uint_32             flags;

    switch( section->align ) {
    case  1:
        flags = IMAGE_SCN_ALIGN_1BYTES;
        break;
    case  2:
        flags = IMAGE_SCN_ALIGN_2BYTES;
        break;
    case  4:
        flags = IMAGE_SCN_ALIGN_4BYTES;
        break;
    case  8:
        flags = IMAGE_SCN_ALIGN_8BYTES;
        break;
    case 16:
        flags = IMAGE_SCN_ALIGN_16BYTES;
        break;
    case 32:
        flags = IMAGE_SCN_ALIGN_32BYTES;
        break;
    case 64:
        flags = IMAGE_SCN_ALIGN_64BYTES;
        break;
    default:
        flags = 0;
        break;
    }
    return( flags );
}

static void formatSectionHeader( owl_section_handle section, coff_section_header *header ) {
//*******************************************************************************

    memset( header, 0, sizeof( coff_section_header ) );
    formatSectionName( section, &header->name[ 0 ] );
    header->size = sectionVirtSize( section );
    if( section->type & OWL_SEC_ATTR_BSS ) {
        header->rawdata_ptr = 0;
        header->reloc_ptr = 0;
        header->num_relocs = 0;
        assert( numRelocs( section ) == 0 );
    } else {
        header->rawdata_ptr = section->x.coff.section_offset;
        header->reloc_ptr = section->x.coff.relocs_offset;
        header->num_relocs = numRelocs( section );
    }
    header->virtsize = 0;
    header->offset = 0;
    header->lineno_ptr = section->x.coff.linenum_offset;
    header->num_lineno = section->num_linenums;
    header->flags = sectionFlags( section->type );
    header->flags |= sectionAlignmentFlags( section );
}

static void emitSectionHeaders( owl_file_handle file ) {
//******************************************************

    uint_32             section_table_size;
    coff_section_header *section_table;
    unsigned            count;
    owl_section_info    *curr;

    section_table_size = numSections( file ) * sizeof( coff_section_header );
    if( section_table_size != 0 ) {
        section_table = _ClientAlloc( file, section_table_size );
        count = 0;
        for( curr = file->sections; curr != NULL; curr = curr->next ) {
            formatSectionHeader( curr, &section_table[ curr->index ] );
            curr->index = count++;
        }
        _ClientWrite( file, (const char *)section_table, section_table_size );
        _ClientFree( file, section_table );
    }
}

static owl_offset sectionPadding( owl_section_handle section, owl_offset offset ) {
//*********************************************************************************

    owl_offset          mod;
    owl_offset          padding;

    padding = 0;
    if( section->align != 0 ) {
        mod = offset % section->align;
        if( mod != 0 ) {
            padding = section->align - mod;
        }
    }
    section->x.coff.alignment_padding = padding;
    return( padding );
}

static void calcSectionOffsets( owl_file_handle file ) {
//*******************************************************

    owl_offset          next;
    owl_section_handle  section;
    int                 nums;

    file->x.coff.header_table_offset = sizeof( coff_file_header );
    next = sizeof( coff_file_header ) +
                numSections( file ) * sizeof( coff_section_header );
    for( section = file->sections; section != NULL; section = section->next ) {
        next += sectionPadding( section, next );
        nums = sectionFileSize( section );
        if( nums == 0 ){
           section->x.coff.section_offset = 0;
        }else{
           section->x.coff.section_offset = next;
        }
        next += nums;
        nums = numRelocs( section );
        if( nums == 0 ){
            section->x.coff.relocs_offset = 0;
        }else{
            section->x.coff.relocs_offset = next;
        }
        next += nums * sizeof( coff_reloc );
        nums =  numLineNums( section );
        if( nums == 0 ){
            section->x.coff.linenum_offset = 0;
        }else{
            section->x.coff.linenum_offset = next;
        }
        next += nums * sizeof( coff_line_num );
    }
    file->x.coff.symbol_table_offset = next;
    next += numSymbols( file ) * sizeof( coff_symbol );
    file->x.coff.string_table_offset = next;
}

#define _CoffSectionIndex( x )          ( (x) + 1 )

// WARNING: Must correspond to stuff in owl_sym_type and owl_sym_linkage enums
static uint_8 complexType[] = {
        IMAGE_SYM_DTYPE_FUNCTION,
        IMAGE_SYM_DTYPE_NULL
};

static image_sym_class storageClass( owl_sym_linkage kind ) {
//***********************************************************

    image_sym_class ret;
    switch( kind ){
    case OWL_SYM_UNDEFINED:
        ret = IMAGE_SYM_CLASS_EXTERNAL;
        break;
    case OWL_SYM_FUNCTION:
        ret = IMAGE_SYM_CLASS_FUNCTION;
        break;
    case OWL_SYM_STATIC:
        ret = IMAGE_SYM_CLASS_STATIC;
        break;
    case OWL_SYM_GLOBAL:
        ret = IMAGE_SYM_CLASS_EXTERNAL;
        break;
    case OWL_SYM_WEAK:
        ret = IMAGE_SYM_CLASS_EXTERNAL;
        break;
    default:
        assert( 0 );
        ret = IMAGE_SYM_CLASS_EXTERNAL;
    }
    return( ret );
}



static void formatSectionSymbol( owl_symbol_info *symbol,
                                        _WCUNALIGNED coff_symbol *buffer) {
//*************************************************************************

    owl_section_handle                  section;
    _WCUNALIGNED coff_sym_section       *aux;

    section = symbol->x.section;
    buffer->name.non_name.zeros = 0;
    buffer->name.non_name.offset = _OWLStringIndexToCOFFStringIndex( OWLStringOffset( symbol->name ) );
    buffer->value = 0;
    buffer->sec_num = _CoffSectionIndex( section->index );
    buffer->type = IMAGE_SYM_DTYPE_NULL;
    buffer->storage_class = IMAGE_SYM_CLASS_STATIC;
    buffer->num_aux = 1;
    aux = (_WCUNALIGNED coff_sym_section *)( buffer + 1 );
    aux->length = sectionVirtSize( section );
    aux->num_relocs = numRelocs( section );
    aux->num_line_numbers = numLineNums( section );
    aux->checksum = 0;
    aux->number = 0;
    aux->selection = IMAGE_COMDAT_SELECT_UNKNOWN;
    if( _OwlSectionComdat( section ) ){
        if( section->comdat_dep != NULL ){
            aux->selection = IMAGE_COMDAT_SELECT_ASSOCIATIVE;
            aux->number = _CoffSectionIndex( section->comdat_dep->index );
        }else{
            aux->selection = IMAGE_COMDAT_SELECT_ANY;
        }
    }
}

_WCUNALIGNED uint_32     *lastFunc;
_WCUNALIGNED uint_32     *lastBf;
_WCUNALIGNED uint_32     *lastFile;

static unsigned doFileRecord( _WCUNALIGNED coff_symbol *buffer,
                                owl_symbol_index index, char const *name ) {
//**************************************************************************

    unsigned            num_aux;

    strcpy( buffer->name.name_string, ".file" );
    if( lastFile != NULL ) {
        *lastFile = index;
    }
    lastFile = &buffer->value;
    buffer->value = 0;
    buffer->sec_num = IMAGE_SYM_DEBUG;
    buffer->type = IMAGE_SYM_DTYPE_NULL;
    buffer->storage_class = IMAGE_SYM_CLASS_FILE;
    num_aux = strlen( name ) + sizeof( coff_symbol ) - 1;
    num_aux /= sizeof( coff_symbol );
    buffer->num_aux = num_aux;
    buffer += 1;
    memset( (char *)buffer, 0, num_aux * sizeof( coff_symbol ) );
    strcpy( (char *)buffer, name );
    return( 1+num_aux );
}

static void formatFileSymbol( owl_symbol_info *symbol,
                                _WCUNALIGNED coff_symbol *buffer ) {
//******************************************************************
    doFileRecord( buffer, symbol->index, OWLStringText( symbol->name )  );
}


static owl_offset functionLineNumOffset( owl_symbol_info *symbol ) {
//******************************************************************

    if( symbol->x.func == NULL ) return( 0 );
    return( symbol->section->x.coff.linenum_offset + symbol->x.func->linenum_offset );
}

static void doBfRecord( _WCUNALIGNED coff_symbol *buffer,
                        owl_symbol_info *symbol, owl_func_info *info ) {
//**********************************************************************

    _WCUNALIGNED coff_sym_bfef  *aux;

    strcpy( buffer->name.name_string, ".bf" );
    buffer->sec_num = _CoffSectionIndex( symbol->section->index );
    buffer->type = IMAGE_SYM_DTYPE_NULL;
    buffer->storage_class = IMAGE_SYM_CLASS_FUNCTION;
    buffer->num_aux = 1;
    aux = (_WCUNALIGNED coff_sym_bfef *)( buffer + 1 );
    aux->linenum = info->first_line;
    aux->next_func = 0;
    if( lastBf != NULL ) {
        *lastBf = symbol->index +2;
    }
    lastBf = &aux->next_func;
    buffer += 2;
}

static void doLfRecord( _WCUNALIGNED coff_symbol *buffer,
                                owl_symbol_index index, int num_lines) {
//**********************************************************************

    strcpy( buffer->name.name_string, ".lf" );
    buffer->value = num_lines;
    buffer->sec_num = _CoffSectionIndex( index );
    buffer->type = IMAGE_SYM_DTYPE_NULL;
    buffer->storage_class = IMAGE_SYM_CLASS_FUNCTION;
    buffer->num_aux = 0;
}

static void doEfRecord( _WCUNALIGNED coff_symbol *buffer,
                        owl_symbol_index index, owl_func_info *info ) {
//*********************************************************************

    _WCUNALIGNED coff_sym_bfef  *aux;

    strcpy( buffer->name.name_string, ".ef" );
    buffer->value = info->end - info->start;
    buffer->sec_num = _CoffSectionIndex( index );
    buffer->type = IMAGE_SYM_DTYPE_NULL;
    buffer->storage_class = IMAGE_SYM_CLASS_FUNCTION;
    buffer->num_aux = 1;
    aux = (_WCUNALIGNED coff_sym_bfef *)( buffer + 1 );
    aux->linenum = info->last_line;
    aux->next_func = 0;
}

static void formatFunctionSymbol( owl_symbol_info *symbol,
                                        _WCUNALIGNED coff_symbol *buffer) {
