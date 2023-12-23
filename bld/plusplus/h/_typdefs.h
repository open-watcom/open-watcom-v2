/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  C++ type related data
*
****************************************************************************/


/*  Use for initializing data that index via type->id */

#ifdef BASETYPES
#define picke(id,prom,prom_asm,type_text)
#else
#define picke(id,prom,prom_asm,type_text) pick(id,prom,prom_asm,type_text)
#endif

/*      id                  prom                prom_asm        type_text     */
pick(  TYP_ERROR,           TYP_ERROR,          0,              "<error> " )
pick(  TYP_BOOL,            TYP_SINT,           SYM_INT1,       "bool " )
pick(  TYP_CHAR,            TYP_SINT,           SYM_INT1,       "char " )
pick(  TYP_SCHAR,           TYP_SINT,           SYM_INT1,       "signed char " )
pick(  TYP_UCHAR,           TYP_SINT,           SYM_INT1,       "unsigned char " )
#if _CPU == 8086
pick(  TYP_WCHAR,           TYP_UINT,           SYM_INT2,       "wchar_t " )
#else
pick(  TYP_WCHAR,           TYP_SINT,           SYM_INT2,       "wchar_t " )
#endif
pick(  TYP_SSHORT,          TYP_SINT,           SYM_INT2,       "short " )
#if _CPU == 8086
pick(  TYP_USHORT,          TYP_UINT,           SYM_INT2,       "unsigned short " )
#else
pick(  TYP_USHORT,          TYP_SINT,           SYM_INT2,       "unsigned short " )
#endif
pick(  TYP_SINT,            TYP_SINT,           SYM_INT,        "int " )
pick(  TYP_UINT,            TYP_UINT,           SYM_INT,        "unsigned " )
pick(  TYP_SLONG,           TYP_SLONG,          SYM_INT4,       "long " )
pick(  TYP_ULONG,           TYP_ULONG,          SYM_INT4,       "unsigned long " )
pick(  TYP_SLONG64,         TYP_SLONG64,        SYM_INT8,       "__int64 " )
pick(  TYP_ULONG64,         TYP_ULONG64,        SYM_INT8,       "unsigned __int64 " )
pick(  TYP_FLOAT,           TYP_FLOAT,          SYM_FLOAT4,     "float " )
pick(  TYP_DOUBLE,          TYP_DOUBLE,         SYM_FLOAT8,     "double " )
pick(  TYP_LONG_DOUBLE,     TYP_LONG_DOUBLE,    SYM_FLOAT8,     "long double " )
picke( TYP_ENUM,            TYP_ERROR,          0,              "<enum> " )
picke( TYP_POINTER,         TYP_ERROR,          0,              "* " )
picke( TYP_TYPEDEF,         TYP_ERROR,          0,              "<typedef> " )
picke( TYP_CLASS,           TYP_ERROR,          0,              "<class> " )
picke( TYP_BITFIELD,        TYP_ERROR,          0,              "<bitfield> " )
picke( TYP_FUNCTION,        TYP_ERROR,          0,              "<function> " )
picke( TYP_ARRAY,           TYP_ERROR,          0,              "<array> " )
pick(  TYP_DOT_DOT_DOT,     TYP_ERROR,          0,              "... " )
pick(  TYP_VOID,            TYP_ERROR,          SYM_INT1,       "void " )
picke( TYP_MODIFIER,        TYP_ERROR,          0,              "<modifier> " )
picke( TYP_MEMBER_POINTER,  TYP_ERROR,          0,              "::* " )
picke( TYP_GENERIC,         TYP_ERROR,          0,              "? " )
// extended types C++11
pick(  TYP_NULLPTR,         TYP_ERROR,          0,              "<nullptr> " )
picke( TYP_TYPENAME,        TYP_ERROR,          0,              "<typename> " )

#undef picke
