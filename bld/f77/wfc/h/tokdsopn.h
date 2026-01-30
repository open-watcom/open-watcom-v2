/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  downscan operand constant tables data
*
****************************************************************************/


// It holds operand indexes
//
//     dsopn_id   opn_proc    opn_id
pick( DSOPN_PHI,  &Phi,       "OPN_PHI" )      // null operand
pick( DSOPN_NAM,  &DSName,    "OPN_NAM" )      // name
pick( DSOPN_LIT,  &LitC,      "OPN_LIT" )      // literal
pick( DSOPN_LGL,  &LogC,      "OPN_LGL" )      // logical
pick( DSOPN_INT,  &IntC,      "OPN_INT" )      // integer
pick( DSOPN_REA,  &RealC,     "OPN_REA" )      // real
pick( DSOPN_DBL,  &DoubleC,   "OPN_DBL" )      // double
pick( DSOPN_EXT,  &ExtendedC, "OPN_EXT" )      // extended
pick( DSOPN_OCT,  &OctalC,    "OPN_OCT" )      // octal constant
pick( DSOPN_HEX,  &HexC,      "OPN_HEX" )      // hexadecimal constant
pick( DSOPN_FMT,  NULL,       "OPN_FMT" )      // FORMAT string
