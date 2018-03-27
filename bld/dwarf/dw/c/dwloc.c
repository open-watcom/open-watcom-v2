/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Location expressions and location lists.
*
****************************************************************************/


#include <stdarg.h>
#include <stddef.h>
#include "dwpriv.h"
#include "dwcliuti.h"
#include "dwutils.h"
#include "dwmem.h"
#include "dwloc.h"


#define ADD_ADDR( cli, loc, amt )                   \
    {                                               \
        dw_loc_offs             __old;              \
                                                    \
        __old = (loc)->addr;                        \
        (loc)->addr += (amt);                       \
        if( loc->addr < __old ) {                   \
            _Abort( ABORT_LOC_EXPR_TOO_LONG );      \
        }                                           \
    }

typedef uint_16                 dw_loc_offs;

typedef struct loc_op {
    struct loc_op               *next;
    unsigned                    size;
    char                        op_code;
    char                        data[1];
} loc_op;
#define BASE_SIZE               (offsetof( loc_op, data ))


struct dw_loc_id {
    loc_op                      *first;
    loc_op                      *last;
    dw_loc_label                labels;
    uint_16                     num_syms;       // number of DWLocStatic's
    dw_loc_offs                 addr;
};


struct dw_loc_label {
    dw_loc_label                next;
    dw_loc_offs                 addr;
};

typedef struct {
    dw_sym_handle               sym;
    dw_reloc_type               kind;
} dw_sym_reloc;

typedef struct list_entry {
    struct list_entry           *next;
    dw_sym_handle               begin;
    dw_sym_handle               end;
    dw_loc_handle               loc;
} list_entry;

struct dw_loc_handle {
    struct dw_loc_handle        *next;
    enum {
        LOC_EXPR,
        LOC_LIST,
        LOC_LIST_REF,
    } is_expr;
    union {
        struct loc_handle_expr {
            dw_loc_offs         size;
            uint_16             num_syms;
            char                expr[1];
        }                       expr;
        dw_out_offset           ref;
        list_entry              *list;

    } u;  // possible variable size so nothing can follow this
};
#define BASE_HANDLE_SIZE        offsetof( struct dw_loc_handle, u )


struct dw_list_id {
    struct dw_loc_handle        hdl;
};


dw_loc_id DWENTRY DWLocInit( dw_client cli )
{
    dw_loc_id       new;

    new = CLIAlloc( cli, sizeof( struct dw_loc_id ) );
    new->first = NULL;
    new->last = NULL;
    new->labels = NULL;
    new->num_syms = 0;
    new->addr = 0;
    return( new );
}


dw_loc_label DWENTRY DWLocNewLabel( dw_client cli, dw_loc_id loc )
{
    dw_loc_label    new;

    new = CarveAlloc( cli, cli->debug_loc.label_carver );
    new->next = loc->labels;
    loc->labels = new;
    new->addr = 0;

    return( new );
}


void DWENTRY DWLocSetLabel( dw_client cli, dw_loc_id loc, dw_loc_label label )
{
    /* unused parameters */ (void)cli;

    label->addr = loc->addr;
}


static loc_op *nextOp( dw_client cli, dw_loc_id loc, uint_8 op_code, uint extra_size )
{
    loc_op          *new;

    new = CLIAlloc( cli, BASE_SIZE + extra_size );
    if( loc->first == NULL ) {
        loc->first = new;
        loc->last = new;
    } else {
        loc->last->next = new;
    }
    loc->last = new;
    new->size = extra_size;
    new->next = NULL;
    new->op_code = op_code;
    ADD_ADDR( cli, loc, 1 );
    return( new );
}


void DWENTRY DWLocReg( dw_client cli, dw_loc_id loc, uint reg )
{
    uint_8          buf[MAX_LEB128];
    loc_op          *op;
    dw_loc_offs     len;

    if( reg < 32 ) {
        op = nextOp( cli, loc, DW_OP_reg( reg ), 0 );
    } else {
        len = (dw_loc_offs)( ULEB128( buf, reg ) - buf );
        op = nextOp( cli, loc, DW_OP_regx, len );
        ADD_ADDR( cli, loc, len );
        memcpy( op->data, buf, len );
    }
}

