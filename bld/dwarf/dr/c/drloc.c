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
* Description:  DWARF reader location expression processing.
*
****************************************************************************/


#include "drpriv.h"
#include "drutils.h"
#include <stdlib.h>
#include <string.h>

static dw_locop_op const LocOpr[] = {
#define DW_LOC_OP( __n, __v )    __v,
#include "dwlocinf.h"
#undef DW_LOC_OP
};

static unsigned_32 ReadVWord( dr_handle where, int size )
//Read an int
{
    unsigned_32 ret;

    switch( size ) {
    case 1:
        ret = DWRVMReadByte( where );
        break;
    case 2:
        ret = DWRVMReadWord( where );
        break;
    case 4:
        ret = DWRVMReadDWord( where );
        break;
    }
    return( ret );
}
static uint_8 *DecodeULEB128( const uint_8 *input, uint_32 *value )
/*****************************************************************/
{
    uint_32     result;
    uint        shift;
    uint_8      in;

    result = 0;
    shift = 0;
    for( ;; ) {
        in = *input++;
        result |= ( in & 0x7f ) << shift;
        if( ( in & 0x80 ) == 0 ) break;
        shift += 7;
    }
    *value = result;
    return( (uint_8 *)input );
}

static uint_8 *DecodeLEB128( const uint_8 *input, uint_32 *value )
/****************************************************************/
{
    int_32      result;
    uint        shift;
    uint_8      in;

    result = 0;
    shift = 0;
    for( ;; ) {
        in = *input++;
        result |= ( in & 0x7f ) << shift;
        shift += 7;
        if( ( in & 0x80 ) == 0 ) break;
    }
    if( ( shift < 32 ) && ( in & 0x40 ) ) {
        result |= - ( 1 << shift );
    }
    *value = (uint_32)result;
    return( (uint_8 *)input );
}
//TODO: check stack bounds
static void DoLocExpr( unsigned_8       *p,
                       int              length,
                       int              addr_size,
                       dr_loc_callbck   *callbck,
                       void             *d,
                       dr_handle        var )
{
    unsigned_8      *end;
    dw_op           op;
    dw_locop_op     opr;
    uint_32         op1;
    uint_32         op2;
    uint_32         stk1;
    uint_32         stk2;
    uint_32         tmp;
    uint_32         stack[100];
    uint_32         *top;
    uint_32         *stk_top;
    dr_loc_kind     kind;

#define Pop( a )     (++a)
#define Push( a )    (--a)
#define IsEmpty( a, b )  ( (a) == (b) )
    end = &p[length];
    stk_top = &stack[100];
    top = stk_top;
    if( callbck->init!= NULL  ) {
        kind = callbck->init( d, &tmp );
        switch( kind ){
        case DR_LOC_NONE:
             kind = DR_LOC_ADDR;
             break;
        case DR_LOC_REG:
        case DR_LOC_ADDR:
            Push( top );
            top[0] = tmp;
        }
    } else {
        kind = DR_LOC_ADDR;
    }
    while( p  < end ) {
        op = *p;
        ++p;
        opr = LocOpr[ op ];
        /* decode operands */
        switch( opr ) {
        case DW_LOP_NOOP:
            break;
        case DW_LOP_ADDR:
            if( addr_size == 4 ) {
                op1 = *(uint_32 _WCUNALIGNED *)p;
                if( DWRCurrNode->byte_swap ) {
                    SWAP_32( op1 );
                }
            } else if( addr_size == 2 ) {
                op1 = *(uint_16 _WCUNALIGNED *)p;
                if( DWRCurrNode->byte_swap ) {
                    SWAP_16( op1 );
                }
            } else if( addr_size == 1 ) {
                op1 = *(uint_8 _WCUNALIGNED *)p;
            } else {
                op1 = 0;
            }
            p += addr_size;
            break;
        case DW_LOP_OPU1:
            op1 = *(uint_8 *)p;
            p += sizeof( uint_8 );
            break;
        case DW_LOP_OPS1:
            op1 = (int_32)*(int_8 *)p;
            p += sizeof(int_8 );
            break;
        case DW_LOP_OPU2:
            op1 = *(uint_16 _WCUNALIGNED *)p;
            p += sizeof(uint_16 );
            break;
        case DW_LOP_OPS2:
            op1 = (int_32)*(int_16 _WCUNALIGNED *)p;
            p += sizeof(int_16);
            break;
        case DW_LOP_OPS4:
            op1 = *(int_32 _WCUNALIGNED *)p;
            p += sizeof(int_32);
            break;
        case DW_LOP_OPU4:
            op1 = *(uint_32 _WCUNALIGNED *)p;
            p += sizeof(uint_32);
            break;
        case DW_LOP_U128:
            p = DecodeULEB128( p, &tmp );
            op1 = tmp;
            break;
        case DW_LOP_S128:
            p = DecodeLEB128( p, &tmp );
            op1 = tmp;
            break;
        case DW_LOP_U128_S128:
            p = DecodeULEB128( p, &tmp );
            op1 = tmp;
            p = DecodeLEB128( p, &tmp );
            op2 = tmp;
            break;
        case DW_LOP_LIT1:
            op1 = op-DW_OP_lit0;
            op = DW_OP_lit0;
            break;
        case DW_LOP_REG1:
            op1 = op-DW_OP_reg0;
            op = DW_OP_reg0;
            break;
        case DW_LOP_BRG1:
            op1 = op-DW_OP_breg0;
            op = DW_OP_breg0;
            p = DecodeLEB128( p, &tmp );
            op2 = tmp;
            break;
        case DW_LOP_STK2:
            stk2 = top[0];
            Pop( top );         /* drop thru */
        case DW_LOP_STK1:
            stk1 = top[0];
            break;
        default:
            DWREXCEPT( DREXCEP_BAD_DBG_INFO );
        }
        switch( op ) {
        case DW_OP_reg0:
        case DW_OP_regx:
            /* reg is location */
            Push( top );
            top[0] = op1;
            kind = DR_LOC_REG;
            break;
        case DW_OP_addr: {
            bool isfar;

            /* get contents of reg op1 */
            Push( top );
            top[0] = op1;
            if( (p != end) && ((*p == DW_OP_xderef) || (*p == DW_OP_xderef_size)) ) {
                isfar = TRUE;
            } else {
                isfar = FALSE;
            }
            if( !callbck->acon( d, top, isfar ) ) {
                return;
            }
        }   break;
        case DW_OP_lit0:
        case DW_OP_const1u:
        case DW_OP_const1s:
        case DW_OP_const2u:
        case DW_OP_const2s:
        case DW_OP_const4s:
        case DW_OP_const4u:
        case DW_OP_const8u:
        case DW_OP_const8s:
        case DW_OP_constu:
        case DW_OP_consts:
            Push( top );
            top[0] = op1;
            break;
        case DW_OP_fbreg:
        /* We should look at DW_AT_frame_base here, however at this
         * point the debugger seems to be able to figure out the base
         * correctly without it - at least assuming that it's (E)BP on x86.
         */
            Push( top );
            if( !callbck->frame( d, top ) ) {
                return;
            }
            top[0] += op1;
            break;
        case DW_OP_breg0:
        case DW_OP_bregx:
            /* get contents of reg op1 */
            Push( top );
            if( !callbck->reg( d, top, op1 ) ) {
                return;
            }
            top[0] += op2;
            break;
        case DW_OP_dup:
            stk1 = top[0];
            Push( top );
            top[0] = stk1;
            break;
        case DW_OP_drop:
            Pop( top );
            break;
        case DW_OP_over:
            stk1 = top[1];
            Push( top );
            top[0] = stk1;
            break;
        case DW_OP_pick:
            stk1 = top[op1];
            Push( top );
            top[0] = stk1;
            break;
        case DW_OP_swap:
            stk1 = top[0];
            top[0] = top[1];
            top[1] = stk1;
            break;
        case DW_OP_rot:
            stk1 = top[0];
            top[0] = top[1];
            top[1] = top[2];
            top[2] = stk1;
            break;
        case DW_OP_deref:
            op1 = addr_size; /* fall thru */
        case DW_OP_deref_size:
            /* dref addr */
            if( kind == DR_LOC_REG ) {  // indirect of reg name
                if( !callbck->reg( d, top, top[0] ) ) {
                    return;
                }
                kind = DR_LOC_ADDR;
            } else {
                if( !callbck->dref( d, top, top[0], op1 ) ) {
                    return;
                }
            }
            break;
        case DW_OP_xderef:
            op1 = addr_size; /* fall thru */
        case DW_OP_xderef_size:
           /*  xdref addr */
           stk1 = top[0];
           Pop( top );
            if( !callbck->drefx( d, top, stk1, top[0], op1 ) ) {
                return;
            }
           break;
        case DW_OP_abs:
            if( (int_32)stk1 < 0 ) {
                stk1 = -(int_32)stk1;
                top[0] =  stk1;
            }
            break;
        case DW_OP_and:
            top[0] = stk2 & stk1;
            break;
        case DW_OP_div:
            top[0] = stk2 / stk1;
            break;
        case DW_OP_minus:
            top[0] = stk2 - stk1;
            break;
        case DW_OP_mod:
            top[0] = stk2 % stk1;
            break;
        case DW_OP_mul:
            top[0] = stk2 * stk1;
            break;
        case DW_OP_neg:
            top[0] = -stk1;
            break;
        case DW_OP_not:
            top[0] = ~stk1;
            break;
        case DW_OP_or:
            top[0] = stk2 | stk1;
            break;
        case DW_OP_plus:
            top[0] = stk2 + stk1;
            break;
        case DW_OP_plus_uconst:
            top[0] += op1;
            break;
        case DW_OP_shl:
            top[0] = stk2 << stk1;
            break;
        case DW_OP_shr:
            top[0] = stk2 >> stk1;
            break;
        case DW_OP_shra:
            top[0] = (int_32) stk2 >> stk1;
            break;
        case DW_OP_xor:
            top[0] = stk2 ^ stk1;
            break;
        case DW_OP_eq:
            top[0] = (int_32)stk2 == (int_32)stk1;
            break;
        case DW_OP_ge:
            top[0] = (int_32)stk2 >= (int_32)stk1;
            break;
        case DW_OP_gt:
            top[0] = (int_32)stk2 > (int_32)stk1;
            break;
        case DW_OP_le:
            top[0] = (int_32)stk2 <= (int_32)stk1;
            break;
        case DW_OP_lt:
            top[0] = (int_32)stk2 < (int_32)stk1;
            break;
        case DW_OP_ne:
            top[0] = (int_32)stk2 != (int_32)stk1;
            break;
        case DW_OP_bra:
            stk1 = top[0];
            Pop( top );
            if( stk1 == 0 ) break;  /* conditional fall thru */
        case DW_OP_skip:
            p += op1;
            break;
        case DW_OP_nop:
            break;
        case DW_OP_piece:
            /* call got piece */
            if( !callbck->ref( d, top[0], op1, kind ) ) {
                return;
            }
            Pop( top );
            kind = DR_LOC_ADDR;
            break;
        default:
            DWREXCEPT( DREXCEP_BAD_DBG_INFO );
        }
    }
    while( !IsEmpty( top, stk_top) ) {
        if( !callbck->ref( d, top[0], addr_size, kind ) ) {
            return;
        }
        Pop( top );
        kind = DR_LOC_ADDR;
    }
    return;
}

