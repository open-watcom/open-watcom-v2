/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  F-Code interpreter (code generation phase)
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "wf77defs.h"
#include "wf77aux.h"
#include "fcodes.h"
#include "cpopt.h"
#include "fcgbls.h"
#include "progsw.h"
#include "cgflags.h"
#include "wf77segs.h"
#include "wf77labe.h"
#include "wf77info.h"
#include "compcfg.h"
#include "emitobj.h"
#include "fcflow.h"
#include "fcsyms.h"
#include "tmpdefs.h"
#include "fctemp.h"
#include "fcgmain.h"
#include "inline.h"
#include "fcstack.h"
#include "cgswitch.h"
#include "cgprotos.h"
#include "cgcli.h"
#include "fcjmptab.h"
#include "gsegs.h"


#define DEF_CGSW_GEN_SWITCHES       (CGSW_GEN_FORTRAN_ALIASING | CGSW_GEN_RELAX_ALIAS)

#if _CPU == 8086
  #define DEF_CGSW_TARGET_SWITCHES  0
#elif _CPU == 386
  #define DEF_CGSW_TARGET_SWITCHES  (CGSW_X86_USE_32)
#elif _CPU == _AXP
  #define DEF_CGSW_TARGET_SWITCHES  0
#elif _CPU == _PPC
  #define DEF_CGSW_TARGET_SWITCHES  0
#else
  #error no or invalid target specified
#endif

cgflags_t               CGFlags = { 0 };

/* Forward declarations */
static  void    CGStart( void );
static  void    InitCG( void );


static  void    CGProgFini( void )
//================================
// Finalize code generator.
{
    if( CGFlags & CG_LOADED ) {
        if( CGFlags & CG_INIT ) {
            FreeGlobalData();
            FiniSegs();
            BEStop();
        }
        BEFini();
        BEUnload();
    }
}


void    CGGenSub( void )
//======================
// Finalize code generator after compiling a subprogram.
{
    InitStack();
    if( (CGFlags & CG_STARTED) == 0 ) {
        CGStart();
    }
    if( CGFlags & CG_INIT ) {
        if( (ProgSw & PS_ERROR) == 0 ) {
            InitSubSegs();
            InitLabels();
            InitRtRtns();
            InitInlinePragmas();
            InitFCode();
            InitTmps();
            DefStructs();
            FCTablePtr = FCJmpTab;
            FCodeSequence();
            FreeRtRtns();
            FreeInlinePragmas();
            if( NumSubProgs == 0 ) {
                CGProgFini();
            }
        }
    }
}