void DWENTRY DWLocPiece( dw_client cli, dw_loc_id loc, uint size )
{
    uint_8          buf[MAX_LEB128];
    loc_op          *op;
    dw_loc_offs     len;

    len = (dw_loc_offs)( ULEB128( buf, size ) - buf );
    op = nextOp( cli, loc, DW_OP_piece, len );
    ADD_ADDR( cli, loc, len );
    memcpy( op->data, buf, len );
}


void DWENTRY DWLocStatic( dw_client cli, dw_loc_id loc, dw_sym_handle sym )
{
    loc_op          *op;
    dw_sym_reloc    *reloc_info;

    op = nextOp( cli, loc, DW_OP_addr, sizeof( *reloc_info ) );
    ++loc->num_syms;
    reloc_info = (dw_sym_reloc *)op->data;
    reloc_info->sym = sym;
    reloc_info->kind = DW_W_STATIC;
    ADD_ADDR( cli, loc, cli->offset_size );
}

void DWENTRY DWLocSym( dw_client cli, dw_loc_id loc, dw_sym_handle sym, dw_reloc_type kind )
{
    loc_op          *op;
    dw_sym_reloc    *reloc_info;

    op = nextOp( cli, loc, DW_OP_addr, sizeof( *reloc_info ) );
    ++loc->num_syms;
    reloc_info = (dw_sym_reloc *)op->data;
    reloc_info->sym = sym;
    reloc_info->kind = kind;

    if( kind == DW_W_SEGMENT || kind == DW_W_LABEL_SEG ) { ///TODO :better linkage
        ADD_ADDR( cli, loc, GET_SEGMENT_SIZE( cli ) );
    } else {
        ADD_ADDR( cli, loc, cli->offset_size );
    }
}

void DWENTRY DWLocSegment( dw_client cli, dw_loc_id loc, dw_sym_handle sym )
{
    loc_op          *op;
    dw_sym_reloc    *reloc_info;

    op = nextOp( cli, loc, DW_OP_addr, sizeof( *reloc_info ) );
    /*
        We need some way of telling DWLocStatic and DWLocSegment apart;
        so we'll stash the reloc type with the sym
    */
    ++loc->num_syms;
    reloc_info = (dw_sym_reloc *)op->data;
    reloc_info->sym = sym;
    reloc_info->kind = DW_W_SEGMENT;
    ADD_ADDR( cli, loc, GET_SEGMENT_SIZE( cli ) ); // size of info written u2
}


void DWENTRY DWLocConstU( dw_client cli, dw_loc_id loc, dw_uconst value )
{
    uint_8          buf[MAX_LEB128];
#ifndef NDEBUG
    uint_8          *end;
#endif
    loc_op          *op;

    _Validate( loc != NULL );

    /* determine how we'll store this thing */
    if( value < 32UL ) {
        nextOp( cli, loc, DW_OP_lit( value ), 0 );
    } else if( value < 256UL ) {
        op = nextOp( cli, loc, DW_OP_const1u, 1 );
        op->data[0] = (uint_8)value;
        ADD_ADDR( cli, loc, 1 );
    } else if( value < 16384UL ) {
        op = nextOp( cli, loc, DW_OP_const2u, sizeof( uint_16 ) );
        WriteU16( op->data, (uint_16)value );
        ADD_ADDR( cli, loc, sizeof( uint_16 ) );
    } else if( value < ( 1UL << 21 ) ) {
        /* will only take 3 bytes to encode in ULEB128 form */
#ifndef NDEBUG
        end = ULEB128( buf, value );
        _Assert( end - buf == 3 );
#else
        ULEB128( buf, value );
#endif
        op = nextOp( cli, loc, DW_OP_constu, 3 );
        op->data[0] = buf[0];
        op->data[1] = buf[1];
        op->data[2] = buf[2];
        ADD_ADDR( cli, loc, 3 );
    } else {
        op = nextOp( cli, loc, DW_OP_const4u, sizeof( uint_32 ) );
        WriteU32( op->data, value );
        ADD_ADDR( cli, loc, 4 );
    }
}


