/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  C data type related definition
*
****************************************************************************/

/*
 * it is used in definition for
 * CGDataType[] table in  cgen.c
 * AsmDataType[] table in cpragx86.c
 * CTypenames[] table in  cdump.c
 * CTypeSizes[] table in  ctype.c
 */

/*      type             dtype            cgtype          asmtype       name                     size */
pick1( TYP_BOOL,        QDT_BOOL,        TY_UINT_1,       SYM_INT1,    "_Bool",                  TARGET_BOOL        )
pick1( TYP_CHAR,        QDT_CHAR,        TY_INT_1,        SYM_INT1,    "signed char",            TARGET_CHAR        )
pick1( TYP_UCHAR,       QDT_UCHAR,       TY_UINT_1,       SYM_INT1,    "unsigned char",          TARGET_CHAR        )
pick1( TYP_SHORT,       QDT_SHORT,       TY_INT_2,        SYM_INT2,    "short",                  TARGET_SHORT       )
pick1( TYP_USHORT,      QDT_USHORT,      TY_UINT_2,       SYM_INT2,    "unsigned short",         TARGET_SHORT       )
pick1( TYP_INT,         QDT_INT,         TY_INTEGER,      SYM_INT,     "int",                    TARGET_INT         )
pick1( TYP_UINT,        QDT_UINT,        TY_UNSIGNED,     SYM_INT,     "unsigned int",           TARGET_INT         )
pick1( TYP_LONG,        QDT_LONG,        TY_INT_4,        SYM_INT4,    "long",                   TARGET_LONG        )
pick1( TYP_ULONG,       QDT_ULONG,       TY_UINT_4,       SYM_INT4,    "unsigned long",          TARGET_LONG        )
pick1( TYP_LONG64,      QDT_LONG64,      TY_INT_8,        SYM_INT8,    "__int64",                TARGET_LONG64      )
pick1( TYP_ULONG64,     QDT_ULONG64,     TY_UINT_8,       SYM_INT8,    "unsigned __int64",       TARGET_LONG64      )
pick1( TYP_FLOAT,       QDT_FLOAT,       TY_SINGLE,       SYM_FLOAT4,  "float",                  TARGET_FLOAT       )
pick1( TYP_DOUBLE,      QDT_DOUBLE,      TY_DOUBLE,       SYM_FLOAT8,  "double",                 TARGET_DOUBLE      )
pick1( TYP_LONG_DOUBLE, QDT_LONG_DOUBLE, TY_LONG_DOUBLE,  SYM_FLOAT10, "long double",            TARGET_LDOUBLE     )
pick1( TYP_FIMAGINARY,  QDT_FIMAGINARY,  TY_SINGLE,       SYM_FLOAT4,  "float _Imaginary",       TARGET_FIMAGINARY  )
pick1( TYP_DIMAGINARY,  QDT_DIMAGINARY,  TY_DOUBLE,       SYM_FLOAT8,  "double _Imaginary",      TARGET_DIMAGINARY  )
pick1( TYP_LDIMAGINARY, QDT_LDIMAGINARY, TY_LONG_DOUBLE,  SYM_FLOAT10, "long double _Imaginary", TARGET_LDIMAGINARY )
pick1( TYP_POINTER,     QDT_POINTER,     TY_POINTER,      0,           "pointer",                0                  )
pick1( TYP_ARRAY,       QDT_ARRAY,       TY_POINTER,      0,           "array",                  0                  )
pick1( TYP_STRUCT,      QDT_STRUCT,      TY_POINTER,      0,           "struct",                 0                  )
pick1( TYP_UNION,       QDT_UNION,       TY_POINTER,      0,           "union",                  0                  )
pick1( TYP_FUNCTION,    QDT_FUNCTION,    TY_DEFAULT,      0,           "function",               0                  )
pick1( TYP_FIELD,       QDT_FIELD,       TY_DEFAULT,      0,           "<field>",                0                  )
pick1( TYP_VOID,        QDT_VOID,        TY_INTEGER,      SYM_INT1,    "void",                   0                  )
pick1( TYP_ENUM,        QDT_ENUM,        TY_INTEGER,      0,           "enum",                   0                  )
pick1( TYP_TYPEDEF,     QDT_TYPEDEF,     TY_INTEGER,      0,           "<typdef>",               0                  )
pick1( TYP_UFIELD,      QDT_UFIELD,      TY_INTEGER,      0,           "<ufield>",               0                  )
pick1( TYP_DOT_DOT_DOT, QDT_DOT_DOT_DOT, TY_INTEGER,      0,           "...",                    0                  )
pick1( TYP_PLAIN_CHAR,  QDT_PLAIN_CHAR,  TY_INTEGER,      SYM_INT1,    "<char>",                 TARGET_CHAR        )
pick1( TYP_WCHAR,       QDT_WCHAR,       TY_INTEGER,      SYM_INT2,    "<wide char>",            TARGET_WCHAR       )
pick1( TYP_FCOMPLEX,    QDT_FCOMPLEX,    TY_POINTER,      0,           "float _Complex",         TARGET_FCOMPLEX    )
pick1( TYP_DCOMPLEX,    QDT_DCOMPLEX,    TY_POINTER,      0,           "double _Complex",        TARGET_DCOMPLEX    )
pick1( TYP_LDCOMPLEX,   QDT_LDCOMPLEX,   TY_POINTER,      0,           "long double _Complex",   TARGET_LDCOMPLEX   )
