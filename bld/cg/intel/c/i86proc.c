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
* Description:  Intel x86 procedure call generation.
*
****************************************************************************/


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "opcodes.h"
#include "procdef.h"
#include "model.h"
#include "rtclass.h"
#include "objrep.h"
#include "ocentry.h"
#include "gen8087.h"
#include "cgaux.h"
#include "stackok.h"
#include "zoiks.h"
#include "feprotos.h"
#include "addrname.h"
#include "display.h"
#include "rtrtn.h"

extern  void        OutDLLExport(uint,sym_handle);
extern  void        GenLeaSP(long);
extern  void        Gcld( void );
extern  void        GenReturn(int,bool,bool);
extern  void        GenLeave( void );
extern  void        GenWindowsProlog( void );
extern  void        GenCypWindowsProlog( void );
extern  void        GenWindowsEpilog( void );
extern  void        GenCypWindowsEpilog( void );
extern  void        GenRdosdevProlog( void );
extern  void        GenRdosdevEpilog( void );
extern  void        EmitRtnEnd( void );
extern  void        EmitEpiBeg( void );
extern  void        GenEnter(int,int);
extern  void        GenUnkEnter(pointer,int);
extern  void        GenRegAnd(hw_reg_set,type_length);
extern  void        GenRegSub(hw_reg_set,type_length);
extern  void        GenUnkSub(hw_reg_set,pointer);
extern  void        AdjustPushLocal(name*);
extern  bool        DoesSomething(instruction*);
extern  void        GenRegAdd(hw_reg_set,type_length);
extern  void        GenRegMove(hw_reg_set,hw_reg_set);
extern  void        GenPushOffset(byte);
extern  void        EmitProEnd( void );
extern  void        DbgRetOffset(type_length);
extern  void        RelocParms( void );
extern  type_length AdjustBase( void );
extern  hw_reg_set  SaveRegs( void );
extern  void        DoCall(label_handle,bool,bool,oc_class);
extern  void        GenUnkPush(pointer);
extern  void        GenPushC(signed_32);
extern  void        GenUnkMov(hw_reg_set,pointer);
extern  void        QuickSave(hw_reg_set,opcode_defs);
extern  void        CodeLabel(label_handle,unsigned);
extern  void        EmitRtnBeg( void );
extern  void        CodeLineNum( cg_linenum,bool);
extern  seg_id      SetOP(seg_id);
extern  seg_id      AskCodeSeg( void );
extern  void        Gpusha( void );
extern  void        Gpopa( void );
extern  void        AbsPatch(abspatch_handle,offset);
extern  unsigned    DepthAlign( unsigned );
extern  char        *CopyStr(char*,char*);
extern  void        EyeCatchBytes(byte*,byte_seq_len);
extern  void        GenSelEntry(bool);
extern  void        TellKeepLabel(label_handle);
extern  void        GenKillLabel(label_handle);
extern  void        GFstpM(pointer);
extern  void        GenTouchStack( bool );
extern  void        GenLoadDS(void);
extern  label_handle    GenFar16Thunk( label_handle, unsigned_16, bool );
extern  void        GenP5ProfilingProlog( label_handle );
extern  void        GenP5ProfilingEpilog( label_handle );
extern  bool        SymIsExported( sym_handle );
extern  void        FlowSave( hw_reg_set * );
extern  void        FlowRestore( hw_reg_set * );

/* forward declarations */
static  void        MoveParms( void );
static  int         PushAll( void );
static  void        CalcUsedRegs( void );
static  void        Enter( void );
static  void        AllocStack( void );
static  int         Push( hw_reg_set to_push );
static  void        DoEnter( int level );
static  void        DoEpilog( void );

extern  block       *HeadBlock;
extern  proc_def    *CurrProc;
extern  bool        BlockByBlock;
extern  type_length MaxStack;
extern  hw_reg_set  GivenRegisters;
extern  byte        OptForSize;
extern  pointer     Parm8087[ MAX_8087_REG+1 ];

#define WINDOWS_CHEAP  ( ( _IsModel( DLL_RESIDENT_CODE ) &&         \
               ( CurrProc->state.attr & ROUTINE_LOADS_DS ) )        \
            || ( _IsTargetModel( CHEAP_WINDOWS )                    \
               && !( CurrProc->prolog_state &                       \
                 ( GENERATE_EXPORT | GENERATE_FAT_PROLOG ) ) ) )

#define DO_WINDOWS_CRAP ( _IsTargetModel( WINDOWS )                 \
               && ( !WINDOWS_CHEAP || CurrProc->contains_call ) )

#define DO_BP_CHAIN ( ( (_IsTargetModel( NEED_STACK_FRAME ) || _IsModel( DBG_CV ) ) \
               && CurrProc->contains_call )                                         \
             || ( CurrProc->prolog_state & GENERATE_FAT_PROLOG ) )

#define CHAIN_FRAME ( DO_WINDOWS_CRAP || DO_BP_CHAIN )

#define CHEAP_FRAME ( _IsTargetModel( NEED_STACK_FRAME ) || \
              _IsntTargetModel( WINDOWS ) || WINDOWS_CHEAP )

