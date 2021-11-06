/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  16-bit instruction encodings.
*
****************************************************************************/


#include "_cgstd.h"
#include "coderep.h"
#include "pccode.h"
#include "system.h"
#include "pcencode.h"
#include "zoiks.h"
#include "zeropage.h"
#include "cfloat.h"
#include "data.h"
#include "x86objd.h"
#include "objprof.h"
#include "targetdb.h"
#include "x86obj.h"
#include "x86esc.h"
#include "rgtbl.h"
#include "namelist.h"
#include "tree.h"
#include "treefold.h"
#include "x86segs.h"
#include "x86enc.h"
#include "x86opseg.h"


#define RMR_MOD_DIR     6
#define RMR_MOD_IND     0x80


static  hw_reg_set IndexTab[] = {
    HW_D_2( HW_BX, HW_SI ),
    HW_D_2( HW_BX, HW_DI ),
    HW_D_2( HW_BP, HW_SI ),
    HW_D_2( HW_BP, HW_DI ),
    HW_D_1( HW_SI ),
    HW_D_1( HW_DI ),
    HW_D_1( HW_BP ),
    HW_D_1( HW_BX )
};

static void OpndSizeIf( void )
/****************************/
{
    if( _IsTargetModel( USE_32 ) ) {
        AddToTemp( M_OPND_SIZE );
    }
}

byte    DoIndex( hw_reg_set regs )
/********************************/
{
    int i;

    for( i = 0; i < sizeof( IndexTab ) / sizeof( IndexTab[0] ); i++ ) {
        if( HW_Equal( regs, IndexTab[i] ) ) {
            return( i << S_RMR_RM );
        }
    }
    _Zoiks( ZOIKS_033 );
    return( 0 );
}


byte    Displacement( signed_32 val, hw_reg_set regs )
/****************************************************/
{
    HW_CTurnOff( regs, HW_SEGS );
    if( val == 0 && !HW_CEqual( regs, HW_BP ) )
        return( DISP0 );
    if( val <= 127 && val >= -128 ) {
        AddByte( val & 0xff );
        return( DISP8 );
    } else {
        val &= 0xffff;
        AddByte( val );
        AddByte( val >> 8 );
        return( DISPW );
    }
}


static  byte    DoDisp( name *op, hw_reg_set regs )
/*************************************************/
{
    name        *base;
    int         val;
    byte        dmod;
    name        *temp_base;

    val = op->i.constant;              /* use integer value*/
    base = op->i.base;
    if( !HasTrueBase( op ) ) {
        dmod = Displacement( val, regs );
    } else if( base->n.class == N_TEMP ) {
        temp_base = DeAlias( base );
        if( temp_base->t.location == NO_LOCATION ) {
            _Zoiks( ZOIKS_034 );
        }
        val += NewBase( temp_base ) + base->v.offset - temp_base->v.offset;
        dmod = Displacement( val, regs );
    } else {
        dmod = DISPW;
        ILen += 2;
        DoSymRef( op->i.base, val, false );
    }
    return( dmod );
}


void    DoRepOp( instruction *ins )
/*********************************/
{
    int size;

    size = ins->result->n.size;
    if( ins->head.opcode == OP_MOV && !UseRepForm( size ) ) {
        LayOpbyte( M_MOVSW );
        size -= 2;
        while( size > 1 ) {
            AddByte( M_MOVSW );
            size -= 2;
        }
        if( size == 1 ) {
            AddByte( M_MOVSB );
        }
    } else {
        LayOpbyte( M_REPE );
        if( ins->head.opcode == OP_MOV ) {
            if( ( size & 1 ) == 0 || OptForSize <= 50 ) {
                AddByte( M_MOVSW );
                if( size & 1 ) {
                    AddByte( M_MOVSB );
                }
            } else {
                AddByte( M_MOVSB );
            }
        } else {
            if( ins->operands[1]->n.size & 1 ) {
                AddByte( M_CMPSB );
            } else {
                AddByte( M_CMPSW );
            }
        }
    }
}


