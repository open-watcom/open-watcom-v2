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
* Description:  C compiler command line option processing.
*
****************************************************************************/


/*****************************************************************************
*                                                                            *
*       If you add an option, don't forget to change ../usage.sp             *
*       Also, don't forget to add a case in MacroDefs                        *
*       to predefine a __SW_xx macro                                         *
*                                                                            *
*****************************************************************************/
#include "cvars.h"
#include <ctype.h>
#include "wio.h"
#include "watcom.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "cgdefs.h"
#include "cgswitch.h"
#include "iopath.h"
#include "pathlist.h"
#ifdef __OSI__
 #include "ostype.h"
#endif
#include "feprotos.h"

enum encoding {
    ENC_ZK = 1,
    ENC_ZK0,
    ENC_ZK1,
    ENC_ZK2,
    ENC_ZK3,
    ENC_ZKL,
    ENC_ZKU,
    ENC_ZK0U
};

struct  option {
    char        *option;
    unsigned    value;
    void        (*function)(void);
};

static unsigned     OptValue;
static const char   *OptScanPtr;
static const char   *OptParm;

#define __isdigit(c)    ((c) >= '0' && (c) <= '9')

#define PEGGED( r )     unsigned peg_##r##s_used        : 1;    \
                        unsigned peg_##r##s_on          : 1

static struct
{
    char        *sys_name;

    enum    /* TARGET CPU SUPPORT - Intel defined (AXP/PPC uses CPU0 as default CPU) */
    {
        SW_CPU_DEF,     /*  No target CPU specified     */
        SW_CPU0,        /*  Target 8086/8               */
        SW_CPU1,        /*  Target 80186/8              */
        SW_CPU2,        /*  Target 80286                */
        SW_CPU3,        /*  Target 80386                */
        SW_CPU4,        /*  Target 80486                */
        SW_CPU5,        /*  Target Pentium              */
        SW_CPU6         /*  Target Pentium-Pro          */
    }cpu;

    enum    /* TARGET FPU SUPPORT */
    {
        SW_FPU_DEF,     /*  No target FPU specified     */
        SW_FPU0,        /*  Target 8087 co-pro          */
        SW_FPU3,        /*  Target 80387 co-pro         */
        SW_FPU5,        /*  Target Pentium int fpu      */
        SW_FPU6         /*  Target Pentium-Pro int fpu  */
    }fpu;

    enum    /* FPU CALL TYPES */
    {
        SW_FPT_DEF,     /*  No FPU call type specified  */
        SW_FPT_CALLS,   /*  FPU calls via library       */
        SW_FPT_EMU,     /*  FPU calls inline & emulated */
        SW_FPT_INLINE   /*  FPU calls inline            */
    }fpt;

    enum    /* MEMORY MODELS */
    {
        SW_M_DEF,       /*  No memory model specified   */
        SW_MF,          /*  Flat memory model           */
        SW_MS,          /*  Small memory model          */
        SW_MM,          /*  Medium memory model         */
        SW_MC,          /*  Compact memory model        */
        SW_ML,          /*  Large memory model          */
        SW_MH           /*  Huge memory model           */
    }mem;

    enum    /*  DEBUGGING INFORMATION TYPE */
    {
        SW_DF_DEF,      /*  No debug type specified     */
        SW_DF_WATCOM,   /*  Use Watcom                  */
        SW_DF_CV,       /*  Use CodeView                */
        SW_DF_DWARF,    /*  Use DWARF                   */
        SW_DF_DWARF_A,  /*  Use DWARF + A?              */
        SW_DF_DWARF_G   /*  Use DWARF + G?              */
    }dbg_fmt;

    PEGGED( d );
    PEGGED( e );
    PEGGED( f );
    PEGGED( g );
    unsigned    nd_used         : 1;
} SwData;

// local variables
static int      character_encoding = 0;
static unsigned unicode_CP = 0;

bool EqualChar( int c )
{
    return( c == '#' || c == '=' );
}

static void SetCharacterEncoding( void )
{
    CompFlags.jis_to_unicode = 0;

    switch( character_encoding ) {
    case ENC_ZKU:
        LoadUnicodeTable( unicode_CP );
        break;
    case ENC_ZK0U:
        CompFlags.use_unicode = 0;
        SetDBChar( 0 );                     /* set double-byte char type */
        CompFlags.jis_to_unicode = 1;
        break;
    case ENC_ZK:
    case ENC_ZK0:
        CompFlags.use_unicode = 0;
        SetDBChar( 0 );                     /* set double-byte char type */
        break;
    case ENC_ZK1:
        CompFlags.use_unicode = 0;
        SetDBChar( 1 );                     /* set double-byte char type */
        break;
    case ENC_ZK2:
        CompFlags.use_unicode = 0;
        SetDBChar( 2 );                     /* set double-byte char type */
        break;
    case ENC_ZK3:
        CompFlags.use_unicode = 0;
        SetDBChar( 3 );                     /* set double-byte char type */
        break;
    case ENC_ZKL:
        CompFlags.use_unicode = 0;
        SetDBChar( -1 );                    /* set double-byte char type to defualt */
        break;
    }
}

local void SetTargName( const char *name, size_t len )
{
    char        *p;

    if( SwData.sys_name != NULL ) {
        CMemFree( SwData.sys_name );
        SwData.sys_name = NULL;
    }
    if( name == NULL || len == 0 )
        return;
    SwData.sys_name = CMemAlloc( len + 1 ); /* for NULLCHAR */
    p = SwData.sys_name;
    while( len != 0 ) {
        *p++ = toupper( *(unsigned char *)name++ );
        --len;
    }
    *p++ = '\0';
}

#define _SetConstTarg( name ) SetTargName( name, sizeof( name ) - 1 )

