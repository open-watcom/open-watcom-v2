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


#include "coffentr.h"
#include "coffflhn.h"
#include "cofflwlv.h"
#include "coffload.h"
#include "cofforl.h"
#include "orlhash.h"

coff_handle COFFENTRY CoffInit( orl_funcs *funcs )
{
    coff_handle                                 coff_hnd;

    coff_hnd = (coff_handle)ORL_CLI_ALLOC( funcs, sizeof( coff_handle_struct ) );
    if( !coff_hnd )
        return( NULL );
    coff_hnd->funcs = funcs;
    coff_hnd->first_file_hnd = NULL;
    return( coff_hnd );
}

orl_return COFFENTRY CoffFini( coff_handle coff_hnd )
{
    orl_return                                  error;

    while( coff_hnd->first_file_hnd != NULL ) {
        error = CoffRemoveFileLinks( coff_hnd->first_file_hnd );
        if( error != ORL_OKAY ) {
            return( error );
        }
    }
    ORL_PTR_FREE( coff_hnd, coff_hnd );
    return( ORL_OKAY );
}

orl_return COFFENTRY CoffFileInit( coff_handle coff_hnd, void *file, coff_file_handle *pcfh )
{
    coff_file_handle    coff_file_hnd;
    orl_return          error;

    coff_file_hnd = (coff_file_handle)ORL_PTR_ALLOC( coff_hnd, sizeof( coff_file_handle_struct ) );
    if( coff_file_hnd == NULL )
        return( ORL_OUT_OF_MEMORY );
    coff_file_hnd->coff_sec_hnd = NULL;
    coff_file_hnd->orig_sec_hnd = NULL;
    coff_file_hnd->file = file;
    coff_file_hnd->sec_name_hash_table = NULL;
    coff_file_hnd->symbol_handles = NULL;
    coff_file_hnd->implib_data = NULL;
    CoffAddFileLinks( coff_hnd, coff_file_hnd );
    error = CoffLoadFileStructure( coff_file_hnd );
    if( error != ORL_OKAY ) {
        CoffRemoveFileLinks( coff_file_hnd );
        coff_file_hnd = NULL;
    }
    *pcfh = coff_file_hnd;
    return( error );
}

orl_return COFFENTRY CoffFileFini( coff_file_handle coff_file_hnd )
{
    return( CoffRemoveFileLinks( coff_file_hnd ) );
}

orl_return COFFENTRY CoffFileScan( coff_file_handle coff_file_hnd, const char *desired, orl_sec_return_func return_func )
{
    orl_hash_data_struct *              data_struct;
    unsigned                            loop;
    orl_return                          error;

    if( desired == NULL ) {
        /* global request */
        for( loop = 0; loop < coff_file_hnd->num_sections; loop++ ) {
            error = return_func( (orl_sec_handle)coff_file_hnd->coff_sec_hnd[loop] );
            if( error != ORL_OKAY ) {
                return( error );
            }
        }
    } else {
        if( !(coff_file_hnd->sec_name_hash_table) ) {
            error = CoffBuildSecNameHashTable( coff_file_hnd );
            if( error != ORL_OKAY ) {
                return( error );
            }
        }
        data_struct = ORLHashTableQuery( coff_file_hnd->sec_name_hash_table, desired );
        while( data_struct != NULL ) {
            error = return_func( (orl_sec_handle)data_struct->data );
            if( error != ORL_OKAY )
                return( error );
            data_struct = data_struct->next;
        }
    }
    return( ORL_OKAY );
}

unsigned long COFFENTRY CoffExportTableRVA(coff_file_handle coff_file_hnd)
{
    return( coff_file_hnd->export_table_rva );
}

orl_machine_type COFFENTRY CoffFileGetMachineType( coff_file_handle coff_file_hnd )
{
    return( coff_file_hnd->machine_type );
}

orl_file_flags COFFENTRY CoffFileGetFlags( coff_file_handle coff_file_hnd )
{
    return( coff_file_hnd->flags );
}

orl_file_type COFFENTRY CoffFileGetType( coff_file_handle coff_file_hnd )
{
    return( coff_file_hnd->type );
}

orl_file_size COFFENTRY CoffFileGetSize( coff_file_handle coff_file_hnd )
{
    return( coff_file_hnd->size );
}

coff_sec_handle COFFENTRY CoffFileGetSymbolTable( coff_file_handle coff_file_hnd )
{
    return( coff_file_hnd->symbol_table );
}


coff_sec_handle COFFENTRY CoffSecGetStringTable( coff_sec_handle coff_sec_hnd )
{
    return( coff_sec_hnd->coff_file_hnd->string_table );
}

coff_sec_handle COFFENTRY CoffSecGetSymbolTable( coff_sec_handle coff_sec_hnd )
{
    return( coff_sec_hnd->coff_file_hnd->symbol_table );
}

char * COFFENTRY CoffSecGetName( coff_sec_handle coff_sec_hnd )
{
    return( coff_sec_hnd->name );
}

