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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*****************************************************************************
*                                                                            *
*                                                                            *
*                                                                            *
*       If you add an option, don't forget to change ../USAGE.SP             *
*       Also, don't forget to add a case in MacroDefs                        *
*       to predefine a __SW_xx macro                                         *
*                                                                            *
*                                                                            *
*                                                                            *
*****************************************************************************/
#include "cvars.h"
#include <ctype.h>
#include "pragdefn.h"
#include "pdefn2.h"
#include "cgswitch.h"
#include "iopath.h"
#include <unistd.h>
#include <fcntl.h>
#ifdef __OSI__
 #include "ostype.h"
#endif

struct  option {
    char        *option;
    unsigned    value;
    void        (*function)(void);
};

unsigned OptValue;
char    *OptScanPtr;
char    *OptParm;

#define __isdigit(c)    ((c) >= '0' && (c) <= '9')

#define PEGGED( r )     unsigned peg_##r##s_used        : 1;    \
                        unsigned peg_##r##s_on          : 1

static struct {
    char        *sys_name;
    enum {
        SW_CPU_DEF, SW_CPU0, SW_CPU1, SW_CPU2, SW_CPU3, SW_CPU4, SW_CPU5, SW_CPU6
    }           cpu;
    enum {
        SW_FPU_DEF, SW_FPU0, SW_FPU3, SW_FPU5, SW_FPU6
    }           fpu;
    enum {
        SW_FPT_DEF, SW_FPT_CALLS, SW_FPT_EMU, SW_FPT_INLINE
    }           fpt;
    enum {
        SW_M_DEF, SW_MF, SW_MS, SW_MM, SW_MC, SW_ML, SW_MH
    }           mem;
    enum {
        SW_DF_DEF, SW_DF_WATCOM, SW_DF_CV, SW_DF_DWARF, SW_DF_DWARF_A, SW_DF_DWARF_G
    }           dbg_fmt;
    PEGGED( d );
    PEGGED( e );
    PEGGED( f );
    PEGGED( g );
    unsigned    nd_used         : 1;
} SwData;

int EqualChar( int c )
{
    return( c == '#' || c == '=' );
}

local void SetTargName( char *name, unsigned len )
{
    char        *p;

    if( SwData.sys_name != NULL ) {
        CMemFree( SwData.sys_name );
        SwData.sys_name = NULL;
    }
    if( name == NULL || len == 0 ) return;
    SwData.sys_name = CMemAlloc( len + 1 ); /* for NULLCHAR */
    p = SwData.sys_name;
    while( len != 0 ) {
        *p++ = toupper( *name++ );
        --len;
    }
    *p++ = '\0';
}

#define _SetConstTarg( name ) SetTargName( name, sizeof( name ) - 1 )

local void SetTargSystem()                               /* 07-aug-90 */
{
    char        buff[128];

#if _MACHINE == _ALPHA
    PreDefine_Macro( "M_ALPHA" );
    PreDefine_Macro( "_M_ALPHA" );
    PreDefine_Macro( "__ALPHA__" );
    PreDefine_Macro( "_ALPHA_" );
    PreDefine_Macro( "__AXP__" );
    PreDefine_Macro( "_STDCALL_SUPPORTED" );
#elif _MACHINE == _SPARC
    PreDefine_Macro( "M_SPARC" );
    PreDefine_Macro( "_M_SPARC" );
    PreDefine_Macro( "__SPARC__" );
    PreDefine_Macro( "_SPARC_" );
#elif _MACHINE == _PPC
    PreDefine_Macro( "M_PPC" );
    PreDefine_Macro( "_M_PPC" );
    PreDefine_Macro( "__POWERPC__" );
    PreDefine_Macro( "__PPC__" );
    PreDefine_Macro( "_PPC_" );
#elif _CPU == 386
    PreDefine_Macro( "M_I386" );                    /* 03-jul-91 */
    PreDefine_Macro( "__386__" );
    PreDefine_Macro( "__X86__" );
    PreDefine_Macro( "_X86_" );
    PreDefine_Macro( "_STDCALL_SUPPORTED" );
#elif _CPU == 8086
    PreDefine_Macro( "M_I86" );
    PreDefine_Macro( "__I86__" );
    PreDefine_Macro( "__X86__" );
    PreDefine_Macro( "_X86_" );
#else
    #error SetTargSystem not configured
#endif
    PreDefine_Macro( "__WATCOM_INT64__" );
    PreDefine_Macro( "_INTEGRAL_MAX_BITS=64" );
    if( SwData.sys_name == NULL ) {
        #if _CPU == 386 || _CPU == 8086
            #if defined( __OSI__ )
                switch( __OS ) {                        // 11-mar-94
                case OS_DOS:
                case OS_WIN:
                    _SetConstTarg( "dos" );
                    break;
                case OS_OS2:
                    _SetConstTarg( "os2" );
                    break;
                case OS_NT:
                    _SetConstTarg( "nt" );
                    break;
                }
            #elif defined( __NOVELL__ )
                _SetConstTarg( "netware" );
            #elif _OS == _QNX
                _SetConstTarg( "qnx" );
            #elif _OS == _OS2
                _SetConstTarg( "os2" );
            #elif _OS == _NT
                _SetConstTarg( "nt" );
            #elif _OS == _DOS
                _SetConstTarg( "dos" );
            #else
                #error "Target OS not defined"
            #endif
        #elif _MACHINE == _ALPHA || _MACHINE == _PPC || _MACHINE == _SPARC
            /* we only have NT libraries for Alpha right now */
            _SetConstTarg( "nt" );
        #else
            #error Target Machine OS not configured
        #endif

    }
    if( strcmp( SwData.sys_name, "DOS" ) == 0 ) {
        TargSys = TS_DOS;
    } else if( strcmp( SwData.sys_name, "NETWARE" ) == 0 ) {
        TargSys = TS_NETWARE;
    } else if( strcmp( SwData.sys_name, "NETWARE5" ) == 0 ) {
        TargSys = TS_NETWARE5;
    } else if( strcmp( SwData.sys_name, "WINDOWS" ) == 0 ) {
        TargSys = TS_WINDOWS;
    } else if( strcmp( SwData.sys_name, "CHEAP_WINDOWS" ) == 0 ) {
        TargSys = TS_CHEAP_WINDOWS;
    } else if( strcmp( SwData.sys_name, "NT" ) == 0 ) {
        TargSys = TS_NT;
    } else if( strcmp( SwData.sys_name, "PENPOINT" ) == 0 ) {
        TargSys = TS_PENPOINT;
    } else {
        TargSys = TS_OTHER;
    }
    switch( TargSys ) {
    case TS_DOS:
        PreDefine_Macro( "MSDOS" );
        PreDefine_Macro( "_DOS" );
        break;
#if _CPU == 386
    case TS_NETWARE:
        Stack87 = 4;
        /* fall through */
    case TS_NETWARE5:
        /* no "fpr" for Netware 5.0 */
        if( SwData.mem == SW_M_DEF ) {
            SwData.mem = SW_MS;
        }
        if( TargSys == TS_NETWARE5 ) PreDefine_Macro( "__NETWARE__" );
        PreDefine_Macro( "__NETWARE_386__" );
        /* fall through */
    case TS_PENPOINT:
        /* PENPOINT & NETWARE used stack based calling conventions
           by default - silly people. */
        if( !CompFlags.register_conv_set ) {
            CompFlags.register_conventions = 0;
        }
        break;
    case TS_NT:
        PreDefine_Macro( "_WIN32" );
        break;
#endif
    case TS_CHEAP_WINDOWS:
        #if _CPU == 8086
            PreDefine_Macro( "__WINDOWS__" );
            PreDefine_Macro( "_WINDOWS" );
        #else
            TargSys = TS_WINDOWS;
        #endif
        /* fall through */
    case TS_WINDOWS:
        #if _CPU == 386
            PreDefine_Macro( "__WINDOWS_386__" );
            if( !SwData.peg_fs_used ) {
                SwData.peg_fs_on = 0;
                SwData.peg_fs_used = 1;
            }
            switch( SwData.fpt ) {
            case SW_FPT_DEF:
            case SW_FPT_EMU:
                SwData.fpt = SW_FPT_INLINE;
                break;
            }
            TargetSwitches |= WINDOWS | CHEAP_WINDOWS;
        #elif _CPU == 8086
            if( !SwData.peg_ds_used ) {
                SwData.peg_ds_on = 1;
                SwData.peg_ds_used = 1;
            }
            TargetSwitches |= WINDOWS | CHEAP_WINDOWS;
        #endif
        break;
    }
    strcpy( buff, "__" );
    strcat( buff, SwData.sys_name );
    strcat( buff, "__" );
    PreDefine_Macro( buff );
}


