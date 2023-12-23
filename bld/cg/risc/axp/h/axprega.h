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


#define AT_FP_REG_IDX           30          // Reserved for the temporary floating-point

#define RT_RET_REG_IDX          0           // $v0

#define RT_VARARGS_REG_IDX      14          // $s5
#define RT_TEMP_REG_IDX         3           // $t2
#define RT_PARM2_REG_IDX        2           // $t1
#define RT_PARM1_REG_IDX        1           // $t0

#define HW_VARARGS_REG32        HW_D14
#define HW_FP_REG               HW_R15
#define HW_FP_REG32             HW_D15
#define HW_RA_REG               HW_R26
#define HW_RA_REG32             HW_D26
#define HW_AT_REG               HW_R28
#define HW_AT_REG32             HW_D28
#define HW_SP_REG               HW_R30
#define HW_SP_REG32             HW_D30
#define HW_ZERO_REG             HW_R31

#define HW_AT_FP_REG            HW_F30

#define HW_RT_RET_REG8          HW_B0       // $v0
#define HW_RT_RET_REG16         HW_W0       // $v0
#define HW_RT_RET_REG32         HW_D0       // $v0
#define HW_RT_RET_REG64         HW_R0       // $v0
#define HW_RT_RET_REGFP         HW_F0       // $f0

#define HW_RT_TEMP_REG          HW_R3       // $t2
#define HW_RT_PARM2_REG         HW_R2       // $t1
#define HW_RT_PARM2_REG32       HW_D2       // $t1
#define HW_RT_PARM1_REG         HW_R1       // $t0
#define HW_RT_PARM1_REG32       HW_D1       // $t0

