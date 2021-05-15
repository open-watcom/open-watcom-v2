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
* Description:  32-bit instruction encodings.
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
#include "cgaux.h"
#include "p5prof.h"
#include "data.h"
#include "objout.h"
#include "x86objd.h"
#include "objprof.h"
#include "object.h"
#include "encode.h"
#include "x86proc.h"
#include "targetdb.h"
#include "opttell.h"
#include "x86obj.h"
#include "x86esc.h"
#include "rgtbl.h"
#include "namelist.h"
#include "optab.h"
#include "blktrim.h"
#include "tree.h"
#include "treefold.h"
#include "x86data.h"
#include "envvar.h"
#include "x86segs.h"
#include "x86enc.h"
#include "feprotos.h"


#define RMR_MOD_IND     0x80
#define RMR_MOD_DIR     5
#define RMR_MOD_SIB     4


static void OpndSizeIf( bool if_32 )
/**********************************/
{
    if( ( if_32 && _IsTargetModel( USE_32 ) ) || _IsntTargetModel( USE_32 ) ) {
        AddToTemp( M_OPND_SIZE );
    }
}


static void TakeUpSlack( type_length size )
/*****************************************/
{
    for( ; size >= 2; size -= 2 ) {
        if( _IsTargetModel( USE_32 ) )
            AddByte( M_OPND_SIZE );
        AddByte( M_MOVSW );
    }
    for( ; size >= 1; --size ) {
        AddByte( M_MOVSB );
    }
}


void    DoRepOp( instruction *ins )
/*********************************/
{
    type_length size;
    bool        first;

    size = ins->result->n.size;
    first = true;
    if( ins->head.opcode == OP_MOV && !UseRepForm( size ) ) {
        for( ; size >= 4; size -= 4 ) {
            if( first ) {
                LayOpbyte( M_MOVSW );
                OpndSizeIf( false );
                first = false;
            } else {
                if( _IsntTargetModel( USE_32 ) ) AddByte( M_OPND_SIZE );
                AddByte( M_MOVSW );
            }
        }
        TakeUpSlack( size );
    } else {
        LayOpbyte( M_REPE );
        if( ins->head.opcode == OP_MOV ) {
            if( ( size & ( 4 - 1 ) ) == 0 || OptForSize <= 50 ) {
                if( _IsntTargetModel( USE_32 ) ) AddByte( M_OPND_SIZE );
                AddByte( M_MOVSW );
                TakeUpSlack( size & ( 4 - 1 ) );
            } else {
                AddByte( M_MOVSB );
            }
        } else {
            if( ins->operands[1]->n.size & 1 ) {
                AddByte( M_CMPSB );
            } else {
                AddByte( M_CMPSW );
                if( _IsntTargetModel( USE_32 ) ) AddByte( M_OPND_SIZE );
            }
        }
    }
}


static  hw_reg_set IndexTab[] = {
        HW_D( HW_EAX ),
        HW_D( HW_ECX ),
        HW_D( HW_EDX ),
        HW_D( HW_EBX ),
        HW_D( HW_ESP ),
        HW_D( HW_EBP ),
        HW_D( HW_ESI ),
        HW_D( HW_EDI )
};


static  void    Add32Displacement( signed_32 val )
/************************************************/
{
    AddByte( val );
    AddByte( val >> 8 );
    AddByte( val >> 16 );
    AddByte( val >> 24 );
}

byte    Displacement( signed_32 val, hw_reg_set regs )
/****************************************************/
{
    if( val == 0 && !HW_COvlap( regs, HW_EBP ) )
        return( DISP0 );
    if( val <= 127 && val >= -128 ) {
        AddByte( val & 0xff );
        return( DISP8 );
    } else {
        Add32Displacement( val );
        return( DISPW );
    }
}


