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
* Description:  Command line processing for Alpha AXP targets.
*
****************************************************************************/


#include "plusplus.h"
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

#define DEF_SWITCHES_ALL        ( MEMORY_LOW_FAILS )

#define DEF_TARGET_SWITCHES     0
#define DEF_SWITCHES            0


void CmdSysInit( void )
/*********************/
{
    GenSwitches = DEF_SWITCHES | DEF_SWITCHES_ALL;
    TargetSwitches = DEF_TARGET_SWITCHES;
    CodeClassName = NULL;
    TextSegName = strsave( "" );
    DataSegName = strsave( "" );
    GenCodeGroup = strsave( "" );
    CompFlags.make_enums_an_int = 1;
    CompFlags.original_enum_setting = 1;
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
    return( "WPPAXP" );
}

void CmdSysSetMaxOptimization( void )
/***********************************/
{
    TargetSwitches |= I_MATH_INLINE;
}

static void setFinalTargetSystem( OPT_STORAGE *data, char *target_name )
{
    char buff[128];

    TargetSystem = TS_OTHER;
    PreDefineStringMacro( "M_ALPHA" );
    PreDefineStringMacro( "_M_ALPHA" );
    PreDefineStringMacro( "__ALPHA__" );
    PreDefineStringMacro( "_ALPHA_" );
    PreDefineStringMacro( "__AXP__" );
    if( target_name == NULL ) {
        /* right now, the only targeted system is NT */
        SetTargetLiteral( &target_name, "NT" );
    }
    if( 0 == strcmp( target_name, "NT" ) ) {
        TargetSystem = TS_NT;
    }
    strcpy( buff, "__" );
    strcat( buff, target_name );
    strcat( buff, "__" );
    PreDefineStringMacro( buff );
    strcpy( buff, target_name );
    strcat( buff, "_INCLUDE" );
    MergeIncludeFromEnv( buff );
    MergeIncludeFromEnv( "INCLUDE" );
    CMemFree( target_name );
    if( data->bm || data->bd ) {
        CompFlags.target_multi_thread = 1;
    }
}

static void macroDefs( void )
{
    if( TargetSwitches & I_MATH_INLINE ) {
        DefSwitchMacro( "OM" );
    }
    if( TargetSwitches & NO_CALL_RET_TRANSFORM ) {
        DefSwitchMacro( "OC" );
    }
    if( TargetSwitches & ASM_OUTPUT ) {
        DefSwitchMacro( "LA" );
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
    if( CompFlags.unique_functions ) {
        DefSwitchMacro( "OU" );
    }
    if( CompFlags.emit_names ) {
        DefSwitchMacro( "EN" );
    }
    if( CompFlags.use_unicode ) {
        DefSwitchMacro( "ZK" );
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
    if( CompFlags.op_switch_used ) {
        DefSwitchMacro( "OP" );
    }
    if( CompFlags.zm_switch_used ) {
        DefSwitchMacro( "ZM" );
    }
    PreDefineStringMacro( "_STDCALL_SUPPORTED" );
    PreDefineStringMacro( "_INTEGRAL_MAX_BITS=64" );
}

static void setMemoryModel( OPT_STORAGE *data )
{
    DataPtrSize = TARGET_POINTER;
    CodePtrSize = TARGET_POINTER;
    if( data->br ) {
        strcpy( CLIB_Name, "1clbdll" );
    } else {
        strcpy( CLIB_Name, "1clib" );
    }
    if( data->br ) {
        if( CompFlags.excs_enabled ) {
            strcpy( WCPPLIB_Name, "4plbdllx" );
        } else {
            strcpy( WCPPLIB_Name, "4plbdll" );
        }
    } else {
        if( CompFlags.excs_enabled ) {
            if( data->bm ) {
                strcpy( WCPPLIB_Name, "4plbxmt" );
            } else {
                strcpy( WCPPLIB_Name, "4plbx" );
            }
        } else {
            if( data->bm ) {
                strcpy( WCPPLIB_Name, "4plibmt" );
            } else {
                strcpy( WCPPLIB_Name, "4plib" );
            }
        }
    }
    if( data->br ) {
        strcpy( MATHLIB_Name, "7mthdll" );
    } else {
        strcpy( MATHLIB_Name, "7math" );
    }
    strcpy( DLL_CLIB_Name, CLIB_Name );
    strcpy( DLL_WCPPLIB_Name, WCPPLIB_Name );
}

static void miscAnalysis( OPT_STORAGE *data )
{
    data = data;
}

void CmdSysAnalyse( OPT_STORAGE *data )
/*************************************/
{
    char *target_name = NULL;

    GenSwitches &= ~( DBG_CV | DBG_DF | DBG_PREDEF );
    switch( data->dbg_output ) {
    case OPT_dbg_output_hd:
    default:
        if( data->fhd ) {
            CompFlags.pch_debug_info_opt = 1;
        }
        GenSwitches |= DBG_DF;
        break;
    case OPT_dbg_output_hda:
        if( data->fhd ) {
            CompFlags.pch_debug_info_opt = 1;
        }
        GenSwitches |= DBG_DF | DBG_PREDEF;
        break;
#if 0
    case OPT_dbg_output_hw:
        GenSwitches &= ~( DBG_CV | DBG_DF | DBG_PREDEF );
        break;
#endif
    case OPT_dbg_output_hc:
        GenSwitches |= DBG_CV;
        break;
    }
    // -zw overrides a build target setting
    if( data->bt ) {
        char *target = SetStringOption( NULL, &(data->bt_value) );
        SetTargetLiteral( &target_name, target );
        CMemFree( target );
    }
    setMemoryModel( data );
    if( data->as ) {
        TargetSwitches |= ALIGNED_SHORT;
    }
    if( data->br ) {
        CompFlags.br_switch_used = 1;
    }
    if( data->la ) {
        TargetSwitches |= ASM_OUTPUT;
    }
    if( data->lo ) {
        TargetSwitches |= OWL_LOGGING;
    }
    if( data->oc ) {
        TargetSwitches |= NO_CALL_RET_TRANSFORM;
    }
    if( data->om ) {
        TargetSwitches |= I_MATH_INLINE;
    }
    if( data->nm ) {
        SetStringOption( &ModuleName, &(data->nm_value) );
    }
    if( data->si ) {
        TargetSwitches |= STACK_INIT;
    }
    if( data->iso == OPT_iso_za ) {
        TargetSwitches &= ~I_MATH_INLINE;
    }
    if( data->vcap ) {
        CompFlags.vc_alloca_parm = 1;
    }
    if( data->zm ) {
        CompFlags.zm_switch_used = 1;
    }

    // frees 'target_name' memory
    setFinalTargetSystem( data, target_name );
    miscAnalysis( data );
    macroDefs();
}
