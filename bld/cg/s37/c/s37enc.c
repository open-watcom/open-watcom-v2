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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "standard.h"
#include "coderep.h"
#include "typedef.h"
#include "opcodes.h"
#include "pattern.h"
#include "vergen.h"
#include "offset.h"
#include "model.h"
#include "s37bead.h"
#include "procdef.h"
#include "zoiks.h"
#include "cgaux.h"

#include "s37enc.def"
#ifndef NXDEBUG
extern  void            DumpInsOnly( instruction * );
extern  void            DumpString( char * );
extern  void            DumpPtr( pointer );
extern  void            DumpInt(int);
extern  void            DumpNL(void);
extern  void            DumpGen(struct opcode_entry*);
#endif
extern  name           *DeAlias(name*);
extern  void            HWInsGen( hwins_opcode hwop, hwins_op_any *op1,
                                         hwins_op_any *op2,
                                         hwins_op_any *op3);
extern  void            HWBRGen(char ,hwins_op_any *);
extern void             HWBIndexGen(char ,hw_sym *);

extern  ref_any        *HWSymRef( hw_sym  * );
extern  ref_any        *HWDispRef( hw_sym  *, hw_sym  * );
extern  ref_any        *HWLitIntGen( offset, int );
extern  ref_any        *HWLitFltGen( pointer, int );
extern ref_any         *HWLitAddr(hw_sym *,offset, bool );
extern bead_addr       *HWSymAddr(hw_sym *,offset ,bool );
extern  void            HWEntry(hw_sym *);
extern  void            HWExtern( hw_sym *);
extern void             HWLabelGen(hw_sym *,char );
extern  void            HWQueue(int );
extern  bool            AskIfRTLabel(label_handle);
extern  char           *AskRTName(int);

extern  sym_handle      AskForLblSym(label_handle);
extern  hw_sym         *AskForHWLabel( label_handle );
extern  hw_loc         *AskForBaseHW( label_handle );
extern  offset          AskAddress( label_handle );
extern  label_handle    AskForSymLabel( pointer, cg_class );
extern  char           *FEName(sym_handle);
extern  fe_attr         FEAttr(pointer);
extern  name           *AllocRegName(hw_reg_set);
extern  void            FlipCond(instruction*);
extern  int             Log2(unsigned_32);
extern  pointer         FEAuxInfo(pointer,int);
extern  void            PSBlip();
extern bool             MemNeedsReloc(pointer ,cg_class );
extern  void            AskRTRALN(int,char*,char*);

extern  proc_def        *CurrProc;
extern  type_length     TypeClassSize[];

static byte UCondTable[] = {
        7,              /* OP_BIT_TEST_TRUE*/
        8,              /* OP_BIT_TEST_FALSE*/
        8,              /* OP_CMP_EQUAL*/
        7,              /* OP_CMP_NOT_EQUAL*/
        2,              /* OP_CMP_GREATER*/
        13,             /* OP_CMP_LESS_EQUAL*/
        4,              /* OP_CMP_LESS*/
        11 };           /* OP_CMP_GREATER_EQUAL*/

static byte SCondTable[] = {
        7,              /* OP_BIT_TEST_TRUE*/
        8,              /* OP_BIT_TEST_FALSE*/
        8,              /* OP_CMP_EQUAL*/
        7,              /* OP_CMP_NOT_EQUAL*/
        2,              /* OP_CMP_GREATER*/
        13,             /* OP_CMP_LESS_EQUAL*/
        4,              /* OP_CMP_LESS*/
        11 };           /* OP_CMP_GREATER_EQUAL*/

typedef enum {
        UNSIGNED,
        SIGNED,
        SIGNED_BOTH
} issigned;

static issigned Signed[] = {
        UNSIGNED,       /* U1*/
        SIGNED_BOTH,    /* I1*/
        UNSIGNED,       /* U2*/
        SIGNED_BOTH,    /* I2*/
        UNSIGNED,       /* U4*/
        SIGNED_BOTH,    /* I4*/
        UNSIGNED,       /* CP*/
        UNSIGNED,       /* PT*/
        SIGNED,         /* FS*/
        SIGNED,         /* FD*/
        UNSIGNED };     /* XX*/


static hwins_opcode GtoHWTable[] = {

    HWOP_LAST, /* G_NO */
    #define _ENC_GHW( op )  HWOP##op,
    #include "ghw.h"
    #undef _ENC_GHW
    HWOP_LAST
};

#define MAXMAP sizeof( GtoHWTable )/sizeof(hwins_opcode)