byte    DoIndex( hw_reg_set regs )
/********************************/
{
    byte i;

    for( i = 0; i < sizeof( IndexTab ) / sizeof( IndexTab[0] ); i++ ) {
        if( HW_Equal( regs, IndexTab[i] ) ) {
            return( i << S_RMR_RM );
        }
    }
    _Zoiks( ZOIKS_033 );
    return( 0 );
}


static  byte    DoScaleIndex( hw_reg_set base_reg,
                              hw_reg_set idx_reg, scale_typ scale )
/***********************************************************/
{
    byte        sib;

    sib = scale << 6;
    sib |= DoIndex( base_reg ) >> S_RMR_RM;
    sib |= ( DoIndex( idx_reg ) >> S_RMR_RM ) << S_RMR_REG;
    AddByte( sib );
    return( RMR_MOD_SIB );
}


type_length TmpLoc( name *base, name *op )
/****************************************/
{
    return( NewBase( base ) + op->v.offset - base->v.offset );
}


static  byte    DoDisp( type_length val, hw_reg_set base_reg, name *op )
/**********************************************************************/
{
    name        *base;

    if( op == NULL ) {
        return( Displacement( val, base_reg ) );
    } else if( op->n.class == N_TEMP ) {
        base = DeAlias( op );
        if( base->t.location == NO_LOCATION ) {
            _Zoiks( ZOIKS_034 );
        }
        val += TmpLoc( base, op );
        return( Displacement( val, base_reg ) );
    } else if( op->n.class == N_MEMORY ) {
        ILen += WORD_SIZE;
        DoSymRef( op, val, false );
        return( DISPW );
    } else if( op->n.class == N_CONSTANT ) {
        DoRelocConst( op, U4 );
        return( DISPW );
    } else {
        _Zoiks( ZOIKS_126 );
        return( DISP0 );
    }
}

byte    DoMDisp( name *op, bool alt_encoding )
/********************************************/
{
    hw_reg_set          regs;

    /* unused parameters */ (void)alt_encoding;

    regs = CalcSegment( op->v.symbol, op->m.memory_type );
    if( HW_COvlap( regs, HW_SEGS ) ) {
        GenSeg( regs );
    }
    DoMAddr( op );
    return( RMR_MOD_DIR );
}


static  void    EA( hw_reg_set base, hw_reg_set index, scale_typ scale,
                         signed_32 val, name *mem_loc, bool lea )
/***************************************************************/
{
    if( HW_CEqual( index, HW_ESP ) )
        _Zoiks( ZOIKS_079 );
    if( HW_CEqual( base, HW_EMPTY ) ) {
        if( HW_CEqual( index, HW_EMPTY ) ) {
            // [disp32]
            if( scale != 0 || val != 0 )
                _Zoiks( ZOIKS_079 );
            Inst[RMR] |= DoMDisp( mem_loc, true );

        } else {
            if( scale != 0 ) {
                // [disp32+scale_index]
                Inst[RMR] |= DoScaleIndex( HW_EBP, index, scale );
                Inst[RMR] |= DISP0;
                if( mem_loc != NULL ) {
                    ILen += 4;
                    DoSymRef( mem_loc, val, false );
                } else {
                    Add32Displacement( val );
                }
            } else {
                // [(disp0|disp8|disp32)+index]
                Inst[RMR] |= DoIndex( index );
                Inst[RMR] |= DoDisp( val, index, mem_loc );
            }
        }
    } else {
        // [(disp0|disp8|disp32)+base+scale_index]
        if( HW_CEqual( index, HW_EMPTY ) ) {
            if( scale == 0 && !HW_CEqual( base, HW_ESP ) ) {
                Inst[RMR] |= DoIndex( base );
            } else {
                Inst[RMR] |= DoScaleIndex( base, HW_ESP, scale );
            }
        } else {
            if( scale == 0 && HW_CEqual( base, HW_EBP )
              && ( lea || _IsntTargetModel( FLOATING_DS ) && _IsntTargetModel( FLOATING_SS ) ) ) {
                /*
                   flip base & index registers so that we might avoid having
                   to output a byte displacement (can't have a zero sized
                   displacement with EBP as the base register)
                */
                base = index;
                HW_CAsgn( index, HW_EBP );
            }
            Inst[RMR] |= DoScaleIndex( base, index, scale );
        }
        Inst[RMR] |= DoDisp( val, base, mem_loc );
    }
}

