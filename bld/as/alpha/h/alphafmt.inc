;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


PICK( IT_MEMORY_LDA,    ITLoadAddress,  OP_GPR, (OP_REG_INDIRECT | OP_IMMED), OP_NOTHING )              /* lda     */
//PICK( IT_MEMORY_LDAH, ITMemLDAH,      OP_GPR, (OP_REG_INDIRECT | OP_IMMED), OP_NOTHING )              /* ldah    */
PICK( IT_MEMORY_ALL,    ITMemAll,       OP_GPR, (OP_REG_INDIRECT | OP_IMMED), OP_NOTHING )              /* ldl     */
PICK( IT_FP_MEMORY_ALL, ITMemAll,       OP_FPR, OP_REG_INDIRECT, OP_NOTHING )                           /* ldf     */
PICK( IT_MEMORY_A,      ITMemA,         OP_GPR, OP_NOTHING, OP_NOTHING )                                /* rpcc    */
PICK( IT_MEMORY_B,      ITMemB,         OP_REG_INDIRECT, OP_NOTHING, OP_NOTHING )                       /* fetch   */
PICK( IT_MEMORY_NONE,   ITMemNone,      OP_NOTHING, OP_NOTHING, OP_NOTHING )                            /* mb      */
PICK( IT_MEMORY_JUMP,   ITMemJump,      (OP_GPR | OP_REG_INDIRECT | OP_IMMED), (OP_REG_INDIRECT | OP_NOTHING | OP_IMMED), (OP_IMMED | OP_NOTHING) ) /* jmp */
PICK( IT_BRANCH,        ITBranch,       (OP_GPR | OP_IMMED), (OP_IMMED | OP_NOTHING), OP_NOTHING )      /* bsr     */
PICK( IT_FP_BRANCH,     ITBranch,       OP_FPR, OP_IMMED, OP_NOTHING )                                  /* fbeq    */
PICK( IT_OPERATE,       ITOperate,      OP_GPR, (OP_GPR | OP_IMMED), OP_GPR )                           /* addl    */
PICK( IT_FP_OPERATE,    ITFPOperate,    OP_FPR, OP_FPR, OP_FPR )                                        /* cpys    */
PICK( IT_FP_CONVERT,    ITFPConvert,    OP_FPR, OP_FPR, OP_NOTHING )                                    /* cvtqs   */
PICK( IT_BR,            ITBr,           (OP_GPR | OP_IMMED), (OP_IMMED | OP_NOTHING), OP_NOTHING )      /* br      */
PICK( IT_MT_MF_FPCR,    ITMTMFFpcr,     OP_FPR, (OP_FPR | OP_NOTHING), OP_FPR )                         /* mf_fpcr */
PICK( IT_RET,           ITRet,          (OP_GPR | OP_NOTHING | OP_REG_INDIRECT | OP_IMMED), (OP_REG_INDIRECT | OP_NOTHING | OP_IMMED), (OP_IMMED | OP_NOTHING) ) /* ret */
PICK( IT_PSEUDO_CLR,    ITPseudoClr,    OP_GPR, OP_NOTHING, OP_NOTHING )                                /* clr     */
PICK( IT_PSEUDO_FCLR,   ITPseudoFclr,   OP_FPR, OP_NOTHING, OP_NOTHING )                                /* fclr    */
PICK( IT_PSEUDO_MOV,    ITPseudoMov,    (OP_GPR | OP_IMMED), OP_GPR, OP_NOTHING )                       /* mov     */
PICK( IT_PSEUDO_FMOV,   ITPseudoFmov,   OP_FPR, OP_FPR, OP_NOTHING )                                    /* fmov    */
PICK( IT_PSEUDO_NOT,    ITPseudoNot,    (OP_GPR | OP_IMMED), OP_GPR, OP_NOTHING )                       /* not     */
PICK( IT_PSEUDO_NEGF,   ITPseudoNegf,   OP_FPR, OP_FPR, OP_NOTHING )                                    /* negf    */
PICK( IT_PSEUDO_FNEG,   ITPseudoFneg,   OP_FPR, OP_FPR, OP_NOTHING )                                    /* fneg    */
PICK( IT_PSEUDO_ABS,    ITPseudoAbs,    (OP_GPR | OP_IMMED), OP_GPR, OP_NOTHING )                       /* absl    */
PICK( IT_CALL_PAL,      ITCallPAL,      OP_IMMED, OP_NOTHING, OP_NOTHING )
