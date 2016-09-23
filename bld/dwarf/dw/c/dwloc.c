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
* Description:  Location expressions and location lists.
*
****************************************************************************/


#include <stdarg.h>
#include <stddef.h>
#include "dwpriv.h"
#include "dwutils.h"
#include "dwmem.h"
#include "dwloc.h"


#define ADD_ADDR( cli, loc, amt )                   \
    {                                               \
        uint_16                 __old;              \
                                                    \
        __old = (loc)->addr;                        \
        (loc)->addr += (amt);                       \
        if( loc->addr < __old ) {                   \
            _Abort( ABORT_LOC_EXPR_TOO_LONG );      \
        }                                           \
    }

typedef struct loc_op {
    struct loc_op               *next;
    unsigned                    size;
    char                        op_code;
    char                        data[1];
} loc_op;
#define BASE_SIZE               (sizeof( loc_op ) - 1)


struct dw_loc_id {
    loc_op                      *first;
    loc_op                      *last;
    dw_loc_label                labels;
    uint                        num_syms;       // number of DWLocStatic's
    uint_16                     addr;
};


struct dw_loc_label {
    dw_loc_label                next;
    uint_16                     addr;
};

typedef struct {
    dw_sym_handle  sym;
    dw_relocs      kind;
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
            uint_16             size;
            uint_16             num_syms;
            char                expr[1];
        }                       expr;
        debug_ref               ref;
        list_entry              *list;

    } x;  // possible variable size so nothing can follow this
};
#define BASE_HANDLE_SIZE        offsetof( struct dw_loc_handle, x )


struct dw_list_id {
    struct dw_loc_handle        hdl;
};


dw_loc_id DWENTRY DWLocInit( dw_client cli )
{
    dw_loc_id                   new;

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
    dw_loc_label                new;

    new = CarveAlloc( cli, cli->debug_loc.label_carver );
    new->next = loc->labels;
    loc->labels = new;
    new->addr = 0;

    return( new );
}


void DWENTRY DWLocSetLabel( dw_client cli, dw_loc_id loc, dw_loc_label label )
{
    cli = cli;
    label->addr = loc->addr;
}


static loc_op *nextOp( dw_client cli, dw_loc_id loc, uint_8 op_code, uint extra_size )
{
    loc_op                      *new;

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
    uint_8                      buf[MAX_LEB128];
    loc_op                      *op;
    uint_16                     len;

    if( reg < 32 ) {
        op = nextOp( cli, loc, DW_OP_reg( reg ), 0 );
    } else {
        len = (uint_16)( ULEB128( buf, reg ) - buf );
        op = nextOp( cli, loc, DW_OP_regx, len );
        ADD_ADDR( cli, loc, len );
        memcpy( op->data, buf, len );
    }
}

void DWENTRY DWLocPiece( dw_client cli, dw_loc_id loc, uint size )
{
    uint_8                      buf[MAX_LEB128];
    loc_op                      *op;
    uint_16                     len;

    len = (uint_16)( ULEB128( buf, size ) - buf );
    op = nextOp( cli, loc, DW_OP_piece, len );
    ADD_ADDR( cli, loc, len );
    memcpy( op->data, buf, len );
}


void DWENTRY DWLocStatic( dw_client cli, dw_loc_id loc, dw_sym_handle sym )
{
    loc_op                      *op;
    dw_sym_reloc                *reloc_info;

    op = nextOp( cli, loc, DW_OP_addr, sizeof( *reloc_info ) );
    ++loc->num_syms;
    reloc_info = (dw_sym_reloc *)op->data;
    reloc_info->sym = sym;
    reloc_info->kind = DW_W_STATIC;
    ADD_ADDR( cli, loc, cli->offset_size );
}

