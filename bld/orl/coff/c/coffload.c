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
* Description:  Load COFF object file into memory for processing.
*
****************************************************************************/


#include "coffload.h"
#include "coffimpl.h"
#include "cofforl.h"

/* This is a hack for a flaw in wlib. Because wlib does not keep track
 * of member sizes, one can't tell if a read past the end of file occurred.
 * Unfortunately for COFF, it is impossible to tell whether a string table
 * is present or not except by attempting to read its size. If we read
 * some ridiculously large value, it's the start of the next member.
 */
#define MAX_STRTAB_SIZE     0x1000000   /* 16MB */

static char SectionNames[3][COFF_SEC_NAME_LEN] =
    { ".rel", ".symtab", ".strtab" };

static int determine_file_specs( coff_file_handle coff_file_hnd,
                                 coff_file_header *f_hdr )
/**************************************************************/
{
    uint_16 cpu_type;
    uint_16 flags;
    int     flag_import_library;

    if( (f_hdr->cpu_type == IMAGE_FILE_MACHINE_UNKNOWN) &&
            (f_hdr->num_sections == IMPORT_OBJECT_HDR_SIG2) ) {
        // COFF import library
        cpu_type = ((coff_import_object_header *)f_hdr)->machine;
        flags = 0;
        flag_import_library = 1;
    } else {
        // other COFF objects
        cpu_type = f_hdr->cpu_type;
        flags = f_hdr->flags;
        flag_import_library = 0;
    }
    switch( cpu_type ) {
        case IMAGE_FILE_MACHINE_I860:
            coff_file_hnd->machine_type = ORL_MACHINE_TYPE_I860;
            break;
        case IMAGE_FILE_MACHINE_I386:
        case IMAGE_FILE_MACHINE_I386A:
            coff_file_hnd->machine_type = ORL_MACHINE_TYPE_I386;
            break;
        case IMAGE_FILE_MACHINE_R3000:
            coff_file_hnd->machine_type = ORL_MACHINE_TYPE_R3000;
            break;
        case IMAGE_FILE_MACHINE_R4000:
            coff_file_hnd->machine_type = ORL_MACHINE_TYPE_R4000;
            break;
        case IMAGE_FILE_MACHINE_ALPHA:
            coff_file_hnd->machine_type = ORL_MACHINE_TYPE_ALPHA;
            break;
        case IMAGE_FILE_MACHINE_POWERPC:
            coff_file_hnd->machine_type = ORL_MACHINE_TYPE_PPC601;
            break;
        case IMAGE_FILE_MACHINE_AMD64:
            coff_file_hnd->machine_type = ORL_MACHINE_TYPE_AMD64;
            break;
        case IMAGE_FILE_MACHINE_UNKNOWN:
            coff_file_hnd->machine_type = ORL_MACHINE_TYPE_NONE;
            break;
        default:
            coff_file_hnd->machine_type = ORL_MACHINE_TYPE_UNKNOWN;
            break;
    }

    if( flags & IMAGE_FILE_DLL ) {
        coff_file_hnd->type = ORL_FILE_TYPE_DLL;
    } else if( flags & IMAGE_FILE_EXECUTABLE_IMAGE ) {
        coff_file_hnd->type = ORL_FILE_TYPE_EXECUTABLE;
    } else {
        coff_file_hnd->type = ORL_FILE_TYPE_OBJECT;
    }

    coff_file_hnd->flags = ORL_FILE_FLAG_NONE;
    if( flags & IMAGE_FILE_RELOCS_STRIPPED ) {
        coff_file_hnd->flags |= ORL_FILE_FLAG_RELOCS_STRIPPED;
    }
    if( flags & IMAGE_FILE_LINE_NUMS_STRIPPED ) {
        coff_file_hnd->flags |= ORL_FILE_FLAG_LINE_NUMS_STRIPPED;
    }
    if( flags & IMAGE_FILE_LOCAL_SYMS_STRIPPED ) {
        coff_file_hnd->flags |= ORL_FILE_FLAG_LOCAL_SYMS_STRIPPED;
    }
    if( flags & IMAGE_FILE_DEBUG_STRIPPED ) {
        coff_file_hnd->flags |= ORL_FILE_FLAG_DEBUG_STRIPPED;
    }
    if( cpu_type == IMAGE_FILE_MACHINE_AMD64 ) {
        coff_file_hnd->flags |= ORL_FILE_FLAG_64BIT_MACHINE;
    } else {
        if( flags & IMAGE_FILE_16BIT_MACHINE ) {
            coff_file_hnd->flags |= ORL_FILE_FLAG_16BIT_MACHINE;
        }
        if( flags & IMAGE_FILE_32BIT_MACHINE ) {
            coff_file_hnd->flags |= ORL_FILE_FLAG_32BIT_MACHINE;
        }
    }
    if( flags & IMAGE_FILE_SYSTEM ) {
        coff_file_hnd->flags |= ORL_FILE_FLAG_SYSTEM;
    }
    if( coff_file_hnd->type != ORL_FILE_TYPE_OBJECT ) {
        /* There are no known big endian PE images, but there are lying
         * cheating PE images that claim to be big endian and aren't.
         */
        coff_file_hnd->flags |= ORL_FILE_FLAG_LITTLE_ENDIAN;
    } else {
        /*
        if( flags & IMAGE_FILE_BYTES_REVERSED_LO ) {
            coff_file_hnd->flags |= ORL_FILE_FLAG_LITTLE_ENDIAN;
        }
        */
        if( flags & IMAGE_FILE_BYTES_REVERSED_HI ) {
            coff_file_hnd->flags |= ORL_FILE_FLAG_BIG_ENDIAN;
        } else {
            /* Inserting a default here - note that the BYTES_REVERSED_LO/HI 
             * flags are now deprecated and neither is supposed to be present.
             */
            coff_file_hnd->flags |= ORL_FILE_FLAG_LITTLE_ENDIAN;
        }
    }

    if( f_hdr->sym_table == 0 ) {
        f_hdr->num_symbols = 0; /* Fix up incorrectly stripped images */
    }
    /*
        At this point, we have filled in
            coff_file_hnd->type
            coff_file_hnd->machine_type
            coff_file_hnd->flags
    */
    return( flag_import_library );
}

