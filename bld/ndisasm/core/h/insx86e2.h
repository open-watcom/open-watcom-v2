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
*          only for extension SSE, SSE2 and SSE3 with OPND_SIZE prefix (0x66)
*
****************************************************************************/

/*
   All these instructions required only mandatory prefix 0x66


      Idx,              Name,           Opcode,         Mask,           Handler
*/

inspick( addpd,         "addpd",        0x0000580f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( addsubpd,      "addsubpd",     0x0000d00f,     0x0000ffff,     X86XMMRegModRM )          // sse3 
inspick( andnpd,        "andnpd",       0x0000550f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( andpd,         "andpd",        0x0000540f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( cmppd,         "cmppd",        0x0000c20f,     0x0000ffff,     X86XMMRegModRMImm )       // sse2 
inspick( comisd,        "comisd",       0x00002f0f,     0x0000ffff,     X86XMMRegModRM64 )        // sse2 
inspick( cvtpd2pi,      "cvtpd2pi",     0x00002d0f,     0x0000ffff,     X86MMRegModRMMixed )      // sse2 
inspick( cvtpd2ps,      "cvtpd2ps",     0x00005a0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( cvtpi2pd,      "cvtpi2pd",     0x00002a0f,     0x0000ffff,     X86XMMRegModRMMixed )     // sse2 
inspick( cvtps2dq,      "cvtps2dq",     0x00005b0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( cvttpd2dq,     "cvttpd2dq",    0x0000e60f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( cvttpd2pi,     "cvttpd2pi",    0x00002c0f,     0x0000ffff,     X86MMRegModRMMixed )      // sse2 
inspick( divpd,         "divpd",        0x00005e0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( haddpd,        "haddpd",       0x00007c0f,     0x0000ffff,     X86XMMRegModRM )          // sse3 
inspick( hsubpd,        "hsubpd",       0x00007d0f,     0x0000ffff,     X86XMMRegModRM )          // sse3 
inspick( maskmovdqu,    "maskmovdqu",   0x00c0f70f,     0x00c0ffff,     X86XMMRegModRM )          // sse2 
inspick( maxpd,         "maxpd",        0x00005f0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( minpd,         "minpd",        0x00005d0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( movapd_1,      "movapd",       0x0000280f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( movapd_2,      "movapd",       0x0000290f,     0x0000ffff,     X86XMMRegModRM_Rev )      // sse2 
inspick( movd_2,        "movd",         0x00006e0f,     0x0000ffff,     X86XMMRegModRMMixed )     // sse2 
inspick( movd_4,        "movd",         0x00007e0f,     0x0000ffff,     X86XMMRegModRMMixed_Rev ) // sse2 
inspick( movdqa_1,      "movdqa",       0x00006f0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( movdqa_2,      "movdqa",       0x00007f0f,     0x0000ffff,     X86XMMRegModRM_Rev )      // sse2 
inspick( movhpd00_1,    "movhpd",       0x0000160f,     0x00c0ffff,     X86XMMRegModRM64 )        // sse2 
inspick( movhpd01_1,    "movhpd",       0x0040160f,     0x00c0ffff,     X86XMMRegModRM64 )        // sse2 
inspick( movhpd10_1,    "movhpd",       0x0080160f,     0x00c0ffff,     X86XMMRegModRM64 )        // sse2 
inspick( movhpd00_2,    "movhpd",       0x0000170f,     0x00c0ffff,     X86XMMRegModRM64_Rev )    // sse2 
inspick( movhpd01_2,    "movhpd",       0x0040170f,     0x00c0ffff,     X86XMMRegModRM64_Rev )    // sse2 
inspick( movhpd10_2,    "movhpd",       0x0080170f,     0x00c0ffff,     X86XMMRegModRM64_Rev )    // sse2 
inspick( movlpd00_1,    "movlpd",       0x0000120f,     0x00c0ffff,     X86XMMRegModRM64 )        // sse2 
inspick( movlpd01_1,    "movlpd",       0x0040120f,     0x00c0ffff,     X86XMMRegModRM64 )        // sse2 
inspick( movlpd10_1,    "movlpd",       0x0080120f,     0x00c0ffff,     X86XMMRegModRM64 )        // sse2 
inspick( movlpd00_2,    "movlpd",       0x0000130f,     0x00c0ffff,     X86XMMRegModRM64_Rev )    // sse2 
inspick( movlpd01_2,    "movlpd",       0x0040130f,     0x00c0ffff,     X86XMMRegModRM64_Rev )    // sse2 
inspick( movlpd10_2,    "movlpd",       0x0080130f,     0x00c0ffff,     X86XMMRegModRM64_Rev )    // sse2 
inspick( movmskpd,      "movmskpd",     0x00c0500f,     0x00c0ffff,     X86RegModRMMixed )        // sse2 
inspick( movntdq00,     "movntdq",      0x0000e70f,     0x00c0ffff,     X86XMMRegModRM_Rev )      // sse2 
inspick( movntdq01,     "movntdq",      0x0040e70f,     0x00c0ffff,     X86XMMRegModRM_Rev )      // sse2 
inspick( movntdq10,     "movntdq",      0x0080e70f,     0x00c0ffff,     X86XMMRegModRM_Rev )      // sse2 
inspick( movntpd00,     "movntpd",      0x00002b0f,     0x00c0ffff,     X86XMMRegModRM_Rev )      // sse2 
inspick( movntpd01,     "movntpd",      0x00402b0f,     0x00c0ffff,     X86XMMRegModRM_Rev )      // sse2 
inspick( movntpd10,     "movntpd",      0x00802b0f,     0x00c0ffff,     X86XMMRegModRM_Rev )      // sse2 
inspick( movq_4,        "movq",         0x0000d60f,     0x0000ffff,     X86XMMRegModRM64_Rev )    // sse2 
inspick( movupd_1,      "movupd",       0x0000100f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( movupd_2,      "movupd",       0x0000110f,     0x0000ffff,     X86XMMRegModRM_Rev )      // sse2 
inspick( mulpd,         "mulpd",        0x0000590f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( orpd,          "orpd",         0x0000560f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( packssdw_2,    "packssdw",     0x00006b0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( packsswb_2,    "packsswb",     0x0000630f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( packuswb_2,    "packuswb",     0x0000670f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( paddb_2,       "paddb",        0x0000fc0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( paddd_2,       "paddd",        0x0000fe0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( paddq_2,       "paddq",        0x0000d40f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( paddsb_2,      "paddsb",       0x0000ec0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( paddsw_2,      "paddsw",       0x0000ed0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( paddusb_2,     "paddusb",      0x0000dc0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( paddusw_2,     "paddusw",      0x0000dd0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( paddw_2,       "paddw",        0x0000fd0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pand_2,        "pand",         0x0000db0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pandn_2,       "pandn",        0x0000df0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pavgb_2,       "pavgb",        0x0000e00f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pavgw_2,       "pavgw",        0x0000e30f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pcmpeqb_2,     "pcmpeqb",      0x0000740f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pcmpeqd_2,     "pcmpeqd",      0x0000760f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pcmpeqw_2,     "pcmpeqw",      0x0000750f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pcmpgtb_2,     "pcmpgtb",      0x0000640f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pcmpgtd_2,     "pcmpgtd",      0x0000660f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pcmpgtw_2,     "pcmpgtw",      0x0000650f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pextrw_2,      "pextrw",       0x00c0c50f,     0x00c0ffff,     X86RegModRMMixedImm )     // sse2 
inspick( pinsrw00_2,    "pinsrw",       0x0000c40f,     0x00c0ffff,     X86XMMRegModRMMixedImm )  // sse2 
inspick( pinsrw01_2,    "pinsrw",       0x0040c40f,     0x00c0ffff,     X86XMMRegModRMMixedImm )  // sse2 
inspick( pinsrw10_2,    "pinsrw",       0x0080c40f,     0x00c0ffff,     X86XMMRegModRMMixedImm )  // sse2 
inspick( pinsrw11_2,    "pinsrw",       0x00c0c40f,     0x00c0ffff,     X86XMMRegModRMMixedImm )  // sse2 
inspick( pmaddwd_2,     "pmaddwd",      0x0000f50f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pmaxsw_2,      "pmaxsw",       0x0000ee0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pmaxub_2,      "pmaxub",       0x0000de0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pminsw_2,      "pminsw",       0x0000ea0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pminub_2,      "pminub",       0x0000da0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pmovmskb_2,    "pmovmskb",     0x00c0d70f,     0x00c0ffff,     X86RegModRMMixed )        // sse2 
inspick( pmulhuw_2,     "pmulhuw",      0x0000e40f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pmulhw_2,      "pmulhw",       0x0000e50f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pmullw_2,      "pmullw",       0x0000d50f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pmuludq_2,     "pmuludq",      0x0000f40f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( por_2,         "por",          0x0000eb0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psadbw_2,      "psadbw",       0x0000f60f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pshufd,        "pshufd",       0x0000700f,     0x0000ffff,     X86XMMRegModRMImm )       // sse2 
inspick( pslldq,        "pslldq",       0x00f8730f,     0x00f8ffff,     X86XMMRegImm )            // sse2 
inspick( pslld_2,       "pslld",        0x00f0720f,     0x00f8ffff,     X86XMMRegImm )            // sse2 
inspick( pslld_4,       "pslld",        0x0000f20f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psllq_2,       "psllq",        0x00f0730f,     0x00f8ffff,     X86XMMRegImm )            // sse2 
inspick( psllq_4,       "psllq",        0x0000f30f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psllw_2,       "psllw",        0x00f0710f,     0x00f8ffff,     X86XMMRegImm )            // sse2 
inspick( psllw_4,       "psllw",        0x0000f10f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psrad_2,       "psrad",        0x00e0720f,     0x00f8ffff,     X86XMMRegImm )            // sse2 
inspick( psrad_4,       "psrad",        0x0000e20f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psraw_2,       "psraw",        0x00e0710f,     0x00f8ffff,     X86XMMRegImm )            // sse2 
inspick( psraw_4,       "psraw",        0x0000e10f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psrldq,        "psrldq",       0x00d8730f,     0x00f8ffff,     X86XMMRegImm )            // sse2 
inspick( psrld_2,       "psrld",        0x00d0720f,     0x00f8ffff,     X86XMMRegImm )            // sse2 
inspick( psrld_4,       "psrld",        0x0000d20f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psrlq_2,       "psrlq",        0x00d0730f,     0x00f8ffff,     X86XMMRegImm )            // sse2 
inspick( psrlq_4,       "psrlq",        0x0000d30f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psrlw_2,       "psrlw",        0x00d0710f,     0x00f8ffff,     X86XMMRegImm )            // sse2 
inspick( psrlw_4,       "psrlw",        0x0000d10f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psubb_2,       "psubb",        0x0000f80f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psubd_2,       "psubd",        0x0000fa0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psubq_2,       "psubq",        0x0000fb0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psubsb_2,      "psubsb",       0x0000e80f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psubsw_2,      "psubsw",       0x0000e90f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psubusb_2,     "psubusb",      0x0000d80f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psubusw_2,     "psubusw",      0x0000d90f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( psubw_2,       "psubw",        0x0000f90f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( punpckhqdq,    "punpckhqdq",   0x00006d0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( punpckhbw_2,   "punpckhbw",    0x0000680f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( punpckhdq_2,   "punpckhdq",    0x00006a0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( punpckhwd_2,   "punpckhwd",    0x0000690f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( punpcklqdq,    "punpcklqdq",   0x00006c0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( punpcklbw_2,   "punpcklbw",    0x0000600f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( punpckldq_2,   "punpckldq",    0x0000620f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( punpcklwd_2,   "punpcklwd",    0x0000610f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( pxor_2,        "pxor",         0x0000ef0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( shufpd,        "shufpd",       0x0000c60f,     0x0000ffff,     X86XMMRegModRMImm )       // sse2 
inspick( sqrtpd,        "sqrtpd",       0x0000510f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( subpd,         "subpd",        0x00005c0f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( ucomisd,       "ucomisd",      0x00002e0f,     0x0000ffff,     X86XMMRegModRM64 )        // sse2 
inspick( unpckhpd,      "unpckhpd",     0x0000150f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( unpcklpd,      "unpcklpd",     0x0000140f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
inspick( xorpd,         "xorpd",        0x0000570f,     0x0000ffff,     X86XMMRegModRM )          // sse2 