/*
 * 2004-11-05  RomanT
 *
 * Check that following instruction is addition or subtraction with constant,
 * and this instruction is safe to delete (ALU flags not generated).
 * If yes, return constant's value and delete instruction.
 * If no, return 0.
 * Caller must embed returned constant into current opcode.
 */

static  signed_32  GetNextAddConstant( instruction *ins )
/*******************************************************/
{
    instruction *next;
    int         neg;
    signed_32   disp;

    next = ins->head.next;
    neg = 1;
    disp = 0;
    switch( next->head.opcode ) {
    case OP_SUB:
        neg = -1;
        /* fall through */
    case OP_ADD:
        if( next->operands[0] != ins->result )
            break;
        if( next->result != ins->result )
            break;
        if( next->operands[1]->n.class != N_CONSTANT )
            break;
        if( next->operands[1]->c.const_type != CONS_ABSOLUTE )
            break;
        /* we should not remove the ADD if its flags are used! */
        if( next->ins_flags & INS_CC_USED )
            break;
        /*
           we've got something like:
                LEA     EAX, [ECX+EDX]
                ADD     EAX, 3
           turn it into:
                LEA     EAX, 3[ECX+EDX]
        */
        disp = neg * next->operands[1]->c.lo.int_value,
        DoNothing( next );
        break;
    }
    return( disp );
}

void    LayLeaRegOp( instruction *ins )
/*************************************/
{
    name        *left;
    name        *right;
    scale_typ   scale = 0;
    int         neg;
    signed_32   disp;

    left = ins->operands[0];
    right = ins->operands[1];
    neg = 1;
    switch( ins->head.opcode ) { /* add/sub/mul/shift transformed into lea */
    case OP_SUB:
        neg = -1;
        /* fall through */
    case OP_ADD:
        if( right->n.class == N_CONSTANT ) {
            if( right->c.const_type == CONS_ABSOLUTE ) {
                EA( HW_EMPTY, left->r.reg, 0, neg * right->c.lo.int_value, NULL, true );
            } else {
                EA( HW_EMPTY, left->r.reg, 0, 0, right, true );
            }
        } else if( right->n.class == N_REGISTER ) {
            disp = GetNextAddConstant( ins );
            EA( left->r.reg, right->r.reg, 0, disp, NULL, true );
        }
        break;
    case OP_MUL:
        switch( right->c.lo.int_value ) {
        case 3: scale = 1;  break;
        case 5: scale = 2;  break;
        case 9: scale = 3;  break;
        }
        disp = GetNextAddConstant( ins );   /* 2004-11-05  RomanT */
        EA( left->r.reg, left->r.reg, scale, disp, NULL, true );
        break;
    case OP_LSHIFT:
        disp = GetNextAddConstant( ins );   /* 2004-11-05  RomanT */
        switch( right->c.lo.int_value ) {
        case 1:
            if( _CPULevel( CPU_586 ) ) {
                // want to avoid the extra big-fat 0 on 586's
                // but two base registers is slower on a 486
                EA( left->r.reg, left->r.reg, 0, disp, NULL, true );
                break;
            }
            /* fall through */
        default:
            EA( HW_EMPTY, left->r.reg, (scale_typ)right->c.lo.int_value, disp, NULL, true );
        }
        break;
    }
}


static  void    CheckSize( void )
/*******************************/
{
    if( _IsntTargetModel( USE_32 ) ) {
        AddToTemp( M_ADDR_SIZE );
    }
}


