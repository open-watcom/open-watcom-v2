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
* Description:  MIPS register usage and linkage conventions.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "mipsregn.h"
#include "zoiks.h"
#include "data.h"

static  hw_reg_set      Empty[] = {
    EMPTY
};

static  hw_reg_set      Reg64Order[] = {
    HW_D_1( HW_D2 ),
    HW_D_1( HW_D3 ),
    HW_D_1( HW_D4 ),
    HW_D_1( HW_D5 ),
    HW_D_1( HW_D6 ),
    HW_D_1( HW_D7 ),
    HW_D_1( HW_D8 ),
    HW_D_1( HW_D9 ),
    HW_D_1( HW_D10 ),
    HW_D_1( HW_D11 ),
    HW_D_1( HW_D12 ),
    HW_D_1( HW_D13 ),
    HW_D_1( HW_D14 ),
    HW_D_1( HW_D15 ),
    HW_D_1( HW_D16 ),
    HW_D_1( HW_D17 ),
    HW_D_1( HW_D18 ),
    HW_D_1( HW_D19 ),
    HW_D_1( HW_D20 ),
    HW_D_1( HW_D21 ),
    HW_D_1( HW_D22 ),
    HW_D_1( HW_D23 ),
    HW_D_1( HW_D24 ),
    HW_D_1( HW_D25 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      ByteRegs[] = {
    HW_D_1( HW_B0 ),
    HW_D_1( HW_B1 ),
    HW_D_1( HW_B2 ),
    HW_D_1( HW_B3 ),
    HW_D_1( HW_B4 ),
    HW_D_1( HW_B5 ),
    HW_D_1( HW_B6 ),
    HW_D_1( HW_B7 ),
    HW_D_1( HW_B8 ),
    HW_D_1( HW_B9 ),
    HW_D_1( HW_B10 ),
    HW_D_1( HW_B11 ),
    HW_D_1( HW_B12 ),
    HW_D_1( HW_B13 ),
    HW_D_1( HW_B14 ),
    HW_D_1( HW_B15 ),
    HW_D_1( HW_B16 ),
    HW_D_1( HW_B17 ),
    HW_D_1( HW_B18 ),
    HW_D_1( HW_B19 ),
    HW_D_1( HW_B20 ),
    HW_D_1( HW_B21 ),
    HW_D_1( HW_B22 ),
    HW_D_1( HW_B23 ),
    HW_D_1( HW_B24 ),
    HW_D_1( HW_B25 ),
    HW_D_1( HW_B26 ),
    HW_D_1( HW_B27 ),
    HW_D_1( HW_B28 ),
    HW_D_1( HW_B29 ),
    HW_D_1( HW_B30 ),
    HW_D_1( HW_B31 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      WordRegs[] = {
    HW_D_1( HW_W0 ),
    HW_D_1( HW_W1 ),
    HW_D_1( HW_W2 ),
    HW_D_1( HW_W3 ),
    HW_D_1( HW_W4 ),
    HW_D_1( HW_W5 ),
    HW_D_1( HW_W6 ),
    HW_D_1( HW_W7 ),
    HW_D_1( HW_W8 ),
    HW_D_1( HW_W9 ),
    HW_D_1( HW_W10 ),
    HW_D_1( HW_W11 ),
    HW_D_1( HW_W12 ),
    HW_D_1( HW_W13 ),
    HW_D_1( HW_W14 ),
    HW_D_1( HW_W15 ),
    HW_D_1( HW_W16 ),
    HW_D_1( HW_W17 ),
    HW_D_1( HW_W18 ),
    HW_D_1( HW_W19 ),
    HW_D_1( HW_W20 ),
    HW_D_1( HW_W21 ),
    HW_D_1( HW_W22 ),
    HW_D_1( HW_W23 ),
    HW_D_1( HW_W24 ),
    HW_D_1( HW_W25 ),
    HW_D_1( HW_W26 ),
    HW_D_1( HW_W27 ),
    HW_D_1( HW_W28 ),
    HW_D_1( HW_W29 ),
    HW_D_1( HW_W30 ),
    HW_D_1( HW_W31 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      DWordRegs[] = {
    HW_D_1( HW_D0 ),
    HW_D_1( HW_D1 ),
    HW_D_1( HW_D2 ),
    HW_D_1( HW_D3 ),
    HW_D_1( HW_D4 ),
    HW_D_1( HW_D5 ),
    HW_D_1( HW_D6 ),
    HW_D_1( HW_D7 ),
    HW_D_1( HW_D8 ),
    HW_D_1( HW_D9 ),
    HW_D_1( HW_D10 ),
    HW_D_1( HW_D11 ),
    HW_D_1( HW_D12 ),
    HW_D_1( HW_D13 ),
    HW_D_1( HW_D14 ),
    HW_D_1( HW_D15 ),
    HW_D_1( HW_D16 ),
    HW_D_1( HW_D17 ),
    HW_D_1( HW_D18 ),
    HW_D_1( HW_D19 ),
    HW_D_1( HW_D20 ),
    HW_D_1( HW_D21 ),
    HW_D_1( HW_D22 ),
    HW_D_1( HW_D23 ),
    HW_D_1( HW_D24 ),
    HW_D_1( HW_D25 ),
    HW_D_1( HW_D26 ),
    HW_D_1( HW_D27 ),
    HW_D_1( HW_D28 ),
    HW_D_1( HW_D29 ),
    HW_D_1( HW_D30 ),
    HW_D_1( HW_D31 ),
    HW_D_1( HW_EMPTY )
};

/* 64-bit "double" registers for MIPS32 */
static  hw_reg_set      QWordRegs[] = {
    HW_D_1( HW_Q2 ),
    HW_D_1( HW_Q4 ),
    HW_D_1( HW_Q6 ),
    HW_D_1( HW_Q8 ),
    HW_D_1( HW_Q10 ),
    HW_D_1( HW_EMPTY )
};

/* These GPRs are 32-bit for MIPS32 and 64-bit for MIPS64 */
static  hw_reg_set      GeneralRegs[] = {
    HW_D_1( HW_R0 ),
    HW_D_1( HW_R1 ),
    HW_D_1( HW_R2 ),
    HW_D_1( HW_R3 ),
    HW_D_1( HW_R4 ),
    HW_D_1( HW_R5 ),
    HW_D_1( HW_R6 ),
    HW_D_1( HW_R7 ),
    HW_D_1( HW_R8 ),
    HW_D_1( HW_R9 ),
    HW_D_1( HW_R10 ),
    HW_D_1( HW_R11 ),
    HW_D_1( HW_R12 ),
    HW_D_1( HW_R13 ),
    HW_D_1( HW_R14 ),
    HW_D_1( HW_R15 ),
    HW_D_1( HW_R16 ),
    HW_D_1( HW_R17 ),
    HW_D_1( HW_R18 ),
    HW_D_1( HW_R19 ),
    HW_D_1( HW_R20 ),
    HW_D_1( HW_R21 ),
    HW_D_1( HW_R22 ),
    HW_D_1( HW_R23 ),
    HW_D_1( HW_R24 ),
    HW_D_1( HW_R25 ),
    HW_D_1( HW_R26 ),
    HW_D_1( HW_R27 ),
    HW_D_1( HW_R28 ),
    HW_D_1( HW_R29 ),
    HW_D_1( HW_R30 ),
    HW_D_1( HW_R31 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      FloatRegs[] = {
    HW_D_1( HW_F0 ),
    HW_D_1( HW_F1 ),
    HW_D_1( HW_F2 ),
    HW_D_1( HW_F3 ),
    HW_D_1( HW_F4 ),
    HW_D_1( HW_F5 ),
    HW_D_1( HW_F6 ),
    HW_D_1( HW_F7 ),
    HW_D_1( HW_F8 ),
    HW_D_1( HW_F9 ),
    HW_D_1( HW_F10 ),
    HW_D_1( HW_F11 ),
    HW_D_1( HW_F12 ),
    HW_D_1( HW_F13 ),
    HW_D_1( HW_F14 ),
    HW_D_1( HW_F15 ),
    HW_D_1( HW_F16 ),
    HW_D_1( HW_F17 ),
    HW_D_1( HW_F18 ),
    HW_D_1( HW_F19 ),
    HW_D_1( HW_F20 ),
    HW_D_1( HW_F21 ),
    HW_D_1( HW_F22 ),
    HW_D_1( HW_F23 ),
    HW_D_1( HW_F24 ),
    HW_D_1( HW_F25 ),
    HW_D_1( HW_F26 ),
    HW_D_1( HW_F27 ),
    HW_D_1( HW_F28 ),
    HW_D_1( HW_F29 ),
    HW_D_1( HW_F30 ),
    HW_D_1( HW_F31 ),
    HW_D_1( HW_EMPTY )
};

/*
 * NOTE: ordering is important here - see state->curr_entry
 * and relating code in ParmReg and CallState. BBB
 */

static  hw_reg_set      AllParmRegs[] = {
    HW_D_1( HW_R4 ),
    HW_D_1( HW_R5 ),
    HW_D_1( HW_R6 ),
    HW_D_1( HW_R7 ),
    HW_D_1( HW_Q4 ),
    HW_D_1( HW_Q6 ),
    HW_D_1( HW_F12 ),
    HW_D_1( HW_F14 ),
    HW_D_1( HW_EMPTY )
};


static  hw_reg_set      Parm8Regs[] = {
    HW_D_1( HW_Q4 ),
    HW_D_1( HW_Q6 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      Parm8Regs2[] = {
    HW_D_1( HW_Q6 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      Return8[] = {
    HW_D_1( HW_Q2 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      Parm4Regs[] = {
    HW_D_1( HW_D4 ),
    HW_D_1( HW_D5 ),
    HW_D_1( HW_D6 ),
    HW_D_1( HW_D7 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      Parm4Regs2[] = {
    HW_D_1( HW_D5 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      Parm4Regs3[] = {
    HW_D_1( HW_D6 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      Return4[] = {
    HW_D_1( HW_D2 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      ParmFRegs[] = {
    HW_D_1( HW_F12 ),
    HW_D_1( HW_F14 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      ParmFRegs2[] = {
    HW_D_1( HW_F17 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      ReturnD[] = {
    HW_D_1( HW_F0 ),
    HW_D_1( HW_EMPTY )
};

hw_reg_set      *RegSets[] = {
    #define RL(a,b,c,d) a
    #include "rl.h"
    #undef RL
    NULL
};

op_regs RegList[] = {
    #define RG( a,b,c,d,e,f ) {a,b,c,d,e}
    #include "rg.h"
    #undef RG
};

static  reg_set_index   IsSets[] = {
    RL_BYTE,                /* U1 */
    RL_BYTE,                /* I1 */
    RL_WORD,                /* U2 */
    RL_WORD,                /* I2 */
    RL_DWORD,               /* U4 */
    RL_DWORD,               /* I4 */
    RL_QWORD,               /* U8 */
    RL_QWORD,               /* I8 */
    RL_DWORD,               /* CP */
    RL_DWORD,               /* PT */
    RL_FLOAT,               /* FS */
    RL_FLOAT,               /* FD */
    RL_,                    /* FL */
    RL_                     /* XX */
};


/*      Information for register set intersections
 *      if sets are of different classes {
 *          intersection is empty
 *      } else {
 *          intersection given in square matrix for class
 *      }
 */

#define _Combine( a, b )        ((a) * RL_NUMBER_OF_SETS + (b))


extern  reg_set_index RegIntersect( reg_set_index s1, reg_set_index s2 )
/**********************************************************************/
{
    if( s1 == s2 ) return( s1 );
    switch( _Combine( s1, s2 ) ) {
    default:
        if( s1 == RL_NUMBER_OF_SETS ) return( s2 );
        if( s2 == RL_NUMBER_OF_SETS ) return( s1 );
        return( RL_ );
    }
}


extern  hw_reg_set InLineParm( hw_reg_set regs, hw_reg_set used )
/***************************************************************/
{
    used = used;

    return( regs );
}


extern  hw_reg_set *ParmChoices( type_class_def class )
/***************************************************/
{
    switch( class ) {
    case I4:
    case U4:
    case CP:
    case PT:
    case XX:
        return( RegSets[RL_PARM_4] );
    case I8:
    case U8:
        return( RegSets[RL_PARM_8] );
    case FS:
    case FD:
    case FL:
        return( RegSets[RL_PARM_F] );
    default:
        _Zoiks( ZOIKS_124 );
        return( NULL );
    }
}


extern  hw_reg_set ReturnReg( type_class_def class )
/**************************************************/
{
    switch( class ) {
    case FS:
    case FD:
    case FL:
        return( HW_F0 );
    case XX:
        return( HW_EMPTY );
    case U1:
    case I1:
        return( HW_B2 );
    case U2:
    case I2:
        return( HW_W2 );
    case U4:
    case I4:
        return( HW_D2 );
    case U8:
    case I8:
        return( HW_Q2 );
    default:
        return( HW_R2 );
    }
}


extern  hw_reg_set *ParmRegs( void )
/**********************************/
{
    return( &AllParmRegs[0] );
}


extern  hw_reg_set ParmRegConflicts( hw_reg_set regs )
/****************************************************/
{
    hw_reg_set          conflicts;

    conflicts = regs;
    if( HW_COvlap( regs, HW_R4 ) ) {
        HW_CTurnOn( conflicts, HW_F4 );
        HW_CTurnOn( conflicts, HW_R4 );
    }
    if( HW_COvlap( regs, HW_R5 ) ) {
        HW_CTurnOn( conflicts, HW_F5 );
        HW_CTurnOn( conflicts, HW_R5 );
    }
    if( HW_COvlap( regs, HW_R6 ) ) {
        HW_CTurnOn( conflicts, HW_F6 );
        HW_CTurnOn( conflicts, HW_R6 );
    }
    if( HW_COvlap( regs, HW_R7 ) ) {
        HW_CTurnOn( conflicts, HW_F7 );
        HW_CTurnOn( conflicts, HW_R7 );
    }
    if( HW_COvlap( regs, HW_F12 ) ) {
        HW_CTurnOn( conflicts, HW_R12 );
        HW_CTurnOn( conflicts, HW_F12 );
    }
    if( HW_COvlap( regs, HW_F14 ) ) {
        HW_CTurnOn( conflicts, HW_R14 );
        HW_CTurnOn( conflicts, HW_F14 );
    }
    return( conflicts );
}


extern  reg_set_index SegIndex( void )
/************************************/
{
    return( RL_ );
}


extern  reg_set_index NoSegments( reg_set_index idx )
/***************************************************/
{
    return( idx );
}


extern  reg_set_index IndexIntersect( reg_set_index curr,
                                      type_class_def class,
                                      bool is_temp_index )
/*********************************************************/
{
    curr = curr;
    class = class;
    is_temp_index = is_temp_index;
    return( RL_DWORD );
}


extern  bool IsIndexReg( hw_reg_set reg, type_class_def class,
                         bool is_temp_index )
/************************************************************/
{
    hw_reg_set          *dregs;

    is_temp_index = is_temp_index;
    class = class;
    for( dregs = &DWordRegs[0]; !HW_CEqual( *dregs, HW_EMPTY ); ++dregs ) {
        if( HW_Equal( *dregs, reg ) ) return( TRUE );
    }
    return( FALSE );
}


extern  bool IndexRegOk( hw_reg_set reg, bool is_temp_index )
/***********************************************************/
{
    is_temp_index = is_temp_index;
    reg = reg;
    return( FALSE );
}


extern  bool IsSegReg( hw_reg_set regs )
/**************************************/
{
    regs = regs;
    return( FALSE );
}


extern  type_class_def RegClass( hw_reg_set regs )
/************************************************/
{
    hw_reg_set          test;
    hw_reg_set          *possible;

    if( HW_COvlap( regs, HW_FPR ) )
        return( FD );
    HW_CAsgn( test, HW_BREGS );
    if( HW_Subset( test, regs ) )
        return( U1 );
    HW_CAsgn( test, HW_WREGS );
    if( HW_Subset( test, regs ) )
        return( U2 );
    // 64-bit registers are really pairs of registers; we must explicitly
    // check to see if we match a valid pair
    for( possible = QWordRegs; !HW_CEqual( *possible, HW_EMPTY ); ++possible ) {
        if( HW_Equal( *possible, regs ) ) {
            return( U8 );
        }
    }
    return( U4 );   // would be different for MIPS64
}


extern  hw_reg_set Low16Reg( hw_reg_set regs )
/********************************************/
{
    regs = regs;
    return( HW_EMPTY );
}


extern  hw_reg_set High16Reg( hw_reg_set regs )
/*********************************************/
{
    regs = regs;
    return( HW_EMPTY );
}


extern  hw_reg_set Low32Reg( hw_reg_set regs )
/********************************************/
{
    regs = regs;
    return( HW_EMPTY );
}


extern  hw_reg_set High32Reg( hw_reg_set regs )
/*********************************************/
{
    regs = regs;
    return( HW_EMPTY );
}


extern  hw_reg_set High48Reg( hw_reg_set regs )
/*********************************************/
{
    regs = regs;
    return( HW_EMPTY );
}


extern  hw_reg_set Low48Reg( hw_reg_set regs )
/********************************************/
{
    regs = regs;
    return( HW_EMPTY );
}


extern  hw_reg_set Low64Reg( hw_reg_set regs )
/*********************************************
 * return the low order part of 64 bit register "regs"
 */
{
    hw_reg_set  low;
    hw_reg_set  *order;

    if( HW_CEqual( regs, HW_EMPTY ) )
        return( HW_EMPTY );
    for( order = Reg64Order; ; ++order ) {
        if( HW_Ovlap( *order, regs ) ) break;
    }
    low = regs;
    HW_OnlyOn( low, *order );
    if( HW_Equal( low, regs ) ) {
        low = HW_EMPTY;
    }
    return( low );
}


extern  hw_reg_set High64Reg( hw_reg_set regs )
/**********************************************
 * return the high order part of 64 bit register "regs"
 */
 {
    hw_reg_set  high;

    high = Low64Reg( regs );
    if( !HW_CEqual( high, HW_EMPTY ) ) {
        HW_TurnOff( regs, high );
        return( regs );
    }
    return( high );
}


extern  hw_reg_set HighReg( hw_reg_set regs )
/*******************************************/
{
    switch( RegClass( regs ) ) {
    case FD:
    case U8:
    case I8:
        return( High64Reg( regs ) );
    default:
        return( HW_EMPTY );
    }
}


extern  hw_reg_set LowReg( hw_reg_set regs )
/******************************************/
{
    switch( RegClass( regs ) ) {
    case U8:
    case I8:
    case FD:
        return( Low64Reg( regs ) );
    default:
        return( HW_EMPTY );
    }
}


extern  hw_reg_set FullReg( hw_reg_set regs )
/*******************************************/
{
    regs = regs;
    return( regs );
}


extern  bool IsRegClass( hw_reg_set regs, type_class_def class )
/**************************************************************/
{
    hw_reg_set  *list;

    for( list = RegSets[IsSets[class]]; !HW_CEqual( *list, HW_EMPTY ); ++list ) {
        if( HW_Equal( *list, regs ) ) return( TRUE );
    }
    return( FALSE );
}


extern  reg_set_index UsualPossible( type_class_def class )
/*********************************************************/
{
    return( IsSets[class] );
}


extern  hw_reg_set  ActualParmReg( hw_reg_set reg )
/*************************************************/
{
    return( reg );
}


extern  hw_reg_set FixedRegs( void )
/**********************************/
{
    /* MJC do you really want to fix them */
    hw_reg_set          fixed;

    HW_CAsgn( fixed, HW_R0 );       // $zero
    HW_CTurnOn( fixed, HW_R1 );     // $at (needed as scratch register)
    HW_CTurnOn( fixed, HW_R28 );    // $gp
    HW_CTurnOn( fixed, HW_R29 );    // $sp
    // We should be able to use $fp as $s8 in theory, but that isn't working
    // when alloca() is used for some reason (alloca() will force us to use
    // a frame pointer separate from $sp). So just make $fp always fixed.
//    if( CurrProc->targ.base_is_fp )
        HW_CTurnOn( fixed, HW_R30 );// $fp
    HW_CTurnOn( fixed, HW_F30 );    // TODO: Used to generate problematic converts (I8->FD and such)
    return( fixed );
}


extern  hw_reg_set VarargsHomePtr( void )
/***************************************/
{
    return( HW_R23 );
}


extern  hw_reg_set StackReg( void )
/*********************************/
{
    /* should be up to linkage conventions? */
    return( HW_R29 );
}


extern  hw_reg_set FrameBaseReg( void )
/*************************************/
{
    return( HW_R30 );
}


extern  hw_reg_set FrameReg( void )
/**********************************/
{
    /* should be up to linkage conventions? */
    if( CurrProc->targ.base_is_fp ) {
        return( HW_R30 );
    }
    return( HW_R29 );
}


extern  hw_reg_set ScratchReg( void )
/***********************************/
{
    return( HW_R1 );
}


extern  hw_reg_set ReturnAddrReg( void )
/**************************************/
{
    /* BBB Like MJC said ^ */
    return( HW_R31 );
}


extern  hw_reg_set DisplayReg( void )
/***********************************/
{
    return( HW_EMPTY );
}


extern  int SizeDisplayReg( void )
/********************************/
{
    return( 0 );
}


extern  hw_reg_set AllCacheRegs( void )
/*************************************/
{
    return( HW_EMPTY );
}


extern  hw_reg_set *GPRegs( void )
/********************************/
{
    return( GeneralRegs );
}


extern  hw_reg_set *FPRegs( void )
/********************************/
{
    return( FloatRegs );
}


extern  hw_reg_set *IdxRegs( void )
/*********************************/
{
    return( DWordRegs );
}


extern  void InitRegTbl( void )
/*****************************/
{
}


extern  byte    RegTrans( hw_reg_set reg )
/****************************************/
{
    int                 i;

    /*
     * This should be cached in the reg name and used instead of a stupid lookup
     */
    for( i = 0; i < sizeof( GeneralRegs ) / sizeof( GeneralRegs[0] ); i++ ) {
        if( HW_Subset( GeneralRegs[i], reg ) ) return( i );
    }
    for( i = 0; i < sizeof( QWordRegs ) / sizeof( QWordRegs[0] ); i++ ) {
        if( HW_Subset( QWordRegs[i], reg ) ) return( i * 2 + 2 );
    }
    for( i = 0; i < sizeof( FloatRegs ) / sizeof( FloatRegs[0] ); i++ ) {
        if( HW_Equal( reg, FloatRegs[i] ) ) return( i );
    }
    return( 0 );
}


extern  mips_regn RegTransN( name *reg_name )
/********************************************
 * Translate reg name to enum name
 */
{
    hw_reg_set reg;
    int       i;
    reg = reg_name->r.reg;

    for( i = 0; i < sizeof( GeneralRegs ) / sizeof( GeneralRegs[0] ); i++ ) {
        if( HW_Subset( GeneralRegs[i], reg ) ) return( i + MIPS_REGN_r0 );
    }
    for( i = 0; i < sizeof( FloatRegs ) / sizeof( FloatRegs[0] ); i++ ) {
        if( HW_Equal( reg, FloatRegs[i] ) ) return( i + MIPS_REGN_f0 );
    }
    _Zoiks( ZOIKS_031 );
    return( MIPS_REGN_END );
}


extern  void SetArchIndex( name *new_r, hw_reg_set regs )
{
    new_r->r.arch_index = RegTrans( regs );
}
