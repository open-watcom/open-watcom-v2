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
* Description:  Top level WOMP include file.
*
****************************************************************************/


#ifndef WOMP_H
#define WOMP_H    1
#include <watcom.h>

#define _WOMP_READ              1       /* code to handle reading objs */
#define _WOMP_WRITE             2       /* code to handle writing objs */
#define _WOMP_EXTRAS            4       /* code for exotic record types */
#define _WOMP_WATFOR            8       /* code specific to WATFOR */
#define _WOMP_WASM              16      /* code specific to WASM */
#define _WOMP_WOMP              32      /* code specific to WOMP */
#define _WOMP_NASM              64      /* code specific to (new) WASM */

/*
   use < > to include wompopt.h so that we don't always get womp version of
   wompopt.h
*/
#include <wompopt.h>

#define STATIC static
#define FORWARD

#define STDIN   0
#define STDOUT  1
#define STDERR  2

#if ( defined( M_I86SM ) || defined( M_I86MM ) ) && defined( _M_I86 )
  #error This code can only be compiled for a large data model
#endif

#if defined( __BIG_ENDIAN__ )
  #define ReadU16(p)      (((uint_8*)p)[0] | (((uint_8*)p)[1] << 8))
  #define ReadU32(p)      (ReadU16(p) | (ReadU16(p+2) << 16))
  #define ReadS16(p)      (((uint_8*)p)[0] | (((uint_8*)p)[1] << 8))
  #define ReadS32(p)      (ReadS16(p) | (ReadS16(p+2) << 16))
  #define WriteU16(r,n)   (*(uint_16*)(r) = (uint_16)(((n) & 0xff) << 8) | (((n) & 0xff00) >> 8))
  #define WriteU32(r,n)   (*(uint_32*)(r) = (uint_32)(((n) & 0xff) << 24) | (((n) & 0xff00) << 8) | (((n) & 0xff0000) >> 8) | (((n) & 0xff000000) >> 24))
  #define WriteS16(r,n)   (*(int_16*)(r) = (int_16)(((n) & 0xff) << 8) | (((n) & 0xff00) >> 8))
  #define WriteS32(r,n)   (*(int_32*)(r) = (int_32)(((n) & 0xff) << 24) | (((n) & 0xff00) << 8) | (((n) & 0xff0000) >> 8) | (((n) & 0xff000000) >> 24))
#else
  #define ReadU16(p)      (*(uint_16*)(p))
  #define ReadU32(p)      (*(uint_32*)(p))
  #define ReadS16(p)      (*(int_16*)(p))
  #define ReadS32(p)      (*(int_16*)(p))
  #define WriteU16(p,n)   (*(uint_16*)(p) = (uint_16)(n))
  #define WriteU32(p,n)   (*(uint_32*)(p) = (uint_32)(n))
  #define WriteS16(p,n)   (*(int_16*)(p) = (int_16)(n))
  #define WriteS32(p,n)   (*(int_32*)(p) = (int_32)(n))
#endif

#endif
