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
* Description:  compile-time symbols type constant tables data
*
****************************************************************************/


//       id              keyword         size    ptype
pick( FT_NO_TYPE,       "",                  0,  PT_NOTYPE  )  // no type specified == PT_NO_TYPE
pick( FT_LOGICAL_1,     "LOGICAL*1",         1,  PT_LOG_1   )  // 0 LOGICAL*1
pick( FT_LOGICAL,       "LOGICAL",           4,  PT_LOG_4   )  // 1 LOGICAL*4
pick( FT_INTEGER_1,     "INTEGER*1",         1,  PT_INT_1   )  // 2 INTEGER*1
pick( FT_INTEGER_2,     "INTEGER*2",         2,  PT_INT_2   )  // 3 INTEGER*2
pick( FT_INTEGER,       "INTEGER",           4,  PT_INT_4   )  // 4 INTEGER*4
pick( FT_REAL,          "REAL",              4,  PT_REAL_4  )  // 5 REAL
pick( FT_DOUBLE,        "DOUBLEPRECISION",   8,  PT_REAL_8  )  // 6 DOUBLE PRECISION
pick( FT_TRUE_EXTENDED, "EXTENDEDPRECISION", 16, PT_REAL_16 )  // 7 EXTENDED PRECISION
pick( FT_COMPLEX,       "COMPLEX",           8,  PT_CPLX_8  )  // 8 COMPLEX
pick( FT_DCOMPLEX,      "DOUBLECOMPLEX",     16, PT_CPLX_16 )  // 9 DOUBLE COMPLEX
pick( FT_TRUE_XCOMPLEX, "EXTENDEDCOMPLEX",   32, PT_CPLX_32 )  // 10 EXTENDED COMPLEX
pick( FT_CHAR,          "CHARACTER",         1,  PT_CHAR    )  // 11 CHARACTER
pick( FT_STRUCTURE,     "STRUCTURE",         0,  PT_STRUCT  )  // 12 structure
pick( FT_UNION,         "",                  0,  PT_NOTYPE  )  // 13 union
pick( FT_HEX,           "",                  0,  PT_NOTYPE  )  // 14 z constants in DATA statements
