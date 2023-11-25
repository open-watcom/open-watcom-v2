/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  ALPHA generation ABI related definition
*
****************************************************************************/


#define VARARGS_PTR_REG_IDX     14      // $s5
#define RT_PARM2_REG_IDX        2       // $t1
#define RT_PARM1_REG_IDX        1       // $t0
#define RT_RET_REG_IDX          0       // $v0

#define HW_VARARGS_REG32        HW_D14
#define HW_FP_REG               HW_R15
#define HW_FP_REG32             HW_D15
#define HW_RA_REG               HW_R26
#define HW_RA_REG32             HW_D26
#define HW_AT_REG32             HW_D28
#define HW_SP_REG               HW_R30
#define HW_SP_REG32             HW_D30
#define HW_ZERO_REG             HW_R31

#define HW_AT_FP_REG            HW_F30
