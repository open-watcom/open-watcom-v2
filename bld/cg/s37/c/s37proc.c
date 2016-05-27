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


#include "cgstd.h"
#include "coderep.h"
#include "opcodes.h"
#include "procdef.h"
#include "model.h"
#include "rtclass.h"
#include "offset.h"
#include "s37bead.h"
#include "zoiks.h"

#include "s37proc.def"

#define _4K     (4*1024)
#define DWORD_SIZE (2*WORD_SIZE)

extern  sym_handle      AskForLblSym(label_handle);
extern  void            RelocParms();
extern  hw_sym          *CodeLabel(label_handle,unsigned);
extern  unsigned        DepthAlign(unsigned);
extern  bead_drop       *HWDrop(char);
extern  bead_using      *HWUsing(hw_sym *,char);
extern  void            CodeLineNum(unsigned_16);
extern  seg_id          SetOP(seg_id);
extern  seg_id          AskCodeSeg();
extern  hw_reg_set      SaveRegs();
extern  hw_reg_set      StackReg();
extern  int             RegTrans(hw_reg_set);
extern  bool            AdjustFarLocals(type_length);
extern  void            AdjustNearLocals(type_length);
extern  ref_any         *HWLitIntGen(offset,int);
extern  hw_reg_set      FixedRegs();
extern  hw_reg_set      LNReg();
extern  void            HWStartProc(void );
extern  void            HWEpilogue(void );
extern  void            HWEndProc(void );
extern  void            EmitRtnEnd();
extern  memory_name     *SAllocMemory(pointer,type_length,cg_class,type_class_def,type_length);
extern  name            *AllocAddrConst(name*,int,constant_class,type_class_def);
extern  label_handle    AskForNewLabel(void);
extern  seg_id          AskBackSeg(void);
extern  void            DataLabel(label_handle);
extern  void            DGUBytes(unsigned_32);
extern  void            GetOpName(hwins_op_any*,name*);
extern  label_handle    RTLabel(int);
extern  bool            FEStackChk(sym_handle);
extern  void            GetRALN(name*,char*,char*);
extern  type_length     FarLocalSize();

extern void HWInsGen( hwins_opcode hwop, hwins_op_any *op1,
                                         hwins_op_any *op2,
                                         hwins_op_any *op3);
/* debug */
extern  void            DumpString(char *);
extern  void            DumpNL();

extern  block           *HeadBlock;
extern  proc_def        *CurrProc;
extern  bool            BlockByBlock;
extern  type_length     MaxStack;

typedef struct {
        hw_reg_set      reg;
        int             i;
} float_save;

float_save      FloatSave[] = {
/*****************************/

        HW_D( HW_D0 ),  0,
        HW_D( HW_D2 ),  2,
        HW_D( HW_D4 ),  4,
        HW_D( HW_D6 ),  6,
        HW_D( HW_EMPTY ),  0
};

static hw_reg_set RegNames[] = {
/******************************/

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
    HW_D( HW_G15 )
};

extern  void    GenProlog() {
/***************************/

    seg_id      old;

    old = SetOP( AskCodeSeg() );
    if( _IsModel( NUMBERS ) ) {
        CodeLineNum( HeadBlock->ins.hd.line_num );
    }
    if( _IsModel( INTERNAL_DBG_OUTPUT ) ) {
        DumpNL();
        DumpString( "===================================" );
        DumpNL();
    }
    HWStartProc();
    CurrProc->targ.using_label = CodeLabel( CurrProc->label, DepthAlign( 1 ) );
    CurrProc->parms.base = 0;
    CalcUsedRegs();
    MakeSaveArea();
    RelocParms();
    CurrProc->prolog_state |= GENERATED_PROLOG;
    SetOP( old );
}

extern  void    CalcUsedRegs() {
/******************************/


    block       *blk;
    instruction *ins;
    name        *result;
    hw_reg_set  used;

    HW_CAsgn( used, HW_EMPTY );
    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            result = ins->result;
            if( result != NULL && result->n.class == N_REGISTER ) {
                HW_TurnOn( used, result->r.reg );
            }
            /* place holder for big label doesn't really zap anything*/
            if( ins->head.opcode != OP_NOP ) {
                HW_TurnOn( used, ins->zap->reg );
            }
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
    HW_TurnOn( CurrProc->state.used, used );
}


static  void    RefInt( hwins_op_any *hwop, offset i ) {
/******************************************************/

    hwop->sx.a = 0;
    hwop->sx.b = CurrProc->state.regs.LN;
    hwop->sx.ref = HWLitIntGen( i, WORD_SIZE );
    hwop->sx.disp = 0;
}


