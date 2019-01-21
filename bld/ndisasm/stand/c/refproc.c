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
#include <string.h>
#include "dis.h"
#include "global.h"
#include "refproc.h"
#include "hashtabl.h"
#include "memfuncs.h"
#include "formasm.h"
#include "main.h"
#include "init.h"

#include "clibext.h"


static void addRef( ref_list sec_ref_list, ref_entry entry )
{
    ref_entry                   walker;

    if( sec_ref_list->first == NULL ) {
        sec_ref_list->first = entry;
        sec_ref_list->last = entry;
        entry->next = NULL;
    } else if( entry->offset >= sec_ref_list->last->offset ) {
        sec_ref_list->last->next = entry;
        sec_ref_list->last = entry;
        entry->next = NULL;
    } else if( entry->offset <= sec_ref_list->first->offset ) {
        entry->next = sec_ref_list->first;
        sec_ref_list->first = entry;
    } else {
        // fixme: this shouldn't happen too often
        // if it does, change to a skip list
        walker = sec_ref_list->first;
        while( walker->next->offset <= entry->offset ) {
            walker = walker->next;
        }
        entry->next = walker->next;
        walker->next = entry;
    }
}

static const char *getFrameModifier( orl_reloc rel )
{
    orl_sec_handle      shnd;
    orl_group_handle    grp;
    orl_symbol_type     typ;
    const char          *name;

    if( rel->symbol == rel->frame ) {
        /* FRAME = TARGET
         */
        if( GetFormat() == ORL_OMF )
            return( NULL );
        typ = ORLSymbolGetType( rel->symbol );
        if( typ & ORL_SYM_TYPE_SECTION ) {
            shnd = ORLSymbolGetSecHandle( rel->symbol );
            if( shnd != ORL_NULL_HANDLE ) {
                grp = ORLSecGetGroup( shnd );
                if( grp != ORL_NULL_HANDLE ) {
                    return( ORLGroupName( grp ) );
                }
            }
        }
    } else {
        /* frame is not target and may require modifier
         */
        typ = ORLSymbolGetType( rel->frame );
        if( typ & ( ORL_SYM_TYPE_SECTION | ORL_SYM_TYPE_GROUP ) ) {
            name = ORLSymbolGetName( rel->frame );
            if( name != NULL && stricmp( name, "FLAT" ) == 0 ) {
                UseFlatModel();
                if( (DFormat & DFF_ASM) == 0 ) {
                    return( NULL );
                }
            }
            return( name );
        }
    }
    return( NULL );
}


orl_return CreateNamedLabelRef( orl_reloc rel )
{
    ref_entry           ref;
    hash_data           *h_data;
    ref_list            sec_ref_list;
    hash_key            h_key;

    if( rel->type == ORL_RELOC_TYPE_PAIR )
        return( ORL_OKAY );
    ref = MemAlloc( sizeof( ref_entry_struct ) );
    if( ref == NULL )
        return( ORL_OUT_OF_MEMORY );
    memset( ref, 0, sizeof( ref_entry_struct ) );
    ref->offset = rel->offset;
    ref->type = rel->type;
    ref->addend = rel->addend;
    if( IsMasmOutput() && rel->frame ) {
        ref->frame = getFrameModifier( rel );
    }
    h_key.u.sym_handle = rel->symbol;
    h_data = HashTableQuery( SymbolToLabelTable, h_key );
    if( h_data != NULL ) {
        ref->label = h_data->u.lab_entry;
        h_key.u.sec_handle = rel->section;
        h_data = HashTableQuery( HandleToRefListTable, h_key );
        if( h_data != NULL ) {
            ref->has_val = true;
            sec_ref_list = h_data->u.sec_ref_list;
            addRef( sec_ref_list, ref );
            return( ORL_OKAY );
        }
        // error!!!!  should have been created
    }
    MemFree( ref );
    return( ORL_ERROR );
}

orl_return DealWithRelocSection( orl_sec_handle shnd )
{
    orl_return  error;

    error = ORLRelocSecScan( shnd, CreateNamedLabelRef );
    return( error );
}

return_val CreateUnnamedLabelRef( orl_sec_handle shnd, label_entry entry, dis_sec_offset loc )
{
    ref_entry           ref;
    hash_data           *h_data;
    ref_list            sec_ref_list;
    hash_key            h_key;

    ref = MemAlloc( sizeof( ref_entry_struct ) );
    if( ref == NULL ) {
        return( RC_OUT_OF_MEMORY );
    }
    memset( ref, 0, sizeof( ref_entry_struct ) );
    ref->offset = loc;
    ref->label = entry;
    ref->type = ORL_RELOC_TYPE_JUMP;
    ref->addend = 0;
    h_key.u.sec_handle = shnd;
    h_data = HashTableQuery( HandleToRefListTable, h_key );
    if( h_data != NULL ) {
        ref->has_val = true;
        sec_ref_list = h_data->u.sec_ref_list;
        addRef( sec_ref_list, ref );
        return( RC_OKAY );
    }
    // error!!!!  should have been created
    MemFree( ref );
    return( RC_ERROR );
}

return_val CreateAbsoluteLabelRef( orl_sec_handle shnd, label_entry entry, dis_sec_offset loc )
{
    ref_entry           ref;
    hash_data           *h_data;
    ref_list            sec_ref_list;
    hash_key            h_key;

    ref = MemAlloc( sizeof( ref_entry_struct ) );
    if( ref == NULL ) {
        return( RC_OUT_OF_MEMORY );
    }
    memset( ref, 0, sizeof( ref_entry_struct ) );
    ref->offset = loc;
    ref->label = entry;
    ref->type = ORL_RELOC_TYPE_MAX + 1;
    h_key.u.sec_handle = shnd;
    h_data = HashTableQuery( HandleToRefListTable, h_key );
    if( h_data != NULL ) {
        ref->has_val = true;
        sec_ref_list = h_data->u.sec_ref_list;
        addRef( sec_ref_list, ref );
        return( RC_OKAY );
    }
    // error!!!!  should have been created
    MemFree( ref );
    return( RC_ERROR );
}
