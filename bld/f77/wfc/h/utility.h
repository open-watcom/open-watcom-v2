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
* Description:  prototypes for various compiler routines
*
****************************************************************************/


#include "itdefn.h"

extern  void    AdvanceITPtr( void );
extern  void    FreeITNodes( itnode *head );
extern  void    ITPurge( void );
extern  void    FreeOneNode( itnode *itptr );
extern  char    *MkNodeStr( itnode *itptr );
extern  void    FrNodeStr( char *str );
extern  bool    CmpNode2Str( itnode *itptr, char *str );
extern  void    ProcExpr( void );
extern  bool    TypeIs( TYPE typ );
extern  bool    ConstExpr( TYPE typ );
extern  void    ChkType( TYPE typ );
extern  void    DimExpr( void );
extern  void    ProcSubExpr( void );
extern  void    ProcDataExpr( void );
extern  void    ProcDataRepExpr( void );
extern  void    ProcDataIExpr( void );
extern  void    ProcIOExpr( void );
extern  bool    CLogicExpr( void );
extern  bool    CCharExpr( void );
extern  bool    CIntExpr( void );
extern  void    CArithExpr( void );
extern  void    BoolExpr( void );
extern  void    BoolSubExpr( void );
extern  void    SelectExpr( void );
extern  void    IntegerExpr( void );
extern  void    IntSubExpr( void );
extern  void    EatDoParm( void );
extern  void    IfExpr( void );
extern  void    CharSubExpr( void );
extern  bool    BitOn( unsigned_16 bits );
extern  TYPE    Map2BaseType( TYPE typ );
extern  bool    ClassIs( unsigned_16 class );
extern  bool    Subscripted( void );
