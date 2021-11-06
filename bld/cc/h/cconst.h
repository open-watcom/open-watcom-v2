/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  C compiler global macro definition
*
****************************************************************************/


#ifndef _CPU
    #error _CPU macro not defined
#endif

#if _CPU != 8086
    /* enable Structured Exception Handling for all 32-bit targets */
    #define __SEH__
#endif

#ifdef __UNIX__
    #define FNAMECMPSTR             strcmp      /* for case sensitive file systems */
#else
    #define FNAMECMPSTR             stricmp     /* for case insensitive file systems */
#endif

#ifndef LARGEST_QUAD_INDEX
    #define LARGEST_QUAD_INDEX      0xFFFF
    #define LARGEST_DATA_QUAD_INDEX 0xFFFFF
#else
    #define LARGEST_DATA_QUAD_INDEX LARGEST_QUAD_INDEX
#endif
#define LARGEST_SYM_INDEX           0xFFFF

#define SYM_BUF_SIZE                1024
#define SYMS_PER_BUF                (SYM_BUF_SIZE/sizeof(SYM_ENTRY))
#define SYMBUFS_PER_SEG             16
#define SYM_SEG_SIZE                (SYM_BUF_SIZE*SYMBUFS_PER_SEG)

#define MAX_SYM_SEGS                (LARGEST_SYM_INDEX/(SYMS_PER_BUF*SYMBUFS_PER_SEG)+1)

#define BUF_SIZE                    512

#define CMPLIT(s,c) memcmp( s, c, sizeof( c ) )
#define CPYLIT(s,c) memcpy( s, c, sizeof( c ) )
#define LENLIT(c)   (sizeof( c ) - 1)

#define PPOPERATOR_DEFINED          "defined"
#define PPOPERATOR_PRAGMA           "_Pragma"

#define IS_PPOPERATOR_DEFINED(s)    (CMPLIT(s, PPOPERATOR_DEFINED) == 0)
#define IS_PPOPERATOR_PRAGMA(s,l)   ((CompFlags.extensions_enabled || CompFlags.c99_extensions) \
                                    && l == LENLIT(PPOPERATOR_PRAGMA) \
                                    && CMPLIT(s, PPOPERATOR_PRAGMA) == 0)

#define ARRAY_SIZE(x)               (sizeof( x ) / sizeof( *x ))