#define FAR_RET_ON_STACK ( (_RoutineIsLong( CurrProc->state.attr ) ) \
             && !(CurrProc->state.attr & ROUTINE_NEVER_RETURNS))

type_length StackDepth;

hw_reg_set   PushRegs[] = {
#if _TARGET & _TARG_80386
#define ALL_REG_SIZE 12*WORD_SIZE
#define HW_STACK_CHECK HW_EAX
    HW_D( HW_EAX ),
    HW_D( HW_EBX ),
    HW_D( HW_ECX ),
    HW_D( HW_EDX ),
    HW_D( HW_ESI ),
    HW_D( HW_EDI ),
    HW_D( HW_DS ),
    HW_D( HW_ES ),
    HW_D( HW_FS ),
    HW_D( HW_GS ),
    HW_D( HW_SS ),
#else
#define ALL_REG_SIZE 8*WORD_SIZE+4*2
#define HW_STACK_CHECK HW_AX
    HW_D( HW_AX ),
    HW_D( HW_BX ),
    HW_D( HW_CX ),
    HW_D( HW_DX ),
    HW_D( HW_SI ),
    HW_D( HW_DI ),
    HW_D( HW_DS ),
    HW_D( HW_ES ),
    HW_D( HW_FS ),
    HW_D( HW_GS ),
    HW_D( HW_SS ),
#endif
    HW_D( HW_BP ),
    HW_D( HW_EMPTY )
};

extern  bool    CanZapBP( void ) {
/**************************/

    return( !CHAIN_FRAME );
}


static  bool    ScanInstructions( void )
/**********************************/
{
    block       *blk;
    instruction     *ins;
    name        *addr;
    bool        sp_constant;

    CurrProc->contains_call = TRUE;
    if( BlockByBlock )
        return( FALSE );
    CurrProc->contains_call = FALSE;
    blk = HeadBlock;
    sp_constant = TRUE;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            if( _OpIsCall( ins->head.opcode ) ) {
                if( ins->head.opcode == OP_CALL_INDIRECT ) {
                    CurrProc->contains_call = TRUE;
                } else {
                    addr = ins->operands[ CALL_OP_ADDR ];
                    if( addr->n.class != N_MEMORY ||
                     addr->m.memory_type != CG_LBL ||
                     !AskIfRTLabel( addr->v.symbol ) ) {
                        CurrProc->contains_call = TRUE;
                    }
                }
                if( HW_COvlap( ins->zap->reg, HW_SP ) ) {
                    CurrProc->state.attr |= ROUTINE_NEEDS_PROLOG;
                    sp_constant = FALSE;
                }
            }
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
    return( sp_constant );
}


#if _TARGET & _TARG_80386
static  void    ChkFDOp( name *op, int depth ) {
/**********************************************/

    if( op->n.class != N_TEMP )
        return;
    if( !( op->v.usage & (USE_IN_ANOTHER_BLOCK|USE_ADDRESS) ) )
        return;
    if( op->t.temp_flags & STACK_PARM )
        return;
    if( op->t.location != NO_LOCATION ) {
        if( op->v.usage & (USE_ADDRESS|HAS_MEMORY) )
            return;
        if( op->t.alias != op )
            return;
        if( depth == 0 )
            return;
        op->t.location = NO_LOCATION;
    }
    op->t.temp_flags |= USED_AS_FD;
    CurrProc->targ.has_fd_temps = TRUE;
}
#endif


#if _TARGET & _TARG_80386
static  void    ScanForFDOps( void )
/****************************/
{
    block       *blk;
    instruction     *ins;
    int         i;
    int         depth;

    CurrProc->contains_call = FALSE;
    if( BlockByBlock )
        return;
    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        depth = blk->depth;
        while( ins->head.opcode != OP_BLOCK ) {
            if( ins->type_class == FD || ins->type_class == FL ) {
                i = ins->num_operands;
                while( --i >= 0 ) {
                    ChkFDOp( ins->operands[i], depth );
                }
                if( ins->result != NULL ) {
                    ChkFDOp( ins->result, depth );
                }
            }
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
}
#endif


#if _TARGET & _TARG_80386
static  block   *ScanForLabelReturn( block *blk ) {
/*********************************************/

    block   *son;
    int     i;

    if( blk->class & (RETURN|CALL_LABEL) )
        return( NULL );
    blk->edge[0].flags |= DOWN_ONE_CALL;
    if( blk->class & LABEL_RETURN )
        return( blk );
    for( i = 0; i < blk->targets; ++i ) {
        son = blk->edge[i].destination;
        if( son->edge[0].flags & DOWN_ONE_CALL )
            continue;
        if( SafeRecurse( (void *(*)(void *))ScanForLabelReturn, son ) == NULL )
            return( NULL );
    }
    return( blk );
}
#endif


#if _TARGET & _TARG_80386
static  bool    ScanLabelCalls( void ) {
/*********************************

    Make sure that all blocks that are called are only called
    one level deep. Mark output edges of all such blocks as
    DOWN_ONE_CALL. This is so we can adjust our x[esp] offsets
    accordingly.
*/

    block   *blk;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( !( blk->class & CALL_LABEL ) )
            continue;
        if( ScanForLabelReturn( blk->edge[0].destination ) == NULL ) {
            return( FALSE );
        }
    }
    return( TRUE );
}
#endif


