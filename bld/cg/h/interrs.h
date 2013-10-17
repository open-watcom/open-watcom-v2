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
* Description:  Internal error constants.
*
****************************************************************************/


typedef enum {
    ZOIKS_000,  /*    these aren't too bad */
    ZOIKS_001,  /*  unfreed code labels */
    ZOIKS_002,  /*  chunks unfreed */
    ZOIKS_003,  /*  Oh! Scary Kids!! Non-NULL AddrList in BGReturn */
    ZOIKS_004,  /*  ENH - out of handles */
    ZOIKS_005,  /*  Debug definition too long */
    ZOIKS_006,  /*  DOSSYSIO: Error patching object deck */
    ZOIKS_007,  /*  FLOWGRAPH - depth < 0 */
    ZOIKS_008,  /*  ExpMove: invalid operand classification */
    ZOIKS_009,  /*  ExpMove: Missed a case! */
    ZOIKS_010,  /*  ExpBinary: invalid operand classification */
    ZOIKS_011,  /*  ExpBinary: Missed a case! */
    ZOIKS_012,  /*  ExpCompare: invalid operand classification */
    ZOIKS_013,  /*  ExpCompare: Missed a case! */
    ZOIKS_014,  /*  ExpEQR1: Illegal operands classification 1 */
    ZOIKS_015,  /*  ExpEQR1: Illegal operands classification 2 */
    ZOIKS_016,  /*  ExpEQR2: Illegal operands classification 1 */
    ZOIKS_017,  /*  ExpEQR2: Illegal operands classification 2 */
    ZOIKS_018,  /*  ExpEQR1: Missed a case! */
    ZOIKS_019,  /*  GENRED: Unexpected reduction */
    ZOIKS_020,  /*  DupSeg - already have a segment */
    ZOIKS_021,  /*  GENRTN: Unknown runtime call */
    ZOIKS_022,  /*  Unknown register in SegName */
    ZOIKS_023,  /*  GenSubs - bad operands type */
    ZOIKS_024,  /*  Unexpected an_format in Points */
    ZOIKS_025,  /*  MAKEADDR: NF_BOOL in GenIns */
    ZOIKS_026,  /*  NewIns - too many operands */
    ZOIKS_027,  /*  PCENCODE: Bad segment override */
    ZOIKS_028,  /*  PCENCODE: Gen routine not handled */
    ZOIKS_029,  /*  PCENCODE: Unknown MF */
    ZOIKS_030,  /*  LayModRM: unallocated temporary */
    ZOIKS_031,  /*  PCENCODE: Undecipherable register */
    ZOIKS_032,  /*  PCENCODE: Undecipherable segment register */
    ZOIKS_033,  /*  PCENCODE: Undecipherable index register */
    ZOIKS_034,  /*  DoDisp: unallocated temporary */
    ZOIKS_035,  /*  PCENCODE: unexpected address of a constant */
    ZOIKS_036,  /*  PCESCAPE: unknown OC class */
    ZOIKS_037,  /*  PCESCAPE: unknown OC class */
    ZOIKS_038,  /*  PCESCAPE: Unknown escape */
    ZOIKS_039,  /*  PCOBJECT: Unresolved pointer reference */
    ZOIKS_040,  /*  REGASSGN: need a register but didn't get one */
    ZOIKS_041,  /*  TGetConst - not a constant */
    ZOIKS_042,  /*  ***Bad label*** */
    ZOIKS_043,  /*  Addr name is not NF_INS in BGCall */
    ZOIKS_044,  /*  PCENCODE: Cannot make U1/I1 relocatable constant */
    ZOIKS_045,  /*  PCENCODE: Relocatable constant is not 2 bytes */
    ZOIKS_046,  /*  SCINFO  : Relocatable constant is not 2 bytes */
    ZOIKS_047,  /*  GENRTN  : Took the address of a register/constant! */
    ZOIKS_048,  /*  SCHASH  : ran out of id's */
    ZOIKS_049,  /*  FIXRETURNS: no way out of remote block */
    ZOIKS_050,  /*  ReplIns - cannot move conflict to new ins! */
    ZOIKS_051,  /*  LongPush- unexpected type */
    ZOIKS_052,  /*  OpTab - illegal type for operation */
    ZOIKS_053,  /*  VERIFY: - V_ routine not handled */
    ZOIKS_054,  /*  TGBinary -- op not handled */
    ZOIKS_055,  /*  TGUnary  -- op not handled */
    ZOIKS_056,  /*  TGFlow  -- op not handled */
    ZOIKS_057,  /*  DoTreeGen  -- tree node not handled */
    ZOIKS_058,  /*  S37CONST -- couldn't change instr back */
    ZOIKS_059,  /*  S37CONST -- general foul up */
    ZOIKS_060,  /*  S37ENC -- name addressing mode not known for 370 */
    ZOIKS_061,  /*  S37TXT -- unknown 370 bead */
    ZOIKS_062,  /*  S37TXT -- #disp bigger than 4K addressability */
    ZOIKS_063,  /*  S37TXT -- bad hwinst bead */
    ZOIKS_064,  /*  S37TXT -- unexpanded bead tag */
    ZOIKS_065,  /*  S37TXT -- name required for 370 object */
    ZOIKS_066,  /*  S37PAGE -- name label for func */
    ZOIKS_067,  /*  BENewBack -- bad back handle allocation */
    ZOIKS_068,  /*  INTRFACE -- fake back handle not expected */
    ZOIKS_069,  /*  Renumber() -- too many instructions */
    ZOIKS_070,  /*  BGParmInline -- type mismatch */
    ZOIKS_071,  /*  BGRetInline -- type mismatch */
    ZOIKS_072,  /*  BGProcInline -- unexpected procedure */
    ZOIKS_073,  /*  I86LDSTR -- bad reduction */
    ZOIKS_074,  /*  I87REG -- block changes stack depth */
    ZOIKS_075,  /*  I87SCHED -- bad gen routine */
    ZOIKS_076,  /*  I87SCHED -- can't find register */
    ZOIKS_077,  /*  I86PROC -- can't calculate stack depth */
    ZOIKS_078,  /*  Unexpected case in if or switch etc.. */
    ZOIKS_079,  /*  I86ENC32 -- bad effective address */
    ZOIKS_080,  /*  I86OBJ -- mixing comdat/comdef with same symbol */
    ZOIKS_081,  /*  INSSCHED -- can't find a damn instruction */
    ZOIKS_082,  /*  TREE -- converting a based pointer into a non-pointer */
    ZOIKS_083,  /*  DBSUPP -- screwy location expression operator */
    ZOIKS_084,  /*  DBSUPP -- invalid ZEX size */
    ZOIKS_085,  /*  DBSUPP -- invalid cg_type for points operation */
    ZOIKS_086,  /*  CSE -- Using a segment constant as a pointer */
    ZOIKS_087,  /*  CSE -- Screwy base class for indexed name */
    ZOIKS_088,  /*  I86OBJ -- Procedure not in EXEC segment */
    ZOIKS_089,  /*  BGSelRange -- illegal range given (signs do not match) */
    ZOIKS_090,  /*  BGSelect -- no valid switch types are allowed */
    ZOIKS_091,  /*  NYI - not yet implemeted */
    ZOIKS_092,  /*  Assert failed */
    ZOIKS_093,  /*  Bad Conversion attempted */
    ZOIKS_094,  /*  No dominator found */
    ZOIKS_095,  /*  Made Windows prolog for a routine which had AX live on entry */
    ZOIKS_096,  /*  ALPHA - comparison of short or byte found */
    ZOIKS_097,  /*  The instruction with G_UNKNOWN encoding being emitted */
    ZOIKS_098,  /*  Invalid alignment found on addr node in Points */
    ZOIKS_099,  /*  Frame size too large in AXP GenProlog */
    ZOIKS_100,  /*  64-bit operation unsupported on PPC */
    ZOIKS_101,  /*  Looking up a convert routine for the Alpha (should be unused) */
    ZOIKS_102,  /*  BLDSEL - cannot derive an unsigned type */
    ZOIKS_103,  /*  CSE - HoistLooksGood - partition problem */
    ZOIKS_104,  /*  CSE - HoistLooksGood - could not decide */
    ZOIKS_105,  /*  CSE - PropOpnd - unexpected constant type */
    ZOIKS_106,  /*  CVTYPES - should never get here */
    ZOIKS_107,  /*  DFSYMS - should never get here */
    ZOIKS_108,  /*  DFTYPES - should never get here */
    ZOIKS_109,  /*  DOMINATE - should never get here */
    ZOIKS_110,  /*  NTBLIPS - should never get here */
    ZOIKS_111,  /*  PEEPOPT - should never get here */
    ZOIKS_112,  /*  TREEFOLD - should never get here */
    ZOIKS_113,  /*  UNROLL - should never get here */
    ZOIKS_114,  /*  i86enc - should never get here */
    ZOIKS_115,  /*  i86segs - should never get here */
    ZOIKS_116,  /*  i87reg - should never get here */
    ZOIKS_117,  /*  s37enc - should never get here */
    ZOIKS_118,  /*  ppcrgtbl - should never get here */
    ZOIKS_119,  /*  ppcenc - should never get here */
    ZOIKS_120,  /*  rscsplit - should never get here */
    ZOIKS_121,  /*  rscptype - should never get here */
    ZOIKS_122,  /*  axpenc - should never get here */
    ZOIKS_123,  /*  axpptype - should never get here */
    ZOIKS_124,  /*  axprgtbl - should never get here */
    ZOIKS_125,  /*  i86splt2 - should never get here */
    ZOIKS_126,  /*  i86enc32 - should never get here */
    ZOIKS_127,  /*  386ptype - should never get here */
    ZOIKS_128,  /*  386rtrtn - should never get here */
    ZOIKS_129,  /*  386splt2 - should never get here */
    ZOIKS_130,  /*  intrface - should never get here */
    ZOIKS_131,  /*  frame >= 32k on Alpha */
    ZOIKS_132,  /*  offset >= 0x7fff8000 on lea of global addr */
    ZOIKS_133,  /*  trying to reserve len bytes of stack where len % 4 != 0 */
    ZOIKS_134,  /*  scaled index var with scale > 4 */
    ZOIKS_135,  /*  i86splt2@Split8Name - can't figure operand class */
    ZOIKS_136,  /*  i86splt2@Split8Name - can't split constant type */
    ZOIKS_137,  /*  trying to convert struct to something */
    ZOIKS_138,  /*  putting non-fp constant in memory - Addressable */
    ZOIKS_139,  /*  struct type in loop comparison */
    ZOIKS_140,  /*  too many blocks to calculate dominators */
    ZOIKS_141,  /*  regalloc - missed last instruction in conflict block */
    ZOIKS_142,  /*  attempt to use OP_BLOCK as usual instruction */
    ZOIKS_143,  /*  register set table too long, increase MAX_RG */
    ZOIKS_LAST
} internal_errors;