void DWENTRY DWLocSym( dw_client cli, dw_loc_id loc, dw_sym_handle sym, dw_relocs kind )
{
    loc_op                      *op;
    dw_sym_reloc                *reloc_info;

    op = nextOp( cli, loc, DW_OP_addr, sizeof( *reloc_info ) );
    ++loc->num_syms;
    reloc_info = (dw_sym_reloc *)op->data;
    reloc_info->sym = sym;
    reloc_info->kind = kind;

    if( kind == DW_W_SEGMENT || kind == DW_W_LABEL_SEG ) { ///TODO :better linkage
        if( cli->segment_size != 0 ) {
            ADD_ADDR( cli, loc, cli->segment_size );
        } else {
            ADD_ADDR( cli, loc, sizeof( dw_defseg ) );
        }
    } else {
        ADD_ADDR( cli, loc, cli->offset_size );
    }
}

void DWENTRY DWLocSegment( dw_client cli, dw_loc_id loc, dw_sym_handle sym )
{
    loc_op                      *op;
    dw_sym_reloc                *reloc_info;
    uint_16                     segment_size;

    op = nextOp( cli, loc, DW_OP_addr, sizeof( *reloc_info ) );
    /*
        We need some way of telling DWLocStatic and DWLocSegment apart;
        so we'll stash the reloc type with the sym
    */
    ++loc->num_syms;
    reloc_info = (dw_sym_reloc *)op->data;
    reloc_info->sym = sym;
    reloc_info->kind = DW_W_SEGMENT;
    if( cli->segment_size == 0 ) {  //TODO not hardwire seg size
        segment_size = sizeof( dw_defseg );
    } else {
        segment_size = cli->segment_size;
    }
    ADD_ADDR( cli, loc, segment_size ); // size of info written u2
}