local void SetTargSystem( void )
{
    char        buff[128];
    size_t      len;

    if( CompFlags.oldmacros_enabled ) {
#if _CPU == _AXP
        PreDefine_Macro( "M_ALPHA" );
#elif _CPU == _SPARC
        PreDefine_Macro( "M_SPARC" );
#elif _CPU == _PPC
        PreDefine_Macro( "M_PPC" );
#elif _CPU == _MIPS
        PreDefine_Macro( "M_MRX000" );
#elif _CPU == 386
        PreDefine_Macro( "M_I386" );
#elif _CPU == 8086
        PreDefine_Macro( "M_I86" );
#endif
    }
#if _CPU == _AXP
    PreDefine_Macro( "_M_ALPHA" );
    PreDefine_Macro( "__ALPHA__" );
    PreDefine_Macro( "_ALPHA_" );
    PreDefine_Macro( "__AXP__" );
    PreDefine_Macro( "_STDCALL_SUPPORTED" );
#elif _CPU == _SPARC
    PreDefine_Macro( "_M_SPARC" );
    PreDefine_Macro( "__SPARC__" );
    PreDefine_Macro( "_SPARC_" );
#elif _CPU == _PPC
    PreDefine_Macro( "_M_PPC" );
    PreDefine_Macro( "__POWERPC__" );
    PreDefine_Macro( "__PPC__" );
    PreDefine_Macro( "_PPC_" );
#elif _CPU == _MIPS
    PreDefine_Macro( "_M_MRX000" );
    PreDefine_Macro( "__MIPS__" );
#elif _CPU == 386
    PreDefine_Macro( "_M_I386" );
    PreDefine_Macro( "__386__" );
    PreDefine_Macro( "__X86__" );
    PreDefine_Macro( "_X86_" );
    PreDefine_Macro( "_STDCALL_SUPPORTED" );
#elif _CPU == 8086
    PreDefine_Macro( "_M_I86" );
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
        switch( __OS ) {
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
    #elif defined( __QNX__ )
        _SetConstTarg( "qnx" );
    #elif defined( __LINUX__ )
        _SetConstTarg( "linux" );
    #elif defined( __SOLARIS__ ) || defined( __sun__ )
        _SetConstTarg( "solaris" );
    #elif defined( __OSX__ ) || defined( __APPLE__ )
        _SetConstTarg( "osx" );
    #elif defined( __OS2__ )
        _SetConstTarg( "os2" );
    #elif defined( __NT__ )
        _SetConstTarg( "nt" );
    #elif defined( __DOS__ )
        _SetConstTarg( "dos" );
    #elif defined( __BSD__ )
        _SetConstTarg( "bsd" );
    #else
        #error "Target OS not defined"
    #endif
#elif _CPU == _AXP || _CPU == _PPC || _CPU == _SPARC || _CPU == _MIPS
        /* we only have NT libraries for Alpha right now */
        _SetConstTarg( "nt" );
#else
    #error Target Machine OS not configured
#endif
    }

    if( CMPLIT( SwData.sys_name, "DOS" ) == 0 ) {
        TargSys = TS_DOS;
    } else if( CMPLIT( SwData.sys_name, "NETWARE" ) == 0 ) {
        TargSys = TS_NETWARE;
    } else if( CMPLIT( SwData.sys_name, "NETWARE5" ) == 0 ) {
        TargSys = TS_NETWARE5;
    } else if( CMPLIT( SwData.sys_name, "WINDOWS" ) == 0 ) {
        TargSys = TS_WINDOWS;
    } else if( CMPLIT( SwData.sys_name, "CHEAP_WINDOWS" ) == 0 ) {
        TargSys = TS_CHEAP_WINDOWS;
    } else if( CMPLIT( SwData.sys_name, "NT" ) == 0 ) {
        TargSys = TS_NT;
    } else if( CMPLIT( SwData.sys_name, "LINUX" ) == 0 ) {
        TargSys = TS_LINUX;
    } else if( CMPLIT( SwData.sys_name, "QNX" ) == 0 ) {
        TargSys = TS_QNX;
    } else if( CMPLIT( SwData.sys_name, "OS2" ) == 0 ) {
        TargSys = TS_OS2;
    } else {
        TargSys = TS_OTHER;
    }

    switch( TargSys ) {
    case TS_DOS:
        if( CompFlags.oldmacros_enabled ) {
            PreDefine_Macro( "MSDOS" );
        }
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
        if( TargSys == TS_NETWARE5 )
            PreDefine_Macro( "__NETWARE__" );
        PreDefine_Macro( "__NETWARE_386__" );
        /*
        //  NETWARE uses stack based calling conventions
        //  by default - silly people.
        */
        if( !CompFlags.register_conv_set ) {
            CompFlags.register_conventions = 0;
        }
        break;

    case TS_NT:
        PreDefine_Macro( "_WIN32" );
        break;
#endif
    case TS_QNX:
        /* fall through */
    case TS_LINUX:
        PreDefine_Macro( "__UNIX__" );
        break;

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
        default:
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
    len = strlen( SwData.sys_name );
    buff[0] = '_';
    buff[1] = '_';
    memcpy( buff + 2, SwData.sys_name, len );
    buff[2 + len] = '_';
    buff[2 + len + 1] = '_';
    buff[2 + len + 2] = '\0';
    PreDefine_Macro( buff );
}

#define SET_PEG( r ) if( !SwData.peg_##r##s_used ) SwData.peg_##r##s_on = 1;

static void SetGenSwitches( void )
{
#if _CPU == 8086 || _CPU == 386
  #if _CPU == 386
    if( SwData.cpu == SW_CPU_DEF ) SwData.cpu = SW_CPU6;
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
    default:
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
    default:
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
        SymDFAbbr = SpcSymbol( "__DFABBREV", GetType( TYPE_USHORT ), SC_EXTERN );
        break;
    case SW_DF_DWARF_G:
        GenSwitches |= DBG_DF | DBG_PREDEF;
        SymDFAbbr = SpcSymbol( "__DFABBREV", GetType( TYPE_USHORT ), SC_NONE );
        break;
    }
}

