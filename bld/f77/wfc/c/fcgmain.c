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
* Description:  F-Code interpreter (code generation phase)
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "wf77defs.h"
#include "cg.h"
#include "cgaux.h"
#include "cgswitch.h"
#include "fcodes.h"
#include "cpopt.h"
#include "fcgbls.h"
#include "progsw.h"
#include "cgflags.h"
#include "wf77segs.h"
#include "wf77labe.h"
#include "wf77auxd.h"
#include "wf77info.h"
#include "compcfg.h"
#include "emitobj.h"
#include "fcflow.h"
#include "fcsyms.h"
#include "tmpdefs.h"
#include "fctemp.h"
#include "fcgmain.h"
#include "rtconst.h"
#include "fcrtns.h"
#include "inline.h"
#include "fcstack.h"
#include "cgswitch.h"
#include "cgprotos.h"
#include "cgcli.h"


extern  void            FreeGlobalSegs(void);

extern  void            (* __FAR FCJmpTab[])(void);

/* Forward declarations */
static  void    CGStart( void );
static  void    InitCG( void );


cgflags_t               CGFlags = { 0 };


static  void    CGProgFini( void ) {
//============================

// Finalize code generator.

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


void    CGGenSub( void ) {
//==================

// Finalize code generator after compiling a subprogram.

    InitStack();
    if( ( CGFlags & CG_STARTED ) == 0 ) {
        CGStart();
    }
    if( CGFlags & CG_INIT ) {
        if( ( ProgSw & PS_ERROR ) == 0 ) {
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


#if _CPU == 386
  #define TARG_SWITCHES (USE_32)
#elif _CPU == 8086
  #define TARG_SWITCHES 0
#elif _CPU == _AXP
  #define TARG_SWITCHES 0
#elif _CPU == _PPC
  #define TARG_SWITCHES 0
#else
  #error no target specified
#endif

static  void    CGStart( void ) {
//=========================

// Initialize code generator.

    cg_switches         cg_opts;
    cg_target_switches  cg_target;
    int                 space_time;
    cg_init_info        info;
    proc_revision       cpu;

    if( BELoad( NULL ) ) {
        CGFlags |= CG_LOADED;
        cpu = 0;
        cg_opts = FORTRAN_ALIASING|RELAX_ALIAS;
#if COMP_CFG_CG_FPE
        if( (OZOpts & OZOPT_O_LOOP_INVAR) == 0 ) {
            cg_opts |= ENABLE_FP_EXCEPTIONS;
        }
#endif
        cg_target = TARG_SWITCHES;

#if _CPU == _AXP || _CPU == _PPC
        if( CGOpts & CGOPT_GENASM ) {
            cg_target |= ASM_OUTPUT;
        }
        if( CGOpts & CGOPT_LOGOWL ) {
            cg_target |= OWL_LOGGING;
        }
#endif
        if( CGOpts & CGOPT_ECHOAPI ) {
            cg_opts |= ECHO_API_CALLS;
        }

#if _CPU == 386 || _CPU == 8086
        if( CGOpts & CGOPT_CONST_CODE ) {
            cg_target |= CONST_IN_CODE;
        }
#endif
#if _CPU == 386
        if( OZOpts & OZOPT_O_BASE_PTR ) {
            cg_opts |= MEMORY_LOW_FAILS;
        }
#endif
        if( OZOpts & OZOPT_O_SUPER_OPTIMAL ) {
            cg_opts |= SUPER_OPTIMAL;
        }
        if( OZOpts & OZOPT_O_FLOW_REG_SAVES ) {
            cg_opts |= FLOW_REG_SAVES;
        }
        if( OZOpts & OZOPT_O_BRANCH_PREDICTION ) {
            cg_opts |= BRANCH_PREDICTION;
        }
        if( OZOpts & OZOPT_O_CALL_RET ) {
            cg_target |= NO_CALL_RET_TRANSFORM;
        }
        if( OZOpts & OZOPT_O_LOOP ) {
            cg_opts |= LOOP_OPTIMIZATION;
        }
        if( OZOpts & OZOPT_O_UNROLL ) {
            cg_opts |= LOOP_OPTIMIZATION | LOOP_UNROLLING;
        }
        if( OZOpts & OZOPT_O_MATH ) {
            cg_target |= I_MATH_INLINE;
        }
        if( OZOpts & OZOPT_O_DISABLE ) {
            cg_opts |= NO_OPTIMIZATION;
        }
#if _CPU == 386 || _CPU == 8086
        if( OZOpts & OZOPT_O_FRAME ) {
            cg_target |= NEED_STACK_FRAME;
        }
#endif
        if( OZOpts & OZOPT_O_INSSCHED ) {
            cg_opts |= INS_SCHEDULING;
        }
        if( OZOpts & OZOPT_O_NUMERIC ) {
            cg_opts |= FP_UNSTABLE_OPTIMIZATION;
        }
        space_time = 50;
        if( OZOpts & OZOPT_O_SPACE ) {
            space_time = 100;
        }
        if( OZOpts & OZOPT_O_TIME ) {
            space_time = 0;
        }
#if _CPU == 386 || _CPU == 8086
        if( _FloatingDS( CGOpts ) ) {
            cg_target |= FLOATING_DS;
        }
        if( _FloatingES( CGOpts ) ) {
            cg_target |= FLOATING_ES;
        }
#endif
#if _CPU == 8086
        if( CGOpts & CGOPT_SS_FLOATS ) {
            cg_target |= FLOATING_SS;
        }
        if( CGOpts & CGOPT_WINDOWS ) {
            cg_target |= WINDOWS | CHEAP_WINDOWS;
        }
#elif _CPU == 386
        if( CGOpts & CGOPT_EZ_OMF ) {
            cg_target |= EZ_OMF;
        }
        if( CGOpts & CGOPT_STACK_GROW ) {
            cg_target |= GROW_STACK;
        }
#endif
#if _CPU == 386 || _CPU == 8086
    #if _CPU == 8086
        if( CPUOpts & (CPUOPT_80386 | CPUOPT_80486 | CPUOPT_80586) ) {
    #endif
            if( _FloatingFS( CGOpts ) ) {
                cg_target |= FLOATING_FS;
            }
            if( _FloatingGS( CGOpts ) ) {
                cg_target |= FLOATING_GS;
            }
    #if _CPU == 8086
        }
    #endif
#endif

        // set memory model

#if _CPU == 8086
        if( CGOpts & CGOPT_M_HUGE ) {
            cg_target |= BIG_CODE | BIG_DATA;
        }
#elif _CPU == 386
        if( CGOpts & CGOPT_M_FLAT ) {
            cg_target |= FLAT_MODEL;
        }
        if( CGOpts & CGOPT_M_COMPACT ) {
            cg_target |= BIG_DATA | CHEAP_POINTER;
        }
#elif _CPU == _AXP || _CPU == _PPC
        cg_target |= CHEAP_POINTER;
#else
    #error no target specified
#endif

#if _CPU == 386 || _CPU == 8086
        if( CGOpts & CGOPT_M_MEDIUM ) {
            cg_target |= BIG_CODE | CHEAP_POINTER;
        }
        if( CGOpts & CGOPT_M_LARGE ) {
            cg_target |= BIG_CODE | BIG_DATA | CHEAP_POINTER;
        }

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
    #if _CPU == 386
            SET_FPU_LEVEL( cpu, FPU_387 );
    #else
            SET_FPU_LEVEL( cpu, FPU_87 );
    #endif
        }

        if( CPUOpts & CPUOPT_FPD ) {
            cg_target |= P5_DIVIDE_CHECK;
        }
#endif

        if( CGOpts & CGOPT_DB_LINE ) {
            cg_opts |= NUMBERS;
        }
        if( CGOpts & CGOPT_DB_LOCALS ) {
            cg_opts |= DBG_TYPES | DBG_LOCALS | NUMBERS | NO_OPTIMIZATION;
        }
        if( CGOpts & CGOPT_DI_CV ) {
            cg_opts |= DBG_CV;
        } else if( CGOpts & CGOPT_DI_DWARF ) {
            cg_opts |= DBG_DF;
        }
        info = BEInit( cg_opts, cg_target, space_time, cpu );

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
    #error no target specified
#endif
            }
        }
#if _CPU == 386 || _CPU == 8086
        if( GET_FPU( cpu ) > FPU_NONE ) {
            CGFlags |= CG_FP_MODEL_80x87;
        }
#endif
    }
    CGFlags |= CG_STARTED;
}


static  void    InitCG( void ) {
//========================

// Initialize code generator.

    CGFlags |= CG_INIT;
    InitSegs();
    BEStart();
    DefTypes();
    AllocSegs();
}


void    FCodeSequence( void ) {
//=======================

// Interpret an F-Code sequence.

    FCODE   f_code;

    for(;;) {
        f_code = GetFCode();
        if( f_code == FC_END_OF_SEQUENCE )
            break;
        FCTablePtr[ f_code ]();
    }
}


void    CGPurge( void ) {
//=================

// Free up unallocated memory if code generator gave a fatal error.

    FiniLabels( 0 );
    FiniLabels( FORMAT_LABEL );
    FiniTmps();
    FreeUsedBacks( false );
    FreeGlobalSegs();
    FreeRtRtns();
    FreeInlinePragmas();
}
