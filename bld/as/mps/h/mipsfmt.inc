/*****************************************************************************
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
* Description:  MIPS instruction formats.
*
*****************************************************************************/


PICK( IT_MEMORY_ALL,    ITMemAll,       OP_GPR, (OP_REG_INDIRECT | OP_IMMED), OP_NOTHING )              /* lw      */
PICK( IT_FP_MEMORY_ALL, ITMemAll,       OP_FPR, OP_REG_INDIRECT, OP_NOTHING )                           /* ldf     */
PICK( IT_MEMORY_A,      ITMemA,         OP_GPR, OP_NOTHING, OP_NOTHING )                                /* rpcc    */
PICK( IT_MEMORY_B,      ITMemB,         OP_REG_INDIRECT, OP_NOTHING, OP_NOTHING )                       /* fetch   */
PICK( IT_MEMORY_NONE,   ITMemNone,      OP_NOTHING, OP_NOTHING, OP_NOTHING )                            /* mb      */
PICK( IT_REG_JUMP,      ITRegJump,      OP_GPR, (OP_GPR | OP_NOTHING), OP_NOTHING )                     /* jalr    */
PICK( IT_JUMP,          ITJump,         (OP_GPR | OP_IMMED), (OP_GPR | OP_NOTHING), OP_NOTHING )        /* j       */
PICK( IT_BRANCH,        ITBranch,       (OP_GPR | OP_IMMED), (OP_IMMED | OP_NOTHING), OP_NOTHING )      /* bsr     */
PICK( IT_BRANCH_TWO,    ITBranchTwo,    OP_GPR, OP_GPR, OP_IMMED )                                      /* beq     */
PICK( IT_BRANCH_ZERO,   ITBranchZero,   OP_GPR, OP_IMMED, OP_NOTHING )                                     /* blez    */
PICK( IT_BRANCH_COP,    ITBranchCop,    OP_IMMED, OP_NOTHING, OP_NOTHING )                              /* bc0f    */
PICK( IT_FP_BRANCH,     ITBranch,       OP_FPR, OP_IMMED, OP_NOTHING )                                  /* fbeq    */
PICK( IT_MF_SPECIAL,    ITMovFromSpc,   OP_GPR, OP_NOTHING, OP_NOTHING )                                /* mflo    */
PICK( IT_MT_SPECIAL,    ITMovToSpc,     OP_GPR, OP_NOTHING, OP_NOTHING )                                /* mflo    */
PICK( IT_MOV_COP,       ITMovCop,       OP_GPR, OP_GPR, OP_NOTHING )                                    /* mtc0    */
PICK( IT_MOV_FP,        ITMovCop,       OP_GPR, OP_FPR, OP_NOTHING )                                    /* mtc1    */
PICK( IT_MUL_DIV,       ITMulDiv,       OP_GPR, OP_GPR, OP_NOTHING )                                    /* mult    */
PICK( IT_OPERATE,       ITOperate,      OP_GPR, OP_GPR, (OP_GPR | OP_IMMED) )                           /* add     */
PICK( IT_OPERATE_IMM,   ITOperateImm,   OP_GPR, OP_GPR, OP_IMMED )                                      /* addi    */
PICK( IT_OPERATE_SHF,   ITShiftImm,     OP_GPR, OP_GPR, OP_IMMED )                                      /* sll     */
PICK( IT_FP_OPERATE,    ITFPOperate,    OP_FPR, OP_FPR, OP_FPR )                                        /* cpys    */
PICK( IT_FP_CONVERT,    ITFPConvert,    OP_FPR, OP_FPR, OP_NOTHING )                                    /* cvtqs   */
PICK( IT_BR,            ITBr,           (OP_GPR | OP_IMMED), (OP_IMMED | OP_NOTHING), OP_NOTHING )      /* br      */
PICK( IT_MT_MF_FPCR,    ITMTMFFpcr,     OP_FPR, (OP_FPR | OP_NOTHING), OP_FPR )                         /* mf_fpcr */
PICK( IT_RET,           ITRet,          (OP_GPR | OP_NOTHING | OP_REG_INDIRECT | OP_IMMED), (OP_REG_INDIRECT | OP_NOTHING | OP_IMMED), (OP_IMMED | OP_NOTHING) ) /* ret */
PICK( IT_SYSCODE,       ITSysCode,      (OP_IMMED | OP_NOTHING), OP_NOTHING, OP_NOTHING )               /* syscall */
PICK( IT_COP0_SPECIAL,  ITCop0Spc,      OP_NOTHING, OP_NOTHING, OP_NOTHING )                            /* rfe     */
PICK( IT_TRAP,          ITTrap,         OP_GPR, OP_GPR, (OP_IMMED | OP_NOTHING) )                       /* teq     */
PICK( IT_TRAP_IMM,      ITTrapImm,      OP_GPR, OP_IMMED, OP_NOTHING )                                  /* teqi    */
PICK( IT_LOAD_UIMM,     ITLoadUImm,     OP_GPR, OP_IMMED, OP_NOTHING )                                  /* lui     */
PICK( IT_PSEUDO_LIMM,   ITPseudoLImm,   OP_GPR, OP_IMMED, OP_NOTHING )                                  /* li      */
PICK( IT_PSEUDO_LADDR,  ITPseudoLAddr,  OP_GPR, (OP_REG_INDIRECT | OP_IMMED), OP_NOTHING )              /* la      */
PICK( IT_PSEUDO_MOV,    ITPseudoMov,    OP_GPR, OP_GPR, OP_NOTHING )                                    /* move    */
PICK( IT_PSEUDO_CLR,    ITPseudoClr,    OP_GPR, OP_NOTHING, OP_NOTHING )                                /* clr     */
PICK( IT_PSEUDO_FCLR,   ITPseudoFclr,   OP_FPR, OP_NOTHING, OP_NOTHING )                                /* fclr    */
PICK( IT_PSEUDO_FMOV,   ITPseudoFmov,   OP_FPR, OP_FPR, OP_NOTHING )                                    /* fmov    */
PICK( IT_PSEUDO_NOT,    ITPseudoNot,    OP_GPR, (OP_GPR | OP_IMMED), OP_NOTHING )                       /* not     */
PICK( IT_PSEUDO_NEGF,   ITPseudoNegf,   OP_FPR, OP_FPR, OP_NOTHING )                                    /* negf    */
PICK( IT_PSEUDO_FNEG,   ITPseudoFneg,   OP_FPR, OP_FPR, OP_NOTHING )                                    /* fneg    */
PICK( IT_PSEUDO_ABS,    ITPseudoAbs,    (OP_GPR | OP_IMMED), OP_GPR, OP_NOTHING )                       /* absl    */
