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
* Description:  operator index for expression generate
*                  or constant expression folding routines
*
****************************************************************************/


//       id         const      gener
pick( OPTR_NULL,    NULL,      NULL     )  //  no operation
//
//  Logical operator group
//
pick( OPTR_EQV,     &LogOp,    &LogOp   )  //  .eqv. operation
pick( OPTR_NEQV,    &LogOp,    &LogOp   )  //  .neqv. operation
pick( OPTR_OR,      &LogOp,    &LogOp   )  //  .or. operation
pick( OPTR_AND,     &LogOp,    &LogOp   )  //  .and. operation
pick( OPTR_NOT,     &LogOp,    &LogOp   )  //  .not. operation
//
//  Field operator
//
pick( OPTR_FILLER,  NULL,      &FieldOp )  //  filler
//
//  Assign operator
//
pick( OPTR_ASGN,    &BadEqual, &AsgnOp  )  //  ,
//
pick( OPTR_CH_ASGN, NULL,      NULL     )  //  character assignment
pick( OPTR_CH_REL,  NULL,      &FieldOp )  //  character compare
//
//  Relation operator group
//
pick( OPTR_REL,     &RelOp,    &RelOp   )  //  relational operation (,
//
//  Arithmetic operator group
//
pick( OPTR_ADD,     &BinOp,    &BinOp   )  //  + operation
pick( OPTR_SUB,     &BinOp,    &BinOp   )  //  - operation
pick( OPTR_MUL,     &BinOp,    &BinOp   )  //  * operation
pick( OPTR_DIV,     &BinOp,    &BinOp   )  //  / operation
//
//  Exponentiation
//
pick( OPTR_EXP,     &ExpOp,    &ExpOp   )  //  exponentiation
//
//  Concatenation
//
pick( OPTR_CAT,     NULL,      NULL     )  //  concatenation

// Note:  concatenation // (gener) handled by FiniCat in UPSCAN who will call ConstCat().
//        concatenation // (const) AsgnOp handles a = b // c
