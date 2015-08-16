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
* Description:  Processing of .WATCOM_references section (for browser).
*
****************************************************************************/


#include "drpriv.h"
#include "drutils.h"
#include "drgetref.h"
#include "drgettab.h"

typedef enum {
    REFERSTO   = 0x01,
    REFERREDBY = 0x02
} ReferWhich;

typedef struct {
    dr_handle entry;
} ToData;

typedef struct {
    dr_handle entry;
} ByData;

typedef struct {
    dr_sym_type search;
} RefData;

typedef bool (*hook_func)( dr_ref_info *, void * );

#define SCOPE_GUESS 0x50

static void ScopePush( dr_scope_stack * stack, dr_handle entry )
/**************************************************************/
{
    if( stack->stack == NULL ) {
        stack->stack = DWRALLOC( SCOPE_GUESS * sizeof( dr_handle ) );
    }
    if( stack->free >= stack->size ) {
        stack->size += SCOPE_GUESS;
        stack->stack = DWRREALLOC( stack->stack, stack->size * sizeof( dr_handle ) );
    }

    stack->stack[ stack->free ] = entry;
    stack->free += 1;
}

static dr_handle ScopePop( dr_scope_stack * stack )
/*************************************************/
{
    if( stack->free <= 0 ) {
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }

    stack->free -= 1;
    return( stack->stack[ stack->free ] );
}

static dr_handle ScopeLastNameable( dr_scope_stack * scope, char ** name )
/************************************************************************/
{
    int             i;
    dr_handle       tmp_entry;
    dr_handle       abbrev;
    dr_abbrev_idx   abbrev_idx;

    for( i = scope->free; i > 0; i -= 1 ) {
        tmp_entry = scope->stack[ i - 1 ];

        abbrev_idx = DWRVMReadULEB128( &tmp_entry );
        if( abbrev_idx != 0 ) {
            abbrev = DWRLookupAbbrev( tmp_entry, abbrev_idx );
            DWRVMSkipLEB128( &abbrev );
            abbrev += sizeof( unsigned_8 );

            *name = DWRGetName( abbrev, tmp_entry );
            if( *name != NULL ) {
                return( scope->stack[ i - 1 ] );
            }
        }
    }
    return( 0 );
}

static bool ToHook( dr_ref_info *reg, void *data )
/************************************************/
{
    ToData  *info = (ToData *)data;

    return( (reg->scope.free > 0)
            && reg->scope.stack[ reg->scope.free - 1 ] == info->entry );
}

static bool ByHook( dr_ref_info *registers, void * data )
/*******************************************************/
{
    return( registers->dependent == ((ByData*)data)->entry );
}


static void References( ReferWhich which, dr_handle entry, void *data1,
                hook_func do_callback, void *data2, DRSYMREF callback )
/*********************************************************************/
{
    dr_handle   loc;
    dr_handle   end;
    dr_handle   owning_node;
    dr_handle   infoOffset;
    unsigned_8  opcode;
    dr_ref_info registers = { { 0, 0, NULL }, 0L, NULL, 1L, 1 };
    bool        quit = FALSE;
    bool        inScope = FALSE;

    loc = DWRCurrNode->sections[ DR_DEBUG_REF ].base;
    end = loc + DWRCurrNode->sections[ DR_DEBUG_REF ].size;
    infoOffset = DWRCurrNode->sections[ DR_DEBUG_INFO ].base;

    loc += sizeof( unsigned_32 );   /* skip size */
    while( loc < end  && !quit ) {
        opcode = DWRVMReadByte( loc );
        loc += sizeof( unsigned_8 );

        switch( opcode ) {
        case REF_BEGIN_SCOPE:
            owning_node = DWRVMReadDWord( loc ) + infoOffset;
            loc += sizeof( unsigned_32 );
            ScopePush( &registers.scope, owning_node );
            if( which & REFERSTO && owning_node == entry ) {
                inScope = TRUE;
            }
            break;

        case REF_END_SCOPE:
            ScopePop( &registers.scope );
            inScope = FALSE;
            break;

        case REF_SET_FILE:
            registers.file = DWRFindFileName( DWRVMReadULEB128( &loc ), infoOffset );
            break;

        case REF_SET_LINE:
            registers.line = DWRVMReadULEB128( &loc );
            break;

        case REF_SET_COLUMN:
            registers.column = DWRVMReadULEB128( &loc );
            break;

        case REF_ADD_LINE:
            registers.line += DWRVMReadSLEB128( &loc );
            registers.column = 0;
            break;

        case REF_ADD_COLUMN:
            registers.column += DWRVMReadSLEB128( &loc );
            break;

        case REF_COPY:
        default:
            if( opcode >= REF_CODE_BASE ) {
                unsigned_32 ld;

                opcode -= REF_CODE_BASE;

                ld = opcode / REF_COLUMN_RANGE;
                if( ld != 0 ) {
                    registers.column = 0;
                    registers.line += ld;
                }
                registers.column += opcode % REF_COLUMN_RANGE;

                registers.dependent = DWRVMReadDWord( loc ) + infoOffset;
                loc += sizeof( unsigned_32 );
            }

            quit = FALSE; /* don't terminate */
            if( do_callback( &registers, data1 ) || inScope ) {
                char    *name = NULL;

                owning_node = ScopeLastNameable( &registers.scope, &name );

                /* make sure that there is something nameable on the stack */
                if( owning_node != DR_HANDLE_NUL ) {
                    quit = !callback( owning_node, &registers, name, data2 );
                }
            }
            break;
        }
    }

    DWRFREE( registers.scope.stack );
}

void DRRefersTo( dr_handle entry, void *data, DRSYMREF callback )
/***************************************************************/
{
    ToData info;
    info.entry = entry;
    References( REFERSTO, entry, &info, ToHook, data, callback );
}

void DRReferredToBy( dr_handle entry, void * data, DRSYMREF callback )
/********************************************************************/
{
    ByData info;
    info.entry = entry;
    References( REFERREDBY, entry, &info, ByHook, data, callback );
}

static bool RefHook( dr_ref_info * reg, void * data )
/***************************************************/
{
    RefData * info = (RefData *) data;

    return( DRGetSymType( reg->dependent ) == info->search );
}

void DRReferencedSymbols( dr_sym_type search, void * data, DRSYMREF callback )
/****************************************************************************/
{
    RefData info;
    info.search = search;

    References( REFERREDBY, 0L, &info, RefHook, data, callback );
}
