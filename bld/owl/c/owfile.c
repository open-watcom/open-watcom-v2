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

static void addFile( owl_handle handle, owl_file_handle file ) {
//**************************************************************

    owl_file_handle     curr;
    owl_file_handle     *last;

    last = &handle->files;
    for( curr = handle->files; curr != NULL; curr = curr->next ) {
        last = &curr->next;
    }
    *last = file;
    file->next = NULL;
}

static void deleteFile( owl_handle handle, owl_file_handle file ) {
//*****************************************************************

    owl_file_handle     curr;
    owl_file_handle     *last;

    last = &handle->files;
    for( curr = handle->files; curr != NULL; curr = curr->next ) {
        if( curr == file ) {
            *last = curr->next;
            return;
        }
        last = &curr->next;
    }
    assert( 0 );
}

void OWLENTRY OWLFileSymbol( owl_file_handle file, const char *name ) {
//*********************************************************************

    owl_symbol_handle sym;

    assert( name != NULL );
    sym = OWLSymbolInit( file, name );
    OWLSymbolDefine( file->symbol_table, sym, NULL, 0, OWL_TYPE_FILE, OWL_SYM_GLOBAL );
    sym->x.file = file;
}

owl_file_handle OWLENTRY OWLFileInit( owl_handle handle, const char *name, owl_client_file client_handle, owl_format format, owl_file_type type ) {
//*************************************************************************************************************************************************

    owl_file_handle     file;

    file = handle->client_funcs.alloc( sizeof( *file ) );
    file->info = handle;
    file->format = format;
    file->client_handle = client_handle;
    file->log = NULL;
    file->type = type;
    file->string_table = OWLStringInit( file );
    file->symbol_table = OWLSymbolTableInit( file );
    file->sections = NULL;
    file->next_index = 0;
    file->name = NULL;
    addFile( handle, file );
    OWLFileSymbol( file, name );
    file->name = OWLStringAdd( file->string_table, name );
    return( file );
}

static void doReloc( owl_section_handle section, owl_reloc_info *reloc ) {
//************************************************************************

    owl_offset          displacement;
    owl_offset          old_loc;
    unsigned_32         data;
    unsigned_32         bit_mask;

    displacement = OWLRelocTargetDisp( section, reloc->location, reloc->symbol->offset );
    bit_mask = OWLRelocBitMask( section->file, reloc );
    // OWLBufferPatch( section->buffer, reloc->location, displacement, bitMask );
    old_loc = OWLBufferTell( section->buffer );
    OWLBufferSeek( section->buffer, reloc->location );
    OWLBufferRead( section->buffer, reloc->location, (char *)&data, 4 );
    if( section->file->format == OWL_FORMAT_COFF &&
        section->file->info->cpu == OWL_CPU_PPC &&
        reloc->type == OWL_RELOC_JUMP_REL ) {
        // ugly kludge for the MOTOROLA PPC linker
        displacement += reloc->location;
    }
    if( section->file->format == OWL_FORMAT_ELF &&
        section->file->info->cpu == OWL_CPU_INTEL &&
        reloc->type == OWL_RELOC_BRANCH_REL ) {
        // ugly kludge for 386 ELF objects
        displacement += 4;
    }
    data = (data&~bit_mask)|(((displacement&bit_mask)+(data&bit_mask))&bit_mask);
    OWLBufferWrite( section->buffer, (char *)&data, 4 );
    OWLBufferSeek( section->buffer, old_loc );
    _ClientFree( section->file, reloc );
    section->num_relocs -= 1;
}

#define _SymIsDefined( s )              ( (s)->section != NULL )

static void resolveRelativeRelocs( owl_file_handle file ) {
//*********************************************************

    owl_section_handle  curr;
    owl_reloc_info      *reloc;
    owl_reloc_info      **last;
    owl_symbol_info     *symbol;

    for( curr = file->sections; curr != NULL; curr = curr->next ) {
        if( curr->num_relocs == 0 ) continue;
        last = &curr->first_reloc;
        reloc = curr->first_reloc;
        while( reloc != NULL ) {
            curr->last_reloc = reloc;
            symbol = reloc->symbol;
            if( symbol->section == curr && OWLRelocIsRelative( file, reloc ) ) {
                // so we have a relocation to a reloc which is in this section
                *last = reloc->next;
                doReloc( curr, reloc );
                reloc = *last;
            } else {
                symbol->num_relocs += 1;
                last = &reloc->next;
                reloc = reloc->next;
            }
        }
    }
}

