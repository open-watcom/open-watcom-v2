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


/* target16.h: target types for 16-bit machine */

#define TARGET_CHAR     1
#define TARGET_SHORT    2
#define TARGET_INT      2
#define TARGET_POINTER  2
#define TARGET_NEAR_POINTER 2
#define TARGET_FAR_POINTER  4
#define TARGET_LONG     4
#define TARGET_ULONG    4
#define TARGET_LONG64   8
#define TARGET_ULON64   8
#define TARGET_UINT     2
#define TARGET_FLOAT    4
#define TARGET_DOUBLE   8
#define TARGET_BITS     32

#define TARGET_INT_MAX  32767
#define TARGET_INT_MIN  (-32767-1)
#define TARGET_UINT_MAX 0xFFFF
#define TARGET_FLT_MAX  3.402823466e+38f

typedef short           target_short;
typedef unsigned short  target_ushort;
typedef short           target_int;
typedef unsigned short  target_uint;
typedef long            target_long;
typedef unsigned long   target_ulong;

