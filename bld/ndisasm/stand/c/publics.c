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
#include "dis.h"
#include "global.h"
#include "publics.h"
#include "buffer.h"
#include "memfuncs.h"
#include "hashtabl.h"
#include "main.h"
#include "print.h"

#include "clibext.h"


#define SECTION_TAB_POS 3
#define ADDRESS_TAB_POS 6

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
        Publics.public_symbols = (label_entry *)MemAlloc( sizeof( label_entry ) * Publics.number );
        for( ptr = Publics.label_lists; ptr != NULL; ptr = ptr->next ) {
            for( entry = ptr->list->first; entry != NULL; entry = entry->next ) {
                for( ; entry != NULL; entry = entry->next ) {
                    if( entry->type != LTYP_SECTION && entry->binding != ORL_SYM_BINDING_LOCAL ) {
                        break;
                    }
                }
                if( entry == NULL )
                    break;
                Publics.public_symbols[index] = entry;
                index++;
            }
        }
        // now sort!
        qsort( Publics.public_symbols, Publics.number, sizeof( label_entry * ), alpha_compare );
    }
}

void PrintPublics( void )
{
    int                 loop;
    label_entry         entry;
    hash_data           *h_data;
    section_ptr         section;
    hash_key            h_key;

    // fixme:  data labels get a _ in front, others one after ??
    BufferMsg( LIST_OF_PUBLICS );
    BufferConcatNL();
    BufferConcatNL();
    BufferMsg( SYMBOL );
    BufferConcat( "\t\t\t" );
    BufferMsg( SECTION );
    BufferConcat( "\t\t\t" );
    BufferMsg( OFFSET );
    BufferConcatNL();
    BufferPrint();
    Print( "--------------------------------------------------------\n" );
    for( loop = 0; loop < Publics.number; loop++ ) {
        entry = Publics.public_symbols[loop];
        BufferQuoteName( entry->label.name );
        h_key.u.sec_handle = entry->shnd;
        h_data = HashTableQuery( HandleToSectionTable, h_key );
        // fixme: what is the proper behavour if no section found??
        if( h_data != NULL ) {
            section = h_data->u.section;
            BufferAlignToTab( SECTION_TAB_POS );
            BufferConcat( section->name );
        }
        BufferAlignToTab( ADDRESS_TAB_POS );
        BufferHexU32( 8, entry->offset );
        BufferConcatNL();
        BufferPrint();
    }
}