orl_sec_offset COFFENTRY CoffSecGetBase( coff_sec_handle coff_sec_hnd )
{
    return( coff_sec_hnd->base );
}

orl_sec_size COFFENTRY CoffSecGetSize( coff_sec_handle coff_sec_hnd )
{
    return( coff_sec_hnd->size );
}

orl_sec_type COFFENTRY CoffSecGetType( coff_sec_handle coff_sec_hnd )
{
    return( coff_sec_hnd->type );
}

orl_sec_flags COFFENTRY CoffSecGetFlags( coff_sec_handle coff_sec_hnd )
{
    return( coff_sec_hnd->flags );
}

orl_sec_alignment COFFENTRY CoffSecGetAlignment( coff_sec_handle coff_sec_hnd )
{
    return( coff_sec_hnd->align );
}

coff_sec_handle COFFENTRY CoffSecGetRelocTable( coff_sec_handle coff_sec_hnd )
{
    if( coff_sec_hnd->type != ORL_SEC_TYPE_RELOCS ) {
        return( coff_sec_hnd->assoc.normal.reloc_sec );
    }
    return( NULL );
}

orl_table_index COFFENTRY CoffSecGetNumLines( coff_sec_handle coff_sec_hnd )
{
    if( coff_sec_hnd->hdr != NULL ) {
        return( coff_sec_hnd->hdr->num_lineno );
    }
    return( 0 );
}

orl_linnum * COFFENTRY CoffSecGetLines( coff_sec_handle coff_sec_hnd )
{
    orl_table_index     numlines;

    numlines = CoffSecGetNumLines( coff_sec_hnd );
    if( numlines > 0 ) {
        return( CoffConvertLines( coff_sec_hnd, numlines ) );
    }
    return( NULL );
}

orl_sec_offset COFFENTRY CoffSecGetOffset( coff_sec_handle coff_sec_hnd )
{
    if( coff_sec_hnd->hdr != NULL ) {
        return( coff_sec_hnd->hdr->offset );
    }
    return( 0 );
}

orl_return COFFENTRY CoffSecGetContents( coff_sec_handle coff_sec_hnd, unsigned char **buffer )
{
    if( coff_sec_hnd->contents != NULL ) {
        *buffer = coff_sec_hnd->contents;
        return( ORL_OKAY );
    }
    return( ORL_ERROR );
}

