/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Command line processing for C++ compiler (x86 targets)
*
****************************************************************************/


#include "plusplus.h"
#include "memmgr.h"
#include "macro.h"
#include "cmdline.h"
#include "cgdata.h"
#include "codegen.h"
#include "pragdefn.h"
#include "compinfo.h"

#include "cmdlnprs.gh"
#include "cmdlnsys.h"

#include "clibext.h"


#if 0
#define DEF_CGSW_GEN_SWITCHES_ALL   (CGSW_GEN_MEMORY_LOW_FAILS | CGSW_GEN_ENABLE_FP_EXCEPTIONS)
#else
#define DEF_CGSW_GEN_SWITCHES_ALL   (CGSW_GEN_MEMORY_LOW_FAILS)
#endif
#if _CPU == 8086
    #define DEF_CGSW_GEN_SWITCHES   0
    #define DEF_CGSW_X86_SWITCHES   CGSW_X86_CHEAP_POINTER
#else
    #define DEF_CGSW_GEN_SWITCHES   0
    #define DEF_CGSW_X86_SWITCHES   (CGSW_X86_CHEAP_POINTER | CGSW_X86_USE_32 | CGSW_X86_FLAT_MODEL)
#endif

#if _CPU == 8086
    #define DEFAULT_CPU CPU_86
    #define DEFAULT_FPU FPU_87
#else
    #define DEFAULT_CPU CPU_686
    #define DEFAULT_FPU FPU_387
#endif

#if _CPU == 8086
#define MM_ARCH "I86"
#else
#define MM_ARCH "386"
#endif

void CmdSysInit( void )
/*********************/
{
    GenSwitches = DEF_CGSW_GEN_SWITCHES | DEF_CGSW_GEN_SWITCHES_ALL;
    TargetSwitches = DEF_CGSW_X86_SWITCHES;
    CodeClassName = NULL;
    TextSegName = CMemStrdup( "" );
    DataSegName = CMemStrdup( "" );
    GenCodeGroup = CMemStrdup( "" );
    CompFlags.use_stdcall_at_number = true;
    CompFlags.register_conventions = true;
}

void CmdSysFini( void )
/*********************/
{
    CMemFreePtr( &ModuleName );
    CMemFreePtr( &CodeClassName );
    CMemFreePtr( &DataSegName );
    CMemFreePtr( &GenCodeGroup );
}

char *CmdSysEnvVar( void )
/************************/
{
#if _CPU == 8086
    return( "WPP" );
#else
    return( "WPP386" );
#endif
}

void CmdX86CheckStack87( unsigned *p )
/*************************************
 * 0 means no change to default
 */
{
    if( *p > 8 ) {
        *p = 0;
    }
}

void CmdX86CheckThreshold( unsigned *p )
/***************************************
 * 0 is allowed
 */
{
    if( *p > TARGET_INT_MAX ) {
        *p = 256;
    }
}

void CmdSysSetMaxOptimization( void )
/***********************************/
{
    GenSwitches |= CGSW_GEN_I_MATH_INLINE;
}

static void defineM_IX86Macro( void )
{
    unsigned cpu;
    char buff[32];

    switch( GET_CPU( CpuSwitches ) ) {
    case CPU_86:        cpu = 0; break;
    case CPU_186:       cpu = 100; break;
    case CPU_286:       cpu = 200; break;
    case CPU_386:       cpu = 300; break;
    case CPU_486:       cpu = 400; break;
    case CPU_586:       cpu = 500; break;
    case CPU_686:       cpu = 600; break;
    }
    sprintf( buff, "_M_IX86=%u", cpu );
    PreDefineStringMacro( buff );
}

static char *setTargetSystem( OPT_STORAGE *data )
{
    char *target_name = NULL;

    if( data->bt ) {
        char *target = SetStringOption( NULL, &(data->bt_value) );
        SetTargetLiteral( &target_name, strupr( target ) );
        CMemFree( target );
    }
    /*
     * -zw overrides a build target name and setting
     */
    switch( data->win ) {
    case OPT_ENUM_win_zw:
        SetTargetLiteral( &target_name, "WINDOWS" );
        break;
#if _CPU == 8086
    case OPT_ENUM_win_zW:
        SetTargetLiteral( &target_name, "CHEAP_WINDOWS" );
        break;
    case OPT_ENUM_win_zws:
        SetTargetLiteral( &target_name, "WINDOWS" );
        TargetSwitches |= CGSW_X86_SMART_WINDOWS;
        break;
    case OPT_ENUM_win_zWs:
        SetTargetLiteral( &target_name, "CHEAP_WINDOWS" );
        TargetSwitches |= CGSW_X86_SMART_WINDOWS;
        break;
#endif
    }

    if( target_name == NULL ) {
#if defined( __QNX__ )
        SetTargetLiteral( &target_name, "QNX" );
#elif defined( __LINUX__ )
        SetTargetLiteral( &target_name, "LINUX" );
#elif defined( __HAIKU__ )
        SetTargetLiteral( &target_name, "HAIKU" );
#elif defined( __OSX__ ) || defined( __APPLE__ )
        SetTargetLiteral( &target_name, "OSX" );
#elif defined( __SOLARIS__ ) || defined( __SUN__ )
        SetTargetLiteral( &target_name, "SOLARIS" );
#elif defined( __OS2__ )
        SetTargetLiteral( &target_name, "OS2" );
#elif defined( __NT__ )
        SetTargetLiteral( &target_name, "NT" );
#elif defined( __DOS__ )
        SetTargetLiteral( &target_name, "DOS" );
#elif defined( __BSD__ )
        SetTargetLiteral( &target_name, "BSD" );
#elif defined( __RDOS__ )
        SetTargetLiteral( &target_name, "RDOS" );
#else
        #error "Target System not defined"
#endif
    }

    TargetSystem = TS_OTHER;
    if( target_name != NULL ) {
        if( 0 == strcmp( target_name, "DOS" ) ) {
            TargetSystem = TS_DOS;
        } else if( 0 == strcmp( target_name, "OS2" ) ) {
            TargetSystem = TS_OS2;
        } else if( 0 == strcmp( target_name, "QNX" ) ) {
            TargetSystem = TS_QNX;
        } else if( 0 == strcmp( target_name, "WINDOWS" ) ) {
            TargetSystem = TS_WINDOWS;
        } else if( 0 == strcmp( target_name, "CHEAP_WINDOWS" ) ) {
#if _CPU == 8086
            TargetSystem = TS_CHEAP_WINDOWS;
#else
            TargetSystem = TS_WINDOWS;
#endif
            SetTargetLiteral( &target_name, "WINDOWS" );
#if _CPU == 386
        } else if( 0 == strcmp( target_name, "NETWARE" ) ) {
            TargetSystem = TS_NETWARE;
        } else if( 0 == strcmp( target_name, "NETWARE5" ) ) {
            TargetSystem = TS_NETWARE5;
            SetTargetLiteral( &target_name, "NETWARE" );
        } else if( 0 == strcmp( target_name, "NT" ) ) {
            TargetSystem = TS_NT;
        } else if( 0 == strcmp( target_name, "LINUX" ) ) {
            TargetSystem = TS_LINUX;
        } else if( 0 == strcmp( target_name, "RDOS" ) ) {
            TargetSystem = TS_RDOS;
        } else if( 0 == strcmp( target_name, "HAIKU" )
              || 0 == strcmp( target_name, "OSX" )
              || 0 == strcmp( target_name, "SOLARIS" )
              || 0 == strcmp( target_name, "BSD" ) ) {
            TargetSystem = TS_UNIX;
#endif
        }
    }
    return( target_name );
}

