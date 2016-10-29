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


#include <stdlib.h>
#include <string.h>
#include "wdismsg.h"
#include "disasm.h"
#include "orl.h"
#include "hashtabl.h"

#define SYMBOL_TO_TARGET_TABLE_SIZE             53
#define SYMBOL_TO_EXPORT_TABLE_SIZE             53
#define SECTION_TO_SEGMENT_TABLE_SIZE           29

typedef FILE            *file_handle;

typedef struct buf_list {
    struct buf_list     *next;
    char                buf[1];
} buf_list;

typedef struct {
    file_handle         hdl;
    buf_list            *buflist;
} buffer_info;

typedef struct section_entry {
    orl_sec_handle              shnd;
    struct section_entry        *next;
} section_entry;

typedef struct section_entry    *section_ptr;

typedef struct section_list {
    section_ptr         first;
    section_ptr         last;
} section_list;

bool                    UseORL;
char                    *ModNameORL;

orl_handle              ORLHnd;
orl_file_handle         ORLFileHnd;

// SymbolToTargetTable keeps all the symbols that the program imports
// (we might export that also). Table is queried with an orl_sym_handle.
hash_table              SymbolToTargetTable;

// Keeps track of all the symbols that the object exports.
hash_table              SymbolToExportTable;

// Translation table from an orl_sec_handle to its correspinding segment.
hash_table              SectionToSegmentTable;

static buffer_info      fileBuff;
static orl_sec_handle   symbolTable;
static section_list     relocSections;

static void *buffRead( void *file, size_t len )
//*********************************************
{
    buf_list    *buf;

    buf = AllocMem( len + sizeof( buf_list ) - 1 );
    if( fread( buf->buf, 1, len, file->hdl ) != len ) {
        FreeMem( buf );
        return NULL;
    }
    buf->next = ((buffer_info *)file)->buflist;
    ((buffer_info *)file)->buflist = buf;
    return( buf->buf );
}

static long buffSeek( void *file, long pos, int where )
//************************************************************
{
    return( fseek( ((buffer_info *)file)->hdl, pos, where ) );
}

static void initBuffer( buffer_info *file, file_handle hdl )
//**********************************************************
{
    file->hdl = hdl;
    file->buflist = NULL;
}

static void finiBuffer( buffer_info *file )
//*****************************************
{
    buf_list    *list;
    buf_list    *next;

    list = file->buflist;
    while( list != NULL ) {
        next = list->next;
        FreeMem( list );
        list = next;
    }
}

static int numberCmp( hash_value n1, hash_value n2 )
//**************************************************
{
    return( n1 - n2 );
}

static void createHashTables( void )
//**********************************
{
    SymbolToTargetTable = HashTableCreate( SYMBOL_TO_TARGET_TABLE_SIZE, HASH_NUMBER, numberCmp );
    if( !SymbolToTargetTable ) {
        SysError( ERR_OUT_OF_MEM, false );
    }
    SymbolToExportTable = HashTableCreate( SYMBOL_TO_EXPORT_TABLE_SIZE, HASH_NUMBER, numberCmp );
    if( !SymbolToExportTable ) {
        SysError( ERR_OUT_OF_MEM, false );
    }
    SectionToSegmentTable = HashTableCreate( SECTION_TO_SEGMENT_TABLE_SIZE, HASH_NUMBER, numberCmp );
    if( !SectionToSegmentTable ) {
        SysError( ERR_OUT_OF_MEM, false );
    }
}

static void destroyHashTables( void )
//***********************************
{
    if( SymbolToTargetTable ) HashTableFree( SymbolToTargetTable );
    if( SymbolToExportTable ) HashTableFree( SymbolToExportTable );
    if( SectionToSegmentTable ) HashTableFree( SectionToSegmentTable );
}

bool InitORL( void )
//******************
// Try and see if we will use ORL. Returns true if we'll use it.
{
    orl_file_flags      o_flags;
    orl_file_format     o_format;
    orl_machine_type    o_machine_type;
    ORLSetFuncs( orl_cli_funcs, buffRead, buffSeek, AllocMem, FreeMem );

    ORLFileHnd = NULL;
    ORLHnd = ORLInit( &orl_cli_funcs );
    if( !ORLHnd ) {
        SysError( ERR_OUT_OF_MEM, false );
    }

    initBuffer( &fileBuff, ObjFile );
    o_format = ORLFileIdentify( ORLHnd, &fileBuff );
    if( o_format != ORL_ELF && o_format != ORL_COFF ) {
        ORLFini( ORLHnd );
        finiBuffer( &fileBuff );
        ORLHnd = NULL;
        return( false );        // Will use ParseObjectOMF
    }

    ORLFileHnd = ORLFileInit( ORLHnd, &fileBuff, o_format );
    if( !ORLFileHnd ) {
        ORLFini( ORLHnd );
        finiBuffer( &fileBuff );
        SysError( ERR_OUT_OF_MEM, false );
    }
    o_machine_type = ORLFileGetMachineType( ORLFileHnd );
    if( o_machine_type != ORL_MACHINE_TYPE_I386 ) {
        FiniORL();
        Error( ERR_ORL_INV_MACHINE_TYPE, true );
        exit( 1 );
    }
    o_flags = ORLFileGetFlags( ORLFileHnd );
    if( !(o_flags & ORL_FILE_FLAG_LITTLE_ENDIAN) ) {
        FiniORL();
        Error( ERR_ORL_INV_BYTE_ORDER, true );
        exit( 1 );
    }
    UseORL = true;
    createHashTables();
    return( true );     // Success: will use ORL
}