static long     DWord( long x ) {
/*******************************/

    return( ( x + ( DWORD_SIZE - 1 ) ) & ~( DWORD_SIZE - 1 ) );
}


static name *GetLabelAddress( hwins_op_any *phwop, label_handle lbl )
/******************************************************************/
{
    name                *mem;
    name                *cons;

    mem = SAllocMemory( lbl, 0, CG_LBL, WD, WORD_SIZE );
    cons = AllocAddrConst( mem, 0, CONS_ADDRESS, WD );
    GetOpName( phwop, cons );
    return( mem );
}


static void MakeStack( hwins_op_any *phwop )
/*****************************************/
{
    seg_id              old;
    label_handle        lbl;

    old = SetOP( AskBackSeg() );
    lbl = AskForNewLabel();
    DataLabel( lbl );
    DGUBytes( 72*WORD_SIZE );
    SetOP( old );
    GetLabelAddress( phwop, lbl );
}


#define SA_BACK_LINK    4
#define SA_FWD_LINK     8
#define SA_FIRST_REG    12
#define MAX_SA_WORDS    18

static int STMPos( int reg, int first_reg )
/*****************************************/
{
    int         position;

    position = reg - first_reg;
    if( position > 15 ) position -= 16;
    if( position < 0 ) position += 16;
    return( position );
}


static int RegPosOS( int reg, int first_reg )
/******************************************/
{
    return( SA_FIRST_REG + WORD_SIZE * STMPos( reg, first_reg ) );
}


static int RegPosCLink( int reg, int first_reg )
/**********************************************/
{
    return( WORD_SIZE * STMPos( reg, first_reg ) );
}


static void GetOSSTMOps( hwins_op_any *first_reg,
                         hwins_op_any *last_reg, hwins_op_any *sa_op )
/*****************************************************************/
{
    sa_op->r = CurrProc->state.regs.SA;
    first_reg->r = CurrProc->state.regs.SA + 1;
    if( first_reg->r > 15 ) first_reg->r -= 16;
    last_reg->r = CurrProc->state.regs.SA + 15;
    if( last_reg->r > 15 ) last_reg->r -= 16;
}


static void RegOffset( hwins_op_any *hwop, int offset, char reg )
/***************************************************************/
{
    hwop->sx.a = 0;
    hwop->sx.b = reg;
    hwop->sx.disp = offset;
    hwop->sx.ref  = NULL;
}


static void BranchAndLinkTo( int idx ) {
/**************************************/

    hwins_op_any        hwop;
    hwins_op_any        hwop2;
    name                *mem;
    char                ra,ln;

/*
    L           LN,=A(routine[idx])             - call routine to save/restore
    BALR        RA,LN                             registers 10 and 12
*/
    mem = GetLabelAddress( &hwop, RTLabel( idx-BEG_RTNS ) );
    GetRALN( mem, &ra, &ln );
    hwop2.r = ln;
    HWInsGen( HWOP_L, &hwop2, &hwop, NULL );
    hwop.r = ra;
    HWInsGen( HWOP_BALR, &hwop, &hwop2, NULL );
}


static bool NeedStackCheck()
/**************************/
{
    return( FEStackChk( AskForLblSym( CurrProc->label ) ) );
}


static bool AddTo( hwins_op_any *reg_op, long amt )
/**************************************************/
{
    hwins_op_any        hwop;

    if( amt == 0 ) return( false );
    if( amt < _4K ) {
        RegOffset( &hwop, amt, reg_op->r );
        HWInsGen( HWOP_LA, reg_op, &hwop, NULL );
    } else {
        RefInt( &hwop, amt );
        HWInsGen( HWOP_A, reg_op, &hwop, NULL );
    }
    return( true );
}


static bool SkipOverParms()
/************************/
{
    hwins_op_any        sp_op;

    if( CurrProc->state.attr & ROUTINE_ALTERNATE_AR ||
        CurrProc->targ.save_area >= _4K-(16*WORD_SIZE)-(4*DWORD_SIZE) ||
        NeedStackCheck() ) {

        sp_op.r = CurrProc->state.regs.SP;
        /*
            A        SP,PARM_SIZE       - bump SP past parms
        */
        CurrProc->targ.save_area -= CurrProc->parms.size;
        return( AddTo( &sp_op, CurrProc->parms.size ) );
    }
    return( false );
}