void DWENTRY DWLocConstS( dw_client cli, dw_loc_id loc, dw_sconst value )
{
    uint_8          buf[MAX_LEB128];
    loc_op          *op;
    dw_loc_offs     len;

    _Validate( loc != NULL );

    /* determine how we'll store this thing */
    if( 0 <= value && value < 32L ) {
        nextOp( cli, loc, DW_OP_lit( value ), 0 );
    } else if( -128L <= value && value < 128L ) {
        op = nextOp( cli, loc, DW_OP_const1s, 1 );
        op->data[0] = (int_8)value;
        ADD_ADDR( cli, loc, 1 );
    } else if( -16384L <= value && value < 16384L ) {
        op = nextOp( cli, loc, DW_OP_const2s, sizeof( int_16 ) );
        WriteS16( op->data, (int_16)value );
        ADD_ADDR( cli, loc, sizeof( int_16 ) );
    } else {
        /* test length of LEB128 form before using DW_OP_const4s */
        len = (dw_loc_offs)( LEB128( buf, value ) - buf );
        if( len > 3 ) {
            op = nextOp( cli, loc, DW_OP_const4s, sizeof( int_32 ) );
            WriteS32( op->data, value );
            ADD_ADDR( cli, loc, sizeof( int_32 ) );
        } else {
            op = nextOp( cli, loc, DW_OP_consts, len );
            memcpy( op->data, buf, len );
            ADD_ADDR( cli, loc, len );
        }
    }
}


static uint_8 const translateOp[] = {
    #define DW_LOC_OP( __n, __v )   __v,
    #include "dwlocop.h"
    #undef DW_LOC_OP
    DW_OP_bregx
};


void DWENTRY DWLocOp0( dw_client cli, dw_loc_id loc, dw_loc_op user_op )
{
    _Validate( loc != NULL );
    if( user_op >= DW_LOC_max )
        _Abort( ABORT_ILLEGAL_LOC_OP );
    nextOp( cli, loc, translateOp[user_op], 0 );
}


void DWENTRY DWLocOp( dw_client cli, dw_loc_id loc, dw_loc_op user_op, ... )
{
    uint_8          buf[2 * MAX_LEB128];
    uint_8          *end;
    uint_8          op_code;
    loc_op          *op;
    va_list         args;
    uint            reg;
    dw_loc_offs     len;

    _Validate( loc != NULL );

    if( user_op >= DW_LOC_max )
        _Abort( ABORT_ILLEGAL_LOC_OP );
    op_code = translateOp[user_op];
    va_start( args, user_op );
    switch( user_op ) {
    case DW_LOC_deref_size:
    case DW_LOC_xderef_size:
    case DW_LOC_pick:
        op = nextOp( cli, loc, op_code, 1 );
        op->data[0] = (uint_8)va_arg( args, int );
        ADD_ADDR( cli, loc, 1 );
        break;
    case DW_LOC_plus_uconst:
        len = (dw_loc_offs)( ULEB128( buf, va_arg( args, dw_uconst ) ) - buf );
        op = nextOp( cli, loc, op_code, len );
        memcpy( op->data, buf, len );
        ADD_ADDR( cli, loc, len );
        break;
    case DW_LOC_skip:
    case DW_LOC_bra:
        op = nextOp( cli, loc, op_code, sizeof( dw_loc_label ) );
        *(dw_loc_label *)(op->data) = va_arg( args, dw_loc_label );
        ADD_ADDR( cli, loc, sizeof( int_16 ) );
        break;
    case DW_LOC_fbreg:
        len = (dw_loc_offs)( LEB128( buf, va_arg( args, dw_sconst ) ) - buf );
        op = nextOp( cli, loc, op_code, len );
        memcpy( op->data, buf, len );
        ADD_ADDR( cli, loc, len );
        break;
    case DW_LOC_breg:
        reg = va_arg( args, uint );
        if( reg < 32 ) {
            op_code = DW_OP_breg( reg );
            end = buf;
        } else {
            end = ULEB128( buf, reg );
        }
        len = (dw_loc_offs)( LEB128( end, va_arg( args, dw_sconst ) ) - buf );
        op = nextOp( cli, loc, op_code, len );
        memcpy( op->data, buf, len );
        ADD_ADDR( cli, loc, len );
        break;
    default:
        nextOp( cli, loc, op_code, 0 );
        break;
    }
    va_end( args );
}


