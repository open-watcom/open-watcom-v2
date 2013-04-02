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
* Description:  Common type definitions and macros widely used by Open
*               Watcom tools.
*
****************************************************************************/


#ifndef _WATCOM_H_INCLUDED_
#define _WATCOM_H_INCLUDED_

#ifndef LONG_IS_64BITS
  #ifdef __LP64__
    #define LONG_IS_64BITS
  #endif
#endif

#include <errno.h>
#ifndef __WATCOMC__
#include "clibext.h"
#endif

#if !defined(__sun__) && !defined(sun) && !defined(__sgi) && !defined(__hppa) && !defined(_AIX) && !defined(__alpha) && !defined(_TYPES_H_) && !defined(_SYS_TYPES_H)
typedef unsigned        uint;
#endif

typedef unsigned char   uint_8;
typedef unsigned short  uint_16;
#if defined (LONG_IS_64BITS)
typedef unsigned int    uint_32;
#else
typedef unsigned long   uint_32;
#endif
typedef unsigned char   unsigned_8;
typedef unsigned short  unsigned_16;
#if defined (LONG_IS_64BITS)
typedef unsigned int    unsigned_32;
#else
typedef unsigned long   unsigned_32;
#endif

typedef signed char     int_8;
typedef signed short    int_16;
#if defined (LONG_IS_64BITS)
typedef signed int      int_32;
#else
typedef signed long     int_32;
#endif
typedef signed char     signed_8;
typedef signed short    signed_16;
#if defined (LONG_IS_64BITS)
typedef signed int      signed_32;
#else
typedef signed long     signed_32;
#endif

#if _INTEGRAL_MAX_BITS >= 64
#if defined( _MSC_VER )
typedef unsigned __int64    uint_64;
typedef __int64             int_64;
#else
typedef unsigned long long  uint_64;
typedef long long           int_64;
#endif
#endif

typedef struct {
    union {
        unsigned_32     _32[2];
        unsigned_16     _16[4];
        unsigned_8       _8[8];
        struct {
#if defined( __BIG_ENDIAN__ )
            unsigned    v       : 1;
            unsigned            : 15;
            unsigned            : 16;
            unsigned            : 16;
            unsigned            : 16;
#else
            unsigned            : 16;
            unsigned            : 16;
            unsigned            : 16;
            unsigned            : 15;
            unsigned    v       : 1;
#endif
        }       sign;
#if _INTEGRAL_MAX_BITS >= 64
        uint_64         _64[1];
#endif
    } u;
} unsigned_64;
typedef unsigned_64     signed_64;

#if defined( _WIN64 )
typedef unsigned __int64    pointer_int;
#else
typedef unsigned long       pointer_int;
#endif

/* Macros for low/high end access on little and big endian machines */

#if defined( __BIG_ENDIAN__ )
    #define I64LO32     1
    #define I64HI32     0
    #define I64LO16     3
    #define I64HI16     0
    #define I64LO8      7
    #define I64HI8      0
#else
    #define I64LO32     0
    #define I64HI32     1
    #define I64LO16     0
    #define I64HI16     3
    #define I64LO8      0
    #define I64HI8      7
#endif

/* Define _crtn for prototypes for external C routines called from C++.
 * Eg. extern _crtn void Foo();
 */
#if !defined( _crtn )
    #if defined( __cplusplus )
        #define _crtn   "C"
    #else
        #define _crtn
    #endif
#endif

/*  Macros for little/big endian conversion; These exist to simplify writing
 *  code that handles both little and big endian data on either little or big
 *  endian host platforms. Some of these macros could be implemented as inline
 *  assembler where instructions to byte swap data in registers or read/write
 *  memory access with byte swapping is available.
 *
 *  NOTE:   The SWAP_XX macros will swap data in place. If you only want to take a 
 *          a copy of the data and leave the original intact, then use the SWAPNC_XX 
 *          macros.
 */
#define SWAPNC_16(w)    (\
                            (((w) & 0x000000FFUL) << 8) |\
                            (((w) & 0x0000FF00UL) >> 8)\
                        )