#define SET_PEG( r ) if( !SwData.peg_##r##s_used ) SwData.peg_##r##s_on = 1;

static void SetGenSwitches()
{
#if _CPU == 8086 || _CPU == 386
    #if _CPU == 386
        if( SwData.cpu == SW_CPU_DEF ) SwData.cpu = SW_CPU5;
        if( SwData.fpu == SW_FPU_DEF ) SwData.fpu = SW_FPU3;
        if( SwData.mem == SW_M_DEF   ) SwData.mem = SW_MF;
        TargetSwitches |= USE_32;
    #else
        if( SwData.cpu == SW_CPU_DEF ) SwData.cpu = SW_CPU0;
        if( SwData.fpu == SW_FPU_DEF ) SwData.fpu = SW_FPU0;
        if( SwData.mem == SW_M_DEF   ) SwData.mem = SW_MS;
        SET_PEG( f );
        SET_PEG( g );
    #endif
    switch( SwData.fpu ) {
    case SW_FPU0:
        SET_FPU_LEVEL( ProcRevision, FPU_87 );
        break;
    case SW_FPU3:
        SET_FPU_LEVEL( ProcRevision, FPU_387 );
        break;
    case SW_FPU5:
        SET_FPU_LEVEL( ProcRevision, FPU_586 );
        break;
    case SW_FPU6:
        SET_FPU_LEVEL( ProcRevision, FPU_686 );
        break;
    }
    switch( SwData.fpt ) {
    case SW_FPT_DEF:
    case SW_FPT_EMU:
        SwData.fpt = SW_FPT_EMU;
        SET_FPU_EMU( ProcRevision );
        break;
    case SW_FPT_INLINE:
        SET_FPU_INLINE( ProcRevision );
        break;
    case SW_FPT_CALLS:
        SET_FPU( ProcRevision, FPU_NONE );
        break;
    }
    SET_CPU( ProcRevision, SwData.cpu - SW_CPU0 + CPU_86 );
    switch( SwData.mem ) {
    case SW_MF:
        TargetSwitches |= FLAT_MODEL | CHEAP_POINTER;
        SET_PEG( d );
        SET_PEG( e );
        SET_PEG( f );
    case SW_MS:
        TargetSwitches |= CHEAP_POINTER;
        SET_PEG( d );
        break;
    case SW_MM:
        TargetSwitches |= BIG_CODE | CHEAP_POINTER;
        SET_PEG( d );
        break;
    case SW_MC:
        TargetSwitches |= BIG_DATA | CHEAP_POINTER;
        break;
    case SW_ML:
        TargetSwitches |= BIG_CODE | BIG_DATA | CHEAP_POINTER;
        break;
    case SW_MH:
#if _CPU == 386
        TargetSwitches |= CHEAP_POINTER;
#endif
        TargetSwitches |= BIG_CODE | BIG_DATA;
        break;
    }
    if( !SwData.peg_ds_on ) TargetSwitches |= FLOATING_DS;
    if( !SwData.peg_es_on ) TargetSwitches |= FLOATING_ES;
    if( !SwData.peg_fs_on ) TargetSwitches |= FLOATING_FS;
    if( !SwData.peg_gs_on ) TargetSwitches |= FLOATING_GS;
#endif
    switch( SwData.dbg_fmt ) {
    case SW_DF_WATCOM:
        /* nothing to do */
        break;
    case SW_DF_CV:
        GenSwitches |= DBG_CV;
        break;
    case SW_DF_DEF:
        /* DWARF is the default */
    case SW_DF_DWARF:
        GenSwitches |= DBG_DF;
        break;
    case SW_DF_DWARF_A:
        GenSwitches |= DBG_DF | DBG_PREDEF;
        SymDFAbbr = SpcSymbol( "__DFABBREV", SC_EXTERN  );
        break;
    case SW_DF_DWARF_G:
        GenSwitches |= DBG_DF | DBG_PREDEF;
        SymDFAbbr = SpcSymbol( "__DFABBREV", SC_NULL  );
        break;
    }
}

static void MacroDefs()
{
    if( TargetSwitches & I_MATH_INLINE ) {
        Define_Macro( "__SW_OM" );
    }
#if _CPU == 8086 || _CPU == 386
 #if _CPU == 8086
  #define X86 "_M_I86"
 #else
  #define X86 "_M_386"
 #endif
    switch( SwData.mem ) {
    case SW_MS:
        Define_Macro( "__SW_MS" );
        Define_Macro( X86 "SM" );
        break;
    case SW_MM:
        Define_Macro( "__SW_MM" );
        Define_Macro( X86 "MM" );
        break;
    case SW_MC:
        Define_Macro( "__SW_MC" );
        Define_Macro( X86 "CM" );
        break;
    case SW_ML:
        Define_Macro( "__SW_ML" );
        Define_Macro( X86 "LM" );
        break;
    case SW_MH:
        Define_Macro( "__SW_MH" );
        Define_Macro( X86 "HM" );
        break;
    case SW_MF:
        Define_Macro( "__SW_MF" );
        break;
    }
    if( TargetSwitches & FLOATING_FS ) {
        Define_Macro( "__SW_ZFF" );
    } else {
        Define_Macro( "__SW_ZFP" );
    }
    if( TargetSwitches & FLOATING_GS ) {
        Define_Macro( "__SW_ZGF" );
    } else {
        Define_Macro( "__SW_ZGP" );
    }
    if( TargetSwitches & FLOATING_DS ) {
        Define_Macro( "__SW_ZDF" );
    } else {
        Define_Macro( "__SW_ZDP" );
    }
    if( TargetSwitches & FLOATING_SS ) {
        Define_Macro( "__SW_ZU" );
    }
    if( TargetSwitches & INDEXED_GLOBALS ) {
        Define_Macro( "__SW_XGV" );
    }
    if( TargetSwitches & WINDOWS ) {
        Define_Macro( "__SW_ZW" );
    }
    if( TargetSwitches & NO_CALL_RET_TRANSFORM ) {
        Define_Macro( "__SW_OC" );
    }
    if( TargetSwitches & NEED_STACK_FRAME ) {
        Define_Macro( "__SW_OF" );
    }
#endif
    if( GenSwitches & SUPER_OPTIMAL ) {
        Define_Macro( "__SW_OH" );
    }
    if( GenSwitches & FLOW_REG_SAVES ) {
        Define_Macro( "__SW_OK" );
    }
    if( GenSwitches & NO_OPTIMIZATION ) {
        Define_Macro( "__SW_OD" );
    }
    if( GenSwitches & RELAX_ALIAS ) {
        Define_Macro( "__SW_OA" );
    }
    if( GenSwitches & LOOP_OPTIMIZATION ) {
        Define_Macro( "__SW_OL" );
    }
    if( GenSwitches & INS_SCHEDULING ) {
        Define_Macro( "__SW_OR" );
    }
    if( GenSwitches & FP_UNSTABLE_OPTIMIZATION ) {
        Define_Macro( "__SW_ON" );
    }
    if( CompFlags.signed_char ) {
        Define_Macro( "__SW_J" );
    }
    if( PCH_FileName != NULL ) {
        Define_Macro( "__SW_FH" );
    }
    if( CompFlags.no_pch_warnings ) {
        Define_Macro( "__SW_FHQ" );
    }
    if( CompFlags.inline_functions ) {
        Define_Macro( "__SW_OI" );
    }
    if( CompFlags.unique_functions ) {
        Define_Macro( "__SW_OU" );
    }
    #if _CPU == 386
        if( CompFlags.register_conventions ) {
            Define_Macro( "__SW_3R" );
        } else {
            Define_Macro( "__SW_3S" );
        }
    #endif
    if( CompFlags.emit_names ) {
        Define_Macro( "__SW_EN" );
    }
    if( CompFlags.make_enums_an_int ) {
        Define_Macro( "__SW_EI" );
    }
    if( CompFlags.zc_switch_used ) {
        Define_Macro( "__SW_ZC" );
    }
    if( CompFlags.use_unicode == 0 ) {
        Define_Macro( "__SW_ZK" );
    }
#if _CPU == 8086 || _CPU == 386
    if( CompFlags.save_restore_segregs ) {
        Define_Macro( "__SW_R" );
    }
    if( CompFlags.sg_switch_used ) {
        Define_Macro( "__SW_SG" );
    }
    if( CompFlags.st_switch_used ) {
        Define_Macro( "__SW_ST" );
    }
    if( CompFlags.zu_switch_used ) {
        Define_Macro( "__SW_ZU" );
    }
#endif
    if( CompFlags.bm_switch_used ) {
        Define_Macro( "__SW_BM" );
        Define_Macro( "_MT" );
    }
    if( CompFlags.bd_switch_used ) {
        Define_Macro( "__SW_BD" );
    }
    if( CompFlags.bc_switch_used ) {
        Define_Macro( "__SW_BC" );
    }
    if( CompFlags.bg_switch_used ) {
        Define_Macro( "__SW_BG" );
    }
    if( CompFlags.br_switch_used ) {
        Define_Macro( "__SW_BR" );
        Define_Macro( "_DLL" );
    }
    if( CompFlags.bw_switch_used ) {
        Define_Macro( "__SW_BW" );
    }
    if( CompFlags.zm_switch_used ) {
        Define_Macro( "__SW_ZM" );
    }
    if( CompFlags.ep_switch_used ) {
        Define_Macro( "__SW_EP" );
    }
    if( CompFlags.ee_switch_used ) {
        Define_Macro( "__SW_EE" );
    }
    if( CompFlags.ec_switch_used ) {
        Define_Macro( "__SW_EC" );
    }
#if _CPU == 8086 || _CPU == 386
    switch( GET_CPU( ProcRevision ) ) {
    case CPU_86:
        Define_Macro( "__SW_0" );
        PreDefine_Macro( "_M_IX86=0" );
        break;
    case CPU_186:
        Define_Macro( "__SW_1" );
        PreDefine_Macro( "_M_IX86=100" );
        break;
    case CPU_286:
        Define_Macro( "__SW_2" );
        PreDefine_Macro( "_M_IX86=200" );
        break;
    case CPU_386:
        Define_Macro( "__SW_3" );
        PreDefine_Macro( "_M_IX86=300" );
        break;
    case CPU_486:
        Define_Macro( "__SW_4" );
        PreDefine_Macro( "_M_IX86=400" );
        break;
    case CPU_586:
        Define_Macro( "__SW_5" );
        PreDefine_Macro( "_M_IX86=500" );
        break;
    case CPU_686:
        Define_Macro( "__SW_6" );
        PreDefine_Macro( "_M_IX86=600" );
        break;
    }
    switch( SwData.fpt ) {
    case SW_FPT_CALLS:
        CompFlags.op_switch_used = 0;           /* 05-sep-92 */
        Define_Macro( "__SW_FPC" );
        break;
    case SW_FPT_EMU:
        Define_Macro( "__SW_FPI" );
        Define_Macro( "__FPI__" );
        break;
    case SW_FPT_INLINE:
        Define_Macro( "__SW_FPI87" );
        Define_Macro( "__FPI__" );
        break;
    }
    switch( GET_FPU_LEVEL( ProcRevision ) ) {
    case FPU_NONE:
        break;
    case FPU_87:
        Define_Macro( "__SW_FP2" );
        break;
    case FPU_387:
        Define_Macro( "__SW_FP3" );
        break;
    case FPU_586:
        Define_Macro( "__SW_FP5" );
        break;
    case FPU_686:
        Define_Macro( "__SW_FP6" );
        break;
    }
    if( SwData.nd_used ) {
        Define_Macro( "__SW_ND" );
    }
    if( CompFlags.op_switch_used ) {            /* 05-sep-92 */
        Define_Macro( "__SW_OP" );
    }
#endif
    if( !(Toggles & TOGGLE_CHECK_STACK) ) {
        Define_Macro( "__SW_S" );
    }
}

