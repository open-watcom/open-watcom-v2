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
* Description:  Reductions used by the Intel x86 cg (see i86/386table.c).
*
****************************************************************************/


_R_( R_CHANGESHIFT,     rCHANGESHIFT ),
_R_( R_FIXSHIFT,        rFIXSHIFT ),
_R_( R_CLRHIGH_B,       rCLRHI_BW ),
_R_( R_CLRHIGH_W,       rCLRHI_BW ),
_R_( R_CONVERT_LOW,     rCONVERT_LOW ),
_R_( R_CYPHIGH,         rCYPHIGH ),
_R_( R_CYPLOW,          rCYPLOW ),
_R_( R_DOCVT,           rDOCVT ),
_R_( R_DOLONGPUSH,      rDOLONGPUSH ),
_R_( R_DOUBLEHALF,      rDOUBLEHALF ),
_R_( R_FDCONSCMP,       rMAKECALL ),
_R_( R_FORCEOP1CMEM,    rOP1CMEM ),
_R_( R_FORCEOP1MEM,     rOP1MEM ),
_R_( R_FORCEOP2CMEM,    rOP2CMEM ),
_R_( R_FORCERESMEM,     rFORCERESMEM ),
_R_( R_FSCONSCMP,       rFSCONSCMP ),
_R_( R_HIGHCMP,         rHIGHCMP ),
_R_( R_HIGHLOWMOVE,     rHIGHLOWMOVE ),
_R_( R_LOADLONGADDR,    rLOADLONGADDR ),
_R_( R_MAKECALL,        rMAKECALL ),
_R_( R_MAKECMPCALL,     rMAKECALL ),
_R_( R_MAKECYPMUL,      rMAKECYPMUL ),
_R_( R_MAKEFNEG,        rMAKEFNEG ),
_R_( R_MAKEMOVE,        rMAKEMOVE ),
_R_( R_MAKESTRCMP,      rMAKESTRCMP ),
_R_( R_MAKESTRMOVE,     rMAKESTRMOVE ),
_R_( R_MAKEU2,          rMAKEU2 ),
_R_( R_MAKEU4,          rMAKEU4 ),
_R_( R_MAKEXORRR,       rMAKEXORRR ),
_R_( R_MOVEINDEX,       rMOVEINDEX ),
_R_( R_MOVELOW,         rMOVELOW ),
_R_( R_MOVOP1TEMP,      rMOVOP1TEMP ),
_R_( R_MOVOP2,          rMOVOP2 ),
_R_( R_MOVOP2TEMP,      rMOVOP2TEMP ),
_R_( R_MOVOP1MEM,       rMOVOP1MEM ),
_R_( R_MOVOP1REG,       rOP1REG ),
_R_( R_MOVOP1RES,       rMOVOP1RES ),
_R_( R_MOVRESREG,       rMOVRESREG ),
_R_( R_OP2CL,           rOP2CL ),
_R_( R_OP2CX,           rOP2CX ),
_R_( R_OP1RESTEMP,      rOP1RESTEMP ),
_R_( R_RESREG,          rRESREG ),
_R_( R_SPLIT8,          rSPLIT8 ),
_R_( R_SPLITCMP,        rSPLITCMP ),
_R_( R_SPLITMOVE,       rSPLITMOVE ),
_R_( R_SPLITOP,         rSPLITOP ),
_R_( R_SPLITUNARY,      rSPLITUNARY ),
_R_( R_SWAPCMP,         rSWAPCMP ),
_R_( R_SWAPOPS,         rSWAPOPS ),
_R_( R_USEREGISTER,     rUSEREGISTER ),
_R_( R_MULREGISTER,     rMULREGISTER ),
_R_( R_DIVREGISTER,     rDIVREGISTER ),
_R_( R_OP1RESREG,       rOP1RESREG ),
_R_( R_SPLITNEG,        rSPLITNEG ),
_R_( R_BYTESHIFT,       rBYTESHIFT ),
_R_( R_CYPSHIFT,        rCYPSHIFT ),
_R_( R_LOADOP2,         rLOADOP2 ),
_R_( R_MAKEADD,         rMAKEADD ),
_R_( R_MAKENEG,         rMAKENEG ),
_R_( R_MAKESUB,         rMAKESUB ),
_R_( R_U_TEST,          rU_TEST ),
_R_( R_CMPtrue,         rCMPtrue ),
_R_( R_CMPfalse,        rCMPfalse ),
_R_( R_PTCADD,          NULL ),
_R_( R_PTVADD,          NULL ),
_R_( R_PTCSUB,          NULL ),
_R_( R_PTVSUB,          NULL ),
_R_( R_CPSUB,           rCPSUB ),
_R_( R_PTSUB,           rPTSUB ),
_R_( R_NEGADD,          rNEGADD ),
_R_( R_EXTPT,           rEXTPT ),
_R_( R_MAYBSTRMOVE,     rMAYBSTRMOVE ),
_R_( R_FORCEOP2MEM,     rOP2MEM ),
_R_( R_SEG_SEG,         rSEG_SEG ),
_R_( R_CHPPT,           rCHPPT ),
_R_( R_CLRHIGH_D,       rCLRHI_D ),
_R_( R_MOVRESMEM,       rMOVRESMEM ),
_R_( R_MAKEU4CONS,      rMAKEU4CONS ),
_R_( R_ADDRR,           rADDRR ),
_R_( R_SPLITPUSH,       rSPLITPUSH ),
_R_( R_EXT_PUSHC,       rEXT_PUSHC ),
_R_( R_EXT_PUSH1,       rEXT_PUSH1 ),
_R_( R_EXT_PUSH2,       rEXT_PUSH2 ),
_R_( R_CLRHIGH_R,       rCLRHI_R ),
_R_( R_MOVOP2RES,       rMOVOP2RES ),
_R_( R_INTCOMP,         rINTCOMP ),
_R_( R_CDQ,             rCDQ ),
_R_( R_CYP_SEX,         rCYP_SEX ),
_R_( R_FLIPSIGN,        rFLIPSIGN ),
_R_( R_TEMP2CONST,      rTEMP2CONST ),
_R_( R_SAVEFACE,        rSAVEFACE ),
_R_( R_MULSAVEFACE,     rMULSAVEFACE ),
_R_( R_CONVERT_UP,      rCONVERT_UP ),
_R_( R_SPLIT8BIN,       rSPLIT8BIN ),
_R_( R_SPLIT8NEG,       rSPLIT8NEG ),
_R_( R_SPLIT8TST,       rSPLIT8TST ),
_R_( R_SPLIT8CMP,       rSPLIT8CMP ),
_R_( R_MOVE8LOW,        rMOVE8LOW ),
_R_( R_CMPCP,           rCMPCP ),
_R_( R_MOVPTI8,         rMOVPTI8 ),
_R_( R_MOVI8PT,         rMOVI8PT ),