static void StackCheck( int idx )
/*******************************/
{
    hwins_op_any        hwop;
    hwins_op_any        hwop2;

    RefInt( &hwop2,
              DWord( CurrProc->parms.size
                   + CurrProc->locals.size
                   + FarLocalSize() )
            + DWord( MAX_SA_WORDS*WORD_SIZE + 4*(DWORD_SIZE) )
            + DWord( MaxStack ) );
    hwop.r = CurrProc->state.regs.AR;
    HWInsGen( HWOP_L, &hwop, &hwop2, NULL );
    BranchAndLinkTo( idx );
}


static void OSSaveRegs( hwins_op_any *savearea )
/**********************************************/
{
    hwins_op_any        sp_op;
    hwins_op_any        sa_op;
    hwins_op_any        hwop;
    hwins_op_any        hwop2;

    sp_op.r = CurrProc->state.regs.SP;
    GetOSSTMOps( &hwop, &hwop2, &sa_op );
    RegOffset( savearea, SA_FIRST_REG, sa_op.r );
    /*
        STM     Rx,Ry,SA_FIRST_REG(SA)          - save all regs
    */
    HWInsGen( HWOP_STM, &hwop, savearea, &hwop2 );
    SwitchBase();
    if( CurrProc->state.attr & ROUTINE_ENTRY ) {
        MakeStack( &hwop );
        /*
            L   SP,=A(STACKAREA)
        */
        HWInsGen( HWOP_L, &sp_op, &hwop, NULL );
    } else {
        if( CurrProc->state.attr & ROUTINE_FUNC ) {
            BranchAndLinkTo( RT_OSPROL );
        }
        if( NeedStackCheck() ) {
            StackCheck( RT_OSSTKCHK );
        }
    }
    RegOffset( savearea, SA_FWD_LINK, sa_op.r );
    /*
        ST   SP,SA_FWD_LINK(SA)         - link the save areas
    */
    HWInsGen( HWOP_ST, &sp_op, savearea, NULL );
    RegOffset( savearea, SA_BACK_LINK, sp_op.r );
    /*
        ST   SA,SA_BACK_LINK(SP)                - link the save areas
    */
    HWInsGen( HWOP_ST, &sa_op, savearea, NULL );
    /*
        LR   SA,SP                      - load up a new save area reg
    */
    HWInsGen( HWOP_LR, &sa_op, &sp_op, NULL );
    savearea->sx.disp = CurrProc->targ.save_area + MAX_SA_WORDS*WORD_SIZE;
    if( CurrProc->state.parm.offset != 0 ) {
        hwop.r = CurrProc->state.regs.PA;
        hwop2.r = CurrProc->state.regs.PR;
        /*
            LR  PA,PR                   - save a permanent pointer to parms
        */
        HWInsGen( HWOP_LR, &hwop, &hwop2, NULL );
    }
}


static void CLinkSaveRegs( hwins_op_any *savearea )
/*************************************************/
{
    hw_reg_set          regs;
    hw_reg_set          tmp;
    int                 first_save_reg;
    int                 last_save_reg;
    hwins_op_any        hwop;
    hwins_op_any        hwop2;

    regs = SaveRegs();
    tmp = FixedRegs();
    HW_TurnOff( regs, tmp );
    tmp = LNReg();
    HW_TurnOff( regs, tmp );
    HW_CTurnOff( regs, HW_FLTS );

    first_save_reg = CurrProc->state.regs.SP;
    if( CurrProc->state.regs.AR < first_save_reg ) {
        first_save_reg = CurrProc->state.regs.AR;
    }
    if( CurrProc->state.regs.RA < first_save_reg ) {
        first_save_reg = CurrProc->state.regs.RA;
    }
    if( CurrProc->state.regs.BR < first_save_reg ) {
        first_save_reg = CurrProc->state.regs.BR;
    }
    if( CurrProc->state.regs.PA < first_save_reg ) {
        first_save_reg = CurrProc->state.regs.PA;
    }
    if( CurrProc->state.regs.SA < first_save_reg ) {
        first_save_reg = CurrProc->state.regs.SA;
    }
    last_save_reg = first_save_reg - 1;
    if( last_save_reg < 0 ) last_save_reg = 15;
    while( last_save_reg >= 0 ) {
        if( HW_Ovlap( regs, RegNames[last_save_reg] ) ) break;
        last_save_reg--;
    }
    if( last_save_reg < 0 ) last_save_reg = 15;
    CurrProc->targ.first_save_reg = first_save_reg;
    CurrProc->targ.last_save_reg  = last_save_reg;
    hwop.r = CurrProc->targ.first_save_reg;
    RegOffset( savearea, CurrProc->targ.save_area, CurrProc->state.regs.SP );
    hwop2.r = CurrProc->targ.last_save_reg;
    /*
        STM     SP,**,SAVE_OFFSET(SP)           - save all used regs
    */
    HWInsGen( HWOP_STM, &hwop, savearea, &hwop2 );
    SwitchBase();
    if( NeedStackCheck() ) {
        StackCheck( RT_STKCHK );
    }
    savearea->sx.disp += WORD_SIZE*( ( ( last_save_reg + 1 ) & 0xF ) )
                     +  WORD_SIZE*( 16 - first_save_reg );
}


