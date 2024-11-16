/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
    /*
     * enable Structured Exception Handling for all 32-bit targets
     */
    #define __SEH__
#endif

#ifdef __UNIX__
    #define FNAMECMPSTR             strcmp      /* for case sensitive file systems */
#else
    #define FNAMECMPSTR             stricmp     /* for case insensitive file systems */
#endif

#define BUF_SIZE                    512

#define LENLIT(c)                   (sizeof( c ) - 1)

#define PPOPERATOR_DEFINED          "defined"
#define PPOPERATOR_PRAGMA           "_Pragma"

#define IS_PPOPERATOR_DEFINED(s)    (strcmp(s, PPOPERATOR_DEFINED) == 0)
#define IS_PPOPERATOR_PRAGMA(s,l)   ((CompFlags.extensions_enabled || CompVars.cstd > STD_C89) \
                                    && l == LENLIT(PPOPERATOR_PRAGMA) \
                                    && strcmp(s, PPOPERATOR_PRAGMA) == 0)

#define ARRAY_SIZE(x)               (sizeof( x ) / sizeof( *x ))

#define IsHugeData()                ((TargetSwitches & (CGSW_X86_BIG_DATA | CGSW_X86_CHEAP_POINTER)) == CGSW_X86_BIG_DATA)

#define DOS_EOF_CHAR                0x1A
