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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


//
// ITYPES -- establish operand types for intermediate codes
//
// note: this file is never included directly
//
//  IOPTYPES.H -- enumerates the code
//  ITYPVEC.H -- generates a vector of opcode types
//  CGIO.C -- generates a dispatch vector for precompiled headers

ITYPE( NUL ),           // null - opcode performs an action without an operand
ITYPE( BIN ),           // binary - integer operand
ITYPE( CON ),           // POOL_CON * - floating point operand
ITYPE( SYM ),           // SYMBOL operand
ITYPE( STR ),           // STRING_CONSTANT operand
ITYPE( TYP ),           // TYPE operand
ITYPE( SCP ),           // SCOPE operand
ITYPE( SRC ),           // SRCFILE operand
#undef ITYPE