void DWENTRY DWLocConstU( dw_client cli, dw_loc_id loc, dw_uconst value )
{
    uint_8                      buf[MAX_LEB128];
#ifndef NDEBUG
    uint_8                      *end;
#endif
    loc_op                      *op;

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
    uint_8                      buf[MAX_LEB128];
    loc_op                      *op;
    uint_16                     len;

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
        len = (uint_16)( LEB128( buf, value ) - buf );
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


void DWENTRY DWLocOp0( dw_client cli, dw_loc_id loc, uint user_op )
{
    _Validate( loc != NULL );
    if( user_op >= DW_LOC_max )
        _Abort( ABORT_ILLEGAL_LOC_OP );
    nextOp( cli, loc, translateOp[user_op], 0 );
}


void DWENTRY DWLocOp( dw_client cli, dw_loc_id loc, uint user_op, ... )
{
    uint_8                      buf[2 * MAX_LEB128];
    uint_8                      *end;
    uint_8                      op_code;
    loc_op                      *op;
    va_list                     args;
    uint                        reg;
    uint_16                     len;

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
        len = (uint_16)( ULEB128( buf, va_arg( args, dw_uconst ) ) - buf );
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
        len = (uint_16)( LEB128( buf, va_arg( args, dw_sconst ) ) - buf );
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
        len = (uint_16)( LEB128( end, va_arg( args, dw_sconst ) ) - buf );
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
    dw_loc_handle           result;
    loc_op                  *cur_op;
    char                    *base_of_block;
    char                    *p;
    uint                    addr;
    dw_loc_label            label;
    int_32                  jump_offset;
    uint_32                 size;
    dw_sym_reloc            *reloc_info;

    size = BASE_HANDLE_SIZE + sizeof( struct loc_handle_expr )
        + (uint_32)( 1 + sizeof( uint_16 ) ) * ( loc->num_syms + 1 )
        + loc->addr + loc->num_syms * sizeof( dw_sym_reloc );
    if( size != (uint_16)size )
        _Abort( ABORT_LOC_EXPR_TOO_LONG );
    result = CLIAlloc( cli, size );
    result->next = cli->debug_loc.handles;
    cli->debug_loc.handles = result;
    result->is_expr = LOC_EXPR;
    result->x.expr.size = loc->addr;
    result->x.expr.num_syms = loc->num_syms;

    addr = 0;
    cur_op = loc->first;
    base_of_block = result->x.expr.expr;
    p = base_of_block + sizeof( uint_16 );
    while( cur_op ) {
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
            if( reloc_info->kind == DW_W_SEGMENT
              || reloc_info->kind == DW_W_LABEL_SEG ) { ///TODO :better linkage
                /* it was a DWLocSegment() */
                int segment_size;

                if( cli->segment_size == 0 ) {  //TODO fix flat with segref
                   segment_size = sizeof( dw_defseg );
                } else {
                   segment_size = cli->segment_size;
                }
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
        cur_op = FreeLink( cli, cur_op );
    }
    *(uint_16 *)base_of_block = (uint_16)( ( p - base_of_block ) - sizeof( uint_16 ) );

    CarveFreeChain( cli->debug_loc.label_carver, loc->labels );
    CLIFree( cli, loc );

    return( result );
}

void EmitLocExprNull( dw_client cli, uint sect, size_t size )
{
    union {
        char     buf[sizeof( uint_32 )];
        uint_8   u8;
        uint_16  u16;
        uint_32  u32;
    } len_form;

    /* ensure that this is really an expression */
    _Assert( size == 1 || size == 2 || size == 4 );
    switch( size ) {
    case 1: // block_8
        len_form.u8 = 0;
        break;
    case 2:   // block_16
        len_form.u16 = 0;
        break;
    case 4:   // block_32
        len_form.u32 = 0;
        break;
    }
    CLIWrite( cli, sect, len_form.buf, size );
}

uint_32 EmitLocExpr( dw_client cli, uint sect, size_t size, dw_loc_handle loc )
{
    union {
        char     buf[sizeof( uint_32 )];
        uint_8   u8;
        uint_16  u16;
        uint_32  u32;
    } len_form;
    char *                      p;
    unsigned                    bytes_left;
    unsigned                    size_of_block;
    uint                        syms_left;
    dw_sym_reloc                *reloc_info;

    /* ensure that this is really an expression */
    _Assert( loc->is_expr == LOC_EXPR );
    _Assert( size == 1 || size == 2 || size == 4 );
    switch( size ) {
    case 1: // block_8
        _Assert( loc->x.expr.size <= 0xff );
        len_form.u8 = (uint_8)loc->x.expr.size;
        break;
    case 2:   // block_16
        // x.expr.size is a uint_16
        //_Assert( loc->x.expr.size <= 0xffff );
        len_form.u16 = (uint_16)loc->x.expr.size;
        break;
    case 4:   // block_32
        len_form.u32 = loc->x.expr.size;
        break;
    }
    CLIWrite( cli, sect, len_form.buf, size );
    syms_left = loc->x.expr.num_syms;
    bytes_left = loc->x.expr.size;
    p = loc->x.expr.expr;
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
            if( reloc_info->kind ==  DW_W_SEGMENT
              || reloc_info->kind == DW_W_LABEL_SEG ) { //TODO :better linkage
                if( cli->segment_size == 0 ) {          //TODO fix flat with segref
                   bytes_left -= sizeof( dw_defseg );
                } else {
                   bytes_left -= cli->segment_size;
                }
            } else {
                bytes_left -= cli->offset_size;
            }
            --syms_left;
        }
    }
    return( (uint_32)( size + loc->x.expr.size ) );
}


dw_list_id DWENTRY DWListInit( dw_client cli )
{
    dw_list_id                  id;

    id = CLIAlloc( cli, sizeof( struct dw_list_id ) );
    id->hdl.next = cli->debug_loc.handles;
    cli->debug_loc.handles = (dw_loc_handle)id;
    id->hdl.is_expr = LOC_LIST;
    id->hdl.x.list = NULL;
    return( id );
}


void DWENTRY DWListEntry( dw_client cli, dw_list_id id, dw_sym_handle begin, dw_sym_handle end, dw_loc_handle loc )
{
    list_entry *                new;

    new = CLIAlloc( cli, sizeof( struct list_entry ) );
    new->next = id->hdl.x.list;
    id->hdl.x.list = new;
    new->begin = begin;
    new->end = end;
    new->loc = loc;
}

void DWENTRY DWListEntryOut( dw_client cli, dw_list_id id, dw_sym_handle begin, dw_sym_handle end, dw_loc_handle loc )
{
    if( id->hdl.is_expr == LOC_LIST ) {
        id->hdl.x.ref = CLITell( cli, DW_DEBUG_LOC );
        id->hdl.is_expr = LOC_LIST_REF;
    }
    CLIReloc4( cli, DW_DEBUG_LOC, DW_W_LOC_RANGE, begin, end );
    EmitLocExpr( cli, DW_DEBUG_LOC, sizeof( uint_16 ), loc );
}

