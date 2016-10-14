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


#include "cgstd.h"
#include "coderep.h"
#include "addrname.h"
#include "procdef.h"
#include "regset.h"
#include "zoiks.h"
#include "x87.h"
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
#include "conflict.h"
#include "regalloc.h"


/* KLUDGEY crud for segment register */

void    OptSegs() {
/*************************/
}

void AddSegment( instruction *ins ) {
/******************************************/
    ins = ins;
}

void MergeIndex() {
/**********************************************/
}

/* KLUDGEY crud for the 8087 */


bool            FPInsIntroduced( instruction *ins ) {
/***********************************************************/

    ins = ins;
    return( false );
}

bool            FPStackReg( name *n ) {
/*********************************************/

    n = n;
    return( false );
}

bool            FPStackIns( instruction *ins ) {
/******************************************************/

    ins = ins;
    return( false );
}

bool            FPFreeIns( instruction *ins ) {
/*****************************************************/

    ins = ins;
    return( false );
}

void             FPRegAlloc(void) {
/****************************************/
}

void             FPParms(void) {
/*************************************/
}

#if 0
void             FPReloc(void) {
/*************************************/
}
#endif

bool    FPSideEffect( instruction *ins ) {
/************************************************/
    ins = ins;
    return( false );
}

void    FPSetStack( name *name ) {
/****************************************/
    name = name;
}

type_class_def FPInsClass( instruction *ins ) {
/*****************************************************/
    ins = ins;
    return( XX );
}

bool    FPIsConvert( instruction *ins ) {
/***********************************************/
    ins = ins;
    return( false );
}

void    FPNotStack( name *name ) {
/****************************************/
    name = name;
}

bool    FPIsStack( name *name ) {
/***************************************/

    name = name;
    return( false );
}

bool    FPStackOp( name *name ) {
/***************************************/

    name = name;
    return( false );
}

int     FPStackExit( block *blk ) {
/*****************************************/
    blk = blk;
    return( 0 );
}

int     FPStkOver( instruction *ins, int depth ) {
/********************************************************/
    ins = ins;
    depth = depth;
    return( 0 );
}

void    FPCalcStk( instruction *ins, int *depth ) {
/*********************************************************/
    ins = ins;
    *depth = 0;
}

void    FPPreSched( block *blk ) {
/****************************************/
    blk = blk;
}

void    FPPostSched( block *blk ) {
/*****************************************/
    blk = blk;
}

void    FPExpand() {
/**************************/
}

void    FPOptimize() {
/****************************/
}

void    InitFP() {
/************************/
}

void     FPPushParms( pn parm, call_state *state ) {
/*********************************************************/
    parm = parm;
    state = state;
}


void    InitZeroPage( void )
/**************************/
{
}


void    FiniZeroPage( void )
/**************************/
{
}

void InitSegment() {
/*****************************/
}


void FiniSegment() {
/*****************************/
}


cg_type NamePtrType( name *op ) {
/*************************************/

    op = op;
    return( TY_NEAR_POINTER );
}

#if 0
int     AdjustBase() {
/****************************/

    return( 0 );
}

void    ZeroMoves() {
/***************************/

}
#endif

#if (_TARGET & _TARG_MIPS) == 0
/* Functions from 386setcc.c; MIPS has its own mipssetc.c, Alpha could
 * have own version as well. Not sure about PowerPC.
 */
bool    SetOnCondition() {
/********************************/

    return( false );
}

reg_set_index   SpecialPossible( instruction *ins ) {
/***********************************************************/
    ins = ins;
    return( RL_ );
}
#endif

void    BuildIndex() {

}

bool    LdStAlloc( void )
/***********************/
{
    return( false );
}

void    LdStCompress( void ) {
/******************************/
}

bool    FreeObjCache() {
/******************************/

    return( false );
}

void    PreCall( cn call ) {
/**********************************/
    call = call;
}

void    PostCall( cn call ) {
/***********************************/
    call = call;
}

void    PushInSameBlock( instruction *ins ) {
/***************************************************/
    ins = ins;
}

void    CheckCC( instruction *ins, instruction *new_ins ) {
/*****************************************************************/
    new_ins = new_ins;
    ins = ins;
}

bool    DivIsADog( type_class_def class ) {
/*************************************************/

    return( _IsFloating( class ) );
}

bool    AskSegNear( segment_id id ) {
/*******************************************/

    id = id;
    return( true );
}

void    MemtoBaseTemp() {
/*******************************/
}

void    FixSegments() {
/*****************************/
}

void    FixMemBases() {
/*****************************/
}

void    FixFPConsts( instruction *ins ) {
/***********************************************/

    ins = ins;
}

bool    LoadAToMove( instruction *ins ) {
/***********************************************/

    ins = ins;
    return( false );
}

void    FixCallIns( instruction *ins ) {
/**********************************************/

    ins = ins;
}

void    GenSetCC( instruction *ins ) {
/************************************/

    ins = ins;
    _Zoiks( 102 );
}

bool    IsUncacheableMemory( name *opnd )
/***************************************/
{
    opnd = opnd;
    return( false );
}