extern  void    AddCacheRegs( void ) {
/******************************/

    #if _TARGET & _TARG_80386
    if( CurrProc->targ.never_sp_frame )
        return;
    if( _IsntModel( MEMORY_LOW_FAILS ) )
        return;
    if( OptForSize > 50 )
        return;
    if( _IsTargetModel( FLOATING_DS | FLOATING_SS ) )
        return;
    if( !ScanInstructions() )
        return;
    if( !CanZapBP() )
        return;
    if( !ScanLabelCalls() )
        return;
    if( CurrProc->state.attr & ROUTINE_WANTS_DEBUGGING )
        return;
    if( CurrProc->lex_level != 0 )
        return;
    if( _FPULevel( FPU_586 ) ) {
        ScanForFDOps();
    }
    if( CurrProc->targ.has_fd_temps ) {
        CurrProc->targ.sp_frame = TRUE;
        CurrProc->targ.sp_align = TRUE;
    } else if( !DO_BP_CHAIN && _IsntTargetModel( WINDOWS ) &&
            !_RoutineIsInterrupt( CurrProc->state.attr ) ) {
        /*
         * We cannot make EBP available under Windows because the SS
         * selector might not cover the data segment and so we cannot use
         * it as an index. Puke - BBB Feb 18, 1994
         */
        CurrProc->targ.sp_frame = TRUE;
        HW_CTurnOff( CurrProc->state.unalterable, HW_BP );
    }
    #endif
}


static  void    AdjustPushLocals( void ) {
/**********************************/

    instruction *ins;

    ins = HeadBlock->ins.hd.next;
    for(;;) {
        if( DoesSomething( ins ) )
            break;
        if( ins->head.opcode == OP_MOV
         && ins->head.state == OPERANDS_NEED_WORK ) {
            QuickSave( ins->operands[ 0 ]->r.reg, OP_PUSH );
            AdjustPushLocal( ins->result );
        }
        ins = ins->head.next;
    }
}

static  bool    NeedBPProlog( void ) {
/******************************/

    if( CurrProc->parms.size != 0 )
        return( TRUE );
    if( CurrProc->locals.size != 0 )
        return( TRUE );
    if( CurrProc->targ.push_local_size != 0 )
        return( TRUE );
    if( CurrProc->lex_level != 0 )
        return( TRUE );
    if( CurrProc->targ.sp_align )
        return( TRUE );
    if( BlockByBlock != 0 )
        return( TRUE );
    if( ( CurrProc->state.attr & ROUTINE_NEEDS_PROLOG ) != 0 )
        return( TRUE );
    if( FAR_RET_ON_STACK ) {
        if( CHAIN_FRAME )
            return( TRUE );
    }
    return( FALSE );
}


static void FindIfExported( void ) {
/****************************/

    sym_handle  sym;

    sym = AskForLblSym( CurrProc->label );
    if( sym == NULL )
        return;
    if( SymIsExported( sym ) ) {
        CurrProc->prolog_state |= GENERATE_EXPORT;
    }
}


extern void RTCall( rt_class rtn, oc_class pop_bit ) {
/****************************************************/

    DoCall( RTLabel( rtn ), TRUE, _IsTargetModel( BIG_CODE ), pop_bit );
}


static  bool    NeedStackCheck( void )
/******************************/
{
    return( FEStackChk( AskForLblSym( CurrProc->label ) ) );
}


static void DoStackCheck( void ) {
/**************************/

    if( CurrProc->prolog_state & ( GENERATE_THUNK_PROLOG | GENERATE_RDOSDEV_PROLOG ) )
        return;
#if _TARGET & _TARG_80386
    if( CurrProc->prolog_state & GENERATE_GROW_STACK ) {
        if( BlockByBlock || CurrProc->locals.size >= 4*1024 ) {
            GenUnkPush( &CurrProc->targ.stack_check );
            RTCall( RT_GROW, ATTR_POP );
        }
        return;
    }
#endif
    if( NeedStackCheck() ) {
#if _TARGET & _TARG_80386
        GenUnkPush( &CurrProc->targ.stack_check );
        RTCall( RT_CHK, ATTR_POP );
#else
        if( HW_COvlap( CurrProc->state.parm.used, HW_AX ) ) {
            QuickSave( HW_STACK_CHECK, OP_PUSH );
        }
        GenUnkMov( HW_STACK_CHECK, &CurrProc->targ.stack_check );
        RTCall( RT_CHK, EMPTY );
        if( HW_COvlap( CurrProc->state.parm.used, HW_AX ) ) {
            QuickSave( HW_STACK_CHECK, OP_POP );
        }
#endif
    }
}


static  void    EmitNameInCode( void ) {
/********************************/

    sym_handle      sym;
    char            *name;
    char            *endname;
    char            b[128];
    label_handle    lbl;

    sym = AskForLblSym( CurrProc->label );
    if( sym == NULL )
        return;
    name = FEName( sym );
    endname = CopyStr( name, b );
    name = b;
    *endname = endname - name;
    lbl = AskForNewLabel();
    TellKeepLabel( lbl );
    CodeLabel( lbl, 0 );
    GenKillLabel( lbl );
    EyeCatchBytes( (byte *)name, *(unsigned char *)endname + 1 );
}