static  void    LayIdxModRM( name *op )
/*************************************/
{
    hw_reg_set  base_reg;
    hw_reg_set  idx_reg;
    name        *mem_loc;

    CheckSize();
    if( HW_COvlap( op->i.index->r.reg, HW_SEGS ) ) {
        /* check for seg override*/
        GenSeg( op->i.index->r.reg );
    }
    idx_reg = op->i.index->r.reg;
    HW_CTurnOff( idx_reg, HW_SEGS );
    HW_CAsgn( base_reg, HW_EMPTY );
    if( HasTrueBase( op ) ) {
        if( op->i.base->n.class == N_TEMP ) {
            if( BaseIsSP( op->i.base ) ) {
                HW_CAsgn( base_reg, HW_ESP );
            } else {
                HW_CAsgn( base_reg, HW_EBP );
            }
        } else if( op->i.base->n.class == N_MEMORY ) {
            GenSeg( CalcSegment( op->i.base->v.symbol,
                                 op->i.base->m.memory_type ) );
        }
        mem_loc = op->i.base;
    } else {
        mem_loc = NULL;
    }
    if( op->i.index_flags & X_BASE ) {
        if( op->i.index_flags & X_HIGH_BASE ) {
            base_reg = High64Reg( idx_reg );
        } else {
            base_reg = Low64Reg( idx_reg );
        }
    }
    HW_TurnOff( idx_reg, base_reg );
    if( HW_CEqual( idx_reg, HW_ESP ) && HW_CEqual( base_reg, HW_EMPTY ) ) {
        HW_CAsgn( base_reg, HW_ESP );
        HW_CAsgn( idx_reg, HW_EMPTY );
    }
    EA( base_reg, idx_reg, op->i.scale, op->i.constant, mem_loc, false );
}


void    LayModRM( name *op )
/**************************/
{
    name        *base;

    switch( op->n.class ) {
    case N_MEMORY:
        CheckSize();
        EA( HW_EMPTY, HW_EMPTY, 0, 0, op, false );
        break;
    case N_TEMP:
        CheckSize();
        base = DeAlias( op );
        if( base->t.location == NO_LOCATION ) {
            _Zoiks( ZOIKS_030 );
        }
        if( BaseIsSP( base ) ) {
            EA( HW_ESP, HW_EMPTY, 0, TmpLoc( base, op ), NULL, false );
        } else {
            EA( HW_EBP, HW_EMPTY, 0, TmpLoc( base, op ), NULL, false );
        }
        break;
    case N_INDEXED:
        LayIdxModRM( op );
        break;
    case N_REGISTER:
        LayRMRegOp( op );
        break;
    }
}


void    DoMAddr( name *op )
/*************************/
{
    ILen += 4;
    if( op->n.class == N_CONSTANT ) {
        _Zoiks( ZOIKS_035 );
    } else {        /* assume global name*/
        DoSymRef( op, op->v.offset, false );
    }
}


void    DoRelocConst( name *op, type_class_def type_class )
/*********************************************************/
{
    if( op->c.const_type == CONS_OFFSET ) {
        ILen += 4;
        DoSymRef( op->c.value, ((var_name *)op->c.value)->offset, false );
    } else if( op->c.const_type == CONS_SEGMENT ) {
        ILen += 2;
        if( op->c.value == NULL ) {
            DoSegRef( (segment_id)op->c.lo.int_value );
        } else {
            DoSymRef( op->c.value, 0, true );
        }
        if( type_class == U4 || type_class == I4 ) {        /* as in PUSH seg _x */
            AddByte( 0 );
            AddByte( 0 );
        }
    } else if( op->c.const_type == CONS_ADDRESS ) {
        _Zoiks( ZOIKS_045 );
    }
}


void    GenUnkPush( pointer value )
/*********************************/
{
    _Code;
    LayOpbyte( M_PUSHI );
    OpndSizeIf( false );
    ILen += 4;
    DoAbsPatch( value, 4 );
    _Emit;
}