#define INT_BUFFER_LEN  20
#define RADIX           16  /* anything up to 16 (not 0) */

static void makeUniqueName( owl_file_handle file, owl_symbol_handle sym, unsigned index ) {
//*****************************************************************************************

    char                buffer[ INT_BUFFER_LEN ];
    char                *ptr;

    ptr = &buffer[ INT_BUFFER_LEN - 1 ];
    *ptr-- = '\0';
    do {
        *ptr-- = "0123456789abcdef"[ index % RADIX ];
        index /= RADIX;
    } while( index != 0 );
    *ptr-- = '$';
    *ptr = 'L';
    sym->name = OWLStringAdd( file->string_table, ptr );
}

static void resolveLabelNames( owl_file_handle file ) {
//*****************************************************

    owl_symbol_handle   sym;
    unsigned            index;

    index = 0;
    for( sym = file->symbol_table->head; sym != NULL; sym = sym->next ) {
        if( _OwlMetaSymbol( sym->type ) ) continue;
        if( sym->name == NULL ) {
            /*
             * Really checking to make sure sym has no special info attached
             * below, but for now func != NULL is good enough.
             */
            if( sym->num_relocs == 0 && sym->x.func == NULL ) {
                sym->flags |= OWL_SYM_DEAD;
                if( _OwlLinkageGlobal(sym->linkage) ) {
                    file->symbol_table->num_global_symbols--;
                } else {
                    file->symbol_table->num_local_symbols--;
                }
            } else {
                makeUniqueName( file, sym, index++ );
            }
        }
    }
}

static int comdefSection( owl_section_handle section ) {
//******************************************************

    if( section->type & OWL_SEC_ATTR_COMDAT ) {
        if( section->type & OWL_SEC_ATTR_BSS ) {
            return( TRUE );
        }
        if( section->size == 0 && section->comdat_sym == NULL ) {
            // hack to nuke empty sections created by front end
            return( TRUE );
        }
    }
    return( FALSE );
}

static void redoSectionIndices( owl_file_handle file ) {
//******************************************************

    owl_section_handle  curr;
    int                 index;

    index = 0;
    for( curr = file->sections; curr != NULL; curr = curr->next ) {
        curr->index = index++;
    }
    file->next_index = index;
}

static void resolveComdefSymbols( owl_file_handle file ) {
//********************************************************
// Want to run through the list of sections and turn any unitialized
// section with only one label at location 0 (and no relocs within it)
// into a comdef symbol.  This is a bit of a space-saver for normal
// code but vital for FORTRAN COMMON blocks which are unitialized.
// For now, we only do this is the comdat attr is set, but we should
// relax this in the future.

    owl_section_handle  curr;
    owl_section_handle  next;
    owl_symbol_handle   sym;
    owl_offset          size;

    curr = file->sections;
    while( curr != NULL ) {
        next = curr->next;
        if( comdefSection( curr ) ) {
            sym = curr->comdat_sym;
            size = curr->size;
            if( _OwlLinkageGlobal(curr->sym->linkage) ) {
                file->symbol_table->num_global_symbols--;
            } else {
                file->symbol_table->num_local_symbols--;
            }
            curr->sym->flags |= OWL_SYM_DEAD;           // section sym no longer needed
            OWLSectionFree( curr );
            // if sym was null then client made section which shouldn't exist...
            // this is a bit of a hack, but compilers do this because of i86obj cruft
            if( sym != NULL ) {
                sym->section = NULL;
                sym->offset = size;
                sym->flags |= OWL_SYM_COMDEF;
            }
        }
        curr = next;
    }
    redoSectionIndices( file );
}

void OWLENTRY OWLFileFini( owl_file_handle file ) {
//*************************************************

    owl_section_handle  curr;
    owl_section_handle  next;

    resolveRelativeRelocs( file );
    resolveLabelNames( file );
    resolveComdefSymbols( file );
    if( file->format == OWL_FORMAT_COFF ) {
        COFFFileEmit( file );
    } else {
        ELFFileEmit( file );
    }
    for( curr = file->sections; curr != NULL; curr = next ) {
        next = curr->next;
        OWLSectionFree( curr );
    }
    OWLSymbolTableFini( file->symbol_table );
    OWLStringFini( file->string_table );
    deleteFile( file->info, file );
    _ClientFree( file, file );
}