orl_return COFFENTRY CoffSecQueryReloc( coff_sec_handle coff_sec_hnd, coff_sec_offset sec_offset, orl_reloc_return_func return_func )
{
    unsigned                                    index;
    coff_sec_handle                             reloc_sec_hnd;
    orl_reloc *                                 reloc;
    orl_return                                  return_val;

    if( coff_sec_hnd->type != ORL_SEC_TYPE_PROG_BITS )
        return( ORL_ERROR );
    reloc_sec_hnd = coff_sec_hnd->assoc.normal.reloc_sec;
    if( reloc_sec_hnd == NULL )
        return( ORL_FALSE );
    if( reloc_sec_hnd->assoc.reloc.relocs == NULL ) {
        return_val = CoffCreateRelocs( coff_sec_hnd, reloc_sec_hnd );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    reloc = reloc_sec_hnd->assoc.reloc.relocs;
    return_val = ORL_FALSE;
    for( index = 0; index < reloc_sec_hnd->assoc.reloc.num_relocs; index++ ) {
        if( reloc->offset == sec_offset ) {
            return_val = return_func( reloc );
            if( return_val != ORL_OKAY ) {
                return( return_val );
            }
        }
        reloc++;
    }
    return( return_val );
}

orl_table_index COFFENTRY CoffCvtSecHdlToIdx( coff_sec_handle shdl )
/******************************************************************/
// NYI: would be nice to have a O(1) way of doing this.
{
    orl_table_index     index;
    orl_table_index     limit;
    coff_file_handle    fhdl;

    fhdl = shdl->coff_file_hnd;
    limit = fhdl->f_hdr_buffer->num_sections;
    for( index = 0; index < limit; index++ ) {
        if( fhdl->orig_sec_hnd[index] == shdl ) {
            return( index + 1 );
        }
    }
    return( 0 );
}

coff_sec_handle COFFENTRY CoffCvtIdxToSecHdl( coff_file_handle fhdl,
                                              orl_table_index idx )
/******************************************************************/
{
    return( fhdl->orig_sec_hnd[idx - 1] );
}

orl_return COFFENTRY CoffSecScanReloc( coff_sec_handle coff_sec_hnd, orl_reloc_return_func return_func )
{
    unsigned                                    index;
    coff_sec_handle                             reloc_sec_hnd;
    orl_reloc *                                 reloc;
    orl_return                                  return_val;

    if( coff_sec_hnd->type != ORL_SEC_TYPE_PROG_BITS )
        return( ORL_ERROR );
    reloc_sec_hnd = coff_sec_hnd->assoc.normal.reloc_sec;
    if( reloc_sec_hnd == NULL )
        return( ORL_FALSE );
    if( reloc_sec_hnd->assoc.reloc.relocs == NULL ) {
        return_val = CoffCreateRelocs( coff_sec_hnd, reloc_sec_hnd );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    reloc = reloc_sec_hnd->assoc.reloc.relocs;
    return_val = ORL_FALSE;
    for( index = 0; index < reloc_sec_hnd->assoc.reloc.num_relocs; index++ ) {
        return_val = return_func( reloc );
        if( return_val != ORL_OKAY )
            return( return_val );
        reloc++;
    }
    return( return_val );
}

orl_return COFFENTRY CoffRelocSecScan( coff_sec_handle coff_sec_hnd, orl_reloc_return_func return_func )
{
    unsigned                                    index;
    orl_reloc *                                 reloc;
    orl_return                                  return_val;

    if( coff_sec_hnd->type != ORL_SEC_TYPE_RELOCS )
        return( ORL_ERROR );
    if( coff_sec_hnd->assoc.reloc.relocs == NULL ) {
        return_val = CoffCreateRelocs( coff_sec_hnd->assoc.reloc.orig_sec, coff_sec_hnd );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    reloc = coff_sec_hnd->assoc.reloc.relocs;
    return_val = ORL_FALSE;
    for( index = 0; index < coff_sec_hnd->assoc.reloc.num_relocs; index++ ) {
        return_val = return_func( reloc );
        if( return_val != ORL_OKAY )
            return( return_val );
        reloc++;
    }
    return( return_val );
}

orl_return COFFENTRY CoffSymbolSecScan( coff_sec_handle coff_sec_hnd, orl_symbol_return_func return_func )
{
    unsigned                                    index;
    orl_return                                  error;
    coff_symbol_handle                          coff_symbol_hnd;

    if( coff_sec_hnd->type != ORL_SEC_TYPE_SYM_TABLE )
        return( ORL_ERROR );
    if( !(coff_sec_hnd->coff_file_hnd->symbol_handles) ) {
        error = CoffCreateSymbolHandles( coff_sec_hnd->coff_file_hnd );
        if( error != ORL_OKAY ) {
            return( error );
        }
    }
    for( index = 0; index < coff_sec_hnd->coff_file_hnd->num_symbols; index++ ) {
        coff_symbol_hnd = coff_sec_hnd->coff_file_hnd->symbol_handles + index;
        error = return_func( (orl_symbol_handle) coff_symbol_hnd );
        if( error != ORL_OKAY )
            return( error );
        index += coff_symbol_hnd->symbol->num_aux;
    }
    return( ORL_OKAY );
}

orl_return COFFENTRY CoffNoteSecScan( coff_sec_handle hnd, orl_note_callbacks *cb, void *cookie )
/***********************************************************************************************/
{
    if( hnd->type != ORL_SEC_TYPE_NOTE )
        return( ORL_ERROR );
    if( strcmp( hnd->name, ".drectve" ) != 0 )
        return( ORL_OKAY );
    if( hnd->size == 0 )
        return( ORL_OKAY );
    return( CoffParseDrectve( (char *)hnd->contents, hnd->size, cb, cookie ) );
}

char * COFFENTRY CoffSymbolGetName( coff_symbol_handle coff_symbol_hnd )
{
    if( coff_symbol_hnd->type & ORL_SYM_TYPE_FILE ) {
        return( (char *)( coff_symbol_hnd->symbol + 1 ) );
    }
    return( coff_symbol_hnd->name );
}

orl_symbol_value COFFENTRY CoffSymbolGetValue( coff_symbol_handle coff_symbol_hnd )
{
    unsigned_64 val64;

    val64.u._32[I64LO32] = coff_symbol_hnd->symbol->value;
    val64.u._32[I64HI32] = 0;
    return( val64 );
}

orl_symbol_binding COFFENTRY CoffSymbolGetBinding( coff_symbol_handle coff_symbol_hnd )
{
    return( coff_symbol_hnd->binding );
}

orl_symbol_type COFFENTRY CoffSymbolGetType( coff_symbol_handle coff_symbol_hnd )
{
    return( coff_symbol_hnd->type );
}

coff_sec_handle COFFENTRY CoffSymbolGetSecHandle( coff_symbol_handle coff_symbol_hnd )
{
    if( coff_symbol_hnd->symbol->sec_num < 1 )
        return( NULL );
    return( coff_symbol_hnd->coff_file_hnd->orig_sec_hnd[coff_symbol_hnd->symbol->sec_num - 1] );
}

coff_symbol_handle COFFENTRY CoffSymbolGetAssociated( coff_symbol_handle hnd )
{
    coff_sym_weak *     weak;

    weak = (coff_sym_weak *)( hnd->symbol + 1 );
    return( &hnd->coff_file_hnd->symbol_handles[weak->tag_index] );
}

