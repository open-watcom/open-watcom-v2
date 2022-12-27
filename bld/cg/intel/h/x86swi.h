/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Target dependent code generation switches for x86.
*
****************************************************************************/


CGSW_X86_EZ_OMF                  = 0x00000001,  /* Generate EZ-OMF objects */
CGSW_X86_BIG_DATA                = 0x00000002,  /* Data pointers are far */
CGSW_X86_BIG_CODE                = 0x00000004,  /* Code pointers are far */
CGSW_X86_CHEAP_POINTER           = 0x00000008,  /* Model isn't huge */
CGSW_X86_FLAT_MODEL              = 0x00000010,  /* Flat memory model */
CGSW_X86_FLOATING_FS             = 0x00000020,  /* FS selector is floating */
CGSW_X86_FLOATING_GS             = 0x00000040,  /* GS selector is floating */
CGSW_X86_FLOATING_ES             = 0x00000080,  /* ES selector is floating */
CGSW_X86_FLOATING_SS             = 0x00000100,  /* SS selector is floating */
CGSW_X86_FLOATING_DS             = 0x00000200,  /* DS selector is floating */
CGSW_X86_USE_32                  = 0x00000400,  /* Generate 32-bit segments */
CGSW_X86_INDEXED_GLOBALS         = 0x00000800,  /* Position Independent Code (faulty!) */
CGSW_X86_WINDOWS                 = 0x00001000,  /* Generate Win16 prologs */
CGSW_X86_CHEAP_WINDOWS           = 0x00002000,  /* Cheap Win16 prologs */
CGSW_X86_CONST_IN_CODE           = 0x00004000,  /* FP consts in code segment */
CGSW_X86_NEED_STACK_FRAME        = 0x00008000,  /* Always generate stack frame */
CGSW_X86_LOAD_DS_DIRECTLY        = 0x00010000,  /* No runtime call to load DS */
CGSW_X86_SMART_WINDOWS           = 0x00020000,  /* Smart Win16 prolog (DS==SS) */
CGSW_X86_P5_PROFILING            = 0x00040000,  /* Pentium RDTSC profiling (-et) */
CGSW_X86_P5_DIVIDE_CHECK         = 0x00080000,  /* Check for bad Pentium FDIV */
CGSW_X86_GENERIC_TLS             = 0x00100000,  /* TLS code not NT specific (unused?) */
CGSW_X86_NEW_P5_PROFILING        = 0x00200000,  /* "New" profiling (-etp) */
CGSW_X86_STATEMENT_COUNTING      = 0x00400000,  /* Statement counting (-esp) */
CGSW_X86_NULL_SELECTOR_BAD       = 0x00800000,  /* Avoid null selectors on i86 */
CGSW_X86_P5_PROFILING_CTR0       = 0x01000000,  /* Use RDPMC instead of RDTSC */
CGSW_X86_GEN_FWAIT_386           = 0x02000000,  /* Generate FWAITs on 386 and up */

