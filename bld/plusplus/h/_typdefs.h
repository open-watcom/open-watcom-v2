/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2018 The Open Watcom Contributors. All Rights Reserved.
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

/*    id                prom            prom_asm        type_text     */
pick(  ERROR,           ERROR,          0,              "<error> " )
pick(  BOOL,            SINT,           SYM_INT1,       "bool " )
pick(  CHAR,            SINT,           SYM_INT1,       "char " )
pick(  SCHAR,           SINT,           SYM_INT1,       "signed char " )
pick(  UCHAR,           SINT,           SYM_INT1,       "unsigned char " )
#if _CPU == 8086
pick(  WCHAR,           UINT,           SYM_INT2,       "wchar_t " )
#else
pick(  WCHAR,           SINT,           SYM_INT2,       "wchar_t " )
#endif
pick(  SSHORT,          SINT,           SYM_INT2,       "short " )
#if _CPU == 8086
pick(  USHORT,          UINT,           SYM_INT2,       "unsigned short " )
#else
pick(  USHORT,          SINT,           SYM_INT2,       "unsigned short " )
#endif
pick(  SINT,            SINT,           SYM_INT,        "int " )
pick(  UINT,            UINT,           SYM_INT,        "unsigned " )
pick(  SLONG,           SLONG,          SYM_INT4,       "long " )
pick(  ULONG,           ULONG,          SYM_INT4,       "unsigned long " )
pick(  SLONG64,         SLONG64,        SYM_INT8,       "__int64 " )
pick(  ULONG64,         ULONG64,        SYM_INT8,       "unsigned __int64 " )
pick(  FLOAT,           FLOAT,          SYM_FLOAT4,     "float " )
pick(  DOUBLE,          DOUBLE,         SYM_FLOAT8,     "double " )
pick(  LONG_DOUBLE,     LONG_DOUBLE,    SYM_FLOAT8,     "long double " )
picke( ENUM,            ERROR,          0,              "<enum> " )
picke( POINTER,         ERROR,          0,              "* " )
picke( TYPEDEF,         ERROR,          0,              "<typedef> " )
picke( CLASS,           ERROR,          0,              "<class> " )
picke( BITFIELD,        ERROR,          0,              "<bitfield> " )
picke( FUNCTION,        ERROR,          0,              "<function> " )
picke( ARRAY,           ERROR,          0,              "<array> " )
pick(  DOT_DOT_DOT,     ERROR,          0,              "... " )
pick(  VOID,            ERROR,          SYM_INT1,       "void " )
picke( MODIFIER,        ERROR,          0,              "<modifier> " )
picke( MEMBER_POINTER,  ERROR,          0,              "::* " )
picke( GENERIC,         ERROR,          0,              "? " )
// extended types C++11
pick(  NULLPTR,         ERROR,          0,              "<nullptr> " )
picke( TYPENAME,        ERROR,          0,              "<typename> " )

#undef picke
