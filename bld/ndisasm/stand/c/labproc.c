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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "global.h"
#include "dis.h"
#include "demangle.h"
#include "labproc.h"
#include "memfuncs.h"
#include "hashtabl.h"
#include "formasm.h"


extern wd_options       Options;
extern hash_table       HandleToLabelListTable;
extern hash_table       SymbolToLabelTable;
extern publics_struct   Publics;
extern char *           SourceFileInObject;
extern dis_format_flags DFormat;

static label_entry resolveTwoLabelsAtLocation( label_list sec_label_list, label_entry entry, label_entry previous_entry, label_entry old_entry )
{
    if( ( entry->type == LTYP_UNNAMED ) && ( old_entry->type != LTYP_FUNC_INFO )
        && ( old_entry->type != LTYP_SECTION || !IsMasmOutput() ) ) {
        // merge entry into old_entry
        MemFree( entry );
        entry = old_entry;
    } else {
        // two labels for this location!
        entry->next = old_entry;
        if( previous_entry ) {
            previous_entry->next = entry;
        } else {
            sec_label_list->first = entry;
        }
    }
    return( entry );
}

static label_entry insertLabelInMiddle( label_list sec_label_list, label_entry entry )
{
    label_entry                 walker;
    label_entry                 previous_entry;
    label_entry                 old_entry;

    if( entry->offset == sec_label_list->first->offset ) {
        old_entry = sec_label_list->first;
        previous_entry = NULL;
    } else {
        walker = sec_label_list->first;
        while( walker->next->offset < entry->offset ) {
            walker = walker->next;
        }
        old_entry = walker->next;
        previous_entry = walker;
    }
    if( old_entry->offset == entry->offset ) {
        entry = resolveTwoLabelsAtLocation( sec_label_list, entry, previous_entry, old_entry );
    } else {
        entry->next = previous_entry->next;
        previous_entry->next = entry;
    }
    return( entry );
}

static label_entry addLabel( label_list sec_label_list, label_entry entry, orl_sec_handle sym_hnd )
{
    if( sec_label_list->first == NULL ) {
        sec_label_list->first = entry;
        sec_label_list->last = entry;
        entry->next = NULL;
    } else if( entry->offset > sec_label_list->last->offset ) {
        sec_label_list->last->next = entry;
        sec_label_list->last = entry;
        entry->next = NULL;
    } else if( entry->offset < sec_label_list->first->offset ) {
        entry->next = sec_label_list->first;
        sec_label_list->first = entry;
    } else {
        // fixme: this shouldn't happen too often
        // if it does, change to a skip list
        entry = insertLabelInMiddle( sec_label_list, entry );
    }
    // add entry to list
    if( sym_hnd != 0 ) {
        HashTableInsert( SymbolToLabelTable, (hash_value) sym_hnd, (hash_data) entry );
    }
    return( entry );
}

bool NeedsQuoting( const char *name )
{
    // If the name contains funny characters and we are producing
    // assemblable output, put back-quotes around it.

    if( !(DFormat & DFF_ASM) ) return( FALSE );
    if( isdigit( *name ) ) return( TRUE );
    while( *name ) {
        if( isalnum( *name ) || *name == '_' || *name == '?' || *name == '$' ) {
            /* OK character */
        } else if( *name == '.' && !IsMasmOutput() ) {
            /* OK character */
        } else {
            /* character needs quoting */
            return( TRUE );
        }
        name++;
    }
    return( FALSE );
}

