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
* Description:  Optimize segment register usage and merge memory accesses.
*
****************************************************************************/


#include <assert.h>

#include "cgstd.h"
#include "coderep.h"
#include "pattern.h"
#include "data.h"
#include "makeins.h"
#include "namelist.h"


extern  void            DoNothing(instruction*);
extern  name            *AllocRegName(hw_reg_set);
extern  name            *DeAlias(name*);
extern  bool            VolatileIns(instruction *);
extern  hw_reg_set      Low16Reg( hw_reg_set regs );
extern  hw_reg_set      High16Reg( hw_reg_set regs );
extern  hw_reg_set      FullReg( hw_reg_set regs );

static opcode_entry LDSES[1] = {
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE(                           NO_CC,  V_NO,           RG_,          G_LDSES,        FU_NO )
};


static  bool    AdjacentMem( name *s, name *r, type_class_def tipe )
/******************************************************************/
{
    name        *base_s;
    name        *base_r;
    int         locn_s;
    int         locn_r;
    int         stride;

    stride = TypeClassSize[tipe];
    if( s->n.class != r->n.class ) return( FALSE );
    if( s->n.class == N_MEMORY ) {
        if( s->v.symbol != r->v.symbol ) return( FALSE );
        if( s->m.memory_type != r->m.memory_type ) return( FALSE );
        if( s->v.offset != r->v.offset + stride ) return( FALSE );
        return( TRUE );
    } else if( s->n.class == N_INDEXED ) {
        if( HasTrueBase( s ) && !HasTrueBase( r ) ) return( FALSE );
        if( !HasTrueBase( s ) && HasTrueBase( r ) ) return( FALSE );
        if( s->i.base != r->i.base ) return( FALSE );
        if( s->i.index != r->i.index ) return( FALSE );
        if( s->i.constant != r->i.constant + stride ) return( FALSE );
        return( TRUE );
    } else if( s->n.class == N_TEMP ) {
        base_s = DeAlias( s );
        locn_s = base_s->t.location + s->v.offset - base_s->v.offset;
        base_r = DeAlias( r );
        locn_r = base_r->t.location + r->v.offset - base_r->v.offset;
        if( locn_s != locn_r + stride ) return( FALSE );
        return( TRUE );
    }
    return( FALSE );
}


static bool     MemMove( instruction *ins )
/*****************************************/
{
    if( ins->head.opcode == OP_MOV ) {
        switch( ins->result->n.class ) {
        case N_TEMP:
        case N_MEMORY:
        case N_INDEXED:
            return( TRUE );
        default:
            break;
        }
    }
    return( FALSE );
}


