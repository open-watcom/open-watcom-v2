/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
#include "wio.h"
#include "dis.h"
#include "global.h"
#include "fini.h"
#include "memfuncs.h"
#include "hashtabl.h"
#include "identsec.h"
#include "init.h"
#include "main.h"
#include "labproc.h"


static void freeScanTabList( scantab_ptr st )
{
    scantab_ptr         next;

    for( ; st != NULL; st = next ) {
        next = st->next;
        MemFree( st );
    }
}

static void freeSectionList( section_list_struct *list )
{
    section_ptr     next_section;

    for( ; list->first != NULL; list->first = next_section ) {
        next_section = list->first->next;
        freeScanTabList( list->first->scan );
        MemFree( list->first );
    }
}

static void freeLabelList( orl_sec_handle shnd )
{
    hash_data           *h_data;
    label_list          sec_label_list;
    label_entry         entry;
    label_entry         next;
    hash_key            h_key;

    if( HandleToLabelListTable != NULL ) {
        h_key.u.sec_handle = shnd;
        h_data = HashTableQuery( HandleToLabelListTable, h_key );
        if( h_data != NULL ) {
            sec_label_list = h_data->u.sec_label_list;
            for( entry = sec_label_list->first; entry != NULL; entry = next ) {
                next = entry->next;
                FreeLabel( entry );
            }
            MemFree( sec_label_list );
        }
    }
}

static void freeRefList( orl_sec_handle shnd )
{
    hash_data           *h_data;
    ref_list            sec_ref_list;
    ref_entry           entry;
    ref_entry           next;
    hash_key            h_key;

    h_key.u.sec_handle = shnd;
    h_data = HashTableQuery( HandleToRefListTable, h_key );
    if( h_data != NULL ) {
        sec_ref_list = h_data->u.sec_ref_list;
        for( entry = sec_ref_list->first; entry != NULL; entry = next ) {
            next = entry->next;
            MemFree( entry );
        }
        MemFree( sec_ref_list );
    }
}

static orl_return SectionFini( orl_sec_handle shnd )
{
    section_type        type;

    type = IdentifySec( shnd );
    switch( type ) {
    case SECTION_TYPE_TEXT:
    case SECTION_TYPE_BSS:
    case SECTION_TYPE_DATA:
    case SECTION_TYPE_DRECTVE:
    case SECTION_TYPE_PDATA:
    default:
        freeRefList( shnd );
        freeLabelList( shnd );
        break;
    }
    return( ORL_OKAY );
}

static void freePublics( void )
{
    label_list_ptr      ptr;

    while( (ptr = Publics.label_lists) != NULL ) {
        Publics.label_lists = Publics.label_lists->next;
        MemFree( ptr );
    }
    if( Publics.public_symbols != NULL ) {
        MemFree( Publics.public_symbols );
    }
}

void FreeHashTables( void )
{
    HashTableFree( HandleToSectionTable );
    HashTableFree( SymbolToLabelTable );
    HashTableFree( HandleToLabelListTable );
    HashTableFree( HandleToRefListTable );
    HashTableFree( NameRecognitionTable );
    HashTableFree( SkipRefTable );
}

void FreeServicesUsed( void )
{
    orl_return          status;

    DisFini( &DHnd );
    status = ORLFini( ORLHnd );
    if( status != ORL_OKAY ) {
        PrintErrorMsg( RC_ERROR, WHERE_CLOSING_ORL );
    }
}

void CloseFiles( void )
{
    CloseObjFile();
    if( ListFileName != NULL ) {
        fclose( OutputDest );
        MemFree( ListFileName );
    }
    if( SourceFileInDwarf != NULL ) {
        MemFree( SourceFileInDwarf );
    }
    if( SourceFileName != NULL ) {
        MemFree( SourceFileName );
    }
}

void Fini( void )
{
    freeSectionList( &Sections );
    freeLabelList( 0 );
    if( ObjFileHnd != ORL_NULL_HANDLE ) {
        ORLFileScan( ObjFileHnd, NULL, SectionFini );
        if( Options & PRINT_PUBLICS ) {
            freePublics();
        }
        FreeServicesUsed();
    }
    FreeHashTables();
    CloseFiles();
    MemPrtList();
    MsgFini();
    MemClose();
}
