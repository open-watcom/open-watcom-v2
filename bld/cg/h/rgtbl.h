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
* Description:  Prototypes for *rgtbl.c
*
****************************************************************************/


extern  void    InitRegTbl( void );
extern  reg_set_index   RegIntersect( reg_set_index s1, reg_set_index s2 );
extern  hw_reg_set      *ParmChoices( type_class_def class );
extern  hw_reg_set      InLineParm( hw_reg_set regs, hw_reg_set used );
extern  hw_reg_set      StructReg( void );
extern  hw_reg_set      ReturnReg( type_class_def class, bool use_87 );
extern  reg_set_index   SegIndex( void );
extern  reg_set_index   NoSegments( reg_set_index idx );
extern  reg_set_index   IndexIntersect( reg_set_index curr, type_class_def class, bool is_temp_index );
extern  bool    IsIndexReg( hw_reg_set reg, type_class_def class, bool is_temp_index );
extern  type_class_def  RegClass( hw_reg_set regs );
extern  bool    IndexRegOk( hw_reg_set reg, bool is_temp_index );
extern  bool    IsSegReg( hw_reg_set regs );
extern  hw_reg_set      Low16Reg( hw_reg_set regs );
extern  hw_reg_set      High16Reg( hw_reg_set regs );
extern  hw_reg_set      Low32Reg( hw_reg_set regs );
extern  hw_reg_set      High32Reg( hw_reg_set regs );
extern  hw_reg_set      HighReg( hw_reg_set regs );
extern  hw_reg_set      HighOffsetReg( hw_reg_set regs );
extern  hw_reg_set      HighTreePart( hw_reg_set regs );
extern  hw_reg_set      LowReg( hw_reg_set regs );
extern  hw_reg_set      LowOffsetReg( hw_reg_set regs );
extern  hw_reg_set      LowTreePart( hw_reg_set regs );
extern  hw_reg_set      FullReg( hw_reg_set regs );
extern  bool    IsRegClass( hw_reg_set regs, type_class_def class );
extern  hw_reg_set      ActualParmReg( hw_reg_set reg );
extern  hw_reg_set      FixedRegs( void );
extern  bool    IsStackReg( name *sp );
extern  hw_reg_set      StackReg( void );
extern  hw_reg_set      DisplayReg( void );
extern  int     SizeDisplayReg( void );
extern  hw_reg_set      AllCacheRegs( void );
extern  hw_reg_set      *IdxRegs( void );