void    Do4CXShift( instruction *ins, void (*rtn)(instruction *) )
/****************************************************************/
{
    hw_reg_set  hreg;
    hw_reg_set  lreg;
    unsigned    shift;

    if( ins->operands[1]->n.class == N_CONSTANT ) {
        shift = ins->operands[1]->c.lo.int_value;
        if( shift < 16 && OptForSize < 50 && ins->result->n.class == N_REGISTER ) {
            hreg = ins->result->r.reg;
            lreg = Low32Reg( hreg );
            hreg = High32Reg( hreg );
            if( ins->head.opcode == OP_LSHIFT ) {
                LayOpbyte( 0xb0 );   /* Move register immediate (byte)*/
                LayRegAC( HW_CL );
                AddByte( shift );
                _Next;
                LayOpword( M_SHL1 | B_KEY_W | 0x02 ); /* SHL reg,CL (word) */
                LayRegRM( hreg );
                _Next;
                LayOpword( 0x00d3 );    /* ROL reg,CL (word) */
                LayRegRM( lreg );
                _Next;
                LayOpword( M_XORRR | B_KEY_W );
                LayReg( lreg );
                LayRegRM( hreg );
                _Next;
                LayOpword( 0x2081 ); /* AND reg,immed (word) */
                LayRegRM( lreg );
                AddWData( ((unsigned_16)~0) << shift, U2 );
                _Next;
                LayOpword( M_XORRR | B_KEY_W );
                LayReg( lreg );
                LayRegRM( hreg );
                return;
            } else if( ins->type_class == U4 ) {
                LayOpbyte( 0xb0 );   /* Move register immediate (byte)*/
                LayRegAC( HW_CL );
                AddByte( shift );
                _Next;
                LayOpword( M_SHR1 | B_KEY_W | 0x02 ); /* SHR reg,CL (word) */
                LayRegRM( lreg );
                _Next;
                LayOpword( 0x08d3 );    /* ROR reg,CL (word) */
                LayRegRM( hreg );
                _Next;
                LayOpword( M_XORRR | B_KEY_W );
                LayReg( hreg );
                LayRegRM( lreg );
                _Next;
                LayOpword( 0x2081 ); /* AND reg,immed (word) */
                LayRegRM( hreg );
                AddWData( ((unsigned_16)~0) >> shift, U2 );
                _Next;
                LayOpword( M_XORRR | B_KEY_W );
                LayReg( hreg );
                LayRegRM( lreg );
                return;
            }
        }
        LayOpbyte( 0xb8 );   /* Move register immediate (word)*/
        LayRegAC( HW_CX );
        AddWCons( ins->operands[1], U2 );
        _Next;
        rtn( ins );
        _Next;
        LayOpword( M_LOOP );
        Inst[DATALO] = - ( -3 + 2 + Temp.hdr.objlen );
    } else {
        LayOpbyte( 0xe3 );    /* JCXZ*/
        AddByte( 0 );        /* to be patched*/
        _Next;
        rtn( ins );
        _Next;
        LayOpword( M_LOOP );
        Inst[DATALO] = - ( -2 + 2 + Temp.hdr.objlen );
        Temp.data[1] = -Inst[DATALO];
    }
}


void    LayLeaRegOp( instruction *ins )
/*************************************/
{
    name        *left;
    name        *right;

    left = ins->operands[0];
    right = ins->operands[1];
    Inst[RMR] |= DoIndex( left->r.reg );
    if( ins->head.opcode == OP_ADD ) {
        if( right->c.const_type == CONS_ABSOLUTE ) {
            Inst[RMR] |= Displacement( right->c.lo.int_value, left->r.reg );
        } else {
            Inst[RMR] |= DISPW;
            DoRelocConst( right, U2 );
        }
    } else {
        Inst[RMR] |= Displacement( -right->c.lo.int_value, left->r.reg );
    }
}


static  void    CheckSize( void )
/*******************************/
{
    if( _IsTargetModel( USE_32 ) ) {
        AddToTemp( M_ADDR_SIZE );
    }
}


void    DoMAddr( name *op )
/*************************/
{
    ILen += 2;
    if( op->n.class == N_CONSTANT ) {
        _Zoiks( ZOIKS_035 );
    } else {        /* assume global name*/
        DoSymRef( op, op->v.offset, false );
    }
}


byte    DoMDisp( name *op, bool alt_encoding )
/********************************************/
{
    hw_reg_set          regs;
    zero_page_scheme    zptype;


    regs = CalcSegment( op->v.symbol, op->m.memory_type );
    if( ( !alt_encoding && ZPageType != ZP_USES_DS )
      ||  !HW_COvlap( regs, HW_SS ) ) {
        zptype = ZP_USES_SS;
    } else {
        zptype = ZPageType;
    }
    switch( zptype ) {
    case ZP_USES_SS:
        if( HW_COvlap( regs, HW_SEGS ) ) {
            GenSeg( regs );
        }
        DoMAddr( op );
        return( RMR_MOD_DIR );
    case ZP_USES_DS:
        DoMAddr( op );
        return( RMR_MOD_DIR );
    case ZP_USES_BP:
        DoMAddr( op );
        return( DoIndex( HW_BP ) + RMR_MOD_IND );
    case ZP_USES_DI:
        DoMAddr( op );
        return( DoIndex( HW_BP_DI ) + RMR_MOD_IND );
    case ZP_USES_SI:
        DoMAddr( op );
        return( DoIndex( HW_BP_SI ) + RMR_MOD_IND );
    }
    return( 0 );
}


