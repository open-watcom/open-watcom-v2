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
* Description:  Compiler configuration macros.
*
****************************************************************************/


#if !defined( _WSTD_H_INCLUDED )

#define _WSTD_H_INCLUDED

#include "watcom.h"
#include "bool.h"

typedef unsigned char   boolean;
typedef unsigned char   byte;
typedef unsigned_8      bit;
typedef unsigned_8      ubit;
typedef unsigned_8      sbit;

/* Operating system macros:
 */
#define OS_DOS                  0
#define OS_QNX                  0
#define OS_QNX_16               0
#define OS_QNX_32               0
#define OS_OS2                  0
#define OS_OS2_16               0
#define OS_OS2_32               0
#define OS_WINDOWS              0
#define OS_WINDOWS_16           0
#define OS_WINDOWS_32           0
#define OS_NT                   0
#define OS_NLM                  0
#define OS_MACINTOSH            0
#define OS_OSI                  0
#define OS_SOLARIS              0
#define OS_IRIX                 0
#define OS_HPUX                 0
#define OS_AIX                  0
#define OS_DECUNIX              0
#define OS_LINUX                0
#define OS_BSD                  0

#if defined( __DOS__ )
    #undef  OS_DOS
    #define OS_DOS              1
#elif defined( __QNX__ )
    #undef  OS_QNX
    #define OS_QNX              1
    #if defined( __386__ )
        #undef  OS_QNX_32
        #define OS_QNX_32       1
    #else
        #undef  OS_QNX_16
        #define OS_QNX_16       1
    #endif
#elif defined( __LINUX__ )
    #undef  OS_LINUX
    #define OS_LINUX            1
#elif defined( __BSD__ )
    #undef OS_BSD
    #define OS_BSD              1           
#elif defined( __OS2__ )
    #undef  OS_OS2
    #define OS_OS2              1
    #if defined( __386__ )
        #undef  OS_OS2_32
        #define OS_OS2_32       1
    #else
        #undef  OS_OS2_16
        #define OS_OS2_16       1
    #endif
#elif defined( __WINDOWS__ )
    #undef  OS_WINDOWS
    #define OS_WINDOWS          1
    #if defined( __WINDOWS_386__ )
        #undef  OS_WINDOWS_32
        #define OS_WINDOWS_32   1
    #else
        #undef  OS_WINDOWS_16
        #define OS_WINDOWS_16   1
    #endif
#elif defined( __NT__ )
    #undef  OS_NT
    #define OS_NT               1
#elif defined( __NLM__ ) || defined( __NETWARE__ )
    #undef  OS_NLM
    #define OS_NLM              1
#elif defined( THINK_C )  ||  defined( __SC__ ) || defined( __MWERKS__ )
    #undef  OS_MACINTOSH
    #define OS_MACINTOSH        1
#elif defined( __OSI__ )
    #undef  OS_OSI
    #define OS_OSI              1
#elif defined( __sun__ ) || defined( __sun )
    #undef OS_SOLARIS
    #define OS_SOLARIS          1
#elif defined( __sgi ) || defined( sgi )
    #undef OS_IRIX
    #define OS_IRIX             1
#elif defined( __hpux__ ) || defined ( __hpux )
    #undef OS_HPUX
    #define OS_HPUX             1
#elif defined( _AIX )
    #undef OS_AIX
    #define OS_AIX              1
#elif defined(__DECC) || defined(__DECCXX)
    #if defined(__unix__)
        #undef OS_DECUNIX
        #define OS_DECUNIX 1
    #elif
        #error OS_xxx macro not enabled.
    #endif
#else
    #error OS_xxx macro not enabled.
#endif

/* Processor type macros:
 */
#define PR_i86                  0
#define PR_8086                 0
#define PR_186                  0
#define PR_286                  0
#define PR_386                  0
#define PR_486                  0
#define PR_PENTIUM              0
#define PR_PENTIUMPRO           0
#define PR_AXP                  0
#define PR_68000                0
#define PR_370                  0
#define PR_PPC                  0
#define PR_SPARC                0
#define PR_MIPS                 0
#define PR_HPPA                 0
#define PR_RS6000               0
#define PR_SH3                  0
#define PR_SH4                  0
#define PR_ARM                  0
#define PR_X64                  0

