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


/* targetax.h: target types for DEC Alpha NT machine */

#define TARGET_BOOL             1
#define TARGET_CHAR             1
#define TARGET_WIDE_CHAR        2
#define TARGET_SHORT            2
#define TARGET_INT              4
#define TARGET_POINTER          4
#define TARGET_NEAR_POINTER     4       // near -> far16 isn't always safe
#define TARGET_FAR16_POINTER    4       // far16 -> near is still a truncation!
#define TARGET_FAR_POINTER      6
#define TARGET_LONG             4
#define TARGET_ULONG            4
#define TARGET_LONG64           8
#define TARGET_ULONG64          8
#define TARGET_UINT             4
#define TARGET_FLOAT            4
#define TARGET_DOUBLE           8
#define TARGET_LONG_DOUBLE      8       // until CG supports long double

#define TARGET_BITS_CHAR        8
#define TARGET_BITS             (TARGET_INT * TARGET_BITS_CHAR)

#define TARGET_INT_MAX  2147483647
#define TARGET_INT_MIN  (-2147483647-1)
#define TARGET_UINT_MAX 4294967295U

#define TARGET_PACKING  8
#define TARGET_MAX_PACKING      8

typedef signed char     target_schar;
typedef unsigned char   target_uchar;
typedef unsigned short  target_wchar;
typedef short           target_short;
typedef unsigned short  target_ushort;
typedef long            target_int;
typedef unsigned long   target_uint;
typedef long            target_long;
typedef unsigned long   target_ulong;

typedef unsigned long   target_size_t;
typedef unsigned long   target_offset_t;
