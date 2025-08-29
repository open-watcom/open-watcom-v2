/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
*       If you add an option, don't forget to change bld/cc/gml/options.gml  *
*       Also, don't forget to add a case in MacroDefs                        *
*       to predefine a __SW_xx macro                                         *
*                                                                            *
*****************************************************************************/


#include "cvars.h"
#include <ctype.h>
#include "wio.h"
#include "watcom.h"
#include "pdefn2.h"
#include "cgdefs.h"
#include "cgswitch.h"
#include "iopath.h"
#include "pathlist.h"
#include "cmdlnprs.gh"
#include "cmdlnprs.h"
#include "cmdscan.h"
#include "cmdlnsys.h"
#include "toggles.h"
#include "feprotos.h"


#ifdef DEVBUILD
    #define DbgNever()  (FEMessage( FEMSG_FATAL, "should never execute this" __location ))
#else
    #define DbgNever()
#endif

static const char   *switch_start = "";

static void BadCmdLine( int error_code )
/***************************************
 * SIGNAL CMD-LINE ERROR
 */
{
    char        buffer[128];
    size_t      len;

    for( CmdScanChar(); !CmdScanSwEnd(); CmdScanChar() ) {
        ;
    }
    len = CmdScanAddr() - switch_start;
    if( len > sizeof( buffer ) - 1 )
        len = sizeof( buffer ) - 1;
    strncpy( buffer, switch_start, len );
    buffer[len] = '\0';
    CErr2p( error_code, buffer );
}

// BAD CHAR DETECTED
void BadCmdLineChar( void )
{
    BadCmdLine( ERR_INVALID_OPTION );
}
// BAD ID DETECTED
void BadCmdLineId( void )
{
    BadCmdLine( ERR_INVALID_OPTION );
}
// BAD NUMBER DETECTED
void BadCmdLineNumber( void )
{
    BadCmdLine( ERR_INVALID_OPTION );
}
// BAD PATH DETECTED
void BadCmdLinePath( void )
{
    BadCmdLine( ERR_INVALID_OPTION );
}
// BAD FILE DETECTED
void BadCmdLineFile( void )
{
    BadCmdLine( ERR_INVALID_OPTION );
}
// BAD TEXT DETECTED
void BadCmdLineOption( void )
{
    BadCmdLine( ERR_INVALID_OPTION );
}

static bool debugOptionAfterOptOption( OPT_STORAGE *data )
{
    if( data->debug_info_timestamp > data->opt_level_timestamp ) {
        if( data->debug_info_timestamp > data->opt_size_time_timestamp ) {
            return( true );
        }
    }
    return( false );
}

char *SetStringOption( char **o, OPT_STRING **h )
/***********************************************/
{
    OPT_STRING *s;
    char *p;

    s = *h;
    p = NULL;
    if( s != NULL ) {
        if( s->data[0] != '\0' ) {
            p = CMemStrDup( s->data );
        }
        OPT_CLEAN_STRING( h );
    }
    if( o != NULL ) {
        CMemFree( *o );
        *o = p;
    }
    return( p );
}

static void reverseList( OPT_STRING **h )
{
    OPT_STRING *s;
    OPT_STRING *p;
    OPT_STRING *n;

    s = *h;
    *h = NULL;
    for( p = s; p != NULL; p = n ) {
        n = p->next;
        p->next = *h;
        *h = p;
    }
}

static void checkPacking( unsigned *p )
{
    *p = VerifyPackAmount( *p );
}

#if _INTEL_CPU
static void checkDataThreshold( unsigned *p )
{
    if( *p > TARGET_INT_MAX ) {
        *p = 256;
    }
}
#endif

static void checkOENumber( unsigned *p )
{
    /* unused parameters */ (void)p;
}

