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
* Description:  Command line processing for C++ compiler (x86 targets)
*
****************************************************************************/


#include "plusplus.h"
#include "errdefns.h"
#include "memmgr.h"
#include "macro.h"
#include "cmdline.h"
#include "cgdata.h"
#include "codegen.h"
#include "pragdefn.h"
#ifdef __OSI__
 #include "ostype.h"
#endif

#include "cmdlnpr1.gh"
#include "cmdlnsys.h"

typedef enum {                  // flags to control memory model settings
    MMC_DS      = 0x01,         // 'ds' reg is pegged/floated by user
    MMC_FS      = 0x02,         // 'fs' reg is pegged/floated by user
    MMC_GS      = 0x04,         // 'gs' reg is pegged/floated by user
    MMC_WIN     = 0x08,         // model forced by Windows
    MMC_NETWARE = 0x10,         // model forced by NetWare
    MMC_NULL    = 0
} mem_model_control;

#if 0
#define DEF_SWITCHES_ALL        ( MEMORY_LOW_FAILS | ENABLE_FP_EXCEPTIONS )
#else
#define DEF_SWITCHES_ALL        ( MEMORY_LOW_FAILS )
#endif

#if _CPU != 386
    #define DEF_TARGET_SWITCHES CHEAP_POINTER
    #define DEF_SWITCHES 0
    #define DEFAULT_CPU CPU_86
    #define DEFAULT_FPU FPU_87
#else
    #define DEF_TARGET_SWITCHES CHEAP_POINTER|USE_32|FLAT_MODEL
    #define DEF_SWITCHES 0
    #define DEFAULT_CPU CPU_686
    #define DEFAULT_FPU FPU_387
#endif

#if _CPU == 386
#define MM_ARCH "386"
#else
#define MM_ARCH "I86"
#endif

void CmdSysInit( void )
/*********************/
{
    GenSwitches = DEF_SWITCHES | DEF_SWITCHES_ALL;
    TargetSwitches = DEF_TARGET_SWITCHES;
    SET_CPU( CpuSwitches, DEFAULT_CPU );
    SET_FPU( CpuSwitches, DEFAULT_FPU );
    SET_FPU_EMU( CpuSwitches );
    Stack87 = 8;
    CodeClassName = NULL;
    TextSegName = strsave( "" );
    DataSegName = strsave( "" );
    GenCodeGroup = strsave( "" );
    CompFlags.use_stdcall_at_number = 1;
    CompFlags.register_conventions = 1;
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
#if _CPU == 386
    return( "WPP386" );
#else
    return( "WPP" );
#endif
}

void CmdX86CheckStack87( unsigned *p )
/************************************/
{
    // 0 means no change to default
    if( *p <= 8 ) {
        return;
    }
    *p = 0;
}

void CmdX86CheckThreshold( unsigned *p )
/**************************************/
{
    // 0 is allowed
    if( *p <= 32767 ) {
        return;
    }
    *p = 256;
}

void CmdSysSetMaxOptimization( void )
/***********************************/
{
    TargetSwitches |= I_MATH_INLINE;
}

static void defineM_IX86Macro( void )
{
    unsigned cpu;
    char buff[32];

    strcpy( buff, "_M_IX86=" );
#if _CPU == 386
    cpu = 3;
#else
    cpu = 0;
#endif
    switch( GET_CPU( CpuSwitches ) ) {
    case CPU_86:        cpu = 0; break;
    case CPU_186:       cpu = 1; break;
    case CPU_286:       cpu = 2; break;
    case CPU_386:       cpu = 3; break;
    case CPU_486:       cpu = 4; break;
    case CPU_586:       cpu = 5; break;
    case CPU_686:       cpu = 6; break;
    }
    ConcatBase10( buff, cpu * 100 );
    PreDefineStringMacro( buff );
}

static void setWindowsSystem( void )
{
#if _CPU == 386
    PreDefineStringMacro( "__WINDOWS_386__" );
    TargetSwitches |= FLOATING_FS;
    SET_FPU_INLINE( CpuSwitches );
#else
    PreDefineStringMacro( "__WINDOWS__" );
    PreDefineStringMacro( "_WINDOWS" );
    TargetSwitches |= WINDOWS | CHEAP_WINDOWS;
    TargetSwitches &= ~ FLOATING_DS;
#endif
}