extern  void    GenObjCode( instruction *ins )  {
/********************************************/

    gentype         gen;
    hwins_opcode    hwins;
    name           *result;
    name           *left;
    name           *right;
    type_class_def  tipe;

    PSBlip();

#ifndef NXDEBUG
    if( _IsModel( INTERNAL_DBG_OUTPUT ) ) { /* debug dump */
        DumpString( "        " );
        DumpGen( ins->u.gen_table );
        DumpString( " - " );
        DumpInsOnly( ins );
        DumpNL();
    }
#endif
    gen = ins->u.gen_table->generate;
    tipe = ins->type_class;
    if( gen != G_NO ) {
        result = ins->result;
        if( ins->num_operands != 0 ) {
            left = ins->operands[ 0 ];
            if( ins->num_operands != 1 ) {
                right = ins->operands[ 1 ];
            }
        }
        if( gen < MAXMAP ) {
            hwins = GtoHWTable[ gen ];
        }
        switch( gen ) {
        case G_AR: /* GPR result right */
        case G_ALR:
        case G_DR:
        case G_MR:
        case G_NR:
        case G_SR:
        case G_SLR:
        case G_XR:
        case G_OR:
            GenRR( hwins, result, right );
            break;
        case G_A: /* GPR RX result right */
        case G_AL:
        case G_D:
        case G_M:
        case G_O:
        case G_N:
        case G_S:
        case G_SL:
        case G_X:
            GenRX( hwins, result, right );
            break;
        case G_AH:/* GPR RX I2 result right */
        case G_MH:
        case G_SH:
            GenRXH( hwins, result, right );
            break;
        case G_SLA:
        case G_SLL:
        case G_SRA:
        case G_SRL:
            GenShift( hwins, result, right );
            break;
        case G_CR: /* GPR RR left right */
        case G_CLR:
            GenRR( hwins, left, right );
            break;
        case G_CH: /* GPR RX I2 left right */
            GenRXH( hwins, left, right );
            break;
        case G_C: /* GPR RX left right */
        case G_CL:
            GenRX( hwins, left, right );
            break;
        case G_CLI:/* SI left right */
            GenSI( hwins, left, right );
            break;
        case G_CLC:/* SS left right */
            GenSS( hwins, left, right, tipe );
            break;
        case G_LH: /* GPR I2 result left */
            GenRXH( hwins, result, left );
            break;
        case G_L: /* GPR RX result, left */
        case G_LA:
        case G_IC:
            GenRX( hwins, result, left );
            break;
        case G_LR:
        case G_LTR:/* GPR RR result left */
        case G_LCR:
        case G_LNR:
        case G_LPR:
            GenRR( hwins, result, left );
            break;
        case G_MVC:/* SS result left */
            GenSS( hwins, result, left, tipe );
            break;
        case G_NC:/* SS result right */
        case G_OC:
        case G_XC:
            GenSS( hwins, result, right, tipe );
            break;
        case G_MVI:/* SI result left */
            GenSI( hwins, result, left );
            break;
        case G_NI:/* SI result, right */
        case G_OI:
        case G_XI:
            GenSI( hwins, result, right );
            break;
        case G_STC:/* GPR RX left result */
        case G_ST:
            GenRX( hwins, left, result );
            break;
        case G_STH: /* GPR I2 left, result */
            GenRXH( hwins, left, result );
            break;
        case G_CLRR:
            GenCLRR( ins->type_class, result );
            break;
        case G_RS_DSHFT:
            break;
        case G_SCONV1 :
            GenRmS( HWOP_ICM, result, 0x8, left );
            GenRDisp( HWOP_SRA, result, 0, 24 );
            break;
        case G_SCONV8 :
            GenRDisp( HWOP_SRDA, result, 0, 32 );
            break;
        case G_UCONV1 :
            GenUConv1( result, left );
            break;
        case G_UCONV2 :
            GenUConv2( result, left );
            break;
        case G_LA1 :
            GenRDisp( HWOP_LA, result, 0, left->c.int_value );
            break;
        case G_LA2 :
            GenRDisp( HWOP_LA, result, RegOp(left), right->c.int_value );
            break;
        case G_LTZ:
            GenRR( HWOP_LTR, left, left );
            break;
        case G_CALL:
            GenCall( ins );
            break;
        case G_ICALL:
            if( ins->operands[ CALL_OP_ADDR ]->n.class == N_REGISTER ) {
                GenRCall( ins );
            } else {
                GenICall( ins );
            }
            break;
        case G_AXR:  /* FPR RR result right*/
        case G_ADR:
        case G_AER:
        case G_AWR:
        case G_AUR:
        case G_DDR:
        case G_DER:
        case G_MXR:
        case G_MDR:
        case G_MXDR:
        case G_MER:
        case G_SXR:
        case G_SDR:
        case G_SER:
        case G_SWR:
        case G_SUR:
            GenRR( hwins, result, right );
            break;
        case G_AE: /* FPR RX SHORT result right */
        case G_AU:
        case G_DE:
        case G_ME:
        case G_SE:
        case G_SU:
            GenRFX( hwins, result, right, FS );
            break;
        case G_AD: /* FPR RX LONG result right */
        case G_AW:
        case G_DD:
        case G_MD:
        case G_MXD:
        case G_SD:
        case G_SW:
            GenRFX( hwins, result, right, FD );
            break;
        case G_CDR: /* FPR RR left right */
        case G_CER: /* FPR RR left right */
            GenRR( hwins, left, right );
            break;
        case G_CE : /* FPR RX SHORT left right */
            GenRFX( hwins, left, right, FS );
            break;
        case G_CD : /* FPR RX LONG left right */
            GenRFX( hwins, left, right, FD );
            break;
        case G_LDR : /* FPR RR result  left */
        case G_LTDR:
        case G_LCDR:
        case G_LNDR:
        case G_LPDR:
        case G_LER :
        case G_LCER:
        case G_LNER:
        case G_LPER:
        case G_HDR:
        case G_HER:
            GenRR( hwins, result, left );
            break;
        case G_LE  : /* FPR RX SHORT  result left */
            GenRFX( hwins, result, left, FS );
            break;
        case G_LD  : /* FPR RX LONG result left */
            GenRFX( hwins, result, left, FD );
            break;
        case G_STE : /* FPR RX SHORT left result */
            GenRFX( hwins, left, result, FS );
            break;
        case G_STD : /* FPR RX LONG left result */
            GenRFX( hwins, left, result, FD );
            break;
        case G_LTEZ:
            GenRR( HWOP_LTER, left, left );
            break;
        case G_LTDZ:
            GenRR( HWOP_LTDR, left, left );
            break;
        case G_BCTR:
            GenRR( hwins, result, AllocRegName( HW_G0 ) );
            break;
        case G_MVCL:
            GenMVCL( hwins, result->i.index, left->i.index );
            break;
        case G_DIVPOW2:
            GenPow2Div( result, right );
            break;
        case G_TM:
            GenSI( hwins, left, right );
            break;
        case G_SELECT:
            GenSelect( left );
            break;
        case G_MEMDOWN:
            GenMemDown( result, left );
            break;
        case G_NORMF:
            GenNormF( result, left );
            break;
        default:
            _Zoiks( ZOIKS_028 );
        }
    }
    if( _OpIsCondition( ins->head.opcode ) ) {
        GenCondJump( ins );
    }
}