void    GenPushC( signed_32 value )
/*********************************/
{
    _Code;
    LayOpbyte( M_PUSHI );
    OpndSizeIf( false );
    AddSData( value, I4 );
    _Emit;
}


pointer GenFar16Thunk( pointer label, unsigned_16 parms_size, bool remove_parms )
/*******************************************************************************/
{
    segment_id  old_segid;
    pointer     code_32;

    old_segid = SetOP( AskCode16Seg() );
    // CodeLabel( label, DepthAlign( PROC_ALIGN ) );
    code_32 = AskForNewLabel();
    TellOptimizerByPassed();
    SetUpObj( false );
    OutLabel( label );
    OutDataByte( 0xb9 );                /* mov cx,# */
    OutDataShort( parms_size );
    OutDataByte( 0x9a );
    OutRTImport( RT_Far32Func, F_FAR16 );
    OutDataShort( 0 );
    OutDataShort( 0 );
    if( remove_parms ) {
        OutDataByte( 0xca );
        OutDataShort( parms_size );
    } else {
        OutDataByte( 0xcb );
        OutDataShort( 0 );              // padding
    }
    // emit "reloc for offset of code_32 label"
    SetUpObj( true );
    TellKeepLabel( code_32 );
    OutReloc( AskCodeSeg(), F_OFFSET, false );
    OutLblPatch( code_32, F_OFFSET, 0 );
    TellByPassOver();
    SetOP( old_segid );
    return( code_32 );
}


static void    doProfilingCode( char *fe_name, label_handle *data, bool prolog )
/******************************************************************************/
{
    if( prolog )
        GenProfileData( fe_name, data, NULL );
    _Code;
    LayOpbyte( 0x68 );
    ILen += 4;
    DoLblRef( *data, (segment_id)(pointer_uint)FEAuxInfo( NULL, P5_PROF_SEG ), 0, OFST );
    _Emit;
    DoRTCall( prolog ? RT_PROFILE_ON : RT_PROFILE_OFF, true );
}


static  void    doProfilingPrologEpilog( label_handle label, bool prolog )
/************************************************************************/
{
    if( _IsTargetModel( NEW_P5_PROFILING ) ) {
        doProfilingCode( "", &CurrProc->targ.routine_profile_data, prolog );
    } else {
        back_handle     bck;
        label_handle    data_lbl;
        segment_id      data_segid;

        bck = (back_handle)FEAuxInfo( AskForLblSym( label ), P5_PROF_DATA );
        if( bck == NULL )
            return;
        data_lbl = bck->lbl;
        data_segid = (segment_id)(pointer_uint)FEAuxInfo( NULL, P5_PROF_SEG );
        TellKeepLabel( data_lbl );
        _Code;
        if( prolog ) {
            LayOpword( 0x05ff );                        /* inc L1+count */
            ILen += 4;
            DoLblRef( data_lbl, data_segid, offsetof( P5_timing_info, count ), OFST);
            _Next;
        }
        LayOpword( prolog ? 0x05ff : 0x0dff );          /* inc/dec L1+semaphore */
        ILen += 4;
        DoLblRef( data_lbl, data_segid, offsetof( P5_timing_info, semaphore ), OFST );
        _Next;
        if( _IsTargetModel( P5_PROFILING_CTR0 ) ) {
            LayOpword( prolog ? 0x1675 : 0x167d );      /* jne/jge skip */
            _Next;
            LayOpbyte( 0x51 );                          /* push ecx */
            _Next;
        } else {
            LayOpword( prolog ? 0x1275 : 0x127d );      /* jne/jge skip */
            _Next;
        }
        LayOpbyte( 0x52 );                              /* push edx */
        _Next;
        if( _IsTargetModel( P5_PROFILING_CTR0 ) ) {
            LayOpword( 0xc931 );                        /* xor ecx,ecx */
            _Next;
        }
        LayOpbyte( 0x50 );                              /* push eax */
        _Next;
        if( _IsTargetModel( P5_PROFILING_CTR0 ) ) {
            LayOpword( 0x330f );                        /* rdpmc */
            _Next;
        } else {
            LayOpword( 0x310f );                        /* rdtsc */
            _Next;
        }
        LayOpword( prolog ? 0x0529 : 0x0501 );          /* sub/add L1+lo_cycle,eax */
        ILen += 4;
        DoLblRef( data_lbl, data_segid, offsetof( P5_timing_info, lo_cycle ), OFST );
        _Next;
        LayOpbyte( 0x58 );                              /* pop eax */
        _Next;
        LayOpword( prolog ? 0x1519 : 0x1511 );          /* sbb/adc L1+hi_cycle,edx */
        ILen += 4;
        DoLblRef( data_lbl, data_segid, offsetof( P5_timing_info, hi_cycle ), OFST );
        _Next;
        LayOpbyte( 0x5a );                              /* pop edx */
        _Next;
        if( _IsTargetModel( P5_PROFILING_CTR0 ) ) {
            LayOpbyte( 0x59 );                          /* pop ecx */
            _Next;
        }
        _Emit;
    }
}