static void determine_section_specs( coff_sec_handle coff_sec_hnd,
                                     coff_section_header *s_hdr )
/****************************************************************/
{
    coff_sec_hnd->flags = ORL_SEC_FLAG_NONE;
    if( s_hdr->flags & IMAGE_SCN_LNK_INFO ) {
        coff_sec_hnd->type = ORL_SEC_TYPE_NOTE;
    } else if( s_hdr->flags & IMAGE_SCN_CNT_CODE ) {
        coff_sec_hnd->type = ORL_SEC_TYPE_PROG_BITS;
        coff_sec_hnd->flags |= ORL_SEC_FLAG_EXEC;
    } else if( s_hdr->flags & IMAGE_SCN_CNT_INITIALIZED_DATA ) {
        coff_sec_hnd->type = ORL_SEC_TYPE_PROG_BITS;
        coff_sec_hnd->flags |= ORL_SEC_FLAG_INITIALIZED_DATA;
    } else if( s_hdr->flags & IMAGE_SCN_CNT_UNINITIALIZED_DATA ) {
        coff_sec_hnd->type = ORL_SEC_TYPE_PROG_BITS;
        coff_sec_hnd->flags |= ORL_SEC_FLAG_UNINITIALIZED_DATA;
    } else {
        coff_sec_hnd->type = ORL_SEC_TYPE_NONE;
    }
//  if( s_hdr->flags & IMAGE_SCN_TYPE_GROUP ) {         // no MS support
//      coff_sec_hnd->flags |= ORL_SEC_FLAG_GROUPED;
//  }
    if( s_hdr->flags & IMAGE_SCN_TYPE_NO_PAD ) {
        coff_sec_hnd->flags |= ORL_SEC_FLAG_NO_PADDING;
    }
//  if( s_hdr->flags & IMAGE_SCN_LNK_OVER ) {           // no MS support
//      coff_sec_hnd->flags |= ORL_SEC_FLAG_OVERLAY;
//  }
    if( s_hdr->flags & IMAGE_SCN_LNK_REMOVE ) {
        coff_sec_hnd->flags |= ORL_SEC_FLAG_REMOVE;
    }
    if( s_hdr->flags & IMAGE_SCN_LNK_COMDAT ) {
        coff_sec_hnd->flags |= ORL_SEC_FLAG_COMDAT;
    }
    if( s_hdr->flags & IMAGE_SCN_MEM_DISCARDABLE ) {
        coff_sec_hnd->flags |= ORL_SEC_FLAG_DISCARDABLE;
    }
    if( s_hdr->flags & IMAGE_SCN_MEM_NOT_CACHED ) {
        coff_sec_hnd->flags |= ORL_SEC_FLAG_NOT_CACHED;
    }
    if( s_hdr->flags & IMAGE_SCN_MEM_NOT_PAGED ) {
        coff_sec_hnd->flags |= ORL_SEC_FLAG_NOT_PAGEABLE;
    }
    if( s_hdr->flags & IMAGE_SCN_MEM_SHARED ) {
        coff_sec_hnd->flags |= ORL_SEC_FLAG_SHARED;
    }
    if( s_hdr->flags & IMAGE_SCN_MEM_EXECUTE ) {
        coff_sec_hnd->flags |= ORL_SEC_FLAG_EXECUTE_PERMISSION;
    }
    if( s_hdr->flags & IMAGE_SCN_MEM_READ ) {
        coff_sec_hnd->flags |= ORL_SEC_FLAG_READ_PERMISSION;
    }
    if( s_hdr->flags & IMAGE_SCN_MEM_WRITE ) {
        coff_sec_hnd->flags |= ORL_SEC_FLAG_WRITE_PERMISSION;
    }
    coff_sec_hnd->align = (s_hdr->flags & IMAGE_SCN_ALIGN_MASK)
                                    >> COFF_SEC_FLAG_ALIGN_SHIFT;
    if( coff_sec_hnd->align == 0 ) {
        coff_sec_hnd->align = 4;
    } else {
        coff_sec_hnd->align -= 1;
    }
}