static dr_handle SearchLocList( uint_32 start, uint_32 context,
                                uint addr_size )
/*************************************************************/
// Search loc list for context return start of loc_expr block or NULL
{
    uint_32     low;
    uint_32     high;
    int         len;
    dr_handle   p;

    p =  DWRCurrNode->sections[DR_DEBUG_LOC].base;
    if( p == 0 ) {
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
        return( 0 );
    }
    p += start;
    for( ;; ) {
        low = ReadVWord( p, addr_size );
        p+= addr_size;
        high = ReadVWord( p, addr_size );
        p+= addr_size;
        if( low == high && low == 0 ) {
            p = 0;
            break;
        }
        if( low <= context && context < high ) break;
        len = DWRVMReadWord( p );
        p+= sizeof(uint_16);
        p += len;
    }
    return( p );
}

static int DWRLocExpr( dr_handle        var,
                       dr_handle        abbrev,
                       dr_handle        info,
                       dr_loc_callbck   *callbck,
                       void             *d )
/***********************************************/
{
    unsigned    form;
    uint_32     size;
    uint_8      loc_buff[256];
    uint_8      *expr;
    int         ret;
    dr_handle   loclist;
    uint_32     context;
    int         addr_size;

    addr_size = DWRGetAddrSize( DWRFindCompileUnit( info ) );
    form = DWRVMReadULEB128( &abbrev );
    ret = TRUE;
    for( ;; ) {
        switch( form ) {
        case DW_FORM_block1:
            size  = DWRVMReadByte( info );
            info += sizeof(unsigned_8);
            goto end_loop;
        case DW_FORM_block2:
             size = DWRVMReadWord( info );
             info += sizeof(unsigned_16);
            goto end_loop;
        case DW_FORM_block4:
            size = DWRVMReadDWord( info );
            info += sizeof(unsigned_32);
            goto end_loop;
        case DW_FORM_block:
            size = DWRVMReadULEB128( &info );
            goto end_loop;
        case DW_FORM_indirect:
            form = DWRVMReadULEB128( &info );
            break;
        case DW_FORM_ref_addr:
        case DW_FORM_data4:
            if( !callbck->live( d, &context ) ) {
                ret = FALSE;
                goto exit;
            }
            loclist =  DWRVMReadDWord( info );
            info = SearchLocList( loclist, context, addr_size );
            if( info == 0 ) {
                ret = FALSE;
                goto exit;
            }
            form = DW_FORM_block2;
            break;
        case DW_FORM_data2: //TODO kludge
            ret = DWRVMReadWord( info );
            ret = callbck->ref( d, ret, addr_size, DR_LOC_ADDR );
            goto exit;
        default:
            ret = FALSE;
            goto exit;
        }
    } end_loop:;
    if( size > sizeof( loc_buff ) ) {
         expr = DWRALLOC( size );
    } else if( size > 0 ) {
        expr = loc_buff;
    } else {
        ret = FALSE;
        goto exit;
    }
    ret = TRUE;
    DWRVMRead( info, expr, size );
    DoLocExpr( expr, size, addr_size, callbck, d, var );
    if( size > sizeof( loc_buff ) ) {
        DWRFREE( expr );
    }
exit:
    return( ret );
}

