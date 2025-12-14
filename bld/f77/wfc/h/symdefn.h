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
* Description:  compile-time symbols type constant tables data
*
****************************************************************************/


//       id              keyword            size  ptype        dwtype
pick( FT_NO_TYPE,       "",                  0,  FPT_NOTYPE,  DW_FT_NONE          ) // no type specified == FPT_NO_TYPE
pick( FT_LOGICAL_1,     "LOGICAL*1",         1,  FPT_LOG_1,   DW_FT_BOOLEAN       ) // 0 LOGICAL*1
pick( FT_LOGICAL,       "LOGICAL",           4,  FPT_LOG_4,   DW_FT_BOOLEAN       ) // 1 LOGICAL*4
pick( FT_INTEGER_1,     "INTEGER*1",         1,  FPT_INT_1,   DW_FT_SIGNED        ) // 2 INTEGER*1
pick( FT_INTEGER_2,     "INTEGER*2",         2,  FPT_INT_2,   DW_FT_SIGNED        ) // 3 INTEGER*2
pick( FT_INTEGER,       "INTEGER",           4,  FPT_INT_4,   DW_FT_SIGNED        ) // 4 INTEGER*4
pick( FT_REAL,          "REAL",              4,  FPT_REAL_4,  DW_FT_FLOAT         ) // 5 REAL
pick( FT_DOUBLE,        "DOUBLEPRECISION",   8,  FPT_REAL_8,  DW_FT_FLOAT         ) // 6 DOUBLE PRECISION
pick( FT_TRUE_EXTENDED, "EXTENDEDPRECISION", 16, FPT_REAL_16, DW_FT_FLOAT         ) // 7 EXTENDED PRECISION
pick( FT_COMPLEX,       "COMPLEX",           8,  FPT_CPLX_8,  DW_FT_COMPLEX_FLOAT ) // 8 COMPLEX
pick( FT_DCOMPLEX,      "DOUBLECOMPLEX",     16, FPT_CPLX_16, DW_FT_COMPLEX_FLOAT ) // 9 DOUBLE COMPLEX
pick( FT_TRUE_XCOMPLEX, "EXTENDEDCOMPLEX",   32, FPT_CPLX_32, DW_FT_COMPLEX_FLOAT ) // 10 EXTENDED COMPLEX
pick( FT_CHAR,          "CHARACTER",         1,  FPT_CHAR,    DW_FT_UNSIGNED_CHAR ) // 11 CHARACTER
pick( FT_STRUCTURE,     "STRUCTURE",         0,  FPT_STRUCT,  DW_FT_NONE          ) // 12 structure
pick( FT_UNION,         "",                  0,  FPT_NOTYPE,  DW_FT_NONE          ) // 13 union
pick( FT_HEX,           "",                  0,  FPT_NOTYPE,  DW_FT_NONE          ) // 14 z constants in DATA statements