orl_return CreateNamedLabel( orl_symbol_handle sym_hnd )
{
    hash_data *         data_ptr;
    label_list          sec_label_list;
    label_entry         entry;
    orl_symbol_type     type;
    orl_symbol_type     primary_type;
    orl_sec_handle      sec;
    char *              SourceName;
    char *              LabName;

    type = ORLSymbolGetType( sym_hnd );
    primary_type = type & 0xFF;
    switch( primary_type ) {
// No harm in including these since elf generates relocs to these.
//      case ORL_SYM_TYPE_NONE:
//      case ORL_SYM_TYPE_FUNC_INFO:
//          return( ORL_OKAY );
        case ORL_SYM_TYPE_FILE:
            SourceName = ORLSymbolGetName( sym_hnd );
            if( (SourceName != NULL) && (SourceFileInObject == NULL) ) {
                SourceFileInObject = SourceName;
            }
            return( ORL_OKAY );
    }
    entry = MemAlloc( sizeof( label_entry_struct ) );
    if( !entry ) return( ORL_OUT_OF_MEMORY );
    entry->offset = (orl_sec_offset) ORLSymbolGetValue( sym_hnd );
    // all symbols from the object file will have names
    entry->shnd = ORLSymbolGetSecHandle( sym_hnd );
    if( primary_type == ORL_SYM_TYPE_SECTION ) {
        entry->type = LTYP_SECTION;
    } else if( primary_type == ORL_SYM_TYPE_GROUP ) {
        entry->type = LTYP_GROUP;
    } else if( entry->shnd == 0 ) {
        entry->type = LTYP_EXTERNAL_NAMED;
    } else if( primary_type == ORL_SYM_TYPE_FUNC_INFO ){
        entry->type = LTYP_FUNC_INFO;
    } else {
        entry->type = LTYP_NAMED;
    }
    entry->binding = ORLSymbolGetBinding( sym_hnd );
    LabName = ORLSymbolGetName( sym_hnd );
    if( LabName == NULL ) {
        sec = ORLSymbolGetSecHandle( sym_hnd );
        if( sec ) {
            LabName = ORLSecGetName( sec );
        } else {
            MemFree( entry );
            return( ORL_OKAY );
        }
    }

    // Demangle the name, if necessary
    if( !((Options & NODEMANGLE_NAMES) || (DFormat & DFF_ASM)) ) {
        entry->label.name = MemAlloc( MAX_LINE_LEN + 3 );
        __demangle_l( LabName, 0, &(entry->label.name[2]), MAX_LINE_LEN );
    } else {
        entry->label.name = MemAlloc( strlen( LabName )+8 );
        strcpy( &(entry->label.name[2]), LabName );
    }

    entry->label.name[0]=0;
    entry->label.name[1]=0;
    LabName = &(entry->label.name[2]);
    if( NeedsQuoting( LabName ) ) {
        // entry->label.name[-1] will be 1 if we have added a quote,
        // 0 otherwise.  This is helpful when freeing the memory.
        entry->label.name[0] = 1;
        entry->label.name[1] = '`';
        entry->label.name += 1;
        LabName += strlen( LabName );
        LabName[0] = '`';
        LabName[1] = '\0';
    } else {
        entry->label.name += 2;
    }
    data_ptr = HashTableQuery( HandleToLabelListTable, (hash_value) entry->shnd );
    if( data_ptr ) {
        sec_label_list = (label_list) *data_ptr;
        entry = addLabel( sec_label_list, entry, sym_hnd );
        if( Options & PRINT_PUBLICS && entry->shnd != 0 &&
                primary_type != ORL_SYM_TYPE_SECTION &&
                entry->binding != ORL_SYM_BINDING_LOCAL ) {
            Publics.number++;
        }
    } else {
        // error!!!! the label list should have been created
        MemFree( entry );
        return( ORL_ERROR );
    }
    return( ORL_OKAY );
}

orl_return DealWithSymbolSection( orl_sec_handle shnd )
{
    orl_return error;

    error = ORLSymbolSecScan( shnd, CreateNamedLabel );
    return( error );
}

void CreateUnnamedLabel( orl_sec_handle shnd, orl_sec_offset loc, unnamed_label_return return_struct )
{
    label_list          sec_label_list;
    hash_data *         data_ptr;
    label_entry         entry;

    entry = MemAlloc( sizeof( label_entry_struct ) );
    if( !entry ) {
        return_struct->error = OUT_OF_MEMORY;
        return;
    }
    entry->offset = loc;
    entry->type = LTYP_UNNAMED;
    entry->label.number = 0;
    entry->shnd = shnd;
    data_ptr = HashTableQuery( HandleToLabelListTable, (hash_value) shnd );
    if( data_ptr ) {
        sec_label_list = (label_list) *data_ptr;
        entry = addLabel( sec_label_list, entry, 0 );
        return_struct->entry = entry;
        return_struct->error = OKAY;
    } else {
        // error!!!! the label list should have been created
        return_struct->error = ERROR;
    }
    return;
}
