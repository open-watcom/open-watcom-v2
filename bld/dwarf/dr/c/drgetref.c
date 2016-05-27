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
    drmem_hdl entry;
} ToData;

typedef struct {
    drmem_hdl entry;
} ByData;

typedef struct {
    dr_sym_type search;
} RefData;

typedef bool (*hook_func)( dr_ref_info *, void * );

#define SCOPE_GUESS 0x50

static void ScopePush( dr_scope_stack *stack, drmem_hdl entry )
/*************************************************************/
{
    if( stack->stack == NULL ) {
        stack->stack = DWRALLOC( SCOPE_GUESS * sizeof( drmem_hdl ) );
    }
    if( stack->free >= stack->size ) {
        stack->size += SCOPE_GUESS;
        stack->stack = DWRREALLOC( stack->stack, stack->size * sizeof( drmem_hdl ) );
    }

    stack->stack[stack->free] = entry;
    stack->free += 1;
}

static drmem_hdl ScopePop( dr_scope_stack *stack )
/************************************************/
{
    if( stack->free <= 0 ) {
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }

    stack->free -= 1;
    return( stack->stack[stack->free] );
}

static drmem_hdl ScopeLastNameable( dr_scope_stack *scope, char **name )
/**********************************************************************/
{
    int             i;
    drmem_hdl       tmp_entry;
    drmem_hdl       abbrev;

    for( i = scope->free; i > 0; i -= 1 ) {
        tmp_entry = scope->stack[i - 1];

        if( !DWRReadTagEnd( &tmp_entry, &abbrev, NULL ) ) {
            abbrev++;   /* skip child flag */
            *name = DWRGetName( abbrev, tmp_entry );
            if( *name != NULL ) {
                return( scope->stack[i - 1] );
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
            && reg->scope.stack[reg->scope.free - 1] == info->entry );
}

static bool ByHook( dr_ref_info *registers, void * data )
/*******************************************************/
{
    return( registers->dependent == ((ByData*)data)->entry );
}


static void References( ReferWhich which, drmem_hdl entry, void *data1,
                hook_func do_callback, void *data2, DRSYMREF callback )
/*********************************************************************/
{
    drmem_hdl   loc;
    drmem_hdl   end;
    drmem_hdl   owning_node;
    drmem_hdl   infoOffset;
    unsigned_8  opcode;
    dr_ref_info registers = { { 0, 0, NULL }, 0L, NULL, 1L, 1 };
    bool        quit = false;
    bool        inScope = false;

    loc = DWRCurrNode->sections[ DR_DEBUG_REF ].base;
    end = loc + DWRCurrNode->sections[ DR_DEBUG_REF ].size;
    infoOffset = DWRCurrNode->sections[ DR_DEBUG_INFO ].base;

    loc += sizeof( unsigned_32 );   /* skip size */
    while( loc < end && !quit ) {
        opcode = DWRVMReadByte( loc );
        loc += sizeof( unsigned_8 );

        switch( opcode ) {
        case REF_BEGIN_SCOPE:
            owning_node = DWRVMReadDWord( loc ) + infoOffset;
            loc += sizeof( unsigned_32 );
            ScopePush( &registers.scope, owning_node );
            if( (which & REFERSTO) != 0 && owning_node == entry ) {
                inScope = true;
            }
            break;

        case REF_END_SCOPE:
            ScopePop( &registers.scope );
            inScope = false;
            break;

        case REF_SET_FILE:
            registers.file = DWRFindFileName( DWRVMReadULEB128( &loc ), infoOffset );
            break;

        case REF_SET_LINE:
            registers.line = DWRVMReadULEB128( &loc );
            break;

        case REF_SET_COLUMN:
            registers.column = (unsigned_8)DWRVMReadULEB128( &loc );
            break;

        case REF_ADD_LINE:
            registers.line += DWRVMReadSLEB128( &loc );
            registers.column = 0;
            break;

        case REF_ADD_COLUMN:
            registers.column += (signed_8)DWRVMReadSLEB128( &loc );
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

            quit = false; /* don't terminate */
            if( do_callback( &registers, data1 ) || inScope ) {
                char    *name = NULL;

                owning_node = ScopeLastNameable( &registers.scope, &name );

                /* make sure that there is something nameable on the stack */
                if( owning_node != DRMEM_HDL_NULL ) {
                    quit = !callback( owning_node, &registers, name, data2 );
                }
            }
            break;
        }
    }

    DWRFREE( registers.scope.stack );
}

void DRRefersTo( drmem_hdl entry, void *data, DRSYMREF callback )
/***************************************************************/
{
    ToData info;
    info.entry = entry;
    References( REFERSTO, entry, &info, ToHook, data, callback );
}

void DRReferredToBy( drmem_hdl entry, void * data, DRSYMREF callback )
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
