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
* Description:  Stubs for routines not needed by the S/370.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cgdefs.h"
#include "regset.h"
#include "addrname.h"
#include "ptrint.h"
#include "x87.h"


/* KLUDGEY crud for segment register */

extern  void    OptSegs() {
/*************************/
}

extern void AddSegment( instruction *ins ) {
/******************************************/
    ins=ins;
}

extern  void    MoveSegRes( instruction *ins, instruction *new_ins ) {
/********************************************************************/
    ins=ins;new_ins=new_ins;
}

extern  void    MoveSegOp(instruction* ins,instruction* new_ins,int i) {
/**********************************************/
    ins=ins;new_ins=new_ins;i=i;
}

extern  void    DelSegRes( instruction *ins ) {
/**********************************************/
    ins=ins;
}

extern  void    DelSegOp( instruction *ins, int i ) {
/**********************************************/
    ins=ins;i=i;
}

extern  void    DupSeg( instruction *ins, instruction *new_ins ) {
/**********************************************/
    ins=ins;new_ins=new_ins;
}

extern  void            DelSeg(instruction* ins ) {
/*************************************************/
    ins=ins;
}

extern  void    DupSegRes( instruction *ins, instruction *new_ins ) {
/**********************************************/
    ins=ins;new_ins=new_ins;
}

extern void MergeIndex() {
/**********************************************/
}

/* KLUDGEY crud for the 8087 */

extern void             FPExpand(void) {
/**********************************************/
}

extern void             FPOptimize(void) {
/**********************************************/
}

extern  bool    FPStackIns( instruction *ins ) {
/**********************************************/
    ins=ins;
    return( FALSE );
}

extern  bool    FPStackReg( name *reg_name ) {
/********************************************/
    reg_name=reg_name;
    return( FALSE );
}

extern  int    FPStkOver( instruction *ins, int stk_depth ) {
/**********************************************************/
    ins=ins;stk_depth=stk_depth;
    return( -1 );
}

extern  void    FPCalcStk( instruction *ins, int *pdepth ) {
/********************************************************/
    ins=ins;pdepth=pdepth;
}

extern  void    FPPreSched( block *blk ) {
/**************************************/
    blk=blk;
}

extern  void    FPPostSched( block *blk ) {
/**************************************/
    blk=blk;
}

extern name  *FArea;
extern  void    FPRegAlloc() {
/****************************/
        FArea = NULL;  /* reset the FP convert area */
}

extern void             FPParms(void) {
/**********************************************/
}

extern void             FPReloc(void) {
/**********************************************/
}

extern  bool    FPSideEffect( instruction *ins ) {
/**********************************************/
    ins=ins;
    return( FALSE );
}

extern  type_class_def  FPInsClass( instruction *ins ) {
/********************************************/
    ins=ins;
    return( XX );
}

extern  bool  FPFreeIns( instruction *ins ) {
/*******************************************/
    ins=ins;
    return( FALSE );
}

extern  void    FPSetStack( name *name ) {
/****************************************/
    name=name;
}

extern  bool    FPIsConvert( instruction *ins ) {
/**********************************************/
    ins=ins;
    return( FALSE );
}

extern  void    FPNotStack( name *name ) {
/**********************************************/
    name=name;
}

extern  bool    FPIsStack( name *name ) {
/**********************************************/
    name=name;
    return( FALSE );
}

extern  bool    FPStackOp( name *name ) {
/***************************************/
    name=name;
    return( FALSE );
}

extern  void    InitFP() {
/**********************************************/
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


extern  cg_type PtrType( name *name ) {
/*************************************/

    name=name;
    return( TY_NEAR_POINTER );
}


extern  type_length    AdjustBase() {
/****************************/

    return( 0 );
}


extern  bool    SetOnCondition() {
/********************************/

    return( FALSE );
}

extern  reg_set_index   SpecialPossible( instruction *ins ) {
/***********************************************************/
    ins = ins;
    return( RL_ );
}

/* Peep hole optimizer stubs */

extern  bool    ShrinkQueue( pointer_int size ) {
/***********************************************/

    size = size;
    return( FALSE );
}


extern  void    InitQueue() {
/***************************/

}

extern  void    FiniQueue() {
/***************************/

}


extern  void    AbortQueue() {
/****************************/

}


extern  bool    InstrFrlFree() {
/******************************/

    return( FALSE );
}

/* runtime routines */


#include "offset.h"

extern  void    SetLocation( offset );
extern  offset  AskLocation( void );

extern  void            IncLocation( offset by ) {
/************************************************/

    SetLocation( AskLocation() + by );
}

extern seg_id AskCodeSeg();
extern  seg_id  AskAltCodeSeg() {
/****************************/

    return( AskCodeSeg() );
}


extern  bool    AskSegNear( segment_id id ) {
/********************************************/

    id=id;
    return( FALSE );
}


extern  bool    LdStAlloc()
/*******************************

    Look for non-move operations with memory operands and change them
    into RISC style load/store instructions. This helps on the 486 and
    up because of instruction scheduling. Return a boolean saying whether
    anything got twiddled so the register scoreboarder can be run again.
*/
{
    return( FALSE );
}


extern  void    LdStCompress()
/**********************************

    Compress any load/store sequences generated by LdStAlloc back
    into memory ops if no optimizations made use of them.
*/
{
}

extern  bool    DivIsADog( type_class_def class )
/***********************************************/
{
    class=class;
    return( FALSE );
}

extern  void    AddCacheRegs(void)
/********************************/
{
}

extern  void    InitStackDepth( block *blk ) {
/********************************************/
    blk=blk;
}

