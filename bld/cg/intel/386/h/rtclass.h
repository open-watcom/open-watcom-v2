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
* Description:  Runtime support routines list for 386. 
*
****************************************************************************/


/*        NB entries up until OK must match typclass.wif */

typedef enum {
        CU1,
        CI1,
        CU2,
        CI2,
        CU4,
        CI4,
        CU8,
        CI8,
        CCP,
        CPT,
        CFS,
        CFD,
        CFL,
        OK,
        C2TO1,
        C4TO1,
        C4TO2,
        C8TO2,
        C8TO4,
        S1TO2,
        S1TO4,
        S1TO8,
        S2TO4,
        S2TO8,
        S4TO8,
        Z1TO2,
        Z1TO4,
        Z1TO8,
        Z2TO4,
        Z2TO8,
        Z4TO8,
        EXT_PT,
        CHP_PT,
	PTTOI8,
	I8TOPT,

        LAST_CONV_TABLE = I8TOPT,

/*  beginning of runtime routine */

        C_U4_S, RT_U4FS = C_U4_S,
        C_I4_S, RT_I4FS = C_I4_S,
        C_U4_D, RT_U4FD = C_U4_D,
        C_I4_D, RT_I4FD = C_I4_D,
        C_S_D,  RT_FSFD = C_S_D,

/*        Following types have both truncation and rounding */

        C_S_4,  RT_FSI4 = C_S_4,
        R_S_4,
        C_S_U,  RT_FSU4 = C_S_U,
        R_S_U,
        C_D_4,  RT_FDI4 = C_D_4,
        R_D_4,
        C_D_U,  RT_FDU4 = C_D_U,
        R_D_U,
        C_D_S,  RT_FDFS = C_D_S,
        R_D_S,

        C_U8_S,
        C_I8_S,
        C_U8_D,
        C_I8_D,
        C_S_I8,
        R_S_I8,
        C_S_U8,
        R_S_U8,
        C_D_I8,
        R_D_I8,
        C_D_U8,
        R_D_U8,

        C7U8_S,
        C7U8_D,
        C7S_U8,
        C7D_U8,

/*        Following types are runtime routines to do arithmetic */
        RT_I8_MUL,
        RT_I8_DIV,
        RT_I8_MOD,
        RT_I8_RSHIFT,
        RT_I8_LSHIFT,

        RT_U8_MUL,
        RT_U8_DIV,
        RT_U8_MOD,
        RT_U8_RSHIFT,
        RT_U8_LSHIFT,

        RT_FSA,
        RT_FSS,
        RT_FSM,
        RT_FSD,
        RT_FSC,
        RT_FSN,
        RT_FDA,
        RT_FDS,
        RT_FDM,
        RT_FDD,
        RT_FDC,
        RT_FDN,
        RT_CHK,
        RT_GROW,
        RT_THUNK,
        RT_THUNK_STK,
        RT_CHOP,
        RT_SCAN1,
        RT_SCAN2,
        RT_SCAN4,
        RT_GETDS,
        RT_EPIHOOK,
        RT_PROHOOK,
        RT_PROFILE_ON,
        RT_PROFILE_OFF,

        RT_FlatToFar16,
        RT_Far16ToFlat,
        RT_Far16Func,
        RT_Far16Cdecl,
        RT_Far16Pascal,
        RT_Far32Func,

        RT_DP5DIV,
        RT_P5DIV,

        RT_DPOW,
        RT_DPOWI,
        RT_POW,
        RT_POWI,
        RT_IPOW,

        RT_DATAN2,
        RT_DFMOD,
        RT_DLOG,
        RT_DCOS,
        RT_DSIN,
        RT_DTAN,
        RT_DSQRT,
        RT_DFABS,
        RT_DACOS,
        RT_DASIN,
        RT_DATAN,
        RT_DCOSH,
        RT_DEXP,
        RT_DLOG10,
        RT_DSINH,
        RT_DTANH,

        RT_ATAN2,
        RT_FMOD,
        RT_LOG,
        RT_COS,
        RT_SIN,
        RT_TAN,
        RT_SQRT,
        RT_FABS,
        RT_ACOS,
        RT_ASIN,
        RT_ATAN,
        RT_COSH,
        RT_EXP,
        RT_LOG10,
        RT_SINH,
        RT_TANH,

/*        Following types are runtime routines for FAST -od codegen */

        RT_LDI2,
        RT_LDI4,
        RT_COPY,
        RT_FADD,
        RT_FSUB,
        RT_FSUBR,
        RT_FMUL,
        RT_FDIV,
        RT_FDIVR,
        RT_FCMP,
        RT_FCMPR,
        RT_TryInit,
        RT_TryUnwind,

/*      End of special runtime routines for FAST -od codegen */

        RT_BUGLIST,
        RT_FDIV_MEM32,
        RT_FDIV_MEM64,
        RT_FDIV_MEM32R,
        RT_FDIV_MEM64R,
        RT_FDIV_FPREG,
        RT_TLS_INDEX,
        RT_TLS_ARRAY,
        RT_TLS_REGION,

        RT_NOP,
        FPOK,
        U4F,
        FU4,
        BAD
} rt_class;

#define ROUNDING        C_S_4
#define BEG_RTNS        C_U4_S
/*  beginning of runtime calls */
#define BEG_ARITH       RT_FSA