static void AddIncList( char *str )
{
    int         len2;
    int         len;
    char        *p;

    len = strlen( str );
    if( HFileList != NULL ) {
        len2 = strlen( HFileList );
        p = (char *) CMemAlloc( len + len2 + 2 );
        memcpy( p, HFileList, len2 );
        p[ len2 ] = INCLUDE_SEP;
        memcpy( p + len2 + 1, str, len );
        p[ len + len2 + 1 ] = '\0';
        CMemFree( HFileList );
        HFileList = p;
    } else {
        p = (char *) CMemAlloc( (len + 1) * sizeof( char ) );
        memcpy( p, str, len );
        p[ len ] = '\0';
        HFileList = p;
    }
}

#define INC_VAR "INCLUDE"

void MergeInclude()
{
    /* must be called after GenCOptions to get req'd HFileList */
    char        *env_var;
    char        buff[128];

    switch( TargSys ) {
    case TS_CHEAP_WINDOWS:
    case TS_WINDOWS:
        strcpy( buff, "WINDOWS" );
        break;
    case TS_NETWARE:
    case TS_NETWARE5:
        strcpy( buff, "NETWARE" );
        break;
    default:
        strcpy( buff, SwData.sys_name );
        break;
    }
    strcat( buff, "_" INC_VAR );
    env_var = FEGetEnv( buff );
    if( env_var != NULL ) {
        while( *env_var == ' ' )  ++env_var;        /* 23-jun-93 */
        AddIncList( env_var );
    }

    #if _CPU == 386
        env_var = FEGetEnv( "INC386" );               /* 03-may-89 */
        if( env_var == NULL ) {                 /* 12-mar-90 */
            env_var = FEGetEnv( INC_VAR );
        }
    #else
        env_var = FEGetEnv( INC_VAR );
    #endif
    if( env_var != NULL ) {
        while( *env_var == ' ' )  ++env_var;        /* 23-jun-93 */
        AddIncList( env_var );
    }
    CMemFree( SwData.sys_name );
}


static int OptionDelimiter( char c )
{
    if( c == ' ' || c == '-' || c == '\0' || c == '\t' || c == SwitchChar ) {
        return( 1 );
    }
    return( 0 );
}

void EnsureEndOfSwitch()
{
    char        c;

    if( !OptionDelimiter( *OptScanPtr ) ) {
        for(;;) {                       // find start of switch
            c = *OptScanPtr;
            if( c == '-' || c == SwitchChar ) break;
            --OptScanPtr;
        }
        OptScanPtr = BadCmdLine( ERR_INVALID_OPTION, OptScanPtr );
    }
}

#if _MACHINE == _ALPHA
void SetStructPack()    { CompFlags.align_structs_on_qwords = 1; }
#endif

void SetPackAmount()    { PackAmount = OptValue; }
void Set_DbgFmt()       { SwData.dbg_fmt = OptValue; }

#if _CPU == 8086 || _CPU == 386
void SetCPU()           { SwData.cpu = OptValue; }
void SetCPU_xR(){ SwData.cpu = OptValue; CompFlags.register_conventions = 1; }
void SetCPU_xS(){ SwData.cpu = OptValue; CompFlags.register_conventions = 0; }
void SetFPU()           { SwData.fpu = OptValue; }
void Set_FPR()          { Stack87 = 4; }
void Set_FPI87()        { SwData.fpt = SW_FPT_INLINE; }
void Set_Emu()          { SwData.fpt = SW_FPT_EMU; }
void Set_FPC()          { SwData.fpt = SW_FPT_CALLS; }
void Set_FPD()          { TargetSwitches |= P5_DIVIDE_CHECK; }

void SetMemoryModel()   { SwData.mem = OptValue; }
#endif

void Set_BD()           { CompFlags.bd_switch_used = 1; GenSwitches |= DLL_RESIDENT_CODE; }
void Set_BC()           { CompFlags.bc_switch_used = 1; }
void Set_BG()           { CompFlags.bg_switch_used = 1; }
void Set_BM()           { CompFlags.bm_switch_used = 1; }
void Set_BR()           { CompFlags.br_switch_used = 1; }
void Set_BW()           { CompFlags.bw_switch_used = 1; }
void Set_BT()           { SetTargName( OptParm,  OptScanPtr - OptParm ); }

void SetExtendedDefines()
{
    CompFlags.extended_defines = 1;
    EnsureEndOfSwitch();
}
void SetBrowserInfo()   { CompFlags.emit_browser_info = 1; }

#if _MACHINE == _ALPHA
void Set_AS()
{
    TargetSwitches |= ALIGNED_SHORTS;
}
#endif

