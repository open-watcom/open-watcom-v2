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
* Description:  Prototypes and pragmas for handling __int64 values.
*
****************************************************************************/


#ifndef _I64_H_INCLUDED
#define _I64_H_INCLUDED

    #define INT64_TYPE __int64
    #define UINT64_TYPE unsigned __int64

    #define _clib_I64Negative( a )      (((__int64)a) < 0)
    #define _clib_I64Positive( a )      (((__int64)a) > 0)
    #define _clib_I32ToI64( a, b )      b = (__int64)(signed long)a
    #define _clib_U32ToU64( a, b )      b = (unsigned __int64)(unsigned long)a
    #define _clib_I64ToInt( a, b )      b = (signed)a
    #define _clib_U64ToUInt( a, b )     b = (unsigned)a
    #define _clib_I64ToLong( a, b )     b = (signed long)a
    #define _clib_U64ToULong( a, b )    b = (unsigned long)a
    #define _clib_I64Neg( a, b )        b = -a
    #define _clib_U64Neg( a, b )        _clib_I64Neg( a, b )
    #define _clib_U64Zero( a )          (a == 0)
    #define _clib_U64Cmp( a, b )        (a > b ? +1 : a < b ? -1 : 0)

    #define _clib_U64Mul( a, b, c )     c = a * b
    #define _clib_U64Div( a, b, c, d )  (d = a % b, c = a / b)
    #define _clib_U64Add( a, b, c )     c = a + b

// conversion functions - use standard ulltoa()/atoll()

#endif