static bool     OptMemMove( instruction *ins, instruction *next )
/***************************************************************/
{
    unsigned_32         shift;
    unsigned_32         lo;
    unsigned_32         hi;
    type_class_def      result_type = 0;
    unsigned_32         result_const;
    name                *result;

    assert( MemMove( ins ) && MemMove( next ) );
    if( ins->type_class == next->type_class ) {
        if(  ins->operands[0]->n.class == N_CONSTANT &&
             ins->operands[0]->c.const_type == CONS_ABSOLUTE &&
            next->operands[0]->n.class == N_CONSTANT &&
            next->operands[0]->c.const_type == CONS_ABSOLUTE ) {
            switch( TypeClassSize[ins->type_class] ) {
            case 1:
                shift = 8;
                result_type = U2;
                break;
            case 2:
#if _TARGET & _TARG_IAPX86
                if( ! _CPULevel( CPU_386 ) ) {
                    shift = 0;
                    break;
                }
#endif
                shift = 16;
                result_type = U4;
                break;
            default:
                shift = 0;
                result_type = 0;
                break;
            }
            if( shift ) {
                result = NULL;
                if( AdjacentMem( ins->result, next->result, ins->type_class ) ) {
                    hi =  ins->operands[0]->c.int_value;
                    lo = next->operands[0]->c.int_value;
                    result = next->result;
                } else if( AdjacentMem( next->result, ins->result, ins->type_class ) ) {
                    lo =  ins->operands[0]->c.int_value;
                    hi = next->operands[0]->c.int_value;
                    result = ins->result;
                } else {
                    return( FALSE );
                }
                lo &= ( ( 1 << shift ) - 1 );
                hi &= ( ( 1 << shift ) - 1 );
                result_const = lo | ( hi << shift );
                ins->operands[0] = AllocS32Const( result_const );
                ins->type_class = result_type;
                ins->result = result;
                DoNothing( next );
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

#if _TARGET & _TARG_IAPX86

static bool isPushX2( instruction *ins )
/**************************************/
{
    if( ins->head.opcode == OP_PUSH ) {
        switch( ins->type_class ) {
        case U2:
        case I2:
            return( TRUE );
        default:
            break;
        }
    }
    return( FALSE );
}


static bool isOpConstant( name *op )
/**********************************/
{
    if( op->n.class == N_CONSTANT ) {
        if( op->c.const_type == CONS_ABSOLUTE ) {
            return( TRUE );
        }
    }
    return( FALSE );
}


static bool OptPushDWORDConstant( instruction *ins, instruction *next )
/*********************************************************************/
{
    name    *opi;
    name    *opn;

    assert( isPushX2( ins ) && isPushX2( next ) );
    opi = ins->operands[0];
    opn = next->operands[0];
    if( ! isOpConstant( opi ) || ! isOpConstant( opn ) ) {
        return( FALSE );
    }
    if( opi->c.int_value != 0 ) {
        if( opi->c.int_value != -1 ) {
            // first word constant isn't 0 or -1
            return( FALSE );
        }
        if( opn->c.int_value >= 0 || opn->c.int_value < -128 ) {
            // second word constant won't sign-extend
            return( FALSE );
        }
    } else {
        if( opn->c.int_value < 0 || opn->c.int_value > 127 ) {
            // second word constant exceeds signed byte positive maximum
            return( FALSE );
        }
    }
    DoNothing( ins );
    next->type_class = I4;
    return( TRUE );
}


static bool OptPushDWORDMemory( instruction *ins, instruction *next )
/*******************************************************************/
{
    assert( isPushX2( ins ) && isPushX2( next ) );
    if( AdjacentMem( ins->operands[0], next->operands[0], U2 ) ) {
        DoNothing( ins );
        next->type_class = I4;
        return( TRUE );
    }
    return( FALSE );
}
#endif


static  bool    NotByteMove( instruction *ins )
/*********************************************/
{
    if( ins->head.opcode != OP_MOV ) return( FALSE );
    if( ins->type_class == U1 || ins->type_class == I1 ) return( FALSE );
    return( TRUE );
}


static  bool    IsLESDS( instruction *ins, instruction *next )
/************************************************************/
{
    if( ins->u.gen_table->generate != G_RM1
        && ins->u.gen_table->generate != G_MOVAM ) return( FALSE );
    if( ins->type_class != WD && ins->type_class != SW ) return( FALSE );
    if( next->u.gen_table->generate != G_SM1 ) return( FALSE );
    return( TRUE );
}


static  void    CheckLDSES( instruction *seg, instruction *reg,
                            bool seg_first )
/*************************************************************/
{
    hw_reg_set  tmp;

    if( !HW_COvlap( seg->result->r.reg, HW_DS_ES_FS_GS ) ) return;
    if( !AdjacentMem( seg->operands[0], reg->operands[0], U2 ) ) return;
    if( seg->operands[0]->n.class == N_INDEXED ) {
        // special case of using result of seg
        if( seg_first ) {
            // don't think we can get here - using one of DS|ES|FS|GS as index reg?!?
            if( HW_Ovlap( reg->operands[0]->i.index->r.reg, seg->result->r.reg ) ) return;
        } else {
            if( HW_Ovlap( seg->operands[0]->i.index->r.reg, reg->result->r.reg ) ) return;
        }
    }
    reg->u.gen_table = LDSES;
    tmp = reg->result->r.reg;
    HW_TurnOn( tmp, seg->result->r.reg );
    reg->result = AllocRegName( tmp );
    DoNothing( seg );
}


extern  void    OptSegs( void )
/*****************************/
{
    block       *blk;
    instruction *ins;
    instruction *next;
    bool        redo;
    instruction *tmp;

    do {
        redo = FALSE;
        for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
            for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = next ) {
                next = ins->head.next;
                if( NotByteMove( ins ) && NotByteMove( next ) ) {
                    if( IsLESDS( ins, next ) ) {
                        CheckLDSES( next, ins, FALSE );
                    } else if( IsLESDS( next, ins ) ) {
                        CheckLDSES( ins, next, TRUE );
                    }
                }
                if( MemMove( ins )  && !VolatileIns( ins )
                 && MemMove( next ) && !VolatileIns( next ) ) {
                    // look for adjacent byte/word moves and combine them
                    if( OptMemMove( ins, next ) ) {
                        // tell ourselves to go around again in case
                        // we combined four byte moves into two word
                        // moves - want to combine into dword move
                        switch( next->type_class ) {
                        case I1:
                        case I2:
                        case U1:
                        case U2:
                            redo = TRUE;
                            break;
                        default:
                            break;
                        }
                        tmp = next->head.next;
                        FreeIns( next );
                        next = tmp;
                    }
                }
#if _TARGET & _TARG_IAPX86
                /* The scoreboarder may split "and ax,imm" into
                   "xor ah,ah; and al,imm". This is sometimes useful
                   (ah can be eliminated for
                      short a, b, c; a &= 0x01; b &= 0x0f; c = (a|b) & 0xff;)
                   but produces longer code if it is not. Remerge them here.
                */
                if( 
                 /* is next of the form "and byte, imm" ? */
                    ( next->head.opcode == OP_AND )
                 && ( next->type_class == I1 || next->type_class == U1 )
                 && ( next->result->n.class == N_REGISTER )
                 && ( next->operands[0] == next->result )
                 && ( next->operands[1]->n.class == N_CONSTANT ) 

                 /* is ins of the form "xor byte, byte" ? */
                 && ( ins->head.opcode == OP_XOR )
                 && ( ins->type_class == next->type_class )
                 && ( ins->result->n.class == N_REGISTER )
                 && ( ins->operands[0] == ins->result )
                 && ( ins->operands[1] == ins->result ) ) {
                    hw_reg_set full_reg = FullReg( next->result->r.reg );
                    /* check if instructions operate on correct halves */
                    if ( HW_Equal( Low16Reg( full_reg ), next->result->r.reg )
                      && HW_Equal( High16Reg( full_reg ), ins->result->r.reg )
                       ) {
                        /* convert to "and fullreg, imm" */
                        next->type_class = next->type_class == I1 ? I2 : U2;
                        next->result->r.reg = full_reg;
                        next->operands[1] = AllocIntConst( next->operands[1]->c.int_value & 0xFF );
                        DoNothing(ins);
                    }
                }
                if( _CPULevel( CPU_386 ) ) {
                    if( isPushX2( ins ) && isPushX2( next ) ) {
                        if( OptPushDWORDConstant( ins, next ) ) {
                            // 'next' was not a OP_BLOCK if we optimized it
                            next = next->head.next;
                        } else if( OptPushDWORDMemory( ins, next ) ) {
                            next = next->head.next;
                        }
                    }
                }
#endif
            }
        }
    } while( redo );
}