dw_loc_handle DWENTRY DWListFini( dw_client cli, dw_list_id id )
{
    static char  const zeros[2 * sizeof( dw_targ_addr )] = {0};

    if( id->hdl.is_expr == LOC_LIST_REF ) {
        CLIWrite( cli, DW_DEBUG_LOC, zeros, sizeof( zeros ) );
    } else {
        id->hdl.x.list = ReverseChain( id->hdl.x.list );
    }
    return( (dw_loc_handle) id );
}


static void trash( dw_client cli, dw_loc_handle loc )
{
    if( loc->is_expr == LOC_LIST ) {
        FreeChain( cli, loc->x.list );
    }
    CLIFree( cli, loc );
}


void DWENTRY DWLocTrash( dw_client cli, dw_loc_handle loc )
{
    dw_loc_handle *             walk;

    walk = &cli->debug_loc.handles;
    while( *walk != loc ) {             /* assume user gave us valid handle */
        walk = &(*walk)->next;
    }
    *walk = loc->next;
    trash( cli, loc );
}


uint_32 EmitLocList( dw_client cli, uint sect, dw_loc_handle loc )
{
    static char const   zeros[2 * sizeof( dw_targ_addr )] = {0};
    list_entry          *cur;
    uint_32             bytes_written;

    /* ensure that this is really a list */
    _Assert( loc->is_expr == LOC_LIST );

    bytes_written = 0;
    for( cur = loc->x.list; cur != NULL; cur = cur->next ) {
        bytes_written += 2 * sizeof( dw_targ_addr );
        CLIReloc4( cli, sect, DW_W_LOC_RANGE, cur->begin, cur->end );
        bytes_written += EmitLocExpr( cli, sect, sizeof( uint_16 ), loc );
    }
    CLIWrite( cli, sect, zeros, sizeof( zeros ) );
    return( bytes_written + sizeof( zeros ) );
}


uint_32 EmitLoc( dw_client cli, uint sect, dw_loc_handle loc )
{
    char                        buf[1];

    switch( loc->is_expr ) {
    case LOC_LIST:
        buf[0] = DW_FORM_data4;
        CLIWrite( cli, sect, buf, 1 );
        CLIReloc3( cli, sect, DW_W_SECTION_POS, DW_DEBUG_LOC );
        EmitLocList( cli, DW_DEBUG_LOC, loc );
        return( 1 + sizeof( debug_ref ) );
    case LOC_LIST_REF:
        buf[0] = DW_FORM_data4;
        CLIWrite( cli, sect, buf, 1 );
        CLISeek( cli, DW_DEBUG_LOC, loc->x.ref, DW_SEEK_SET );
        CLIReloc3( cli, sect, DW_W_SECTION_POS, DW_DEBUG_LOC );
        CLISeek( cli, DW_DEBUG_LOC, 0, DW_SEEK_END );
        return( 1 + sizeof( debug_ref ) );
    case LOC_EXPR:
        buf[0] = DW_FORM_block2;
        CLIWrite( cli, sect, buf, 1 );
        return( 1 + EmitLocExpr( cli, sect, sizeof( uint_16 ), loc ) );
    }
    return( 0 );
}

uint_32 EmitLocNull( dw_client cli, uint sect )
{
    static char const zero[2] = {DW_FORM_block1, 0};

    CLIWrite( cli, sect, zero, sizeof( zero ) );
    return( sizeof( zero ) );
}

void InitDebugLoc( dw_client cli )
{
    cli->debug_loc.handles = 0;
    cli->debug_loc.label_carver = CarveCreate( cli, sizeof( struct dw_loc_label ), 16 );
}


void FiniDebugLoc( dw_client cli )
{
    dw_loc_handle               cur;
    dw_loc_handle               next;

    for( cur = cli->debug_loc.handles; cur; cur = next ) {
        next = cur->next;
        trash( cli, cur );
    }
    CarveDestroy( cli, cli->debug_loc.label_carver );
}
