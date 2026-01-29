/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
//    tok_id    opn_id
pick( TOK_OPR,  DSOPN_PHI )      // null operand
pick( TOK_NAM,  DSOPN_NAM )      // name
pick( TOK_LIT,  DSOPN_LIT )      // literal
pick( TOK_LGL,  DSOPN_LGL )      // logical
pick( TOK_INT,  DSOPN_INT )      // integer
pick( TOK_REA,  DSOPN_REA )      // real
pick( TOK_DBL,  DSOPN_DBL )      // double
pick( TOK_EXT,  DSOPN_EXT )      // extended
pick( TOK_OCT,  DSOPN_OCT )      // octal constant
pick( TOK_HEX,  DSOPN_HEX )      // hexadecimal constant
pick( TOK_FMT,  DSOPN_FMT )      // FORMAT string
