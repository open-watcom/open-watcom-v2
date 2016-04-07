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


/* KLUDGEY crud for segment register */

extern  void    OptSegs() {
/*************************/
}

extern void AddSegment( instruction *ins ) {
/******************************************/
    ins = ins;
}

extern  void    MoveSegRes( instruction *ins, instruction *new_ins ) {
/********************************************************************/
    new_ins = new_ins;
    ins = ins;
}

extern  void    MoveSegOp(instruction* ins,instruction* new_ins,int i) {
/**********************************************************************/
    new_ins = new_ins;
    i = i;
    ins = ins;
}

extern  void    DelSegRes( instruction *ins ) {
/**********************************************/
    ins = ins;
}

extern  void    DelSegOp( instruction *ins, int i ) {
/**********************************************/
    i = i;
    ins = ins;
}

extern  void    DupSeg( instruction *ins, instruction *new_ins ) {
/**********************************************/
    new_ins = new_ins;
    ins = ins;
}

extern  void    DupSegRes( instruction *ins, instruction *new_ins ) {
/*******************************************************************/
    new_ins = new_ins;
    ins = ins;
}

extern void MergeIndex() {
/**********************************************/
}

/* KLUDGEY crud for the 8087 */


extern  bool            FPInsIntroduced( instruction *ins ) {
/***********************************************************/

    ins = ins;
    return( FALSE );
}

extern  bool            FPStackReg( name *n ) {
/*********************************************/

    n = n;
    return( FALSE );
}

extern  bool            FPStackIns( instruction *ins ) {
/******************************************************/

    ins = ins;
    return( FALSE );
}

extern  bool            FPFreeIns( instruction *ins ) {
/*****************************************************/

    ins = ins;
    return( FALSE );
}

extern void             FPRegAlloc(void) {
/****************************************/
}

extern void             FPParms(void) {
/*************************************/
}

extern void             FPReloc(void) {
/*************************************/
}

extern  bool    FPSideEffect( instruction *ins ) {
/************************************************/
    ins = ins;
    return( FALSE );
}

extern  void    FPSetStack( name *name ) {
/****************************************/
    name = name;
}

extern  type_class_def FPInsClass( instruction *ins ) {
/*****************************************************/
    ins = ins;
    return( XX );
}

extern  bool    FPIsConvert( instruction *ins ) {
/***********************************************/
    ins = ins;
    return( FALSE );
}

extern  void    FPNotStack( name *name ) {
/****************************************/
    name = name;
}

extern  bool    FPIsStack( name *name ) {
/***************************************/

    name = name;
    return( FALSE );
}

extern  bool    FPStackOp( name *name ) {
/***************************************/

    name = name;
    return( FALSE );
}

extern  int     FPStackExit( block *blk ) {
/*****************************************/
    blk = blk;
    return( 0 );
}

extern  int     FPStkOver( instruction *ins, int depth ) {
/********************************************************/
    ins = ins;
    depth = depth;
    return( 0 );
}

extern  void    FPCalcStk( instruction *ins, int *depth ) {
/*********************************************************/
    ins = ins;
    *depth = 0;
}

extern  void    FPPreSched( block *blk ) {
/****************************************/
    blk = blk;
}

extern  void    FPPostSched( block *blk ) {
/*****************************************/
    blk = blk;
}

extern  void    FPExpand() {
/**************************/
}

extern  void    FPOptimize() {
/****************************/
}

extern  void    InitFP() {
/************************/
}

extern void     FPPushParms( pn parm, call_state *state ) {
/*********************************************************/
    parm = parm;
    state = state;
}


extern  void    InitZeroPage() {
/******************************/
}


extern  void    FiniZeroPage() {
/******************************/
}

extern  void InitSegment() {
/*****************************/
}


extern  void FiniSegment() {
/*****************************/
}


extern  cg_type NamePtrType( name *op ) {
/*************************************/

    op = op;
    return( TY_NEAR_POINTER );
}


extern  int     AdjustBase() {
/****************************/

    return( 0 );
}


extern  void    ZeroMoves() {
/***************************/

}

#if !(_TARGET & _TARG_MIPS)
/* Functions from 386setcc.c; MIPS has its own mipssetc.c, Alpha could
 * have own version as well. Not sure about PowerPC.
 */
extern  bool    SetOnCondition() {
/********************************/

    return( FALSE );
}

extern  reg_set_index   SpecialPossible( instruction *ins ) {
/***********************************************************/
    ins = ins;
    return( RL_ );
}
#endif

extern  void    BuildIndex() {
/****************************/
}

extern  void    LdStAlloc() {
/***************************/
}

extern  void    LdStCompress() {
/******************************/
}

extern  bool    FreeObjCache() {
/******************************/

    return( FALSE );
}

extern  void    PreCall( cn call ) {
/**********************************/
    call = call;
}

extern  void    PostCall( cn call ) {
/***********************************/
    call = call;
}

extern  void    PushInSameBlock( instruction *ins ) {
/***************************************************/
    ins = ins;
}

extern  void    CheckCC( instruction *ins, instruction *new_ins ) {
/*****************************************************************/
    new_ins = new_ins;
    ins = ins;
}

extern  bool    DivIsADog( type_class_def class ) {
/*************************************************/

    return( _IsFloating( class ) );
}

extern  bool    AskSegNear( segment_id id ) {
/*******************************************/

    id = id;
    return( TRUE );
}

extern  void    MemtoBaseTemp() {
/*******************************/
}

extern  void    FixSegments() {
/*****************************/
}

extern  void    FixMemBases() {
/*****************************/
}

extern  void    FixFPConsts( instruction *ins ) {
/***********************************************/

    ins = ins;
}

extern  bool    LoadAToMove( instruction *ins ) {
/***********************************************/

    ins = ins;
    return( FALSE );
}

extern  void    FixCallIns( instruction *ins ) {
/**********************************************/

    ins = ins;
}

extern  void    DelSeg( instruction *ins ) {
/******************************************/

    ins = ins;
}

void    GenSetCC( instruction *ins ) {
/************************************/

    ins = ins;
    _Zoiks( 102 );
}

extern  bool    IsUncacheableMemory( name *opnd ) {

    opnd = opnd;
    return( FALSE );
}