static void free_coff_sec_handles( coff_file_handle coff_file_hnd,
                                   int num_alloced )
/****************************************************************/
{
    int                                 loop;

    if( coff_file_hnd->coff_sec_hnd != NULL ) {
        for( loop = 0; loop < num_alloced; loop++ ) {
            if( coff_file_hnd->coff_sec_hnd[loop]->name_alloced ) {
                _ClientFree( coff_file_hnd,
                             coff_file_hnd->coff_sec_hnd[loop]->name );
            }
            _ClientFree( coff_file_hnd, coff_file_hnd->coff_sec_hnd[loop] );
        }
        _ClientFree( coff_file_hnd, coff_file_hnd->coff_sec_hnd );
    } else {
        for( loop = 0; loop < num_alloced; loop++ ) {
            _ClientFree( coff_file_hnd, coff_file_hnd->orig_sec_hnd[loop] );
        }
    }
    _ClientFree( coff_file_hnd, coff_file_hnd->orig_sec_hnd );
}

static orl_return load_coff_sec_handles( coff_file_handle coff_file_hnd,
                                         coff_file_header * f_hdr )
/**********************************************************************/
{
    coff_section_header *               s_hdr;
    coff_sec_handle                     coff_sec_hnd;
    coff_sec_handle                     coff_reloc_sec_hnd;
    int                                 loop;
    coff_quantity                       num_reloc_secs = 0;
    orl_file_offset *                   reloc_sec_offset;
    orl_sec_size *                      reloc_sec_size;
    coff_quantity                       reloc_secs_created;

    if( coff_file_hnd->num_sections == 0 ) {
        reloc_sec_offset = NULL;
        reloc_sec_size = NULL;
        coff_file_hnd->orig_sec_hnd = NULL;
    } else {
        reloc_sec_offset = (orl_file_offset *) _ClientAlloc( coff_file_hnd, sizeof( orl_file_offset ) * coff_file_hnd->num_sections );
        if( !(reloc_sec_offset) ) return( ORL_OUT_OF_MEMORY );
        reloc_sec_size = (orl_sec_size *) _ClientAlloc( coff_file_hnd, sizeof( orl_sec_size ) * coff_file_hnd->num_sections );
        if( !(reloc_sec_size) ) {
            _ClientFree( coff_file_hnd, reloc_sec_offset );
            return( ORL_OUT_OF_MEMORY );
        }
        memset( reloc_sec_offset, 0, sizeof( orl_file_offset ) * coff_file_hnd->num_sections );
        memset( reloc_sec_size, 0, sizeof( orl_sec_size ) * coff_file_hnd->num_sections );
        coff_file_hnd->orig_sec_hnd = (coff_sec_handle *) _ClientAlloc( coff_file_hnd, sizeof( coff_sec_handle ) * coff_file_hnd->num_sections );
        if( !( coff_file_hnd->orig_sec_hnd ) ) {
            _ClientFree( coff_file_hnd, reloc_sec_offset );
            _ClientFree( coff_file_hnd, reloc_sec_size );
            return( ORL_OUT_OF_MEMORY );
        }
    }
    coff_file_hnd->coff_sec_hnd = NULL;
    s_hdr = (coff_section_header *) coff_file_hnd->s_hdr_table_buffer;
    for( loop = 0; loop < coff_file_hnd->num_sections; loop++ ) {
        coff_sec_hnd = (coff_sec_handle) _ClientAlloc( coff_file_hnd, sizeof( coff_sec_handle_struct ) );
        if( !coff_sec_hnd ) {
            free_coff_sec_handles( coff_file_hnd, loop );
            _ClientFree( coff_file_hnd, reloc_sec_offset );
            _ClientFree( coff_file_hnd, reloc_sec_size );
            return( ORL_OUT_OF_MEMORY );
        }
        coff_file_hnd->orig_sec_hnd[loop] = coff_sec_hnd;
        if( s_hdr->name[0] != '/' ) {
            coff_sec_hnd->name = _ClientAlloc( coff_file_hnd, COFF_SEC_NAME_LEN + 1 );
            if( !(coff_sec_hnd->name) ) {
                free_coff_sec_handles( coff_file_hnd, loop );
                _ClientFree( coff_file_hnd, reloc_sec_offset );
                _ClientFree( coff_file_hnd, reloc_sec_size );
                return( ORL_OUT_OF_MEMORY );
            }
            coff_sec_hnd->name_alloced = COFF_TRUE;
            strncpy( coff_sec_hnd->name, s_hdr->name, COFF_SEC_NAME_LEN );
            coff_sec_hnd->name[COFF_SEC_NAME_LEN] = 0;
        } else {
            coff_sec_hnd->name = s_hdr->name;
            coff_sec_hnd->name_alloced = COFF_FALSE;
        }
        coff_sec_hnd->file_format = ORL_COFF;
        coff_sec_hnd->relocs_done = COFF_FALSE;
        coff_sec_hnd->coff_file_hnd = coff_file_hnd;
        coff_sec_hnd->size = s_hdr->size;
        coff_sec_hnd->base = s_hdr->offset;
        coff_sec_hnd->offset = s_hdr->rawdata_ptr;
        coff_sec_hnd->hdr = s_hdr;
        determine_section_specs( coff_sec_hnd, s_hdr );
        coff_sec_hnd->contents = NULL;
        coff_sec_hnd->assoc.normal.reloc_sec = NULL;
        reloc_sec_offset[loop] = s_hdr->reloc_ptr;
        if( s_hdr->num_relocs > 0 ) {
            num_reloc_secs++;
        }
        reloc_sec_size[loop] = s_hdr->num_relocs * sizeof( coff_reloc );
        s_hdr++;
    }
    /* There are num_reloc_secs + 2 additional section handles to create
     (one each for the symbol and string tables) */
    coff_file_hnd->coff_sec_hnd = (coff_sec_handle *) _ClientAlloc( coff_file_hnd, sizeof( coff_sec_handle ) * (coff_file_hnd->num_sections + num_reloc_secs + 2) );
    if( !(coff_file_hnd->coff_sec_hnd) ) {
        free_coff_sec_handles( coff_file_hnd, coff_file_hnd->num_sections );
        _ClientFree( coff_file_hnd, reloc_sec_offset );
        _ClientFree( coff_file_hnd, reloc_sec_size );
        return( ORL_OUT_OF_MEMORY );
    }
    memcpy( coff_file_hnd->coff_sec_hnd, coff_file_hnd->orig_sec_hnd, sizeof( coff_sec_handle ) * coff_file_hnd->num_sections );
    reloc_secs_created = 0;
    for( loop = 0; loop < coff_file_hnd->num_sections; loop++ ) {
        if( reloc_sec_size[loop] > 0 ) {
            reloc_secs_created++;
            // create a reloc section
            coff_file_hnd->coff_sec_hnd[coff_file_hnd->num_sections + reloc_secs_created] = (coff_sec_handle) _ClientAlloc( coff_file_hnd, sizeof( coff_sec_handle_struct ) );
            coff_reloc_sec_hnd = coff_file_hnd->coff_sec_hnd[coff_file_hnd->num_sections + reloc_secs_created];
            if( !coff_reloc_sec_hnd ) {
                free_coff_sec_handles( coff_file_hnd, coff_file_hnd->num_sections + reloc_secs_created );
                _ClientFree( coff_file_hnd, reloc_sec_offset );
                _ClientFree( coff_file_hnd, reloc_sec_size );
                return( ORL_OUT_OF_MEMORY );
            }
            coff_reloc_sec_hnd->file_format = ORL_COFF;
            coff_reloc_sec_hnd->coff_file_hnd = coff_file_hnd;
            coff_reloc_sec_hnd->name = SectionNames[0];     // ".rel"
            coff_reloc_sec_hnd->name_alloced = COFF_FALSE;
            coff_reloc_sec_hnd->relocs_done = COFF_FALSE;
            coff_reloc_sec_hnd->size = reloc_sec_size[loop];
            coff_reloc_sec_hnd->base = 0;
            coff_reloc_sec_hnd->offset = reloc_sec_offset[loop];
            coff_reloc_sec_hnd->type = ORL_SEC_TYPE_RELOCS;
            coff_reloc_sec_hnd->flags = ORL_SEC_FLAG_NONE;
            coff_reloc_sec_hnd->hdr = NULL;
            coff_reloc_sec_hnd->assoc.reloc.orig_sec = coff_file_hnd->coff_sec_hnd[loop];
            coff_reloc_sec_hnd->assoc.reloc.relocs = NULL;
            coff_reloc_sec_hnd->align = 4;
            coff_file_hnd->coff_sec_hnd[loop]->assoc.normal.reloc_sec = coff_reloc_sec_hnd;
            coff_file_hnd->coff_sec_hnd[coff_file_hnd->num_sections + reloc_secs_created - 1] = coff_reloc_sec_hnd;
        }
    }
    // create the symbol table section
    coff_file_hnd->symbol_table = (coff_sec_handle) _ClientAlloc( coff_file_hnd, sizeof( coff_sec_handle_struct ) );
    if( !(coff_file_hnd->symbol_table) ) {
        free_coff_sec_handles( coff_file_hnd, loop + coff_file_hnd->num_sections );
        _ClientFree( coff_file_hnd, reloc_sec_offset );
        _ClientFree( coff_file_hnd, reloc_sec_size );
        return( ORL_OUT_OF_MEMORY );
    }
    coff_file_hnd->symbol_table->file_format = ORL_COFF;
    coff_file_hnd->symbol_table->coff_file_hnd = coff_file_hnd;
    coff_file_hnd->symbol_table->name = SectionNames[1];    // ".symtab"
    coff_file_hnd->symbol_table->name_alloced = COFF_FALSE;
    coff_file_hnd->symbol_table->relocs_done = COFF_FALSE;
    coff_file_hnd->symbol_table->size = f_hdr->num_symbols * sizeof( coff_symbol );
    coff_file_hnd->symbol_table->base = 0;
    coff_file_hnd->symbol_table->offset = f_hdr->sym_table;
    coff_file_hnd->symbol_table->hdr = NULL;
    coff_file_hnd->symbol_table->assoc.normal.reloc_sec = NULL;
    coff_file_hnd->symbol_table->type = ORL_SEC_TYPE_SYM_TABLE;
    coff_file_hnd->symbol_table->flags = ORL_SEC_FLAG_NONE;
    coff_file_hnd->symbol_table->align = 4;
    coff_file_hnd->coff_sec_hnd[coff_file_hnd->num_sections + reloc_secs_created] = coff_file_hnd->symbol_table;
    loop++;
    // create the string table section
    coff_file_hnd->string_table = (coff_sec_handle) _ClientAlloc( coff_file_hnd, sizeof( coff_sec_handle_struct ) );
    if( !(coff_file_hnd->string_table) ) {
        free_coff_sec_handles( coff_file_hnd, loop + coff_file_hnd->num_sections );
        _ClientFree( coff_file_hnd, reloc_sec_offset );
        _ClientFree( coff_file_hnd, reloc_sec_size );
        return( ORL_OUT_OF_MEMORY );
    }
    coff_file_hnd->string_table->file_format = ORL_COFF;
    coff_file_hnd->string_table->coff_file_hnd = coff_file_hnd;
    coff_file_hnd->string_table->name = SectionNames[2];    // ".strtab"
    coff_file_hnd->string_table->name_alloced = COFF_FALSE;
    coff_file_hnd->string_table->relocs_done = COFF_FALSE;
    coff_file_hnd->string_table->size = 0;  // determined later
    coff_file_hnd->string_table->base = 0;
    coff_file_hnd->string_table->offset = coff_file_hnd->symbol_table->offset + coff_file_hnd->symbol_table->size;
    coff_file_hnd->string_table->hdr = NULL;
    coff_file_hnd->string_table->assoc.normal.reloc_sec = NULL;
    coff_file_hnd->string_table->type = ORL_SEC_TYPE_STR_TABLE;
    coff_file_hnd->string_table->flags = ORL_SEC_FLAG_NONE;
    coff_file_hnd->string_table->align = 4;
    coff_file_hnd->coff_sec_hnd[coff_file_hnd->num_sections + reloc_secs_created + 1] = coff_file_hnd->string_table;
    _ClientFree( coff_file_hnd, reloc_sec_offset );
    _ClientFree( coff_file_hnd, reloc_sec_size );
    coff_file_hnd->num_sections += num_reloc_secs + 2;
    return( ORL_OKAY );
}

