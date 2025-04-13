/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "cvars.h"
#include "cmdlnprs.gh"
#include "cmdlnprs.h"
#include "cmdscan.h"
#include "toggles.h"
#include "cmdlnsys.h"
#include "cgdefs.h"
#include "feprotos.h"

#include "clibext.h"


#ifdef DEVBUILD
    #define DbgNever()  (FEMessage( FEMSG_FATAL, "should never execute this" __location ))
#else
    #define DbgNever()
#endif

void CmdSysInit( void )
{
    SetAuxWatcallInfo();

    GenSwitches = CGSW_GEN_MEMORY_LOW_FAILS;
    PackAmount = 8;
    GblPackAmount = PackAmount;
    TextSegName = "";
    DataSegName = ".data";
    GenCodeGroup = "";
    Inline_Threshold = 20;
    CompFlags.make_enums_an_int = true;     /* make enums ints */
    CompFlags.original_enum_setting = true;

#if _RISC_CPU || _CPU == _SPARC
#else
    #error InitCPUModInfo not configured for system
#endif
}

static char *setTargetSystem( OPT_STORAGE *data )
{
    char *target_name = NULL;

    if( data->bt ) {
        char *target = SetStringOption( NULL, &(data->bt_value) );
        SetTargetName( &target_name, strupr( target ) );
        CMemFree( target );
    }

    if( target_name == NULL ) {
#if _RISC_CPU
        /*
         * we only have NT libraries for Alpha right now
         */
        SetTargetName( &target_name, "NT" );
#elif _CPU == _SPARC
        SetTargetName( &target_name, "SOLARIS" );
#else
    #error Target Machine OS not configured
#endif
    }

    if( target_name != NULL ) {
        if( strcmp( target_name, "NT" ) == 0 ) {
            TargetSystem = TS_NT;
        } else if( strcmp( target_name, "LINUX" ) == 0 ) {
            TargetSystem = TS_LINUX;
#if _CPU == _PPC
        } else if( strcmp( target_name, "OS2" ) == 0 ) {
            TargetSystem = TS_OS2;
#endif
        } else if( strcmp( target_name, "HAIKU" ) == 0
                || strcmp( target_name, "OSX" ) == 0
                || strcmp( target_name, "SOLARIS" ) == 0
                || strcmp( target_name, "BSD" ) == 0 ) {
            TargetSystem = TS_UNIX;
        }
    }
    return( target_name );
}

static void SetFinalTargetSystem( OPT_STORAGE *data, char *target_name )
{
    char buff[128];

    /* unused parameters */ (void)data;

    if( CompFlags.non_iso_compliant_names_enabled ) {
#if _CPU == _AXP
        PreDefine_Macro( "M_ALPHA" );
#elif _CPU == _PPC
        PreDefine_Macro( "M_PPC" );
#elif _CPU == _MIPS
        PreDefine_Macro( "M_MRX000" );
#elif _CPU == _SPARC
        PreDefine_Macro( "M_SPARC" );
#endif
    }
#if _CPU == _AXP
    PreDefine_Macro( "_M_ALPHA" );
    PreDefine_Macro( "__ALPHA__" );
    PreDefine_Macro( "_ALPHA_" );
    PreDefine_Macro( "__AXP__" );
    PreDefine_Macro( "_STDCALL_SUPPORTED" );
#elif _CPU == _PPC
    PreDefine_Macro( "_M_PPC" );
    PreDefine_Macro( "__POWERPC__" );
    PreDefine_Macro( "__PPC__" );
    PreDefine_Macro( "_PPC_" );
#elif _CPU == _MIPS
    PreDefine_Macro( "_M_MRX000" );
    PreDefine_Macro( "__MIPS__" );
#elif _CPU == _SPARC
    PreDefine_Macro( "_M_SPARC" );
    PreDefine_Macro( "__SPARC__" );
    PreDefine_Macro( "_SPARC_" );
#else
    #error SetTargetSystem not configured
#endif

    PreDefine_Macro( "__WATCOM_INT64__" );
    PreDefine_Macro( "_INTEGRAL_MAX_BITS=64" );

    if( target_name != NULL ) {
        sprintf( buff, "__%s__", target_name );
        PreDefine_Macro( buff );
    }

    switch( TargetSystem ) {
    case TS_NT:
        PreDefine_Macro( "_WIN32" );
        break;
    case TS_LINUX:
    case TS_UNIX:
        PreDefine_Macro( "__UNIX__" );
        break;
    default:
        break;
    }

    if( target_name != NULL ) {
        strcpy( buff, target_name );
        strcat( buff, "_INCLUDE" );
        MergeIncludeFromEnv( buff );
    }
    MergeIncludeFromEnv( "INCLUDE" );

    if( (GenSwitches & (CGSW_GEN_OBJ_ELF | CGSW_GEN_OBJ_COFF)) == 0 ) {
        if( TargetSystem == TS_NT ) {
            GenSwitches |= CGSW_GEN_OBJ_COFF;
        } else {
            GenSwitches |= CGSW_GEN_OBJ_ELF;
        }
    }

    CMemFree( target_name );
}