static void setFinalTargetSystem( OPT_STORAGE *data, char *target_name )
{
    char buff[128];

    TargetSystem = TS_OTHER;
#if _CPU == 386
    PreDefineStringMacro( "M_I386" );
    PreDefineStringMacro( "_M_I386" );
    PreDefineStringMacro( "__386__" );
#else
    PreDefineStringMacro( "M_I86" );
    PreDefineStringMacro( "_M_I86" );
    PreDefineStringMacro( "__I86__" );
#endif
    PreDefineStringMacro( "__X86__" );
    PreDefineStringMacro( "_X86_" );
    if( target_name == NULL ) {
#if defined( __OSI__ )
        switch( __OS ) {
        case OS_DOS:
        case OS_WIN:
            SetTargetLiteral( &target_name, "DOS" );
            break;
        case OS_OS2:
            SetTargetLiteral( &target_name, "OS2" );
            break;
        case OS_NT:
            SetTargetLiteral( &target_name, "NT" );
            break;
        }
#elif defined( __QNX__ )
        SetTargetLiteral( &target_name, "QNX" );
#elif defined( __LINUX__ )
        SetTargetLiteral( &target_name, "LINUX" );
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
#else
        #error "Target System not defined"
#endif
    }
    if( 0 == strcmp( target_name, "DOS" ) ) {
        TargetSystem = TS_DOS;
        PreDefineStringMacro( "MSDOS" );
        PreDefineStringMacro( "__DOS__" );
        PreDefineStringMacro( "_DOS" );
#if _CPU == 386
    } else if( 0 == strcmp( target_name, "NETWARE" ) ) {
        TargetSystem = TS_NETWARE;
        PreDefineStringMacro( "__NETWARE_386__" );
    } else if( 0 == strcmp( target_name, "NETWARE5" ) ) {
        TargetSystem = TS_NETWARE5;
        PreDefineStringMacro( "__NETWARE_386__" );
        PreDefineStringMacro( "__NETWARE__" );
        /* can get away with this because "netware5" is longer */
        strcpy( target_name, "NETWARE" );
    } else if( 0 == strcmp( target_name, "NT" ) ) {
        PreDefineStringMacro( "_WIN32" );
        TargetSystem = TS_NT;
    } else if( 0 == strcmp( target_name, "OS2" ) ) {
        TargetSystem = TS_OS2;
    } else if( 0 == strcmp( target_name, "QNX" ) ) {
        TargetSystem = TS_QNX;
        PreDefineStringMacro( "__QNX__" );
        PreDefineStringMacro( "__UNIX__" );
    } else if( 0 == strcmp( target_name, "LINUX" ) ) {
        TargetSystem = TS_LINUX;
        PreDefineStringMacro( "__LINUX__" );
        PreDefineStringMacro( "__UNIX__" );
#endif
    } else if( 0 == strcmp( target_name, "WINDOWS" ) ) {
        TargetSystem = TS_WINDOWS;
        setWindowsSystem();
    } else if( 0 == strcmp( target_name, "CHEAP_WINDOWS" ) ) {
#if _CPU == 8086
        TargetSystem = TS_CHEAP_WINDOWS;
#else
        TargetSystem = TS_WINDOWS;
#endif
        /* can get away with this because "cheap_windows" is longer */
        strcpy( target_name, "WINDOWS" );
        setWindowsSystem();
    } else {
        TargetSystem = TS_OTHER;
    }
    strcpy( buff, "__" );
    strcat( buff, target_name );
    strcat( buff, "__" );
    PreDefineStringMacro( buff );

    /*
    //  Note the hacks for windows/cheap_windows & netware/netware5, above.
    */
    strcpy( buff, target_name );
    strcat( buff, "_INCLUDE" );
    MergeIncludeFromEnv( buff );

    MergeIncludeFromEnv( "INCLUDE" );
    CMemFree( target_name );
    if( data->bm ) {
        CompFlags.target_multi_thread = 1;
    }
    if( data->bd ) {
        switch( TargetSystem ) {
        case TS_WINDOWS:
#if _CPU == 8086
        case TS_CHEAP_WINDOWS:
#endif
            break;
        default:
            CompFlags.target_multi_thread = 1;
        }
    }
}