#if defined( _M_I86 )
    #undef  PR_i86
    #define PR_i86              1
    #if defined( __SW_0 )
        #undef  PR_8086
        #define PR_8086         1
    #elif defined( __SW_1 )
        #undef  PR_186
        #define PR_186          1
    #elif defined( __SW_2 )
        #undef  PR_286
        #define PR_286          1
    #else
        #undef  PR_8086
        #define PR_8086         1
        // #error _M_I86 defined, but no __SW_n macro defined.
    #endif
#elif defined( _M_I386 )
    #undef  PR_i86
    #define PR_i86              1
    #if defined( __SW_3 )
        #undef  PR_386
        #define PR_386          1
    #elif defined( __SW_4 )
        #undef  PR_486
        #define PR_486          1
    #elif defined( __SW_5 )
        #undef  PR_PENTIUM
        #define PR_PENTIUM      1
    #elif defined( __SW_6 )
        #undef  PR_PENTIUMPRO
        #define PR_PENTIUMPRO   1
    #else
        #error _M_I386 defined, but no __SW_n macro defined.
    #endif
#elif defined( __386__ )
    #undef  PR_i86
    #define PR_i86              1
    #undef  PR_386
    #define PR_386              1
#elif defined( _M_X64 )
    #undef  PR_X64
    #define PR_X64              1
#elif defined( THINK_C )        \
 ||   defined( __SC__ )         \
 ||   defined( __MWERKS__ ) && !defined( __powerc )
    #undef  PR_68000
    #define PR_68000            1
#elif defined(__ALPHA__)
    #undef PR_AXP
    #define PR_AXP              1
#elif defined(__DECC) || defined(__DECCXX)
    #if defined(__alpha)
        #undef PR_AXP
        #define PR_AXP          1
    #endif
#elif defined( __powerc )
    #undef PR_PPC
    #define PR_PPC              1
#elif defined( __sparc__ ) || defined( __sparc )
    #undef PR_SPARC
    #define PR_SPARC
#elif defined( __mips ) || defined( __host_mips ) || defined( _MIPS_ )
    #undef PR_MIPS
    #define PR_MIPS             1
#elif defined( _SH3_ )
    #undef PR_SH3
    #define PR_SH3              1
#elif defined( _SH4_ )
    #undef PR_SH4
    #define PR_SH4              1
#elif defined( _ARM_ )
    #undef PR_ARM
    #define PR_ARM              1
#elif defined( __hppa ) || defined( __hppa__ )
    #undef PR_HPPA
    #define PR_HPPA             1
#elif defined( _AIX ) || defined( __rs6000__ )
    #undef PR_RS6000
    #define PR_RS6000           1
#elif defined( SUN86 )
    #undef  PR_386
    #define PR_386              1
#else
    #error PR_xxx macro not enabled.
#endif

/* Memory model macros:
 */
#define SMALL_CODE              0
#define SMALL_DATA              0
#define LARGE_CODE              0
#define LARGE_DATA              0
#define HUGE_DATA               0
#define SEGMENTED_CODE          0
#define SEGMENTED_DATA          0

#if defined( UNIX )             \
||  defined( THINK_C )          \
||  defined( __SC__ )           \
||  defined( __MWERKS__ )       \
||  defined( __ALPHA__ )        \
|| (defined( _MSC_VER )  &&  (defined( __386__ ) || defined( _M_X64 ) || defined( UNDER_CE )))
    #undef __FLAT__
    #define __FLAT__
#endif

#if defined( __SMALL__ )
    #undef  SMALL_CODE
    #define SMALL_CODE          1
    #undef  SMALL_DATA
    #define SMALL_DATA          1
#elif defined( __COMPACT__ )
    #undef  SMALL_CODE
    #define SMALL_CODE          1
    #undef  LARGE_DATA
    #define LARGE_DATA          1
    #undef  SEGMENTED_DATA
    #define SEGMENTED_DATA      1
#elif defined( __MEDIUM__ )
    #undef  LARGE_CODE
    #define LARGE_CODE          1
    #undef  SMALL_DATA
    #define SMALL_DATA          1
    #undef  SEGMENTED_CODE
    #define SEGMENTED_CODE      1
