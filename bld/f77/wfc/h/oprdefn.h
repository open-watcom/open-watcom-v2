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
* Description:  upscan operator constant tables data
*
****************************************************************************/

// it hold relationship for OPR, OPRI and OPTR indexes
//
//      id      opr_index      proc_index
pick( OPR_PHI,  OPRI_PHI,      OPTR_NULL    ) //  null operator
pick( OPR_TRM,  OPRI_TRM,      OPTR_NULL    ) //  start/terminate symbol
pick( OPR_FBR,  OPRI_FBR,      OPTR_NULL    ) //  function/array bracket
pick( OPR_LBR,  OPRI_LBR,      OPTR_NULL    ) //  (
pick( OPR_COM,  OPRI_COM,      OPTR_NULL    ) //  ,
pick( OPR_COL,  OPRI_COL,      OPTR_NULL    ) //  :
//  assign operator
pick( OPR_EQU,  OPRI_EQU,      OPTR_ASGN    ) //  =
//
pick( OPR_RBR,  OPRI_RBR,      OPTR_CH_ASGN ) //  )
pick( OPR_DPT,  OPRI_FLD,      OPTR_CH_REL  ) //  .
pick( OPR_AST,  OPRI_PHI,      OPTR_REL     ) //  * for alternate return specifier
// relational operators
pick( OPR_EQ,   OPRI_REL,      OPTR_REL     ) //  .EQ.
pick( OPR_NE,   OPRI_REL,      OPTR_REL     ) //  .NE.
pick( OPR_LT,   OPRI_REL,      OPTR_REL     ) //  .LT.
pick( OPR_GE,   OPRI_REL,      OPTR_REL     ) //  .GE.
pick( OPR_LE,   OPRI_REL,      OPTR_REL     ) //  .LE.
pick( OPR_GT,   OPRI_REL,      OPTR_REL     ) //  .GT.
// logical operators
pick( OPR_EQV,  OPRI_EQV_NEQV, OPTR_EQV     ) //  .EQV.
pick( OPR_NEQV, OPRI_EQV_NEQV, OPTR_NEQV    ) //  .NEQV.
pick( OPR_OR,   OPRI_OR,       OPTR_OR      ) //  .OR.
pick( OPR_AND,  OPRI_AND,      OPTR_AND     ) //  .AND.
pick( OPR_NOT,  OPRI_NOT,      OPTR_NOT     ) //  .NOT.
// field selection operator
pick( OPR_FLD,  OPRI_FLD,      OPTR_FILLER  ) //  %
// arithmetic operators
pick( OPR_PLS,  OPRI_PLS_MIN,  OPTR_ADD     ) //  +
pick( OPR_MIN,  OPRI_PLS_MIN,  OPTR_SUB     ) //  -
pick( OPR_MUL,  OPRI_MUL_DIV,  OPTR_MUL     ) //  *
pick( OPR_DIV,  OPRI_MUL_DIV,  OPTR_DIV     ) //  /
// exponentiation
pick( OPR_EXP,  OPRI_EXP,      OPTR_EXP     ) //  **
// concatenation
pick( OPR_CAT,  OPRI_CAT,      OPTR_CAT     ) //  //   character operator
