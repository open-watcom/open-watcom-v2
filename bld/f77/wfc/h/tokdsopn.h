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
* Description:  token/downscan operand constant tables data
*
****************************************************************************/


// It holds relationship for token class and operand indexes
//
//    tok_id    opn_id     opn_proc
pick( TO_OPR,  DSOPN_PHI,  &Phi       )      // null operand
pick( TO_NAM,  DSOPN_NAM,  &DSName    )      // name
pick( TO_LIT,  DSOPN_LIT,  &LitC      )      // literal
pick( TO_LGL,  DSOPN_LGL,  &LogC      )      // logical
pick( TO_INT,  DSOPN_INT,  &IntC      )      // integer
pick( TO_REA,  DSOPN_REA,  &RealC     )      // real
pick( TO_DBL,  DSOPN_DBL,  &DoubleC   )      // double
pick( TO_EXT,  DSOPN_EXT,  &ExtendedC )      // extended
pick( TO_OCT,  DSOPN_OCT,  &OctalC    )      // octal constant
pick( TO_HEX,  DSOPN_HEX,  &HexC      )      // hexadecimal constant
pick( TO_FMT,  DSOPN_FMT,  NULL       )      // FORMAT string