void Set_AA()           { CompFlags.auto_agg_inits = 1; }
void Set_AI()           { CompFlags.no_check_inits = 1; }
void Set_AQ()           { CompFlags.no_check_qualifiers = 1; }
void Set_D0()
{
    GenSwitches &= ~(NUMBERS | DBG_TYPES | DBG_LOCALS | NO_OPTIMIZATION);
    CompFlags.debug_info_some = 0;
    CompFlags.no_debug_type_names = 0;
    EnsureEndOfSwitch();
}
void Set_D1()
{
    GenSwitches |= NUMBERS;
    if( *OptScanPtr == '+' ) {
        ++OptScanPtr;
        CompFlags.debug_info_some = 1;
        GenSwitches |= DBG_TYPES | DBG_LOCALS;
    }
    EnsureEndOfSwitch();
}
void Set_D2()
{
    GenSwitches |= NUMBERS | DBG_TYPES | DBG_LOCALS | NO_OPTIMIZATION;
    CompFlags.inline_functions = 0;
    if( *OptScanPtr == '~' ) {
        ++OptScanPtr;
        CompFlags.no_debug_type_names = 1;
    }
    EnsureEndOfSwitch();
}
void Set_D3()
{
    CompFlags.dump_types_with_names = 1;
    Set_D2();
}
void Set_D9()           { CompFlags.use_full_codegen_od = 1; }
void DefineMacro()      { OptScanPtr = Define_UserMacro( OptScanPtr ); }

void SetErrorLimit()    { ErrLimit = OptValue; }
void Set_EC()           { CompFlags.ec_switch_used = 1; }
void Set_EE()           { CompFlags.ee_switch_used = 1; }
void Set_EF()           { CompFlags.ef_switch_used = 1; }
void Set_EN()           { CompFlags.emit_names = 1; }
void Set_EI()           { CompFlags.make_enums_an_int = 1;
                          CompFlags.original_enum_setting = 1;}
void Set_EM()           { CompFlags.make_enums_an_int = 0;
                          CompFlags.original_enum_setting = 0;}

#if _CPU == 8086 || _CPU == 386
void Set_ET()           { TargetSwitches |= P5_PROFILING; }
void Set_ETP()          { TargetSwitches |= NEW_P5_PROFILING; }
void Set_ESP()          { TargetSwitches |= STATEMENT_COUNTING; }
void Set_EZ()           { TargetSwitches |= EZ_OMF; }
#endif

void Set_EP()
{
    CompFlags.ep_switch_used = 1;
    ProEpiDataSize = OptValue;
}

void SetNoCurrInc( void ) { CompFlags.curdir_inc = 0;}

static void StripQuotes( char *fname )
{
    char *s;
    char *d;

    if( *fname == '"' ) {
        // string will shrink so we can reduce in place
        d = fname;
        for( s = d + 1; *s && *s != '"'; ++s ) {
            if( *s == '\0' )break;
            if( s[0] == '\\' && s[1] == '"' ) {
                ++s;
            }
            *d++ = *s;
        }
        *d = '\0';
    }
}

char *CopyOfParm()
{
    char        *p;
    unsigned    len;

    len = OptScanPtr - OptParm;
    p = (char *) CMemAlloc( len + 1 );
    memcpy( p, OptParm, len );
    p[ len ] = '\0';
    return( p );
}

char *GetAFileName()
{
    char *fname;
    fname = CopyOfParm();
    StripQuotes( fname );
    return( fname );
}

void Set_FH()
{
    if( OptParm == OptScanPtr ) {
        PCH_FileName = DEFAULT_PCH_NAME;
    } else {
        PCH_FileName = GetAFileName();
    }
}
void Set_FHQ()
{
    CompFlags.no_pch_warnings = 1;
    Set_FH();
}
void Set_FI()
{
    ForceInclude = GetAFileName();
}
void Set_FO()
{
    CMemFree( ObjectFileName );
    ObjectFileName = GetAFileName();
    CompFlags.cpp_output_to_file = 1;   /* in case '-p' option */
}

void Set_FR()
{
    CMemFree( ErrorFileName );
    ErrorFileName = GetAFileName();
    if( *ErrorFileName == '\0' ){
        CMemFree( ErrorFileName );
        ErrorFileName = NULL;
    }
}

#if _CPU == 8086 || _CPU == 386
void SetCodeClass()     { CodeClassName = CopyOfParm(); }
void SetDataSegName()
{
    SwData.nd_used = 1;
    DataSegName = CopyOfParm();
    SegData = -1;
    if( *DataSegName == '\0' ) {
        CMemFree( DataSegName );
        DataSegName = NULL;
    }
}
void SetTextSegName()   { TextSegName = CopyOfParm(); }
void SetGroup()         { GenCodeGroup = CopyOfParm(); }
#endif
void SetModuleName()    { ModuleName = CopyOfParm(); }

void SetAPILogging()    { GenSwitches |= ECHO_API_CALLS; }

#ifndef NDEBUG
#ifdef ASM_OUTPUT
void SetAsmListing()    { TargetSwitches |= ASM_OUTPUT; }
#endif
#ifdef OWL_LOGGING
void SetOwlLogging()    { TargetSwitches |= OWL_LOGGING; }
#endif
#endif

void SetInclude(){
    char *fname;

    fname = GetAFileName();
    AddIncList( fname );
    CMemFree( fname );
}

void SetReadOnlyDir(){
    char *dirpath;

    dirpath = GetAFileName();
    SrcFileReadOnlyDir( dirpath );
    CMemFree( dirpath );
}

void SetCharType()
{
    SetSignedChar();
    CompFlags.signed_char = 1;
}

#if _CPU == 8086 || _CPU == 386
void Set_RE()           { CompFlags.rent = 1; }
void Set_RI()           { CompFlags.returns_promoted = 1; }
void Set_R()            { CompFlags.save_restore_segregs = 1; }
void Set_SG()           { CompFlags.sg_switch_used = 1; }
void Set_ST()           { CompFlags.st_switch_used = 1; }
#endif
#if _MACHINE == _ALPHA
void Set_SI()           { TargetSwitches |= STACK_INIT; }
#endif
void Set_S()            { Toggles &= ~TOGGLE_CHECK_STACK; }

void Set_TP(){
    char *togname;
    togname = CopyOfParm();
    SetToggleFlag( togname, TRUE );
    CMemFree( togname );
}

void SetDataThreshHold(){ DataThreshold = OptValue; }

void UndefMacroName()   { OptScanPtr = AddUndefName( OptScanPtr ); }

void Set_U(){
    char *name;
    name = CopyOfParm();
    AddUndefName( name );
    CMemFree( name );
}
void Set_V()
{
    CompFlags.dump_prototypes     = 1;
    CompFlags.generate_prototypes = 1;
}

void Set_WE()           { CompFlags.warnings_cause_bad_exit = 1; }
void Set_WO()           { CompFlags.using_overlays = 1; }
void Set_WX()           { WngLevel = 4; }
void SetWarningLevel()  { WngLevel = OptValue; }
void Set_WCD()          { EnableDisableMessage( 0, OptValue ); }
void Set_WCE()          { EnableDisableMessage( 1, OptValue ); }

#if _CPU == 386
void Set_XGV()          { GenSwitches |= INDEXED_GLOBALS; }
#endif

void Set_XBSA()
{
    CompFlags.unaligned_segs = 1;
}

#if _MACHINE == _ALPHA
void Set_XD()           { TargetSwitches |= EXCEPT_FILTER_USED; }
#endif

void Set_ZA()
{
    CompFlags.extensions_enabled = 0;
    CompFlags.unique_functions = 1;
    TargetSwitches &= ~I_MATH_INLINE;
}
void SetStrictANSI()
{
    CompFlags.strict_ANSI = 1;
    Set_ZA();
}

#if _CPU == 8086 || _CPU == 386
void Set_ZC()
{
    CompFlags.strings_in_code_segment = 1;
    CompFlags.zc_switch_used = 1;       /* 04-jun-91 */
    TargetSwitches |= CONST_IN_CODE;
}
void Set_ZDF()          { SwData.peg_ds_used = 1; SwData.peg_ds_on = 0; }
void Set_ZDP()          { SwData.peg_ds_used = 1; SwData.peg_ds_on = 1; }
void Set_ZDL()          { TargetSwitches |= LOAD_DS_DIRECTLY; }
void Set_ZFF()          { SwData.peg_fs_used = 1; SwData.peg_fs_on = 0; }
void Set_ZFP()          { SwData.peg_fs_used = 1; SwData.peg_fs_on = 1; }
void Set_ZGF()          { SwData.peg_gs_used = 1; SwData.peg_gs_on = 0; }
void Set_ZGP()          { SwData.peg_gs_used = 1; SwData.peg_gs_on = 1; }
#endif
void Set_ZE()           { CompFlags.extensions_enabled = 1; }
void Set_ZG()           { CompFlags.generate_prototypes = 1; }