static void setMemoryModel( OPT_STORAGE *data, mem_model_control control )
{
    char model;
    unsigned long bit;

    if( data->mem_model == OPT_mem_model_default ) {
#if _CPU == 386
        data->mem_model = OPT_mem_model_mf;
#else
        data->mem_model = OPT_mem_model_ms;
#endif
    }
#if _CPU == 386
    if( control & MMC_NETWARE ) {
        data->mem_model = OPT_mem_model_ms;
    }
#endif
    bit = 0;
    switch( data->mem_model ) {
    case OPT_mem_model_ms:
        model = 's';
        DataPtrSize = TARGET_POINTER;
        CodePtrSize = TARGET_POINTER;
        PreDefineStringMacro( "M_" MM_ARCH "SM" );
        PreDefineStringMacro( "_M_" MM_ARCH "SM" );
        PreDefineStringMacro( "__SMALL__" );
        CompFlags.strings_in_code_segment = 0;
        TargetSwitches &= ~CONST_IN_CODE;
        bit |= CHEAP_POINTER;
        break;
    case OPT_mem_model_mm:
        model = 'm';
        WatcallInfo.cclass |= FAR;
        DataPtrSize = TARGET_POINTER;
        CodePtrSize = TARGET_FAR_POINTER;
        PreDefineStringMacro( "M_" MM_ARCH "MM" );
        PreDefineStringMacro( "_M_" MM_ARCH "MM" );
        PreDefineStringMacro( "__MEDIUM__" );
        CompFlags.strings_in_code_segment = 0;
        TargetSwitches &= ~CONST_IN_CODE;
        bit |= BIG_CODE | CHEAP_POINTER;
        break;
    case OPT_mem_model_ml:
        model = 'l';
        PreDefineStringMacro( "M_" MM_ARCH "LM" );
        PreDefineStringMacro( "_M_" MM_ARCH "LM" );
        PreDefineStringMacro( "__LARGE__" );
        WatcallInfo.cclass |= FAR;
        CodePtrSize = TARGET_FAR_POINTER;
        DataPtrSize = TARGET_FAR_POINTER;
        bit |= BIG_CODE | BIG_DATA | CHEAP_POINTER;
        break;
    case OPT_mem_model_mc:
        model = 'c';
        PreDefineStringMacro( "M_" MM_ARCH "CM" );
        PreDefineStringMacro( "_M_" MM_ARCH "CM" );
        PreDefineStringMacro( "__COMPACT__" );
        CodePtrSize = TARGET_POINTER;
        DataPtrSize = TARGET_FAR_POINTER;
        bit |= BIG_DATA | CHEAP_POINTER;
        break;
#if _CPU == 386
    case OPT_mem_model_mfi:
        CompFlags.mfi_switch_used = 1;
        /* fall thru */
    case OPT_mem_model_mf:
        model = 's';
        DataPtrSize = TARGET_POINTER;
        CodePtrSize = TARGET_POINTER;
        PreDefineStringMacro( "M_" MM_ARCH "FM" );
        PreDefineStringMacro( "_M_" MM_ARCH "FM" );
        PreDefineStringMacro( "__FLAT__" );
        bit |= FLAT_MODEL | CHEAP_POINTER;
        break;
#else
    case OPT_mem_model_mh:
        model = 'h';
        PreDefineStringMacro( "M_" MM_ARCH "HM" );
        PreDefineStringMacro( "_M_" MM_ARCH "HM" );
        PreDefineStringMacro( "__HUGE__" );
        WatcallInfo.cclass |= FAR;
        CodePtrSize = TARGET_FAR_POINTER;
        DataPtrSize = TARGET_FAR_POINTER;
        bit |= BIG_CODE | BIG_DATA;
        break;
#endif
    default:
        DbgNever();
    }
// setup default "floating" segment registers
#if _CPU == 8086
    bit |= FLOATING_ES;
#else
    // 386 flat model needs at least one floating segment register
    bit |= FLOATING_GS;
    if( !( bit & FLAT_MODEL ) ) {
        bit |= FLOATING_ES;
    }
#endif
    if( bit & BIG_DATA ) {
        bit |= FLOATING_DS;
    }
    if( control & ( MMC_DS | MMC_WIN ) ) {
        bit &= ~FLOATING_DS;
    } else {
        TargetSwitches &= ~ FLOATING_DS;
    }
    if( control & MMC_FS ) {
        bit &= ~FLOATING_FS;
    } else {
        TargetSwitches &= ~ FLOATING_FS;
    }
    if( control & MMC_GS ) {
        bit &= ~FLOATING_GS;
    } else {
        TargetSwitches &= ~ FLOATING_GS;
    }
    TargetSwitches &= ~( FLAT_MODEL | BIG_CODE | BIG_DATA | CHEAP_POINTER
                    | FLOATING_ES);
    TargetSwitches |= bit;
#if _CPU == 8086
    if( data->bm ) { // .DLL
        strcpy( DLL_CLIB_Name, "2clibmt?" );
    } else {
        if( control & MMC_WIN ) {
            strcpy( DLL_CLIB_Name, "2clib?" );
        } else {
            strcpy( DLL_CLIB_Name, "2clibdl?" );
        }
    }
    if( data->bm ) {
        strcpy( CLIB_Name, "2clibmt?" );
    } else if( data->bd ) {
        if( control & MMC_WIN ) {
            strcpy( CLIB_Name, "2clib?" );
        } else {
            strcpy( CLIB_Name, "2clibdl?" );
        }
    } else {
        strcpy( CLIB_Name, "2clib?" );
    }
    if( CompFlags.excs_enabled ) {
        if( data->bm ) { // .DLL
            strcpy( DLL_WCPPLIB_Name, "4plbxmt?" );
        } else {
            if( control & MMC_WIN ) {
                strcpy( DLL_WCPPLIB_Name, "4plbx?" );
            } else {
                strcpy( DLL_WCPPLIB_Name, "4plbxmt?" );
            }
        }
        if( data->bm ) {
            strcpy( WCPPLIB_Name, "4plbxmt?" );
        } else if( data->bd ) {
            if( control & MMC_WIN ) {
                strcpy( WCPPLIB_Name, "4plbx?" );
            } else {
                strcpy( WCPPLIB_Name, "4plbxmt?" );
            }
        } else {
            strcpy( WCPPLIB_Name, "4plbx?" );
        }
    } else {
        if( data->bm ) { // .DLL
            strcpy( DLL_WCPPLIB_Name, "4plibmt?" );
        } else {
            if( control & MMC_WIN ) {
                strcpy( DLL_WCPPLIB_Name, "4plib?" );
            } else {
                strcpy( DLL_WCPPLIB_Name, "4plibmt?" );
            }
        }
        if( data->bm ) {
            strcpy( WCPPLIB_Name, "4plibmt?" );
        } else if( data->bd ) {
            if( control & MMC_WIN ) {
                strcpy( WCPPLIB_Name, "4plib?" );
            } else {
                strcpy( WCPPLIB_Name, "4plibmt?" );
            }
        } else {
            strcpy( WCPPLIB_Name, "4plib?" );
        }
    }
    if( GET_FPU_EMU( CpuSwitches ) ) {
        strcpy( MATHLIB_Name, "8math87?" );
        EmuLib_Name = "9emu87";
    } else if( GET_FPU_LEVEL( CpuSwitches ) == FPU_NONE ) {
        strcpy( MATHLIB_Name, "5math?" );
        EmuLib_Name = NULL;
    } else {
        strcpy( MATHLIB_Name, "8math87?" );
        EmuLib_Name = "9noemu87";
    }
#else
    if( CompFlags.register_conventions ) {
        model = 'r';
        PreDefineStringMacro( "__3R__" );
    } else {
        model = 's';
        PreDefineStringMacro( "__3S__" );
    }
    strcpy( CDLL_Name, "1clb?dll" );
    strcpy( CLIB_Name, "2clib3?" );
    strcpy( DLL_CLIB_Name, "2clib3?" );
    if( CompFlags.excs_enabled ) {
        strcpy( WCPPDLL_Name, "3plb?dllx" );
        if( data->bm ) { // .DLL
            strcpy( DLL_WCPPLIB_Name, "4plbxmt3?" );
        } else {
            if( control & MMC_WIN ) {
                strcpy( DLL_WCPPLIB_Name, "4plbx3?" );
            } else {
                strcpy( DLL_WCPPLIB_Name, "4plbxmt3?" );
            }
        }
        if( data->bm ) {
            strcpy( WCPPLIB_Name, "4plbxmt3?" );
        } else if( data->bd ) {
            if( control & MMC_WIN ) {
                strcpy( WCPPLIB_Name, "4plbx3?" );
            } else {
                strcpy( WCPPLIB_Name, "4plbxmt3?" );
            }
        } else {
            strcpy( WCPPLIB_Name, "4plbx3?" );
        }
    } else {
        strcpy( WCPPDLL_Name, "3plb?dll" );
        if( data->bm ) { // .DLL
            strcpy( DLL_WCPPLIB_Name, "4plibmt3?" );
        } else {
            if( control & MMC_WIN ) {
                strcpy( DLL_WCPPLIB_Name, "4plib3?" );
            } else {
                strcpy( DLL_WCPPLIB_Name, "4plibmt3?" );
            }
        }
        if( data->bm ) {
            strcpy( WCPPLIB_Name, "4plibmt3?" );
        } else if( data->bd ) {
            if( control & MMC_WIN ) {
                strcpy( WCPPLIB_Name, "4plib3?" );
            } else {
                strcpy( WCPPLIB_Name, "4plibmt3?" );
            }
        } else {
            strcpy( WCPPLIB_Name, "4plib3?" );
        }
    }
    if( GET_FPU_EMU( CpuSwitches ) ) {
        if( data->br ) {
            strcpy( MATHLIB_Name, "8mt7?dll" );
        } else {
            strcpy( MATHLIB_Name, "8math387?" );
        }
        EmuLib_Name = "9emu387";
    } else if( GET_FPU_LEVEL( CpuSwitches ) == FPU_NONE ) {
        if( data->br ) {
            strcpy( MATHLIB_Name, "5mth?dll" );
        } else {
            strcpy( MATHLIB_Name, "5math3?" );
        }
        EmuLib_Name = NULL;
    } else {
        if( data->br ) {
            strcpy( MATHLIB_Name, "8mt7?dll" );
        } else {
            strcpy( MATHLIB_Name, "8math387?" );
        }
        EmuLib_Name = "9noemu387";
    }
    *strchr( CDLL_Name, '?' ) = model;
    *strchr( WCPPDLL_Name, '?' ) = model;
#endif
    *strchr( CLIB_Name, '?' ) = model;
    *strchr( MATHLIB_Name, '?' ) = model;
    *strchr( WCPPLIB_Name, '?' ) = model;
    *strchr( DLL_CLIB_Name, '?' ) = model;
    *strchr( DLL_WCPPLIB_Name, '?' ) = model;
}

