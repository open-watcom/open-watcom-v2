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
*          only for extension 3DNow!, MMX, SSE, SSE2 and SSE3 
*          without OPND_SIZE, REPNE and REPE prefixes
*
****************************************************************************/

/*
      Idx,              Name,           Opcode,         Mask,           Handler
*/

// Intel MMX, SSE, SSE2 and SSE3 instructions

inspick( addps,         "addps",        0x0000580f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( andnps,        "andnps",       0x0000550f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( andps,         "andps",        0x0000540f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( clflush00,     "clflush",      0x0038ae0f,     0x00f8ffff,     X86ModRM_24 )            // sse2 
inspick( clflush01,     "clflush",      0x0078ae0f,     0x00f8ffff,     X86ModRM_24 )            // sse2 
inspick( clflush10,     "clflush",      0x00b8ae0f,     0x00f8ffff,     X86ModRM_24 )            // sse2 
inspick( cmpps,         "cmpps",        0x0000c20f,     0x0000ffff,     X86XMMRegModRMImm )      // sse  
inspick( comiss,        "comiss",       0x00002f0f,     0x0000ffff,     X86XMMRegModRM32 )       // sse  
inspick( cvtdq2ps,      "cvtdq2ps",     0x00005b0f,     0x0000ffff,     X86XMMRegModRM )         // sse2 
inspick( cvtpi2ps,      "cvtpi2ps",     0x00002a0f,     0x0000ffff,     X86XMMRegModRMMixed )    // sse  
inspick( cvtps2pd,      "cvtps2pd",     0x00005a0f,     0x0000ffff,     X86XMMRegModRM64 )       // sse2 
inspick( cvtps2pi,      "cvtps2pi",     0x00002d0f,     0x0000ffff,     X86MMRegModRMMixed )     // sse  
inspick( cvttps2pi,     "cvttps2pi",    0x00002c0f,     0x0000ffff,     X86MMRegModRMMixed )     // sse  
inspick( divps,         "divps",        0x00005e0f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( emms,          "emms",         0x0000770f,     0x0000ffff,     X86NoOp_16 )             // mmx  
inspick( ldmxcsr00,     "ldmxcsr",      0x0010ae0f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( ldmxcsr01,     "ldmxcsr",      0x0050ae0f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( ldmxcsr10,     "ldmxcsr",      0x0090ae0f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( lfence,        "lfence",       0x00e8ae0f,     0x00ffffff,     X86NoOp_24 )             // sse2 
inspick( maskmovq,      "maskmovq",     0x00c0f70f,     0x00c0ffff,     X86MMRegModRM )          // sse  
inspick( maxps,         "maxps",        0x00005f0f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( mfence,        "mfence",       0x00f0ae0f,     0x00ffffff,     X86NoOp_24 )             // sse2 
inspick( minps,         "minps",        0x00005d0f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( monitor,       "monitor",      0x00c8010f,     0x00ffffff,     X86NoOp_24 )             // sse3 
inspick( movaps_1,      "movaps",       0x0000280f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( movaps_2,      "movaps",       0x0000290f,     0x0000ffff,     X86XMMRegModRM_Rev )     // sse  
inspick( movd_1,        "movd",         0x00006e0f,     0x0000ffff,     X86MMRegModRMMixed )     // mmx  
inspick( movd_3,        "movd",         0x00007e0f,     0x0000ffff,     X86MMRegModRMMixed_Rev ) // mmx  
inspick( movhps00_1,    "movhps",       0x0000160f,     0x00c0ffff,     X86XMMRegModRM64 )       // sse  
inspick( movhps01_1,    "movhps",       0x0040160f,     0x00c0ffff,     X86XMMRegModRM64 )       // sse  
inspick( movhps10_1,    "movhps",       0x0080160f,     0x00c0ffff,     X86XMMRegModRM64 )       // sse  
inspick( movlhps,       "movlhps",      0x00c0160f,     0x00c0ffff,     X86XMMRegModRM64 )       // sse  
inspick( movhps00_2,    "movhps",       0x0000170f,     0x00c0ffff,     X86XMMRegModRM64_Rev )   // sse  
inspick( movhps01_2,    "movhps",       0x0040170f,     0x00c0ffff,     X86XMMRegModRM64_Rev )   // sse  
inspick( movhps10_2,    "movhps",       0x0080170f,     0x00c0ffff,     X86XMMRegModRM64_Rev )   // sse  
inspick( movlps00_1,    "movlps",       0x0000120f,     0x00c0ffff,     X86XMMRegModRM64 )       // sse  
inspick( movlps01_1,    "movlps",       0x0040120f,     0x00c0ffff,     X86XMMRegModRM64 )       // sse  
inspick( movlps10_1,    "movlps",       0x0080120f,     0x00c0ffff,     X86XMMRegModRM64 )       // sse  
inspick( movhlps,       "movhlps",      0x00c0120f,     0x00c0ffff,     X86XMMRegModRM64 )       // sse  
inspick( movlps00_2,    "movlps",       0x0000130f,     0x00c0ffff,     X86XMMRegModRM64_Rev )   // sse  
inspick( movlps01_2,    "movlps",       0x0040130f,     0x00c0ffff,     X86XMMRegModRM64_Rev )   // sse  
inspick( movlps10_2,    "movlps",       0x0080130f,     0x00c0ffff,     X86XMMRegModRM64_Rev )   // sse  
inspick( movmskps,      "movmskps",     0x00c0500f,     0x00c0ffff,     X86RegModRMMixed )       // sse  
inspick( movnti00,      "movnti",       0x0000c30f,     0x00c0ffff,     X86RegModRM32_Rev )      // sse2 
inspick( movnti01,      "movnti",       0x0040c30f,     0x00c0ffff,     X86RegModRM32_Rev )      // sse2 
inspick( movnti10,      "movnti",       0x0080c30f,     0x00c0ffff,     X86RegModRM32_Rev )      // sse2 
inspick( movntps00,     "movntps",      0x00002b0f,     0x00c0ffff,     X86XMMRegModRM_Rev )     // sse  
inspick( movntps01,     "movntps",      0x00402b0f,     0x00c0ffff,     X86XMMRegModRM_Rev )     // sse  
inspick( movntps10,     "movntps",      0x00802b0f,     0x00c0ffff,     X86XMMRegModRM_Rev )     // sse  
inspick( movntq00,      "movntq",       0x0000e70f,     0x00c0ffff,     X86MMRegModRM_Rev )      // sse  
inspick( movntq01,      "movntq",       0x0040e70f,     0x00c0ffff,     X86MMRegModRM_Rev )      // sse  
inspick( movntq10,      "movntq",       0x0080e70f,     0x00c0ffff,     X86MMRegModRM_Rev )      // sse  
inspick( movq_1,        "movq",         0x00006f0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( movq_3,        "movq",         0x00007f0f,     0x0000ffff,     X86MMRegModRM_Rev )      // mmx  
inspick( movups_1,      "movups",       0x0000100f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( movups_2,      "movups",       0x0000110f,     0x0000ffff,     X86XMMRegModRM_Rev )     // sse  
inspick( mulps,         "mulps",        0x0000590f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( mwait,         "mwait",        0x00c9010f,     0x00ffffff,     X86NoOp_24 )             // sse3 
inspick( orps,          "orps",         0x0000560f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( packssdw_1,    "packssdw",     0x00006b0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( packsswb_1,    "packsswb",     0x0000630f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( packuswb_1,    "packuswb",     0x0000670f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( paddb_1,       "paddb",        0x0000fc0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( paddd_1,       "paddd",        0x0000fe0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( paddq_1,       "paddq",        0x0000d40f,     0x0000ffff,     X86MMRegModRM )          // sse2 
inspick( paddsb_1,      "paddsb",       0x0000ec0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( paddsw_1,      "paddsw",       0x0000ed0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( paddusb_1,     "paddusb",      0x0000dc0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( paddusw_1,     "paddusw",      0x0000dd0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( paddw_1,       "paddw",        0x0000fd0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( pand_1,        "pand",         0x0000db0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( pandn_1,       "pandn",        0x0000df0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( pavgb_1,       "pavgb",        0x0000e00f,     0x0000ffff,     X86MMRegModRM )          // sse  
inspick( pavgw_1,       "pavgw",        0x0000e30f,     0x0000ffff,     X86MMRegModRM )          // sse  
inspick( pcmpeqb_1,     "pcmpeqb",      0x0000740f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( pcmpeqd_1,     "pcmpeqd",      0x0000760f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( pcmpeqw_1,     "pcmpeqw",      0x0000750f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( pcmpgtb_1,     "pcmpgtb",      0x0000640f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( pcmpgtd_1,     "pcmpgtd",      0x0000660f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( pcmpgtw_1,     "pcmpgtw",      0x0000650f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( pextrw_1,      "pextrw",       0x00c0c50f,     0x00c0ffff,     X86RegModRMMixedImm )    // sse  
inspick( pinsrw00_1,    "pinsrw",       0x0000c40f,     0x00c0ffff,     X86MMRegModRMMixedImm )  // sse  
inspick( pinsrw01_1,    "pinsrw",       0x0040c40f,     0x00c0ffff,     X86MMRegModRMMixedImm )  // sse  
inspick( pinsrw10_1,    "pinsrw",       0x0080c40f,     0x00c0ffff,     X86MMRegModRMMixedImm )  // sse  
inspick( pinsrw11_1,    "pinsrw",       0x00c0c40f,     0x00c0ffff,     X86MMRegModRMMixedImm )  // sse  
inspick( pmaddwd_1,     "pmaddwd",      0x0000f50f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( pmaxsw_1,      "pmaxsw",       0x0000ee0f,     0x0000ffff,     X86MMRegModRM )          // sse  
inspick( pmaxub_1,      "pmaxub",       0x0000de0f,     0x0000ffff,     X86MMRegModRM )          // sse  
inspick( pminsw_1,      "pminsw",       0x0000ea0f,     0x0000ffff,     X86MMRegModRM )          // sse  
inspick( pminub_1,      "pminub",       0x0000da0f,     0x0000ffff,     X86MMRegModRM )          // sse  
inspick( pmovmskb_1,    "pmovmskb",     0x00c0d70f,     0x00c0ffff,     X86RegModRMMixed )       // sse  
inspick( pmulhuw_1,     "pmulhuw",      0x0000e40f,     0x0000ffff,     X86MMRegModRM )          // sse  
inspick( pmulhw_1,      "pmulhw",       0x0000e50f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( pmullw_1,      "pmullw",       0x0000d50f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( pmuludq_1,     "pmuludq",      0x0000f40f,     0x0000ffff,     X86MMRegModRM )          // sse2 
inspick( por_1,         "por",          0x0000eb0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( prefetchnta00, "prefetchnta",  0x0000180f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( prefetchnta01, "prefetchnta",  0x0040180f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( prefetchnta10, "prefetchnta",  0x0080180f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( prefetcht000,  "prefetcht0",   0x0008180f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( prefetcht001,  "prefetcht0",   0x0048180f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( prefetcht010,  "prefetcht0",   0x0088180f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( prefetcht100,  "prefetcht1",   0x0010180f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( prefetcht101,  "prefetcht1",   0x0050180f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( prefetcht110,  "prefetcht1",   0x0090180f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( prefetcht200,  "prefetcht2",   0x0018180f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( prefetcht201,  "prefetcht2",   0x0058180f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( prefetcht210,  "prefetcht2",   0x0098180f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( psadbw_1,      "psadbw",       0x0000f60f,     0x0000ffff,     X86MMRegModRM )          // sse  
inspick( pshufw,        "pshufw",       0x0000700f,     0x0000ffff,     X86MMRegModRMImm )       // sse  
inspick( pslld_1,       "pslld",        0x00f0720f,     0x00f8ffff,     X86MMRegImm )            // mmx  
inspick( pslld_3,       "pslld",        0x0000f20f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psllq_1,       "psllq",        0x00f0730f,     0x00f8ffff,     X86MMRegImm )            // mmx  
inspick( psllq_3,       "psllq",        0x0000f30f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psllw_1,       "psllw",        0x00f0710f,     0x00f8ffff,     X86MMRegImm )            // mmx  
inspick( psllw_3,       "psllw",        0x0000f10f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psrad_1,       "psrad",        0x00e0720f,     0x00f8ffff,     X86MMRegImm )            // mmx  
inspick( psrad_3,       "psrad",        0x0000e20f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psraw_1,       "psraw",        0x00e0710f,     0x00f8ffff,     X86MMRegImm )            // mmx  
inspick( psraw_3,       "psraw",        0x0000e10f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psrld_1,       "psrld",        0x00d0720f,     0x00f8ffff,     X86MMRegImm )            // mmx  
inspick( psrld_3,       "psrld",        0x0000d20f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psrlq_1,       "psrlq",        0x00d0730f,     0x00f8ffff,     X86MMRegImm )            // mmx  
inspick( psrlq_3,       "psrlq",        0x0000d30f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psrlw_1,       "psrlw",        0x00d0710f,     0x00f8ffff,     X86MMRegImm )            // mmx  
inspick( psrlw_3,       "psrlw",        0x0000d10f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psubb_1,       "psubb",        0x0000f80f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psubd_1,       "psubd",        0x0000fa0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psubq_1,       "psubq",        0x0000fb0f,     0x0000ffff,     X86MMRegModRM )          // sse2 
inspick( psubsb_1,      "psubsb",       0x0000e80f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psubsw_1,      "psubsw",       0x0000e90f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psubusb_1,     "psubusb",      0x0000d80f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psubusw_1,     "psubusw",      0x0000d90f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( psubw_1,       "psubw",        0x0000f90f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( punpckhbw_1,   "punpckhbw",    0x0000680f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( punpckhdq_1,   "punpckhdq",    0x00006a0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( punpckhwd_1,   "punpckhwd",    0x0000690f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( punpcklbw_1,   "punpcklbw",    0x0000600f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( punpckldq_1,   "punpckldq",    0x0000620f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( punpcklwd_1,   "punpcklwd",    0x0000610f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( pxor_1,        "pxor",         0x0000ef0f,     0x0000ffff,     X86MMRegModRM )          // mmx  
inspick( rcpps,         "rcpps",        0x0000530f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( rsqrtps,       "rsqrtps",      0x0000520f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( sfence,        "sfence",       0x00f8ae0f,     0x00ffffff,     X86NoOp_24 )             // sse  
inspick( shufps,        "shufps",       0x0000c60f,     0x0000ffff,     X86XMMRegModRMImm )      // sse  
inspick( sqrtps,        "sqrtps",       0x0000510f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( stmxcsr00,     "stmxcsr",      0x0018ae0f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( stmxcsr01,     "stmxcsr",      0x0058ae0f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( stmxcsr10,     "stmxcsr",      0x0098ae0f,     0x00f8ffff,     X86ModRM_24 )            // sse  
inspick( subps,         "subps",        0x00005c0f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( ucomiss,       "ucomiss",      0x00002e0f,     0x0000ffff,     X86XMMRegModRM32 )       // sse  
inspick( unpckhps,      "unpckhps",     0x0000150f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( unpcklps,      "unpcklps",     0x0000140f,     0x0000ffff,     X86XMMRegModRM )         // sse  
inspick( xorps,         "xorps",        0x0000570f,     0x0000ffff,     X86XMMRegModRM )         // sse
