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
*             only for extension SSE, SSE2 and SSE3 with REPNE prefix (0xF2)
*
****************************************************************************/

/*
   All these instructions required only mandatory prefix 0xf2


      Idx,              Name,           Opcode,         Mask,           Handler
*/

inspick( addsd,         "addsd",        0x0000580f,     0x0000ffff,     X86XMMRegModRM64 )      // sse2 
inspick( addsubps,      "addsubps",     0x0000d00f,     0x0000ffff,     X86XMMRegModRM )        // sse3 
inspick( cmpsd,         "cmpsd",        0x0000c20f,     0x0000ffff,     X86XMMRegModRM64Imm )   // sse2 
inspick( cvtpd2dq,      "cvtpd2dq",     0x0000e60f,     0x0000ffff,     X86XMMRegModRM )        // sse2 
inspick( cvtsd2si,      "cvtsd2si",     0x00002d0f,     0x0000ffff,     X86RegModRMMixed )      // sse2 
inspick( cvtsd2ss,      "cvtsd2ss",     0x00005a0f,     0x0000ffff,     X86XMMRegModRM64 )      // sse2 
inspick( cvtsi2sd,      "cvtsi2sd",     0x00002a0f,     0x0000ffff,     X86XMMRegModRMMixed )   // sse2 
inspick( cvttsd2si,     "cvttsd2si",    0x00002c0f,     0x0000ffff,     X86RegModRMMixed )      // sse2 
inspick( divsd,         "divsd",        0x00005e0f,     0x0000ffff,     X86XMMRegModRM64 )      // sse2 
inspick( haddps,        "haddps",       0x00007c0f,     0x0000ffff,     X86XMMRegModRM )        // sse3 
inspick( hsubps,        "hsubps",       0x00007d0f,     0x0000ffff,     X86XMMRegModRM )        // sse3 
inspick( lddqu00,       "lddqu",        0x0000f00f,     0x00c0ffff,     X86XMMRegModRM )        // sse3 
inspick( lddqu01,       "lddqu",        0x0040f00f,     0x00c0ffff,     X86XMMRegModRM )        // sse3 
inspick( lddqu10,       "lddqu",        0x0080f00f,     0x00c0ffff,     X86XMMRegModRM )        // sse3 
inspick( maxsd,         "maxsd",        0x00005f0f,     0x0000ffff,     X86XMMRegModRM64 )      // sse2 
inspick( minsd,         "minsd",        0x00005d0f,     0x0000ffff,     X86XMMRegModRM64 )      // sse2 
inspick( movddup,       "movddup",      0x0000120f,     0x0000ffff,     X86XMMRegModRM64 )      // sse3 
inspick( movdq2q,       "movdq2q",      0x00c0d60f,     0x00c0ffff,     X86MMRegModRMMixed )    // sse2 
inspick( movsd_1,       "movsd",        0x0000100f,     0x0000ffff,     X86XMMRegModRM64 )      // sse2 
inspick( movsd_2,       "movsd",        0x0000110f,     0x0000ffff,     X86XMMRegModRM64_Rev )  // sse2 
inspick( mulsd,         "mulsd",        0x0000590f,     0x0000ffff,     X86XMMRegModRM64 )      // sse2 
inspick( pshuflw,       "pshuflw",      0x0000700f,     0x0000ffff,     X86XMMRegModRMImm )     // sse2 
inspick( sqrtsd,        "sqrtsd",       0x0000510f,     0x0000ffff,     X86XMMRegModRM64 )      // sse2 
inspick( subsd,         "subsd",        0x00005c0f,     0x0000ffff,     X86XMMRegModRM64 )      // sse2 