static void MacroDefs( void )
{
    if( TargetSwitches & I_MATH_INLINE ) {
        Define_Macro( "__SW_OM" );
    }
#if _CPU == 8086 || _CPU == 386
  #if _CPU == 8086
    #define MX86 "M_I86"
  #else
    #define MX86 "M_386"
  #endif
    if( CompFlags.oldmacros_enabled ) {
        switch( SwData.mem ) {
        case SW_MS:
            Define_Macro( MX86 "SM" );
            break;
        case SW_MM:
            Define_Macro( MX86 "MM" );
            break;
        case SW_MC:
            Define_Macro( MX86 "CM" );
            break;
        case SW_ML:
            Define_Macro( MX86 "LM" );
            break;
        case SW_MH:
            Define_Macro( MX86 "HM" );
            break;
        case SW_MF:
            Define_Macro( MX86 "FM" );
            break;
        default:
            break;
        }
    }
  #if _CPU == 8086
    #define X86 "_M_I86"
  #else
    #define X86 "_M_386"
  #endif
    switch( SwData.mem ) {
    case SW_MS:
        Define_Macro( "__SW_MS" );
        Define_Macro( X86 "SM" );
        Define_Macro( "__SMALL__" );
        break;
    case SW_MM:
        Define_Macro( "__SW_MM" );
        Define_Macro( X86 "MM" );
        Define_Macro( "__MEDIUM__" );
        break;
    case SW_MC:
        Define_Macro( "__SW_MC" );
        Define_Macro( X86 "CM" );
        Define_Macro( "__COMPACT__" );
        break;
    case SW_ML:
        Define_Macro( "__SW_ML" );
        Define_Macro( X86 "LM" );
        Define_Macro( "__LARGE__" );
        break;
    case SW_MH:
        Define_Macro( "__SW_MH" );
        Define_Macro( X86 "HM" );
        Define_Macro( "__HUGE__" );
        break;
    case SW_MF:
        Define_Macro( "__SW_MF" );
        Define_Macro( X86 "FM" );
        Define_Macro( "__FLAT__" );
        break;
    default:
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
    if( TargetSwitches & GEN_FWAIT_386 ) {
        Define_Macro( "__SW_ZFW" );
    }
#endif
#if _CPU == _AXP || _CPU == _PPC || _CPU == _MIPS
    if( GenSwitches & OBJ_ENDIAN_BIG ) {
        Define_Macro( "__BIG_ENDIAN__" );
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
    if( GenSwitches & FPU_ROUNDING_OMIT ) {
        Define_Macro( "__SW_ZRO" );
    }
    if( GenSwitches & FPU_ROUNDING_INLINE ) {
        Define_Macro( "__SW_ZRI" );
    }
    if( CompFlags.use_long_double ) {
        Define_Macro( "__SW_FLD" );
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
    if( CompFlags.bc_switch_used ) { /* Target is console application */
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
        CompFlags.op_switch_used = 0;
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
    default:
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
    if( CompFlags.op_switch_used ) {
        Define_Macro( "__SW_OP" );
    }
#endif
    if( (Toggles & TOGGLE_CHECK_STACK) == 0 ) {
        Define_Macro( "__SW_S" );
    }
}

static void AddIncList( const char *path_list )
{
    size_t      old_len;
    size_t      len;
    char        *old_list;
    char        *p;

    if( path_list != NULL && *path_list != '\0' ) {
        len = strlen( path_list );
        old_list = IncPathList;
        old_len = strlen( old_list );
        IncPathList = CMemAlloc( old_len + 1 + len + 1 );
        memcpy( IncPathList, old_list, old_len );
        CMemFree( old_list );
        p = IncPathList + old_len;
        while( *path_list != '\0' ) {
            if( p != IncPathList )
                *p++ = PATH_LIST_SEP;
            path_list = GetPathElement( path_list, NULL, &p );
        }
        *p = '\0';
    }
}

#define INC_VAR "INCLUDE"

void MergeInclude( void )
{
    /* must be called after GenCOptions to get req'd IncPathList */
    char        *env_var;
    char        buff[128];
    size_t      len;

    if( !CompFlags.cpp_ignore_env ) {
        switch( TargSys ) {
        case TS_CHEAP_WINDOWS:
        case TS_WINDOWS:
            len = sizeof( "WINDOWS" ) - 1;
            memcpy( buff, "WINDOWS", len );
            break;
        case TS_NETWARE:
        case TS_NETWARE5:
            len = sizeof( "NETWARE" ) - 1;
            memcpy( buff, "NETWARE", len );
            break;
        default:
            len = strlen( SwData.sys_name );
            memcpy( buff, SwData.sys_name, len );
            break;
        }
        CPYLIT( buff + len, "_" INC_VAR );
        AddIncList( FEGetEnv( buff ) );

#if _CPU == 386
        env_var = FEGetEnv( "INC386" );
        if( env_var == NULL ) {
            env_var = FEGetEnv( INC_VAR );
        }
#else
        env_var = FEGetEnv( INC_VAR );
#endif
        AddIncList( env_var );
    }
    CMemFree( SwData.sys_name );
}


static bool OptionDelimiter( char c )
{
    if( c == ' ' || c == '-' || c == '\0' || c == '\t' || c == SwitchChar ) {
        return( TRUE );
    }
    return( FALSE );
}

static void EnsureEndOfSwitch( void )
{
    char        c;

    if( !OptionDelimiter( *OptScanPtr ) ) {
        for( ;; ) {                       // find start of switch
            c = *OptScanPtr;
            if( c == '-' || c == SwitchChar )
                break;
            --OptScanPtr;
        }
        OptScanPtr = BadCmdLine( ERR_INVALID_OPTION, OptScanPtr );
    }
}

#if _CPU == _AXP
static void SetStructPack( void )   { CompFlags.align_structs_on_qwords = 1; }
#endif

static void Set_ZP( void )          { SetPackAmount( OptValue ); }
static void Set_DbgFmt( void )      { SwData.dbg_fmt = OptValue; }

#if _CPU == 8086 || _CPU == 386
static void SetCPU( void )          { SwData.cpu = OptValue; }
#endif
#if _CPU == 386
static void SetCPU_xR( void )   { SwData.cpu = OptValue; CompFlags.register_conventions = 1; }
static void SetCPU_xS( void )   { SwData.cpu = OptValue; CompFlags.register_conventions = 0; }
#endif
#if _CPU == 8086 || _CPU == 386
static void SetFPU( void )          { SwData.fpu = OptValue; }
static void Set_FPR( void )         { Stack87 = 4; }
static void Set_FPI87( void )       { SwData.fpt = SW_FPT_INLINE; }
static void Set_Emu( void )         { SwData.fpt = SW_FPT_EMU; }
static void Set_FPC( void )         { SwData.fpt = SW_FPT_CALLS; }
static void Set_FPD( void )         { TargetSwitches |= P5_DIVIDE_CHECK; }

static void SetMemoryModel( void )  { SwData.mem = OptValue; }
#endif

static void Set_BD( void )          { CompFlags.bd_switch_used = 1; GenSwitches |= DLL_RESIDENT_CODE; }
static void Set_BC( void )          { CompFlags.bc_switch_used = 1; }
static void Set_BG( void )          { CompFlags.bg_switch_used = 1; }
static void Set_BM( void )          { CompFlags.bm_switch_used = 1; }

#if _CPU != 8086
static void Set_BR( void )          { CompFlags.br_switch_used = 1; }
#endif

static void Set_BW( void )          { CompFlags.bw_switch_used = 1; }
static void Set_BT( void )          { SetTargName( OptParm,  OptScanPtr - OptParm ); }

static void SetExtendedDefines( void )
{
    CompFlags.extended_defines = 1;
    EnsureEndOfSwitch();
}
static void SetBrowserInfo( void )  { CompFlags.emit_browser_info = 1; }

#if _CPU == _AXP
static void Set_AS( void )
{
    TargetSwitches |= ALIGNED_SHORT;
}
#endif

static void Set_AA( void )          { CompFlags.auto_agg_inits = 1; }
static void Set_AI( void )          { CompFlags.no_check_inits = 1; }
static void Set_AQ( void )          { CompFlags.no_check_qualifiers = 1; }
static void Set_D0( void )
{
    GenSwitches &= ~(NUMBERS | DBG_TYPES | DBG_LOCALS | NO_OPTIMIZATION);
    CompFlags.debug_info_some = 0;
    CompFlags.no_debug_type_names = 0;
    EnsureEndOfSwitch();
}
static void Set_D1( void )
{
    GenSwitches |= NUMBERS;
    if( *OptScanPtr == '+' ) {
        ++OptScanPtr;
        CompFlags.debug_info_some = 1;
        GenSwitches |= DBG_TYPES | DBG_LOCALS;
    }
    EnsureEndOfSwitch();
}
static void Set_D2( void )
{
    GenSwitches |= NUMBERS | DBG_TYPES | DBG_LOCALS | NO_OPTIMIZATION;
    CompFlags.inline_functions = 0;
    if( *OptScanPtr == '~' ) {
        ++OptScanPtr;
        CompFlags.no_debug_type_names = 1;
    }
    EnsureEndOfSwitch();
}
static void Set_D3( void )
{
    CompFlags.dump_types_with_names = 1;
    Set_D2();
}
static void Set_D9( void )          { CompFlags.use_full_codegen_od = 1; }
static void DefineMacro( void )     { OptScanPtr = Define_UserMacro( OptScanPtr ); }

static void SetErrorLimit( void )   { ErrLimit = OptValue; }

#if _CPU == 8086 || _CPU == 386
static void SetDftCallConv( void )
{
    switch( OptValue ) {
    case 1:
        DftCallConv = &CdeclInfo;
        break;
    case 2:
        DftCallConv = &StdcallInfo;
        break;
    case 3:
        DftCallConv = &FastcallInfo;
        break;
    case 4:
        DftCallConv = &OptlinkInfo;
        break;
    case 5:
        DftCallConv = &PascalInfo;
        break;
    case 6:
        DftCallConv = &SyscallInfo;
        break;
    case 7:
        DftCallConv = &FortranInfo;
        break;
    case 8:
    default:
        DftCallConv = &WatcallInfo;
        break;
    }
}
static void Set_EC( void )          { CompFlags.ec_switch_used = 1; }
#endif

static void Set_EE( void )          { CompFlags.ee_switch_used = 1; }
static void Set_EF( void )          { CompFlags.ef_switch_used = 1; }
static void Set_EN( void )          { CompFlags.emit_names = 1; }
static void Set_EI( void )          { CompFlags.make_enums_an_int = 1;
                                      CompFlags.original_enum_setting = 1;}
static void Set_EM( void )          { CompFlags.make_enums_an_int = 0;
                                      CompFlags.original_enum_setting = 0;}

#if _CPU == 8086 || _CPU == 386
static void Set_ET( void )          { TargetSwitches |= P5_PROFILING; }
static void Set_ETP( void )         { TargetSwitches |= NEW_P5_PROFILING; }
static void Set_ESP( void )         { TargetSwitches |= STATEMENT_COUNTING; }
#endif

#if _CPU == 386
static void Set_EZ( void )          { TargetSwitches |= EZ_OMF; }
static void Set_OMF( void )         { TargetSwitches &= ~(OBJ_ELF | OBJ_COFF); }
#endif

#if /*_CPU == 386 || */_CPU == _AXP || _CPU == _PPC || _CPU == _MIPS
static void Set_ELF( void )         { GenSwitches &= ~OBJ_OWL;
                                      GenSwitches |= OBJ_ELF; }
static void Set_COFF( void )        { GenSwitches &= ~OBJ_OWL;
                                      GenSwitches |= OBJ_COFF; }
#endif
#if _CPU == _AXP || _CPU == _PPC || _CPU == _MIPS
static void Set_EndianLittle( void ) { GenSwitches &= ~OBJ_ENDIAN_BIG; }
static void Set_EndianBig( void )    { GenSwitches |= OBJ_ENDIAN_BIG; }
#endif

static void Set_EP( void )
{
    CompFlags.ep_switch_used = 1;
    ProEpiDataSize = OptValue;
}

static void StripQuotes( char *fname )
{
    char    *s;
    char    *d;

    if( *fname == '"' ) {
        // string will shrink so we can reduce in place
        d = fname;
        for( s = d + 1; *s && *s != '"'; ++s ) {
            // collapse double backslashes, only then look for escaped quotes
            if( s[0] == '\\' && s[1] == '\\' ) {
                ++s;
            } else if( s[0] == '\\' && s[1] == '"' ) {
                ++s;
            }
            *d++ = *s;
        }
        *d = '\0';
    }
}

static char *CopyOfParm( void )
{
    char        *p;
    size_t      len;

    len = OptScanPtr - OptParm;
    p = (char *)CMemAlloc( len + 1 );
    memcpy( p, OptParm, len );
    p[len] = '\0';
    return( p );
}

static char *GetAFileName( void )
{
    char    *fname;

    fname = CopyOfParm();
    StripQuotes( fname );
    return( fname );
}

static void Set_FH( void )
{
    if( OptParm == OptScanPtr ) {
        PCH_FileName = DEFAULT_PCH_NAME;
    } else {
        PCH_FileName = GetAFileName();
    }
}

static void Set_FHQ( void )
{
    CompFlags.no_pch_warnings = 1;
    Set_FH();
}

static void Set_FI( void )
{
    ForceInclude = GetAFileName();
}

static void Set_FLD( void )
{
    CompFlags.use_long_double = 1;
}

static void SetTrackInc( void )
{
    CompFlags.track_includes = 1;
}

static void Set_FO( void )
{
    CMemFree( ObjectFileName );
    ObjectFileName = GetAFileName();
    CompFlags.cpp_output_to_file = 1;   /* in case '-p' option */
}

static void Set_FR( void )
{
    CMemFree( ErrorFileName );
    ErrorFileName = GetAFileName();
    if( *ErrorFileName == '\0' ) {
        CMemFree( ErrorFileName );
        ErrorFileName = NULL;
    }
}

#if _CPU == 8086 || _CPU == 386
static void SetCodeClass( void )    { CodeClassName = CopyOfParm(); }
static void SetDataSegName( void )
{
    SwData.nd_used = 1;
    DataSegName = CopyOfParm();
    SegData = SEG_UNKNOWN - 1;
    if( *DataSegName == '\0' ) {
        CMemFree( DataSegName );
        DataSegName = NULL;
    }
}
static void SetTextSegName( void )  { TextSegName = CopyOfParm(); }
static void SetGroup( void )        { GenCodeGroup = CopyOfParm(); }
#endif
static void SetModuleName( void )   { ModuleName = CopyOfParm(); }

static void SetAPILogging( void )   { GenSwitches |= ECHO_API_CALLS; }

#ifndef NDEBUG
#ifdef ASM_OUTPUT
static void SetAsmListing( void )   { TargetSwitches |= ASM_OUTPUT; }
#endif
#ifdef OWL_LOGGING
static void SetOwlLogging( void )   { TargetSwitches |= OWL_LOGGING; }
#endif
#endif

static void SetInclude( void )
{
    char    *fname;

    fname = GetAFileName();
    AddIncList( fname );
    CMemFree( fname );
}

static void SetReadOnlyDir( void )
{
    char    *dirpath;

    dirpath = GetAFileName();
    SrcFileReadOnlyDir( dirpath );
    CMemFree( dirpath );
}

static void SetCharType( void )
{
    SetSignedChar();
    CompFlags.signed_char = 1;
}

#if _CPU == 8086 || _CPU == 386
static void Set_RE( void )          { CompFlags.rent = 1; }
static void Set_RI( void )          { CompFlags.returns_promoted = 1; }
static void Set_R( void )           { CompFlags.save_restore_segregs = 1; }
static void Set_SG( void )          { CompFlags.sg_switch_used = 1; }
static void Set_ST( void )          { CompFlags.st_switch_used = 1; }
#endif
#if _CPU == _AXP || _CPU == _MIPS
static void Set_SI( void )          { TargetSwitches |= STACK_INIT; }
#endif
static void Set_S( void )           { Toggles &= ~TOGGLE_CHECK_STACK; }

static void Set_TP( void )
{
    char    *togname;

    togname = CopyOfParm();
    SetToggleFlag( togname, TRUE );
    CMemFree( togname );
}

static void SetDataThreshHold( void ) { DataThreshold = OptValue; }

static void Set_U( void )
{
    char    *name;

    name = CopyOfParm();
    AddUndefName( name );
    CMemFree( name );
}
static void Set_V( void )           { CompFlags.generate_prototypes = 1; }

static void Set_WE( void )          { CompFlags.warnings_cause_bad_exit = 1; }
static void Set_WO( void )          { CompFlags.using_overlays = 1; }
static void Set_WX( void )          { WngLevel = 4; }
static void SetWarningLevel( void ) { WngLevel = OptValue; }
static void Set_WCD( void )         { EnableDisableMessage( 0, OptValue ); }
static void Set_WCE( void )         { EnableDisableMessage( 1, OptValue ); }

#if _CPU == 386
static void Set_XGV( void )         { TargetSwitches |= INDEXED_GLOBALS; }
#endif

static void Set_XBSA( void )
{
    CompFlags.unaligned_segs = 1;
}

#if _CPU == _AXP
static void Set_XD( void )          { TargetSwitches |= EXCEPT_FILTER_USED; }
#endif

static void Set_ZA99( void )
{
    CompFlags.c99_extensions = 1;
}

static void Set_ZA( void )
{
    CompFlags.extensions_enabled = 0;
    CompFlags.oldmacros_enabled = 0;
    CompFlags.unique_functions = 1;
    TargetSwitches &= ~I_MATH_INLINE;
}

static void SetStrictANSI( void )
{
    CompFlags.strict_ANSI = 1;
    Set_ZA();
}

static void Set_ZAM( void )
{
    CompFlags.oldmacros_enabled = 0;
}

#if _CPU == 8086 || _CPU == 386
static void Set_ZC( void )
{
    CompFlags.strings_in_code_segment = 1;
    CompFlags.zc_switch_used = 1;
    TargetSwitches |= CONST_IN_CODE;
}
static void Set_ZDF( void )         { SwData.peg_ds_used = 1; SwData.peg_ds_on = 0; }
static void Set_ZDP( void )         { SwData.peg_ds_used = 1; SwData.peg_ds_on = 1; }
static void Set_ZDL( void )         { TargetSwitches |= LOAD_DS_DIRECTLY; }
static void Set_ZFF( void )         { SwData.peg_fs_used = 1; SwData.peg_fs_on = 0; }
static void Set_ZFP( void )         { SwData.peg_fs_used = 1; SwData.peg_fs_on = 1; }
static void Set_ZGF( void )         { SwData.peg_gs_used = 1; SwData.peg_gs_on = 0; }
static void Set_ZGP( void )         { SwData.peg_gs_used = 1; SwData.peg_gs_on = 1; }
#endif
static void Set_ZE( void )
{
    CompFlags.extensions_enabled = 1;
    CompFlags.oldmacros_enabled = 1;
}
static void Set_ZG( void )
{
    CompFlags.generate_prototypes = 1;
    CompFlags.dump_prototypes     = 1;
}

static void Set_ZI( void )          { CompFlags.extra_stats_wanted = 1; }

static void Set_ZK( void )          { character_encoding = ENC_ZK; }
static void Set_ZK0( void )         { character_encoding = ENC_ZK0; }
static void Set_ZK1( void )         { character_encoding = ENC_ZK1; }
static void Set_ZK2( void )         { character_encoding = ENC_ZK2; }
static void Set_ZK3( void )         { character_encoding = ENC_ZK3; }
static void Set_ZKL( void )         { character_encoding = ENC_ZKL; }
static void Set_ZKU( void )
{
    character_encoding = ENC_ZKU;
    unicode_CP = OptValue;
}
static void Set_ZK0U( void )        { character_encoding = ENC_ZK0U; }

static void Set_ZL( void )          { CompFlags.emit_library_names = 0; }
static void Set_ZLF( void )         { CompFlags.emit_all_default_libs = 1; }
static void Set_ZLD( void )         { CompFlags.emit_dependencies = 0; }
static void Set_ZLS( void )         { CompFlags.emit_targimp_symbols = 0; }
static void Set_ZEV( void )         { CompFlags.unix_ext = 1; }
static void Set_ZM( void )
{
    CompFlags.multiple_code_segments = 1;
    CompFlags.zm_switch_used = 1;
}
static void Set_ZPW( void )         { CompFlags.slack_byte_warning = 1; }

#if _CPU == 8086 || _CPU == 386
static void Set_ZRO( void )
{
    GenSwitches |= FPU_ROUNDING_OMIT;
    GenSwitches &= ~FPU_ROUNDING_INLINE;
}
#endif

#if _CPU == 386
static void Set_ZRI( void )
{
    GenSwitches |= FPU_ROUNDING_INLINE;
    GenSwitches &= ~FPU_ROUNDING_OMIT;
}
#endif

static void Set_ZQ( void )          { CompFlags.quiet_mode = 1; }
static void Set_ZS( void )          { CompFlags.check_syntax = 1; }

#if _CPU == 8086 || _CPU == 386
static void Set_EQ( void )          { CompFlags.no_conmsg  = 1; }

static void Set_ZFW( void )
{
    TargetSwitches |= GEN_FWAIT_386;
}

static void Set_ZU( void )
{
    CompFlags.zu_switch_used = 1;
    TargetSwitches |= FLOATING_SS;
}

#if _CPU == 386
static void Set_ZZ( void )
{
    CompFlags.use_stdcall_at_number = 0;
}
#endif

#if _CPU == 8086
static void ChkSmartWindows( void )
{
    if( tolower( *(unsigned char *)OptScanPtr ) == 's' ) {
        TargetSwitches |= SMART_WINDOWS;
        ++OptScanPtr;
    }
    EnsureEndOfSwitch();
}

static void SetCheapWindows( void )
{
    _SetConstTarg( "cheap_windows" );
    ChkSmartWindows();
}
#endif

static void SetWindows( void )
{
    _SetConstTarg( "windows" );
#if _CPU == 8086
    ChkSmartWindows();
#endif
}
#endif

static void SetGenerateMakeAutoDepend( void )
{
    CompFlags.generate_auto_depend = 1;
    CMemFree( DependFileName );
    DependFileName = GetAFileName();
    if( !DependFileName[0] )
    {
        CMemFree( DependFileName );
        DependFileName = NULL;
    }
}

static void SetAutoDependTarget( void )
{
   // auto set depend yes...
    CompFlags.generate_auto_depend = 1;
    CMemFree( DependTarget );
    DependTarget = GetAFileName();
}

static void SetAutoDependSrcDepend( void )
{
    CompFlags.generate_auto_depend = 1;
    CMemFree( SrcDepName );
    SrcDepName = GetAFileName();
}

static void SetAutoDependHeaderPath( void )
{
    CompFlags.generate_auto_depend = 1;
    CMemFree( DependHeaderPath );
    DependHeaderPath = GetAFileName();
}

static void SetAutoDependForeSlash( void )
{
    DependForceSlash = '/';
}

static void SetAutoDependBackSlash( void )
{
    DependForceSlash = '\\';
}

static void Set_X( void )           { CompFlags.cpp_ignore_env = 1; }
static void Set_XX( void )          { CompFlags.ignore_default_dirs = 1; }
static void Set_PIL( void )         { CompFlags.cpp_ignore_line = 1; }
static void Set_PL( void )          { CompFlags.cpp_line_wanted = 1; }
static void Set_NA( void )          { CompFlags.disable_ialias = 1; }
static void Set_PC( void )
{
    CompFlags.keep_comments = 1;
    CompFlags.comments_wanted = 1;
}
static void Set_PW( void )
{
    if( OptValue != 0 && OptValue < 20 )
        OptValue = 20;
    if( OptValue > 10000 )
        OptValue = 10000;
    SetCppWidth( OptValue );
}
static void Set_PreProcChar( void ) { PreProcChar = *OptScanPtr++; }

static void Set_OA( void )          { GenSwitches |= RELAX_ALIAS; }
static void Set_OB( void )          { GenSwitches |= BRANCH_PREDICTION; }
static void Set_OD( void )          { GenSwitches |= NO_OPTIMIZATION; }
static void Set_OE( void )
{
    Inline_Threshold = OptValue;
    Toggles |= TOGGLE_INLINE;
}

#if _CPU == 8086 || _CPU == 386
static void Set_OC( void )          { TargetSwitches |= NO_CALL_RET_TRANSFORM; }
static void Set_OF( void )
{
    TargetSwitches |= NEED_STACK_FRAME;
    if( OptValue != 0 ) {
        WatcallInfo.cclass |= GENERATE_STACK_FRAME;
    }
}
static void Set_OM( void )          { TargetSwitches |= I_MATH_INLINE; }
static void Set_OP( void )          { CompFlags.op_switch_used = 1; } // force floats to memory
#endif
static void Set_OH( void )          { GenSwitches |= SUPER_OPTIMAL; }
static void Set_OK( void )          { GenSwitches |= FLOW_REG_SAVES; }
static void Set_OI( void )          { CompFlags.inline_functions = 1; }
static void Set_OL( void )          { GenSwitches |= LOOP_OPTIMIZATION; }
static void Set_OL_plus( void )     { GenSwitches |= LOOP_OPTIMIZATION | LOOP_UNROLLING; }
static void Set_ON( void )          { GenSwitches |= FP_UNSTABLE_OPTIMIZATION; }
static void Set_OO( void )          { GenSwitches &= ~MEMORY_LOW_FAILS; }
static void Set_OR( void )          { GenSwitches |= INS_SCHEDULING; }
static void Set_OS( void )          { GenSwitches &= ~NO_OPTIMIZATION; OptSize = 100; }
static void Set_OT( void )          { GenSwitches &= ~NO_OPTIMIZATION; OptSize = 0; }
static void Set_OU( void )          { CompFlags.unique_functions = 1; }
static void Set_OX( void )
{
    Toggles &= ~TOGGLE_CHECK_STACK;
    GenSwitches &= ~NO_OPTIMIZATION;
    GenSwitches |= LOOP_OPTIMIZATION | INS_SCHEDULING | BRANCH_PREDICTION;
    CompFlags.inline_functions = 1;
    OptValue = 20; // Otherwise we effectively disable inlining!
    Set_OE();
#if _CPU == 8086 || _CPU == 386
    TargetSwitches |= I_MATH_INLINE;
#endif
}
static void Set_OZ( void )          { GenSwitches |= NULL_DEREF_OK; }

// '=' indicates optional '='
// '#' indicates a decimal numeric value
// '$' indicates identifier
// '@' indicates filename
// '*' indicates additional characters will be scanned by option routine
// if a capital letter appears in the option, then input must match exactly
// otherwise all input characters are changed to lower case before matching

static struct option const Optimization_Options[] = {
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

static struct option const Preprocess_Options[] = {
    { "c",      0,              Set_PC },
    { "l",      0,              Set_PL },
    { "w=#",    0,              Set_PW },
    { "=",      0,              Set_PreProcChar },
    { "#",      0,              Set_PreProcChar },
    { 0,        0,              0 },
};

static void SetOptimization( void );
static void SetPreprocessOptions( void );
static struct option const CFE_Options[] = {
    { "o*",     0,              SetOptimization },
    { "i=@",    0,              SetInclude },
    { "zq",     0,              Set_ZQ },
    { "q",      0,              Set_ZQ },
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
    // more specific commands first ... otherwise the
    // short command sets us up for failure...
    { "adt=@",  0,              SetAutoDependTarget },
    { "adbs",   0,              SetAutoDependBackSlash },
    { "add=@",  0,              SetAutoDependSrcDepend },
    { "adfs",   0,              SetAutoDependForeSlash },
    { "adhp=@", 0,              SetAutoDependHeaderPath },
    { "ad=@",   0,              SetGenerateMakeAutoDepend },
    { "ai",     0,              Set_AI },
    { "aq",     0,              Set_AQ },
#if _CPU == _AXP
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
    { "ecc",    1,              SetDftCallConv },
    { "ecd",    2,              SetDftCallConv },
    { "ecf",    3,              SetDftCallConv },
    { "eco",    4,              SetDftCallConv },
    { "ecp",    5,              SetDftCallConv },
    { "ecs",    6,              SetDftCallConv },
    { "ecr",    7,              SetDftCallConv },
    { "ecw",    8,              SetDftCallConv },
    { "ec",     0,              Set_EC },
    { "et",     0,              Set_ET },
    { "eq",     0,              Set_EQ },
    { "etp",    0,              Set_ETP },
    { "esp",    0,              Set_ESP },
#endif
#if /*_CPU == 386 ||*/ _CPU == _AXP || _CPU == _PPC || _CPU == _MIPS
    { "eoe",    0,              Set_ELF },
    { "eoc",    0,              Set_COFF },
#endif
#if _CPU == _AXP || _CPU == _PPC || _CPU == _MIPS
    { "el",     0,              Set_EndianLittle },
    { "eb",     0,              Set_EndianBig },
#endif
#if _CPU == 386
    { "eoo",    0,              Set_OMF },
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
    { "na",     0,              Set_NA },
#if _CPU == 8086 || _CPU == 386
    { "nc=$",   0,              SetCodeClass },
    { "nd=$",   0,              SetDataSegName },
    { "nt=$",   0,              SetTextSegName },
#endif
    { "nm=$",   0,              SetModuleName },
    { "pil",    0,              Set_PIL },
    { "p*",     0,              SetPreprocessOptions },
    { "rod=@",  0,              SetReadOnlyDir },
#if _CPU == 8086 || _CPU == 386
    { "re",     0,              Set_RE },
    { "ri",     0,              Set_RI },
    { "r",      0,              Set_R },
    { "sg",     0,              Set_SG },
    { "st",     0,              Set_ST },
#endif
#if _CPU == _AXP || _CPU == _MIPS
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
    { "fld",    0,              Set_FLD },
    { "fo=@",   0,              Set_FO },
    { "fr=@",   0,              Set_FR },
    { "fti",    0,              SetTrackInc },
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
    { "x",      0,              Set_X },
#if _CPU == 386
    { "xgv",    0,              Set_XGV },
#endif
    { "xbsa",   0,              Set_XBSA },
#if _CPU == _AXP
    { "xd",     0,              Set_XD },
#endif
    { "xx",     0,              Set_XX },
    { "za99",   0,              Set_ZA99 },
    { "zam",    0,              Set_ZAM },
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
#if _CPU == 8086 || _CPU == 386
    { "zfw",    0,              Set_ZFW },
#endif
    { "zg",     0,              Set_ZG },
    { "zi",     0,              Set_ZI },
    { "zk0u",   0,              Set_ZK0U },
    { "zk0",    0,              Set_ZK0 },
    { "zk1",    0,              Set_ZK1 },
    { "zk2",    0,              Set_ZK2 },
    { "zk3",    0,              Set_ZK3 },
    { "zkl",    0,              Set_ZKL },
    { "zku=#",  0,              Set_ZKU },
    { "zk",     0,              Set_ZK },
    { "zld",    0,              Set_ZLD },
    { "zlf",    0,              Set_ZLF },
    { "zls",    0,              Set_ZLS },
    { "zl",     0,              Set_ZL },
    { "zm",     0,              Set_ZM },
    { "zpw",    0,              Set_ZPW },
    { "zp=#",   1,              Set_ZP },
#if _CPU == _AXP
    { "zps",    0,              SetStructPack },
#endif
#if _CPU == 8086 || _CPU == 386
    { "zro",    0,              Set_ZRO },
#endif
#if _CPU == 386
    { "zri",    0,              Set_ZRI },
#endif
    { "zs",     0,              Set_ZS },
    { "zt=#",   256,            SetDataThreshHold },
#if _CPU == 8086 || _CPU == 386
    { "zu",     0,              Set_ZU },
#endif
    { "zev",    0,              Set_ZEV },
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

static const char *ProcessOption( struct option const *op_table, const char *p, const char *option_start )
{
    int         i;
    int         j;
    char        *opt;
    char        c;

    for( i = 0; (opt = op_table[i].option) != NULL; i++ ) {
        c = tolower( *(unsigned char *)p );
        if( c == *opt ) {
            OptValue = op_table[i].value;
            j = 1;
            for( ;; ) {
                ++opt;
                if( *opt == '\0' || *opt == '*' ) {
                    if( *opt == '\0' ) {
                        if( p - option_start == 1 ) {
                            // make sure end of option
                            if( !OptionDelimiter( p[j] ) ) {
                                break;
                            }
                        }
                    }
                    OptScanPtr = p + j;
                    op_table[i].function();
                    return( OptScanPtr );
                }
                if( *opt == '#' ) {             // collect a number
                    if( p[j] >= '0' && p[j] <= '9' ) {
                        OptValue = 0;
                        for( ;; ) {
                            c = p[j];
                            if( c < '0' || c > '9' )
                                break;
                            OptValue = OptValue * 10 + c - '0';
                            ++j;
                        }
                    }
                } else if( *opt == '$' ) {      // collect an identifer
                    OptParm = &p[j];
                    for( ; (c = p[j]) != '\0'; ) {
                        if( c == '-' )
                            break;
                        if( c == ' ' )
                            break;
                        if( c == SwitchChar )
                            break;
                        ++j;
                    }
                } else if( *opt == '@' ) {      // collect a filename
                    OptParm = &p[j];
                    c = p[j];
                    if( c == '"' ) { // "filename"
                        for( ;; ) {
                            c = p[++j];
                            if( c == '"' ) {
                                ++j;
                                break;
                            }
                            if( c == '\0' )
                                break;
                            if( c == '\\' ) {
                                ++j;
                            }
                        }
                    } else {
                        for( ; (c = p[j]) != '\0'; ) {
                            if( c == ' ' )
                                break;
                            if( c == '\t' )
                                break;
#if !defined( __UNIX__ )
                            if( c == SwitchChar )
                                break;
#endif
                            ++j;
                        }
                    }
                } else if( *opt == '=' ) {      // collect an optional '='
                    if( p[j] == '=' || p[j] == '#' ) {
                        ++j;
                    }
                } else {
                    c = tolower( (unsigned char)p[j] );
                    if( *opt != c ) {
                        if( *opt < 'A' || *opt > 'Z' )
                            break;
                        if( *opt != p[j] ) {
                            break;
                        }
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

static void ProcessSubOption( struct option const *op_table )
{
    const char  *option_start;

    option_start = OptScanPtr - 2;
    for( ;; ) {
        OptScanPtr = ProcessOption( op_table, OptScanPtr, option_start );
        if( OptionDelimiter( *OptScanPtr ) ) {
            break;
        }
    }
}

static void SetOptimization( void )
{
    ProcessSubOption( Optimization_Options );
}

static void SetPreprocessOptions( void )
{
    CompFlags.cpp_output_requested = 1;
    if( !OptionDelimiter( *OptScanPtr ) ) {
        ProcessSubOption( Preprocess_Options );
    }
}

static const char *CollectEnvOrFileName( const char *str )
{
    char        *env;
    char        ch;

    while( *str == ' ' || *str == '\t' )
        ++str;
    env = TokenBuf;
    for( ; (ch = *str) != '\0'; ) {
        ++str;
        if( ch == ' ' )
            break;
        if( ch == '\t' )
            break;
#if ! defined( __UNIX__ )
        if( ch == '-' )
            break;
        if( ch == SwitchChar )
            break;
#endif
        *env++ = ch;
    }
    *env = '\0';
    return( str );
}

static char *ReadIndirectFile( void )
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
        while( (ch = *str) != '\0' ) {
            if( ch == '\r' || ch == '\n' ) {
                *str = ' ';
            }
#if !defined( __UNIX__ )
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

local void ProcOptions( const char *str )
{
    unsigned    level;
    const char  *save[MAX_NESTING];
    char        *buffers[MAX_NESTING];
    char        *ptr;

    if( str != NULL ) {
        level = 0;
        buffers[0] = NULL;
        for( ;; ) {
            while( *str == ' ' || *str == '\t' )
                ++str;
            if( *str == '@' && level < MAX_NESTING ) {
                save[level] = CollectEnvOrFileName( str + 1 );
                ++level;
                buffers[level] = NULL;
                ptr = FEGetEnv( TokenBuf );
                if( ptr == NULL ) {
                    ptr = ReadIndirectFile();
                    buffers[level] = ptr;
                }
                if( ptr != NULL ) {
                    str = ptr;
                    continue;
                }
                str = save[--level];
            }
            if( *str == '\0' ) {
                if( level == 0 )
                    break;
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
                const char *beg;
                char *p;
                size_t len;

                beg = str;
                if( *str == '"' ) {
                    for( ;; ) {
                        ++str;
                        if( *str == '"' ) {
                            ++str;
                            break;
                        }
                        if( *str == '\0' )
                            break;
                        if( *str == '\\' ) {
                            ++str;
                        }
                    }
                } else {
                    for( ; *str != '\0'; ) {
                        if( *str == ' '  )
                            break;
                        if( *str == '\t'  )
                            break;
#if ! defined( __UNIX__ )
                        if( *str == SwitchChar )
                            break;
#endif
                        ++str;
                    }
                }
                len = str - beg;
                p = (char *)CMemAlloc( len + 1 );
                memcpy( p, beg, len );
                p[len] = '\0';
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

static void InitCPUModInfo( void )
{
    CodeClassName = NULL;
    PCH_FileName  = NULL;
    TargetSwitches = 0;
    TargSys = TS_OTHER;
#if _CPU == _AXP || _CPU == _PPC || _CPU == _SPARC || _CPU == _MIPS
    TextSegName   = ".text";
    DataSegName   = ".data";
    GenCodeGroup  = "";
    DataPtrSize   = TARGET_POINTER;
    CodePtrSize   = TARGET_POINTER;
    GenSwitches   = MEMORY_LOW_FAILS;
  #if _CPU == _AXP
    GenSwitches  |= OBJ_COFF;
  #else
    GenSwitches  |= OBJ_ELF;
  #endif
#elif _CPU == 386 || _CPU == 8086
    Stack87 = 8;
    TextSegName   = "";
    DataSegName   = "";
    GenCodeGroup  = "";
    CompFlags.register_conv_set = 0;
    CompFlags.register_conventions = 1;
    GenSwitches = MEMORY_LOW_FAILS;
#else
    #error InitCPUModInfo not configured for system
#endif
}

local void Define_Memory_Model( void )
{
#if _CPU == 8086 || _CPU == 386
    char        model;
#endif

    DataPtrSize = TARGET_POINTER;
    CodePtrSize = TARGET_POINTER;
#if _CPU == 8086 || _CPU == 386
    switch( SwData.mem ) {
    case SW_MF:
        model = 's';
        TargetSwitches &= ~CONST_IN_CODE;
        break;
    case SW_MS:
        model = 's';
        CompFlags.strings_in_code_segment = 0;
        TargetSwitches &= ~CONST_IN_CODE;
        break;
    case SW_MM:
        model = 'm';
        WatcallInfo.cclass |= FAR_CALL;
        CompFlags.strings_in_code_segment = 0;
        TargetSwitches &= ~CONST_IN_CODE;
        CodePtrSize = TARGET_FAR_POINTER;
        break;
    case SW_MC:
        model = 'c';
        DataPtrSize = TARGET_FAR_POINTER;
        break;
    case SW_ML:
        model = 'l';
        WatcallInfo.cclass |= FAR_CALL;
        CodePtrSize = TARGET_FAR_POINTER;
        DataPtrSize = TARGET_FAR_POINTER;
        break;
    case SW_MH:
        model = 'h';
        WatcallInfo.cclass |= FAR_CALL;
        CodePtrSize = TARGET_FAR_POINTER;
        DataPtrSize = TARGET_FAR_POINTER;
        break;
    default:
        model = '?';
        break;
    }
#endif
#if _CPU == 8086
    CPYLIT( CLIB_Name, "1clib?" );
    if( CompFlags.bm_switch_used ) {
        CPYLIT( CLIB_Name, "1clibmt?" );
    }
    if( CompFlags.bd_switch_used ) {
        if( TargSys == TS_WINDOWS ||
            TargSys == TS_CHEAP_WINDOWS ) {
            CPYLIT( CLIB_Name, "1clib?" );
        } else {
            CPYLIT( CLIB_Name, "1clibdl?" );
        }
    }
    if( GET_FPU_EMU( ProcRevision ) ) {
        CPYLIT( MATHLIB_Name, "7math87?" );
        EmuLib_Name = "8emu87";
    } else if( GET_FPU_LEVEL( ProcRevision ) == FPU_NONE ) {
        CPYLIT( MATHLIB_Name, "5math?" );
        EmuLib_Name = NULL;
    } else {
        CPYLIT( MATHLIB_Name, "7math87?" );
        EmuLib_Name = "8noemu87";
    }
#elif _CPU == 386
    model = 'r';
    if( ! CompFlags.register_conventions ) model = 's';
    if( CompFlags.br_switch_used ) {
        CPYLIT( CLIB_Name, "1clb?dll" );
    } else {
        CPYLIT( CLIB_Name, "1clib3?" );     // There is only 1 CLIB now!
    }
    if( GET_FPU_EMU( ProcRevision ) ) {
        if( CompFlags.br_switch_used ) {
            CPYLIT( MATHLIB_Name, "7mt7?dll" );
        } else {
            CPYLIT( MATHLIB_Name, "7math387?" );
        }
        EmuLib_Name = "8emu387";
    } else if( GET_FPU_LEVEL( ProcRevision ) == FPU_NONE ) {
        if( CompFlags.br_switch_used ) {
            CPYLIT( MATHLIB_Name, "5mth?dll" );
        } else {
            CPYLIT( MATHLIB_Name, "5math3?" );
        }
        EmuLib_Name = NULL;
    } else {
        if( CompFlags.br_switch_used ) {
            CPYLIT( MATHLIB_Name, "7mt7?dll" );
        } else {
            CPYLIT( MATHLIB_Name, "7math387?" );
        }
        EmuLib_Name = "8noemu387";
    }
#elif _CPU == _AXP || _CPU == _PPC || _CPU == _SPARC || _CPU == _MIPS
    if( CompFlags.br_switch_used ) {
        CPYLIT( CLIB_Name, "1clbdll" );
        CPYLIT( MATHLIB_Name, "7mthdll" );
    } else {
        CPYLIT( CLIB_Name, "1clib" );
        CPYLIT( MATHLIB_Name, "7math" );
    }
    EmuLib_Name = NULL;
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
    /* Add precision warning but disabled by default */
    EnableDisableMessage( 0, ERR_LOSE_PRECISION );
    /* Warning about non-prototype declarations is disabled by default
     * because Windows and OS/2 API headers use it
     */
    EnableDisableMessage( 0, ERR_OBSOLETE_FUNC_DECL );
    /* Warnings about calling functions with non-prototype declaration */
    /* Disabled at least until source tree is cleaned up. */
    EnableDisableMessage( 0, ERR_NONPROTO_FUNC_CALLED );
    EnableDisableMessage( 0, ERR_NONPROTO_FUNC_CALLED_INDIRECT );
    /* Warning about pointer truncation during cast is disabled by
     * default because it would cause too many build breaks right now
     * by correctly diagnosing broken code.
     */
    EnableDisableMessage( 0, ERR_CAST_POINTER_TRUNCATION );
    InitModInfo();
    InitCPUModInfo();
#if _CPU == 386
    ProcOptions( FEGetEnv( "WCC386" ) );
#elif _CPU == 8086
    ProcOptions( FEGetEnv( "WCC" ) );
#elif _CPU == _AXP
    ProcOptions( FEGetEnv( "WCCAXP" ) );
#elif _CPU == _PPC
    ProcOptions( FEGetEnv( "WCCPPC" ) );
#elif _CPU == _MIPS
    ProcOptions( FEGetEnv( "WCCMPS" ) );
#elif _CPU == _SPARC
    ProcOptions( FEGetEnv( "WCCSPC" ) );
#else
    #error Compiler environment variable not configured
#endif
    for( ;*cmdline != NULL; ++cmdline ) {
        ProcOptions( *cmdline );
    }
    if( CompFlags.cpp_output_requested )
        CompFlags.cpp_output = 1;
    if( CompFlags.cpp_output )
        CompFlags.quiet_mode = 1;
    CBanner();          /* print banner if -zq not specified */
    GblPackAmount = PackAmount;
    SetTargSystem();
    SetGenSwitches();
    SetCharacterEncoding();
    Define_Memory_Model();
#if _CPU == 8086 || _CPU == 386
    if( GET_CPU( ProcRevision ) < CPU_386 ) {
        /* issue warning message if /zf[f|p] or /zg[f|p] spec'd? */
        TargetSwitches &= ~(FLOATING_FS|FLOATING_GS);
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
  #if _CPU == 386
    if( ! CompFlags.register_conventions )
        SetAuxStackConventions();
  #endif
#endif
    MacroDefs();
    MiscMacroDefs();
}