static void setFinalTargetSystem( OPT_STORAGE *data, char *target_name )
{
    char buff[128];
    bool target_multi_thread;

    if( CompFlags.non_iso_compliant_names_enabled ) {
#if _CPU == 8086
        PreDefineStringMacro( "M_I86" );
#else
        PreDefineStringMacro( "M_I386" );
#endif
    }
#if _CPU == 8086
    PreDefineStringMacro( "_M_I86" );
    PreDefineStringMacro( "__I86__" );
#else
    PreDefineStringMacro( "_M_I386" );
    PreDefineStringMacro( "__386__" );
#endif
    PreDefineStringMacro( "__X86__" );
    PreDefineStringMacro( "_X86_" );
    /*
     * create macro for target system
     */
    if( target_name != NULL ) {
        strcpy( buff, "__" );
        strcat( buff, target_name );
        strcat( buff, "__" );
        PreDefineStringMacro( buff );
    }
    /*
     * additional setup for target system
     */
    target_multi_thread = true;
    switch( TargetSystem ) {
    case TS_DOS:
        if( CompFlags.non_iso_compliant_names_enabled ) {
            PreDefineStringMacro( "MSDOS" );
        }
        PreDefineStringMacro( "_DOS" );
        break;
    case TS_CHEAP_WINDOWS:
        PreDefineStringMacro( "__CHEAP_WINDOWS__" );
        /* fall through */
    case TS_WINDOWS:
        target_multi_thread = false;
#if _CPU == 8086
        PreDefineStringMacro( "_WINDOWS" );
        TargetSwitches |= CGSW_X86_WINDOWS | CGSW_X86_CHEAP_WINDOWS;
#else
        PreDefineStringMacro( "__WINDOWS_386__" );
#endif
        break;
    case TS_NETWARE5:
        PreDefineStringMacro( "__NETWARE5__" );
        /* fall through */
    case TS_NETWARE:
        PreDefineStringMacro( "__NETWARE_386__" );
        break;
    case TS_RDOS:
        PreDefineStringMacro( "_RDOS" );
        break;
    case TS_NT:
        PreDefineStringMacro( "_WIN32" );
        break;
    case TS_QNX:
    case TS_LINUX:
    case TS_UNIX:
        PreDefineStringMacro( "__UNIX__" );
        break;
    default:
        break;
    }
    if( data->bm
      || target_multi_thread && data->bd ) {
        CompFlags.target_multi_thread = true;
    }

    if( target_name != NULL ) {
        strcpy( buff, target_name );
        strcat( buff, "_INCLUDE" );
        MergeIncludeFromEnv( buff );
    }
    MergeIncludeFromEnv( "INCLUDE" );
    CMemFree( target_name );
}