static  void    GenSRL( name *result, int by ) {
/**********************************************/

    hwins_op_any hwop1;
    hwins_op_any hwop2;

    hwop1.r = RegOp( result );
    hwop2.sx.a = 0;
    hwop2.sx.b = 0;
    hwop2.sx.ref  = NULL;
    hwop2.sx.disp = by;
    HWInsGen( HWOP_SRL, &hwop1, &hwop2, NULL );
}


static  bool    ZapsOperand( name *result, name *left ) {
/******************************************************/

    if( left->n.class != N_INDEXED ) return( FALSE );
    if( left->i.index == result ) return( TRUE );
    if( left->i.base == result ) return( TRUE );
    return( FALSE );
}


static  void    GenUConv1( name *result, name *left ) {
/*****************************************************/

    if( ZapsOperand( result, left ) ) {
        GenRmS( HWOP_ICM, result, 0x8, left );
        GenSRL( result, 24 );
    } else {
        GenRR( HWOP_SR, result, result );
        GenRX( HWOP_IC, result, left );
    }
}


static  void    GenUConv2( name *result, name *left ) {
/*****************************************************/

    if( ZapsOperand( result, left ) ) {
        GenRmS( HWOP_ICM, result, 0xc, left );
        GenSRL( result, 16 );
    } else {
        GenRR( HWOP_SR, result, result );
        GenRmS( HWOP_ICM, result, 0x3, left );
    }
}

