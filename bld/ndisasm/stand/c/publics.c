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
#include "global.h"
#include "publics.h"
#include "buffer.h"
#include "memfuncs.h"
#include "hashtabl.h"
#include "main.h"
#include "print.h"

#define SECTION_TAB_POS 3
#define ADDRESS_TAB_POS 6

extern hash_table       HandleToSectionTable;
extern publics_struct   Publics;

static int alpha_compare( const void *entry1, const void *entry2 )
{
    return( stricmp( (*(label_entry *)entry1)->label.name, (*(label_entry *)entry2)->label.name ) );
}

void CreatePublicsArray( void )
{
    label_list_ptr      ptr;
    label_entry         entry;
    int                 index = 0;

    if( Publics.number ) {
        Publics.public_symbols = (label_entry *) MemAlloc( sizeof( label_entry ) * Publics.number );
        ptr = Publics.label_lists;
        while( ptr ) {
            entry = ptr->list->first;
            while( entry ) {
                while( entry && ( entry->type == LTYP_SECTION ||
                            entry->binding == ORL_SYM_BINDING_LOCAL ) ) {
                    entry = entry->next;
                }
                if( !entry ) break;
                Publics.public_symbols[index] = entry;
                index++;
                entry = entry->next;
            }
            ptr = ptr->next;
        }
        // now sort!
        qsort( Publics.public_symbols, Publics.number, sizeof( label_entry * ), alpha_compare );
    }
}

void PrintPublics( void ) {
    int                 loop;
    label_entry         entry;
    hash_data *         data_ptr;
    section_ptr         sec;

    // fixme:  data labels get a _ in front, others one after ??
    BufferMsg( LIST_OF_PUBLICS );
    BufferMsg( SYMBOL );
    BufferConcat("\t\t\t");
    BufferMsg( SECTION );
    BufferConcat("\t\t\t");
    BufferMsg( OFFSET );
    BufferConcatNL();
    BufferPrint();
    Print( "--------------------------------------------------------\n" );
    for( loop = 0; loop < Publics.number; loop++ ) {
        entry = Publics.public_symbols[loop];
        BufferConcat( entry->label.name );
        data_ptr = HashTableQuery( HandleToSectionTable, (hash_value) entry->shnd );
        // fixme: what is the proper behavour if no section found??
        if( data_ptr ) {
            sec = (section_ptr) *data_ptr;
            BufferAlignToTab( SECTION_TAB_POS );
            BufferConcat( sec->name );
        }
        BufferAlignToTab( ADDRESS_TAB_POS );
        BufferStore( "%08X\n", entry->offset );
        BufferPrint();
    }
}