static void setMemoryModel( OPT_STORAGE *data )
{
    char lib_model = '\0';
    unsigned bit;
    bool target_win;

    if( data->mem_model == OPT_ENUM_mem_model_default ) {
#if _CPU == 8086
        data->mem_model = OPT_ENUM_mem_model_ms;
#else
        if( TargetSystem == TS_NETWARE
          || TargetSystem == TS_NETWARE5 ) {
            data->mem_model = OPT_ENUM_mem_model_ms;
        } else {
            data->mem_model = OPT_ENUM_mem_model_mf;
        }
#endif
    }
    bit = 0;
    if( CompFlags.non_iso_compliant_names_enabled ) {
        switch( data->mem_model ) {
        case OPT_ENUM_mem_model_ms:
            PreDefineStringMacro( "M_" MM_ARCH "SM" );
            break;
        case OPT_ENUM_mem_model_mm:
            PreDefineStringMacro( "M_" MM_ARCH "MM" );
            break;
        case OPT_ENUM_mem_model_ml:
            PreDefineStringMacro( "M_" MM_ARCH "LM" );
            break;
        case OPT_ENUM_mem_model_mc:
            PreDefineStringMacro( "M_" MM_ARCH "CM" );
            break;
#if _CPU == 8086
        case OPT_ENUM_mem_model_mh:
            PreDefineStringMacro( "M_" MM_ARCH "HM" );
            break;
#else
        case OPT_ENUM_mem_model_mfi:
        case OPT_ENUM_mem_model_mf:
            PreDefineStringMacro( "M_" MM_ARCH "FM" );
            break;
#endif
        default:
            break;
        }
    }
    DataPtrSize = TARGET_POINTER;
    CodePtrSize = TARGET_POINTER;
    switch( data->mem_model ) {
    case OPT_ENUM_mem_model_ms:
        lib_model = 's';
        PreDefineStringMacro( "_M_" MM_ARCH "SM" );
        PreDefineStringMacro( "__SMALL__" );
        CompFlags.strings_in_code_segment = false;
        TargetSwitches &= ~CGSW_X86_CONST_IN_CODE;
        bit |= CGSW_X86_CHEAP_POINTER;
        break;
    case OPT_ENUM_mem_model_mm:
        lib_model = 'm';
        WatcallInfo.cclass_target |= FECALL_X86_FAR_CALL;
        CodePtrSize = TARGET_FAR_POINTER;
        PreDefineStringMacro( "_M_" MM_ARCH "MM" );
        PreDefineStringMacro( "__MEDIUM__" );
        CompFlags.strings_in_code_segment = false;
        TargetSwitches &= ~CGSW_X86_CONST_IN_CODE;
        bit |= CGSW_X86_BIG_CODE | CGSW_X86_CHEAP_POINTER;
        break;
    case OPT_ENUM_mem_model_mc:
        lib_model = 'c';
        PreDefineStringMacro( "_M_" MM_ARCH "CM" );
        PreDefineStringMacro( "__COMPACT__" );
        DataPtrSize = TARGET_FAR_POINTER;
        bit |= CGSW_X86_BIG_DATA | CGSW_X86_CHEAP_POINTER;
        break;
    case OPT_ENUM_mem_model_ml:
        lib_model = 'l';
        PreDefineStringMacro( "_M_" MM_ARCH "LM" );
        PreDefineStringMacro( "__LARGE__" );
        WatcallInfo.cclass_target |= FECALL_X86_FAR_CALL;
        CodePtrSize = TARGET_FAR_POINTER;
        DataPtrSize = TARGET_FAR_POINTER;
        bit |= CGSW_X86_BIG_CODE | CGSW_X86_BIG_DATA | CGSW_X86_CHEAP_POINTER;
        break;
#if _CPU == 8086
    case OPT_ENUM_mem_model_mh:
        lib_model = 'h';
        PreDefineStringMacro( "_M_" MM_ARCH "HM" );
        PreDefineStringMacro( "__HUGE__" );
        WatcallInfo.cclass_target |= FECALL_X86_FAR_CALL;
        CodePtrSize = TARGET_FAR_POINTER;
        DataPtrSize = TARGET_FAR_POINTER;
        bit |= CGSW_X86_BIG_CODE | CGSW_X86_BIG_DATA;
        break;
#else
    case OPT_ENUM_mem_model_mfi:
        CompFlags.mfi_switch_used = true;
        /* fall through */
    case OPT_ENUM_mem_model_mf:
        lib_model = 's';
        PreDefineStringMacro( "_M_" MM_ARCH "FM" );
        PreDefineStringMacro( "__FLAT__" );
        bit |= CGSW_X86_FLAT_MODEL | CGSW_X86_CHEAP_POINTER;
        break;
#endif
    default:
        DbgNever();
    }
    /*
     * setup "floating" segment registers
     * any memory model needs at least one floating segment register
     * by default it is ES segment register for 16-bit code and
     * GS segment register for 32-bit code
     */

    /*
     * setup SS segment register floating
     */
    if( data->zu ) {
        CompFlags.zu_switch_used = true;
        bit |= CGSW_X86_FLOATING_SS;
    }
    /*
     * setup ES segment register floating
     */
    if( (bit & CGSW_X86_FLAT_MODEL) == 0 ) {
        bit |= CGSW_X86_FLOATING_ES;
    }
    /*
     * setup DS segment register floating
     */
    if( bit & CGSW_X86_BIG_DATA ) {
        bit |= CGSW_X86_FLOATING_DS;
    }
#if _CPU == 386
    /*
     * 32-bit code with zdl option has DS pegged
     */
    if( data->zdl ) {
        TargetSwitches |= CGSW_X86_LOAD_DS_DIRECTLY;
        bit &= ~CGSW_X86_FLOATING_DS;
    }
#endif
#if _CPU == 8086
    /*
     * any model of 16-bit Windows has DS pegged
     */
    if( TargetSystem == TS_WINDOWS
      || TargetSystem == TS_CHEAP_WINDOWS ) {
        bit &= ~CGSW_X86_FLOATING_DS;
    }
#endif
    switch( data->ds_peg ) {
    case OPT_ENUM_ds_peg_zdp:
        TargetSwitches &= ~CGSW_X86_FLOATING_DS;
        bit &= ~CGSW_X86_FLOATING_DS;
        break;
    case OPT_ENUM_ds_peg_zdf:
        TargetSwitches |= CGSW_X86_FLOATING_DS;
        bit &= ~CGSW_X86_FLOATING_DS;
        break;
    }
    /*
     * setup FS segment register floating
     *
     * FS segment register is used by 32-bit C++ run-time libraries
     */
    if( GET_CPU( CpuSwitches ) >= CPU_386 ) {
#if _CPU == 8086
        bit |= CGSW_X86_FLOATING_FS;
#else
        if( TargetSystem == TS_WINDOWS ) {
            bit |= CGSW_X86_FLOATING_FS;
        }
#endif
    }
    switch( data->fs_peg ) {
    case OPT_ENUM_fs_peg_zfp:
        TargetSwitches &= ~CGSW_X86_FLOATING_FS;
        bit &= ~CGSW_X86_FLOATING_FS;
        break;
    case OPT_ENUM_fs_peg_zff:
        TargetSwitches |= CGSW_X86_FLOATING_FS;
        bit &= ~CGSW_X86_FLOATING_FS;
        break;
    }
    /*
     * setup GS segment register floating
     */
    if( GET_CPU( CpuSwitches ) >= CPU_386 ) {
        bit |= CGSW_X86_FLOATING_GS;
    }
    switch( data->gs_peg ) {
    case OPT_ENUM_gs_peg_zgp:
        TargetSwitches &= ~CGSW_X86_FLOATING_GS;
        bit &= ~CGSW_X86_FLOATING_GS;
        break;
    case OPT_ENUM_gs_peg_zgf:
        TargetSwitches |= CGSW_X86_FLOATING_GS;
        bit &= ~CGSW_X86_FLOATING_GS;
        break;
    }

    TargetSwitches &= ~(CGSW_X86_FLAT_MODEL | CGSW_X86_BIG_CODE | CGSW_X86_BIG_DATA | CGSW_X86_CHEAP_POINTER);
    TargetSwitches |= bit;

    if( GET_CPU( CpuSwitches ) < CPU_386 ) {
        /*
         * issue warning message if /zf[f|p] or /zg[f|p] spec'd?
         */
        TargetSwitches &= ~(CGSW_X86_FLOATING_FS | CGSW_X86_FLOATING_GS);
    }

#if _CPU == 8086
    target_win = ( TargetSystem == TS_WINDOWS || TargetSystem == TS_CHEAP_WINDOWS );
    if( data->bm ) { /* .DLL */
        strcpy( DLL_CLIB_Name, "2clibmt?" );
    } else {
        if( target_win ) {
            strcpy( DLL_CLIB_Name, "2clib?" );
        } else {
            strcpy( DLL_CLIB_Name, "2clibdl?" );
        }
    }
    if( data->bm ) {
        strcpy( CLIB_Name, "2clibmt?" );
    } else if( data->bd ) {
        if( target_win ) {
            strcpy( CLIB_Name, "2clib?" );
        } else {
            strcpy( CLIB_Name, "2clibdl?" );
        }
    } else {
        strcpy( CLIB_Name, "2clib?" );
    }
    if( CompFlags.excs_enabled ) {
        if( data->bm ) { /* .DLL */
            strcpy( DLL_WCPPLIB_Name, "4plbxmt?" );
        } else {
            if( target_win ) {
                strcpy( DLL_WCPPLIB_Name, "4plbx?" );
            } else {
                strcpy( DLL_WCPPLIB_Name, "4plbxmt?" );
            }
        }
        if( data->bm ) {
            strcpy( WCPPLIB_Name, "4plbxmt?" );
        } else if( data->bd ) {
            if( target_win ) {
                strcpy( WCPPLIB_Name, "4plbx?" );
            } else {
                strcpy( WCPPLIB_Name, "4plbxmt?" );
            }
        } else {
            strcpy( WCPPLIB_Name, "4plbx?" );
        }
    } else {
        if( data->bm ) { /* .DLL */
            strcpy( DLL_WCPPLIB_Name, "4plibmt?" );
        } else {
            if( target_win ) {
                strcpy( DLL_WCPPLIB_Name, "4plib?" );
            } else {
                strcpy( DLL_WCPPLIB_Name, "4plibmt?" );
            }
        }
        if( data->bm ) {
            strcpy( WCPPLIB_Name, "4plibmt?" );
        } else if( data->bd ) {
            if( target_win ) {
                strcpy( WCPPLIB_Name, "4plib?" );
            } else {
                strcpy( WCPPLIB_Name, "4plibmt?" );
            }
        } else {
            strcpy( WCPPLIB_Name, "4plib?" );
        }
    }
    if( GET_FPU_FPC( CpuSwitches ) ) {
        strcpy( MATHLIB_Name, "5math?" );
        EmuLib_Name = NULL;
    } else {
        strcpy( MATHLIB_Name, "7math87?" );
        if( GET_FPU_EMU( CpuSwitches ) ) {
            EmuLib_Name = "8emu87";
        } else {
            EmuLib_Name = "8noemu87";
        }
    }
#else
    target_win = ( TargetSystem == TS_WINDOWS );
    if( CompFlags.register_conventions ) {
        lib_model = 'r';
        PreDefineStringMacro( "__3R__" );
    } else {
        lib_model = 's';
        PreDefineStringMacro( "__3S__" );
    }
    strcpy( CDLL_Name, "1clb?dll" );
    strcpy( CLIB_Name, "2clib3?" );
    strcpy( DLL_CLIB_Name, "2clib3?" );
    if( CompFlags.excs_enabled ) {
        strcpy( WCPPDLL_Name, "3plb?dllx" );
        if( data->bm ) { /* .DLL */
            strcpy( DLL_WCPPLIB_Name, "4plbxmt3?" );
        } else {
            if( target_win ) {
                strcpy( DLL_WCPPLIB_Name, "4plbx3?" );
            } else {
                strcpy( DLL_WCPPLIB_Name, "4plbxmt3?" );
            }
        }
        if( data->bm ) {
            strcpy( WCPPLIB_Name, "4plbxmt3?" );
        } else if( data->bd ) {
            if( target_win ) {
                strcpy( WCPPLIB_Name, "4plbx3?" );
            } else {
                strcpy( WCPPLIB_Name, "4plbxmt3?" );
            }
        } else {
            strcpy( WCPPLIB_Name, "4plbx3?" );
        }
    } else {
        strcpy( WCPPDLL_Name, "3plb?dll" );
        if( data->bm ) { /* .DLL */
            strcpy( DLL_WCPPLIB_Name, "4plibmt3?" );
        } else {
            if( target_win ) {
                strcpy( DLL_WCPPLIB_Name, "4plib3?" );
            } else {
                strcpy( DLL_WCPPLIB_Name, "4plibmt3?" );
            }
        }
        if( data->bm ) {
            strcpy( WCPPLIB_Name, "4plibmt3?" );
        } else if( data->bd ) {
            if( target_win ) {
                strcpy( WCPPLIB_Name, "4plib3?" );
            } else {
                strcpy( WCPPLIB_Name, "4plibmt3?" );
            }
        } else {
            strcpy( WCPPLIB_Name, "4plib3?" );
        }
    }
    if( GET_FPU_FPC( CpuSwitches ) ) {
        if( data->br ) {
            strcpy( MATHLIB_Name, "5mth?dll" );
        } else {
            strcpy( MATHLIB_Name, "5math3?" );
        }
        EmuLib_Name = NULL;
    } else {
        if( data->br ) {
            strcpy( MATHLIB_Name, "7mt7?dll" );
        } else {
            strcpy( MATHLIB_Name, "7math387?" );
        }
        if( GET_FPU_EMU( CpuSwitches ) ) {
            EmuLib_Name = "8emu387";
        } else {
            EmuLib_Name = "8noemu387";
        }
    }
    *strchr( CDLL_Name, '?' ) = lib_model;
    *strchr( WCPPDLL_Name, '?' ) = lib_model;
#endif
    *strchr( CLIB_Name, '?' ) = lib_model;
    *strchr( MATHLIB_Name, '?' ) = lib_model;
    *strchr( WCPPLIB_Name, '?' ) = lib_model;
    *strchr( DLL_CLIB_Name, '?' ) = lib_model;
    *strchr( DLL_WCPPLIB_Name, '?' ) = lib_model;
}

