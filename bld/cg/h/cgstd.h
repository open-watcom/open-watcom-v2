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


#ifndef STANDARD_INCLUDED
#define STANDARD_INCLUDED
#include <limits.h>
#include <stddef.h>
#include <assert.h>
#include "cgdll.h"

#if INT_MAX == SHRT_MAX
    #define _HOST_INTEGER       16
#else
    #define _HOST_INTEGER       32
#endif

typedef char *char_ptr;

#if !defined(BY_FORTRAN_FRONT_END)

    #define NULLCHAR        '\0'
    #if !defined(TRUE)
        #define TRUE            ((bool)1)
        #define FALSE           ((bool)0)
        #define MAYBE           ((bool)2)
    #endif

    #include "watcom.h"

#ifndef __cplusplus
    typedef unsigned        char    bool;
#endif
    typedef unsigned        char    byte;
    typedef void            *pointer;
    typedef float           real;
    typedef double          reallong;
    // typedef long double          realreallong;

#endif

#define _RoundUp( size, word )        ( ((size)+((word)-1)) & ~((word)-1) )
#define _IsPowerOfTwo( x )              ( ( (x) & ( -(x) + 1 ) ) == (x) )

#ifdef __AXP__
#define _AlignmentCheck( ptr, size )  assert( (((unsigned)ptr) & ((size)-1)) == 0 )
#else
#define _AlignmentCheck( ptr, size )    {}
#endif

typedef unsigned_32     cg_linenum;

#endif