static  int ProEpiDataSize( void )
/***************************
*/
{
    return( _RoundUp( (pointer_int)FEAuxInfo( NULL, PROEPI_DATA_SIZE ), WORD_SIZE ) );
}


static  void    PrologHook( void )
/***************************
*/
{
    int      size;

    if( !( CurrProc->prolog_state & GENERATE_PROLOG_HOOKS ) )
        return;
    size = ProEpiDataSize();
    if( size != 0 ) {
        GenRegSub( HW_SP, size );
        CurrProc->targ.base_adjust += size;
    }
#if _TARGET & _TARG_80386
//    GenPushC( CurrProc->parms.size );
    RTCall( RT_PROHOOK, EMPTY );
#else
    RTCall( RT_PROHOOK, EMPTY );
#endif
}


static  void    EpilogHook( void )
/***************************
*/
{
    int      size;

    if( ( CurrProc->prolog_state & GENERATE_EPILOG_HOOKS ) ) {
        RTCall( RT_EPIHOOK, EMPTY );
    }
    size = ProEpiDataSize();
    if( size != 0 )
        GenRegAdd( HW_SP, size );
}


static  void    DoLoadDS( void )
{
#if _TARGET & _TARG_80386
    if( _IsntTargetModel( LOAD_DS_DIRECTLY ) ) {
        RTCall( RT_GETDS, EMPTY );
    } else
#endif
    {
        if( HW_COvlap( CurrProc->state.parm.used, HW_AX ) ) {
            QuickSave( HW_STACK_CHECK, OP_PUSH );
        }
        GenLoadDS();
        if( HW_COvlap( CurrProc->state.parm.used, HW_AX ) ) {
            QuickSave( HW_STACK_CHECK, OP_POP );
        }
    }
}


static  int LoadDS( void )
/**********************/
{
    int     size;

    size = 0;
    if( CurrProc->state.attr & ROUTINE_LOADS_DS ) {
        if( HW_COvlap( CurrProc->state.unalterable, HW_DS ) ) {
            QuickSave( HW_DS, OP_PUSH );
            size = WORD_SIZE;
            DoLoadDS();
        }
    }
    return( size );
}


static  void    UnloadDS( void ) {
/**************************/

    if( CurrProc->state.attr & ROUTINE_LOADS_DS ) {
        if( HW_COvlap( CurrProc->state.unalterable, HW_DS ) ) {
            QuickSave( HW_DS, OP_POP );
        }
    }
}