static void setIntelArchitecture( OPT_STORAGE *data )
{
#if _CPU == 8086
    /*
     * Set CPU for 16-bit architecture, default is 8086
     */
    switch( data->arch_i86 ) {
    case OPT_ENUM_arch_i86_default:
    case OPT_ENUM_arch_i86__0:
        SET_CPU( CpuSwitches, CPU_86 );
        break;
    case OPT_ENUM_arch_i86__1:
        SET_CPU( CpuSwitches, CPU_186 );
        break;
    case OPT_ENUM_arch_i86__2:
        SET_CPU( CpuSwitches, CPU_286 );
        break;
    case OPT_ENUM_arch_i86__3:
        SET_CPU( CpuSwitches, CPU_386 );
        break;
    case OPT_ENUM_arch_i86__4:
        SET_CPU( CpuSwitches, CPU_486 );
        break;
    case OPT_ENUM_arch_i86__5:
        SET_CPU( CpuSwitches, CPU_586 );
        break;
    case OPT_ENUM_arch_i86__6:
        SET_CPU( CpuSwitches, CPU_686 );
        break;
    }
#else
    /*
     * Set CPU for 32-bit architecture, default is 80686
     */
    switch( data->arch_386 ) {
    case OPT_ENUM_arch_386__3s:
        CompFlags.register_conventions = false;
        SET_CPU( CpuSwitches, CPU_386 );
        break;
    case OPT_ENUM_arch_386__3r:
        CompFlags.register_conventions = true;
        SET_CPU( CpuSwitches, CPU_386 );
        break;
    case OPT_ENUM_arch_386__4s:
        CompFlags.register_conventions = false;
        SET_CPU( CpuSwitches, CPU_486 );
        break;
    case OPT_ENUM_arch_386__4r:
        CompFlags.register_conventions = true;
        SET_CPU( CpuSwitches, CPU_486 );
        break;
    case OPT_ENUM_arch_386__5s:
        CompFlags.register_conventions = false;
        SET_CPU( CpuSwitches, CPU_586 );
        break;
    case OPT_ENUM_arch_386__5r:
        CompFlags.register_conventions = true;
        SET_CPU( CpuSwitches, CPU_586 );
        break;
    case OPT_ENUM_arch_386__6s:
        CompFlags.register_conventions = false;
        SET_CPU( CpuSwitches, CPU_686 );
        break;
    case OPT_ENUM_arch_386_default:
    case OPT_ENUM_arch_386__6r:
        CompFlags.register_conventions = true;
        SET_CPU( CpuSwitches, CPU_686 );
        break;
    }
    /*
     * NETWARE uses stack based calling conventions
     * by default - silly people.
     */
    if( TargetSystem == TS_NETWARE
      || TargetSystem == TS_NETWARE5 ) {
        CompFlags.register_conventions = false;
    }
#endif

    /*
     * Set FPU level, default is 8087/80387
     */
    switch( data->intel_fpu_level ) {
    case OPT_ENUM_intel_fpu_level_fp6:
        SET_FPU_LEVEL( CpuSwitches, FPU_686 );
        break;
    case OPT_ENUM_intel_fpu_level_fp5:
        SET_FPU_LEVEL( CpuSwitches, FPU_586 );
        break;
#if _CPU == 386
    case OPT_ENUM_intel_fpu_level_default:
#endif
    case OPT_ENUM_intel_fpu_level_fp3:
        SET_FPU_LEVEL( CpuSwitches, FPU_387 );
        break;
#if _CPU == 8086
    case OPT_ENUM_intel_fpu_level_default:
#endif
    case OPT_ENUM_intel_fpu_level_fp2:
        SET_FPU_LEVEL( CpuSwitches, FPU_87 );
        break;
    }
    /*
     * Set FPU model, default is fpi
     */
    switch( data->intel_fpu_model ) {
    case OPT_ENUM_intel_fpu_model_default:
    case OPT_ENUM_intel_fpu_model_fpi:
#if _CPU == 386
        if( TargetSystem != TS_WINDOWS ) {
#endif
            SET_FPU_EMU( CpuSwitches );
            break;
#if _CPU == 386
        }
        /* fall through */
#endif
    case OPT_ENUM_intel_fpu_model_fpi87:
        SET_FPU_INLINE( CpuSwitches );
        break;
    case OPT_ENUM_intel_fpu_model_fpc:
        SET_FPU_FPC( CpuSwitches );
        CompFlags.op_switch_used = false;
        break;
    }

    if( data->fpd ) {
        TargetSwitches |= CGSW_X86_P5_DIVIDE_CHECK;
    }
    /*
     * If using NetWare, set Stack87 unless the target
     * is NetWare 5 or higher.
     */
    if( data->fpr ) {
        Stack87 = 4;
    } else if( TargetSystem == TS_NETWARE ) {
        Stack87 = 4;
    } else {
        Stack87 = 8;
    }
    if( data->_87d ) {
        if( data->_87d_value > 0 ) {
            Stack87 = data->_87d_value;
        }
    }

    if( data->zfw ) {
        TargetSwitches |= CGSW_X86_GEN_FWAIT_386;
    }
    switch( data->fp_rounding ) {
    case OPT_ENUM_fp_rounding_zri:
        GenSwitches |= CGSW_GEN_FPU_ROUNDING_INLINE;
        break;
    case OPT_ENUM_fp_rounding_zro:
        GenSwitches |= CGSW_GEN_FPU_ROUNDING_OMIT;
        break;
    default:
        break;
    }
}

