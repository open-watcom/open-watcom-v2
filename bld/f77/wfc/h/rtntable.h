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
* Description:  function table for operators sequence testing
*
****************************************************************************/


//    id     proc
pick( BB,  &BadBracket   )  // for odd parenthesis sequences
pick( BC,  &BadColonOpn  )  // colon operator expects integer operands
pick( BE,  &BadEqual     )  // illegal quantity on left side of =
pick( BR,  &BadRelOpn    )  // relational operator has logical operand
pick( BS,  &BadSequence  )  // bad sequence of operators
pick( CA,  &Call         )  // detach ss list, substr list, call subprog
pick( EE,  &EndExpr      )  // opr sequence is start,terminate
pick( GC,  &GrabColon    )  // substring indexing expression using :
pick( GO,  &Generate     )  // go and generate some code
pick( PA,  &PrepArg      )  // prepare item in function or subscript list
pick( RP,  &RemoveParen  )  // remove parenthesis
pick( FC,  &FiniCat      )  // finish concatenation
pick( MO,  &Missing      )  // missing operator
pick( BT,  &BackTrack    )  // scan backwards
pick( CB,  &CatBack      )  // maybe scan backwards on = // sequence
pick( CR,  &CatAxeParens )  // remove parenthesis set on ( // sequence
pick( CO,  &CatOpn       )  // concatenation operand
pick( CP,  &CatParen     )  // check if ) is for a substring operand
pick( EV,  &ProcOpn      )  // process operand and then scan backwards
pick( HC,  &HighColon    )  // check for DSOPN_PHI on : ) sequence
pick( LC,  &LowColon     )  // handle [ : and called from GrabColon
pick( PC,  &ParenCat     )  // check if ) is for a substring operand
pick( PE,  &ParenExpr    )  // done evaluating parenthesized expression
pick( KO,  &ChkCatOpn    )  // check if concatenation operand