dw_loc_handle DWENTRY DWLocFini( dw_client cli, dw_loc_id loc )
{
    dw_loc_handle   result;
    loc_op          *cur_op;
    char            *base_of_block;
    char            *p;
    uint            addr;
    dw_loc_label    label;
    int_32          jump_offset;
    uint_32         size;
    dw_sym_reloc    *reloc_info;

    size = BASE_HANDLE_SIZE + sizeof( struct loc_handle_expr )
        + (uint_32)( 1 + sizeof( uint_16 ) ) * ( loc->num_syms + 1 )
        + loc->addr + loc->num_syms * sizeof( dw_sym_reloc );
    if( size != (uint_16)size )
        _Abort( ABORT_LOC_EXPR_TOO_LONG );
    result = CLIAlloc( cli, size );
    result->next = cli->debug_loc.handles;
    cli->debug_loc.handles = result;
    result->is_expr = LOC_EXPR;
    result->u.expr.size = loc->addr;
    result->u.expr.num_syms = loc->num_syms;

    addr = 0;
    base_of_block = result->u.expr.expr;
    p = base_of_block + sizeof( uint_16 );
    for( cur_op = loc->first; cur_op != NULL; cur_op = FreeLink( cli, cur_op ) ) {
        *p++ = cur_op->op_code;
        ++addr;
        switch( cur_op->op_code ) {
        case DW_OP_skip:
        case DW_OP_bra:
            label = *(dw_loc_label *)(cur_op->data);
            addr += sizeof( int_16 );
            jump_offset = (int_32)label->addr - (int_32)addr;
            if( jump_offset < -32768 || jump_offset > 32767 ) {
                _Abort( ABORT_LOC_JUMP_OUT_OF_RANGE );
            }
            WriteS16( p, (int_16)jump_offset );
            p += sizeof( int_16 );
            break;
        case DW_OP_addr:
            *(uint_16 *)base_of_block = (uint_16)( ( p - base_of_block ) - sizeof( uint_16 ) );
            reloc_info = (dw_sym_reloc *)cur_op->data;
            if( reloc_info->kind == DW_W_SEGMENT || reloc_info->kind == DW_W_LABEL_SEG ) { ///TODO :better linkage
                /* it was a DWLocSegment() */
                int segment_size;

                segment_size = GET_SEGMENT_SIZE( cli );
                switch( segment_size ) {
                case 1:
                    p[-1] = DW_OP_const1u;
                    break;
                case 2:
                    p[-1] = DW_OP_const2u;
                    break;
                case 4:
                    p[-1] = DW_OP_const4u;
                    break;
                default:
                    _Abort( ABORT_SEG_SIZE_NOT_SET );
                }
                addr += segment_size;
            } else {
                addr += cli->offset_size;
            }
            *(dw_sym_reloc *)p = *reloc_info;
            p += sizeof( *reloc_info );
            base_of_block = p;
            p += sizeof( uint_16 );
            break;
        default:
            if( cur_op->size ) {
                memcpy( p, cur_op->data, cur_op->size );
                p += cur_op->size;
                addr += cur_op->size;
            }
            break;
        }
    }
    *(uint_16 *)base_of_block = (uint_16)( ( p - base_of_block ) - sizeof( uint_16 ) );

    CarveFreeChain( cli->debug_loc.label_carver, loc->labels );
    CLIFree( cli, loc );

    return( result );
}