orl_return CoffLoadFileStructure( coff_file_handle coff_file_hnd )
/****************************************************************/
{
    orl_return          error;
    coff_file_header *  f_hdr;
    coff_quantity       sec_header_table_size;
    coff_quantity       buf_size;
    coff_sec_offset     string_table_index;
    coff_sec_handle     last_sec_hnd;
    coff_sec_handle     coff_sec_hnd;
    int                 loop;
    int                 loop_limit;
    pe_header *         pe_hdr;
    char *              PE;
    orl_file_offset     PEoffset = 0;
    orl_sec_size        *string_sec_size;

    pe_hdr = _ClientRead( coff_file_hnd, 2 );
    _ClientSeek( coff_file_hnd, -2, SEEK_CUR );
    if( pe_hdr->MZ[0] == 'M' && pe_hdr->MZ[1] == 'Z' ) {
        pe_hdr = _ClientRead( coff_file_hnd, sizeof( pe_header ) );
        _ClientSeek( coff_file_hnd, pe_hdr->offset - sizeof( pe_header ), SEEK_CUR );
        PE = _ClientRead( coff_file_hnd, 4 );
        if( PE[0] == 'P' && PE[1] == 'E' && PE[2] == '\0' && PE[3] == '\0' ) {
            PEoffset = pe_hdr->offset + 4;
        } else {
            _ClientSeek( coff_file_hnd, -pe_hdr->offset-4, SEEK_CUR );
        }
    }
    coff_file_hnd->f_hdr_buffer = _ClientRead( coff_file_hnd, sizeof( coff_file_header ) );
    if( !(coff_file_hnd->f_hdr_buffer) )
        return( ORL_OUT_OF_MEMORY );
    f_hdr = (coff_file_header *) coff_file_hnd->f_hdr_buffer;
    if( determine_file_specs( coff_file_hnd, f_hdr ) ) {
        // we have identified an import_object_header
        // convert short import library structures to long import
        // library structures, change _ClientRead and _ClientSeek
        // macros to read from converted metadata
        error = convert_import_library( coff_file_hnd );
        if ( error != ORL_OKAY ) {
            return( error );
        }
        // reread new converted file header and next process as normal
        coff_file_hnd->f_hdr_buffer = _ClientRead( coff_file_hnd, sizeof( coff_file_header ) );
        if( !(coff_file_hnd->f_hdr_buffer) )
            return( ORL_OUT_OF_MEMORY );
        f_hdr = (coff_file_header *) coff_file_hnd->f_hdr_buffer;
        determine_file_specs( coff_file_hnd, f_hdr );
    }
    if( f_hdr->opt_hdr_size > 0 ) {     // skip optional header
        pe_opt_hdr *opt_hdr = (pe_opt_hdr *)_ClientRead( coff_file_hnd, f_hdr->opt_hdr_size );

        if( (opt_hdr->magic == 0x10b) || (opt_hdr->magic == 0x20b) ) {
            coff_file_hnd->export_table_rva = opt_hdr->export_table_rva;
        } else {
            coff_file_hnd->export_table_rva = 0L;
        }
    }
    coff_file_hnd->initial_size = sizeof( coff_file_header ) + f_hdr->opt_hdr_size + PEoffset;
    switch( coff_file_hnd->machine_type ) {
    case ORL_MACHINE_TYPE_UNKNOWN:
        return( ORL_ERROR );
    default:
        break;
    }
    coff_file_hnd->num_symbols = f_hdr->num_symbols;
    coff_file_hnd->num_sections = f_hdr->num_sections;
    sec_header_table_size = coff_file_hnd->num_sections * sizeof( coff_section_header );
    if( coff_file_hnd->num_sections > 0 ) {
        coff_file_hnd->s_hdr_table_buffer = _ClientRead( coff_file_hnd, sec_header_table_size);
        if( !(coff_file_hnd->s_hdr_table_buffer) ) {
            return( ORL_ERROR );
        }
    }
    error = load_coff_sec_handles( coff_file_hnd, f_hdr );
    if( error != ORL_OKAY ) {
        return( error );
    }
    last_sec_hnd = coff_file_hnd->coff_sec_hnd[coff_file_hnd->num_sections - 1];
    coff_file_hnd->initial_size += sec_header_table_size;

    buf_size = 0;
    for( loop=0; loop < coff_file_hnd->num_sections; loop++ ) {
        coff_sec_hnd = coff_file_hnd->coff_sec_hnd[loop];
        if( (coff_sec_hnd->offset + coff_sec_hnd->size) > buf_size && !(coff_sec_hnd->flags & ORL_SEC_FLAG_UNINITIALIZED_DATA) ) {
            buf_size = coff_sec_hnd->offset + coff_sec_hnd->size;
        }
    }
    coff_file_hnd->size = buf_size;
    buf_size -= coff_file_hnd->initial_size;
    coff_file_hnd->rest_of_file_buffer = _ClientRead( coff_file_hnd, buf_size );
    if( !(coff_file_hnd->rest_of_file_buffer ) ) {
        return( ORL_ERROR );
    }
    loop_limit = coff_file_hnd->num_sections;
    // read string table; always follows the symbol table, but may not exist
    if( last_sec_hnd == coff_file_hnd->string_table ) {
        // read the string table size; if that fails, there isn't any
        if( f_hdr->sym_table ) {
            string_sec_size = _ClientRead( coff_file_hnd, sizeof( coff_sec_size ) );
            if( string_sec_size ) {
                last_sec_hnd->size = *string_sec_size;
            }
        }
        if( last_sec_hnd->size <= sizeof( coff_sec_size ) 
         || last_sec_hnd->size > MAX_STRTAB_SIZE ) {
            last_sec_hnd->size = 0;
        }
        if( last_sec_hnd->size != 0 ) {
            last_sec_hnd->size -= sizeof( coff_sec_size );
        }
        if( last_sec_hnd->size > 0 && last_sec_hnd->offset != 0 ) {
            if( last_sec_hnd->offset == buf_size + coff_file_hnd->initial_size ) {
                last_sec_hnd->contents = _ClientRead( coff_file_hnd, last_sec_hnd->size );
                coff_file_hnd->size += last_sec_hnd->size;
            } else {
                last_sec_hnd->contents = coff_file_hnd->rest_of_file_buffer + last_sec_hnd->offset - coff_file_hnd->initial_size + sizeof( coff_sec_size );
            }
            if( !(last_sec_hnd->contents ) ) {
                return( ORL_ERROR );
            }
        } else {
            last_sec_hnd->contents = NULL;
        }
        loop_limit--;
    }
    // determine contents pointers of all sections
    for( loop = 0; loop < loop_limit; loop++ ) {
        coff_sec_hnd = coff_file_hnd->coff_sec_hnd[loop];
        if( (coff_sec_hnd->size > 0 || coff_sec_hnd->type == ORL_SEC_TYPE_STR_TABLE) && coff_sec_hnd->offset != 0 ) {
            coff_sec_hnd->contents = coff_file_hnd->rest_of_file_buffer + coff_sec_hnd->offset - coff_file_hnd->initial_size;
        }
    }
    if( last_sec_hnd != coff_file_hnd->string_table ) {
        memcpy( &(coff_file_hnd->string_table->size), coff_file_hnd->string_table->contents, sizeof( coff_sec_size ) );
        if( coff_file_hnd->string_table->size != 0 ) {
            coff_file_hnd->string_table->size -= sizeof( coff_sec_size );
            coff_file_hnd->string_table->contents += sizeof( coff_sec_size );
        }
    }
    // determine section names
    for( loop = 0; loop < coff_file_hnd->num_sections; loop++ ) {
        coff_sec_hnd = coff_file_hnd->coff_sec_hnd[loop];
        if( coff_sec_hnd->name[0] == '/' ) {
            // change pointer to point into string table
            string_table_index = atoi( &(coff_sec_hnd->name[1]) ) - sizeof(coff_sec_size);
            coff_sec_hnd->name = (char *)&(coff_file_hnd->string_table->contents[string_table_index]);
        }
    }
    return( ORL_OKAY );
}
