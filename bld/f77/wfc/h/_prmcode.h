/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  compile-time constants indicating parameter class
*
****************************************************************************/


//
// NOTE: These constants should stay in the following order.
//       ( routines rely on them being sequential )
//
/*    enum              text                            */
pick( PC_CONST,         "expression"                    )   // Non-modifiable argument. Passed by value.
pick( PC_VARIABLE,      "simple variable"               )   // Simple variable name.
pick( PC_ARRAY_ELT,     "array element"                 )   // Array element.
pick( PC_SS_ARRAY,      "substring array element"       )   // Substring array element.
pick( PC_ARRAY_NAME,    "array name"                    )   // Array name.
pick( PC_PROCEDURE,     "subprogram name"               )   // Subprogram name. function/subroutine established
pick( PC_FN_OR_SUB,     "subprogram name"               )   // Subprogram name. could be function or subroutine
pick( PC_STATEMENT,     "alternate return specifier"    )   // Alternate return specifier.
pick( PC_RESULT,        ""                              )   // Expected result type of subprogram.