static bool checkSTD( unsigned *value )
{
    cstd_ver    cstd;

    cstd = STD_NONE;
    *value = 0;
    CmdRecogEquals();
    while( !CmdScanSwEnd() ) {
        if( CmdRecogChar( 'c' ) ) {
            if( CmdRecogChar( '8' ) && CmdRecogChar( '9' ) ) {
                cstd = STD_C89;
            } else if( CmdRecogChar( '9' ) && CmdRecogChar( '9' ) ) {
                cstd = STD_C99;
            } else if( CmdRecogChar( '1' ) ) {
                if( CmdRecogChar( '1' ) ) {
                    cstd = STD_C11;
                } else if( CmdRecogChar( '7' ) ) {
                    cstd = STD_C17;
                } else {
                    BadCmdLineChar();
                }
            } else if( CmdRecogChar( '2' ) && CmdRecogChar( '3' ) ) {
                cstd = STD_C23;
            } else {
                BadCmdLineChar();
            }
        } else {
            BadCmdLineChar();
        }
    }
    if( cstd != STD_NONE ) {
        *value = cstd;
        return( true );
    }
    return( false );
}

static void checkPrologSize( unsigned *p )
{
    *p = _RoundUp( *p, TARGET_UINT );
}

static void checkErrorLimit( unsigned *p )
{
    /* unused parameters */ (void)p;
}

static bool scanDefine( OPT_STRING **p )
{
    /* unused parameters */ (void)p;

    CmdScanLineInit( Define_UserMacro( CmdScanAddr(), CompFlags.extended_defines ) );
    switch_start = CmdScanAddr();
    return( true );
}

static bool scanDefinePlus( OPT_STRING **p )
{
    /* unused parameters */ (void)p;

    if( CmdScanSwEnd() ) {
        CompFlags.extended_defines = true;
    } else {
        CmdScanLineInit( Define_UserMacro( CmdScanAddr(), true ) );
        switch_start = CmdScanAddr();
    }
    return( true );
}

static bool scanUndefine( OPT_STRING **p )
{
    const char  *name;
    size_t      len;
    char        *mname;

    /* unused parameters */ (void)p;

    len = CmdScanId( &name );
    mname = ToStringDup( name, len );
    AddUndefMacro( mname );
    CMemFree( mname );
    return( true );
}

static void handleOptionEQ( OPT_STORAGE *data, bool value )
{
    /* unused parameters */ (void)data;

    CompFlags.eq_switch_used = value;
}

static unsigned verifyWarnLevel( unsigned level )
{
    if( level > WLEVEL_MAX ) {
        level = WLEVEL_MAX;
    }
    return( level );
}

static void checkWarnLevel( unsigned *p )
{
    *p = verifyWarnLevel( *p );
}

static unsigned verifyPPWidth( unsigned width )
{
    if( width != 0 ) {
        if( width < 20 ) {
            width = 20;
        } else if( width > 10000 ) {
            width = 10000;
        }
    }
    return( width );
}

static void checkPPWidth( unsigned *p )
{
    *p = verifyPPWidth( *p );
}

static void setMessageStatus( OPT_STRING *s, bool state )
{
    msg_codes   msgnum;

    while( s != NULL ) {
        if( GetMsgNum( s->data, &msgnum ) ) {
            WarnEnableDisable( state, msgnum );
        }
        s = s->next;
    }
}

void DefSwitchMacro( const char *str )
{
    char buff[64];

    PreDefine_Macro( strcpy( strcpy( buff, "__SW_" ) + 5, str ) - 5 );
}