static  void    GenMemDown( name *op1,  name *op2 ) {
/**Do a mem to mem convert of big int op2 to small int op1******/
    hwins_op_any hwop1;
    hwins_op_any hwop2;
    int adjust;

    GetOpName( &hwop1, op1 );
    GetOpName( &hwop2, op2 );
    hwop1.sx.a = op1->n.size;
    adjust = op2->n.size - op1->n.size;
    hwop2.sx.disp += adjust; /* JD says it will never be over 4K */
    HWInsGen( HWOP_MVC, &hwop1, &hwop2, NULL );
}

static  void    GenSelect( name *targ ) {
/***************************************/

    reg_num      reg;
    label_handle label;
    hw_sym       *hwlabel;
    name         *base;

    reg = RegOp( targ->i.index );
    base = targ->i.base;
    label = AskForSymLabel( base->v.symbol, base->m.memory_type );
    hwlabel = AskForHWLabel( label );
    HWBIndexGen( reg, hwlabel );
}


static  void    GenCLRR( type_class_def class, name *result ) {
/*****************************************************/

    hwins_opcode        op;

    op = HWOP_SR;
    if( class == FD ) {
        op = HWOP_SDR;
    } else if( class == FS ) {
        op = HWOP_SER;
    }
    GenRR( op, result, result );
}


static void GenPow2Div( name *res, name *cons ) {
/***********************************************/

    hwins_op_any hwop1;
    hwins_op_any hwop2;

    hwop2.r = RegOp( res );
    hwop1.r = hwop2.r + 1;
    HWInsGen( HWOP_AR, &hwop1, &hwop2, NULL );
    hwop2.sx.b = 0;
    hwop2.sx.ref  = NULL;
    hwop2.sx.a = 0;
    hwop2.sx.disp = Log2( cons->c.int_value );
    HWInsGen( HWOP_SRA, &hwop1, &hwop2, NULL );
    hwop2.r = hwop1.r - 1;
    HWInsGen( HWOP_SR, &hwop1, &hwop2, NULL );
}


static void GenMVCL( hwins_opcode hwins, name *op1, name *op2 ) {
/*************************************************************/

    hwins_op_any hwop1;
    hwins_op_any hwop2;

    hwop1.r = RegOp( op1 ) - 1;
    hwop2.r = RegOp( op2 ) - 1;
    HWInsGen( hwins, &hwop1, &hwop2, NULL );
}


static void GenRR( hwins_opcode hwins, name *op1, name *op2 ) {
/*************************************************************/

    hwins_op_any hwop1;
    hwins_op_any hwop2;

    hwop1.r = RegOp( op1 );
    hwop2.r = RegOp( op2 );
    HWInsGen( hwins, &hwop1, &hwop2, NULL );
}


static void GenRDisp( hwins_opcode hwins, name *op1, int b, int disp ) {
/*******************************************************/

  /* RX instruction where op2 + op3 form base disp */
    hwins_op_any hwop1;
    hwins_op_any hwop2;

    hwop1.r = RegOp( op1 );
    hwop2.sx.ref = NULL;
    hwop2.sx.disp = disp;
    hwop2.sx.b = b;
    hwop2.sx.a = 0;
    HWInsGen( hwins, &hwop1, &hwop2, NULL );
}


static void GenRX( hwins_opcode hwins, name *op1, name *op2 ) {
/*************************************************************/

    hwins_op_any hwop1;
    hwins_op_any hwop2;

    hwop1.r = RegOp( op1 );
    GetOpName( &hwop2, op2 );
    HWInsGen( hwins, &hwop1, &hwop2, NULL );
}


static void GenRXH( hwins_opcode hwins, name *op1, name *op2 ) {
/****watch out for half word lits******************************/

    hwins_op_any hwop1;
    hwins_op_any hwop2;

    hwop1.r = RegOp( op1 );
    ChkLitOp( &hwop2, op2, I2 );
    HWInsGen( hwins, &hwop1, &hwop2, NULL );
}

static void GenRFX( hwins_opcode hwins, name *op1, name *op2,
                                        type_class_def tipe ) {
/*** Check for short or long float literal in X op2**********************/

    hwins_op_any hwop1;
    hwins_op_any hwop2;

    hwop1.r = RegOp( op1 );
    ChkLitOp( &hwop2, op2, tipe );
    HWInsGen( hwins, &hwop1, &hwop2, NULL );
}

static  void    GenNormF( name *op1,  name *op2 ) {
/**********************************************************************
 Normalize result from CONV of op2 in mem to op1 reg
 if integer subtract 0x80000000  to set sign and compliment is negative
*/
    hwins_op_any hwop1;
    hwins_op_any hwop2;

    hwop1.r = RegOp( op1 );
    HWInsGen( HWOP_SDR, &hwop1, &hwop1, NULL );
    GetOpName( &hwop2, op2 );
    HWInsGen( HWOP_AD, &hwop1, &hwop2, NULL );
}



