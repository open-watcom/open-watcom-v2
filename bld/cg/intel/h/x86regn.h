/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  x86 register info processing.
*
****************************************************************************/


typedef enum {
    GPR_IDX  = 0x01,    /* general purpose registers */
    FPR_IDX  = 0x02,    /* floating-point registers */
    SEG_IDX  = 0x04,    /* segment registers */
    GPRB_IDX = 0x08,    /* 8-bit registers */
    GPRD_IDX = 0x10,    /* 16-bit registers */
    GPRW_IDX = 0x20,    /* 32-bit registers */
} reg_cls;

typedef enum {
    #define pick(id,ci,start,len) WV_REG_ ## id,
    #include "watdbreg.h"
    #undef pick
    WV_REG_END
} wv_regs;

typedef enum {
    #define pick(id,name,ci,start,len) DW_REG_ ## id,
    #include "dwregx86.h"
    #undef pick
    DW_REG_END
} dw_regs;

typedef uint_8      reg_idx;

typedef struct arch_reg_info {
    hw_reg_set      hw_reg;     /* CG register value */
    reg_idx         idx;        /* register encoding index */
    reg_cls         cls;        /* register class */
    wv_regs         wv_idx;     /* WATCOM debug register index */
    dw_regs         dw_idx;     /* Dwarf debug register index */
} arch_reg_info;

extern const arch_reg_info  RegsTab[];

extern hw_reg_set   GetFPReg( int idx );
extern int          CountFPRegs( hw_reg_set regs );
extern int          FPRegNum( name *reg_name );
extern reg_idx      RegTrans( hw_reg_set reg );
extern dw_regs      RegTransDW( hw_reg_set reg );
extern int          RegTransWV( hw_reg_set reg );