void Set_ZI()           { CompFlags.extra_stats_wanted = 1; }
void Set_ZKU()
{
    CompFlags.use_unicode = 1;
    OptScanPtr = LoadUnicodeTable( OptScanPtr );
}
void Set_ZK0()
{
    CompFlags.use_unicode = 0;          /* 05-jun-91 */
    SetDBChar( 0 );                     /* set double-byte char type */
}
void Set_ZK0U()
{
    CompFlags.use_unicode = 0;          /* 05-jun-91 */
    SetDBChar( 0 );                     /* set double-byte char type */
    CompFlags.jis_to_unicode = 1;
}
void Set_ZK1()
{
    CompFlags.use_unicode = 0;          /* 05-jun-91 */
    SetDBChar( 1 );                     /* set double-byte char type */
}
void Set_ZK2()
{
    CompFlags.use_unicode = 0;          /* 05-jun-91 */
    SetDBChar( 2 );                     /* set double-byte char type */
}
void Set_ZK3()
{
    CompFlags.use_unicode = 0;          /* 24-mar-00 */
    SetDBChar( 3 );                     /* set double-byte char type */
}
void Set_ZKL()
{
    CompFlags.use_unicode = 0;          /* 05-jun-91 */
    SetDBChar( -1 );                   /* set double-byte char type to defualt */
}
void Set_ZL()                   { CompFlags.emit_library_with_main = 0; }
void Set_ZLF()                  { CompFlags.emit_library_any  = 1; }
void Set_ZLD()                  { CompFlags.emit_dependencies = 0; }
void Set_ZV()                   { CompFlags.unix_ext = 1; }
void Set_ZM()
{
    CompFlags.multiple_code_segments = 1;
    CompFlags.zm_switch_used = 1;
}
void Set_ZPW()                { CompFlags.slack_byte_warning = 1; }
void Set_ZQ()                 { CompFlags.quiet_mode = 1; }
void Set_EQ()                 { CompFlags.no_conmsg  = 1; }
void Set_ZS()                   { CompFlags.check_syntax = 1; }
#if _CPU == 8086 || _CPU == 386
void Set_ZU()
{
    CompFlags.zu_switch_used = 1;
    TargetSwitches |= FLOATING_SS;
}
void Set_ZZ()
{
    CompFlags.use_stdcall_at_number = 0;
}
#if _CPU == 8086
void ChkSmartWindows()
{
    if( tolower(*OptScanPtr) == 's' ) {        /* 22-mar-94 */
        TargetSwitches |= SMART_WINDOWS;
        ++OptScanPtr;
    }
    EnsureEndOfSwitch();
}
void SetCheapWindows()
{
    _SetConstTarg( "cheap_windows" );
    ChkSmartWindows();
}
#endif
void SetWindows()
{
    _SetConstTarg( "windows" );
#if _CPU == 8086
    ChkSmartWindows();
#endif
}
#endif

void Set_PL()           { CompFlags.cpp_line_wanted = 1; }
void Set_PC()
{
    CompFlags.keep_comments = 1;
    CompFlags.comments_wanted = 1;
}
void Set_PW()
{
    if( OptValue != 0 && OptValue < 20 ) OptValue = 20;
    if( OptValue > 10000 ) OptValue = 10000;
    SetCppWidth( OptValue );
}
void Set_PreProcChar()  { PreProcChar = *OptScanPtr++; }

void Set_OA() { GenSwitches |= RELAX_ALIAS; }
void Set_OB() { GenSwitches |= BRANCH_PREDICTION; }
void Set_OD() { GenSwitches |= NO_OPTIMIZATION; }
void Set_OE()
{
    Inline_Threshold = OptValue;
    Toggles |= TOGGLE_INLINE;
}
#if _CPU == 8086 || _CPU == 386
void Set_OC() { TargetSwitches |= NO_CALL_RET_TRANSFORM; }
void Set_OF()
{
    TargetSwitches |= NEED_STACK_FRAME;
    if( OptValue != 0 )  DefaultInfo.class |= GENERATE_STACK_FRAME;
}
void Set_OM() { TargetSwitches |= I_MATH_INLINE; }
void Set_OP() { CompFlags.op_switch_used = 1; } // force floats to memory
#endif
void Set_OH() { GenSwitches |= SUPER_OPTIMAL; }
void Set_OK() { GenSwitches |= FLOW_REG_SAVES; }
void Set_OI() { CompFlags.inline_functions = 1; }
void Set_OL()      { GenSwitches |= LOOP_OPTIMIZATION; }
void Set_OL_plus() { GenSwitches |= LOOP_OPTIMIZATION | LOOP_UNROLLING; }
void Set_ON() { GenSwitches |= FP_UNSTABLE_OPTIMIZATION; }
void Set_OO() { GenSwitches &= ~MEMORY_LOW_FAILS; }
void Set_OR() { GenSwitches |= INS_SCHEDULING; }
void Set_OS() { GenSwitches &= ~NO_OPTIMIZATION; OptSize = 100; }
void Set_OT() { GenSwitches &= ~NO_OPTIMIZATION; OptSize = 0; }
void Set_OU() { CompFlags.unique_functions = 1; }
void Set_OX()
{
    Toggles &= ~TOGGLE_CHECK_STACK;
    GenSwitches &= ~NO_OPTIMIZATION;
    GenSwitches |= LOOP_OPTIMIZATION | INS_SCHEDULING | BRANCH_PREDICTION;
    CompFlags.inline_functions = 1;
    Set_OE();
#if _CPU == 8086 || _CPU == 386
    TargetSwitches |= I_MATH_INLINE;
#endif
}
void Set_OZ()   { GenSwitches |= NULL_DEREF_OK; }

// '=' indicates optional '='
// '#' indicates a decimal numeric value
// '$' indicates identifier
// '@' indicates filename
// '*' indicates additional characters will be scanned by option routine
// if a capital letter appears in the option, then input must match exactly
// otherwise all input characters are changed to lower case before matching

struct option const Optimization_Options[] = {
    { "a",      0,              Set_OA },
    { "b",      0,              Set_OB },
    { "d",      0,              Set_OD },
    { "e=#",    20,             Set_OE },
#if _CPU == 8086 || _CPU == 386
    { "c",      0,              Set_OC },
    { "f+",     1,              Set_OF },
    { "f",      0,              Set_OF },
    { "m",      0,              Set_OM },
    { "p",      0,              Set_OP },
#endif
    { "h",      0,              Set_OH },
    { "i",      0,              Set_OI },
    { "k",      0,              Set_OK },
    { "l+",     0,              Set_OL_plus },
    { "l",      0,              Set_OL },
    { "n",      0,              Set_ON },
    { "o",      0,              Set_OO },
    { "r",      0,              Set_OR },
    { "s",      0,              Set_OS },
    { "t",      0,              Set_OT },
    { "u",      0,              Set_OU },
    { "x",      0,              Set_OX },
    { "z",      0,              Set_OZ },
    { 0,        0,              0 },
};

struct option const Preprocess_Options[] = {
    { "c",      0,              Set_PC },
    { "l",      0,              Set_PL },
    { "w=#",    0,              Set_PW },
    { "=",      0,              Set_PreProcChar },
    { "#",      0,              Set_PreProcChar },
    { 0,        0,              0 },
};

extern void SetOptimization();
extern void SetPreprocessOptions();