static void setIntelArchitecture( OPT_STORAGE *data, mem_model_control control )
{
#if _CPU == 386
    switch( data->arch_386 ) {
    case OPT_arch_386__3s:
        CompFlags.register_conventions = 0;
        SET_CPU( CpuSwitches, CPU_386 );
        break;
    case OPT_arch_386__3r:
        CompFlags.register_conventions = 1;
        SET_CPU( CpuSwitches, CPU_386 );
        break;
    case OPT_arch_386__4s:
        CompFlags.register_conventions = 0;
        SET_CPU( CpuSwitches, CPU_486 );
        break;
    case OPT_arch_386__4r:
        CompFlags.register_conventions = 1;
        SET_CPU( CpuSwitches, CPU_486 );
        break;
    case OPT_arch_386__5s:
        CompFlags.register_conventions = 0;
        SET_CPU( CpuSwitches, CPU_586 );
        break;
    case OPT_arch_386__5r:
        CompFlags.register_conventions = 1;
        SET_CPU( CpuSwitches, CPU_586 );
        break;
    case OPT_arch_386__6s:
        CompFlags.register_conventions = 0;
        SET_CPU( CpuSwitches, CPU_686 );
        break;
    case OPT_arch_386__6r:
        CompFlags.register_conventions = 1;
        SET_CPU( CpuSwitches, CPU_686 );
        break;
    }
    if( control & MMC_NETWARE ) {
        CompFlags.register_conventions = 0;
    }
#else
    control = control;
    switch( data->arch_i86 ) {
    case OPT_arch_i86__0:
        SET_CPU( CpuSwitches, CPU_86 );
        break;
    case OPT_arch_i86__1:
        SET_CPU( CpuSwitches, CPU_186 );
        break;
    case OPT_arch_i86__2:
        SET_CPU( CpuSwitches, CPU_286 );
        break;
    case OPT_arch_i86__3:
        SET_CPU( CpuSwitches, CPU_386 );
        break;
    case OPT_arch_i86__4:
        SET_CPU( CpuSwitches, CPU_486 );
        break;
    case OPT_arch_i86__5:
        SET_CPU( CpuSwitches, CPU_586 );
        break;
    case OPT_arch_i86__6:
        SET_CPU( CpuSwitches, CPU_686 );
        break;
    }
#endif
    defineM_IX86Macro();
}