extern int DRLocBasedAT( dr_handle      var,
                         dr_loc_callbck *callbck,
                         void           *d )
/***********************************************/
{
    dw_tagnum   tag;
    dw_atnum    at;
    dr_handle   abbrev;
    dr_handle   sym = var;
    int         ret;

    abbrev = DWRVMReadULEB128( &var );
    abbrev = DWRLookupAbbrev( var, abbrev );
    tag = DWRVMReadULEB128( &abbrev );
    ++abbrev; /* skip child flag */
    switch( tag ) {
        break;
    case DW_TAG_member:
    case DW_TAG_inheritance:
        at = DW_AT_data_member_location;
        break;
    case DW_TAG_subprogram:
        at = DW_AT_vtable_elem_location;
        break;
    default:
        return( FALSE );
    }
    if( DWRScanForAttrib( &abbrev, &var, at ) != 0 ) {
        ret = DWRLocExpr( sym, abbrev, var, callbck, d );
    } else {
        if( tag == DW_TAG_member ) {
            // For members of a union, it is valid not to have any
            // DW_AT_data_member_location attribute (we might want to check
            // that we are in fact dealing with a union here). Just create
            // a dummy location "+0".
            uint_8      dummy_loc[] = { DW_OP_lit0, DW_OP_plus };
            int         addr_size;

            addr_size = DWRGetAddrSize( DWRFindCompileUnit( var ) );
            DoLocExpr( dummy_loc, sizeof(dummy_loc), addr_size, callbck, d, var );
            ret = TRUE;
        } else {
            ret = FALSE;
        }
    }
    return( ret );
}

