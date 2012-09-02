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
* Description:  C data type related definition
*
****************************************************************************/

/* it is used in definition for 
    CGDataType[] table in  cgen.c
    AsmDataType[] table in cpragx86.c
    CTypenames[] table in  cdump.c
    CTypeSizes[] table in  ctype.c
*/

/*      enum         cgtype          asmtype       name                     size */
pick1( BOOL,        TY_UINT_1,       SYM_INT1,    "_Bool",                  TARGET_BOOL        )
pick1( CHAR,        TY_INT_1,        SYM_INT1,    "signed char",            TARGET_CHAR        )
pick1( UCHAR,       TY_UINT_1,       SYM_INT1,    "unsigned char",          TARGET_CHAR        )
pick1( SHORT,       TY_INT_2,        SYM_INT2,    "short",                  TARGET_SHORT       )
pick1( USHORT,      TY_UINT_2,       SYM_INT2,    "unsigned short",         TARGET_SHORT       )
pick1( INT,         TY_INTEGER,      SYM_INT,     "int",                    TARGET_INT         )
pick1( UINT,        TY_UNSIGNED,     SYM_INT,     "unsigned int",           TARGET_INT         )
pick1( LONG,        TY_INT_4,        SYM_INT4,    "long",                   TARGET_LONG        )
pick1( ULONG,       TY_UINT_4,       SYM_INT4,    "unsigned long",          TARGET_LONG        )
pick1( LONG64,      TY_INT_8,        SYM_INT8,    "__int64",                TARGET_LONG64      )
pick1( ULONG64,     TY_UINT_8,       SYM_INT8,    "unsigned __int64",       TARGET_LONG64      )
pick1( FLOAT,       TY_SINGLE,       SYM_FLOAT4,  "float",                  TARGET_FLOAT       )
pick1( DOUBLE,      TY_DOUBLE,       SYM_FLOAT8,  "double",                 TARGET_DOUBLE      )
pick1( LONG_DOUBLE, TY_LONG_DOUBLE,  SYM_FLOAT10, "long double",            TARGET_LDOUBLE     )
pick1( FIMAGINARY,  TY_SINGLE,       SYM_FLOAT4,  "float _Imaginary",       TARGET_FIMAGINARY  )
pick1( DIMAGINARY,  TY_DOUBLE,       SYM_FLOAT8,  "double _Imaginary",      TARGET_DIMAGINARY  )
pick1( LDIMAGINARY, TY_LONG_DOUBLE,  SYM_FLOAT10, "long double _Imaginary", TARGET_LDIMAGINARY )
pick1( POINTER,     TY_POINTER,      0,           "pointer",                0                  )
pick1( ARRAY,       TY_POINTER,      0,           "array",                  0                  )
pick1( STRUCT,      TY_POINTER,      0,           "struct",                 0                  )
pick1( UNION,       TY_POINTER,      0,           "union",                  0                  )
pick1( FUNCTION,    TY_DEFAULT,      0,           "function",               0                  )
pick1( FIELD,       TY_DEFAULT,      0,           "<field>",                0                  )
pick1( VOID,        TY_INTEGER,      SYM_INT1,    "void",                   0                  )
pick1( ENUM,        TY_INTEGER,      0,           "enum",                   0                  )
pick1( TYPEDEF,     TY_INTEGER,      0,           "<typdef>",               0                  )
pick1( UFIELD,      TY_INTEGER,      0,           "<ufield>",               0                  )
pick1( DOT_DOT_DOT, TY_INTEGER,      0,           "...",                    0                  )
pick1( PLAIN_CHAR,  TY_INTEGER,      SYM_INT1,    "<char>",                 TARGET_CHAR        )
pick1( WCHAR,       TY_INTEGER,      SYM_INT2,    "<wide char>",            TARGET_WCHAR       )
pick1( FCOMPLEX,    TY_POINTER,      0,           "float _Complex",         TARGET_FCOMPLEX    )
pick1( DCOMPLEX,    TY_POINTER,      0,           "double _Complex",        TARGET_DCOMPLEX    )
pick1( LDCOMPLEX,   TY_POINTER,      0,           "long double _Complex",   TARGET_LDCOMPLEX   )