static void defineFSRegistration( void )
{
    if( CompFlags.zo_switch_used ) {
        CErr1( WARN_ZO_OBSOLETE );
    }
    CompFlags.rw_registration = TRUE;
#if _CPU == 386
    if( TargetSystem == TS_NT
     || TargetSystem == TS_OS2 ) {
        CompFlags.fs_registration = TRUE;
    }
#endif
}

static void defEmu( void )
{
    if( GET_FPU_EMU( CpuSwitches ) ) {
        DefSwitchMacro( "FPI" );
    } else {
        DefSwitchMacro( "FPI87" );
    }
}

static void macroDefs( void )
{
    if( TargetSwitches & I_MATH_INLINE ) {
        DefSwitchMacro( "OM" );
    }
    switch( TargetSwitches & (BIG_DATA|BIG_CODE|CHEAP_POINTER|FLAT_MODEL) ) {
    case CHEAP_POINTER:
        DefSwitchMacro( "MS" );
        break;
    case BIG_CODE|CHEAP_POINTER:
        DefSwitchMacro( "MM" );
        break;
    case BIG_DATA|CHEAP_POINTER:
        DefSwitchMacro( "MC" );
        break;
    case BIG_CODE|BIG_DATA|CHEAP_POINTER:
        DefSwitchMacro( "ML" );
        break;
    case BIG_CODE|BIG_DATA:
        DefSwitchMacro( "MH" );
        break;
    case FLAT_MODEL|CHEAP_POINTER:
        DefSwitchMacro( "MF" );
        break;
    }
    if( TargetSwitches & FLOATING_FS ) {
        DefSwitchMacro( "ZFF" );
    } else {
        DefSwitchMacro( "ZFP" );
    }
    if( TargetSwitches & FLOATING_GS ) {
        DefSwitchMacro( "ZGF" );
    } else {
        DefSwitchMacro( "ZGP" );
    }
    if( TargetSwitches & FLOATING_DS ) {
        DefSwitchMacro( "ZDF" );
    } else {
        DefSwitchMacro( "ZDP" );
    }
    if( TargetSwitches & FLOATING_SS ) {
        DefSwitchMacro( "ZU" );
    }
    if( TargetSwitches & INDEXED_GLOBALS ) {
        DefSwitchMacro( "XGV" );
    }
    if( TargetSwitches & WINDOWS ) {
        DefSwitchMacro( "ZW" );
    }
    if( TargetSwitches & SMART_WINDOWS ) {
        DefSwitchMacro( "ZWS" );
    }
    if( TargetSwitches & NO_CALL_RET_TRANSFORM ) {
        DefSwitchMacro( "OC" );
    }
    if( TargetSwitches & NEED_STACK_FRAME ) {
        DefSwitchMacro( "OF" );
    }
    if( GenSwitches & NO_OPTIMIZATION ) {
        DefSwitchMacro( "OD" );
    }
    if( GenSwitches & RELAX_ALIAS ) {
        DefSwitchMacro( "OA" );
    }
    if( GenSwitches & BRANCH_PREDICTION ) {
        DefSwitchMacro( "OB" );
    }
    if( GenSwitches & LOOP_OPTIMIZATION ) {
        DefSwitchMacro( "OL" );
    }
    if( GenSwitches & INS_SCHEDULING ) {
        DefSwitchMacro( "OR" );
    }
    if( GenSwitches & FPU_ROUNDING_INLINE ) {
        DefSwitchMacro( "ZRI" );
    }
    if( GenSwitches & FPU_ROUNDING_OMIT ) {
        DefSwitchMacro( "ZRO" );
    }
    if( TargetSwitches & GEN_FWAIT_386 ) {
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
    if( CompFlags.use_unicode ) {
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
    switch( GET_FPU_LEVEL( CpuSwitches ) ) {
    case FPU_NONE:
        CompFlags.op_switch_used = 0;
        DefSwitchMacro( "FPC" );
        break;
    case FPU_87:
        DefSwitchMacro( "FP287" );
        defEmu();
        break;
    case FPU_387:
        DefSwitchMacro( "FP387" );
        defEmu();
        break;
    case FPU_586:
        DefSwitchMacro( "FP5" );
        defEmu();
        break;
    case FPU_686:
        DefSwitchMacro( "FP6" );
        defEmu();
        break;
    }
    if( CompFlags.op_switch_used ) {
        DefSwitchMacro( "OP" );
    }
#if _CPU == 386
    PreDefineStringMacro( "_STDCALL_SUPPORTED" );
    PreDefineStringMacro( "_INTEGRAL_MAX_BITS=64" );
    if( CompFlags.extensions_enabled ) {
        PreDefineStringMacro( "SOMLINK=_Syscall" );
        PreDefineStringMacro( "SOMDLINK=_Syscall" );
    }
#endif
#if _CPU == 8086
    PreDefineStringMacro( "_INTEGRAL_MAX_BITS=64" );
    if( CompFlags.extensions_enabled ) {
        if( TargetSwitches & WINDOWS ) {
            PreDefineStringMacro( "SOMLINK=__cdecl" );
            PreDefineStringMacro( "SOMDLINK=__far" );
        }
    }
#endif
}

static void miscAnalysis( OPT_STORAGE *data )
{
#if _CPU == 8086
    if( data->bd || data->zu ) {
        if( TargetSwitches & SMART_WINDOWS ) {
            CErr1( ERR_ZWS_MUST_HAVE_SS_DS_SAME );
        }
    }
#endif
    if( GET_CPU( CpuSwitches ) < CPU_386 ) {
        /* issue warning message if /zf[f|p] or /zg[f|p] spec'd? */
        TargetSwitches &= ~( FLOATING_FS | FLOATING_GS );
    }
    if( ! CompFlags.save_restore_segregs ) {
        if( TargetSwitches & FLOATING_DS ) {
            HW_CTurnOff( WatcallInfo.save, HW_DS );
        }
        if( TargetSwitches & FLOATING_ES ) {
            HW_CTurnOff( WatcallInfo.save, HW_ES );
        }
        if( TargetSwitches & FLOATING_FS ) {
            HW_CTurnOff( WatcallInfo.save, HW_FS );
        }
        if( TargetSwitches & FLOATING_GS ) {
            HW_CTurnOff( WatcallInfo.save, HW_GS );
        }
    }
    if( GET_FPU( CpuSwitches ) > FPU_NONE ) {
        PreDefineStringMacro( "__FPI__" );
    }
#if _CPU == 386
    if( ! CompFlags.register_conventions ) {
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
    mem_model_control mmc = MMC_NULL;

    GenSwitches &= ~( DBG_CV | DBG_DF | DBG_PREDEF );
    switch( data->dbg_output ) {
    case OPT_dbg_output_hc:
        GenSwitches |= DBG_CV;
        break;
    case OPT_dbg_output_hda:
        if( data->fhd ) {
            CompFlags.pch_debug_info_opt = 1;
        }
        GenSwitches |= DBG_DF | DBG_PREDEF;
        break;
    case OPT_dbg_output_hw:
        break;
    case OPT_dbg_output_hd:
    default:
        if( data->fhd ) {
            CompFlags.pch_debug_info_opt = 1;
        }
        GenSwitches |= DBG_DF;
        break;
    }
    switch( data->ds_peg ) {
    case OPT_ds_peg_zdp:
        TargetSwitches &= ~ FLOATING_DS;
        mmc |= MMC_DS;
        break;
    case OPT_ds_peg_zdf:
        TargetSwitches |= FLOATING_DS;
        mmc |= MMC_DS;
        break;
    }

    if( data->zfw ) {
        TargetSwitches |= GEN_FWAIT_386;
    }
#if _CPU == 386
    if( data->zro && data->zri ) {
//        DbgDefault( "invalid fp rounding flags - ignored" );
        data->zro = data->zri = 0;
    }
    if( data->zri ) {
        GenSwitches |= FPU_ROUNDING_INLINE;
    } else if( data->zro ) {
        GenSwitches |= FPU_ROUNDING_OMIT;
    }
#else
    if( data->zro ) {
        GenSwitches |= FPU_ROUNDING_OMIT;
    }
#endif

#if _CPU == 386
    if( data->zdl ) {
        TargetSwitches |= LOAD_DS_DIRECTLY;
        mmc |= MMC_DS;
    }
#endif
    switch( data->fs_peg ) {
    case OPT_fs_peg_zfp:
        TargetSwitches &= ~ FLOATING_FS;
        mmc |= MMC_FS;
        break;
    case OPT_fs_peg_zff:
        TargetSwitches |= FLOATING_FS;
        mmc |= MMC_FS;
        break;
    }
    switch( data->gs_peg ) {
    case OPT_gs_peg_zgp:
        TargetSwitches &= ~ FLOATING_GS;
        mmc |= MMC_GS;
        break;
    case OPT_gs_peg_zgf:
        TargetSwitches |= FLOATING_GS;
        mmc |= MMC_GS;
        break;
    }
    // -zw overrides a build target setting
    if( data->bt ) {
        char *target = SetStringOption( NULL, &(data->bt_value) );
        SetTargetLiteral( &target_name, target );
        if( target_name != NULL ) {
            if( strcmp( target_name, "WINDOWS" ) == 0 ||
                  strcmp( target_name, "CHEAP_WINDOWS" ) == 0 ) {
                mmc |= MMC_WIN;
            } else if( strcmp( target_name, "NETWARE" ) == 0 ) {
                mmc |= MMC_NETWARE;
            } else if( strcmp( target_name, "NETWARE5" ) == 0 ) {
                mmc |= MMC_NETWARE;
            }
        }
        CMemFree( target );
    }
#if _CPU == 8086
    switch( data->win ) {
    case OPT_win_zw:
        SetTargetLiteral( &target_name, "windows" );
        mmc |= MMC_WIN;
        break;
    case OPT_win_zW:
        SetTargetLiteral( &target_name, "cheap_windows" );
        mmc |= MMC_WIN;
        break;
    case OPT_win_zws:
        SetTargetLiteral( &target_name, "windows" );
        TargetSwitches |= SMART_WINDOWS;
        mmc |= MMC_WIN;
        break;
    case OPT_win_zWs:
        SetTargetLiteral( &target_name, "cheap_windows" );
        TargetSwitches |= SMART_WINDOWS;
        mmc |= MMC_WIN;
        break;
    }
#else
    switch( data->win ) {
    case OPT_win_zw:
        SetTargetLiteral( &target_name, "windows" );
        mmc |= MMC_WIN;
        break;
    }
#endif
    // depends on 'mmc' being set fully
    setIntelArchitecture( data, mmc );

    switch( data->intel_fpu_model ) {
    case OPT_intel_fpu_model_fpi:
        SET_FPU_EMU( CpuSwitches );
        break;
#ifdef OPT_intel_fpu_model__7
    case OPT_intel_fpu__7:
        /* fall through */
#endif
    case OPT_intel_fpu_model_fpi87:
        SET_FPU_INLINE( CpuSwitches );
        break;
    case OPT_intel_fpu_model_fpc:
        SET_FPU( CpuSwitches, FPU_NONE );
        break;
    case OPT_intel_fpu_model__87d:
        if( data->_87d_value ) {
            Stack87 = data->_87d_value;
        }
        break;
    }

    switch( data->intel_fpu_level ) {
    case OPT_intel_fpu_level_fp6:
        SET_FPU_LEVEL( CpuSwitches, FPU_686 );
        break;
    case OPT_intel_fpu_level_fp5:
        SET_FPU_LEVEL( CpuSwitches, FPU_586 );
        break;
    case OPT_intel_fpu_level_fp3:
        SET_FPU_LEVEL( CpuSwitches, FPU_387 );
        break;
    case OPT_intel_fpu_level_fp2:
        SET_FPU_LEVEL( CpuSwitches, FPU_87 );
        break;
    }

    // depends on architecture and fpu being set
    setMemoryModel( data, mmc );
    if( data->fpd ) {
        TargetSwitches |= P5_DIVIDE_CHECK;
    }
    /*
    //  Changed the ordering.
    //      Always set Stack87 if fpr switch is used.
    //      If fpr switch is not present but are using NetWare then set Stack87
    //      unless the target is NetWare 5 or above.
    */
    if( data->fpr ) {
        Stack87 = 4;
    } else {
        if( mmc & MMC_NETWARE ) {
            if(TS_NETWARE5 != TargetSystem){
                Stack87 = 4;    /* no fpr for netware 5 */
            }
        }
    }
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
        TargetSwitches |= NO_CALL_RET_TRANSFORM;
    }
    if( data->of ) {
        TargetSwitches |= NEED_STACK_FRAME;
    }
    if( data->of_plus ) {
        TargetSwitches |= NEED_STACK_FRAME;
        WatcallInfo.cclass |= GENERATE_STACK_FRAME;
    }
    if( data->om ) {
        TargetSwitches |= I_MATH_INLINE;
    }
    if( data->r ) {
        CompFlags.save_restore_segregs = 1;
    }
    if( data->ri ) {
        CompFlags.returns_promoted = 1;
    }
    if( data->sg ) {
        CompFlags.sg_switch_used = 1;
    }
    if( data->st ) {
        CompFlags.st_switch_used = 1;
    }
    if( data->zc ) {
        CompFlags.strings_in_code_segment = 1;
        CompFlags.zc_switch_used = 1;
        TargetSwitches |= CONST_IN_CODE;
    }
    if( data->zm ) {
        CompFlags.zm_switch_used = 1;
    }
    if( data->zmf ) {
        CompFlags.zm_switch_used = 1;
        CompFlags.zmf_switch_used = 1;
    }
    if( data->zu ) {
        CompFlags.zu_switch_used = 1;
        TargetSwitches |= FLOATING_SS;
    }
    if( data->zx ) {
        CompFlags.zx_switch_used = 1;
    }
    if( data->zt ) {
        DataThreshold = data->zt_value;
    }
#if _CPU == 386
    if( data->vcap ) {
        CompFlags.vc_alloca_parm = 1;
    }
    if( data->br ) {
        CompFlags.br_switch_used = 1;
    }
    if( data->ez ) {
        TargetSwitches |= EZ_OMF;
    }
    if( data->etp ) {
        TargetSwitches |= NEW_P5_PROFILING;
    }
    if( data->esp ) {
        TargetSwitches |= STATEMENT_COUNTING;
    }
    if( data->et ) {
        TargetSwitches |= P5_PROFILING;
    }
    if( data->et0 ) {
        TargetSwitches |= P5_PROFILING | P5_PROFILING_CTR0;
    }
    if( data->xgv ) {
        TargetSwitches |= INDEXED_GLOBALS;
    }
    if( data->zo ) {
        CompFlags.zo_switch_used = 1;
    }
    if( data->zz ) {
        CompFlags.use_stdcall_at_number = 0;
    }
#else
    if( data->xgls ) {
        TargetSwitches |= NULL_SELECTOR_BAD;
    }
#endif
    if( data->iso == OPT_iso_za ) {
        TargetSwitches &= ~I_MATH_INLINE;
    }
    switch( data->intel_call_conv ) {
#if ( _CPU == 8086 ) || ( _CPU == 386 )
    case OPT_intel_call_conv_ecc:
        DftCallConv = &CdeclInfo;
        break;
    case OPT_intel_call_conv_ecd:
        DftCallConv = &StdcallInfo;
        break;
    case OPT_intel_call_conv_ecf:
        DftCallConv = &FastcallInfo;
        break;
    case OPT_intel_call_conv_eco:
        DftCallConv = &OptlinkInfo;
        break;
    case OPT_intel_call_conv_ecp:
        DftCallConv = &PascalInfo;
        break;
    case OPT_intel_call_conv_ecr:
        DftCallConv = &SyscallInfo;
        break;
    case OPT_intel_call_conv_ecs:
        DftCallConv = &FortranInfo;
        break;
    case OPT_intel_call_conv_ecw:
#endif
    case OPT_intel_call_conv_default:
    default:
        DftCallConv = &WatcallInfo;
        break;
    }
    // frees 'target_name' memory
    setFinalTargetSystem( data, target_name );
    miscAnalysis( data );
    defineFSRegistration();
    macroDefs();
}
