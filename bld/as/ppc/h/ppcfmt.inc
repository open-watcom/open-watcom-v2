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


PICK( IT_BIN,           ITBinary,       OP_GPR, OP_GPR, OP_GPR, OP_NOTHING, OP_NOTHING )                /* addx  */
PICK( IT_BIN2,          ITBinary2,      OP_GPR, OP_GPR, OP_GPR, OP_NOTHING, OP_NOTHING )                /* eqv   */
PICK( IT_BIN_IMM,       ITBinaryImmed,  OP_GPR, OP_GPR, OP_IMMED, OP_NOTHING, OP_NOTHING )              /* addi  */
PICK( IT_BIN_IMM2,      ITBinaryImmed2, OP_GPR, OP_GPR, OP_IMMED, OP_NOTHING, OP_NOTHING )              /* ori   */
PICK( IT_UN,            ITUnary,        OP_GPR, OP_GPR, OP_NOTHING, OP_NOTHING, OP_NOTHING )            /* absx  */
PICK( IT_UN2,           ITUnary2,       OP_GPR, OP_GPR, OP_NOTHING, OP_NOTHING, OP_NOTHING )            /* extsb */
PICK( IT_FP_BIN,        ITFPBin,        OP_FPR, OP_FPR, OP_FPR, OP_NOTHING, OP_NOTHING )                /* faddx */
PICK( IT_FP_UN,         ITFPUnary,      OP_FPR, OP_FPR, OP_NOTHING, OP_NOTHING, OP_NOTHING )            /* fabsx */
PICK( IT_FP_CMP,        ITFPCmp,        OP_CRF, OP_FPR, OP_FPR, OP_NOTHING, OP_NOTHING )                /* fcmpo */
PICK( IT_FP_MULADD,     ITFPMulAdd,     OP_FPR, OP_FPR, OP_FPR, OP_FPR, OP_NOTHING )                    /* fmadd */
PICK( IT_FP_MUL,        ITFPMul,        OP_FPR, OP_FPR, OP_FPR, OP_NOTHING, OP_NOTHING )                /* fmul  */
PICK( IT_BR,            ITBranch,       OP_IMMED, OP_NOTHING, OP_NOTHING, OP_NOTHING, OP_NOTHING )      /* bx    */
PICK( IT_BR_COND,       ITBranchCond,   OP_IMMED, OP_IMMED, OP_IMMED, OP_NOTHING, OP_NOTHING )          /* bcx   */
PICK( IT_BR_SPEC,       ITBranchSpec,   OP_IMMED, OP_IMMED, OP_NOTHING, OP_NOTHING, OP_NOTHING )        /* bcctr */
PICK( IT_CMP,           ITCmp,          (OP_CRF | OP_IMMED), (OP_IMMED | OP_GPR), OP_GPR, (OP_GPR | OP_NOTHING), OP_NOTHING )                   /* cmp   */
PICK( IT_CMP_IMM,       ITCmpImmed,     (OP_CRF | OP_IMMED), (OP_IMMED | OP_GPR), (OP_GPR | OP_IMMED), (OP_IMMED | OP_NOTHING), OP_NOTHING )    /* cmpi  */
PICK( IT_CRB_BIN,       ITCondBit,      OP_CRB, OP_CRB, OP_CRB, OP_NOTHING, OP_NOTHING )                /* crand */
PICK( IT_CRF_UN,        ITCondUn,       OP_CRF, OP_CRF, OP_NOTHING, OP_NOTHING, OP_NOTHING )            /* mcrf  */
PICK( IT_CRF_MCRXR,     ITMcrxr,        OP_CRF, OP_NOTHING, OP_NOTHING, OP_NOTHING, OP_NOTHING )        /* mcrxr */
PICK( IT_MFCRMSR,       ITMfcr,         OP_GPR, OP_NOTHING, OP_NOTHING, OP_NOTHING, OP_NOTHING )        /* mfcr  */
PICK( IT_MFFS,          ITMffs,         OP_FPR, OP_NOTHING, OP_NOTHING, OP_NOTHING, OP_NOTHING )        /* mffsx */
PICK( IT_MFSPR,         ITMfspr,        OP_GPR, OP_IMMED, OP_NOTHING, OP_NOTHING, OP_NOTHING )          /* mfspr */
PICK( IT_MFSR,          ITMfsr,         OP_GPR, OP_IMMED, OP_NOTHING, OP_NOTHING, OP_NOTHING )          /* mfsr  */
PICK( IT_MFSRIN,        ITMfsrin,       OP_GPR, OP_GPR, OP_NOTHING, OP_NOTHING, OP_NOTHING )            /* mfsrin*/
PICK( IT_MFTB,          ITMftb,         OP_GPR, OP_IMMED, OP_NOTHING, OP_NOTHING, OP_NOTHING )          /* mftb  */
PICK( IT_MTCRF,         ITMtcrf,        OP_IMMED, OP_GPR, OP_NOTHING, OP_NOTHING, OP_NOTHING )          /* mtcrf */
PICK( IT_MTFSB,         ITMtfsb,        OP_CRB, OP_NOTHING, OP_NOTHING, OP_NOTHING, OP_NOTHING )        /* mtfsb */
PICK( IT_MTFSF,         ITMtfsf,        OP_IMMED, OP_FPR, OP_NOTHING, OP_NOTHING, OP_NOTHING )          /* mtfsf */
PICK( IT_MTFSF_IMM,     ITMtfsfImmed,   OP_CRF, OP_IMMED, OP_NOTHING, OP_NOTHING, OP_NOTHING )          /* mtfsfi*/
PICK( IT_MTSPR,         ITMtspr,        OP_IMMED, OP_GPR, OP_NOTHING, OP_NOTHING, OP_NOTHING )          /* mtspr */
PICK( IT_MTSR,          ITMtsr,         OP_IMMED, OP_GPR, OP_NOTHING, OP_NOTHING, OP_NOTHING )          /* mtsr  */
PICK( IT_DC,            ITDc,           OP_GPR, OP_GPR, OP_NOTHING, OP_NOTHING, OP_NOTHING )            /* dcbf  */
PICK( IT_EIEIO,         ITEieio,        OP_NOTHING, OP_NOTHING, OP_NOTHING, OP_NOTHING, OP_NOTHING )    /* eieio */
PICK( IT_FCTID,         ITFctid,        OP_FPR, OP_FPR, OP_NOTHING, OP_NOTHING, OP_NOTHING )            /* fctid */
PICK( IT_LS,            ITLdStr,        OP_GPR, OP_REG_INDIRECT, OP_NOTHING, OP_NOTHING, OP_NOTHING )   /* lbz   */
PICK( IT_LS_INDEX,      ITLdStrIndex,   OP_GPR, OP_GPR, OP_GPR, OP_NOTHING, OP_NOTHING )                /* lbzux */
PICK( IT_LS_FP,         ITLdStrFP,      OP_FPR, OP_REG_INDIRECT, OP_NOTHING, OP_NOTHING, OP_NOTHING )   /* lfd   */
PICK( IT_LS_INDEX_FP,   ITLdStrIndexFP, OP_FPR, OP_GPR, OP_GPR, OP_NOTHING, OP_NOTHING )                /* lfdux */
PICK( IT_LSWI,          ITLswi,         OP_GPR, OP_GPR, OP_IMMED, OP_NOTHING, OP_NOTHING )              /* lswi  */
PICK( IT_SHIFT,         ITShift,        OP_GPR, OP_GPR, OP_IMMED, OP_NOTHING, OP_NOTHING )              /* sliqx */
PICK( IT_SHIFT_IMM,     ITShiftImmed,   OP_GPR, OP_GPR, OP_IMMED, OP_IMMED, OP_IMMED )                  /* rlwimi*/
PICK( IT_SHIFT_INDEX,   ITShiftIndex,   OP_GPR, OP_GPR, OP_GPR, OP_IMMED, OP_IMMED )                    /* rlwnmx*/
PICK( IT_TLBIE,         ITTlbie,        OP_GPR, OP_NOTHING, OP_NOTHING, OP_NOTHING, OP_NOTHING )        /* tlbie */
PICK( IT_TRAP,          ITTrap,         OP_IMMED, OP_GPR, OP_GPR, OP_NOTHING, OP_NOTHING )              /* tw    */
PICK( IT_TRAP_IMM,      ITTrapImmed,    OP_IMMED, OP_GPR, OP_IMMED, OP_NOTHING, OP_NOTHING )            /* twi   */
PICK( IT_SM_LI,         ITSMLoadImmed,  OP_GPR, OP_IMMED, OP_NOTHING, OP_NOTHING, OP_NOTHING )          /* li    */
PICK( IT_SM_LA,         ITSMLoadAddr,   OP_GPR, OP_REG_INDIRECT, OP_NOTHING, OP_NOTHING, OP_NOTHING )   /* la    */
PICK( IT_SM_SUBI,       ITSMSubImmed,   OP_GPR, OP_GPR, OP_IMMED, OP_NOTHING, OP_NOTHING )              /* subi  */
PICK( IT_SM_MR,         ITSMMovReg,     OP_GPR, OP_GPR, OP_NOTHING, OP_NOTHING, OP_NOTHING )            /* mr    */
PICK( IT_SM_B_C_ICC,    ITSMBCIcc,      (OP_CRF | OP_IMMED), (OP_NOTHING | OP_IMMED), OP_NOTHING, OP_NOTHING, OP_NOTHING )              /* blt   */
PICK( IT_SM_B_C,        ITSMBC,         (OP_CRF | OP_BI | OP_IMMED), (OP_NOTHING | OP_IMMED), OP_NOTHING, OP_NOTHING, OP_NOTHING )      /* bdnz  */
PICK( IT_SM_B_SP_ICC,   ITSMBSpecIcc,   (OP_CRF | OP_NOTHING), OP_NOTHING, OP_NOTHING, OP_NOTHING, OP_NOTHING )                         /* bltlr */
PICK( IT_SM_B_SP,       ITSMBSpec,      (OP_CRF | OP_BI | OP_IMMED | OP_NOTHING), OP_NOTHING, OP_NOTHING, OP_NOTHING, OP_NOTHING )      /* bdnzlr*/
PICK( IT_SM_CMPWI,      ITSMCmpwi,      (OP_CRF | OP_GPR), (OP_GPR | OP_IMMED), (OP_IMMED | OP_NOTHING), OP_NOTHING, OP_NOTHING )       /* cmpwi */
PICK( IT_SM_CMPW,       ITSMCmpw,       (OP_CRF | OP_GPR), OP_GPR, (OP_GPR | OP_NOTHING), OP_NOTHING, OP_NOTHING )                      /* cmpw  */
PICK( IT_SM_EXTLWI,     ITSMExtlwi,     OP_GPR, OP_GPR, OP_IMMED, OP_IMMED, OP_NOTHING )                /* extlwi*/
PICK( IT_SM_SRWI,       ITSMSrwi,       OP_GPR, OP_GPR, OP_IMMED, OP_NOTHING, OP_NOTHING )              /* srwi  */
PICK( IT_SM_ROTLW,      ITSMRotlw,      OP_GPR, OP_GPR, OP_GPR, OP_NOTHING, OP_NOTHING )                /* rotlw */
PICK( IT_SM_INSLW,      ITSMInslw,      OP_GPR, OP_GPR, OP_IMMED, OP_IMMED, OP_NOTHING )                /* inslw */
PICK( IT_SM_TRAP_ANY,   ITSMTrapUncond, OP_NOTHING, OP_NOTHING, OP_NOTHING, OP_NOTHING, OP_NOTHING )    /* trap  */
PICK( IT_SM_TRAP,       ITSMTrap,       OP_GPR, OP_GPR, OP_NOTHING, OP_NOTHING, OP_NOTHING )            /* twlt  */
PICK( IT_SM_TRAPI,      ITSMTrapImmed,  OP_GPR, OP_IMMED, OP_NOTHING, OP_NOTHING, OP_NOTHING )          /* twlti */
PICK( IT_SM_MOV_SPR,    ITSMMovSpr,     OP_GPR, OP_NOTHING, OP_NOTHING, OP_NOTHING, OP_NOTHING )        /* mtxer */
PICK( IT_SM_MTSPRN,     ITSMMtsprN,     OP_IMMED, OP_GPR, OP_NOTHING, OP_NOTHING, OP_NOTHING )          /* mtsprg*/
PICK( IT_SM_MFSPRN,     ITSMMfsprN,     OP_GPR, OP_IMMED, OP_NOTHING, OP_NOTHING, OP_NOTHING )          /* mfsprg*/