#elif defined( __LARGE__ )
    #undef  LARGE_CODE
    #define LARGE_CODE          1
    #undef  LARGE_DATA
    #define LARGE_DATA          1
    #undef  SEGMENTED_CODE
    #define SEGMENTED_CODE      1
    #undef  SEGMENTED_DATA
    #define SEGMENTED_DATA      1
#elif defined( __HUGE__ )
    #undef  LARGE_CODE
    #define LARGE_CODE          1
    #undef  HUGE_DATA
    #define HUGE_DATA           1
    #undef  SEGMENTED_CODE
    #define SEGMENTED_CODE      1
    #undef  SEGMENTED_DATA
    #define SEGMENTED_DATA      1
#elif defined( __FLAT__ )
    #undef  SMALL_CODE
    #define SMALL_CODE          1
    #undef  SMALL_DATA
    #define SMALL_DATA          1
#else
    #error Memory model macro not enabled.
#endif

/* Character set macros:
 */
#define CS_ASCII                0
#define CS_ASCII_PLUS           0
#define CS_KANJI                0
#define CS_KANJI_UNICODE        0
#define CS_KOREAN               0
#define CS_CHINESE              0
#define CS_EBCDIC               0

#if defined( __SW_ZK0 )
    #undef  CS_ASCII_PLUS
    #undef  CS_KANJI
    #define CS_ASCII_PLUS       1
    #define CS_KANJI            1
#elif defined( __SW_ZK0U )
    #undef  CS_ASCII_PLUS
    #undef  CS_KANJI_UNICODE
    #define CS_ASCII_PLUS       1
    #define CS_KANJI_UNICODE    1
#elif defined( __SW_ZK1 )
    #undef  CS_ASCII_PLUS
    #undef  CS_CHINESE
    #define CS_ASCII_PLUS       1
    #define CS_CHINESE          1
#elif defined( __SW_ZK2 )
    #undef  CS_ASCII_PLUS
    #undef  CS_KOREAN
    #define CS_ASCII_PLUS       1
    #define CS_KOREAN           1
#elif PR_370
    #undef  CS_EBCDIC
    #define CS_EBCDIC           1
#elif PR_i86    \
||    PR_AXP    \
||    PR_68000  \
||    PR_PPC    \
||    PR_MIPS   \
||    PR_SH3    \
||    PR_SH4    \
||    PR_ARM    \
||    PR_X64    \
||    defined( UNIX )
    #undef  CS_ASCII_PLUS
    #undef  CS_ASCII
    #define CS_ASCII_PLUS       1
    #define CS_ASCII            1
#else
    #error Character set macro not enabled.
#endif

/* Miscellaneous:
 *
 * Big- or little-endian (does the high- or low-valued byte get stored at a
 * lower address in an integer):
 */
#if PR_370 || PR_68000 || PR_PPC
    #define HI_BYTE_LO_ADDR     1
    #define HI_BYTE_HI_ADDR     0
#else
    #define HI_BYTE_LO_ADDR     0
    #define HI_BYTE_HI_ADDR     1
#endif

/* Miscellaneous constants:
 */
#if !defined( TRUE )
    #define TRUE        1
#endif

#if !defined( FALSE )
    #define FALSE       0
#endif

#if !defined( NULLCHAR )
    #define NULLCHAR    '\0'
#endif

#if !defined( _unused )
    #if defined( THINK_C )
        #define _unused( v )
    #else
        #define _unused( v ) (v = v)
    #endif
#endif

#if !defined( NULL )
    #if defined( THINK_C )  ||  defined( __SC__ )
      #if defined( __cplusplus )
        #define NULL    0
      #else
        #define NULL    ((void *) 0)
      #endif
    #elif SMALL_DATA  ||  defined( __386__ )
        #define NULL    0
    #else
        #define NULL    0L
    #endif
#endif

#if !defined( _NULL )
    #define _NULL               ((void *) 0)
#endif

#if !defined( _FNULL )
    #define _FNULL              ((void __far *) 0)
#endif

#if !defined( _NNULL )
    #define _NNULL              ((void __near *) 0)
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

/* Fill an object with zeroes:
 */
#if !defined( _zerofill )
    #define _zerofill( obj )    memset( &(obj), 0, sizeof( obj ) )
#endif

#endif
