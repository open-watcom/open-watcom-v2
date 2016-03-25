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
* Description:  Configure 'j', 'z', and 't' modifiers for __scnf and __prtf.
*
****************************************************************************/


#ifndef PTRSCNF_H_INCLUDED
#define PTRSCNF_H_INCLUDED

#include <limits.h>
#include <stdint.h>

/* Currently size_t is always 'unsigned int', but won't be on LP64 systems */

#if SIZE_MAX == UINT_MAX
    #define ZSPEC_IS_INT
#elif SIZE_MAX == ULONG_MAX
    #define ZSPEC_IS_LONG
#else
    #error Could not configure zspec
#endif

/* Currently intmax_t is always 'long long int' but might be something
 * else, in theory at least
 */
#if INTMAX_MAX == LLONG_MAX
    #define JSPEC_IS_LLONG
    #define JSPEC_CASE_LLONG    case STRING( 'j' ):
#else
    #error Could not configure jspec
#endif

/* Currently ptrdiff_t can be either 'long int' or 'int' */
#if PTRDIFF_MAX == INT_MAX
    #define TSPEC_IS_INT
#elif PTRDIFF_MAX == LONG_MAX
    #define TSPEC_IS_LONG
#else
    #error Could not configure tspec
#endif

#ifdef ZSPEC_IS_INT
    #define ZSPEC_CASE_INT      case STRING( 'z' ):
#else
    #define ZSPEC_CASE_INT
#endif

#ifdef ZSPEC_IS_LONG
    #define ZSPEC_CASE_LONG     case STRING( 'z' ):
#else
    #define ZSPEC_CASE_LONG
#endif

#ifdef TSPEC_IS_INT
    #define TSPEC_CASE_INT      case STRING( 't' ):
#else
    #define TSPEC_CASE_INT
#endif

#ifdef TSPEC_IS_LONG
    #define TSPEC_CASE_LONG     case STRING( 't' ):
#else
    #define TSPEC_CASE_LONG
#endif

#endif