extern  void    GenProlog( void ) {
/***************************/

    seg_id  old;
    hw_reg_set  to_push;
    unsigned    ret_size;
    pointer label;
    pointer origlabel; // Original label for generated __far16 thunks
    fe_attr attr;

    ScanInstructions();       /* Do These 2 calls before using DO_WINDOWS_CRAP! */
    FindIfExported();
    old = SetOP( AskCodeSeg() );

    if( CurrProc->prolog_state & GENERATE_FUNCTION_NAME ) {
        EmitNameInCode();
    }

    if( _IsModel( NUMBERS ) ) {
        CodeLineNum( HeadBlock->ins.hd.line_num, FALSE );
    }

    if( _IsModel( DBG_LOCALS ) ){  // d1+ or d2
        EmitRtnBeg();
    }
    if( CurrProc->state.attr & ROUTINE_WANTS_DEBUGGING ) {
        CurrProc->state.attr |= ROUTINE_NEEDS_PROLOG;
    }

    CurrProc->parms.base = 0;
    CurrProc->parms.size = CurrProc->state.parm.offset;

    origlabel = label = CurrProc->label;

#if _TARGET & _TARG_80386
    if( _RoutineIsFar16( CurrProc->state.attr ) ) {
        label = GenFar16Thunk( CurrProc->label, CurrProc->parms.size,
                    CurrProc->state.attr & ROUTINE_REMOVES_PARMS );
        // CurrProc->label = label; - ugly mess if following are combined
    }
#endif

    CodeLabel( label, DepthAlign( PROC_ALIGN ) );

    attr = FEAttr( AskForLblSym( origlabel ) );

    if( CurrProc->prolog_state & GENERATE_RDOSDEV_PROLOG ) {
        GenRdosdevProlog();
    }

#if _TARGET & _TARG_80386
    if( ( attr & FE_NAKED ) == EMPTY ) {
        if( _IsTargetModel( NEW_P5_PROFILING|P5_PROFILING ) ) {
            GenP5ProfilingProlog( label );
        }
        if( CurrProc->prolog_state & GENERATE_THUNK_PROLOG ) {
            QuickSave( HW_SP, OP_PUSH );
            GenPushC( CurrProc->parms.size );
            GenUnkPush( &CurrProc->targ.stack_check );
        if( NeedStackCheck() ) {
            RTCall( RT_THUNK_STK, ATTR_POP );
        } else {
            RTCall( RT_THUNK, ATTR_POP );
        }
        CurrProc->parms.base += WORD_SIZE;
        }
    }
#endif

    if( _RoutineIsInterrupt( CurrProc->state.attr ) ||
        ( CurrProc->state.attr & ROUTINE_NEVER_RETURNS ) ) {
       ret_size = 0;
    } else if( _RoutineIsLong( CurrProc->state.attr ) ) {
        ret_size = 2 * WORD_SIZE;
    } else if( _RoutineIsFar16( CurrProc->state.attr ) ) {
        ret_size = 2 * WORD_SIZE;
    } else {
        ret_size = WORD_SIZE;
    }

    CurrProc->parms.base += ret_size;
    CalcUsedRegs();

    to_push = SaveRegs();
    HW_CTurnOff( to_push, HW_FLTS );
    if( !CurrProc->targ.sp_frame || CurrProc->targ.sp_align ) {
        HW_CTurnOff( to_push, HW_BP );
    }

    if( ( attr & FE_NAKED ) != EMPTY ) {
        // don't do anything - empty prologue
    } else if( _RoutineIsInterrupt( CurrProc->state.attr ) ) {
        ret_size = -PushAll();
        CurrProc->targ.base_adjust = 0;
        MoveParms();
    } else {
        if( CHAIN_FRAME ) {
            CurrProc->targ.base_adjust = 0;
            if( NeedBPProlog() ) {
                HW_CTurnOn( CurrProc->state.used, HW_BP );
                CurrProc->parms.base += WORD_SIZE;
                if( FAR_RET_ON_STACK ) {
                    if( CHEAP_FRAME ) {
                        GenCypWindowsProlog();
                    } else {
#if _TARGET & _TARG_IAPX86
                        // Windows prologs zap AX, so warn idiot user if we
                        // generate one for a routine in which AX is live
                        // upon entry to routine, or unalterable.
                        if( HW_COvlap( CurrProc->state.unalterable, HW_AX ) ||
                            HW_COvlap( CurrProc->state.parm.used, HW_AX ) ) {
                            FEMessage( MSG_ERROR,
                            "exported routine with AX live on entry" );
                        }
#endif
                        GenWindowsProlog();
                        CurrProc->targ.base_adjust += 2; /* the extra push DS */
                    }
                } else {
                    QuickSave( HW_BP, OP_PUSH );
                    GenRegMove( HW_SP, HW_BP );
                }
                PrologHook();
            }
            DoStackCheck();
            CurrProc->targ.base_adjust += LoadDS();
            CurrProc->targ.base_adjust += Push( to_push );
            CurrProc->parms.base += CurrProc->targ.base_adjust;
            AllocStack();
            AdjustPushLocals();
        } else {
            DoStackCheck();
            CurrProc->parms.base += LoadDS();
            if( (CurrProc->state.attr & ROUTINE_NEVER_RETURNS) == 0 ) {
                CurrProc->parms.base += Push( to_push );
            }
            Enter();
            AdjustPushLocals();
            if( _IsModel( NO_OPTIMIZATION ) || CurrProc->targ.sp_frame ) {
                CurrProc->targ.base_adjust = 0;
            } else {
                CurrProc->targ.base_adjust = AdjustBase();
                if( CurrProc->targ.base_adjust != 0 ) {
                    GenRegSub( HW_BP, -CurrProc->targ.base_adjust );
                }
            }
        }
        RelocParms();
        MoveParms();
    }
    CurrProc->prolog_state |= GENERATED_PROLOG;

    if( _IsModel( DBG_LOCALS ) ){  // d1+ or d2
        DbgRetOffset( CurrProc->parms.base - CurrProc->targ.base_adjust - ret_size );
        EmitProEnd();
    }
    SetOP( old );

    if( CurrProc->prolog_state & GENERATE_EXPORT ) {
        OutDLLExport( ( CurrProc->parms.size+WORD_SIZE-1 ) / WORD_SIZE, AskForLblSym( CurrProc->label ) );
    }
}


static  void    MoveParms( void ) {
/***************************/

    int     i;

    i = 0;
    while( Parm8087[ i ] != NULL ) {
        GFstpM( Parm8087[ i ] );
        ++i;
    }
}


extern  void    InitStackDepth( block *blk ) {
/********************************************/

    if( blk->edge[0].flags & DOWN_ONE_CALL ) {
        StackDepth = WORD_SIZE;
    } else {
        StackDepth = 0;
    }
    StackDepth += blk->stack_depth;
}


extern  void        AdjustStackDepth( instruction *ins ) {
/************************************************************/

    name    *op;
    type_length adjust;

    if( !DoesSomething( ins ) )
        return;
    switch( ins->head.opcode ) {
    case OP_ADD:
    case OP_SUB:
        if( ins->operands[0] != ins->result )
            return;
        if( ins->result->n.class != N_REGISTER )
            return;
        if( !HW_CEqual( ins->result->r.reg, HW_SP ) )
            return;
        op = ins->operands[1];
        if( op->n.class != N_CONSTANT ) {
            _Zoiks( ZOIKS_077 );
            return;
        }
        adjust = op->c.int_value;
        if( ins->head.opcode == OP_SUB ) {
            StackDepth += adjust;
        } else {
            StackDepth -= adjust;
        }
        break;
    case OP_PUSH:
        StackDepth += WORD_SIZE;
        break;
    case OP_POP:
        StackDepth -= WORD_SIZE;
        break;
    case OP_CALL:
    case OP_CALL_INDIRECT:
        if( ins->flags.call_flags & CALL_POPS_PARMS ) {
            op = ins->operands[ CALL_OP_POPS ];
            if( op->n.class == N_CONSTANT ) {
                StackDepth -= op->c.int_value;
            }
        }
    default:
        break;
    }
}