#if 0
void    GenProfilingCode( char *fe_name, label_handle *data, bool prolog )
/************************************************************************/
{
    if( _IsTargetModel( NEW_P5_PROFILING ) ) {
        doProfilingCode( fe_name, data, prolog );
    }
}
#endif

segment_id GenProfileData( char *fe_name, label_handle *data, label_handle *stack )
/*********************************************************************************/
/* generate P5 profiler code                                                     */
/*********************************************************************************/
{
    segment_id      old_segid;
    segment_id      data_segid = (segment_id)(pointer_uint)FEAuxInfo( NULL, P5_PROF_SEG );

    old_segid = SetOP( data_segid );
    TellOptimizerByPassed();
    SetUpObj( true );
    *data = AskForNewLabel();
    OutLabel( *data );
    OutDataByte( *fe_name );                    //flag
    OutDataByte( 0 );
    OutDataByte( 0 );
    OutDataByte( 0 );
    OutDataLong( 0 );                           //semaphore
    if( stack == NULL ) {
        OutDataLong( 0 );                       //stack
    } else {
        OutReloc( data_segid, F_OFFSET, false );  //caller
        OutLblPatch( *stack, F_OFFSET, 0 );
    }
    OutDataLong( 0 );                           //esp
    OutDataLong( 0 );                           //dynamic
    OutDataLong( 0 );                           //lo_count
    OutDataLong( 0 );                           //hi_count
    OutDataLong( 0 );                           //lo_cycle
    OutDataLong( 0 );                           //hi_cycle
    OutDataLong( 0 );                           //lo_start_time
    OutDataLong( 0 );                           //hi_start_time
    OutReloc( AskCodeSeg(), F_OFFSET, false );  //caller
    OutLblPatch( CurrProc->label, F_OFFSET, 0 );
    OutDataLong( 0 );                           //call_ins
    OutDataLong( 0 );                           //callee
    TellByPassOver();
    SetOP( old_segid );
    return( data_segid );
}


void    GenP5ProfilingProlog( label_handle label )
/************************************************/
{
    doProfilingPrologEpilog( label, true );
}


void    GenP5ProfilingEpilog( label_handle label )
/************************************************/
{
    doProfilingPrologEpilog( label, false );
}


void    GFstp10( type_length where )
/**********************************/
{
    GCondFwait();
    CheckSize();
    LayOpword( 0x3ddb );
    EA( HW_EMPTY, HW_EBP, 0, -where, NULL, false );
    _Emit;
}


void    GFld10( type_length where )
/*********************************/
{
    GCondFwait();
    CheckSize();
    LayOpword( 0x2ddb );
    EA( HW_EMPTY, HW_EBP, 0, -where, NULL, false );
    _Emit;
}


void    Do4Shift( instruction *ins )
/**********************************/
{
    /* unused parameters */ (void)ins;
}