static void GenShift( hwins_opcode hwins, name *op1, name *op2 ) {
/****************************************************************/

  /* generate r,mask,s type instruction */
    hwins_op_any hwop1;
    hwins_op_any hwop2;
    hwins_op_any hwop3;

    hwop1.r = RegOp( op1 );
    hwop2.sx.a = 0;
    hwop2.sx.b = 0;
    hwop2.sx.ref  = NULL;
    if( op2->n.class == N_CONSTANT ) {
        hwop2.sx.disp = op2->c.int_value;
    } else {
        hwop2.sx.b = RegOp( op2 );
        hwop2.sx.disp = 0;
    }
    HWInsGen( hwins, &hwop1, &hwop2, &hwop3 );
}


static void GenRmS( hwins_opcode hwins, name *op1, int mask, name *op2 ) {
/*******************************************************/

  /* generate r,mask,s type instruction */
    hwins_op_any hwop1;
    hwins_op_any hwop2;
    hwins_op_any hwop3;

    hwop1.r = RegOp( op1 );
    hwop3.i = mask;
    GetOpName( &hwop2, op2 );
    HWInsGen( hwins, &hwop1, &hwop2, &hwop3 );
}


static void GenSS( hwins_opcode hwins, name *op1, name *op2,
                                       type_class_def tipe ){
/*************************************************************/

    hwins_op_any hwop1;
    hwins_op_any hwop2;

    ChkLitOp( &hwop1, op1, tipe );
    hwop1.sx.a = op1->n.size;
    ChkLitOp( &hwop2, op2, tipe );
    HWInsGen( hwins, &hwop1, &hwop2, NULL );
}


static void GenSI( hwins_opcode hwins, name *op1, name *op2 ) {
/*************************************************************/

    hwins_op_any hwop1;
    hwins_op_any hwop2;

    GetOpName( &hwop1, op1 );
    hwop2.i = op2->c.int_value;
    HWInsGen( hwins, &hwop1, &hwop2, NULL );
}


static void GetConsLit( hwins_op_any *hwop, name *op ){
/*****************************************************/

    name *op2;

    switch( op->c.const_type ) {
    case CONS_ABSOLUTE:
        hwop->sx.b  = 0xff; /* set to a bad value for now */
        hwop->sx.ref = HWLitIntGen( op->c.int_value, 4 );
        break;
    case CONS_ADDRESS:
        op2 = op->c.value;
        GetAddrLit( hwop, op2 );
        break;
    case CONS_OFFSET:
        Zoiks( ZOIKS_060 );
        break;
    case CONS_SEGMENT:
        Zoiks( ZOIKS_060 );
        break;
    case CONS_TEMP_ADDR:
        Zoiks( ZOIKS_060 );
        break;
    }

}

static void GetAddrLit( hwins_op_any *hwop, name *op ){
/*****************************************************/
    label_handle  label;
    hw_sym       *hwlabel;
    bool          rel;

    label = AskForSymLabel( op->v.symbol, op->m.memory_type );
    hwlabel = AskForHWLabel( label );
    if( op->m.memory_type == CG_FE ) {
        if( FEAttr( op->v.symbol ) & FE_IMPORT ){
            HWExtern( hwlabel );
        }
    }else if( AskIfRTLabel( label ) ) {
        HWExtern( hwlabel );
    }
    hwop->sx.a  = 0;
    hwop->sx.b  = 0;
    hwop->sx.disp = 0;
    rel = MemNeedsReloc( op->v.symbol, op->m.memory_type );
    hwop->sx.ref =   HWLitAddr( hwlabel, op->v.offset, rel );
}

extern bead_def *GetanAddr( label_handle label, bool reloc ){
/**Make an addr bead for label used by dbginfo stuff****/

    sym_handle  sym;
    hw_sym      *hwlabel;
    bead_addr   *bead;

    hwlabel = AskForHWLabel( label );
    sym = AskForLblSym( label );
    if( sym != NULL ) {
        if( FEAttr( sym ) & FE_IMPORT ){
            HWExtern( hwlabel );
        }

    }
    bead =   HWSymAddr( hwlabel, 0, reloc );
    return( (bead_def *)bead );
}


