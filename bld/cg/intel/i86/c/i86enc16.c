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
* Description:  16-bit instruction encodings.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "ocentry.h"
#include "pccode.h"
#include "system.h"
#include "escape.h"
#include "pcencode.h"
#include "zoiks.h"
#include "zeropage.h"
#include "fppatch.h"
#include "cfloat.h"
#include "data.h"


extern  void            DoSymRef( name *, offset, bool );
extern  void            LayRegAC( hw_reg_set );
extern  void            LayOpword( gen_opcode );
extern  hw_reg_set      High32Reg( hw_reg_set );
extern  hw_reg_set      Low32Reg( hw_reg_set );
extern  hw_reg_set      CalcSegment( cg_sym_handle, cg_class );
extern  name            *DeAlias( name * );
extern  void            AddByte( byte );
extern  void            LayRMRegOp( name * );
extern  void            LayOpbyte( gen_opcode );
extern  void            LayRegRM( hw_reg_set );
extern  void            DoSegRef( segment_id );
extern  void            GenSeg( hw_reg_set );
extern  void            LayW( type_class_def );
extern  void            AddWCons( name *, type_class_def );
extern  void            AddSData( signed_32, type_class_def );
extern  void            AddWData( signed_32, type_class_def );
extern  void            AddToTemp( byte );
extern  void            DoAbsPatch( abspatch_handle *, int );
extern  type_class_def  OpndSize( hw_reg_set );
extern  void            LayReg( hw_reg_set );
extern  void            GCondFwait( void );
extern  type_length     NewBase( name * );
extern  int             GetLog2( unsigned_32 );
extern  unsigned        UseRepForm( unsigned );

/* forward declarations */
extern  void            DoRelocConst( name *op, type_class_def kind );
static  void            SetOff( name *op, int val );

extern  zero_page_scheme        ZPageType;

#define RMR_MOD_DIR     6
#define RMR_MOD_IND     0x80

#define D0      (0 << S_RMR_MOD)
#define D8      (1 << S_RMR_MOD)
#define D16     (2 << S_RMR_MOD)

static void OpndSizeIf( void )
/****************************/
{
    if( _IsTargetModel( USE_32 ) ) {
        AddToTemp( M_OPND_SIZE );
    }
}

static  hw_reg_set IndexTab[] = {
#define INDICES 8
#define BP_INDEX 6
        HW_D_2( HW_BX, HW_SI ),
        HW_D_2( HW_BX, HW_DI ),
        HW_D_2( HW_BP, HW_SI ),
        HW_D_2( HW_BP, HW_DI ),
        HW_D_1( HW_SI ),
        HW_D_1( HW_DI ),
        HW_D_1( HW_BP ),
        HW_D_1( HW_BX ) };

static  byte    DoIndex( hw_reg_set regs )
/****************************************/
{
    int i;

    for( i = 0; i < INDICES; ++i ) {
        if( HW_Equal( regs, IndexTab[i] ) ) {
            break;
        }
    }
    if( i >= INDICES ) {
        _Zoiks( ZOIKS_033 );
    }
    i <<= S_RMR_RM;
    return( i );
}


static  byte    Displacement( signed_32 val, hw_reg_set regs )
/************************************************************/
{
    HW_CTurnOff( regs, HW_SEGS );
    if( val == 0 && !HW_CEqual( regs, HW_BP ) )
        return( D0 );
    if( val <= 127 && val >= -128 ) {
        AddByte( val & 0xff );
        return( D8 );
    } else {
        val &= 0xffff;
        AddByte( val );
        AddByte( val >> 8 );
        return( D16 );
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
        dmod = D16;
        ILen += 2;
        DoSymRef( op->i.base, val, FALSE );
    }
    return( dmod );
}


extern  void    DoRepOp( instruction *ins )
/*****************************************/
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