static void AddIncList( const char *path_list )
{
    size_t      old_len;
    size_t      len;
    char        *old_list;
    char        *p;

    if( path_list != NULL
      && *path_list != '\0' ) {
        len = strlen( path_list );
        old_list = IncPathList;
        old_len = strlen( old_list );
        IncPathList = CMemAlloc( old_len + 1 + len + 1 );
        strcpy( IncPathList, old_list );
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

static void SetCharacterEncoding( OPT_STORAGE *data )
{
    CompFlags.jis_to_unicode = false;

    switch( data->char_set ) {
    case OPT_ENUM_char_set_zku:
        LoadUnicodeTable( data->zku_value );
        break;
    case OPT_ENUM_char_set_zk0u:
        CompFlags.jis_to_unicode = true;
        /* fall through */
    case OPT_ENUM_char_set_zk0:
        CompFlags.use_double_byte = true;
        SetDBChar( 0 );
        break;
    case OPT_ENUM_char_set_zk1:
        CompFlags.use_double_byte = true;
        SetDBChar( 1 );
        break;
    case OPT_ENUM_char_set_zk2:
        CompFlags.use_double_byte = true;
        SetDBChar( 2 );
        break;
    case OPT_ENUM_char_set_zk3:
        CompFlags.use_double_byte = true;
        SetDBChar( 3 );
        break;
    case OPT_ENUM_char_set_zkl:
        CompFlags.use_double_byte = true;
        SetDBChar( -1 );
        break;
    case OPT_ENUM_char_set_default:
        break;
    default:
        DbgNever();
        break;
    }
}

void SetTargetName( char **target_name, const char *name )
/********************************************************/
{
    if( *target_name != NULL ) {
        CMemFree( *target_name );
    }
    *target_name = CMemStrDup( name );
}

static void AnalyseAnyTargetOptions( OPT_STORAGE *data )
/******************************************************/
{
    /*
     * quickly do the quiet option so the banner can be printed
     */
    if( data->q
      || data->zq ) {
        CompFlags.quiet_mode = true;
    }
    switch( data->warn_level ) {
    case OPT_ENUM_warn_level_w:
        WngLevel = data->w_value;
        break;
    case OPT_ENUM_warn_level_wx:
        WngLevel = WLEVEL_WX;
        break;
    case OPT_ENUM_warn_level_default:
        WngLevel = WLEVEL_DEFAULT;
        break;
    default:
        DbgNever();
        break;
    }
    switch( data->file_83 ) {
    case OPT_ENUM_file_83_fx:
        CompFlags.check_truncated_fnames = false;
        break;
    case OPT_ENUM_file_83_ft:
    case OPT_ENUM_file_83_default:
        CompFlags.check_truncated_fnames = true;
        break;
    default:
        DbgNever();
        break;
    }
    switch( data->opt_level ) {
    case OPT_ENUM_opt_level_ox:  /* -ox => -obmiler -s */
        GenSwitches &= ~CGSW_GEN_NO_OPTIMIZATION;
        GenSwitches |= CGSW_GEN_BRANCH_PREDICTION;  // -ob
        GenSwitches |= CGSW_GEN_I_MATH_INLINE;      // -om
        data->oi = true;                            // -oi
        GenSwitches |= CGSW_GEN_LOOP_OPTIMIZATION;  // -ol
        Inline_Threshold = 20;                      // -oe=20
        TOGGLE( inline ) = true;                    // ...
        GenSwitches |= CGSW_GEN_INS_SCHEDULING;     // -or
        TOGGLE( check_stack ) = false;              // -s
        break;
    case OPT_ENUM_opt_level_od:
        GenSwitches |= CGSW_GEN_NO_OPTIMIZATION;
        break;
    case OPT_ENUM_opt_level_default:
        break;
    default:
        DbgNever();
        break;
    }
    switch( data->opt_size_time ) {
    case OPT_ENUM_opt_size_time_ot:
        OptSize = 0;
        GenSwitches &= ~CGSW_GEN_NO_OPTIMIZATION;
        break;
    case OPT_ENUM_opt_size_time_os:
        OptSize = 100;
        GenSwitches &= ~CGSW_GEN_NO_OPTIMIZATION;
        break;
    case OPT_ENUM_opt_size_time_default:
        OptSize = 50;
        break;
    default:
        DbgNever();
        break;
    }
    switch( data->cstd ) {
    case OPT_ENUM_cstd_za99:
        CompVars.cstd = STD_C99;
        break;
    case OPT_ENUM_cstd_zastd:
        if( data->zastd_value != STD_NONE )
            CompVars.cstd = data->zastd_value;
        break;
    case OPT_ENUM_cstd_default:
        break;
    default:
        DbgNever();
        break;
    }
    switch( data->iso ) {
    case OPT_ENUM_iso_zA:
        CompFlags.strict_ANSI = true;
        /* fall through */
    case OPT_ENUM_iso_za:
        CompFlags.extensions_enabled = false;
        CompFlags.non_iso_compliant_names_enabled = false;
        CompFlags.unique_functions = true;
        break;
    case OPT_ENUM_iso_ze:
    case OPT_ENUM_iso_default:
        CompFlags.extensions_enabled = true;
        CompFlags.non_iso_compliant_names_enabled = true;
        break;
    default:
        DbgNever();
        break;
    }
    // following must precede processing of data->oe
    switch( data->debug_info ) {
    case OPT_ENUM_debug_info_d9:
        CompFlags.use_full_codegen_od = true;
        break;
    case OPT_ENUM_debug_info_d3:
        GenSwitches |= CGSW_GEN_DBG_NUMBERS | CGSW_GEN_DBG_TYPES | CGSW_GEN_DBG_LOCALS;
        CompFlags.dump_types_with_names = true;
        if( debugOptionAfterOptOption( data ) ) {
            GenSwitches |= CGSW_GEN_NO_OPTIMIZATION;
//            data->oe = false;
//            TOGGLE( inline ) = false;
        }
        GenSwitches |= CGSW_GEN_NO_OPTIMIZATION;
        data->oi = false;
        break;
    case OPT_ENUM_debug_info_d2_tilde:
        CompFlags.no_debug_type_names = true;
        /* fall through */
    case OPT_ENUM_debug_info_d2:
        GenSwitches |= CGSW_GEN_DBG_NUMBERS | CGSW_GEN_DBG_TYPES | CGSW_GEN_DBG_LOCALS;
        if( debugOptionAfterOptOption( data ) ) {
//            data->oe = false;
//            TOGGLE( inline ) = false;
//            data->oi = false;
        }
        GenSwitches |= CGSW_GEN_NO_OPTIMIZATION;
        data->oi = false;
        break;
    case OPT_ENUM_debug_info_d1_plus:
        GenSwitches |= CGSW_GEN_DBG_NUMBERS | CGSW_GEN_DBG_TYPES | CGSW_GEN_DBG_LOCALS;
        CompFlags.debug_info_some = true;
        break;
    case OPT_ENUM_debug_info_d1:
        GenSwitches |= CGSW_GEN_DBG_NUMBERS;
        break;
    case OPT_ENUM_debug_info_d0:
        break;
    case OPT_ENUM_debug_info_default:
        break;
    default:
        DbgNever();
        break;
    }

    switch( data->enum_size ) {
    case OPT_ENUM_enum_size_ei:
        CompFlags.make_enums_an_int = true;
        CompFlags.original_enum_setting = true;
        break;
    case OPT_ENUM_enum_size_em:
        CompFlags.make_enums_an_int = false;
        CompFlags.original_enum_setting = false;
        break;
    case OPT_ENUM_enum_size_default:
        /* default set in CmdSysInit() */
        break;
    default:
        DbgNever();
        break;
    }

    if( data->aa ) {
        CompFlags.auto_agg_inits = true;
    }
    if( data->ad ) {
        SetStringOption( &DependFileName, &(data->ad_value) );
        CompFlags.generate_auto_depend = true;
    }
    if( data->adbs ) {
        DependForceSlash = '\\';
    }
    if( data->add ) {
        SetStringOption( &SrcDepName, &(data->add_value) );
        CompFlags.generate_auto_depend = true;
    }
    if( data->adfs ) {
        DependForceSlash = '/';
    }
    if( data->adhp ) {
        SetStringOption( &DependHeaderPath, &(data->adhp_value) );
        CompFlags.generate_auto_depend = true;
    }
    if( data->adt ) {
        SetStringOption( &DependTarget, &(data->adt_value) );
        CompFlags.generate_auto_depend = true;
    }
    if( data->ai ) {
        CompFlags.no_check_inits = true;
    }
    if( data->aq ) {
        CompFlags.no_check_qualifiers = true;
    }
    if( data->bc ) {
        CompFlags.bc_switch_used = true;
    }
    if( data->bd ) {
        CompFlags.bd_switch_used = true;
        GenSwitches |= CGSW_GEN_DLL_RESIDENT_CODE;
    }
    if( data->bg ) {
        CompFlags.bg_switch_used = true;
    }
    if( data->bm ) {
        CompFlags.bm_switch_used = true;
    }
    if( data->bw ) {
        CompFlags.bw_switch_used = true;
    }
    if( data->db ) {
        CompFlags.emit_browser_info = true;
    }
    if( data->e ) {
        ErrLimit = data->e_value;
    }
    if( data->ef ) {
        CompFlags.ef_switch_used = true;
    }
    if( data->en ) {
        CompFlags.emit_names = true;
    }
    if( data->ep ) {
        CompFlags.ep_switch_used = true;
        ProEpiDataSize = data->ep_value;
    }
    if( data->fh || data->fhq ) {
        if( data->fhq ) {
            CompFlags.no_pch_warnings = true;
        }
        if( data->fh && ( !data->fhq || data->fh_timestamp > data->fhq_timestamp ) ) {
            SetStringOption( &PCH_FileName, &(data->fh_value) );
            OPT_CLEAN_STRING( &(data->fhq_value) );
        } else if( data->fhq && ( !data->fh || data->fhq_timestamp > data->fh_timestamp ) ) {
            SetStringOption( &PCH_FileName, &(data->fhq_value) );
            OPT_CLEAN_STRING( &(data->fh_value) );
        }
    }
    if( data->fi ) {
        SetStringOption( &ForceInclude, &(data->fi_value) );
    }
    if( data->fip ) {
        SetStringOption( &ForcePreInclude, &(data->fip_value) );
    }
    if( data->fo ) {
        SetStringOption( &ObjectFileName, &(data->fo_value) );
        CompFlags.cpp_output_to_file = true;    /* in case '-p' option */
    }
    if( data->fr ) {
        SetStringOption( &ErrorFileName, &(data->fr_value) );
    }
    if( data->fti ) {
        CompFlags.track_includes = true;
    }
    if( data->i ) {
        OPT_STRING *s;
        reverseList( &(data->i_value) );
        for( s = data->i_value; s != NULL; s = s->next ) {
            if( s->data[0] != '\0' ) {
                AddIncList( s->data );
            }
        }
    }
    if( data->j ) {
        SetSignedChar();
        CompFlags.signed_char = true;
    }
#ifdef DEVBUILD
    if( data->lc ) {
        GenSwitches |= CGSW_GEN_ECHO_API_CALLS;
    }
#endif
    if( data->oa ) {
        GenSwitches |= CGSW_GEN_RELAX_ALIAS;
    }
    if( data->ob ) {
        GenSwitches |= CGSW_GEN_BRANCH_PREDICTION;
    }
    // following must follow processing of debug options
    if( data->oe ) {
        Inline_Threshold = data->oe_value;
        TOGGLE( inline ) = true;
    }
    if( data->oh ) {
        GenSwitches |= CGSW_GEN_SUPER_OPTIMAL;
    }
    if( data->oi ) {
        CompFlags.inline_functions = true;
    }
    if( data->ok ) {
        GenSwitches |= CGSW_GEN_FLOW_REG_SAVES;
    }
    if( data->ol ) {
        GenSwitches |= CGSW_GEN_LOOP_OPTIMIZATION;
    }
    if( data->ol_plus ) {
        GenSwitches |= CGSW_GEN_LOOP_OPTIMIZATION | CGSW_GEN_LOOP_UNROLLING;
    }
    if( data->on ) {
        GenSwitches |= CGSW_GEN_FP_UNSTABLE_OPTIMIZATION;
    }
    if( data->oo ) {
        GenSwitches &= ~CGSW_GEN_MEMORY_LOW_FAILS;
    }
    if( data->or ) {
        GenSwitches |= CGSW_GEN_INS_SCHEDULING;
    }
    if( data->ou ) {
        CompFlags.unique_functions = true;
    }
    if( data->oz ) {
        GenSwitches |= CGSW_GEN_NULL_DEREF_OK;
    }
    if( data->p ) {
        CompFlags.cpp_mode = true;
    }
    if( data->p_sharp ) {
        PreProcChar = (char)data->p_sharp_value;
    }
    if( data->pil ) {
        CompFlags.cpp_ignore_line = true;
    }
    if( data->pl ) {
        CompFlags.cpp_mode = true;
        CompFlags.cpp_line_wanted = true;
    }
    if( data->pc ) {
        CompFlags.cpp_mode = true;
        CompFlags.cpp_keep_comments = true;
    }
    if( data->pw ) {
        CompFlags.cpp_mode = true;
        SetPPWidth( data->pw_value );
    } else {
        /*
         * #line directives get screwed by wrapped lines but we don't want
         * to interfere with a user's setting of the width
         */
        if( data->pl ) {
            SetPPWidth( 0 );
        }
    }
    if( CompFlags.cpp_mode ) {
        CompFlags.cpp_output = true;
        CompFlags.quiet_mode = true;
    }
    if( data->rod ) {
        OPT_STRING *s;
        for( s = data->rod_value; s != NULL; s = s->next ) {
            SrcFileReadOnlyDir( s->data );
        }
    }
    if( data->s ) {
        TOGGLE( check_stack ) = false;
    }
    if( data->v ) {
        CompFlags.dump_prototypes = true;
    }
    if( data->wcd ) {
        setMessageStatus( data->wcd_value, false );
    }
    if( data->wce ) {
        setMessageStatus( data->wce_value, true );
    }
    if( data->we ) {
        CompFlags.warnings_cause_bad_exit = true;
    }
    if( data->wpx ) {
        Check_global_prototype = true;
    }
    if( data->x ) {
        CompFlags.cpp_ignore_env = true;
    }
    if( data->xbsa ) {
        CompFlags.unaligned_segs = true;
    }
    if( data->xx ) {
        CompFlags.ignore_default_dirs = true;
    }
    if( data->zam ) {
        CompFlags.non_iso_compliant_names_enabled = false;
    }
    if( data->zev ) {
        CompFlags.unix_ext = true;
    }
    if( data->zg ) {
        CompFlags.generate_prototypes = true;
        CompFlags.dump_prototypes = true;
    }
    if( data->zl ) {
        CompFlags.emit_library_names = false;
    }
    if( data->zld ) {
        CompFlags.emit_dependencies = false;
    }
    if( data->zlf ) {
        CompFlags.emit_all_default_libs = true;
    }
    if( data->zls ) {
        CompFlags.emit_targimp_symbols = false;
    }
    if( data->zp ) {
        PackAmount = data->zp_value;
        GblPackAmount = PackAmount;
    }
    if( data->zpw ) {
        CompFlags.slack_byte_warning = true;
    }
    if( data->zs ) {
        CompFlags.check_syntax = true;
    }
#ifdef DEVBUILD
    if( data->tp ) {
        OPT_STRING *str;
        while( (str = data->tp_value) != NULL ) {
            data->tp_value = str->next;
            SetToggleFlag( str->data, 1, false );
            CMemFree( str );
        }
    }
    if( data->zi ) {
        CompFlags.extra_stats_wanted = true;
    }
#endif
    SetCharacterEncoding( data );
    CBanner();
}

bool MergeIncludeFromEnv( const char *env )
/*****************************************/
{
    const char  *env_value;

    if( CompFlags.cpp_ignore_env )
        return( false );
    env_value = FEGetEnv( env );
    if( env_value != NULL ) {
        AddIncList( env_value );
        return( true );
    }
    return( false );
}

static char *ReadIndirectFile( const char *fname )
{
    char        *env;
    char        *str;
    FILE        *fp;
    size_t      len;
    char        ch;

    env = NULL;
    fp = fopen( fname, "rb" );
    if( fp != NULL ) {
        fseek( fp, 0, SEEK_END );
        len = (size_t)ftell( fp );
        env = CMemAlloc( len + 1 );
        rewind( fp );
        len = fread( env, 1, len, fp );
        env[len] = '\0';
        fclose( fp );
        /*
         * zip through characters changing \r, \n etc into ' '
         */
        for( str = env; (ch = *str) != '\0'; str++ ) {
            if( ch == '\r'
              || ch == '\n' ) {
                *str = ' ';
            }
            /*
             * if DOS end of file (^Z) -> mark end of str
             */
            if( ch == DOS_EOF_CHAR ) {
                *str = '\0';
                break;
            }
        }
    }
    return( env );
}

#include "cmdlnprs.gc"

static void ProcOptions( OPT_STORAGE *data, const char *str )
{
#define MAX_NESTING 32
    int         level;
    const char  *save[MAX_NESTING];
    char        *buffers[MAX_NESTING];
    const char  *penv;
    char        *ptr;
    int         ch;                      // - next character
    OPT_STRING  *fname;

    if( str != NULL ) {
        level = -1;
        CmdScanLineInit( str );
        for( ;; ) {
            CmdScanSkipWhiteSpace();
            ch = CmdScanChar();
            if( ch == '@' ) {
                switch_start = CmdScanAddr() - 1;
                CmdScanSkipWhiteSpace();
                fname = NULL;
                if( OPT_GET_FILE( &fname ) ) {
                    penv = NULL;
                    level++;
                    if( level < MAX_NESTING ) {
                        ptr = NULL;
                        penv = FEGetEnv( fname->data );
                        if( penv == NULL ) {
                            ptr = ReadIndirectFile( fname->data );
                            penv = ptr;
                        }
                        if( penv != NULL ) {
                            save[level] = CmdScanLineInit( penv );
                            buffers[level] = ptr;
                        }
                    }
                    if( penv == NULL ) {
                        level--;
                    }
                    OPT_CLEAN_STRING( &fname );
                }
                continue;
            }
            if( ch == '\0' ) {
                if( level < 0 )
                    break;
                CMemFree( buffers[level] );
                CmdScanLineInit( save[level] );
                level--;
                continue;
            }
            if( CmdScanSwitchChar( ch ) ) {
                switch_start = CmdScanAddr() - 1;
                OPT_PROCESS( data );
            } else {  /* collect  file name */
                CmdScanUngetChar();
                switch_start = CmdScanAddr();
                fname = NULL;
                if( OPT_GET_FILE( &fname ) ) {
                    if( WholeFName != NULL ) {
                        /*
                         * more than one file to compile ?
                         */
                        CBanner();
                        CErr1( ERR_CAN_ONLY_COMPILE_ONE_FILE );
                    }
                    SetStringOption( &WholeFName, &fname );
                    OPT_CLEAN_STRING( &fname );
                }
            }
        }
    }
#undef MAX_NESTING
}

void GenCOptions( char **cmdline )
{
    OPT_STORAGE data;

    /*
     * Add precision warning but disabled by default
     */
    WarnEnableDisable( false, ERR_LOSE_PRECISION );
    /*
     * Warning about non-prototype declarations is disabled by default
     * because Windows and OS/2 API headers use it
     */
    WarnEnableDisable( false, ERR_OBSOLETE_FUNC_DECL );
    /*
     * Warning about pointer truncation during cast is disabled by
     * default because it would cause too many build breaks right now
     * by correctly diagnosing broken code.
     */
    WarnEnableDisable( false, ERR_CAST_POINTER_TRUNCATION );
    InitModInfo();
    CmdSysInit();
    OPT_INIT( &data );
#if _CPU == 8086
    ProcOptions( &data, FEGetEnv( "WCC" ) );
#elif _CPU == 386
    ProcOptions( &data, FEGetEnv( "WCC386" ) );
#elif _CPU == _AXP
    ProcOptions( &data, FEGetEnv( "WCCAXP" ) );
#elif _CPU == _PPC
    ProcOptions( &data, FEGetEnv( "WCCPPC" ) );
#elif _CPU == _MIPS
    ProcOptions( &data, FEGetEnv( "WCCMPS" ) );
#elif _CPU == _SPARC
    ProcOptions( &data, FEGetEnv( "WCCSPC" ) );
#else
    #error Compiler environment variable not configured
#endif
    for( ;*cmdline != NULL; ++cmdline ) {
        ProcOptions( &data, *cmdline );
    }
    if( CompFlags.cpp_mode ) {
        CompFlags.cpp_output = true;
        CompFlags.quiet_mode = true;
    }
    /*
     * print banner if -zq not specified
     */
    AnalyseAnyTargetOptions( &data );
    CmdSysAnalyse( &data );
    OPT_FINI( &data );
    MiscMacroDefs();
}