static void SaveFPRs( hwins_op_any * savearea )
/*********************************************/
{
    float_save          *flt_save;
    hw_reg_set          regs;
    hwins_op_any        fp_reg;

    regs = SaveRegs();
    HW_COnlyOn( regs, HW_FLTS );
    flt_save = FloatSave;
    while( !HW_CEqual( regs, HW_EMPTY ) ) {
        if( HW_Ovlap( regs, flt_save->reg ) ) {
            fp_reg.r = flt_save->i;
            if( HW_Subset( regs, flt_save->reg ) ) {
                HWInsGen( HWOP_STD, &fp_reg, savearea, NULL );
                savearea->sx.disp += DWORD_SIZE;
            } else {
                HWInsGen( HWOP_STE, &fp_reg, savearea, NULL );
                savearea->sx.disp += WORD_SIZE;
            }
            HW_TurnOff( regs, flt_save->reg );
        }
        ++flt_save;
    }
}


static void RestoreFPRs()
/***********************/
{
    float_save          *flt_save;
    hw_reg_set          regs;
    hwins_op_any        fp_reg;
    hwins_op_any        savearea;

    RegOffset( &savearea,
               CurrProc->targ.save_area + CurrProc->targ.flt_save_area,
               CurrProc->state.regs.AR );
    regs = SaveRegs();
    HW_COnlyOn( regs, HW_FLTS );
    flt_save = FloatSave;
    while( !HW_CEqual( flt_save->reg, HW_EMPTY ) ) {
        ++flt_save;
    }
    while( !HW_CEqual( regs, HW_EMPTY ) ) {
        --flt_save;
        if( HW_Ovlap( regs, flt_save->reg ) ) {
            fp_reg.r = flt_save->i;
            if( HW_Subset( regs, flt_save->reg ) ) {
                savearea.sx.disp -= DWORD_SIZE;
                HWInsGen( HWOP_LD, &fp_reg, &savearea, NULL );
            } else {
                savearea.sx.disp -= WORD_SIZE;
                HWInsGen( HWOP_LE, &fp_reg, &savearea, NULL );
            }
            HW_TurnOff( regs, flt_save->reg );
        }
    }
}


static void SetUpAR( hwins_op_any *savearea, bool sp_bumped )
/***********************************************************/
{
    hwins_op_any        ar_op;
    hwins_op_any        sp_op;
    hwins_op_any        hwop;
    long                savesize;

    ar_op.r = CurrProc->state.regs.AR;
    sp_op.r = CurrProc->state.regs.SP;
    savesize = savearea->sx.disp - CurrProc->targ.save_area;
    AdjustNearLocals( savesize );
    AdjustFarLocals( savearea->sx.disp+CurrProc->locals.size );
    /*
        LR      AR,SP                   - set up pointer to local vars
    */
    HWInsGen( HWOP_LR, &ar_op, &sp_op, NULL );
    if( sp_bumped ) {
        RefInt( &hwop, CurrProc->parms.size );
        /*
            S   SP,CurrProc->parms.size - bump SP back down
        */
        HWInsGen( HWOP_S, &sp_op, &hwop, NULL );
        RegOffset( &hwop,
                   ( RegPosCLink( sp_op.r, CurrProc->targ.first_save_reg ) +
                     CurrProc->targ.save_area ),
                   ar_op.r );
        /*
            ST  SP,SP_OFFSET(AR)        - save the orignal SP value
        */
        HWInsGen( HWOP_ST, &sp_op, &hwop, NULL );
        if( !( CurrProc->state.attr & ROUTINE_ALTERNATE_AR ) ) {
            /*
                LR      AR,SP                   - set up pointer to local vars
            */
            HWInsGen( HWOP_LR, &ar_op, &sp_op, NULL );
            CurrProc->targ.save_area += CurrProc->parms.size;
        }
    }
    AddTo( &sp_op, DWord( CurrProc->parms.size + CurrProc->locals.size
                        + FarLocalSize() + savesize ) );
}


