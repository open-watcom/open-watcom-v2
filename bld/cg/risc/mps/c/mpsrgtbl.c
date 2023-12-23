/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "_cgstd.h"
#include "coderep.h"
#include "mpsenc.h"
#include "zoiks.h"
#include "data.h"
#include "rgtbl.h"


#define _Combine( a, b )    ((a) * RL_NUMBER_OF_SETS + (b))

/*
 * register, register class and range definitions
 */
#define pick_item(id)   ARCH_IDX_ ## id
#define pick_start(id)  ARCH_ ## id ## _START
#define pick_end(id)    ARCH_ ## id ## _END

enum {
    #define pick(id,idx,cls) pick_item(id),
    #include "regindex.h"
    #undef pick
    pick_item( END ),
    pick_item( START ) = 0
};

enum {
    #define MAPREGCLASS
    #include "regindex.h"
    #undef MAPREGCLASS
};

#undef pick_end
#undef pick_start
#undef pick_item

const arch_reg_info RegsTab[] = {
    #define pick(id,idx,cls) { HW_D_1( HW_ ## id ), idx, cls ## _IDX, DW_REG_ ## id },
    #include "regindex.h"
    #undef pick
};

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
 * and relating code in ParmReg and CallState.
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
    HW_D_1( HW_RT_RET_REG64 ),
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
    HW_D_1( HW_RT_RET_REG32 ),
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
    HW_D_1( HW_RT_RET_REGFP ),
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


#if 0
static int findArchRegIndex( hw_reg_set regs, reg_cls cls )
{
    int         i;

    if( cls & GPR_IDX ) {
        for( i = ARCH_GPR_START; i < ARCH_GPR_END; i++ ) {
            if( HW_Equal( regs, RegsTab[i].hw_reg ) ) {
                return( i );
            }
        }
    }
    if( cls & FPR_IDX ) {
        for( i = ARCH_FPR_START; i < ARCH_FPR_END; i++ ) {
            if( HW_Equal( regs, RegsTab[i].hw_reg ) ) {
                return( i );
            }
        }
    }
    return( -1 );
}

hw_reg_set GetArchReg( int idx, reg_cls cls )
{
    if( cls & GPR_IDX ) {
        return( RegsTab[ARCH_GPR_START + idx].hw_reg );
    }
    if( cls & FPR_IDX ) {
        return( RegsTab[ARCH_FPR_START + idx].hw_reg );
    }
    return( HW_EMPTY );
}
#endif

/*
 *  Information for register set intersections
 *  if sets are of different classes {
 *      intersection is empty
 *  } else {
 *      intersection given in square matrix for class
 *  }
 */

reg_set_index RegIntersect( reg_set_index s1, reg_set_index s2 )
/**************************************************************/
{
    if( s1 == s2 )
        return( s1 );
    switch( _Combine( s1, s2 ) ) {
    default:
        if( s1 == RL_NUMBER_OF_SETS )
            return( s2 );
        if( s2 == RL_NUMBER_OF_SETS )
            return( s1 );
        return( RL_ );
    }
}


hw_reg_set InLineParm( hw_reg_set regs, hw_reg_set used )
/*******************************************************/
{
    /* unused parameters */ (void)used;

    return( regs );
}


