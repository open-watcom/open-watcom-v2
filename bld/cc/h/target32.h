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
* Description:  Determines type sizes/limits for 32 bit platform
*
****************************************************************************/


/* target32.h: target types for 32-bit machine */

#define TARGET_CHAR     1
#define TARGET_SHORT    2
#define TARGET_INT      4
#define TARGET_POINTER  4
#define TARGET_NEAR_POINTER 4
#define TARGET_FAR_POINTER  6
#define TARGET_LONG     4
#define TARGET_ULONG    4
#define TARGET_LONG64   8
#define TARGET_ULON64   8
#define TARGET_UINT     4
#define TARGET_FLOAT    4
#define TARGET_DOUBLE   8
#define TARGET_LDOUBLE     10
#define TARGET_FCOMPLEX     8
#define TARGET_DCOMPLEX    16
#define TARGET_LDCOMPLEX   20
#define TARGET_FIMAGINARY   4
#define TARGET_DIMAGINARY   8
#define TARGET_LDIMAGINARY 10
#define TARGET_BOOL     1
#define TARGET_WCHAR    2
#define TARGET_BITS     32

#define TARGET_INT_MAX  2147483647
#define TARGET_INT_MIN  (-2147483647-1)
#define TARGET_UINT_MAX 4294967295U
#define TARGET_FLT_MAX  3.402823466e+38f

typedef short           target_short;
typedef unsigned short  target_ushort;
typedef long            target_int;
typedef unsigned long   target_uint;
typedef long            target_long;
typedef unsigned long   target_ulong;

