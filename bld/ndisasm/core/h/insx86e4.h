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
* Description:  Table of x86 opcodes and corresponding decode routines.
*             only for extension SSE, SSE2 and SSE3 with REPE prefix (0xF3)
*
****************************************************************************/

/*
   All these instructions required only mandatory prefix 0xf3


      Idx,              Name,           Opcode,         Mask,           Handler
*/

inspick( addss,         "addss",        0x0000580f,     0x0000ffff,     X86XMMRegModRM32 )      // sse  
inspick( cmpss,         "cmpss",        0x0000c20f,     0x0000ffff,     X86XMMRegModRM32Imm )   // sse  
inspick( cvtdq2pd,      "cvtdq2pd",     0x0000e60f,     0x0000ffff,     X86XMMRegModRM64 )      // sse2 
inspick( cvtsi2ss,      "cvtsi2ss",     0x00002a0f,     0x0000ffff,     X86XMMRegModRMMixed )   // sse  
inspick( cvtss2sd,      "cvtss2sd",     0x00005a0f,     0x0000ffff,     X86XMMRegModRM32 )      // sse2 
inspick( cvtss2si,      "cvtss2si",     0x00002d0f,     0x0000ffff,     X86RegModRMMixed )      // sse  
inspick( cvttps2dq,     "cvttps2dq",    0x00005b0f,     0x0000ffff,     X86XMMRegModRM )        // sse2 
inspick( cvttss2si,     "cvttss2si",    0x00002c0f,     0x0000ffff,     X86RegModRMMixed )      // sse  
inspick( divss,         "divss",        0x00005e0f,     0x0000ffff,     X86XMMRegModRM32 )      // sse  
inspick( maxss,         "maxss",        0x00005f0f,     0x0000ffff,     X86XMMRegModRM32 )      // sse  
inspick( minss,         "minss",        0x00005d0f,     0x0000ffff,     X86XMMRegModRM32 )      // sse  
inspick( movdqu_1,      "movdqu",       0x00006f0f,     0x0000ffff,     X86XMMRegModRM )        // sse2 
inspick( movdqu_2,      "movdqu",       0x00007f0f,     0x0000ffff,     X86XMMRegModRM_Rev )    // sse2 
inspick( movq_2,        "movq",         0x00007e0f,     0x0000ffff,     X86XMMRegModRM64 )      // sse2 
inspick( movq2dq,       "movq2dq",      0x00c0d60f,     0x00c0ffff,     X86XMMRegModRMMixed )   // sse2 
inspick( movshdup,      "movshdup",     0x0000160f,     0x0000ffff,     X86XMMRegModRM )        // sse3 
inspick( movsldup,      "movsldup",     0x0000120f,     0x0000ffff,     X86XMMRegModRM )        // sse3 
inspick( movss_1,       "movss",        0x0000100f,     0x0000ffff,     X86XMMRegModRM32 )      // sse  
inspick( movss_2,       "movss",        0x0000110f,     0x0000ffff,     X86XMMRegModRM32_Rev )  // sse  
inspick( mulss,         "mulss",        0x0000590f,     0x0000ffff,     X86XMMRegModRM32 )      // sse  
// Part of xchg
inspick( pause,         "pause",        0x00000090,     0x00000000,     NULL )                  // sse2 
inspick( pshufhw,       "pshufhw",      0x0000700f,     0x0000ffff,     X86XMMRegModRMImm )     // sse2 
inspick( rcpss,         "rcpss",        0x0000530f,     0x0000ffff,     X86XMMRegModRM32 )      // sse  
inspick( rsqrtss,       "rsqrtss",      0x0000520f,     0x0000ffff,     X86XMMRegModRM32 )      // sse  
inspick( sqrtss,        "sqrtss",       0x0000510f,     0x0000ffff,     X86XMMRegModRM32 )      // sse  
inspick( subss,         "subss",        0x00005c0f,     0x0000ffff,     X86XMMRegModRM32 )      // sse  