void EmitLocExprNull( dw_client cli, dw_sectnum sect, size_t size )
{
    /* ensure that size is correct value */
    _Assert( size == 1 || size == 2 || size == 4 );
    CLISectionWriteZeros( cli, sect, size );
}

uint_32 EmitLocExpr( dw_client cli, dw_sectnum sect, size_t size, dw_loc_handle loc )
{
    char            *p;
    size_t          bytes_left;
    size_t          size_of_block;
    size_t          syms_left;
    dw_sym_reloc    *reloc_info;
    uint_32         expr_size;

    /* ensure that this is really an expression */
    _Assert( loc->is_expr == LOC_EXPR );
    _Assert( size == 1 || size == 2 || size == 4 );
    expr_size = loc->u.expr.size;
    switch( size ) {
    case 1: // block_8
        // value is a uint_8
        _Assert( expr_size <= 0xff );
        CLIWriteU8( cli, sect, (uint_8)expr_size );
        break;
    case 2:   // block_16
        // value is a uint_16
        _Assert( expr_size <= 0xffff );
        CLIWriteU16( cli, sect, (uint_16)expr_size );
        break;
    case 4:   // block_32
        CLIWriteU32( cli, sect, expr_size );
        break;
    }
    syms_left = loc->u.expr.num_syms;
    bytes_left = expr_size;
    p = loc->u.expr.expr;
    while( bytes_left ) {
        size_of_block = *(uint_16 *)p;
        p += sizeof( uint_16 );
        CLIWrite( cli, sect, p, size_of_block );
        bytes_left -= size_of_block;
        p += size_of_block;
        if( syms_left ) {
            reloc_info = (dw_sym_reloc *)p;
            p += sizeof( *reloc_info );
            CLIReloc3( cli, sect, reloc_info->kind, reloc_info->sym );
            if( reloc_info->kind == DW_W_SEGMENT || reloc_info->kind == DW_W_LABEL_SEG ) { //TODO :better linkage
                bytes_left -= GET_SEGMENT_SIZE( cli );
            } else {
                bytes_left -= cli->offset_size;
            }
            --syms_left;
        }
    }
    return( (uint_32)( size + expr_size ) );
}


dw_list_id DWENTRY DWListInit( dw_client cli )
{
    dw_list_id      id;

    id = CLIAlloc( cli, sizeof( struct dw_list_id ) );
    id->hdl.next = cli->debug_loc.handles;
    cli->debug_loc.handles = (dw_loc_handle)id;
    id->hdl.is_expr = LOC_LIST;
    id->hdl.u.list = NULL;
    return( id );
}


void DWENTRY DWListEntry( dw_client cli, dw_list_id id, dw_sym_handle begin, dw_sym_handle end, dw_loc_handle loc )
{
    list_entry      *new;

    new = CLIAlloc( cli, sizeof( struct list_entry ) );
    new->next = id->hdl.u.list;
    id->hdl.u.list = new;
    new->begin = begin;
    new->end = end;
    new->loc = loc;
}

void DWENTRY DWListEntryOut( dw_client cli, dw_list_id id, dw_sym_handle begin, dw_sym_handle end, dw_loc_handle loc )
{
    if( id->hdl.is_expr == LOC_LIST ) {
        id->hdl.is_expr = LOC_LIST_REF;
        id->hdl.u.ref = CLISectionAbs( cli, DW_DEBUG_LOC );
    }
    CLIReloc4( cli, DW_DEBUG_LOC, DW_W_LOC_RANGE, begin, end );
    EmitLocExpr( cli, DW_DEBUG_LOC, sizeof( uint_16 ), loc );
}