hw_reg_set *ParmChoices( type_class_def type_class )
/**************************************************/
{
    switch( type_class ) {
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


hw_reg_set ReturnReg( type_class_def type_class )
/***********************************************/
{
    switch( type_class ) {
    case FS:
    case FD:
    case FL:
        return( HW_RT_RET_REGFP );
    case XX:
        return( HW_EMPTY );
    case U1:
    case I1:
        return( HW_RT_RET_REG8 );
    case U2:
    case I2:
        return( HW_RT_RET_REG16 );
    case U4:
    case I4:
    default:
        return( HW_RT_RET_REG32 );
    case U8:
    case I8:
        return( HW_RT_RET_REG64 );
    }
}


hw_reg_set *ParmRegs( void )
/**************************/
{
    return( &AllParmRegs[0] );
}


hw_reg_set ParmRegConflicts( hw_reg_set regs )
/********************************************/
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


reg_set_index SegIndex( void )
/****************************/
{
    return( RL_ );
}


reg_set_index NoSegments( reg_set_index regs_idx )
/************************************************/
{
    return( regs_idx );
}


reg_set_index IndexIntersect( reg_set_index curr,
                                      type_class_def type_class,
                                      bool is_temp_index )
/*********************************************************/
{
    /* unused parameters */ (void)curr; (void)type_class; (void)is_temp_index;

    return( RL_DWORD );
}


bool IsIndexReg( hw_reg_set reg, type_class_def type_class,
                         bool is_temp_index )
/************************************************************/
{
    hw_reg_set          *dregs;

    /* unused parameters */ (void)type_class; (void)is_temp_index;

    for( dregs = &DWordRegs[0]; !HW_CEqual( *dregs, HW_EMPTY ); ++dregs ) {
        if( HW_Equal( *dregs, reg ) ) {
            return( true );
        }
    }
    return( false );
}


bool IndexRegOk( hw_reg_set reg, bool is_temp_index )
/***************************************************/
{
    /* unused parameters */ (void)reg; (void)is_temp_index;

    return( false );
}


bool IsSegReg( hw_reg_set regs )
/******************************/
{
    /* unused parameters */ (void)regs;

    return( false );
}


type_class_def RegClass( hw_reg_set regs )
/****************************************/
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


hw_reg_set Low16Reg( hw_reg_set regs )
/************************************/
{
    /* unused parameters */ (void)regs;

    return( HW_EMPTY );
}


hw_reg_set High16Reg( hw_reg_set regs )
/*************************************/
{
    /* unused parameters */ (void)regs;

    return( HW_EMPTY );
}


hw_reg_set Low32Reg( hw_reg_set regs )
/************************************/
{
    /* unused parameters */ (void)regs;

    return( HW_EMPTY );
}


hw_reg_set High32Reg( hw_reg_set regs )
/*************************************/
{
    /* unused parameters */ (void)regs;

    return( HW_EMPTY );
}


hw_reg_set High48Reg( hw_reg_set regs )
/*************************************/
{
    /* unused parameters */ (void)regs;

    return( HW_EMPTY );
}


hw_reg_set Low48Reg( hw_reg_set regs )
/************************************/
{
    /* unused parameters */ (void)regs;

    return( HW_EMPTY );
}


hw_reg_set Low64Reg( hw_reg_set regs )
/*************************************
 * return the low order part of 64 bit register "regs"
 */
{
    hw_reg_set  low;
    hw_reg_set  *order;

    if( HW_CEqual( regs, HW_EMPTY ) )
        return( HW_EMPTY );
    for( order = Reg64Order; ; ++order ) {
        if( HW_Ovlap( *order, regs ) ) {
            break;
        }
    }
    low = regs;
    HW_OnlyOn( low, *order );
    if( HW_Equal( low, regs ) ) {
        low = HW_EMPTY;
    }
    return( low );
}


hw_reg_set High64Reg( hw_reg_set regs )
/**************************************
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


hw_reg_set HighReg( hw_reg_set regs )
/***********************************/
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


hw_reg_set LowReg( hw_reg_set regs )
/**********************************/
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


hw_reg_set FullReg( hw_reg_set regs )
/***********************************/
{
    return( regs );
}


bool IsRegClass( hw_reg_set regs, type_class_def type_class )
/***********************************************************/
{
    hw_reg_set  *list;

    for( list = RegSets[IsSets[type_class]]; !HW_CEqual( *list, HW_EMPTY ); ++list ) {
        if( HW_Equal( *list, regs ) ) {
            return( true );
        }
    }
    return( false );
}


reg_set_index UsualPossible( type_class_def type_class )
/******************************************************/
{
    return( IsSets[type_class] );
}


hw_reg_set  ActualParmReg( hw_reg_set reg )
/*****************************************/
{
    return( reg );
}


hw_reg_set FixedRegs( void )
/**************************/
{
    /* MJC do you really want to fix them */
    hw_reg_set          fixed;

    HW_CAsgn( fixed, HW_ZERO_REG );     // $zero
    HW_CTurnOn( fixed, HW_AT_REG );     // $at (needed as scratch register)
    HW_CTurnOn( fixed, HW_GP_REG );     // $gp
    HW_CTurnOn( fixed, HW_SP_REG );     // $sp
    // We should be able to use $fp as $s8 in theory, but that isn't working
    // when alloca() is used for some reason (alloca() will force us to use
    // a frame pointer separate from $sp). So just make $fp always fixed.
//    if( CurrProc->targ.base_is_fp )
    HW_CTurnOn( fixed, HW_FP_REG );     // $fp
    return( fixed );
}


hw_reg_set VarargsHomePtr( void )
/*******************************/
{
    return( HW_VARARGS_REG );
}


hw_reg_set StackReg( void )
/*************************/
{
    /* should be up to linkage conventions? */
    return( HW_SP_REG );
}


hw_reg_set FrameBaseReg( void )
/*****************************/
{
    return( HW_FP_REG );
}


hw_reg_set FrameReg( void )
/*************************/
{
    /* should be up to linkage conventions? */
    if( CurrProc->targ.base_is_fp ) {
        return( HW_FP_REG );
    }
    return( HW_SP_REG );
}


hw_reg_set ScratchReg( void )
/***************************/
{
    return( HW_AT_REG );
}


hw_reg_set ReturnAddrReg( void )
/******************************/
{
    /* BBB Like MJC said ^ */
    return( HW_RA_REG );
}


hw_reg_set DisplayReg( void )
/***************************/
{
    return( HW_EMPTY );
}


int SizeDisplayReg( void )
/************************/
{
    return( 0 );
}


hw_reg_set AllCacheRegs( void )
/*****************************/
{
    return( HW_EMPTY );
}


hw_reg_set *GPRegs( void )
/************************/
{
    return( GeneralRegs );
}


hw_reg_set *FPRegs( void )
/************************/
{
    return( FloatRegs );
}


hw_reg_set *IdxRegs( void )
/*************************/
{
    return( DWordRegs );
}


void InitRegTbl( void )
/*********************/
{
}


reg_idx RegTrans( hw_reg_set regs )
/**********************************
 * Translate reg to register index
 */
{
    int         i;

    for( i = ARCH_IDX_START; i < ARCH_IDX_END; i++ ) {
        if( HW_Equal( regs, RegsTab[i].hw_reg ) ) {
            return( RegsTab[i].idx );
        }
    }
    return( 0 );
}

int GetArchIndex( hw_reg_set regs )
/*********************************/
{
    int         i;

    for( i = ARCH_IDX_START; i < ARCH_IDX_END; i++ ) {
        if( HW_Equal( regs, RegsTab[i].hw_reg ) ) {
            return( RegsTab[i].idx );
        }
    }
    return( 0 );
}


dw_regs RegTransDW( hw_reg_set reg )
/***********************************
 * Translate reg to Dwarf enum name
 */
{
    int         i;

    for( i = ARCH_IDX_START; i < ARCH_IDX_END; i++ ) {
        if( HW_Equal( reg, RegsTab[i].hw_reg ) ) {
            return( RegsTab[i].dw_idx );
        }
    }
    _Zoiks( ZOIKS_031 );
    return( DW_REG_END );
}

hw_reg_set      FirstReg( reg_set_index regs_idx )
/*************************************************
    The table RTInfo[] uses reg_set_indexes instead of hw_reg_sets since
    they are only one byte long.  This retrieves the first hw_reg_set
    from the reg_set table "index".

    the tables above use RL_ consts rather that hw_reg_sets cause
    it cheaper. This just picks off the first register from a
    register list and returns it.
*/
{
    return( *RegSets[regs_idx] );
}