void    Do4RShift( instruction *ins )
/***********************************/
/* NOT NEEDED ON 386 */
{
    /* unused parameters */ (void)ins;
}


void    Gen4RNeg( instruction *ins )
/**********************************/
/* NOT NEEDED ON 386 */
{
    /* unused parameters */ (void)ins;
}


void    Pow2Div( instruction *ins )
/*********************************/
{
    int         log2;
    bool        if_32;

    if_32 = false;
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
        if_32 = true;
        /* fall through */
    case I4:
    case U4:
        LayOpword( 0xe2c1 );    /* shl  edx,n */
        OpndSizeIf( if_32 );
        AddByte( log2 );
        _Next;
        LayOpword( 0xc21b );    /* sbb  eax,edx */
        OpndSizeIf( if_32 );
        _Next;
        LayOpword( 0xf8c1 );    /* sar  eax,n */
        OpndSizeIf( if_32 );
        AddByte( log2 );
        break;
    }
}

void    By2Div( instruction *ins )
/********************************/
{
    bool        if_32;

    if_32 = false;
    switch( ins->type_class ) {
    case I1:
    case U1:
        LayOpword( 0xc42a );    /* sub  al,ah */
        _Next;
        LayOpword( 0xf8d0 );    /* sar  al,1 */
        break;
    case I2:
    case U2:
        if_32 = true;
        /* fall through */
    case I4:
    case U4:
        LayOpword( 0xc22b );    /* sub  eax,edx */
        OpndSizeIf( if_32 );
        _Next;
        LayOpword( 0xf8d1 );    /* sar  eax,1 */
        OpndSizeIf( if_32 );
        break;
    }
}


void    Gen4Neg( instruction *ins )
/*********************************/
/* NOT NEEDED ON 386 */
{
    /* unused parameters */ (void)ins;
}


void    Do4CXShift( instruction *ins, void (*rtn)(instruction *) )
/****************************************************************/
/* NOT NEEDED ON 386 */
{
    /* unused parameters */ (void)ins; (void)rtn;
}


void StartBlockProfiling( block *blk )
/************************************/
{
    segment_id          old_segid;
    segment_id          data_segid;
    label_handle        data;

    if( _IsntTargetModel( NEW_P5_PROFILING ) )
        return;
    if( _IsntTargetModel( STATEMENT_COUNTING ) )
        return;
    data_segid = (segment_id)(pointer_uint)FEAuxInfo( NULL, P5_PROF_SEG );
    if( blk->label == NULL )
        return;
    TellKeepLabel( blk->label );
    old_segid = SetOP( data_segid );
    TellOptimizerByPassed();
    SetUpObj( true );
    data = AskForNewLabel();
    TellKeepLabel( data );
    OutLabel( data );
    OutDataByte( PROFILE_FLAG_BLOCK );          //flag
    OutDataByte( 0 );                           //...
    OutDataByte( 0 );                           //...
    OutDataByte( 0 );                           //...
    OutDataLong( 0 );                           //lo_count
    OutDataLong( 0 );                           //hi_count
    OutReloc( AskCodeSeg(), F_OFFSET, false );  //block
    OutLblPatch( blk->label, F_OFFSET, 0 );
    OutReloc( AskCodeSeg(), F_OFFSET, false );  //function
    OutLblPatch( CurrProc->label, F_OFFSET, 0 );
    TellByPassOver();
    SetOP( old_segid );
    _Code;
    LayOpword( 0x0583 );                /* add L1+lo_count,1 */
    ILen += 4;
    DoLblRef( data, data_segid, offsetof( block_count_info, lo_count ), OFST );
    AddByte( 1 );
    _Next;
    LayOpword( 0x1583 );                /* adc L1+hi_count,0 */
    ILen += 4;
    DoLblRef( data, data_segid, offsetof( block_count_info, hi_count ), OFST );
    AddByte( 0 );
    _Emit;
}


void EndBlockProfiling( void )
/****************************/
{
}