#define SWAPNC_32(w)    (\
                            (((w) & 0x000000FFUL) << 24) |\
                            (((w) & 0x0000FF00UL) << 8) |\
                            (((w) & 0x00FF0000UL) >> 8) |\
                            (((w) & 0xFF000000UL) >> 24)\
                        )
#define SWAPNC_64(w)    (\
                            (((w) & 0x00000000000000FFULL) << 56) |\
                            (((w) & 0x000000000000FF00ULL) << 40) |\
                            (((w) & 0x0000000000FF0000ULL) << 24) |\
                            (((w) & 0x00000000FF000000ULL) << 8) |\
                            (((w) & 0x000000FF00000000ULL) >> 8) |\
                            (((w) & 0x0000FF0000000000ULL) >> 24) |\
                            (((w) & 0x00FF000000000000ULL) >> 40) |\
                            (((w) & 0xFF00000000000000ULL) >> 56)\
                        )   

#if defined( __BIG_ENDIAN__ )
    /* Macros to get little endian data */
    #define GET_LE_16(w)    SWAPNC_16(w)
    #define GET_LE_32(w)    SWAPNC_32(w)
    #define GET_LE_64(w)    SWAPNC_64(w)
    /* Macros to get big endian data */
    #define GET_BE_16(w)    (w)
    #define GET_BE_32(w)    (w)
    #define GET_BE_64(w)    (w)
    /* Macros to convert little endian data in place */
    #define CONV_LE_16(w)   (w) = SWAPNC_16(w)
    #define CONV_LE_32(w)   (w) = SWAPNC_32(w)
    #define CONV_LE_64(w)   (w) = SWAPNC_64(w)
    /* Macros to convert big endian data in place */
    #define CONV_BE_16(w)
    #define CONV_BE_32(w)
    #define CONV_BE_64(w)
    /* Macros to swap byte order */
    #define SWAP_16     CONV_LE_16
    #define SWAP_32     CONV_LE_32
    #define SWAP_64     CONV_LE_64
#else
    /* Macros to get little endian data */
    #define GET_LE_16(w)    (w)
    #define GET_LE_32(w)    (w)
    #define GET_LE_64(w)    (w)
    /* Macros to get big endian data */
    #define GET_BE_16(w)    SWAPNC_16(w)
    #define GET_BE_32(w)    SWAPNC_32(w)
    #define GET_BE_64(w)    SWAPNC_64(w)
    /* Macros to convert little endian data in place */
    #define CONV_LE_16(w)
    #define CONV_LE_32(w)
    #define CONV_LE_64(w)
    /* Macros to convert big endian data in place */
    #define CONV_BE_16(w)   (w) = SWAPNC_16(w)
    #define CONV_BE_32(w)   (w) = SWAPNC_32(w)
    #define CONV_BE_64(w)   (w) = SWAPNC_64(w)
    /* Macros to swap byte order */
    #define SWAP_16     CONV_BE_16
    #define SWAP_32     CONV_BE_32
    #define SWAP_64     CONV_BE_64
#endif

/* Macros to swap byte order in 64-bit structure */
#if defined( __BIG_ENDIAN__ )
  #if _INTEGRAL_MAX_BITS >= 64
    #define SCONV_LE_64(w)   (w).u._64[0] = SWAPNC_64((w).u._64[0])
  #else
    #define SCONV_LE_64(w)   {unsigned_32 x = SWAPNC_32(w.u._32[I64LO32]);w.u._32[I64LO32]=SWAPNC_32(w.u._32[I64HI32]);w.u._32[I64HI32] = x;}
  #endif
    #define SCONV_BE_64(w)
#else
    #define SCONV_LE_64(w)
  #if _INTEGRAL_MAX_BITS >= 64
    #define SCONV_BE_64(w)   (w).u._64[0] = SWAPNC_64((w).u._64[0])
  #else
    #define SCONV_BE_64(w)   {unsigned_32 x = SWAPNC_32(w.u._32[I64LO32]);w.u._32[I64LO32]=SWAPNC_32(w.u._32[I64HI32]);w.u._32[I64HI32] = x;}
  #endif
#endif

#endif