extern void     AdjustStackDepthDirect( int adjust ) {
/************************************************************/
    StackDepth += adjust;
}


extern  bool    BaseIsSP( name *op ) {
/************************************/

    if( !CurrProc->targ.sp_frame ) return( FALSE );
        if( CurrProc->targ.sp_align && ( op->t.temp_flags & STACK_PARM ) ) {
            return( FALSE );
        }
    return( TRUE );
}


extern  type_length NewBase( name *op ) {
/*******************************************/

    if( !BaseIsSP( op ) ) {
        return( op->t.location - CurrProc->targ.base_adjust );
    }
    return( op->t.location + CurrProc->locals.size
        + CurrProc->targ.push_local_size + StackDepth );
}


static  int PushAll( void )
/*************************/
/* Save all registers and establish somewhat sane environment.
 * Used for __interrupt routines only.
 */
{
    if( _CPULevel( CPU_186 ) ) {
        Gpusha();
    } else {
        QuickSave( HW_AX, OP_PUSH );
        QuickSave( HW_CX, OP_PUSH );
        QuickSave( HW_DX, OP_PUSH );
        QuickSave( HW_BX, OP_PUSH );
        QuickSave( HW_SP, OP_PUSH );
        QuickSave( HW_BP, OP_PUSH );
        QuickSave( HW_SI, OP_PUSH );
        QuickSave( HW_DI, OP_PUSH );
    }
    QuickSave( HW_DS, OP_PUSH );
    QuickSave( HW_ES, OP_PUSH );
    if( _CPULevel( CPU_386 ) ) {
        QuickSave( HW_FS, OP_PUSH );
        QuickSave( HW_GS, OP_PUSH );
    } else {
        QuickSave( HW_AX, OP_PUSH );
        QuickSave( HW_AX, OP_PUSH );
    }
    GenRegMove( HW_SP, HW_BP );
    AllocStack();
    Gcld();
    if( HW_COvlap( CurrProc->state.unalterable, HW_DS ) ) {
        DoLoadDS();
        // If ES is also unalterable, copy DS to ES; else things
        // like memcpy() are likely to blow up
        if( HW_COvlap( CurrProc->state.unalterable, HW_ES ) ) {
            QuickSave( HW_DS, OP_PUSH );
            QuickSave( HW_ES, OP_POP );
        }
    }
    return( ALL_REG_SIZE );
}


static  void    PopAll( void ) {
/************************/

    if( CurrProc->locals.size != 0 ) {
        GenRegMove( HW_BP, HW_SP );
    }
    if( _CPULevel( CPU_386 ) ) {
        QuickSave( HW_GS, OP_POP );
        QuickSave( HW_FS, OP_POP );
    } else {
        QuickSave( HW_AX, OP_POP );
        QuickSave( HW_AX, OP_POP );
    }
    QuickSave( HW_ES, OP_POP );
    QuickSave( HW_DS, OP_POP );
    if( _CPULevel( CPU_186 ) ) {
        Gpopa();
    } else {
        QuickSave( HW_DI, OP_POP );
        QuickSave( HW_SI, OP_POP );
        QuickSave( HW_BP, OP_POP );
        QuickSave( HW_BX, OP_POP );
        QuickSave( HW_BX, OP_POP );
        QuickSave( HW_DX, OP_POP );
        QuickSave( HW_CX, OP_POP );
        QuickSave( HW_AX, OP_POP );
    }
}


static  void    Enter( void ) {
/***********************/

    int     lex_level;
    int     i;

    lex_level = CurrProc->lex_level;
    if( !CurrProc->targ.sp_frame && _CPULevel( CPU_186 ) &&
#if _TARGET & _TARG_80386
    CurrProc->locals.size <= 65535 &&
#endif
    ( lex_level != 0 || ( CurrProc->locals.size != 0 && OptForSize > 50 ) ) ) {
        DoEnter( lex_level );
        HW_CTurnOn( CurrProc->state.used, HW_BP );
        CurrProc->state.attr |= ROUTINE_NEEDS_PROLOG;
    } else {
        if( NeedBPProlog() ) {
            if( !CurrProc->targ.sp_frame || CurrProc->targ.sp_align ) {
                HW_CTurnOn( CurrProc->state.used, HW_BP );
                CurrProc->parms.base += WORD_SIZE;
                QuickSave( HW_BP, OP_PUSH );
                i = 0;
                while( --lex_level > 0 ) {
                    i -= 2;
                    GenPushOffset( i );
                }
                GenRegMove( HW_SP, HW_BP );
                if( CurrProc->lex_level > 1 ) {
                    GenRegAdd( HW_BP, ( CurrProc->lex_level - 1 ) * WORD_SIZE );
                }
                if( CurrProc->lex_level != 0 ) {
                    QuickSave( HW_BP, OP_PUSH );
                }
            }
            CurrProc->state.attr |= ROUTINE_NEEDS_PROLOG;
        }
        AllocStack();
    }
}


