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
* Description:  Prototypes for makeaddr.c
*
****************************************************************************/


extern  an      NewAddrName( void );
extern  an      NewBoolNode( void );
extern  an      MakeTypeTempAddr( name *op, type_def *tipe );
extern  an      MakeTempAddr( name *op );
extern  void    InitMakeAddr( void );
extern  name    *GenIns( an addr );
extern  void    NamesCrossBlocks( void );
extern  bool    AddrFrlFree( void );
extern  void    AddrFree( an node );
extern  an      MakeGets( an dst, an src, type_def *tipe );
extern  an      MakeConst( float_handle cf, type_def *tipe );
extern  an      MakePoints( an name, type_def *tipe );
extern  an      RegName( hw_reg_set reg, type_def *tipe );
extern  an      InsName( instruction *ins, type_def *tipe );
extern  name    *LoadTemp( name *temp, type_class_def class );
extern  an      AddrEval( an addr );
extern  void    MoveAddress( an src, an dest );
extern  void    Convert( an addr, type_class_def class );
extern  bool    PointLess( an l_addr, an r_addr );
extern  an      AddrToIns( an addr );
extern  void    InsToAddr( an addr );
extern  an      AddrDuplicate( an node );
extern  an      AddrCopy( an node );
extern  an      AddrSave( an node );
extern  void    AddrDemote( an node );
extern  name    *MaybeTemp( name *op, type_class_def kind );
extern  void    CheckPointer( an addr );
extern  void    FixCodePtr( an addr );
extern  bool    NeedPtrConvert( an addr, type_def *tipe );
extern  name    *LoadAddress( name *op, name *suggest, type_def *type_ptr );
extern  an      MakeAddrName( cg_class class, cg_sym_handle sym, type_def *tipe );