void    LayModRM( name *op )
/**************************/
{
    hw_reg_set  regs;
    name        *base;
    hw_reg_set  tmp;

    switch( op->n.class ) {
    case N_MEMORY:
        CheckSize();
        Inst[RMR] |= DoMDisp( op, true );
        break;
    case N_TEMP:
        CheckSize();
        base = DeAlias( op );
        if( base->t.location == NO_LOCATION ) {
            _Zoiks( ZOIKS_030 );
        }
        Inst[RMR] |= Displacement( NewBase( base ) + op->v.offset - base->v.offset, HW_BP );
        Inst[RMR] |= DoIndex( HW_BP );
        break;
    case N_INDEXED:
        CheckSize();
        regs = op->i.index->r.reg;
        if( HW_COvlap( regs, HW_SEGS ) ) {
            /* check for seg override*/
            GenSeg( regs );
            HW_CTurnOff( regs, HW_SEGS );
        }
        if( HasTrueBase( op ) ) {
            if( op->i.base->n.class == N_TEMP ) {
                HW_CTurnOn( regs, HW_BP );
            } else if( op->i.base->n.class == N_MEMORY ) {
                tmp = CalcSegment( op->i.base->v.symbol,
                                  op->i.base->m.memory_type );
                HW_TurnOn( regs, tmp );
                if( HW_COvlap( regs, HW_SS ) ) {
                    switch( ZPageType ) {
                    case ZP_USES_SS:
                    case ZP_USES_DI:
                    case ZP_USES_SI:
                        GenSeg( regs );
                        break;
                    case ZP_USES_DS:
                        /* nothing*/
                        break;
                    case ZP_USES_BP:
                        if( HW_COvlap( regs, HW_BX ) ) {
                            GenSeg( regs );
                        } else {
                            HW_CTurnOn( regs, HW_BP );
                        }
                        break;
                    }
                } else {
                    GenSeg( regs );
                }
                HW_CTurnOff( regs, HW_SEGS );
            }
        }
        Inst[RMR] |= DoDisp( op, regs );
        Inst[RMR] |= DoIndex( regs );
        break;
    case N_REGISTER:
        LayRMRegOp( op );
        break;
    default:
        break;
    }
}


void    DoRelocConst( name *op, type_class_def type_class )
/*********************************************************/
{
    /* unused parameters */ (void)type_class;

    ILen += 2;
    if( op->c.const_type == CONS_OFFSET ) {
        DoSymRef( op->c.value, ((var_name *)op->c.value)->offset, false );
    } else if( op->c.const_type == CONS_SEGMENT ) {
        if( op->c.value == NULL ) {
            DoSegRef( op->c.lo.int_value );
        } else {
            DoSymRef( op->c.value, 0, true );
        }
    } else if( op->c.const_type == CONS_ADDRESS ) {
        _Zoiks( ZOIKS_045 );
    }
}

static  void    SetOff( name *op, int val )
/*****************************************/
{
    if( op->n.class == N_INDEXED ) {
        op->i.constant += val;
    } else if( op->n.class == N_TEMP ) {
        op = DeAlias( op );
        op->t.location += val;
    } else { /* N_MEMORY*/
        op->v.offset += val;
    }
}


void    Do4Shift( instruction *ins )
/**********************************/
{
    name        *op;

    op = ins->result;
    if( ins->head.opcode == OP_LSHIFT ) {
        LayOpword( M_SHL1 | B_KEY_W );
        LayModRM( op );
        _Next;
        SetOff( op,  2 );
        LayOpword( M_RCL1 | B_KEY_W );
        LayModRM( op );
        SetOff( op, -2 );
    } else {
        SetOff( op,  2 );
        LayOpword( M_SHR1 | B_KEY_W );
        if( ins->type_class == I4 ) {
            Inst[RMR] |= B_RMR_SHR_SAR;
        }
        LayModRM( op );
        _Next;
        SetOff( op, -2 );
        LayOpword( M_RCR1 | B_KEY_W );
        LayModRM( op );
    }
}


void    Do4RShift( instruction *ins )
/***********************************/
{
    hw_reg_set  regs;

    regs = ins->result->r.reg;
    if( ins->head.opcode == OP_LSHIFT ) {
        LayOpword( M_SHL1 | B_KEY_W );
        LayRegRM( Low32Reg( regs ) );
        _Next;
        LayOpword( M_RCL1 | B_KEY_W );
        LayRegRM( High32Reg( regs ) );
    } else {
        LayOpword( M_SHR1 | B_KEY_W );
        if( ins->type_class == I4 ) {
            Inst[RMR] |= B_RMR_SHR_SAR;
        }
        LayRegRM( High32Reg( regs ) );
        _Next;
        LayOpword( M_RCR1 | B_KEY_W );
        LayRegRM( Low32Reg( regs ) );
    }
}