static  void    CGStart( void )
//=============================
// Initialize code generator.
{
    cg_switches         cg_gen_opts;
    cg_target_switches  cg_target_opts;
    int                 space_time;
    cg_init_info        info;
    proc_revision       cpu;

    if( BELoad( NULL ) ) {
        CGFlags |= CG_LOADED;
        cpu = 0;
        cg_gen_opts = DEF_CGSW_GEN_SWITCHES;
        cg_target_opts = DEF_CGSW_TARGET_SWITCHES;

#if COMP_CFG_CG_FPE
        if( (OZOpts & OZOPT_O_LOOP_INVAR) == 0 ) {
            cg_gen_opts |= CGSW_GEN_ENABLE_FP_EXCEPTIONS;
        }
#endif

#if _RISC_CPU
        if( CGOpts & CGOPT_GENASM ) {
            cg_target_opts |= CGSW_RISC_ASM_OUTPUT;
        }
        if( CGOpts & CGOPT_LOGOWL ) {
            cg_target_opts |= CGSW_RISC_OWL_LOGGING;
        }
#endif
        if( CGOpts & CGOPT_ECHOAPI ) {
            cg_gen_opts |= CGSW_GEN_ECHO_API_CALLS;
        }

#if _INTEL_CPU
        if( CGOpts & CGOPT_CONST_CODE ) {
            cg_target_opts |= CGSW_X86_CONST_IN_CODE;
        }
        #if _CPU == 386
        if( OZOpts & OZOPT_O_BASE_PTR ) {
            cg_gen_opts |= CGSW_GEN_MEMORY_LOW_FAILS;
        }
        #endif
#endif
        if( OZOpts & OZOPT_O_SUPER_OPTIMAL ) {
            cg_gen_opts |= CGSW_GEN_SUPER_OPTIMAL;
        }
        if( OZOpts & OZOPT_O_FLOW_REG_SAVES ) {
            cg_gen_opts |= CGSW_GEN_FLOW_REG_SAVES;
        }
        if( OZOpts & OZOPT_O_BRANCH_PREDICTION ) {
            cg_gen_opts |= CGSW_GEN_BRANCH_PREDICTION;
        }
        if( OZOpts & OZOPT_O_CALL_RET ) {
            cg_gen_opts |= CGSW_GEN_NO_CALL_RET_TRANSFORM;
        }
        if( OZOpts & OZOPT_O_LOOP ) {
            cg_gen_opts |= CGSW_GEN_LOOP_OPTIMIZATION;
        }
        if( OZOpts & OZOPT_O_UNROLL ) {
            cg_gen_opts |= CGSW_GEN_LOOP_OPTIMIZATION | CGSW_GEN_LOOP_UNROLLING;
        }
        if( OZOpts & OZOPT_O_MATH ) {
            cg_gen_opts |= CGSW_GEN_I_MATH_INLINE;
        }
        if( OZOpts & OZOPT_O_DISABLE ) {
            cg_gen_opts |= CGSW_GEN_NO_OPTIMIZATION;
        }
#if _INTEL_CPU
        if( OZOpts & OZOPT_O_FRAME ) {
            cg_target_opts |= CGSW_X86_NEED_STACK_FRAME;
        }
#endif
        if( OZOpts & OZOPT_O_INSSCHED ) {
            cg_gen_opts |= CGSW_GEN_INS_SCHEDULING;
        }
        if( OZOpts & OZOPT_O_NUMERIC ) {
            cg_gen_opts |= CGSW_GEN_FP_UNSTABLE_OPTIMIZATION;
        }
        space_time = 50;
        if( OZOpts & OZOPT_O_SPACE ) {
            space_time = 100;
        }
        if( OZOpts & OZOPT_O_TIME ) {
            space_time = 0;
        }
#if _INTEL_CPU
        if( _FloatingDS( CGOpts ) ) {
            cg_target_opts |= CGSW_X86_FLOATING_DS;
        }
        if( _FloatingES( CGOpts ) ) {
            cg_target_opts |= CGSW_X86_FLOATING_ES;
        }
    #if _CPU == 8086
        if( CGOpts & CGOPT_SS_FLOATS ) {
            cg_target_opts |= CGSW_X86_FLOATING_SS;
        }
        if( CGOpts & CGOPT_WINDOWS ) {
            cg_target_opts |= CGSW_X86_WINDOWS | CGSW_X86_CHEAP_WINDOWS;
        }
    #else
        if( CGOpts & CGOPT_EZ_OMF ) {
            cg_target_opts |= CGSW_X86_EZ_OMF;
        }
    #endif
    #if _CPU == 8086
        if( CPUOpts & (CPUOPT_80386 | CPUOPT_80486 | CPUOPT_80586) ) {
    #endif
            if( _FloatingFS( CGOpts ) ) {
                cg_target_opts |= CGSW_X86_FLOATING_FS;
            }
            if( _FloatingGS( CGOpts ) ) {
                cg_target_opts |= CGSW_X86_FLOATING_GS;
            }
    #if _CPU == 8086
        }
    #endif
#endif

        // set memory model

#if _INTEL_CPU
        if( CGOpts & CGOPT_M_MEDIUM ) {
            cg_target_opts |= CGSW_X86_BIG_CODE | CGSW_X86_CHEAP_POINTER;
        }
        if( CGOpts & CGOPT_M_LARGE ) {
            cg_target_opts |= CGSW_X86_BIG_CODE | CGSW_X86_BIG_DATA | CGSW_X86_CHEAP_POINTER;
        }
    #if _CPU == 8086
        if( CGOpts & CGOPT_M_HUGE ) {
            cg_target_opts |= CGSW_X86_BIG_CODE | CGSW_X86_BIG_DATA;
        }
    #else
        if( CGOpts & CGOPT_M_FLAT ) {
            cg_target_opts |= CGSW_X86_FLAT_MODEL;
        }
        if( CGOpts & CGOPT_M_COMPACT ) {
            cg_target_opts |= CGSW_X86_BIG_DATA | CGSW_X86_CHEAP_POINTER;
        }
    #endif
#elif _RISC_CPU
        cg_target_opts |= CGSW_X86_CHEAP_POINTER;
#else
    #error no or invalid target specified
#endif

#if _INTEL_CPU

        // set CPU type

    #if _CPU == 8086
        if( CPUOpts & CPUOPT_8086 ) {
            SET_CPU( cpu, CPU_86 );
        }
        if( CPUOpts & CPUOPT_80186 ) {
            SET_CPU( cpu, CPU_186 );
        }
        if( CPUOpts & CPUOPT_80286 ) {
            SET_CPU( cpu, CPU_286 );
        }
    #endif
        if( CPUOpts & CPUOPT_80386 ) {
            SET_CPU( cpu, CPU_386 );
        }
        if( CPUOpts & CPUOPT_80486 ) {
            SET_CPU( cpu, CPU_486 );
        }
        if( CPUOpts & CPUOPT_80586 ) {
            SET_CPU( cpu, CPU_586 );
        }
        if( CPUOpts & CPUOPT_80686 ) {
            SET_CPU( cpu, CPU_686 );
        }

        // set floating-point model

        SET_FPU( cpu, FPU_NONE );   // floating-point calls
        if( CPUOpts & CPUOPT_FPI ) {
            SET_FPU_EMU( cpu );
        }
        if( CPUOpts & CPUOPT_FPI87 ) {
            SET_FPU_INLINE( cpu );
        }

        // Set Floating-point level

        if( CPUOpts & CPUOPT_FP287 ) {
            SET_FPU_LEVEL( cpu, FPU_87 );
        } else if( CPUOpts & CPUOPT_FP387 ) {
            SET_FPU_LEVEL( cpu, FPU_387 );
        } else if( CPUOpts & CPUOPT_FP5 ) {
            SET_FPU_LEVEL( cpu, FPU_586 );
        } else if( CPUOpts & CPUOPT_FP6 ) {
            SET_FPU_LEVEL( cpu, FPU_686 );
        } else if( CPUOpts & (CPUOPT_FPI | CPUOPT_FPI87) ) {
            // no level specified; use default
    #if _CPU == 8086
            SET_FPU_LEVEL( cpu, FPU_87 );
    #else
            SET_FPU_LEVEL( cpu, FPU_387 );
    #endif
        }

        if( CPUOpts & CPUOPT_FPD ) {
            cg_target_opts |= CGSW_X86_P5_DIVIDE_CHECK;
        }
#endif

        if( CGOpts & CGOPT_DB_LINE ) {
            cg_gen_opts |= CGSW_GEN_DBG_NUMBERS;
        }
        if( CGOpts & CGOPT_DB_LOCALS ) {
            cg_gen_opts |= CGSW_GEN_DBG_TYPES | CGSW_GEN_DBG_LOCALS | CGSW_GEN_DBG_NUMBERS | CGSW_GEN_NO_OPTIMIZATION;
        }
        if( CGOpts & CGOPT_DI_CV ) {
            cg_gen_opts |= CGSW_GEN_DBG_CV;
        } else if( CGOpts & CGOPT_DI_DWARF ) {
            cg_gen_opts |= CGSW_GEN_DBG_DF;
        }
        info = BEInit( cg_gen_opts, cg_target_opts, space_time, cpu );

        if( info.success != 0 ) {
            if( ( info.version.target == II_TARG_STUB ) ||
                ( info.version.target == II_TARG_CHECK ) ) {
                InitCG();
#if _CPU == 8086
            } else if( info.version.target == II_TARG_8086 ) {
                InitCG();
#elif _CPU == 386
            } else if( info.version.target == II_TARG_80386 ) {
                InitCG();
#elif _CPU == _AXP
            } else if( info.version.target == II_TARG_AXP ) {
                InitCG();
#elif _CPU == _PPC
            } else if( info.version.target == II_TARG_PPC ) {
                InitCG();
#else
    #error no or invalid target specified
#endif
            }
        }
#if _INTEL_CPU
        if( GET_FPU( cpu ) > FPU_NONE ) {
            CGFlags |= CG_FP_MODEL_80x87;
        }
#endif
    }
    CGFlags |= CG_STARTED;
}


static  void    InitCG( void )
//============================
// Initialize code generator.
{
    CGFlags |= CG_INIT;
    InitSegs();
    BEStart();
    DefTypes();
    AllocSegs();
}


void    FCodeSequence( void )
//===========================
// Interpret an F-Code sequence.
{
    FCODE   f_code;

    for(;;) {
        f_code = GetFCode();
        if( f_code == FC_END_OF_SEQUENCE )
            break;
        FCTablePtr[ f_code ]();
    }
}


void    CGPurge( void )
//=====================
// Free up unallocated memory if code generator gave a fatal error.
{
    FiniLabels( 0 );
    FiniLabels( FORMAT_LABEL );
    FiniTmps();
    FreeUsedBacks( false );
    FreeGlobalSegs();
    FreeRtRtns();
    FreeInlinePragmas();
}