void FiniORL( void )
//******************
{
    if( ORLFileHnd ) {
        ORLFileFini( ORLFileHnd );
        ORLFini( ORLHnd );
    }
    destroyHashTables();
    finiBuffer( &fileBuff );
}

static bool addRelocSection( orl_sec_handle shnd )
//************************************************
{
    section_ptr         sec;

    sec = AllocMem( sizeof( struct section_entry ) );
    if( sec ) {
        sec->shnd = shnd;
        sec->next = NULL;
        if( relocSections.first ) {
            relocSections.last->next = sec;
            relocSections.last = sec;
        } else {
            relocSections.first = sec;
            relocSections.last = sec;
        }
    } else {
        return( false );
    }
    return( true );
}

static void registerSegment( orl_sec_handle o_shnd )
//**************************************************
{
    orl_sec_flags               sec_flags;
    orl_sec_handle              reloc_section;
    orl_sec_alignment           alignment;
    char *                      content;
    int                         ctr;
    segment                     *seg;

    seg = NewSegment();
    seg->name = ORLSecGetName( o_shnd );
    seg->size = ORLSecGetSize( o_shnd );
    seg->start = 0;
    seg->use_32 = 1;            // only 32-bit object files use ORL
    seg->attr = ( 2 << 2 );     // (?) combine public
    alignment = ORLSecGetAlignment( o_shnd );
    // FIXME: Need better alignment translation.
    switch( alignment ) {
    case 0:
        seg->attr |= ( 1 << 5 ); break;
    case 1:
        seg->attr |= ( 2 << 5 ); break;
    case 3:
    case 4:
        seg->attr |= ( 3 << 5 ); break;
    case 8:
        seg->attr |= ( 4 << 5 ); break;
    case 2:
        seg->attr |= ( 5 << 5 ); break;
    case 12:
        seg->attr |= ( 6 << 5 ); break;
    default:
        // fprintf( stderr, "NOTE! 'Strange' alignment (%d) found. Using byte alignment.\n", alignment );
        seg->attr |= ( 1 << 5 ); break;
    }
    sec_flags = ORLSecGetFlags( o_shnd );
    if( !( sec_flags & ORL_SEC_FLAG_EXEC ) ) {
        seg->data_seg = true;
    }
    if( seg->size > 0 && ORLSecGetContents( o_shnd, &content ) == ORL_OKAY ) {
        Segment = seg;
        // Putting contents into segment struct.
        for( ctr = 0; ctr < seg->size; ctr++ ) {
            PutSegByte( ctr, content[ctr] );
        }
    }
    if( !HashTableInsert( SectionToSegmentTable, (hash_value)o_shnd, (hash_data)seg ) ) {
        SysError( ERR_OUT_OF_MEM, false );
    }
    reloc_section = ORLSecGetRelocTable( o_shnd );
    if( reloc_section ) {
        if( !addRelocSection( reloc_section ) ) {
            SysError( ERR_OUT_OF_MEM, false );
        }
    }
}

static orl_return processSection( orl_sec_handle o_shnd )
//*******************************************************
{
    orl_sec_type                sec_type;

    sec_type = ORLSecGetType( o_shnd );
    switch( sec_type ) {
    /* List the section types that you don't want to disassemble
    case ORL_SEC_TYPE_WHICHEVER_YOU_DO_NOT_WANT_LIKE_THE_ONES_BELOW:
    case ORL_SEC_TYPE_LINK_INFO:
    */
    case ORL_SEC_TYPE_STR_TABLE:
    case ORL_SEC_TYPE_RELOCS:
        // printf( "Skipping section: %s\n", ORLSecGetName( o_shnd ) );
        break;
    case ORL_SEC_TYPE_SYM_TABLE:
        symbolTable = o_shnd;
        break;
    default:
        registerSegment( o_shnd );
        break;
    }
    return( ORL_OKAY );
}

static import_sym *addExtDef( char *name, bool public, bool exported, export_sym *exp )
