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


#ifndef _WATCOM_H_INCLUDED_

/*
   This next set of lines is a temp fix until the 11.0 headers are
   in universal usage.
*/
#if ( !defined( _WCUNALIGNED ) && ( __WATCOMC__ < 1100 ) )
    #include <errno.h>
    #ifndef _WCUNALIGNED
        #define _WCUNALIGNED
    #endif
#endif

#if !defined(__sun__) && !defined(sun) && !defined(__sgi) && !defined(__hppa) && !defined(_AIX) && !defined( __alpha ) && !defined( linux )
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

typedef struct {
    union {
        unsigned_32     _32[2];
        unsigned_16     _16[4];
        unsigned_8       _8[8];
        struct {
#if defined( WATCOM_BIG_ENDIAN )
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
        #ifdef __WATCOM_INT64__
           unsigned __int64     _64[1];
        #endif
    } u;
} unsigned_64;
typedef unsigned_64     signed_64;

/* for little endian machines */

#if defined( WATCOM_BIG_ENDIAN )
    #define I64LO32     1
    #define I64HI32     0
#else
    #define I64LO32     0
    #define I64HI32     1
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

#define _WATCOM_H_INCLUDED_
#endif