struct option const CFE_Options[] = {
    { "o*",     0,              SetOptimization },
    { "i=@",    0,              SetInclude },
    { "zq",     0,              Set_ZQ },
#if _CPU == 8086
    { "0",      SW_CPU0,        SetCPU },
    { "1",      SW_CPU1,        SetCPU },
    { "2",      SW_CPU2,        SetCPU },
    { "3",      SW_CPU3,        SetCPU },
    { "4",      SW_CPU4,        SetCPU },
    { "5",      SW_CPU5,        SetCPU },
    { "6",      SW_CPU6,        SetCPU },
#endif
#if _CPU == 386
    { "6r",     SW_CPU6,        SetCPU_xR },
    { "6s",     SW_CPU6,        SetCPU_xS },
    { "6",      SW_CPU6,        SetCPU },
    { "5r",     SW_CPU5,        SetCPU_xR },
    { "5s",     SW_CPU5,        SetCPU_xS },
    { "5",      SW_CPU5,        SetCPU },
    { "4r",     SW_CPU4,        SetCPU_xR },
    { "4s",     SW_CPU4,        SetCPU_xS },
    { "4",      SW_CPU4,        SetCPU },
    { "3r",     SW_CPU3,        SetCPU_xR },
    { "3s",     SW_CPU3,        SetCPU_xS },
    { "3",      SW_CPU3,        SetCPU },
#endif
    { "aa",     0,              Set_AA },
    { "ai",     0,              Set_AI },
    { "aq",     0,              Set_AQ },
#if _MACHINE == _ALPHA
    { "as",     0,              Set_AS },
#endif
    { "d0*",    0,              Set_D0 },
    { "d1*",    1,              Set_D1 },
    { "d2*",    2,              Set_D2 },
    { "d3*",    3,              Set_D3 },
    { "d9*",    9,              Set_D9 },
    { "d+*",    0,              SetExtendedDefines },
    { "db",     0,              SetBrowserInfo },
    { "d*",     0,              DefineMacro },
    { "en",     0,              Set_EN },
    { "ep=#",   0,              Set_EP },
    { "ee",     0,              Set_EE },
    { "ef",     0,              Set_EF },
    { "ei",     0,              Set_EI },
    { "em",     0,              Set_EM },
#if _CPU == 8086 || _CPU == 386
    { "ec",     0,              Set_EC },
    { "et",     0,              Set_ET },
    { "eq",     0,              Set_EQ },
    { "etp",    0,              Set_ETP },
    { "esp",    0,              Set_ESP },
#endif
#if _CPU == 386
    { "ez",     0,              Set_EZ },
#endif
    { "e=#",    0,              SetErrorLimit },
#if _CPU == 8086 || _CPU == 386
    { "hw",     SW_DF_WATCOM,   Set_DbgFmt },
#endif
    { "hda",    SW_DF_DWARF_A,  Set_DbgFmt },
    { "hdg",    SW_DF_DWARF_G,  Set_DbgFmt },
    { "hd",     SW_DF_DWARF,    Set_DbgFmt },
    { "hc",     SW_DF_CV,       Set_DbgFmt },
#if _CPU == 8086 || _CPU == 386
    { "g=$",    0,              SetGroup },
#endif
    { "lc",     0,              SetAPILogging },
#ifndef NDEBUG
#ifdef ASM_OUTPUT
    { "la",     0,              SetAsmListing },
#endif
#ifdef OWL_LOGGING
    { "lo",     0,              SetOwlLogging },
#endif
#endif
#if _CPU == 8086 || _CPU == 386
    { "ms",     SW_MS,          SetMemoryModel },
    { "mm",     SW_MM,          SetMemoryModel },
    { "mc",     SW_MC,          SetMemoryModel },
    { "ml",     SW_ML,          SetMemoryModel },
#endif
#if _CPU == 8086
    { "mh",     SW_MH,          SetMemoryModel },
#endif
#if _CPU == 386
    { "mf",     SW_MF,          SetMemoryModel },
#endif
#if _CPU == 8086 || _CPU == 386
    { "nc=$",   0,              SetCodeClass },
    { "nd=$",   0,              SetDataSegName },
    { "nt=$",   0,              SetTextSegName },
#endif
    { "nm=$",   0,              SetModuleName },
    { "p*",     0,              SetPreprocessOptions },
    { "rod=@",  0,              SetReadOnlyDir },
#if _CPU == 8086 || _CPU == 386
    { "re",     0,              Set_RE },
    { "ri",     0,              Set_RI },
    { "r",      0,              Set_R },
    { "sg",     0,              Set_SG },
    { "st",     0,              Set_ST },
#endif
#if _MACHINE == _ALPHA
    { "si",     0,              Set_SI },
#endif
    { "s",      0,              Set_S },
    { "bd",     0,              Set_BD },
    { "bc",     0,              Set_BC },
    { "bg",     0,              Set_BG },
    { "bm",     0,              Set_BM },
#if _CPU != 8086
    { "br",     0,              Set_BR },
#endif
    { "bw",     0,              Set_BW },
    { "bt=$",   0,              Set_BT },
    { "fhq=@",  0,              Set_FHQ },
    { "fh=@",   0,              Set_FH },
    { "fi=@",   0,              Set_FI },
    { "fo=@",   0,              Set_FO },
    { "fr=@",   0,              Set_FR },
#if _CPU == 8086 || _CPU == 386
    { "fp2",    SW_FPU0,        SetFPU },
    { "fp3",    SW_FPU3,        SetFPU },
    { "fp5",    SW_FPU5,        SetFPU },
    { "fp6",    SW_FPU6,        SetFPU },
    { "fpr",    0,              Set_FPR },
    { "fpi87",  0,              Set_FPI87 },
    { "fpi",    0,              Set_Emu },
    { "fpc",    0,              Set_FPC },
    { "fpd",    0,              Set_FPD },
#endif
    { "j",      0,              SetCharType },
    { "tp=$",   0,              Set_TP },
    { "u$",     0,              Set_U },
    { "v",      0,              Set_V },
    { "wcd=#",  0,              Set_WCD },
    { "wce=#",  0,              Set_WCE },
    { "we",     0,              Set_WE },
    { "wo",     0,              Set_WO },
    { "wx",     0,              Set_WX },
    { "w=#",    0,              SetWarningLevel },
#if _CPU == 386
    { "xgv",    0,              Set_XGV },
#endif
    { "xbsa",   0,              Set_XBSA },
#if _MACHINE == _ALPHA
    { "xd",     0,              Set_XD },
#endif
    { "zA",     0,              SetStrictANSI },
    { "za",     0,              Set_ZA },
#if _CPU == 8086 || _CPU == 386
    { "zc",     0,              Set_ZC },
    { "zdf",    0,              Set_ZDF },
    { "zdp",    0,              Set_ZDP },
    { "zdl",    0,              Set_ZDL },
    { "zff",    0,              Set_ZFF },
    { "zfp",    0,              Set_ZFP },
    { "zgf",    0,              Set_ZGF },
    { "zgp",    0,              Set_ZGP },
#endif
    { "ze",     0,              Set_ZE },
    { "zg",     0,              Set_ZG },
    { "zi",     0,              Set_ZI },
    { "zk0u",   0,              Set_ZK0U },
    { "zk0",    0,              Set_ZK0 },
    { "zk1",    0,              Set_ZK1 },
    { "zk2",    0,              Set_ZK2 },
    { "zk3",    0,              Set_ZK3 },
    { "zkl",    0,              Set_ZKL },
    { "zku*",   0,              Set_ZKU },
    { "zld",    0,              Set_ZLD },
    { "zlf",    0,              Set_ZLF },
    { "zl",     0,              Set_ZL },
    { "zm",     0,              Set_ZM },
    { "zpw",    0,              Set_ZPW },
    { "zp=#",   1,              SetPackAmount },
#if _MACHINE == _ALPHA
    { "zps",    0,              SetStructPack },
#endif
    { "zs",     0,              Set_ZS },
    { "zt=#",   256,            SetDataThreshHold },
#if _CPU == 8086 || _CPU == 386
    { "zu",     0,              Set_ZU },
#endif
    { "zv",     0,              Set_ZV },
#if _CPU == 8086
    { "zW*",    0,              SetCheapWindows },
    { "zw*",    0,              SetWindows },
#endif
#if _CPU == 386
    { "zw",     0,              SetWindows },
    { "zz",     0,              Set_ZZ },
#endif
    { 0,        0,              0 },
};