static void defineFSRegistration( void )
{
    if( CompFlags.zo_switch_used ) {
        CErr1( WARN_ZO_OBSOLETE );
    }
    CompFlags.rw_registration = true;
#if _CPU == 386
    if( TargetSystem == TS_NT
      || TargetSystem == TS_OS2 ) {
        CompFlags.fs_registration = true;
    }
#endif
}

static void macroDefs( void )
{
    if( GenSwitches & CGSW_GEN_I_MATH_INLINE ) {
        DefSwitchMacro( "OM" );
    }
    switch( TargetSwitches & (CGSW_X86_BIG_DATA | CGSW_X86_BIG_CODE | CGSW_X86_CHEAP_POINTER | CGSW_X86_FLAT_MODEL) ) {
    case CGSW_X86_CHEAP_POINTER:
        DefSwitchMacro( "MS" );
        break;
    case CGSW_X86_BIG_CODE | CGSW_X86_CHEAP_POINTER:
        DefSwitchMacro( "MM" );
        break;
    case CGSW_X86_BIG_DATA | CGSW_X86_CHEAP_POINTER:
        DefSwitchMacro( "MC" );
        break;
    case CGSW_X86_BIG_CODE | CGSW_X86_BIG_DATA | CGSW_X86_CHEAP_POINTER:
        DefSwitchMacro( "ML" );
        break;
    case CGSW_X86_BIG_CODE | CGSW_X86_BIG_DATA:
        DefSwitchMacro( "MH" );
        break;
    case CGSW_X86_FLAT_MODEL | CGSW_X86_CHEAP_POINTER:
        DefSwitchMacro( "MF" );
        break;
    }
    if( TargetSwitches & CGSW_X86_FLOATING_FS ) {
        DefSwitchMacro( "ZFF" );
    } else {
        DefSwitchMacro( "ZFP" );
    }
    if( TargetSwitches & CGSW_X86_FLOATING_GS ) {
        DefSwitchMacro( "ZGF" );
    } else {
        DefSwitchMacro( "ZGP" );
    }
    if( TargetSwitches & CGSW_X86_FLOATING_DS ) {
        DefSwitchMacro( "ZDF" );
    } else {
        DefSwitchMacro( "ZDP" );
    }
    if( TargetSwitches & CGSW_X86_FLOATING_SS ) {
        DefSwitchMacro( "ZU" );
    }
    if( TargetSwitches & CGSW_X86_INDEXED_GLOBALS ) {
        DefSwitchMacro( "XGV" );
    }
    if( TargetSwitches & CGSW_X86_WINDOWS ) {
        DefSwitchMacro( "ZW" );
    }
    if( TargetSwitches & CGSW_X86_SMART_WINDOWS ) {
        DefSwitchMacro( "ZWS" );
    }
    if( GenSwitches & CGSW_GEN_NO_CALL_RET_TRANSFORM ) {
        DefSwitchMacro( "OC" );
    }
    if( TargetSwitches & CGSW_X86_NEED_STACK_FRAME ) {
        DefSwitchMacro( "OF" );
    }
    if( GenSwitches & CGSW_GEN_NO_OPTIMIZATION ) {
        DefSwitchMacro( "OD" );
    }
    if( GenSwitches & CGSW_GEN_RELAX_ALIAS ) {
        DefSwitchMacro( "OA" );
    }
    if( GenSwitches & CGSW_GEN_BRANCH_PREDICTION ) {
        DefSwitchMacro( "OB" );
    }
    if( GenSwitches & CGSW_GEN_LOOP_OPTIMIZATION ) {
        DefSwitchMacro( "OL" );
    }
    if( GenSwitches & CGSW_GEN_INS_SCHEDULING ) {
        DefSwitchMacro( "OR" );
    }
    if( GenSwitches & CGSW_GEN_FPU_ROUNDING_INLINE ) {
        DefSwitchMacro( "ZRI" );
    }
    if( GenSwitches & CGSW_GEN_FPU_ROUNDING_OMIT ) {
        DefSwitchMacro( "ZRO" );
    }
    if( TargetSwitches & CGSW_X86_GEN_FWAIT_386 ) {
        DefSwitchMacro( "ZFW" );
    }
    if( CompFlags.signed_char ) {
        DefSwitchMacro( "J" );
    }
    if( CompFlags.use_pcheaders ) {
        DefSwitchMacro( "FH" );
    }
    if( CompFlags.no_pch_warnings ) {
        DefSwitchMacro( "FHQ" );
    }
    if( CompFlags.inline_intrinsics ) {
        DefSwitchMacro( "OI" );
    }
    if( CompFlags.zu_switch_used ) {
        DefSwitchMacro( "ZU" );
    }
    if( CompFlags.unique_functions ) {
        DefSwitchMacro( "OU" );
    }
#if _CPU == 386
    if( CompFlags.register_conventions ) {
        DefSwitchMacro( "3R" );
    } else {
        DefSwitchMacro( "3S" );
    }
#endif
    if( CompFlags.emit_names ) {
        DefSwitchMacro( "EN" );
    }
    if( CompFlags.save_restore_segregs ) {
        DefSwitchMacro( "R" );
    }
    if( CompFlags.zc_switch_used ) {
        DefSwitchMacro( "ZC" );
    }
    if( CompFlags.use_double_byte ) {
        DefSwitchMacro( "ZK" );
    }
    if( CompFlags.sg_switch_used ) {
        DefSwitchMacro( "SG" );
    }
    if( CompFlags.st_switch_used ) {
        DefSwitchMacro( "ST" );
    }
    if( CompFlags.bm_switch_used ) {
        PreDefineStringMacro( "_MT" );
        DefSwitchMacro( "BM" );
    }
    if( CompFlags.bd_switch_used ) {
        DefSwitchMacro( "BD" );
    }
    if( CompFlags.br_switch_used ) {
        PreDefineStringMacro( "_DLL" );
        DefSwitchMacro( "BR" );
    }
    if( CompFlags.bw_switch_used ) {
        DefSwitchMacro( "BW" );
    }
    if( CompFlags.zm_switch_used ) {
        DefSwitchMacro( "ZM" );
    }
    if( CompFlags.ep_switch_used ) {
        DefSwitchMacro( "EP" );
    }
    if( CompFlags.ee_switch_used ) {
        DefSwitchMacro( "EE" );
    }
    switch( CompInfo.dt_method_speced ) {
    case DTM_DIRECT :
        DefSwitchMacro( "XDT" );
        DefSwitchMacro( "XD" );
        break;
    case DTM_DIRECT_SMALL :
        DefSwitchMacro( "XDS" );
        break;
    case DTM_TABLE :
        DefSwitchMacro( "XS" );
        break;
    case DTM_DIRECT_TABLE :
        DefSwitchMacro( "XST" );
        break;
    case DTM_TABLE_SMALL :
        DefSwitchMacro( "XSS" );
        break;
    DbgDefault( "macroDefs -- invalid exception switch" );
    }

    switch( GET_CPU( CpuSwitches ) ) {
    case CPU_86:
        DefSwitchMacro( "0" );
        break;
    case CPU_186:
        DefSwitchMacro( "1" );
        break;
    case CPU_286:
        DefSwitchMacro( "2" );
        break;
    case CPU_386:
        DefSwitchMacro( "3" );
        break;
    case CPU_486:
        DefSwitchMacro( "4" );
        break;
    case CPU_586:
        DefSwitchMacro( "5" );
        break;
    case CPU_686:
        DefSwitchMacro( "6" );
        break;
    }

    defineM_IX86Macro();

    switch( GET_FPU_LEVEL( CpuSwitches ) ) {
    case FPU_87:
        DefSwitchMacro( "FP287" );
        break;
    case FPU_387:
        DefSwitchMacro( "FP387" );
        break;
    case FPU_586:
        DefSwitchMacro( "FP5" );
        break;
    case FPU_686:
        DefSwitchMacro( "FP6" );
        break;
    }
    if( GET_FPU_FPC( CpuSwitches ) ) {
        DefSwitchMacro( "FPC" );
    } else {
        if( GET_FPU_EMU( CpuSwitches ) ) {
            DefSwitchMacro( "FPI" );
        } else {
            DefSwitchMacro( "FPI87" );
        }
        PreDefineStringMacro( "__FPI__" );
    }
    if( CompFlags.op_switch_used ) {
        DefSwitchMacro( "OP" );
    }
    PreDefineStringMacro( "_INTEGRAL_MAX_BITS=64" );
#if _CPU == 386
    PreDefineStringMacro( "_STDCALL_SUPPORTED" );
#endif
    if( CompFlags.non_iso_compliant_names_enabled ) {
#if _CPU == 8086
        if( TargetSystem == TS_WINDOWS
          || TargetSystem == TS_CHEAP_WINDOWS ) {
            PreDefineStringMacro( "SOMLINK=__cdecl" );
            PreDefineStringMacro( "SOMDLINK=__far" );
        }
#else
        PreDefineStringMacro( "SOMLINK=_Syscall" );
        PreDefineStringMacro( "SOMDLINK=_Syscall" );
#endif
    }
}