void    Gen4RNeg( instruction *ins )
/**********************************/
{
    hw_reg_set  regs;

    regs = ins->result->r.reg;
    LayOpword( M_NEG );
    LayW( I2 );
    LayRegRM( High32Reg( regs ) );
    _Next;
    LayOpword( M_NEG );
    LayW( I2 );
    LayRegRM( Low32Reg( regs ) );
    _Next;
    LayOpword( M_SBB );
    LayW( I2 );
    LayRegRM( High32Reg( regs ) );
    AddSData( 0, I2 );
}


void    Gen4Neg( instruction *ins )
/*********************************/
{
    name        *res;

    res = ins->result;
    SetOff( res,  2 );
    LayOpword( M_NEG );
    LayW( I2 );
    LayModRM( res );
    _Next;
    SetOff( res, -2 );
    LayOpword( M_NEG );
    LayW( I2 );
    LayModRM( res );
    _Next;
    SetOff( res,  2 );
    LayOpword( M_SBB );
    LayW( I2 );
    LayModRM( res );
    AddSData( 0, I2 );
    SetOff( res, -2 );
}


void    By2Div( instruction *ins )
/********************************/
{
    switch( ins->type_class ) {
    case I1:
    case U1:
        LayOpword( 0xc42a );    /* sub  al,ah */
        _Next;
        LayOpword( 0xf8d0 );    /* sar  al,1 */
        break;
    case I2:
    case U2:
        LayOpword( 0xc22b );    /* sub  ax,dx */
        OpndSizeIf();
        _Next;
        LayOpword( 0xf8d1 );    /* sar  ax,1 */
        OpndSizeIf();
    default:
        break;
    }
}


void Pow2Div286( instruction *ins )
/*********************************/
{
    int         log2;

    log2 = GetLog2( ins->operands[1]->c.lo.int_value );
    switch( ins->type_class ) {
    case I1:
    case U1:
        LayOpword( 0xe4c0 );    /* shl  ah,n */
        AddByte( log2 );
        _Next;
        LayOpword( 0xc41a );    /* sbb  al,ah */
        _Next;
        LayOpword( 0xf8c0 );    /* sar  al,n */
        AddByte( log2 );
        break;
    case I2:
    case U2:
        LayOpword( 0xe2c1 );    /* shl  dx,n */
        OpndSizeIf();
        AddByte( log2 );
        _Next;
        LayOpword( 0xc21b );    /* sbb  ax,dx */
        OpndSizeIf();
        _Next;
        LayOpword( 0xf8c1 );    /* sar  ax,n */
        OpndSizeIf();
        AddByte( log2 );
        break;
    default:
        break;
    }
}


void    Pow2Div( instruction *ins )
/*********************************/
{
    int         log2;

    log2 = GetLog2( ins->operands[1]->c.lo.int_value );
    switch( ins->type_class ) {
    case I1:
    case U1:
        LayOpbyte( 0xb1 );              /* mov  cl,n */
        AddByte( log2 );
        _Next;
        LayOpbyte( 0xe4d2 );    /* shl  ah,cl */
        _Next;
        LayOpword( 0xc41a );    /* sbb  al,ah */
        _Next;
        break;
    case I2:
    case U2:
        LayOpbyte( 0xb1 );              /* mov  cl,n */
        AddByte( log2 );
        _Next;
        LayOpword( 0xe2d3 );    /* shl  dx,cl */
        OpndSizeIf();
        _Next;
        LayOpword( 0xc21b );    /* sbb  ax,dx */
        OpndSizeIf();
        _Next;
        LayOpword( 0xf8d3 );    /* sar  ax,cl */
        OpndSizeIf();
        break;
    default:
        break;
    }
}

void    GFstp10( type_length where )
/**********************************/
{
    GCondFwait();
    LayOpword( 0x3edb );
    Inst[RMR] |= Displacement( -where, HW_BP );
    _Emit;
}


void    GFld10( type_length where )
/*********************************/
{
    GCondFwait();
    LayOpword( 0x2edb );
    Inst[RMR] |= Displacement( -where, HW_BP );
    _Emit;
}

void StartBlockProfiling( block *blk )
/************************************/
{
    /* unused parameters */ (void)blk;
}

void EndBlockProfiling( void )
/****************************/
{
}

segment_id GenProfileData( char *fe_name, label_handle *data, label_handle *stack )
/***********************************************************************************/
{
    /* unused parameters */ (void)fe_name; (void)data; (void)stack;

    return( 0 );
}