char *ProcessOption( struct option const *op_table, char *p, char *option_start )
{
    int         i;
    int         j;
    char        *opt;
    char        c;

    for( i = 0; ; i++ ) {
        opt = op_table[i].option;
        if( opt == NULL ) break;
        c = tolower( *p );
        if( c == *opt ) {
            OptValue = op_table[i].value;
            j = 1;
            for(;;) {
                ++opt;
                if( *opt == '\0' || *opt == '*' ) {
                    if( *opt == '\0' ) {
                        if( p - option_start == 1 ) {
                            // make sure end of option
                            if( !OptionDelimiter( p[j] ) ) break;
                        }
                    }
                    OptScanPtr = p + j;
                    op_table[i].function();
                    return( OptScanPtr );
                }
                if( *opt == '#' ) {             // collect a number
                    if( p[j] >= '0' && p[j] <= '9' ) {
                        OptValue = 0;
                        for(;;) {
                            c = p[j];
                            if( c < '0' || c > '9' ) break;
                            OptValue = OptValue * 10 + c - '0';
                            ++j;
                        }
                    }
                } else if( *opt == '$' ) {      // collect an identifer
                    OptParm = &p[j];
                    for(;;) {
                        c = p[j];
                        if( c == '\0' ) break;
                        if( c == '-' ) break;
                        if( c == ' ' ) break;
                        if( c == SwitchChar ) break;
                        ++j;
                    }
                } else if( *opt == '@' ) {      // collect a filename
                    OptParm = &p[j];
                    c = p[j];
                    if( c == '"' ){ // "filename"
                        for(;;){
                            c = p[++j];
                            if( c == '"' ){
                                ++j;
                                break;
                            }
                            if( c == '\0' )break;
                            if( c == '\\' ){
                                ++j;
                            }
                        }
                    }else{
                        for(;;) {
                            c = p[j];
                            if( c == '\0' ) break;
                            if( c == ' ' ) break;
                            if( c == '\t' ) break;
                            #if _OS != _QNX
                                if( c == SwitchChar ) break;
                            #endif
                            ++j;
                        }
                    }
                } else if( *opt == '=' ) {      // collect an optional '='
                    if( p[j] == '=' || p[j] == '#' ) ++j;
                } else {
                    c = tolower( p[j] );
                    if( *opt != c ) {
                        if( *opt < 'A' || *opt > 'Z' ) break;
                        if( *opt != p[j] ) break;
                    }
                    ++j;
                }
            }
        }
    }
    if( op_table == Optimization_Options ) {
        p = BadCmdLine( ERR_INVALID_OPTIMIZATION, p );
    } else {
        p = BadCmdLine( ERR_INVALID_OPTION, option_start );
    }
    return( p );
}

void ProcessSubOption( struct option const *op_table )
{
    char        *option_start;

    option_start = OptScanPtr - 2;
    for(;;) {
        OptScanPtr = ProcessOption( op_table, OptScanPtr, option_start );
        if( OptionDelimiter( *OptScanPtr ) ) break;
    }
}

void SetOptimization()
{
    ProcessSubOption( Optimization_Options );
}

void SetPreprocessOptions()
{
    CompFlags.cpp_output_requested = 1;
    if( !OptionDelimiter( *OptScanPtr ) ) {
        ProcessSubOption( Preprocess_Options );
    }
}

char *CollectEnvOrFileName( char *str )
{
    char        *env;
    char        ch;

    while( *str == ' ' || *str == '\t' ) ++str;
    env = TokenBuf;
    for( ;; ) {
        ch = *str;
        if( ch == '\0' ) break;
        ++str;
        if( ch == ' ' ) break;
        if( ch == '\t' ) break;
        #if _OS != _QNX
            if( ch == '-' ) break;
            if( ch == SwitchChar ) break;
        #endif
        *env++ = ch;
    }
    *env = '\0';
    return( str );
}

char *ReadIndirectFile()
{
    char        *env;
    char        *str;
    int         handle;
    int         len;
    char        ch;

    env = NULL;
    handle = open( TokenBuf, O_RDONLY | O_BINARY );
    if( handle != -1 ) {
        len = filelength( handle );
        env = CMemAlloc( len + 1 );
        read( handle, env, len );
        env[len] = '\0';
        close( handle );
        // zip through characters changing \r, \n etc into ' '
        str = env;
        while( *str ) {
            ch = *str;
            if( ch == '\r' || ch == '\n' ) {
                *str = ' ';
            }
            #if _OS != _QNX
                if( ch == 0x1A ) {      // if end of file
                    *str = '\0';        // - mark end of str
                    break;
                }
            #endif
            ++str;
        }
    }
    return( env );
}

#define MAX_NESTING 32

local void ProcOptions( char *str )
{
    unsigned    level;
    char        *save[MAX_NESTING];
    char        *buffers[MAX_NESTING];

    if( str != NULL ) {
        level = 0;
        buffers[0] = NULL;
        for(;;) {
            while( *str == ' ' || *str == '\t' ) ++str;     /* 16-mar-91 */
            if( *str == '@' && level < MAX_NESTING ) {
                save[level] = CollectEnvOrFileName( str + 1 );
                ++level;
                buffers[level] = NULL;
                str = FEGetEnv( TokenBuf );
                if( str == NULL ) {
                    str = ReadIndirectFile();
                    buffers[level] = str;
                }
                if( str != NULL )  continue;
                str = save[--level];
            }
            if( *str == '\0' ) {
                if( level == 0 ) break;
                if( buffers[level] != NULL ) {
                    CMemFree( buffers[level] );
                    buffers[level] = NULL;
                }
                str = save[--level];
                continue;
            }
            if( *str == '-'  ||  *str == SwitchChar ) {
                str = ProcessOption( CFE_Options, str + 1, str );
            } else {  /* collect  file name */
                char *beg, *p;
                int len;

                beg = str;
                if( *str == '"' ){
                    for(;;){
                        ++str;
                        if( *str == '"' ){
                            ++str;
                            break;
                        }
                        if( *str == '\0' ) break;
                        if( *str == '\\' ){
                            ++str;
                        }
                    }
                }else{
                    for(;;) {
                        if( *str == '\0' ) break;
                        if( *str == ' '  ) break;
                        if( *str == '\t'  ) break;              /* 16-mar-91 */
                        #if _OS != _QNX
                            if( *str == SwitchChar ) break;
                        #endif
                        ++str;
                    }
                }
                len = str-beg;
                p = (char *) CMemAlloc( len + 1 );
                memcpy( p, beg, len );
                p[ len ] = '\0';
                StripQuotes( p );
                if( WholeFName != NULL ) {
                    /* more than one file to compile ? */
                    CBanner();
                    CErr1( ERR_CAN_ONLY_COMPILE_ONE_FILE );
                    CMemFree( WholeFName );
                }
                WholeFName = p;
            }
        }
    }
}

static void InitCPUModInfo()
{
    CodeClassName = NULL;
    PCH_FileName  = NULL;
    TargetSwitches = 0;
#if _MACHINE == _ALPHA | _MACHINE == _PPC | _MACHINE == _SPARC
    TextSegName   = ".text";
    DataSegName   = ".data";
    GenCodeGroup  = "";
    DataPtrSize   = TARGET_POINTER;
    CodePtrSize   = TARGET_POINTER;
    GenSwitches   = MEMORY_LOW_FAILS;
#elif _CPU == 386 || _CPU == 8086
    Stack87 = 8;
    TextSegName   = "";
    DataSegName   = "";
    GenCodeGroup  = "";
    TargSys = TS_OTHER;
    CompFlags.register_conv_set = 0;
    CompFlags.register_conventions = 1;
    GenSwitches = MEMORY_LOW_FAILS;
#else
    #error InitCPUModInfo not configured for system
#endif
}