static void GetMemRef( hwins_op_any *hwop, name *op ) {
/*****************************************************/

    label_handle label;
    hw_sym      *hwlabel;

    label = AskForSymLabel( op->v.symbol, op->m.memory_type );
    hwlabel =  AskForHWLabel( label );
    hwop->sx.b  = 0;
    hwop->sx.ref = HWSymRef( hwlabel );
    hwop->sx.disp = op->v.offset;
}

static void GetMemDisp( hwins_op_any *hwop, name *op ) {
/*****************************************************/

    label_handle label;
    hw_sym      *hwlabel;
    hw_loc      *hwbase;
    name        *base;

    base = op->i.base;
    label = AskForSymLabel( base->v.symbol, base->m.memory_type );
    hwlabel =  AskForHWLabel( label );
    hwbase  =  AskForBaseHW( label );
    if( hwlabel->defflag & TXT_DATA ){ /* defined in another file */
        hwop->sx.disp = op->i.constant;
    }else{
        hwop->sx.disp  = base->v.offset - hwbase->disp;
        hwop->sx.ref = HWDispRef( hwlabel, hwbase->sym );
    }
}

static  reg_num TempBase( name *temp ) {
/**************************************/

    if( temp->t.temp_flags & STACK_PARM ) return( CurrProc->state.regs.PA );
    return( CurrProc->state.regs.AR );
}

static void ChkLitOp( hwins_op_any *hwop, name *op, type_class_def tipe ) {
/**** check for int & flt lits with regard to type or just load op********/
    type_length tlen;

    if( op->n.class == N_CONSTANT
     && op->c.const_type == CONS_ABSOLUTE ){
        hwop->sx.a = 0;
        hwop->sx.disp = 0;
        hwop->sx.b  = 0xff; /* not known yet */
        tlen = TypeClassSize[ tipe ];
        if( _IsFloating( tipe )  ){
            hwop->sx.ref = HWLitFltGen( op->c.value, tlen );
        }else{
            hwop->sx.ref = HWLitIntGen( op->c.int_value, tlen );
        }
    }else{
        GetOpName( hwop, op );
    }
}

extern void GetOpName( hwins_op_any *hwop, name *op ) {
/**************************************/

    hw_reg_set  regs;
    name        *base;

    hwop->sx.a = 0;
    hwop->sx.b = 0;
    hwop->sx.disp = 0;
    hwop->sx.ref  = NULL;
    switch( op->n.class ) {
    case N_CONSTANT: /* ref a lit constant */
        GetConsLit( hwop, op );
        break;
    case N_MEMORY:
        /* ref memory label */;
        GetMemRef( hwop, op );
        break;
    case N_TEMP:
        base = DeAlias( op );
        if( base->t.location == NO_LOCATION ) {
            _Zoiks( ZOIKS_030 );
        }
        hwop->sx.disp = base->t.location + op->v.offset - base->v.offset;
        hwop->sx.b = TempBase( base );
        hwop->sx.a = 0;
        break;
    case N_INDEXED:
        regs = op->i.index->r.reg;
        if( op->i.index_flags ==( X_FAKE_BASE | X_BASE_IS_INDEX) ){
            GetMemDisp( hwop, op );
        }else{
            hwop->sx.disp  = op->i.constant;              /* use integer value*/
        }
        GetIdx( hwop, regs, op->i.index_flags );
        if( HasTrueBase( op ) ) {
            op = op->i.base;
            if( op->n.class != N_TEMP ) _Zoiks( ZOIKS_030 );
            base = DeAlias( op );
            if( hwop->sx.b == 0 ) {
                hwop->sx.b = TempBase( base );
            } else if( hwop->sx.a == 0 ) {
                hwop->sx.a = TempBase( base );
            } else {
                _Zoiks( ZOIKS_030 );
            }
        }
        break;
    case N_REGISTER:
        hwop->r = RegOp( op );
        break;
    }
}

extern unsigned DepthAlign( unsigned depth ) {
/********************************************/
    depth = 1;
    return( 2 );
}

extern  hw_sym  *CodeLabel( label_handle label, unsigned align ) {
/****************************************************************/
 align = 2;
 return(  EncLabel( label, align ) );
}