dw_loc_handle DWENTRY DWListFini( dw_client cli, dw_list_id id )
{
    if( id->hdl.is_expr == LOC_LIST_REF ) {
        CLISectionWriteZeros( cli, DW_DEBUG_LOC, 2 * sizeof( dw_targ_addr ) );
    } else {
        id->hdl.u.list = ReverseChain( id->hdl.u.list );
    }
    return( (dw_loc_handle)id );
}


static void trash( dw_client cli, dw_loc_handle loc )
{
    if( loc->is_expr == LOC_LIST ) {
        FreeChain( cli, loc->u.list );
    }
    CLIFree( cli, loc );
}


void DWENTRY DWLocTrash( dw_client cli, dw_loc_handle loc )
{
    dw_loc_handle   *walk;

    walk = &cli->debug_loc.handles;
    while( *walk != loc ) {             /* assume user gave us valid handle */
        walk = &(*walk)->next;
    }
    *walk = loc->next;
    trash( cli, loc );
}


uint_32 EmitLocList( dw_client cli, dw_sectnum sect, dw_loc_handle loc )
{
    list_entry      *cur;
    uint_32         bytes_written;

    /* ensure that this is really a list */
    _Assert( loc->is_expr == LOC_LIST );

    bytes_written = 0;
    for( cur = loc->u.list; cur != NULL; cur = cur->next ) {
        bytes_written += 2 * sizeof( dw_targ_addr );
        CLIReloc4( cli, sect, DW_W_LOC_RANGE, cur->begin, cur->end );
        bytes_written += EmitLocExpr( cli, sect, sizeof( uint_16 ), loc );
    }
    CLISectionWriteZeros( cli, sect, 2 * sizeof( dw_targ_addr ) );
    return( bytes_written + 2 * sizeof( dw_targ_addr ) );
}


uint_32 EmitLoc( dw_client cli, dw_sectnum sect, dw_loc_handle loc )
{
    switch( loc->is_expr ) {
    case LOC_LIST:
        CLIWriteU8( cli, sect, DW_FORM_data4 );
        CLIReloc3( cli, sect, DW_W_SECTION_POS, DW_DEBUG_LOC );
        EmitLocList( cli, DW_DEBUG_LOC, loc );
        return( 1 + sizeof( dw_sect_offs ) );
    case LOC_LIST_REF:
        CLIWriteU8( cli, sect, DW_FORM_data4 );
        CLISectionSeekAbs( cli, DW_DEBUG_LOC, loc->u.ref );
        CLIReloc3( cli, sect, DW_W_SECTION_POS, DW_DEBUG_LOC );
        CLISectionSeekEnd( cli, DW_DEBUG_LOC );
        return( 1 + sizeof( dw_sect_offs ) );
    case LOC_EXPR:
        CLIWriteU8( cli, sect, DW_FORM_block2 );
        return( 1 + EmitLocExpr( cli, sect, sizeof( uint_16 ), loc ) );
    }
    return( 0 );
}

uint_32 EmitLocNull( dw_client cli, dw_sectnum sect )
{
    static const uint_8 loc_null[] = {DW_FORM_block1, 0};

    CLIWrite( cli, sect, loc_null, sizeof( loc_null ) );
    return( sizeof( loc_null ) );
}

void InitDebugLoc( dw_client cli )
{
    cli->debug_loc.handles = 0;
    cli->debug_loc.label_carver = CarveCreate( cli, sizeof( struct dw_loc_label ), 16 );
}


void FiniDebugLoc( dw_client cli )
{
    dw_loc_handle   cur;
    dw_loc_handle   next;

    for( cur = cli->debug_loc.handles; cur != NULL; cur = next ) {
        next = cur->next;
        trash( cli, cur );
    }
    CarveDestroy( cli, cli->debug_loc.label_carver );
}
