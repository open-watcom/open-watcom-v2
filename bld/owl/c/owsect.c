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

static void addSectionSymbol( owl_section_handle section, const char *name ) {
//****************************************************************************

    section->sym = OWLSymbolInit( section->file, name );
    OWLSymbolDefine( section->file->symbol_table, section->sym, section, 0, OWL_TYPE_SECTION, OWL_SYM_GLOBAL );
    section->sym->x.section = section;
}

static void addSection( owl_file_handle file, owl_section_handle section ) {
//**************************************************************************

    owl_section_handle  curr;
    owl_section_handle  *last;

    last = &file->sections;
    for( curr = file->sections; curr != NULL; curr = curr->next ) {
        last = &curr->next;
    }
    *last = section;
    section->next = NULL;
    section->index = file->next_index;
    file->next_index += 1;
}

static void deleteSection( owl_file_handle file, owl_section_handle section ) {
//*****************************************************************************

    owl_section_handle  curr;
    owl_section_handle  *last;

    last = &file->sections;
    for( curr = file->sections; curr != NULL; curr = curr->next ) {
        if( curr == section ) {
            *last = curr->next;
            return;
        }
        last = &curr->next;
    }
    assert( 0 );
}

static void freeRelocs( owl_file_handle file, owl_reloc_info *relocs ) {
/**********************************************************************/

    owl_reloc_info      *ptr;
    owl_reloc_info      *next;

    for( ptr = relocs; ptr != NULL; ptr = next ) {
        next = ptr->next;
        _ClientFree( file, ptr );
    }
}

owl_section_handle OWLENTRY OWLSectionInit( owl_file_handle file, const char *name, owl_section_type type, owl_alignment align ) {
//********************************************************************************************************************************

    owl_section_handle          section;

    section = _ClientAlloc( file, sizeof( owl_section_info ) );
    section->file = file;
    section->name = OWLStringAdd( file->string_table, name );
    section->type = type;
    section->align = align;
    section->buffer = ( type & OWL_SEC_ATTR_BSS ) ? NULL : OWLBufferInit( file );
    section->linenum_buffer = NULL;
    section->num_linenums = 0;
    section->size = 0;
    section->location = 0;
    section->first_reloc = NULL;
    section->last_reloc = NULL;
    section->num_relocs = 0;
    section->comdat_sym = NULL;
    section->comdat_dep = NULL;
    addSection( file, section );
    addSectionSymbol( section, name );
    _Log(( file, "OWLSectionInit( %x, '%s', %x, %x ) -> %x\n", file, name, type, align, section ));
    return( section );
}

void OWLENTRY OWLComdatDep( owl_section_handle section, owl_section_handle dep ) {
//*********************************************************************************************

    _Log(( section->file, "OWLComdatDep( %x, %x )\n", section, dep ));
    section->comdat_dep = dep;
}

void OWLENTRY OWLSectionFini( owl_section_handle section ) {
//**********************************************************

    // while user may be done with this section, we can't trash
    // it until we are ready to write the entire file
    section = section;
    _Log(( section->file, "OWLSectionFini( %x )\n", section ));
}

void OWLENTRY OWLSectionFree( owl_section_handle section ) {
//**********************************************************

    owl_file_handle     file;

    file = section->file;
    deleteSection( file, section );
    if( section->buffer != NULL ) {
        OWLBufferFini( section->buffer );
    }
    if( section->linenum_buffer != NULL ) {
        OWLBufferFini( section->linenum_buffer );
    }
    if( section->first_reloc != NULL ) {
        freeRelocs( file, section->first_reloc );
    }
    _ClientFree( file, section );
    _Log(( file, "OWLSectionFree( %x )\n", section ));
}