local void Define_Memory_Model()
{
#if _CPU == 8086 || _CPU == 386
    char        model;
#endif

    DataPtrSize = TARGET_POINTER;
    CodePtrSize = TARGET_POINTER;
    #if _CPU == 8086 || _CPU == 386
    switch( TargetSwitches & (BIG_CODE|BIG_DATA) ) {
    case 0:                     /* -ms */
        model = 's';
        if( TargetSwitches & FLAT_MODEL ) {     /* 06-apr-89 */
            Define_Macro( "__FLAT__" );
        } else {
            Define_Macro( "M_I86SM" );
            Define_Macro( "__SMALL__" );
            CompFlags.strings_in_code_segment = 0;      /* 01-sep-89 */
        }
        TargetSwitches &= ~CONST_IN_CODE;
        break;
    case BIG_CODE:                      /* -mm */
        model = 'm';
        DefaultInfo.class |= FAR;
        CodePtrSize = TARGET_FAR_POINTER;
        Define_Macro( "M_I86MM" );
        Define_Macro( "__MEDIUM__" );
        CompFlags.strings_in_code_segment = 0;  /* 01-sep-89 */
        TargetSwitches &= ~CONST_IN_CODE;
        break;
    case BIG_DATA:                      /* -mc */
        model = 'c';
        Define_Macro( "M_I86CM" );
        Define_Macro( "__COMPACT__" );
        DataPtrSize = TARGET_FAR_POINTER;                       /* 04-may-90 */
        break;
    case BIG_CODE | BIG_DATA:
        DefaultInfo.class |= FAR;
        CodePtrSize = TARGET_FAR_POINTER;                       /* 04-may-90 */
        if( TargetSwitches & CHEAP_POINTER ) {
            model = 'l';
            Define_Macro( "M_I86LM" );      /* -ml */
            Define_Macro( "__LARGE__" );
        } else {
            model = 'h';
            Define_Macro( "M_I86HM" );      /* -mh */
            Define_Macro( "__HUGE__" );
        }
        DataPtrSize = TARGET_FAR_POINTER;                       /* 04-may-90 */
        break;
    }
    #endif
    #if _CPU == 8086
        strcpy( CLIB_Name, "1clib?" );                          /* 15-may-00 */
        if( CompFlags.bm_switch_used ) {
            strcpy( CLIB_Name, "1clibmt?" );
        }
        if( CompFlags.bd_switch_used ) {
            if( TargSys == TS_WINDOWS ||
                TargSys == TS_CHEAP_WINDOWS ) {
                strcpy( CLIB_Name, "1clib?" );
            } else {
                strcpy( CLIB_Name, "1clibdl?" );
            }
        }
        #ifdef __PCODE__
            PcodeLib_Name = "2p16fpi";                      /* 19-may-91 */
        #endif
        if( GET_FPU_EMU( ProcRevision ) ) {         /* 07-jan-90 */
            strcpy( MATHLIB_Name, "8math87?" );
            EmuLib_Name = "9emu87";                     /* 02-apr-90 */
        } else if( GET_FPU_LEVEL( ProcRevision ) == FPU_NONE ) {
            strcpy( MATHLIB_Name, "5math?" );
            EmuLib_Name = NULL;
            #ifdef __PCODE__
                PcodeLib_Name = "2p16fpc";                  /* 19-may-91 */
            #endif
        } else {
            strcpy( MATHLIB_Name, "8math87?" );
            EmuLib_Name = "9noemu87";                   /* 02-apr-90 */
        }
    #elif _CPU == 386
        model = 'r';                                    /* 07-nov-89 */
        if( ! CompFlags.register_conventions ) model = 's';
        if( CompFlags.br_switch_used ) {                /* 15-may-95 */
            strcpy( CLIB_Name, "1clb?dll" );
        } else {
            strcpy( CLIB_Name, "1clib3?" );     // There is only 1 CLIB now!
        }
        if( GET_FPU_EMU( ProcRevision ) ) {
            if( CompFlags.br_switch_used ) {            /* 19-jun-95 */
                strcpy( MATHLIB_Name, "8mt7?dll" );
            } else {
                strcpy( MATHLIB_Name, "8math387?" );
            }
            EmuLib_Name = "9emu387";
        } else if( GET_FPU_LEVEL( ProcRevision ) == FPU_NONE ) {
            if( CompFlags.br_switch_used ) {            /* 19-jun-95 */
                strcpy( MATHLIB_Name, "5mth?dll" );
            } else {
                strcpy( MATHLIB_Name, "5math3?" );
            }
            EmuLib_Name = NULL;
        } else {
            if( CompFlags.br_switch_used ) {            /* 19-jun-95 */
                strcpy( MATHLIB_Name, "8mt7?dll" );
            } else {
                strcpy( MATHLIB_Name, "8math387?" );
            }
            EmuLib_Name = "9noemu387";
        }
    #elif _MACHINE == _ALPHA || _MACHINE == _PPC || _MACHINE == _SPARC
        if( CompFlags.br_switch_used ) {                /* 15-may-95 */
            strcpy( CLIB_Name, "1clbdll" );
            strcpy( MATHLIB_Name, "8mthdll" );
        } else {
            strcpy( CLIB_Name, "1clib" );
            strcpy( MATHLIB_Name, "8math" );
        }
    #else
        #error Define_Memory_Model not configured
    #endif
    #if _CPU == 8086 || _CPU == 386
        *strchr( CLIB_Name, '?' ) = model;
        *strchr( MATHLIB_Name, '?' ) = model;
    #endif
}

void GenCOptions( char **cmdline )
{
    memset( &SwData,0, sizeof( SwData ) ); //re-useable
    EnableDisableMessage( 0, ERR_PARM_NOT_REFERENCED );
    InitModInfo();
    InitCPUModInfo();
    #if _CPU == 386
        ProcOptions( FEGetEnv( "WCC386" ) );              /* 12-mar-90 */
    #elif _CPU == 8086
        ProcOptions( FEGetEnv( "WCC" ) );                 /* 12-mar-90 */
    #elif _MACHINE == _ALPHA
        ProcOptions( FEGetEnv( "WCCAXP" ) );
    #elif _MACHINE == _PPC
        ProcOptions( FEGetEnv( "WCCPPC" ) );
    #elif _MACHINE == _SPARC
        ProcOptions( FEGetEnv( "WCCSPC" ) );
    #else
        #error Compiler environment variable not configured
    #endif
    for( ;*cmdline != NULL; ++cmdline ) {
        ProcOptions( *cmdline );
    }
    if( CompFlags.cpp_output_requested )  CompFlags.cpp_output = 1;
    if( CompFlags.cpp_output )  CompFlags.quiet_mode = 1;       /* 29-sep-90 */
    CBanner();          /* print banner if -zq not specified */
    GblPackAmount = PackAmount;
    SetTargSystem();
    SetGenSwitches();
    Define_Memory_Model();
    #ifdef __PCODE__                                                /* 04-feb-91 */
        if( Toggles & TOGGLE_PCODE )  CompFlags.inline_functions = 0;
    #endif
    #if _CPU == 8086 || _CPU == 386
        if( GET_CPU( ProcRevision ) < CPU_386 ) {
            /* issue warning message if /zf[f|p] or /zg[f|p] spec'd? */
            TargetSwitches &= ~(FLOATING_FS|FLOATING_GS);
        }
        if( ! CompFlags.save_restore_segregs ) {                /* 11-apr-91 */
            if( TargetSwitches & FLOATING_DS ) {
                HW_CTurnOff( DefaultInfo.save, HW_DS );
            }
            if( TargetSwitches & FLOATING_ES ) {
                HW_CTurnOff( DefaultInfo.save, HW_ES );
            }
            if( TargetSwitches & FLOATING_FS ) {
                HW_CTurnOff( DefaultInfo.save, HW_FS );
            }
            if( TargetSwitches & FLOATING_GS ) {
                HW_CTurnOff( DefaultInfo.save, HW_GS );
            }
        }
        FastCallInfo = DefaultInfo; // save reg conventions
        #if _CPU == 386
            if( ! CompFlags.register_conventions )  SetStackConventions();
        #endif
    #endif
    MacroDefs();                                        /* 07-aug-90 */
    MiscMacroDefs();
}

#if _CPU == 386

hw_reg_set MetaWareParms[] = {
        0, 0
};

local void SetStackConventions()
{
    DefaultInfo.class &= (GENERATE_STACK_FRAME | FAR); /* 19-nov-93 */
    DefaultInfo.class |= CALLER_POPS | NO_8087_RETURNS;
    DefaultInfo.code  = NULL;
    DefaultInfo.parms = (hw_reg_set *)CMemAlloc( sizeof(MetaWareParms) );
    memcpy( DefaultInfo.parms, MetaWareParms, sizeof( MetaWareParms ) );
    HW_CAsgn( DefaultInfo.returns, HW_EMPTY );
    HW_CAsgn( DefaultInfo.streturn, HW_EMPTY );
    HW_CTurnOff( DefaultInfo.save, HW_EAX );
    HW_CTurnOff( DefaultInfo.save, HW_EDX );
    HW_CTurnOff( DefaultInfo.save, HW_ECX );
    if( ! CompFlags.save_restore_segregs ) {            /* 11-apr-91 */
        if( TargetSwitches & FLOATING_DS ) {
            HW_CTurnOff( DefaultInfo.save, HW_DS );
        }
        if( TargetSwitches & FLOATING_ES ) {
            HW_CTurnOff( DefaultInfo.save, HW_ES );
        }
        if( TargetSwitches & FLOATING_FS ) {
            HW_CTurnOff( DefaultInfo.save, HW_FS );
        }
        if( TargetSwitches & FLOATING_GS ) {
            HW_CTurnOff( DefaultInfo.save, HW_GS );
        }
    }
    HW_CTurnOff( DefaultInfo.save, HW_FLTS );
    DefaultInfo.use     = 0;
    DefaultInfo.objname = CStrSave( "*" );   /* DefaultObjName; */
}
#endif