extern int DRLocationAT( dr_handle      var,
                         dr_loc_callbck *callbck,
                         void           *d )
/***********************************************/
{
    dw_tagnum   tag;
    dw_atnum    at;
    dr_handle   abbrev;
    dr_handle   sym = var;
    int         ret;

    abbrev = DWRVMReadULEB128( &var );
    abbrev = DWRLookupAbbrev( var, abbrev );
    tag = DWRVMReadULEB128( &abbrev );
    ++abbrev; /* skip child flag */
    switch( tag ) {
    case DW_TAG_common_block:
    case DW_TAG_formal_parameter:
    case DW_TAG_variable:
    case DW_TAG_subprogram:
        at = DW_AT_location;
        break;
    case DW_TAG_string_type:
        at = DW_AT_string_length;
        break;
    default:
        return( FALSE );
    }
    if( DWRScanForAttrib( &abbrev, &var, at ) != 0 ) {
         ret = DWRLocExpr( sym, abbrev, var, callbck, d );
    } else {
        ret = FALSE;
    }
    return( ret );
}

extern int DRParmEntryAT( dr_handle         var,
                          dr_loc_callbck    *callbck,
                          void              *d )
/***************************************************/
{
    dw_tagnum   tag;
    dr_handle   abbrev;
    dr_handle   sym = var;
    int         ret;

    abbrev = DWRVMReadULEB128( &var );
    abbrev = DWRLookupAbbrev( var, abbrev );
    tag = DWRVMReadULEB128( &abbrev );
    ++abbrev; /* skip child flag */
    if( DWRScanForAttrib( &abbrev, &var, DW_AT_WATCOM_parm_entry ) != 0 ) {
         ret = DWRLocExpr( sym, abbrev, var, callbck, d );
    } else {
        ret = FALSE;
    }
    return( ret );
}
extern dr_handle DRStringLengthAT( dr_handle str )
/************************************************/
{
    dr_handle   abbrev;

    abbrev = DWRGetAbbrev( &str );
    if( DWRScanForAttrib( &abbrev, &str, DW_AT_string_length ) != 0 ) {
         return( str );
    } else {
         return( 0 );
    }
}

extern int DRRetAddrLocation( dr_handle      var,
                              dr_loc_callbck *callbck,
                              void           *d )
/****************************************************/
{
    dr_handle   abbrev;
    dr_handle   sym = var;
    int         ret;

    abbrev = DWRGetAbbrev( &var );
    if( DWRScanForAttrib( &abbrev, &var, DW_AT_return_addr ) != 0 ) {
         ret = DWRLocExpr( sym, abbrev, var, callbck, d );
    } else {
        ret = FALSE;
    }
    return( ret );
}

extern int DRSegLocation( dr_handle      var,
                          dr_loc_callbck *callbck,
                          void           *d )
/************************************************/
{
    dr_handle   abbrev;
    dr_handle   sym = var;
    int         ret;

    abbrev = DWRGetAbbrev( &var );
    if( DWRScanForAttrib( &abbrev, &var, DW_AT_segment ) != 0 ) {
         ret = DWRLocExpr( sym, abbrev, var, callbck, d );
    } else {
        ret = FALSE;
    }
    return( ret );
}
