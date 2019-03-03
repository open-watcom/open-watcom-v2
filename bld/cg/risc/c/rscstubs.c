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
* Description:  Stub functions for RISC codegens for functionality that
*               cannot be or isn't implemented.
*
****************************************************************************/


#include "_cgstd.h"
#include "coderep.h"
#include "addrname.h"
#include "procdef.h"
#include "regset.h"
#include "zoiks.h"
#include "fpu.h"
#include "objout.h"
#include "encode.h"
#include "object.h"
#include "index.h"
#include "fixindex.h"
#include "cgaux.h"
#include "bgcall.h"
#include "generate.h"
#include "split.h"
#include "makeaddr.h"
#include "expand.h"
#include "confldef.h"
#include "regalloc.h"


/* KLUDGEY crud for segment register */

void    OptSegs( void )
/*********************/
{
}

void AddSegment( instruction *ins )
/*********************************/
{
    /* unused parameters */ (void)ins;
}

void MergeIndex( void )
/*********************/
{
}

/* KLUDGEY crud for the 8087 */


bool FPInsIntroduced( instruction *ins )
/**************************************/
{
    /* unused parameters */ (void)ins;

    return( false );
}

bool FPStackReg( name *n )
/************************/
{
    /* unused parameters */ (void)n;

    return( false );
}

bool FPStackIns( instruction *ins )
/*********************************/
{
    /* unused parameters */ (void)ins;

    return( false );
}

bool FPFreeIns( instruction *ins )
/********************************/
{
    /* unused parameters */ (void)ins;

    return( false );
}

void FPRegAlloc( void )
/*********************/
{
}

void FPParms( void )
/******************/
{
}

#if 0
void FPReloc( void )
/******************/
{
}
#endif

bool FPSideEffect( instruction *ins )
/***********************************/
{
    /* unused parameters */ (void)ins;

    return( false );
}

void FPSetStack( name *name )
/***************************/
{
    /* unused parameters */ (void)name;
}

type_class_def FPInsClass( instruction *ins )
/*******************************************/
{
    /* unused parameters */ (void)ins;

    return( XX );
}

bool FPIsConvert( instruction *ins )
/**********************************/
{
    /* unused parameters */ (void)ins;

    return( false );
}

void FPNotStack( name *name )
/***************************/
{
    /* unused parameters */ (void)name;
}

bool FPIsStack( name *name )
/**************************/
{
    /* unused parameters */ (void)name;

    return( false );
}

bool FPStackOp( name *name )
/**************************/
{
    /* unused parameters */ (void)name;

    return( false );
}

int FPStackExit( block *blk )
/***************************/
{
    /* unused parameters */ (void)blk;

    return( 0 );
}

int FPStkOver( instruction *ins, int depth )
/******************************************/
{
    /* unused parameters */ (void)ins; (void)depth;

    return( 0 );
}

void FPCalcStk( instruction *ins, int *depth )
/********************************************/
{
    /* unused parameters */ (void)ins;

    *depth = 0;
}

void FPPreSched( block *blk )
/***************************/
{
    /* unused parameters */ (void)blk;
}

void FPPostSched( block *blk )
/****************************/
{
    /* unused parameters */ (void)blk;
}

void FPExpand( void )
/*******************/
{
}

void FPOptimize( void )
/*********************/
{
}

void FPInit( void )
/*****************/
{
}

void FPPushParms( pn parm, call_state *state )
/********************************************/
{
    /* unused parameters */ (void)parm; (void)state;
}


void InitZeroPage( void )
/***********************/
{
}


void FiniZeroPage( void )
/***********************/
{
}

void InitSegment( void )
/**********************/
{
}


void FiniSegment( void )
/**********************/
{
}


cg_type NamePtrType( name *op )
/*****************************/
{
    /* unused parameters */ (void)op;

    return( TY_NEAR_POINTER );
}

#if 0
int AdjustBase( void )
/********************/
{
    return( 0 );
}

void ZeroMoves( void )
/********************/
{
}
#endif

#if (_TARGET & _TARG_MIPS) == 0
/* Functions from 386setcc.c; MIPS has its own mipssetc.c, Alpha could
 * have own version as well. Not sure about PowerPC.
 */
bool SetOnCondition( void )
/*************************/
{
    return( false );
}

reg_set_index SpecialPossible( instruction *ins )
/***********************************************/
{
    /* unused parameters */ (void)ins;

    return( RL_ );
}
#endif

void BuildIndex( void )
{
}

bool LdStAlloc( void )
/********************/
{
    return( false );
}

void LdStCompress( void )
/***********************/
{
}

bool FreeObjCache( void )
/***********************/
{
    return( false );
}

void PreCall( cn call )
/*********************/
{
    /* unused parameters */ (void)call;
}

void PostCall( cn call )
/**********************/
{
    /* unused parameters */ (void)call;
}

void PushInSameBlock( instruction *ins )
/**************************************/
{
    /* unused parameters */ (void)ins;
}

void CheckCC( instruction *ins, instruction *new_ins )
/****************************************************/
{
    /* unused parameters */ (void)ins; (void)new_ins;
}

bool FPDivIsADog( type_class_def type_class )
/*******************************************/
{
    return( _IsFloating( type_class ) );
}

bool AskSegIsNear( segment_id segid )
/***********************************/
{
    /* unused parameters */ (void)segid;

    return( true );
}

void MemtoBaseTemp( void )
/************************/
{
}

void FixSegments( void )
/**********************/
{
}

void FixMemBases( void )
/**********************/
{
}

void FixFPConsts( instruction *ins )
/**********************************/
{
    /* unused parameters */ (void)ins;
}

bool LoadAToMove( instruction *ins )
/**********************************/
{
    /* unused parameters */ (void)ins;

    return( false );
}

void FixCallIns( instruction *ins )
/*********************************/
{
    /* unused parameters */ (void)ins;
}

void GenSetCC( instruction *ins )
/*******************************/
{
    /* unused parameters */ (void)ins;

    _Zoiks( 102 );
}

bool IsUncacheableMemory( name *opnd )
/************************************/
{
    /* unused parameters */ (void)opnd;

    return( false );
}