static  void    CalcUsedRegs( void ) {
/******************************/

    block   *blk;
    instruction *ins;
    name    *result;
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
            /* place holder for big label doesn't really zap anything */
            if( ins->head.opcode != OP_NOP ) {
                HW_TurnOn( used, ins->zap->reg );
                if( HW_COvlap( ins->zap->reg, HW_SP ) ) {
                    CurrProc->prolog_state |= GENERATE_RESET_SP;
                }
            }
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
    if( !CurrProc->targ.sp_frame || CurrProc->targ.sp_align ) {
        HW_CTurnOff( used, HW_BP );
    }
    HW_TurnOn( CurrProc->state.used, used );
}


static  int Push( hw_reg_set to_push ) {
/******************************************/

    hw_reg_set  *curr_push;
    int     size;

    size = 0;
    if( _IsntModel( NO_OPTIMIZATION ) && CurrProc->targ.sp_frame && !CurrProc->targ.sp_align ) {
        FlowSave( &to_push );
    }
    curr_push = PushRegs;
    while( !HW_CEqual( to_push, HW_EMPTY ) ) {
        if( HW_Ovlap( *curr_push, to_push ) ) {
            QuickSave( *curr_push, OP_PUSH );
            size += WORD_SIZE;
            HW_TurnOff( to_push, *curr_push );
        }
        ++ curr_push;
        if( HW_CEqual( *curr_push, HW_EMPTY ) )
            break;
    }
    return( size );
}

static  void        Pop( hw_reg_set to_pop ) {
/************************************************/
    hw_reg_set  *curr_pop;

    if( _IsntModel( NO_OPTIMIZATION ) && CurrProc->targ.sp_frame && !CurrProc->targ.sp_align ) {
        FlowRestore( &to_pop );
    }
    curr_pop = PushRegs;
    while( !HW_CEqual( *curr_pop, HW_EMPTY ) ) {
        ++ curr_pop;
    }
    while( !HW_CEqual( to_pop, HW_EMPTY ) ) {
        -- curr_pop;
        if( HW_Ovlap( *curr_pop, to_pop ) ) {
            QuickSave( *curr_pop, OP_POP );
            HW_TurnOff( to_pop, *curr_pop );
        }
    }
}

extern  type_length PushSize( type_length len ) {
/***************************************************/

    if( len < WORD_SIZE )
        return( WORD_SIZE );
    return( ( len + (WORD_SIZE-1) ) & ~(WORD_SIZE-1) );
}


static  void    AllocStack( void ) {
/****************************/

    type_length     size;

    /* keep stack aligned */
    size = ( CurrProc->locals.size + (WORD_SIZE-1) ) & ~(WORD_SIZE-1);
    CurrProc->locals.size = size;
    if( BlockByBlock ) {
        GenUnkSub( HW_SP, &CurrProc->targ.prolog_loc );
        if( CurrProc->prolog_state & GENERATE_TOUCH_STACK ) {
            GenTouchStack( TRUE );
        }
    } else if( size <= 2*WORD_SIZE && OptForSize > 50 ) {
        while( size > 0 ) {
            #if _TARGET & _TARG_80386
            QuickSave( HW_EAX, OP_PUSH );
            #else
            QuickSave( HW_AX, OP_PUSH );
            #endif
            size -= WORD_SIZE;
        }
    } else if( size != 0 ) {
        GenRegSub( HW_SP, size );
        if( CurrProc->prolog_state & GENERATE_TOUCH_STACK ) {
            GenTouchStack( FALSE );
        }
    }
    if( CurrProc->targ.sp_align ) {
        GenRegAnd( HW_SP, -( 2*WORD_SIZE ) );
        CurrProc->prolog_state |= GENERATE_RESET_SP;
    }
}


static  void    DoEnter( int level ) {
/************************************/

    type_length size;

    /* keep stack aligned */
    size = ( CurrProc->locals.size + (WORD_SIZE-1) ) & ~(WORD_SIZE-1);
    CurrProc->locals.size = size;

    CurrProc->parms.base += WORD_SIZE;
    if( BlockByBlock ) {
        GenUnkEnter( &CurrProc->targ.prolog_loc, level );
        if( CurrProc->prolog_state & GENERATE_TOUCH_STACK ) {
            GenTouchStack( TRUE );
        }
    } else {
        GenEnter( size, level );
        if( size != 0 && ( CurrProc->prolog_state & GENERATE_TOUCH_STACK ) ) {
            GenTouchStack( FALSE );
        }
    }
}


