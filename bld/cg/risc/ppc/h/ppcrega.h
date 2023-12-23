/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  PPC generation ABI related definition
*
****************************************************************************/


//#define AT_FP_REG_IDX           13          // Reserved for the temporary floating-point

#define RT_RET_REG_IDX          3           // r3 or r3+r4

#define RT_VARARGS_REG_IDX      14          // r14
#define RT_TEMP_REG_IDX         5           // r5
#define RT_PARM2_REG_IDX        4           // r4
#define RT_PARM1_REG_IDX        3           // r3

#define HW_SP_REG               HW_R1       // r1
#define HW_SP_REG32             HW_D1       // r1
#define HW_RTOC_REG             HW_R2       // r2
#define HW_RTOC_REG32           HW_D2       // r2
#define HW_AT_REG               HW_R12      // r12
#define HW_AT_REG32             HW_D12      // r12
#define HW_VARARGS_REG32        HW_D14      // r14
#define HW_FP_REG               HW_R31      // r31
#define HW_FP_REG32             HW_D31      // r31

#define HW_RT_RET_REG8          HW_B3       // r3
#define HW_RT_RET_REG16         HW_W3       // r3
#define HW_RT_RET_REG32         HW_D3       // r3
#define HW_RT_RET_REG64         HW_Q3       // r3+r4
#define HW_RT_RET_REGFP         HW_F1       // f1