static void SetDebugInfoFormat( OPT_STORAGE *data )
{
    GenSwitches &= ~(CGSW_GEN_DBG_CV | CGSW_GEN_DBG_DF | CGSW_GEN_DBG_PREDEF);
    switch( data->dbg_output ) {
    case OPT_ENUM_dbg_output_hc:
        GenSwitches |= CGSW_GEN_DBG_CV;
        break;
#if 0
    case OPT_ENUM_dbg_output_hw:
        break;
#endif
    case OPT_ENUM_dbg_output_hda:
        GenSwitches |= CGSW_GEN_DBG_DF | CGSW_GEN_DBG_PREDEF;
        SymDFAbbr = SpcSymbol( "__DFABBREV", GetType( TYP_USHORT ), SC_EXTERN );
        break;
    case OPT_ENUM_dbg_output_hdg:
        GenSwitches |= CGSW_GEN_DBG_DF | CGSW_GEN_DBG_PREDEF;
        SymDFAbbr = SpcSymbol( "__DFABBREV", GetType( TYP_USHORT ), SC_NONE );
        break;
    case OPT_ENUM_dbg_output_default:
    case OPT_ENUM_dbg_output_hd:
        GenSwitches |= CGSW_GEN_DBG_DF;
        break;
    default:
        DbgNever();
        break;
    }
}

static void SetGenSwitches( OPT_STORAGE *data )
{
#if _CPU == _AXP
    if( data->as ) {
        TargetSwitches |= CGSW_RISC_ALIGNED_SHORT;
    }
#endif
    if( data->br ) {
        CompFlags.br_switch_used = true;
    }
    if( data->el ) {
        GenSwitches &= ~CGSW_GEN_OBJ_ENDIAN_BIG;
    }
    if( data->eb ) {
        GenSwitches |= CGSW_GEN_OBJ_ENDIAN_BIG;
    }
    if( data->eoe ) {
        GenSwitches = (GenSwitches & ~CGSW_GEN_OBJ_COFF) | CGSW_GEN_OBJ_ELF;
    }
    if( data->eoc ) {
        GenSwitches = (GenSwitches & ~CGSW_GEN_OBJ_ELF) | CGSW_GEN_OBJ_COFF;
    }
#ifdef DEVBUILD
    if( data->la ) {
        TargetSwitches |= CGSW_RISC_ASM_OUTPUT;
    }
    if( data->lo ) {
        TargetSwitches |= CGSW_RISC_OWL_LOGGING;
    }
#endif
    if( data->nm ) {
        SetStringOption( &ModuleName, &(data->nm_value) );
    }
#if _CPU == _AXP /* || _CPU == _MIPS */
    if( data->si ) {
        TargetSwitches |= CGSW_RISC_STACK_INIT;
    }
#endif
#if _CPU == _AXP
    if( data->exc_level == OPT_ENUM_exc_level_xd ) {
        TargetSwitches |= CGSW_RISC_EXCEPT_FILTER_USED;
    }
#endif
    if( data->zm ) {
        CompFlags.multiple_code_segments = true;
        CompFlags.zm_switch_used = true;
    }
#if _CPU == _AXP
    if( data->zps ) {
        CompFlags.align_structs_on_qwords = true;
    }
#endif
}