static void miscAnalysis( OPT_STORAGE *data )
{
#if _CPU == 8086
    if( data->bd
      || data->zu ) {
        if( TargetSwitches & CGSW_X86_SMART_WINDOWS ) {
            CErr1( ERR_ZWS_MUST_HAVE_SS_DS_SAME );
        }
    }
#endif
    if( !CompFlags.save_restore_segregs ) {
        if( TargetSwitches & CGSW_X86_FLOATING_DS ) {
            HW_CTurnOff( WatcallInfo.save, HW_DS );
        }
        if( TargetSwitches & CGSW_X86_FLOATING_ES ) {
            HW_CTurnOff( WatcallInfo.save, HW_ES );
        }
        if( TargetSwitches & CGSW_X86_FLOATING_FS ) {
            HW_CTurnOff( WatcallInfo.save, HW_FS );
        }
        if( TargetSwitches & CGSW_X86_FLOATING_GS ) {
            HW_CTurnOff( WatcallInfo.save, HW_GS );
        }
    }
#if _CPU == 386
    if( !CompFlags.register_conventions ) {
        SetAuxStackConventions();
    }
#endif
    if( data->zx ) {
        HW_CTurnOff( WatcallInfo.save, HW_FLTS );
    }
}

void CmdSysAnalyse( OPT_STORAGE *data )
/*************************************/
{
    char *target_name = NULL;

    target_name = setTargetSystem( data );

    GenSwitches &= ~(CGSW_GEN_DBG_CV | CGSW_GEN_DBG_DF | CGSW_GEN_DBG_PREDEF);
    switch( data->dbg_output ) {
    case OPT_ENUM_dbg_output_hc:
        GenSwitches |= CGSW_GEN_DBG_CV;
        break;
    case OPT_ENUM_dbg_output_hda:
        if( data->fhd ) {
            CompFlags.pch_debug_info_opt = true;
        }
        GenSwitches |= CGSW_GEN_DBG_DF | CGSW_GEN_DBG_PREDEF;
        break;
    case OPT_ENUM_dbg_output_hw:
        break;
    case OPT_ENUM_dbg_output_hd:
    default:
        if( data->fhd ) {
            CompFlags.pch_debug_info_opt = true;
        }
        GenSwitches |= CGSW_GEN_DBG_DF;
        break;
    }

    setIntelArchitecture( data );

    /*
     * depends on architecture and fpu being set
     */
    setMemoryModel( data );

    if( data->g ) {
        SetStringOption( &GenCodeGroup, &(data->g_value) );
    }
    if( data->nc ) {
        SetStringOption( &CodeClassName, &(data->nc_value) );
    }
    if( data->nd ) {
        SetStringOption( &DataSegName, &(data->nd_value) );
    }
    if( data->nm ) {
        SetStringOption( &ModuleName, &(data->nm_value) );
    }
    if( data->nt ) {
        SetStringOption( &TextSegName, &(data->nt_value) );
    }
    if( data->oc ) {
        GenSwitches |= CGSW_GEN_NO_CALL_RET_TRANSFORM;
    }
    if( data->of ) {
        TargetSwitches |= CGSW_X86_NEED_STACK_FRAME;
    }
    if( data->of_plus ) {
        TargetSwitches |= CGSW_X86_NEED_STACK_FRAME;
        WatcallInfo.cclass_target |= FECALL_X86_GENERATE_STACK_FRAME;
    }
    if( data->om ) {
        GenSwitches |= CGSW_GEN_I_MATH_INLINE;
    }
    if( data->r ) {
        CompFlags.save_restore_segregs = true;
    }
    if( data->ri ) {
        CompFlags.returns_promoted = true;
    }
    if( data->sg ) {
        CompFlags.sg_switch_used = true;
    }
    if( data->st ) {
        CompFlags.st_switch_used = true;
    }
    if( data->zc ) {
        CompFlags.strings_in_code_segment = true;
        CompFlags.zc_switch_used = true;
        TargetSwitches |= CGSW_X86_CONST_IN_CODE;
    }
    if( data->zm ) {
        CompFlags.zm_switch_used = true;
    }
    if( data->zmf ) {
        CompFlags.zm_switch_used = true;
        CompFlags.zmf_switch_used = true;
    }
    if( data->zx ) {
        CompFlags.zx_switch_used = true;
    }
    if( data->zt ) {
        DataThreshold = data->zt_value;
    }
#if _CPU == 8086
    if( data->xgls ) {
        TargetSwitches |= CGSW_X86_NULL_SELECTOR_BAD;
    }
#else
    if( data->vcap ) {
        CompFlags.vc_alloca_parm = true;
    }
    if( data->br ) {
        CompFlags.br_switch_used = true;
    }
    if( data->ez ) {
        TargetSwitches |= CGSW_X86_EZ_OMF;
    }
    if( data->etp ) {
        TargetSwitches |= CGSW_X86_NEW_P5_PROFILING;
    }
    if( data->esp ) {
        TargetSwitches |= CGSW_X86_STATEMENT_COUNTING;
    }
    if( data->et ) {
        TargetSwitches |= CGSW_X86_P5_PROFILING;
    }
    if( data->et0 ) {
        TargetSwitches |= CGSW_X86_P5_PROFILING | CGSW_X86_P5_PROFILING_CTR0;
    }
    if( data->xgv ) {
        TargetSwitches |= CGSW_X86_INDEXED_GLOBALS;
    }
    if( data->zo ) {
        CompFlags.zo_switch_used = true;
    }
    if( data->zz ) {
        CompFlags.use_stdcall_at_number = false;
    }
#endif
    if( data->iso == OPT_ENUM_iso_za ) {
        GenSwitches &= ~CGSW_GEN_I_MATH_INLINE;
    }
    switch( data->intel_call_conv ) {
    case OPT_ENUM_intel_call_conv_ecc:
        DftCallConv = &CdeclInfo;
        break;
    case OPT_ENUM_intel_call_conv_ecd:
        DftCallConv = &StdcallInfo;
        break;
    case OPT_ENUM_intel_call_conv_ecf:
        DftCallConv = &FastcallInfo;
        break;
    case OPT_ENUM_intel_call_conv_eco:
        DftCallConv = &OptlinkInfo;
        break;
    case OPT_ENUM_intel_call_conv_ecp:
        DftCallConv = &PascalInfo;
        break;
    case OPT_ENUM_intel_call_conv_ecr:
        DftCallConv = &FortranInfo;
        break;
    case OPT_ENUM_intel_call_conv_ecs:
        DftCallConv = &SyscallInfo;
        break;
    case OPT_ENUM_intel_call_conv_ecw:
    case OPT_ENUM_intel_call_conv_default:
    default:
        DftCallConv = &WatcallInfo;
        break;
    }
    /*
     * frees 'target_name' memory
     */
    setFinalTargetSystem( data, target_name );
    miscAnalysis( data );
    defineFSRegistration();
    macroDefs();
}
