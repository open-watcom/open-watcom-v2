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


#include "variety.h"
#undef __INLINE_FUNCTIONS__
#include <stdio.h>
#include <stdlib.h>
#include "extfunc.h"

typedef int qcomp( const void *, const void * );
#if defined(_M_IX86)
    #pragma aux (__outside_CLIB) qcomp;
#endif


/* Function to find the median value */
static char *med3( char *a, char *b, char *c, qcomp cmp )
{
    if( cmp( a, b ) > 0 ) {
        if( cmp( a, c ) > 0 ) {
            if( cmp( b, c ) > 0 ) {
                return( b );
            } else {
                return( c );
            }
        } else {
            return( a );
        }
    } else {
        if( cmp( a, c ) >= 0 ) {
            return( a );
        } else {
            if( cmp( b, c ) > 0 ) {
                return( c );
            } else {
                return( b );
            }
        }
    }
}


#ifdef __AXP__

    #define FUNCTION_LINKAGE    static
    #define FUNCTION_NAME       aligned_qsort
    #define PTRATTR
    #define MED3                med3
    #define BYTESWAP            aligned_byteswap
    #include "qsortrtn.c"

    #undef FUNCTION_NAME
    #undef PTRATTR
    #undef MED3
    #undef BYTESWAP
    #define FUNCTION_NAME       unaligned_qsort
    #define PTRATTR             __unaligned
    #define BYTESWAP            unaligned_byteswap
    #define MED3(a,b,c,f)       med3( (char*)(a), (char*)(b), (char*)(c), (f) )
    #include "qsortrtn.c"

    _WCRTLINK void qsort( void *in_base, size_t n, size_t size,
                          int (*compar)(const void *, const void *) )
    {
        /*
         * If size%4 is 0 and the input base pointer is on a word boundary,
         * call the aligned version of qsort.  Otherwise, call the version
         * that knows its pointers will be unaligned, so all the fixups can
         * be done by qsort rather than by expensive OS exceptions.
         */
        if( (size&3) == 0  &&  (((unsigned)in_base)&3) == 0 ) {
            aligned_qsort( in_base, n, size, compar );
        } else {
            unaligned_qsort( (__unaligned void*)in_base, n, size, compar );
        }
    }

#else

    #define FUNCTION_LINKAGE    _WCRTLINK
    #define FUNCTION_NAME       qsort
    #define PTRATTR
    #define MED3                med3
    #define BYTESWAP            byteswap
    #include "qsortrtn.c"

#endif
