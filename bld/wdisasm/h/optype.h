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


typedef enum {
    OT_NULL,
    OT_ACCUM_DX,
    OT_ACCUM_IMMED,
    OT_ACCUM_MEM,
    OT_ACCUM_REG16,
    OT_ESC,
    OT_IMMED,
    OT_IMMED8,
    OT_IMMED16,
    OT_IMMED_WORD,
    OT_IP_INC8,
    OT_IP_INC16,
    OT_MEM_IMMED8,
    OT_MOD_RM,
    OT_REG8_IMMED,
    OT_REG16_IMMED,
    OT_REG16,
    OT_RM,
    OT_RM_CL,
    OT_RM_IMMED,
    OT_SEG_ADDR,
    OT_SEG_REG,
    OT_SR_RM,
    OT_THREE,
    OT_MEM_IMMED16,
    OT_RM_1,
    OT_MOD_REG,
    OT_RM_TEST,
    OT_SEG_OP,
    OT_ENTER,
    OT_PREFIX,
    OT_DS_SI,
    OT_DS_ESI,
    OT_ES_DI,
    OT_ES_EDI,
    OT_DS_SI_ES_DI,
    OT_DS_ESI_ES_EDI,
    OT_REG_MEM_IMMED8,
    OT_REG_MEM_IMMED16,
    OT_DX_ACCUM,
    OT_ACCUM_IMMED8,
    OT_IMMED8_ACCUM,
    OT_286,
    OT_FS,
    OT_GS,
    OT_OPND_SIZE,
    OT_ADDR_SIZE,
    OT_RMW,
    OT_M,
    OT_RV_RMW,
    OT_RMB,
    OT_RV_RMB,
    OT_RD_RMW,
    OT_MOD_RM_IMMED8,
    OT_MOD_RM_CL,
    OT_MOD_RM_NOSIZE,
    OT_TEST,
    OT_DEBUG,
    OT_CONTROL,
    OT_NYI,
    OT_EXTRA,
    OT_MM_MODRM,
    OT_MODRM_MM,
    OT_MM_IMMED8,
    OT_CC_MODRM
}                       operand_type;

typedef enum {
    EREG,
    WST,
    WDT,
    ANY_SINGLE,
    ANY_DOUBLE,
    REAL_ANY
}   wop_type;
