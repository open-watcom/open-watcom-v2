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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "disasm.h"

/* do it this funny way so no R/W relocations under Windows */
#define mmpick( e, n ) static const char mmlbl_##e[] = { n };
#include "mmpick.h"//

#undef mmpick
#define mmpick( e, n )  mmlbl_##e,

char const                    * const ModifierTab[] = {
#include "mmpick.h"
};


/* do it this funny way so no R/W relocations under Windows */
#define regpick( e, n ) static const char reglbl_##e[] = { n };
#include "regpick.h"

#undef regpick
#define regpick( e, n ) reglbl_##e,

char const                    * const X86RegisterName[] = {
#include "regpick.h"
};

ins_name  const               ImmedTable[] = {
    I_ADD,
    I_OR,
    I_ADC,
    I_SBB,
    I_AND,
    I_SUB,
    I_XOR,
    I_CMP
};

ins_name const                ShiftTable[] = {
    I_ROL,
    I_ROR,
    I_RCL,
    I_RCR,
    I_SHL,
    I_SHR,
    I_INVALID,
    I_SAR
};

ins_name const                Group1Table[] = {
    I_TEST,
    I_INVALID,
    I_NOT,
    I_NEG,
    I_MUL,
    I_IMUL,
    I_DIV,
    I_IDIV
};

ins_name const                Group2Table[] = {
    I_INC,
    I_DEC,
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID
};

ins_name const                Group3Table[] = {
    I_INC,
    I_DEC,
    I_CALL,
    I_CALL_FAR,
    I_JMP,
    I_JMP_FAR,
    I_PUSH,
    I_INVALID
};

ins_name const                S00Ops[] = {
    I_SLDT,
    I_STR,
    I_LLDT,
    I_LTR,
    I_VERR,
    I_VERW,
    I_INVALID,
    I_INVALID
};

ins_name  const               S01Ops[] = {
    I_SGDT,
    I_SIDT,
    I_LGDT,
    I_LIDT,
    I_SMSW,
    I_INVALID,
    I_LMSW,
    I_INVLPG
};

ins_name const                S2xOpType[] = {
    OT_CONTROL,
    OT_DEBUG,
    OT_CONTROL,
    OT_DEBUG,
    OT_TEST,
    OT_NULL,
    OT_TEST
};

ins_name  const               S8xOps[] = {
    I_JO,
    I_JNO,
    I_JB,
    I_JAE,
    I_JE,
    I_JNE,
    I_JBE,
    I_JA,
    I_JS,
    I_JNS,
    I_JP,
    I_JPO,
    I_JL,
    I_JGE,
    I_JLE,
    I_JG
};

ins_name  const               S9xOps[] = {
    I_SETO,
    I_SETNO,
    I_SETB,
    I_SETAE,
    I_SETE,
    I_SETNE,
    I_SETBE,
    I_SETA,
    I_SETS,
    I_SETNS,
    I_SETP,
    I_SETPO,
    I_SETL,
    I_SETGE,
    I_SETLE,
    I_SETG
};

ins_name  const               SAxOps[] = {
    I_PUSH,
    I_POP,
    I_CPUID,
    I_BT,
    I_SHLD,
    I_SHLD,
    I_INVALID,          /* was I_CMPXCHG        08-apr-91 */
    I_INVALID,          /* was I_CMPXCHG        08-apr-91 */
    I_PUSH,
    I_POP,
    I_RSM,
    I_BTS,
    I_SHRD,
    I_SHRD,
    I_INVALID,
    I_IMUL
};

ins_name const                SAxOpType[] = {
    OT_FS,
    OT_FS,
    OT_NULL,
    OT_MOD_RM_NOSIZE,
    OT_MOD_RM_IMMED8,
    OT_MOD_RM_CL,
    OT_NULL,
    OT_NULL,
    OT_GS,
    OT_GS,
    OT_NULL,
    OT_MOD_RM_NOSIZE,
    OT_MOD_RM_IMMED8,
    OT_MOD_RM_CL,
    OT_NULL,
    OT_MOD_RM
};

ins_name const                SBAOps[] = {
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_BT,
    I_BTS,
    I_BTR,
    I_BTC
};

ins_name const                SBxOps[] = {
    I_CMPXCHG,                                          /* 08-apr-91 */
    I_CMPXCHG,                                          /* 08-apr-91 */
    I_LSS,
    I_BTR,
    I_LFS,
    I_LGS,
    I_MOVZX,
    I_MOVZX,
    I_INVALID,
    I_INVALID,
    I_BTx,
    I_BTC,
    I_BSF,
    I_BSR,
    I_MOVSX,
    I_MOVSX
};

ins_name  const               SBxOpType[] = {
    OT_MOD_RM,
    OT_MOD_RM,
    OT_MOD_REG,
    OT_MOD_RM_NOSIZE,
    OT_MOD_REG,
    OT_MOD_REG,
    OT_RV_RMB,
    OT_RD_RMW,
    OT_NULL,
    OT_NULL,
    OT_MEM_IMMED8,
    OT_MOD_RM_NOSIZE,
    OT_MOD_RM_NOSIZE,
    OT_MOD_RM_NOSIZE,
    OT_RV_RMB,
    OT_RD_RMW
};

ins_name const                SCxOps[] = {
    I_XADD,
    I_XADD,
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID
};

ins_name  const               SCxOpType[] = {
    OT_MOD_RM,
    OT_MOD_RM,
    OT_NULL,
    OT_NULL,
    OT_NULL,
    OT_NULL,
    OT_NULL,
    OT_NULL
};

prefix const                  PrefixTab[] = {
    PREF_LOCK,
    0,
    PREF_REPNE,
    PREF_REPE
};

prefix const                  SegPrefixTab[] = {
    PREF_ES,
    PREF_CS,
    PREF_SS,
    PREF_DS
};
