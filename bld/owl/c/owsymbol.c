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

owl_symbol_handle OWLENTRY OWLSymbolInit( owl_file_handle file, const char *name ) {
//**********************************************************************************
    owl_symbol_handle   handle;
    owl_symbol_table    *table;

    table = file->symbol_table;
    handle = _ClientAlloc( table->file, sizeof( owl_symbol_info ) );
    handle->section = NULL;
    handle->index = 0;
    handle->comdat = 0;
    handle->type = OWL_TYPE_OBJECT;
    handle->offset = 0;
    handle->linkage = OWL_SYM_UNDEFINED;
    handle->num_relocs = 0;
    handle->name = NULL;
    if( name != NULL ) {
        handle->name = OWLStringAdd( table->file->string_table, name );
    }
    handle->flags = 0;
    handle->x.func = NULL;
    handle->next = NULL;
    *table->lnk = handle;
    table->lnk = &handle->next;
    table->num_global_symbols += 1;             // assume it to be global
    _Log((file, "OWLSymbolInit( %x, '%s' ) -> %x\n", file, name, handle ));
    return( handle );
}

void OWLENTRY OWLSymbolDefine( owl_symbol_table *table, owl_symbol_handle handle, owl_section_handle section,
                                                owl_offset offset, owl_sym_type type, owl_sym_linkage linkage ) {
//***************************************************************************************************************

    handle->section = section;
    handle->type = type;
    handle->offset = offset;
    handle->linkage = linkage;
    if( !_OwlLinkageGlobal( linkage ) ) {
        table->num_local_symbols += 1;
        table->num_global_symbols -= 1;
    }
    if( section != NULL &&
        _OwlSectionComdat( section ) && !_OwlMetaSymbol( type ) ) {
        section->comdat_sym = handle;
        handle->flags |= OWL_SYM_FLAG_COMDAT;
    }
}

owl_symbol_index OWLENTRY OWLSymbolIndex( owl_symbol_handle symbol ) {
//********************************************************************

    return( symbol->index );
}

owl_symbol_table * OWLENTRY OWLSymbolTableInit( owl_file_handle file ) {
//**********************************************************************

    owl_symbol_table    *table;

    table = _ClientAlloc( file, sizeof( owl_symbol_table ) );
    table->file = file;
    table->num_global_symbols = 0;
    table->num_local_symbols = 0;
    table->head = NULL;
    table->lnk = &table->head;
    return( table );
}

void OWLENTRY OWLSymbolTableFini( owl_symbol_table *table ) {
//***********************************************************

    owl_symbol_handle   curr;
    owl_symbol_handle   next;

    curr = table->head;
    while( curr != NULL ) {
        next = curr->next;

        switch( curr->type ) {
        case OWL_TYPE_FUNCTION:
            if( curr->linkage != OWL_SYM_UNDEFINED ) {
                owl_func_info *info;

                info = curr->x.func;
                if( info != NULL ) {
                    owl_func_file  *curr;
                    owl_func_file  *next;

                    curr = info->head;
                    while( curr != NULL ){
                        next = curr->next;
                        _ClientFree( table->file, curr );
                        curr = next;
                    }
                    _ClientFree( table->file, info );
                }
            }
            break;
        case OWL_TYPE_SECTION:
        case OWL_TYPE_FILE:
        case OWL_TYPE_OBJECT:
            break;
        }
        _ClientFree( table->file, curr );
        curr = next;
    }
    _ClientFree( table->file, table );
}