static void MacroDefs( void )
{
    if( GenSwitches & CGSW_GEN_I_MATH_INLINE ) {
        DefSwitchMacro( "OM" );
    }
    if( GenSwitches & CGSW_GEN_OBJ_ENDIAN_BIG ) {
        Define_Macro( "__BIG_ENDIAN__" );
    }
    if( GenSwitches & CGSW_GEN_NO_CALL_RET_TRANSFORM ) {
        DefSwitchMacro( "OC" );
    }
    if( GenSwitches & CGSW_GEN_SUPER_OPTIMAL ) {
        DefSwitchMacro( "OH" );
    }
    if( GenSwitches & CGSW_GEN_FLOW_REG_SAVES ) {
        DefSwitchMacro( "OK" );
    }
    if( GenSwitches & CGSW_GEN_NO_OPTIMIZATION ) {
        DefSwitchMacro( "OD" );
    }
    if( GenSwitches & CGSW_GEN_RELAX_ALIAS ) {
        DefSwitchMacro( "OA" );
    }
    if( GenSwitches & CGSW_GEN_LOOP_OPTIMIZATION ) {
        DefSwitchMacro( "OL" );
    }
    if( GenSwitches & CGSW_GEN_INS_SCHEDULING ) {
        DefSwitchMacro( "OR" );
    }
    if( GenSwitches & CGSW_GEN_FP_UNSTABLE_OPTIMIZATION ) {
        DefSwitchMacro( "ON" );
    }
    if( GenSwitches & CGSW_GEN_FPU_ROUNDING_OMIT ) {
        DefSwitchMacro( "ZRO" );
    }
    if( GenSwitches & CGSW_GEN_FPU_ROUNDING_INLINE ) {
        DefSwitchMacro( "ZRI" );
    }
    if( CompFlags.use_long_double ) {
        DefSwitchMacro( "FLD" );
    }
    if( CompFlags.signed_char ) {
        DefSwitchMacro( "J" );
    }
    if( PCH_FileName != NULL ) {
        DefSwitchMacro( "FH" );
    }
    if( CompFlags.no_pch_warnings ) {
        DefSwitchMacro( "FHQ" );
    }
    if( CompFlags.inline_functions ) {
        DefSwitchMacro( "OI" );
    }
    if( CompFlags.unique_functions ) {
        DefSwitchMacro( "OU" );
    }
    if( CompFlags.emit_names ) {
        DefSwitchMacro( "EN" );
    }
    if( CompFlags.make_enums_an_int ) {
        DefSwitchMacro( "EI" );
    }
    if( CompFlags.zc_switch_used ) {
        DefSwitchMacro( "ZC" );
    }
    if( CompFlags.use_double_byte ) {
        DefSwitchMacro( "ZK" );
    }
    if( CompFlags.bm_switch_used ) {
        DefSwitchMacro( "BM" );
        Define_Macro( "_MT" );
    }
    if( CompFlags.bd_switch_used ) {
        DefSwitchMacro( "BD" );
    }
    if( CompFlags.bc_switch_used ) {
        DefSwitchMacro( "BC" );
    }
    if( CompFlags.bg_switch_used ) {
        DefSwitchMacro( "BG" );
    }
    if( CompFlags.br_switch_used ) {
        DefSwitchMacro( "BR" );
        Define_Macro( "_DLL" );
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
    if( CompFlags.ec_switch_used ) {
        DefSwitchMacro( "EC" );
    }
    if( !TOGGLE( check_stack ) ) {
        DefSwitchMacro( "S" );
    }
}

static void Define_Memory_Model( OPT_STORAGE *data )
{
    /* unused parameters */ (void)data;

    DataPtrSize = TARGET_POINTER;
    CodePtrSize = TARGET_POINTER;
#if _RISC_CPU || _CPU == _SPARC
    if( CompFlags.br_switch_used ) {
        strcpy( CLIB_Name, "1clbdll" );
        strcpy( MATHLIB_Name, "7mthdll" );
    } else {
        strcpy( CLIB_Name, "1clib" );
        strcpy( MATHLIB_Name, "7math" );
    }
    EmuLib_Name = NULL;
#else
    #error Define_Memory_Model not configured
#endif
}

void CmdSysAnalyse( OPT_STORAGE *data )
{
    char        *target_name;

    target_name = setTargetSystem( data );
    SetDebugInfoFormat( data );
    SetGenSwitches( data );
    Define_Memory_Model( data );
    SetFinalTargetSystem( data, target_name );
    MacroDefs();
}

void CmdSysSetMaxOptimization( void )
{
    GenSwitches |= CGSW_GEN_I_MATH_INLINE;
}