extern  hw_sym  *EncLabel( label_handle label, char align ) {
/****************************************************************/

    sym_handle  sym;
    fe_attr     attr;
    hw_sym    *hwlabel;

    hwlabel = AskForHWLabel( label );
    sym = AskForLblSym( label );
    if( AskIfRTLabel( label ) ) {
        attr = FE_STATIC;
    } else if( sym != NULL ){
        attr = FEAttr( sym );
    }else{
        attr = FE_STATIC;
    }
    HWLabelGen( hwlabel, align );
    if( attr &  FE_GLOBAL ){
        HWEntry( hwlabel  );
    }

#ifndef NXDEBUG
    if( _IsModel( INTERNAL_DBG_OUTPUT ) ) { /* debug dump */
        if( sym != NULL ) {
            DumpString( FEName( sym ) );
        } else if( AskIfRTLabel( label ) ) {
            DumpString( AskRTName( (int)AskForLblSym( label ) ) );
        } else {
            DumpString( "L" );
            DumpPtr( label );
        }
        DumpString( ":" );
        DumpNL();
    }
#endif
    return( hwlabel );
}

extern  void    CodeLineNum( unsigned_16 line ) {
/**********************************************/

    HWQueue( line );

#ifndef NXDEBUG
    if( _IsModel( INTERNAL_DBG_OUTPUT ) ) { /* debug dump */
        DumpString( "Source Line: " );
        DumpInt( line );
        DumpNL();
    }
#endif
}


extern  void    GenJumpLabel( label_handle label ) {
/**********************************************/

    hw_sym   *hwlabel;
    hwins_op_any  dst;

    hwlabel =  AskForHWLabel( label );
    dst.sx.ref = HWSymRef( hwlabel );
    dst.sx.disp = 0;
    dst.sx.a = 0;
    dst.sx.b = 0;
    HWBRGen( 15,  &dst );

#ifndef NXDEBUG
    if( _IsModel( INTERNAL_DBG_OUTPUT ) ) { /* debug dump */
        DumpString( "JMP L" );
        DumpPtr( label );
        DumpNL();
    }
#endif
}


static void    GenJumpIf( instruction *cond, label_handle label ) {
/*************************************************************/

    hw_sym   *hwlabel;
    hwins_op_any  dst;

    hwlabel =  AskForHWLabel( label );
    dst.sx.ref = HWSymRef( hwlabel );
    dst.sx.disp = 0;
    dst.sx.a = 0;
    dst.sx.b = 0;
    HWBRGen( CondCode( cond ),  &dst );
#ifndef NXDEBUG
    if( _IsModel( INTERNAL_DBG_OUTPUT ) ) { /* debug dump */
        DumpString( "Jcc L" );
        DumpPtr( label );
        DumpNL();
   }
#endif
}


extern  void    GenKillLabel( pointer label ) {
/**********************************************/
    label = label;      /* NYI */
}


extern  void    GenCallLabel( pointer label ) {
/**********************************************/
    label = label;      /* NYI */
}


extern  void    GenLabelReturn() {
/**********************************************/
        /* NYI */
}


static  byte    CondCode( instruction *cond ) {
/*********************************************/

    if(  Signed[  cond->type_class  ] ) {
        return( SCondTable[ cond->head.opcode-FIRST_CONDITION ] );
    } else {
        return( UCondTable[ cond->head.opcode-FIRST_CONDITION ] );
    }
}


extern  label_handle    LocateLabel( instruction *ins, int index ) {
/******************************************************************/

    if( index == NO_JUMP ) return( NULL );
    for(;;) {
        ins = ins->head.next;
        if( ins->head.opcode == OP_BLOCK ) break;
    }
    return( _BLOCK( ins )->edge[  index  ].destination );
}


extern  void    GenCondJump( instruction *cond ) {
/************************************************/

    label_handle        dest_false;
    label_handle        dest_true;
    label_handle        dest_next;
    instruction         *next;

    next = cond->head.next;
    while( next->head.opcode != OP_BLOCK ) {
        next = next->head.next;
    }
    dest_next = NULL;
    if( _BLOCK( next )->next_block != NULL ) {
        dest_next = _BLOCK( next )->next_block->label;
    }
    dest_false = LocateLabel( cond, _FalseIndex( cond ) );
    dest_true = LocateLabel( cond, _TrueIndex( cond ) );
    if( dest_true == dest_next ) {
        FlipCond( cond );
        dest_true = dest_false;
        dest_false = dest_next;
    }
    if( dest_false != dest_true && dest_true != dest_next ) {
        GenJumpIf( cond, dest_true );
    }
    if( dest_false != dest_next ) {
        GenJumpLabel( dest_false );
    }
}


