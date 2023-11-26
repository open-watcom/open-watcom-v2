/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  MIPS generation ABI related definition
*
****************************************************************************/


//#define AT_FP_REG_IDX           1           // Reserved for the temporary floating-point

#define RT_RET_REG_IDX          2           // $v0

#define RT_VARARGS_REG_IDX      23          // $s7 !TODO!
#define RT_TEMP_REG_IDX         10          // $t2
#define RT_PARM2_REG_IDX        9           // $t1
#define RT_PARM1_REG_IDX        8           // $t0

#define HW_ZERO_REG             HW_R0       // $zero
#define HW_AT_REG               HW_R1       // $at
#define HW_VARARGS_REG          HW_R23      // $r23
#define HW_GP_REG               HW_R28      // $gp
#define HW_SP_REG               HW_R29      // $sp
#define HW_FP_REG               HW_R30      // $fp
#define HW_RA_REG               HW_R31      // $ra

#define HW_RT_RET_REG8          HW_B2       // $v0
#define HW_RT_RET_REG16         HW_W2       // $v0
#define HW_RT_RET_REG32         HW_D2       // $v0
#define HW_RT_RET_REG64         HW_Q2       // $v0+$v1
#define HW_RT_RET_REGFP         HW_F0       // $f0

#define HW_RT_TEMP_REG          HW_R10      // $t2
#define HW_RT_PARM2_REG         HW_R9       // $t1
#define HW_RT_PARM1_REG         HW_R8       // $t0