extern  void    GenEpilog( void ) {
/***************************/

    type_length stack;
    fe_attr attr;

    attr = FEAttr( AskForLblSym( CurrProc->label ) );

    stack = - CurrProc->locals.size - CurrProc->locals.base
         - CurrProc->targ.push_local_size;
    PatchBigLabels( stack );

    if( _IsModel( DBG_LOCALS ) ){  // d1+ or d2
        EmitEpiBeg();
    }

    if( ( attr & FE_NAKED ) == EMPTY ) {
        if( (CurrProc->state.attr & ROUTINE_NEVER_RETURNS) == 0 ) {
            DoEpilog();
        }

        if( BlockByBlock ) {
            AbsPatch( CurrProc->targ.prolog_loc, CurrProc->locals.size );
        }

        if( CurrProc->targ.stack_check != NULL ) {
            AbsPatch( CurrProc->targ.stack_check,
                  CurrProc->locals.size +
                  CurrProc->parms.base  +
                  WORD_SIZE*CurrProc->lex_level +
                  CurrProc->targ.push_local_size +
                  MaxStack );
        }
    }


    CurrProc->prolog_state |= GENERATED_EPILOG;
    if( _IsModel( DBG_LOCALS ) ){  // d1+ or d2
        EmitRtnEnd();
    }
}


static  void    DoEpilog( void ) {
/**************************/

    hw_reg_set  to_pop;
    bool    is_long;
    type_length size;

    if( _RoutineIsInterrupt( CurrProc->state.attr ) ) {
        PopAll();
    } else {
        to_pop = SaveRegs();
        HW_CTurnOff( to_pop, HW_FLTS );
        if( CHAIN_FRAME ) {
            if( ( CurrProc->state.attr & ROUTINE_NEEDS_PROLOG ) != 0
             || CurrProc->locals.size+CurrProc->targ.push_local_size != 0 ) {
                if( CurrProc->targ.base_adjust == 0 ) {
                    GenRegMove( HW_BP, HW_SP );
                } else {
                    GenLeaSP( -CurrProc->targ.base_adjust );
                }
            }
            HW_CTurnOff( to_pop, HW_BP );
        } else {
            if( CurrProc->state.attr & ROUTINE_NEEDS_PROLOG ) {
                size = CurrProc->locals.size + CurrProc->targ.push_local_size;
                if( (CurrProc->prolog_state & GENERATE_RESET_SP) || size!=0 ) {
                    /* sp is not pointing at saved registers already */
                    if( CurrProc->targ.sp_frame ) {
                        if( CurrProc->targ.sp_align ) {
                            GenRegMove( HW_BP, HW_SP );
                            QuickSave( HW_BP, OP_POP );
                        } else if( size != 0 ) {
                            GenRegAdd( HW_SP, size );
                        }
                    } else if( CurrProc->targ.base_adjust != 0 ) {
                        GenLeaSP( -CurrProc->targ.base_adjust );
                    } else if( _CPULevel( CPU_186 ) && (!_CPULevel( CPU_486 ) ||
                        OptForSize > 50) ) {
                        GenLeave();
                        HW_CTurnOff( to_pop, HW_BP );
                    } else {
                        GenRegMove( HW_BP, HW_SP );
                    }
                }
            }
        }
        Pop( to_pop );
        UnloadDS();
        if( CHAIN_FRAME ) {
            if( NeedBPProlog() ) {
                EpilogHook();
                if( FAR_RET_ON_STACK ) {
                    if( CHEAP_FRAME ) {
                      GenCypWindowsEpilog();
                    } else {
                        GenWindowsEpilog();
                    }
                } else {
                    QuickSave( HW_BP, OP_POP );
                }
            }
        }
        if( CurrProc->prolog_state & GENERATE_RDOSDEV_PROLOG ) {
            GenRdosdevEpilog();
        }
    }

    is_long = _RoutineIsLong( CurrProc->state.attr ) ||
        _RoutineIsFar16( CurrProc->state.attr );
    #if _TARGET & _TARG_80386
    if( CurrProc->prolog_state & GENERATE_THUNK_PROLOG ) {
        QuickSave( HW_SP, OP_POP );
    }
    if( _IsTargetModel( NEW_P5_PROFILING|P5_PROFILING ) ) {
        GenP5ProfilingEpilog( CurrProc->label );
    }
    #endif

    if( _RoutineIsInterrupt( CurrProc->state.attr ) ) {
        GenReturn( 0, FALSE, TRUE );
    } else if( CurrProc->state.attr & ROUTINE_REMOVES_PARMS ) {
        GenReturn( CurrProc->parms.size, is_long, FALSE );
    } else {
        GenReturn( 0, is_long, FALSE );
    }
}


extern  int AskDisplaySize( int level ) {
/*******************************************/

    return( level*WORD_SIZE );
}

extern  int ParmsAtPrologue( void ) {
/***************************/

    unsigned    ret_size;
    unsigned    parms_off_sp;

    parms_off_sp= 0;

    #if _TARGET & _TARG_80386
    if( CurrProc->prolog_state & GENERATE_THUNK_PROLOG ) {
        parms_off_sp += WORD_SIZE;
    }
    #endif

    if( _RoutineIsInterrupt( CurrProc->state.attr ) ||
       ( CurrProc->state.attr & ROUTINE_NEVER_RETURNS ) ) {
        ret_size = 0;
    } else if( _RoutineIsLong( CurrProc->state.attr ) ) {
        ret_size = 2 * WORD_SIZE;
    } else if( _RoutineIsFar16( CurrProc->state.attr ) ) {
        ret_size = 2 * WORD_SIZE;
    } else {
        ret_size = WORD_SIZE;
    }

    parms_off_sp += ret_size;
    return( parms_off_sp );
}
