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