extern  void    Do4CXShift( instruction *ins, void (*rtn)(instruction *) )
/************************************************************************/
{
    hw_reg_set  hreg;
    hw_reg_set  lreg;
    unsigned    shift;

    if( ins->operands[1]->n.class == N_CONSTANT ) {
        shift = ins->operands[1]->c.int_value;
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


extern  void    LayLeaRegOp( instruction *ins )
/*********************************************/
{
    name        *left;
    name        *right;

    left = ins->operands[0];
    right = ins->operands[1];
    Inst[RMR] |= DoIndex( left->r.reg );
    if( ins->head.opcode == OP_ADD ) {
        if( right->c.const_type == CONS_ABSOLUTE ) {
            Inst[RMR] |=
                      Displacement( right->c.int_value,
                                    left->r.reg );
        } else {
            Inst[RMR] |= D16;
            DoRelocConst( right, U2 );
        }
    } else {
        Inst[RMR] |= Displacement( -right->c.int_value,
                                    left->r.reg );
    }
}


static  void    CheckSize( void )
/*******************************/
{
    if( _IsTargetModel( USE_32 ) ) {
        AddToTemp( M_ADDR_SIZE );
    }
}


extern  void    DoMAddr( name *op )
/*********************************/
{
    ILen += 2;
    if( op->n.class == N_CONSTANT ) {
        _Zoiks( ZOIKS_035 );
    } else {        /* assume global name*/
        DoSymRef( op, op->v.offset, FALSE );
    }
}


extern  byte    DoMDisp( name *op, bool alt_encoding )
/****************************************************/
{
    hw_reg_set          regs;
    zero_page_scheme    zptype;


    regs = CalcSegment( op->v.symbol, op->m.memory_type );
    if( ( alt_encoding == FALSE && ZPageType != ZP_USES_DS )
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


extern  void    LayModRM( name *op )
/**********************************/
{
    hw_reg_set  regs;
    name        *base;
    hw_reg_set  tmp;

    switch( op->n.class ) {
    case N_MEMORY:
        CheckSize();
        Inst[RMR] |= DoMDisp( op, TRUE );
        break;
    case N_TEMP:
        CheckSize();
        base = DeAlias( op );
        if( base->t.location == NO_LOCATION ) {
            _Zoiks( ZOIKS_030 );
        }
        Inst[RMR] |= Displacement( NewBase( base ) + op->v.offset
                                        - base->v.offset, HW_BP )
                         + ( BP_INDEX << S_RMR_RM );
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
        Inst[RMR] |= DoDisp( op, regs ) + DoIndex( regs );
        break;
    case N_REGISTER:
        LayRMRegOp( op );
        break;
    default:
        break;
    }
}


extern  void    DoRelocConst( name *op, type_class_def kind )
/***********************************************************/
{
    kind = kind;
    ILen += 2;
    if( op->c.const_type == CONS_OFFSET ) {
        DoSymRef( op->c.value, ((var_name *)op->c.value)->offset, FALSE );
    } else if( op->c.const_type == CONS_SEGMENT ) {
        if( op->c.value == NULL ) {
            DoSegRef( op->c.int_value );
        } else {
            DoSymRef( op->c.value, 0, TRUE );
        }
    } else if( op->c.const_type == CONS_ADDRESS ) {
        _Zoiks( ZOIKS_045 );
    }
}


extern  void    Do4Shift( instruction *ins )
/******************************************/
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


extern  void    Do4RShift( instruction *ins )
/*******************************************/
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


extern  void    Gen4RNeg( instruction *ins )
/******************************************/
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


extern  void    Gen4Neg( instruction *ins )
/*****************************************/
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


extern  void    By2Div( instruction *ins )
/****************************************/
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


extern void Pow2Div286( instruction *ins )
/****************************************/
{
    int         log2;

    log2 = GetLog2( ins->operands[1]->c.int_value );
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


extern  void    Pow2Div( instruction *ins )
/*****************************************/
{
    int         log2;

    log2 = GetLog2( ins->operands[1]->c.int_value );
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

extern  void    GenUnkLea( pointer value )
/****************************************/
{
    LayOpword( M_LEA );
    OpndSize( HW_SP );
    LayReg( HW_SP );
    Inst[RMR] |= D16;
    ILen += 2;
    DoAbsPatch( value, 2 );
    Inst[RMR] |= DoIndex( HW_BP );
}

extern  void    GenLeaSP( int offset )
/**************************************
    LEA         sp,offset[bp]
*/
{
    _Code;
    LayOpword( M_LEA );
    OpndSize( HW_SP );
    LayReg( HW_SP );
    Inst[RMR] |= Displacement( offset, HW_BP );
    Inst[RMR] |= DoIndex( HW_BP );
    _Emit;
}

extern  void    GFstp10( type_length where )
/******************************************/
{
    GCondFwait();
    LayOpword( 0x3edb );
    Inst[RMR] |= Displacement( -where, HW_BP );
    _Emit;
}


extern  void    GFld10( type_length where )
/*****************************************/
{
    GCondFwait();
    LayOpword( 0x2edb );
    Inst[RMR] |= Displacement( -where, HW_BP );
    _Emit;
}

void StartBlockProfiling( block *blk )
/************************************/
{
    blk = blk;
}

void EndBlockProfiling( void )
/****************************/
{
}

segment_id GenP5ProfileData( char *fe_name, label_handle *data, label_handle *stack )
/*****************************************************************************/
{
    fe_name = fe_name;
    data = data;
    stack = stack;
    return( 0 );
}