extern void GetRALN( name *op, char *ra, char *ln ) {
/***************************************************/

    pointer             aux;
    linkage_regs        *link;
    label_handle        label;

    *ra = 13;
    *ln = 11;
    if( op != NULL ) {
        if( op->m.memory_type == CG_FE ) {
            aux = FEAuxInfo( op->v.symbol, AUX_LOOKUP );
            link = FEAuxInfo( aux, LINKAGE_REGS );
            *ra = RegTrans( link->ra );
            *ln = RegTrans( link->ln );
        } else if( op->m.memory_type == CG_LBL ) {
            label = (label_handle)op->v.symbol;
            if( AskIfRTLabel( label ) ) {
                AskRTRALN( (int)AskForLblSym( label ), ra, ln );
            }
        }
    }
}


extern  void    GenCall( instruction *ins ) {
/***************************************/

    name                *op;
    hwins_op_any hwop1;
    hwins_op_any hwop2;

    op = ins->operands[ CALL_OP_ADDR ];
    GetRALN( op, &hwop1.r, &hwop2.r );
    HWInsGen( HWOP_BALR, &hwop1, &hwop2, NULL );
}


extern  void    GenRCall( instruction *ins ) {
/********************************************/

    hwins_op_any hwop1;
    hwins_op_any hwop2;
    hwins_op_any hwop3;

    GetRALN( NULL, &hwop1.r, &hwop2.r );
    hwop3.r = RegOp( ins->operands[ CALL_OP_ADDR ] );
    HWInsGen( HWOP_LR, &hwop2, &hwop3, NULL );
    HWInsGen( HWOP_BALR, &hwop1, &hwop2, NULL );
}


extern  void    GenICall( instruction *ins ) {
/********************************************/

    hwins_op_any hwop1;
    hwins_op_any hwop2;
    hwins_op_any hwop3;

    GetRALN( NULL, &hwop1.r, &hwop2.r );
    GetOpName( &hwop3, ins->operands[ CALL_OP_ADDR ] );
    HWInsGen( HWOP_L, &hwop2, &hwop3, NULL );
    HWInsGen( HWOP_BALR, &hwop1, &hwop2, NULL );
}


static reg_num RegOp( name *r ) {
/***********************************/

    return ( RegTrans( r->r.reg ) );
}

static hw_reg_set RegNames[] = {
    HW_D( HW_G0 ),
    HW_D( HW_G1 ),
    HW_D( HW_G2 ),
    HW_D( HW_G3 ),
    HW_D( HW_G4 ),
    HW_D( HW_G5 ),
    HW_D( HW_G6 ),
    HW_D( HW_G7 ),
    HW_D( HW_G8 ),
    HW_D( HW_G9 ),
    HW_D( HW_G10 ),
    HW_D( HW_G11 ),
    HW_D( HW_G12 ),
    HW_D( HW_G13 ),
    HW_D( HW_G14 ),
    HW_D( HW_G15 ),
    HW_D( HW_EMPTY )
};

static reg_num GetRegNum( hw_reg_set regs ) {
/****************************************/

    reg_num i;

    i = 0;
    while( !HW_CEqual( RegNames[i], HW_EMPTY ) ) {
        if( HW_Ovlap( regs, RegNames[i] ) ) break;
        i++;
    }
    return( i );
}


static void GetIdx( hwins_op_any *hwop, hw_reg_set regs, i_flags flag ) {
/***********************************************************************/

    reg_num high;
    reg_num low;

    low = GetRegNum( regs );
    HW_TurnOff( regs, RegNames[ low ] );
    if( !HW_CEqual( regs, HW_EMPTY ) ) {
        high = GetRegNum( regs );
    } else {
        high = 0;
    }
    if( flag & X_HIGH_BASE ) {
        hwop->sx.b = high;
        hwop->sx.a = low;
    } else {
        hwop->sx.b = low;
        hwop->sx.a = high;
    }
}


extern reg_num RegTrans( hw_reg_set regs ) {
/*******************************************/

/* convert bit set into bit position which we'll use as the regname */

    HW_CTurnOff( regs, HW_UNUSED );
    if( HW_CEqual( regs, HW_EMPTY ) ) {
        _Zoiks( ZOIKS_031 );
    } else if( HW_COvlap( regs, HW_FLTS ) ) {
        if( HW_COvlap( regs, HW_Y0 ) ) return( 0 );
        if( HW_COvlap( regs, HW_Y2 ) ) return( 2 );
        if( HW_COvlap( regs, HW_Y4 ) ) return( 4 );
        if( HW_COvlap( regs, HW_Y6 ) ) return( 6 );
    } else {
        return( GetRegNum( regs ) );
    }
    _Zoiks( ZOIKS_117 );  /* if we get here bug */
    return( 0 );
}
