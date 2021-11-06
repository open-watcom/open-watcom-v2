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
#include "dis.h"
#include "global.h"
#include "demangle.h"
#include "labproc.h"
#include "memfuncs.h"
#include "hashtabl.h"
#include "formasm.h"


static void labelNameAlloc( label_entry entry, const char *name )
{
    size_t  len;

    // Demangle the name, if necessary
    if( !((Options & NODEMANGLE_NAMES) || (DFormat & DFF_ASM)) ) {
        len = __demangle_l( name, 0, NULL, 0 );
        entry->label.name = MemAlloc( len + 1 );
        __demangle_l( name, 0, entry->label.name, len + 1 );
    } else {
        len = strlen( name );
        entry->label.name = MemAlloc( len + 1 );
        strcpy( entry->label.name, name );
    }
}

void FreeLabel( label_entry entry )
{
    switch( entry->type ) {
    case LTYP_UNNAMED:
    case LTYP_ABSOLUTE:
        break;
    default:
        if( entry->label.name != NULL ) {
            MemFree( entry->label.name );
        }
        break;
    }
    MemFree( entry );
}

static label_entry resolveTwoLabelsAtLocation( label_list sec_label_list, label_entry entry, label_entry previous_entry, label_entry old_entry )
{
    label_entry first_entry = old_entry;

    for( ; old_entry != NULL && old_entry->offset == entry->offset; old_entry = old_entry->next ) {
        if( entry->type == LTYP_UNNAMED ) {
            if( old_entry->type == LTYP_UNNAMED ) {
                FreeLabel( entry );
                return( old_entry );
            } else if( old_entry->type == LTYP_NAMED ) {
                FreeLabel( entry );
                return( old_entry );
            } else if( old_entry->type == LTYP_ABSOLUTE ) {
            } else if( old_entry->type == LTYP_FUNC_INFO ) {
            } else if( old_entry->type == LTYP_SECTION ) {
                if( !IsMasmOutput() ) {
                    FreeLabel( entry );
                    return( old_entry );
                }
            } else {
                FreeLabel( entry );
                return( old_entry );
            }
        } else if( entry->type == LTYP_ABSOLUTE ) {
            if( old_entry->type == LTYP_ABSOLUTE ) {
                // merge entry into old_entry
                FreeLabel( entry );
                return( old_entry );
            }
        } else if( entry->type == LTYP_EXTERNAL_NAMED ) {
            break;
        } else {
        }
    }
    // two labels for this location!
    entry->next = first_entry;
    if( previous_entry != NULL ) {
        previous_entry->next = entry;
    } else {
        sec_label_list->first = entry;
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

static label_entry addLabel( label_list sec_label_list, label_entry entry, orl_symbol_handle sym_hnd )
{
    hash_entry_data key_entry;

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
    if( sym_hnd != ORL_NULL_HANDLE ) {
        key_entry.key.u.sym_handle = sym_hnd;
        key_entry.data.u.lab_entry = entry;
        HashTableInsert( SymbolToLabelTable, &key_entry );
    }
    return( entry );
}

bool NeedsQuoting( const char *name )
{
    // If the name contains funny characters and we are producing
    // assemblable output, put back-quotes around it.

    if( (DFormat & DFF_ASM) == 0 )
        return( false );
    if( isdigit( *name ) )
        return( true );
    for( ; *name != '\0'; ++name ) {
        if( isalnum( *name ) || *name == '_' || *name == '?' || *name == '$' ) {
            /* OK character */
        } else if( *name == '.' && !IsMasmOutput() ) {
            /* OK character */
        } else {
            /* character needs quoting */
            return( true );
        }
    }
    return( false );
}

orl_return CreateNamedLabel( orl_symbol_handle sym_hnd )
{
    hash_data           *h_data;
    label_list          sec_label_list;
    label_entry         entry;
    orl_symbol_type     type;
    orl_symbol_type     primary_type;
    orl_sec_handle      shnd;
    const char          *source_name;
    const char          *label_name;
    unsigned_64         val64;
    hash_key            h_key;

    type = ORLSymbolGetType( sym_hnd );
    primary_type = type & 0xFF;
    switch( primary_type ) {
// No harm in including these since elf generates relocs to these.
//    case ORL_SYM_TYPE_NONE:
//    case ORL_SYM_TYPE_FUNC_INFO:
//        return( ORL_OKAY );
    case ORL_SYM_TYPE_FILE:
        source_name = ORLSymbolGetName( sym_hnd );
        if( ( source_name != NULL ) && ( SourceFileInObject == NULL ) ) {
            SourceFileInObject = source_name;
        }
        return( ORL_OKAY );
    }
    entry = MemAlloc( sizeof( label_entry_struct ) );
    if( entry == NULL )
        return( ORL_OUT_OF_MEMORY );
    ORLSymbolGetValue( sym_hnd, &val64 );
    entry->offset = val64.u._32[I64LO32];
    // all symbols from the object file will have names
    entry->shnd = ORLSymbolGetSecHandle( sym_hnd );
    if( primary_type == ORL_SYM_TYPE_SECTION ) {
        entry->type = LTYP_SECTION;
    } else if( primary_type == ORL_SYM_TYPE_GROUP ) {
        entry->type = LTYP_GROUP;
    } else if( entry->shnd == ORL_NULL_HANDLE ) {
        entry->type = LTYP_EXTERNAL_NAMED;
    } else if( primary_type == ORL_SYM_TYPE_FUNC_INFO ){
        entry->type = LTYP_FUNC_INFO;
    } else {
        entry->type = LTYP_NAMED;
    }
    entry->binding = ORLSymbolGetBinding( sym_hnd );
    label_name = ORLSymbolGetName( sym_hnd );
    if( label_name == NULL ) {
        shnd = ORLSymbolGetSecHandle( sym_hnd );
        if( shnd == ORL_NULL_HANDLE ) {
            entry->label.name = NULL;
            FreeLabel( entry );
            return( ORL_OKAY );
        }
        label_name = ORLSecGetName( shnd );
    }
    labelNameAlloc( entry, label_name );

    h_key.u.sec_handle = entry->shnd;
    h_data = HashTableQuery( HandleToLabelListTable, h_key );
    if( h_data == NULL ) {
        // error!!!! the label list should have been created
        FreeLabel( entry );
        return( ORL_ERROR );
    }
    sec_label_list = h_data->u.sec_label_list;
    entry = addLabel( sec_label_list, entry, sym_hnd );
    if( (Options & PRINT_PUBLICS) && entry->shnd != ORL_NULL_HANDLE &&
            primary_type != ORL_SYM_TYPE_SECTION && entry->binding != ORL_SYM_BINDING_LOCAL ) {
        Publics.number++;
    }
    return( ORL_OKAY );
}

orl_return DealWithSymbolSection( orl_sec_handle shnd )
{
    orl_return error;

    error = ORLSymbolSecScan( shnd, CreateNamedLabel );
    return( error );
}

void CreateUnnamedLabel( orl_sec_handle shnd, dis_sec_offset loc, unnamed_label_return return_struct )
{
    label_list          sec_label_list;
    hash_data           *h_data;
    label_entry         entry;
    hash_key            h_key;

    entry = MemAlloc( sizeof( label_entry_struct ) );
    if( entry == NULL ) {
        return_struct->error = RC_OUT_OF_MEMORY;
        return;
    }
    entry->offset = loc;
    entry->type = LTYP_UNNAMED;
    entry->label.number = 0;
    entry->shnd = shnd;
    h_key.u.sec_handle = shnd;
    h_data = HashTableQuery( HandleToLabelListTable, h_key );
    if( h_data != NULL ) {
        sec_label_list = h_data->u.sec_label_list;
        entry = addLabel( sec_label_list, entry, 0 );
        return_struct->entry = entry;
        return_struct->error = RC_OKAY;
    } else {
        // error!!!! the label list should have been created
        MemFree( entry );
        return_struct->error = RC_ERROR;
    }
}

void CreateAbsoluteLabel( orl_sec_handle shnd, dis_sec_offset loc, unnamed_label_return return_struct )
{
    label_list          sec_label_list;
    hash_data           *h_data;
    label_entry         entry;
    hash_key            h_key;

    entry = MemAlloc( sizeof( label_entry_struct ) );
    if( entry == NULL ) {
        return_struct->error = RC_OUT_OF_MEMORY;
        return;
    }
    entry->offset = loc;
    entry->type = LTYP_ABSOLUTE;
    entry->label.number = 0;
    entry->shnd = shnd;
    h_key.u.sec_handle = shnd;
    h_data = HashTableQuery( HandleToLabelListTable, h_key );
    if( h_data != NULL ) {
        sec_label_list = h_data->u.sec_label_list;
        entry = addLabel( sec_label_list, entry, 0 );
        return_struct->entry = entry;
        return_struct->error = RC_OKAY;
    } else {
        // error!!!! the label list should have been created
        MemFree( entry );
        return_struct->error = RC_ERROR;
    }
}
