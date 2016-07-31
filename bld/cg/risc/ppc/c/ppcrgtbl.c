/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  PowerPC register usage and linkage conventions.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "ppcregn.h"
#include "zoiks.h"
#include "data.h"
#include "rgtbl.h"


static  hw_reg_set      Empty[] = {
    EMPTY
};

static  hw_reg_set      Reg64Order[] = {
    HW_D_1( HW_D3 ),
    HW_D_1( HW_D4 ),
    HW_D_1( HW_D5 ),
    HW_D_1( HW_D6 ),
    HW_D_1( HW_D7 ),
    HW_D_1( HW_D8 ),
    HW_D_1( HW_D9 ),
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

static  hw_reg_set      QWordRegs[] = {
    HW_D_1( HW_Q3 ),
    HW_D_1( HW_Q5 ),
    HW_D_1( HW_Q7 ),
    HW_D_1( HW_Q9 ),
    HW_D_1( HW_Q15 ),
    HW_D_1( HW_Q17 ),
    HW_D_1( HW_Q19 ),
    HW_D_1( HW_Q21 ),
    HW_D_1( HW_Q23 ),
    HW_D_1( HW_Q25 ),
    HW_D_1( HW_Q27 ),
    HW_D_1( HW_Q29 ),
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
    HW_D_1( HW_R3 ),
    HW_D_1( HW_R4 ),
    HW_D_1( HW_R5 ),
    HW_D_1( HW_R6 ),
    HW_D_1( HW_R7 ),
    HW_D_1( HW_R8 ),
    HW_D_1( HW_R9 ),
    HW_D_1( HW_R10 ),
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
    HW_D_1( HW_EMPTY )
};


static  hw_reg_set      Parm8Regs[] = {
    HW_D_1( HW_Q3 ),
    HW_D_1( HW_Q5 ),
    HW_D_1( HW_Q7 ),
    HW_D_1( HW_Q9 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      Parm8Regs2[] = {
    HW_D_1( HW_Q5 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      Return8[] = {
    HW_D_1( HW_Q3 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      Parm4Regs[] = {
    HW_D_1( HW_D3 ),
    HW_D_1( HW_D4 ),
    HW_D_1( HW_D5 ),
    HW_D_1( HW_D6 ),
    HW_D_1( HW_D7 ),
    HW_D_1( HW_D8 ),
    HW_D_1( HW_D9 ),
    HW_D_1( HW_D10 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      Parm4Regs2[] = {
    HW_D_1( HW_D4 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      Parm4Regs3[] = {
    HW_D_1( HW_D5 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      Return4[] = {
    HW_D_1( HW_D3 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      ParmFRegs[] = {
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
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      ReturnD[] = {
    HW_D_1( HW_F1 ),
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
    RL_BYTE,                /* U1*/
    RL_BYTE,                /* I1*/
    RL_WORD,                /* U2*/
    RL_WORD,                /* I2*/
    RL_DWORD,               /* U4*/
    RL_DWORD,               /* I4*/
    RL_QWORD,               /* U8*/
    RL_QWORD,               /* I8*/
    RL_DWORD,               /* CP*/
    RL_DWORD,               /* PT*/
    RL_FLOAT,               /* FS*/
    RL_FLOAT,               /* FD*/
    RL_,                    /* FL*/
    RL_                     /* XX*/
};

/*      Information for register set intersections
 *      if sets are of different classes {
 *          intersection is empty
 *      } else {
 *          intersection given in square matrix for class
 *      }
 */

#define _Combine( a, b )        ((a) * RL_NUMBER_OF_SETS + (b))

reg_set_index   RegIntersect( reg_set_index s1, reg_set_index s2 )
/****************************************************************/
{
    if( s1 == s2 )
        return( s1 );
    if( s1 == RL_NUMBER_OF_SETS )
        return( s2 );
    if( s2 == RL_NUMBER_OF_SETS )
        return( s1 );
    return( RL_ );
}

hw_reg_set      InLineParm( hw_reg_set regs, hw_reg_set used )
/************************************************************/
{
    regs = regs; used = used;
    return( HW_EMPTY );
}


hw_reg_set      *ParmChoices( type_class_def class )
/**************************************************/
{
    switch( class ) {
    case I4:
    case U4:
    case CP:
    case PT:
        return( RegSets[ RL_PARM_4 ] );
    case I8:
    case U8:
    case XX:
        return( RegSets[ RL_PARM_8 ] );
    case FS:
    case FD:
    case FL:
        return( RegSets[ RL_PARM_F ] );
    default:
        _Zoiks( ZOIKS_118 );
        return( NULL );
    }
}


hw_reg_set      ReturnReg( type_class_def class )
/***********************************************/
{
    switch( class ) {
    case FS:
    case FD:
    case FL:
        return( HW_F1 );
    case XX:
        return( HW_EMPTY );
    case U4:
    case I4:
        return( HW_D3 );
    case U8:
    case I8:
        return( HW_Q3 );
    default:
        return( HW_D3 );
    }
}

hw_reg_set      *ParmRegs( void )
/*******************************/
{
    return( &AllParmRegs[0] );
}


type_class_def  RegClass( hw_reg_set regs )
/*****************************************/
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
    return( U4 );
}


reg_set_index   SegIndex( void )
/******************************/
{
    return( RL_  );
}


reg_set_index   NoSegments( reg_set_index idx )
/*********************************************/
{
    return( idx );
}


reg_set_index   IndexIntersect( reg_set_index curr,
                                       type_class_def class,
                                       bool is_temp_index )
/***********************************************************/
{
    curr = curr; class = class;
    is_temp_index = is_temp_index;
    return( RL_DWORD );
}


bool    IsIndexReg( hw_reg_set reg, type_class_def class,
                            bool is_temp_index )
/**********************************************/
{
    hw_reg_set          *dregs;

    is_temp_index = is_temp_index;
    class = class;
    for( dregs = &DWordRegs[0]; !HW_CEqual( *dregs, HW_EMPTY ); ++dregs ) {
        if( HW_Equal( *dregs, reg ) ) {
            return( true );
        }
    }
    return( false );
}


bool    IndexRegOk( hw_reg_set reg, bool is_temp_index )
/******************************************************/
{
    is_temp_index = is_temp_index;
    reg = reg;
    return( false );
}


bool    IsSegReg( hw_reg_set regs )
/*********************************/
{
    regs = regs;
    return( false );
}


hw_reg_set      Low16Reg( hw_reg_set regs )
/*****************************************/
{
    regs = regs;
    return( HW_EMPTY );
}


hw_reg_set      High16Reg( hw_reg_set regs )
/******************************************/
{
    regs = regs;
    return( HW_EMPTY );
}


hw_reg_set      Low32Reg( hw_reg_set regs )
/*****************************************/
{
    regs = regs;
    return( HW_EMPTY );
}


hw_reg_set      High32Reg( hw_reg_set regs )
/******************************************/
{
    regs = regs;
    return( HW_EMPTY );
}


hw_reg_set      High48Reg( hw_reg_set regs )
/******************************************/
{
    regs = regs;
    return( HW_EMPTY );
}


hw_reg_set      Low48Reg( hw_reg_set regs )
/*****************************************/
{
    regs = regs;
    return( HW_EMPTY );
}


hw_reg_set      Low64Reg( hw_reg_set regs )
/******************************************
 * return the low order part of 64 bit register "regs"
 */
{
    hw_reg_set  low;
    hw_reg_set  *order;

    if( HW_CEqual( regs, HW_EMPTY ) )
        return( HW_EMPTY );
    order = Reg64Order;
    for( ;; ) {
        if( HW_Ovlap( *order, regs ) )
            break;
        ++order;
    }
    low = regs;
    HW_OnlyOn( low, *order );
    if( HW_Equal( low, regs ) ) {
        low = HW_EMPTY;
    }
    return( low );
}


hw_reg_set      High64Reg( hw_reg_set regs )
/*******************************************
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


hw_reg_set      HighReg( hw_reg_set regs )
/****************************************/
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


hw_reg_set      LowReg( hw_reg_set regs )
/***************************************/
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


hw_reg_set      FullReg( hw_reg_set regs )
/****************************************/
{
    return( regs );
}


bool    IsRegClass( hw_reg_set regs, type_class_def class )
/*********************************************************/
{
    hw_reg_set  *list;

    for( list = RegSets[IsSets[class]]; !HW_CEqual( *list, HW_EMPTY ); ++list ) {
        if( HW_Equal( *list, regs ) ) {
            return( true );
        }
    }
    return( false );
}


reg_set_index   UsualPossible( type_class_def class )
/***************************************************/
{
    return( IsSets[ class ] );
}


hw_reg_set      ActualParmReg( hw_reg_set reg )
/***********************************************/
{
    return( reg );
}

hw_reg_set      FixedRegs( void )
/*******************************/
/* MJC do you really want to fix them */
{
    hw_reg_set          fixed;

    HW_CAsgn( fixed, HW_R0 );
    HW_CTurnOn( fixed, HW_R1 );         // stack
    HW_CTurnOn( fixed, HW_R2 );         // toc
    HW_CTurnOn( fixed, HW_R13 );        // must not be used (unlucky)
    HW_CTurnOn( fixed, HW_R31 );        // frame reg
    return( fixed );
}

hw_reg_set      VarargsHomePtr( void )
/************************************/
{
    return( HW_D14 );
}

hw_reg_set      StackReg( void )
/******************************/
/* MJC should be up to linkage conventions */
{
    return( HW_D1 );
}

hw_reg_set      FrameReg( void )
/******************************/
/* MJC should be up to linkage conventions */
{
    if( CurrProc->targ.base_is_fp ) {
        return( HW_D31 );
    }
    return( HW_D1 );
}

hw_reg_set      TocReg( void )
/****************************/
{
    return( HW_D2 );
}

hw_reg_set      FrameBaseReg( void )
/**********************************/
{
    return( HW_D31 );
}

hw_reg_set      ScratchReg( void )
/********************************/
{
    return( HW_D12 );
}

hw_reg_set      ReturnAddrReg( void )
/***********************************/
/* BBB Like MJC said ^ */
{
    return( HW_EMPTY );
}


hw_reg_set      DisplayReg( void )
/********************************/
{
    return( HW_EMPTY );
}


int     SizeDisplayReg( void )
/****************************/
{
    return( 0 );
}


hw_reg_set      AllCacheRegs( void )
/**********************************/
{
    return( HW_EMPTY );
}

hw_reg_set      *GPRegs( void )
/*****************************/
{
    return( DWordRegs );
}

hw_reg_set      *FPRegs( void )
/*****************************/
{
    return( FloatRegs );
}

hw_reg_set      *IdxRegs( void )
/******************************/
{
    return( DWordRegs );
}

void            InitRegTbl( void )
/********************************/
{
}

byte    RegTrans( hw_reg_set reg )
/********************************/
{
    int                 i;

    for( i = 0; i < sizeof( DWordRegs ) / sizeof( DWordRegs[0] ); i++ ) {
        if( HW_Subset( DWordRegs[i], reg ) ) return( i );
    }
    for( i = 0; i < sizeof( FloatRegs ) / sizeof( FloatRegs[0] ); i++ ) {
        if( HW_Equal( reg, FloatRegs[i] ) ) return( i );
    }
    return( 0 );
}

ppc_regn RegTransN( name *reg_name )
/**********************************/
/***** Translate reg name to enum name ****/
{
    hw_reg_set reg;
    int       i;

    reg = reg_name->r.reg;

    for( i = 0; i < sizeof( DWordRegs ) / sizeof( DWordRegs[0] ); i++ ) {
        if( HW_Subset( DWordRegs[i], reg ) )
            return( i + PPC_REGN_r0 );
    }
    for( i = 0; i < sizeof( FloatRegs ) / sizeof( FloatRegs[0] ); i++ ) {
        if( HW_Equal( reg, FloatRegs[i] ) )
            return( i + PPC_REGN_f0 );
    }
    _Zoiks( ZOIKS_031 );
    return( PPC_REGN_END );
}

hw_reg_set ParmRegConflicts( hw_reg_set r )
/*****************************************/
{
    return( r );
}


void SetArchIndex( name *new_r, hw_reg_set regs )
{
    new_r->r.arch_index = RegTrans( regs );
}


hw_reg_set      FirstReg( reg_set_index index )
/**********************************************
    The table RTInfo[] uses reg_set_indexes instead of hw_reg_sets since
    they are only one byte long.  This retrieves the first hw_reg_set
    from the reg_set table "index".

    the tables above use RL_ consts rather that hw_reg_sets cause
    it cheaper. This just picks off the first register from a
    register list and returns it.
*/
{
    return( *RegSets[index] );
}