static void SwitchBase()
/**********************/
{
    hwins_op_any        ln_op;
    hwins_op_any        br_op;

    ln_op.r = CurrProc->state.regs.LN;
    br_op.r = CurrProc->state.regs.BR;
    /*
        LR      BR,LN                   - set up a new base register
    */
    HWInsGen( HWOP_LR, &br_op, &ln_op, NULL ); /* set BR */
    HWDrop( ln_op.r );
    HWUsing( CurrProc->targ.using_label, br_op.r );
}


static void MakeSaveArea() {
/**********************/

    hwins_op_any        savearea;
    bool                sp_bumped;


    HWUsing( CurrProc->targ.using_label, CurrProc->state.regs.LN );
    CurrProc->targ.save_area = CurrProc->parms.size;
    if( CurrProc->state.attr & ROUTINE_OS ) {
        OSSaveRegs( &savearea );
    } else {
        sp_bumped = SkipOverParms();
        CLinkSaveRegs( &savearea );
    }
    SaveFPRs( &savearea );
    CurrProc->targ.flt_save_area = savearea.sx.disp - CurrProc->targ.save_area;
    SetUpAR( &savearea, sp_bumped );
}


static void OSRestoreRegs()
/*************************/
{
    hwins_op_any        first_reg;
    hwins_op_any        last_reg;
    hwins_op_any        sa_op;
    hwins_op_any        ln_op;
    hwins_op_any        savearea;

    GetOSSTMOps( &first_reg, &last_reg, &sa_op );
    RegOffset( &savearea, SA_BACK_LINK, sa_op.r );
    /*
        L       SA,SA_BACK_LINK(SA)             - restore ptr to original save area
    */
    HWInsGen( HWOP_L, &sa_op, &savearea, NULL );
    ln_op.r = CurrProc->state.regs.LN;
    RegOffset( &savearea, RegPosOS( ln_op.r, first_reg.r ), sa_op.r );
    /*
        ST      LN,LN_OFFSET(SA)        - save LN in original save area
    */
    HWInsGen( HWOP_ST, &ln_op, &savearea, NULL );
    if( CurrProc->state.attr & ROUTINE_FUNC ) {
        BranchAndLinkTo( RT_OSEPIL );
    }
    HWEpilogue();
    HWDrop( CurrProc->state.regs.BR );
    RegOffset( &savearea, SA_FIRST_REG, CurrProc->state.regs.SA );
    /*
        LM      Rx,Ry,SA_FIRST_REG(SA)          - restore all regs (except LN)
    */
    HWInsGen( HWOP_LM, &first_reg, &savearea, &last_reg );
}


static void CLinkRestoreRegs()
/***************************/
{
    hwins_op_any        first_reg;
    hwins_op_any        last_reg;
    hwins_op_any        savearea;

    first_reg.r = CurrProc->targ.first_save_reg;
    last_reg.r = CurrProc->targ.last_save_reg;
    RegOffset( &savearea, CurrProc->targ.save_area, CurrProc->state.regs.AR );
    HWEpilogue();
    HWDrop( CurrProc->state.regs.BR );
    /*
        LM      SP,xxx,0(AR)            - restore all regs
    */
    HWInsGen( HWOP_LM, &first_reg, &savearea, &last_reg );
}


static void DoReturn()
/********************/
{
    hwins_op_any        hwop1;
    hwins_op_any        hwop2;

    hwop1.r = 15;/* always */
    hwop2.r = CurrProc->state.regs.RA;
    /*
        BR      RA                      - return to caller
    */
    HWInsGen( HWOP_BCR, &hwop1, &hwop2, NULL );
}


extern  void    GenEpilog() {
/***************************/
    seg_id      old;

    old = SetOP( AskCodeSeg() );
    RestoreFPRs();
    if( CurrProc->state.attr & ROUTINE_OS ) {
        OSRestoreRegs();
    } else {
        CLinkRestoreRegs();
    }
    DoReturn();
    HWEndProc();
    CurrProc->prolog_state |= GENERATED_EPILOG;
    if( CurrProc->state.attr & ROUTINE_WANTS_DEBUGGING ) {
        EmitRtnEnd();
    }
    SetOP( old );
}


extern  int     AskDisplaySize( int level ) {
/*******************************************/

    level=level;
    return( 0 );
}


extern  type_length     PushSize( type_length len ) {
/*******************************************/

    if( len < WORD_SIZE ) return( WORD_SIZE );
    return( ( len + (WORD_SIZE-1) ) & ~(WORD_SIZE-1) );
}
